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
#include "FFmpegStruct.h"
#include "FFmpegStreamInfo.h"
#include "FFmpegWrapper.h"
#include <cstring>

class FFmpegFileOutContext : public QObject
{
    Q_OBJECT
public:
    FFmpegFileOutContext()
        : m_isReady(false)
        , m_avSubripDecoderContext(nullptr)
        , m_avAudioDecoderContext(nullptr)
        , m_avAudioEncoderContext(nullptr)
        , m_avAudioResampleContext(nullptr)
        , m_subTitleOutBuffer(nullptr)
        , m_fileNo(1)
        , m_startTime(0)
        , m_prevSeconds(0)
    {
        std::memset(&m_subTitle, 0, sizeof(AVSubtitle));
    }
    ~FFmpegFileOutContext();

    void SetOutFileInfo(QString fileDir, QString fileName);
    bool DoWrite(Wisenet::Media::MediaSourceFrameBaseSharedPtr mediaData);
    void CloseFile();

private:
    bool QueueSetup(void);

    bool SetupFirst();
    bool GenericSetup(Wisenet::Media::MediaSourceFrameBaseSharedPtr mediaData, bool &isVideoDecodingFail);
    bool GenericVideoSetup(Wisenet::Media::MediaSourceFrameBaseSharedPtr mediaData, bool &isVideoDecodingFail);
    bool GenericAudioSetup(Wisenet::Media::MediaSourceFrameBaseSharedPtr mediaData);
    bool TimedTextSetup();

    bool IsCodecChagned(Wisenet::Media::MediaSourceFrameBaseSharedPtr mediaData);
    bool OpenFile();

    bool WriteFile(Wisenet::Media::MediaSourceFrameBaseSharedPtr mediaData);
    bool WriteVideo(Wisenet::Media::MediaSourceFrameBaseSharedPtr mediaData,
                    AVStream *outStream,
                    FFmpegStreamInfo *streamInfo,
                    AVPacket *pkt,
                    AVRational msecTimebase);
    bool WriteAudio(Wisenet::Media::MediaSourceFrameBaseSharedPtr mediaData,
                    AVStream *outStream,
                    FFmpegStreamInfo *streamInfo,
                    AVPacket *pkt,
                    AVRational msecTimebase);
    bool WriteSubtitle(Wisenet::Media::MediaSourceFrameBaseSharedPtr mediaData);

    bool AllocContext();
    void ReleaseContext();
    void ClearQueue();
    void Finalize();

signals:
    void writeFailed(const QString &message);
    void newFileCreated(const QString &filePath);

private:
    bool m_isReady = false;

    bool m_isTimedText = true;
    QString m_fileDir = "";
    QString m_fileName = "";

    unsigned int m_streamID = 0;
    unsigned int m_fileNo = 1;
    std::string m_currentOutFilePath = "";

    AVFormatContext* m_avOutFormatContext = nullptr;    

    FFmpegStreamInfo m_avOutVideoStream;
    FFmpegStreamInfo m_avOutAudioStream;
    FFmpegStreamInfo m_avOutSubtitleStream;

    AVCodecContextPtr m_avAudioDecoderContext = nullptr;
    AVCodecContextPtr m_avAudioEncoderContext = nullptr;
    AVCodecContextPtr m_avSubripDecoderContext = nullptr;

    std::vector<Wisenet::Media::MediaSourceFrameBaseSharedPtr> m_readyQueue;

    uint8_t* m_subTitleOutBuffer;
    static const int kKaxSubTitleOutSize = 10240;
    AVSubtitle m_subTitle;
    int64_t m_prevSeconds;
    int64_t m_startTime = 0;
    int64_t m_endTime = 0;

    SwrContext *m_avAudioResampleContext;

    std::mutex m_mutex;
    bool m_videoFrameChecked = false;
    bool m_audioFrameChecked = false;
    std::chrono::steady_clock::time_point m_lastVideoFrameCheckedClock;

private:
    static const int kBufferWaitTimeMsec = 1000; // 1seconds
    static const unsigned int kMaxFileSizeByte = 2147483648;    // 2GB
};
