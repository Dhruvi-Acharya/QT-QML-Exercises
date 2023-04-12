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
#include "WnmReader.h"
#include "FileWriterBase.h"
#include "VideoDecoder.h"
#include "VideoEncoder.h"
#include "Masking/MaskItem.h"
#include "CvMaskPainter.h"

class MaskingTranscoder : public QObject, public MediaRunnable
{
    Q_OBJECT

public:
    explicit MaskingTranscoder(QObject *parent = nullptr);
    ~MaskingTranscoder();

protected:
    void loopTask() override;

public slots:
    void onReaderOpenResult(MediaControlBase::Error error);
    void onWriteFinished(FileWriterBase::ErrorCode errorCode);

public:
    // public functions
    void setMaskItemList(const QList<MaskItem>& maskItemList);
    void setWnmInitalizeParameter(const WnmInitalizeParameter initParam);
    void setMaskPainter(const CvMaskPainter painter);
    void open(qint64 startTime, qint64 endTime, QUrl inputFileUrl, QString outputFilePath, QString outputFileName, QString password);
    void close();

signals:
    void progressChanged(int progress);
    void writeFinished(FileWriterBase::ErrorCode errorCode);

private:
    // private functions
    void checkCommands();
    void _open();
    void _close();
    bool initializeFileWriter(QString filePath, QString fileName);
    bool decodeAndMaskFrame(Wisenet::Media::VideoSourceFrameSharedPtr& videoFramePtr);

private:
    // private variables
    qint64 m_startTime = 0;
    qint64 m_endTime = 0;
    QString m_inputFileNativePath = "";
    QString m_outputFilePath = "";
    QString m_outputFileName = "";
    QString m_password = "";
    bool m_readerOpened = false;
    int m_readerEmptyCount = 0;
    int m_commandSeq = 0;
    WnmInitalizeParameter m_wnmInitParam;
    QScopedPointer<WnmReader> m_reader;
    SourceFrameQueueSharedPtr m_mediaSourceFramesQueue = nullptr;   // shared queue between reader and transcoder
    QScopedPointer<FileWriterBase> m_writer;
    QScopedPointer<VideoDecoder> m_videoDecoder;
    QScopedPointer<VideoEncoder> m_videoEncoder;
    VideoOutputFrameSharedPtr m_VideoOutBuffer = nullptr;   // decoder output frame
    Wisenet::Media::VideoSourceFrameSharedPtr m_encodedFrame = nullptr; // encoder output frame
    QList<MaskItem> m_maskItemList;
    CvMaskPainter m_maskPainter;
    int m_progress = 0;

    std::mutex m_mutex;
    std::atomic<bool> m_isOpenCommand;
    std::atomic<bool> m_isCloseCommand;
};

