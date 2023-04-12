/*********************************************************************************
 * Copyright(c) 2023 Hanwha Vision Co., Ltd. All rights reserved.
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

#include "ExportVideoModel.h"
#include "LogSettings.h"
#include "MediaWriter/FileWriteManager.h"
#include "ExportVideoProgressModel.h"
#include "StorageInfo.h"
#include <QDateTime>

ExportVideoModel::ExportVideoModel()
{

}

ExportVideoModel::~ExportVideoModel()
{

}

void ExportVideoModel::setMultiChannelPlayerList(QVariantList playerList)
{
    m_multiChannelList.clear();

    foreach(QVariant v, playerList)
    {
        if (v.canConvert<WisenetMediaPlayer*>())
        {
            WisenetMediaPlayer* mediaPlayer = v.value<WisenetMediaPlayer*>();
            WisenetMediaParam* mediaParam = mediaPlayer->mediaParam();
            if(mediaParam->sourceType() != WisenetMediaParam::SourceType::Camera)
                continue;

            ExportChannelItem channelItem;
            channelItem.deviceId = mediaParam->deviceId();
            channelItem.channeId = mediaParam->channelId();
            channelItem.trackId = mediaParam->trackId();
            channelItem.player = mediaPlayer;

            Wisenet::Device::Device::Channel channel;
#ifndef MEDIA_FILE_ONLY
            bool ret = QCoreServiceManager::Instance().DB()->FindChannel(channelItem.deviceId.toStdString(), channelItem.channeId.toStdString(), channel);
            if(!ret)
                continue;
#endif
            channelItem.channelName = QString::fromUtf8(channel.name.c_str());
            if(m_fileName == "")
            {
                m_fileName = channelItem.channelName;
                emit fileNameChanged();
            }

            m_multiChannelList.push_back(channelItem);
        }
    }

    // 이름순 정렬 추가
    std::sort(m_multiChannelList.begin(), m_multiChannelList.end(), [](const ExportChannelItem ch1, const ExportChannelItem ch2) {
        return ch1.channelName < ch2.channelName;
    });
}

void ExportVideoModel::setMultiChannelList(QVariantList channelList)
{
    m_multiChannelList.clear();

    foreach(QVariant v, channelList)
    {
        if (v.canConvert<QString>())
        {
            // channelUuid 포멧 : deviceId_channelId_trackId
            QString channelUuid = v.value<QString>();
            QStringList token = channelUuid.split("_");
            if(token.size() == 3)
            {
                ExportChannelItem channelItem;
                channelItem.deviceId = token[0];
                channelItem.channeId = token[1];
                channelItem.trackId = token[2].toInt();

                Wisenet::Device::Device::Channel channel;
#ifndef MEDIA_FILE_ONLY
                bool ret = QCoreServiceManager::Instance().DB()->FindChannel(channelItem.deviceId.toStdString(), channelItem.channeId.toStdString(), channel);
                if(!ret)
                    continue;
#endif
                channelItem.channelName = QString::fromUtf8(channel.name.c_str());
                if(m_fileName == "")
                {
                    m_fileName = channelItem.channelName;
                    emit fileNameChanged();
                }

                m_multiChannelList.push_back(channelItem);
            }
        }
    }

    // 이름순 정렬 추가
    std::sort(m_multiChannelList.begin(), m_multiChannelList.end(), [](const ExportChannelItem ch1, const ExportChannelItem ch2) {
        return ch1.channelName < ch2.channelName;
    });
}

QString ExportVideoModel::getChannelName(int index)
{
    if(m_multiChannelList.size() <= index)
        return "";
    else
        return m_multiChannelList[index].channelName;
}

void ExportVideoModel::startSingleChannelExport(WisenetMediaPlayer* mediaPlayer, QList<int> overlappedIdList)
{
    WisenetMediaParam* mediaParam = mediaPlayer->mediaParam();
    if(mediaParam->sourceType() != WisenetMediaParam::SourceType::Camera)
        return;

    SPDLOG_DEBUG("ExportVideoModel::startSingleChannelExport() overlappedIdCount:{}", overlappedIdList.count());
    if(overlappedIdList.count() == 0) {
        overlappedIdList.push_back(-1);  // overlappedId가 하나도 없으면 default 값 입력
    }

    ExportChannelItem channelItem;
    channelItem.deviceId = mediaParam->deviceId();
    channelItem.channeId = mediaParam->channelId();
    channelItem.trackId = mediaParam->trackId();
    channelItem.player = mediaPlayer;

    startExport(channelItem, false, overlappedIdList);
}

void ExportVideoModel::startMultichannelExport(QVariantList multichannelCheckList)
{
    if(m_multiChannelList.size() != multichannelCheckList.size())
        return;

    // m_folderName에 시간 추가 후 폴더 생성 -> m_filPath 수정해주기
    QString time_format = "yyyyMMdd_HHmmss";
    m_folderName = m_folderName.append("_");
    m_folderName = m_folderName.append(QDateTime::currentDateTime().toString(time_format));


    QString newDir = m_filePath + "/" + m_folderName;
    //qDebug() <<  "ExportVideoModel :: newDir " << newDir;

    // 폴더 유무 체크 및 생성
    if(!QDir(newDir).exists()) {
        if(!QDir().mkpath(newDir)) {
            SPDLOG_ERROR("ExportVideoModel::startMultiChannelExport() Failed to create a directory. path={}", newDir.toLocal8Bit().toStdString());
            return;
        }
        m_filePath = newDir;
    }

    int checkedItemCount = 0;
    for(auto& checked : multichannelCheckList) {
        if(checked.value<bool>())
            checkedItemCount++;

        if(checkedItemCount >= 2)
            break;
    }

    // Device 별로 체크 된 채널 리스트 구성
    QMap<Wisenet::uuid_string, QList<ExportChannelItem>> deviceMap;
    for(int i=0 ; i<m_multiChannelList.size() ; i++) {
        if (multichannelCheckList[i].value<bool>() == false)
            continue;

        Wisenet::uuid_string deviceId = m_multiChannelList[i].deviceId.toStdString();
        deviceMap[deviceId].push_back(m_multiChannelList[i]);
    }

    // Device 별로 선택 구간의 OverlappedId List 획득
    for(auto itor = deviceMap.begin() ; itor != deviceMap.end() ; itor++) {
        auto request = std::make_shared<Wisenet::Device::DeviceGetOverlappedIdRequest>();
        request->deviceID = itor.key();
        for(auto& channelItem : itor.value()) {
            request->param.ChannelIDList.push_back(channelItem.channeId.toStdString());
        }
        request->param.FromDate = m_fromDateTime.toMSecsSinceEpoch();
        request->param.ToDate = m_toDateTime.toMSecsSinceEpoch();

        QList<ExportChannelItem> exportChannelItemList = itor.value();
#ifndef MEDIA_FILE_ONLY
        QCoreServiceManager::Instance().RequestToCoreService(
                    &Wisenet::Core::ICoreService::DeviceGetOverlappedId, this, request,
                    [this, checkedItemCount, exportChannelItemList](const QCoreServiceReplyPtr& reply)
        {
            // 응답받은 overlappedIdList로 영상 내보내기 요청
            QList<int> overlappedIdList;
            auto response = std::static_pointer_cast<Wisenet::Device::DeviceGetOverlappedIdResponse>(reply->responseDataPtr);
            if(response->isSuccess()) {
                for(auto& overlappedId : response->result.overlappedIDList) {
                    overlappedIdList.push_back(overlappedId);
                }
            }
            else {
                overlappedIdList.push_back(-1);
            }

            for(int i=0 ; i<exportChannelItemList.size() ; i++) {
                startExport(exportChannelItemList[i], checkedItemCount >= 2, overlappedIdList);
            }
        });
#endif
    }
}

void ExportVideoModel::startExport(ExportChannelItem channelItem, bool isMultiChannelExport, QList<int> overlappedIdList)
{
    if(channelItem.player != nullptr && channelItem.player->mediaParam() != nullptr)
    {
        WisenetMediaParam* mediaParam = channelItem.player->mediaParam();
        Wisenet::Device::Device device;
#ifndef MEDIA_FILE_ONLY
        if(QCoreServiceManager::Instance().DB()->FindDevice(mediaParam->deviceId().toStdString(), device)
                && device.deviceType == Wisenet::Device::DeviceType::SunapiCamera
                && WisenetMediaParam::isPlayback(mediaParam->streamType()))
        {
            channelItem.player->changeCameraStream(WisenetMediaParam::StreamType::LiveAuto);
        }
#endif
    }

    // 중복구간 수 만큼 개별로 영상 내보내기 수행
    for(int i=0 ; i<overlappedIdList.count() ; i++) {
        ExportMediaRequest request;
        request.deviceId = channelItem.deviceId;
        request.channeId = channelItem.channeId;
        request.trackId = overlappedIdList[i]; //channelItem.trackId;
        request.trackIndex = overlappedIdList.count() == 1 ? -1 : i;
        request.startTime = m_fromDateTime.toMSecsSinceEpoch();
        request.endTime = m_toDateTime.toMSecsSinceEpoch();
        request.filePath = m_filePath;
        request.fileName = getAvailableFileName(m_fileName);
        request.streamType = m_enableHighResolution ? WisenetMediaParam::StreamType::BackupHigh : WisenetMediaParam::StreamType::BackupLow;
        request.usePassword = m_usePassword;
        request.password = m_password;
        request.useDigitalSignature = m_enableDigitalSignature;

        if(isMultiChannelExport)
        {
            int extensionIndex = m_fileName.lastIndexOf(".");
            QString extension = m_fileName.right(m_fileName.length() - extensionIndex - 1);
            request.fileName = getAvailableFileName(channelItem.channelName) + "." + extension;
        }

        FileWriteManager::getInstance()->requestVideoExport(request);
    }
}

qint64 ExportVideoModel::getMbytesAvailable()
{
    return GetMbytesAvailable(m_filePath);
}

QString ExportVideoModel::getAvailableFileName(QString fileName)
{
    while(fileName.length() > 0 && fileName.at(0) == '.') {
        fileName.replace(0, 1, ""); // 첫 문자가 . 이 되지 않도록.
    }
    fileName.replace("\\", ""); // \ 문자 제거
    fileName = fileName.remove(QRegExp("[/:*?<>|\"]")); // 나머지 특수문자 제거

    return fileName;
}
