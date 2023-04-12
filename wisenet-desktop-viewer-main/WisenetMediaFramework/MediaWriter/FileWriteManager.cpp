/*********************************************************************************
 * Copyright(c) 2023 Hanwha Vision Co., Ltd.  All rights reserved.
 *
 * This software is copyrighted by, and is the sole property of Hanwha Vision.
 * All rigths, title, ownership, or other interests in the software remain the
 * property of Hanwha Vision. This software may only be used in accordance with
 * the corresponding license agreement. Any unauthorized use, duplication,
 * transmission, distribution, or disclosure of this software is expressly
 * forbidden.
 *
 * This Copyright notice may not be removed or modified without prior written
 * consent of Hanwha Vision.
 *
 * Hanwha Vision reserves the right to modify this software without notice.
 *
 * Hanwha Vision Co., Ltd.
 * KOREA
 * https://www.hanwhavision.com/
 *********************************************************************************/

#include "FileWriteManager.h"

FileWriteManager::FileWriteManager(QObject *parent)
    : QObject(parent)
    , m_localRecordingCount(0)
    , m_contextPool(std::make_shared<ThreadPool>(4))
    , m_strand(m_contextPool->ioContext().get_executor())
{
    m_contextPool->Start();
}

FileWriteManager::~FileWriteManager()
{
    m_contextPool->Stop();
}

void FileWriteManager::requestVideoExport(ExportMediaRequest request)
{
    QMutexLocker lock(&m_mutex);

    ExportMediaItem* exportItem = new ExportMediaItem;
    connect(exportItem, &ExportMediaItem::endExport, this, &FileWriteManager::onEndExport, Qt::QueuedConnection);
    exportItem->setExportMediaRequest(request);
    emit exportMediaItemAdded(exportItem);

    auto deviceRequest = m_exportDeviceMap.find(request.deviceId.toStdString());

    // 같은 device가 백업 중이거나 최대 동시 백업 장비 수 초과 시, 대기열에 추가 후 리턴
    if(deviceRequest != m_exportDeviceMap.end() || m_exportDeviceMap.size() >= MaxExportDeviceCount) {
        m_exportWaitList.push_back(exportItem);
        return;
    }

    m_exportDeviceMap[request.deviceId.toStdString()] = exportItem; // RequestMap에 추가
    exportItem->startExport();  // 신규 Backup Media Open
}

void FileWriteManager::stopVideoExport(void* stopItem, bool startNext)
{
    QMutexLocker lock(&m_mutex);

    for(auto& deviceExportItem : m_exportDeviceMap)
    {
        // 중지 할 Item이 현재 백업 중인 경우
        if(deviceExportItem == stopItem)
        {
            // 백업 중지
            ExportMediaRequest request = deviceExportItem->getExportMediaRequest();
            deviceExportItem->stopExport();
            deviceExportItem->deleteLater();
            deviceExportItem = nullptr;
            m_exportDeviceMap.remove(request.deviceId.toStdString());

            copyWisenetPlayer(request.filePath);

            // 다음 장비 백업 시작
            if(startNext)
                scanExportWaitList();

            break;
        }
    }

    // 중지 할 Item이 대기 중인 경우 대기열에서 삭제
    for(int i=0 ; i<m_exportWaitList.size() ; i++)
    {
        if(m_exportWaitList[i] == stopItem)
        {
            m_exportWaitList.removeAt(i);
            break;
        }
    }

    // 전 채널 백업 완료 체크 및 signal 전송
    if(m_exportDeviceMap.size() == 0 && m_exportWaitList.size() == 0)
        emit endExportAll();
}

void FileWriteManager::onEndExport(ExportMediaItem* item, bool isFailed)
{
    QMutexLocker lock(&m_mutex);

    ExportMediaRequest request = item->getExportMediaRequest();
    item->deleteLater();
    item = nullptr;
    m_exportDeviceMap.remove(request.deviceId.toStdString());

    copyWisenetPlayer(request.filePath);
    scanExportWaitList();

    // 전 채널 백업 완료 체크 및 signal 전송
    if(m_exportDeviceMap.size() == 0 && m_exportWaitList.size() == 0)
        emit endExportAll();

    if (!isFailed) {
        emit endExport(request.filePath);
    }
}

void FileWriteManager::copyWisenetPlayer(QString filePath)
{
    boost::asio::post(m_strand, [this, filePath]() {
        bool isSamePathRemain = false;

        // 내보내기 중인 장비 Map에서 경로가 같은 item 탐색
        for(auto& exportItem : m_exportDeviceMap.values())
        {
            if(filePath == exportItem->getExportMediaRequest().filePath)
            {
                isSamePathRemain = true;
                break;
            }
        }

        // 내보내기 대기 목록에서 경로가 같은 item 탐색
        if(!isSamePathRemain)
        {
            for(auto& exportItem : m_exportWaitList)
            {
                if(filePath == exportItem->getExportMediaRequest().filePath)
                {
                    isSamePathRemain = true;
                    break;
                }
            }
        }

        // 경로가 같은 item이 남아있지 않으면 WisenetPlayer 파일을 복사
        if(!isSamePathRemain)
        {
            SPDLOG_DEBUG("FileWriteManager::copyWisenetPlayer() copy started.");

            QString fileName = "/WisenetPlayer.exe";
#ifdef Q_OS_MAC
            fileName = "/WisenetPlayer.zip";
#endif
#ifdef Q_OS_LINUX
            QDirIterator subDirFileIt(QCoreApplication::applicationDirPath(), {"*.deb"}, QDir::Files);
            while (subDirFileIt.hasNext()) {
                subDirFileIt.next();
                QString debFileName = subDirFileIt.fileName();
                if(debFileName.contains("WisenetPlayer")) {
                    fileName = "/" + subDirFileIt.fileName();
                }
            }
#endif

            QString newFileName = filePath + fileName;

            if (QFile::exists(newFileName) && !QFile::remove(newFileName)) {
                SPDLOG_ERROR("FileWriteManager::copyWisenetPlayer() old file remove failed.");
                return;
            }

#ifdef Q_OS_WINDOWS
            QFile fromFile;
            QDataStream fromDataStream;
            fromFile.setFileName(QCoreApplication::applicationDirPath() + fileName);
            if(fromFile.open(QIODevice::ReadOnly) == false)
            {
                SPDLOG_ERROR("FileWriteManager::copyWisenetPlayer() From file open error. {}", fromFile.fileName().toLocal8Bit().toStdString());

                return;
            }
            fromDataStream.setDevice(&fromFile);

            QFile toFile;
            QDataStream toDataStream;
            toFile.setFileName(newFileName);
            if(toFile.open(QIODevice::WriteOnly) == false)
            {
                SPDLOG_ERROR("FileWriteManager::copyWisenetPlayer() To file open error. {}", newFileName.toLocal8Bit().toStdString());
                fromFile.close();
                return;
            }
            toDataStream.setDevice(&toFile);

            int blockLength = 102400;
            char * data = new char[blockLength];
            while ((blockLength = fromDataStream.readRawData(data, blockLength)) > 0) {
                if(toDataStream.writeRawData(data, blockLength) < 0) {
                    SPDLOG_ERROR("FileWriteManager::copyWisenetPlayer() To file Write error");
                    break;
                }
                QThread::msleep(1);
            }
            delete [] data;

            fromFile.close();
            toFile.close();
#else
            if(QFile::copy(QCoreApplication::applicationDirPath() + fileName, newFileName))
                SPDLOG_DEBUG("FileWriteManager::copyWisenetPlayer() copy successed.");
            else
                SPDLOG_ERROR("FileWriteManager::copyWisenetPlayer() copy failed.");
#endif
            SPDLOG_DEBUG("FileWriteManager::copyWisenetPlayer() copy finished. from:{} to:{}",
                         (QCoreApplication::applicationDirPath() + fileName).toLocal8Bit().toStdString(), newFileName.toLocal8Bit().toStdString());
        }
        else
        {
            SPDLOG_DEBUG("FileWriteManager::copyWisenetPlayer() Same Path Remain");
        }
    });
}

void FileWriteManager::scanExportWaitList()
{
    // 대기열 탐색하여 백업 시작
    for(int i=0 ; i<m_exportWaitList.size() ; )
    {
        if(m_exportDeviceMap.size() >= MaxExportDeviceCount)
            break;

        ExportMediaRequest waitRequest = m_exportWaitList[i]->getExportMediaRequest();
        if(m_exportDeviceMap.find(waitRequest.deviceId.toStdString()) == m_exportDeviceMap.end())
        {
            m_exportWaitList[i]->startExport();
            m_exportDeviceMap[waitRequest.deviceId.toStdString()] = m_exportWaitList[i];
            m_exportWaitList.removeAt(i);
        }
        else
        {
            i++;
        }
    }
}

QList<ExportMediaItem*> FileWriteManager::getAllExportMediaList()
{
    QMutexLocker lock(&m_mutex);

    QList<ExportMediaItem*> exportMediaList;

    // 현재 백업 중인 Item들
    for(auto& item : m_exportDeviceMap)
    {
        exportMediaList.push_back(item);
    }

    // 백업 대기중인 Item들
    exportMediaList.append(m_exportWaitList);

    return exportMediaList;
}

void FileWriteManager::requestUpdateLocalDir()
{
    emit endExport("");
}
