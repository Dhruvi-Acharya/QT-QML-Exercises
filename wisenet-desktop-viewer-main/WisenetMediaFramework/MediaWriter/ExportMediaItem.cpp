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

#include "ExportMediaItem.h"
#include "StorageInfo.h"

ExportMediaItem::ExportMediaItem()
{

}

ExportMediaItem::~ExportMediaItem()
{
    stopExport();
}

void ExportMediaItem::startExport()
{
    // 내보내기 시작 전 용량 체크
    qint64 bytes = GetMbytesAvailable(m_request.filePath);
    if(bytes == -1 || bytes < 50)
    {
        emit endExport(this, true);
        return;
    }

    m_mediaControl = new ExportMediaControl(this);
    connect(m_mediaControl, &MediaControlBase::openResult, this, &ExportMediaItem::openResult, Qt::QueuedConnection);
    connect(m_mediaControl, &ExportMediaControl::progressChanged, this, &ExportMediaItem::onProgressChanged, Qt::QueuedConnection);
    connect(m_mediaControl, &ExportMediaControl::writeFinished, this, &ExportMediaItem::onWriteFinished, Qt::QueuedConnection);

    m_mediaControl->setExportParam(m_request.streamType,
                                   m_request.deviceId,
                                   m_request.channeId,
                                   m_request.trackId,
                                   m_request.startTime,
                                   m_request.endTime);
    m_mediaControl->initializeFileWriter(m_request.filePath, m_request.fileName, m_request.usePassword, m_request.password, m_request.useDigitalSignature);
    m_mediaControl->open();
}

void ExportMediaItem::stopExport()
{
    if (m_mediaControl) {
        m_mediaControl->close();
        delete m_mediaControl;
        m_mediaControl = nullptr;
    }
}

void ExportMediaItem::openResult(MediaControlBase::Error error)
{
    // Backup Media Open 실패 한 경우 정리
    if(error != MediaControlBase::Error::NoError)
    {
        SPDLOG_DEBUG("ExportMediaItem::openResult(), deviceId={} channelId={} error={}" ,
                     m_request.deviceId.toStdString(), m_request.channeId.toStdString(), error);

        stopExport();

        emit endExport(this, true);
    }
}

void ExportMediaItem::onProgressChanged(int progress)
{
    m_progress = progress;
    emit progressChanged(progress);
}

void ExportMediaItem::onWriteFinished(FileWriterBase::ErrorCode errorCode)
{
    stopExport();

    if(errorCode != FileWriterBase::ErrorCode::NoError)
    {
        SPDLOG_DEBUG("ExportMediaItem::onWriteFinished(), deviceId={} channelId={} error={}" ,
                     m_request.deviceId.toStdString(), m_request.channeId.toStdString(), errorCode);
        emit endExport(this, true);
    }
    else
    {
        emit endExport(this, false);
    }
}
