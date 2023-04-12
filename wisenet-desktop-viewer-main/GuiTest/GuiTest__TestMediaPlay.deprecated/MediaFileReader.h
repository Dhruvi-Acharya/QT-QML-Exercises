/*********************************************************************************
 * Copyright(c) 2021 by Hanwha Techwin Co., Ltd.
 *
 * This software is copyrighted by, and is the sole property of Hanwha Techwin.
 * All rigths, title, ownership, or other interests in the software remain the
 * property of Hanwha Techwin. This software may only be used in accordance with
 * the corresponding license agreement. Any unauthorized use, duplication,
 * transmission, distribution, or disclosure of this software is expressly
 * forbidden.
 *
 * This Copyright notice may not be removed or modified without prior written
 * consent of Hanwha Techwin.
 *
 * Hanwha Techwin reserves the right to modify this software without notice.
 *
 * Hanwha Techwin Co., Ltd.
 * KOREA
 * https://www.hanwha-security.com/
 *********************************************************************************/
#pragma once

#include <QObject>
#include <QString>
#include "VideoDecoder.h"
#include "ThreadSafeQueue.h"

extern "C"
{
#include "libavutil/imgutils.h"
#include "libavutil/samplefmt.h"
#include "libavformat/avformat.h"
#include "libavutil/frame.h"
}

class MediaFileReader : public QObject
{
    Q_OBJECT
    AVFormatContext*    m_fmtContext;
    AVStream*           m_videoStream;
    AVPacket            m_packet;
    ThreadSafeQueue<RawVideoDataPtr>& m_decodeQueue;

public:
    MediaFileReader(ThreadSafeQueue<RawVideoDataPtr>& decodeQueue);
    ~MediaFileReader();

public slots:
    void open(const QString& filePath);
    void read();
    void close();

signals:
    void openResultReady(const int ret, const double fps);
    void openCodecContextReady(FFmpegCodecParamWrapperPtr contextPtr);

    void readResultReady(const int ret);
    void readVideoResultReady(const int ret);
    void eofResultReady(const int ret);

    void closeResultReady(const int ret);
    void videoInfoChanged(const QString message);

private:
    void closeInternal();
};

