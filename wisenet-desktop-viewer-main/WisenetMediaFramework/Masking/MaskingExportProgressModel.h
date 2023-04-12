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
#include "FileWriterBase.h"

class MaskingExportProgressModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int progress READ progress WRITE setProgress NOTIFY progressChanged)
    Q_PROPERTY(bool failed READ failed WRITE setFailed NOTIFY failedChanged)
    Q_PROPERTY(bool canceled READ canceled WRITE setCanceled NOTIFY canceledChanged)
    Q_PROPERTY(QString filePath READ filePath WRITE setFilePath NOTIFY filePathChanged)

public:
    explicit MaskingExportProgressModel();
    ~MaskingExportProgressModel();
    const int progress(){return m_progress;}
    const bool failed(){return m_failed;}
    const bool canceled(){return m_canceled;}
    const QString filePath(){return m_filePath;}
    void setProgress(const int progress);
    void setFailed(const bool failed);
    void setCanceled(const bool canceled);
    void setFilePath(const QString filePath);
    Q_INVOKABLE void openFilePath();

public slots:
    void onProgressChanged(int progress);
    void onWriteFinished(FileWriterBase::ErrorCode errorCode);

signals:
    void progressChanged();
    void failedChanged();
    void canceledChanged();
    void filePathChanged();

private:
    int m_progress = 0;
    bool m_failed = false;
    bool m_canceled = false;
    QString m_filePath = "";
};
