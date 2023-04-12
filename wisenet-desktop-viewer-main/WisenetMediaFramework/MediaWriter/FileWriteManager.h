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
#include <atomic>
#include "ExportMediaItem.h"

class FileWriteManager : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(FileWriteManager)

public:
    explicit FileWriteManager(QObject *parent = nullptr);
    ~FileWriteManager();

    static FileWriteManager* getInstance(){
        static FileWriteManager instance;
        return &instance;
    }

    // Local Recording
    void increaseLocalRecordingCount(){m_localRecordingCount++;}
    void decreaseLocalRecordingCount(){m_localRecordingCount--;}
    bool isLocalRecordingFull(){return m_localRecordingCount >= MaxLocalRecordingCount;}

    // Video Export
    void requestVideoExport(ExportMediaRequest request);
    void stopVideoExport(void* stopItem, bool startNext);
    QList<ExportMediaItem*> getAllExportMediaList();

    void requestUpdateLocalDir();
    void copyWisenetPlayer(QString filePath);

private slots:
    void onEndExport(ExportMediaItem* item, bool isFailed);

signals:
    void exportMediaItemAdded(ExportMediaItem* item);
    void endExportAll();
    void endExport(QString fileDir);

private:    
    void scanExportWaitList();

private:
    static const int MaxLocalRecordingCount = 16;
    std::atomic<int> m_localRecordingCount;

    static const int MaxExportDeviceCount = 16;
    QBasicMutex m_mutex;
    QMap<Wisenet::uuid_string, ExportMediaItem*> m_exportDeviceMap; // key: deviceID
    QList<ExportMediaItem*> m_exportWaitList;

    // Thread for Wisenet Player copy
    std::shared_ptr<ThreadPool> m_contextPool;
    boost::asio::strand<boost::asio::io_context::executor_type> m_strand;
};
