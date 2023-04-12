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
#pragma once
#include <QObject>
#include "WisenetMediaParam.h"
#include "ExportMediaControl.h"

struct ExportMediaRequest
{
    QString deviceId = "";
    QString channeId = "";
    int trackId = -1;
    int trackIndex = -1;    // single track이면 -1, multi track이면 trackId의 index (0부터 시작)
    qint64 startTime = 0;
    qint64 endTime = 0;
    QString filePath = "";
    QString fileName = "";
    WisenetMediaParam::StreamType streamType = WisenetMediaParam::StreamType::LiveAuto;
    bool usePassword = false;
    QString password = "";
    bool useDigitalSignature = false;
};

class ExportMediaItem : public QObject
{
    Q_OBJECT

public:
    explicit ExportMediaItem();
    ~ExportMediaItem();

    void setExportMediaRequest(ExportMediaRequest request){m_request = request;}
    ExportMediaRequest getExportMediaRequest(){return m_request;}
    void startExport();
    void stopExport();
    const int progress(){return m_progress;}
    const QString filePath(){return m_request.filePath;}

private slots:
    void openResult(MediaControlBase::Error error);
    void onProgressChanged(int progress);
    void onWriteFinished(FileWriterBase::ErrorCode errorCode);

signals:
    void endExport(ExportMediaItem* item, bool isFailed);
    void progressChanged(int progress);

private:
    ExportMediaRequest m_request;
    ExportMediaControl* m_mediaControl = nullptr;
    int m_progress = 0;
};
