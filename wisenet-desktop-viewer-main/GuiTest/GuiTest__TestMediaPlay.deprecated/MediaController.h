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
#include <QThread>
#include <QString>
#include "ThreadSafeQueue.h"
#include "MediaFileReader.h"
#include "VideoDecoder.h"
#include "IVideoRenderer.h"

class MediaController : public QObject
{
    Q_OBJECT
    QThread readThread;
    QThread decodeThread;
    ThreadSafeQueue<RawVideoDataPtr>& m_decodeQueue;
    ThreadSafeQueue<DecodedVideoDataPtr>& m_renderVideoQueue;
    IVideoRenderer *m_renderer = nullptr;

public:
    MediaController(ThreadSafeQueue<RawVideoDataPtr>& decodeVideoQueue,
                    ThreadSafeQueue<DecodedVideoDataPtr>& renderVideoQueue,
                    bool useYuvTexture = false);
    ~MediaController();
    void mediaOpen(const QString& filePath, IVideoRenderer* renderer);
    void setVideoOutSize(const int width, const int height);

public slots:
    void handleOpenResult(const int ret, const double fps);
    void handleReadResult(const int ret);
    void handleEofResult(const int ret);
    void handleVideoDecodeResult(const int ret, const int elapsedMsec);

private slots:
    void handleReadTimeout();
    void handleDecodeTimeout();

signals:
    void mediaPlayStart(const QString& filePath);
    void mediaReadContinue();
    void videoDecode(const int ret);
    void videoInfoChanged(const QString message);
    void setVideoOutResolution(const int width, const int height);

private:
    QTimer *m_readTimer;
    QTimer *m_decodeTimer;
    int     m_decodeSleepMsec = 0;
};

