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

class ExportProgressItem : public QObject
{
    Q_OBJECT

public:
    explicit ExportProgressItem(const QString name, const int progress, const QString filePath, void* exportMediaItemPtr);
    ~ExportProgressItem();
    const QString name(){return m_name;}
    const int progress(){return m_progress;}    
    void* exportMediaItemPtr(){return m_exportMediaItemPtr;}
    const bool isFailed(){return m_failed;}
    const bool isCanceled(){return m_canceled;}
    const QString filePath(){return m_filePath;}
    void setIsCanceled(const bool canceled){m_canceled = canceled;}

public slots:
    void onProgressChanged(int progress);
    void onEndExport(void* item, bool isFailed);

signals:
    void progressChanged(ExportProgressItem* item);
    void failedChanged(ExportProgressItem* item);

private:
    QString m_name = "";
    int m_progress = 0;    
    void* m_exportMediaItemPtr;
    bool m_failed = false;
    bool m_canceled = false;
    QString m_filePath = "";
};
