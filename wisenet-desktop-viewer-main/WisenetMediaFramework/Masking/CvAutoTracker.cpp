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
#include "CvAutoTracker.h"
#include "LogSettings.h"
#include "WisenetVideoBuffer.h"

#include <QDateTime>

#ifdef Q_OS_WINDOWS
#ifndef MEDIA_FILE_ONLY
#include <opencv2/core.hpp>
#include <opencv2/core/ocl.hpp>
#endif
#endif


CvAutoTracker::CvAutoTracker(QObject *parent)
    : QObject(parent)
{
    SPDLOG_DEBUG("CvAutoTracker::CvAutoTracker()");
}

CvAutoTracker::~CvAutoTracker()
{
    SPDLOG_DEBUG("CvAutoTracker::~CvAutoTracker()");
    stop();
    disconnect();
}

void CvAutoTracker::InitTracker(int maskId, NormalMaskRect initialRect)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    SPDLOG_DEBUG("CvAutoTracker::InitTracker()");

    m_trackerInfo.trackerId = maskId;
    m_trackerInfo.initialRectPos = initialRect;

    m_isInitTrackerCommand = true;
}

void CvAutoTracker::RestTracker()
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    SPDLOG_DEBUG("CvAutoTracker::RestTracker()");

    m_isRestTrackerCommand = true;
}

void CvAutoTracker::onNewVideoFrame(const QVideoFrame &frame, const qint64 playbackTimeMsec, const bool jpegColorSpace)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isNewVideoFrame = true;
    m_lastVideoFrame.frame = frame;
    m_lastVideoFrame.playbackTimeMsec = playbackTimeMsec;
    m_lastVideoFrame.jpegColorSpace = jpegColorSpace;
}

void CvAutoTracker::StartTracking()
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isStartTrackingCommand = true;
}

void CvAutoTracker::StopTracking()
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isStopTrackingCommand = true;
}

void CvAutoTracker::loopTask()
{
#ifdef Q_OS_WINDOWS
#ifndef MEDIA_FILE_ONLY
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    QPair<int, QRectF> resultPair; // key : maskId
    resultPair.first = -1;

    CommandParam cp;
    checkCommandParam(cp);  // check new command and video

    if (!cp.isNewVideoFrame || !cp.newVideo.frame.isValid()) {
        return;
    }

    // check tracking started
    if(!m_isTracking || m_isRest) {
        // emit signal (received frame and empty mask)
        emit newVideoFrame(cp.newVideo.frame, cp.newVideo.playbackTimeMsec, cp.newVideo.jpegColorSpace, resultPair);
        return;
    }

    // unsupported pixelFormat
    if(cp.newVideo.frame.pixelFormat() != QVideoFrame::PixelFormat::Format_YUV420P || cp.newVideo.jpegColorSpace) {
        // emit signal (received frame and empty mask)
        //SPDLOG_DEBUG("CvAutoTracker::loopTask() pixelFormat {} {}",cp.newVideo.frame.pixelFormat(), cp.newVideo.jpegColorSpace  );
        emit newVideoFrame(cp.newVideo.frame, cp.newVideo.playbackTimeMsec, cp.newVideo.jpegColorSpace, resultPair);
        return;
    }

    QVideoFrame frame = cp.newVideo.frame;
    QAbstractVideoBuffer* buffer = frame.buffer();
    WisenetVideoBuffer* wisenetBuffer = (WisenetVideoBuffer*)buffer;
    int numBytes = 0;
    int bytesPerLine[4] = {0};
    uchar *data[4] = {nullptr};
    wisenetBuffer->map(QAbstractVideoBuffer::MapMode::ReadOnly, &numBytes, bytesPerLine, data);
    //SPDLOG_DEBUG("onNewVideoFrame numBytes:{}, bytesPerLine:{},{},{},{}", numBytes, bytesPerLine[0],bytesPerLine[1],bytesPerLine[2],bytesPerLine[3]);

    if(numBytes > 0 && bytesPerLine[0] != 0 && data[0] != nullptr) {
        int videoWidth = frame.width();
        int videoHeight = frame.height();
        cv::Mat mY(videoHeight, bytesPerLine[0], CV_8UC1, data[0]);
        QRect rect = m_trackerInfo.initialRectPos.toRealSizeRect(videoWidth, videoHeight);
        cv::Rect boundingBox = cv::Rect(rect.x(), rect.y(), rect.width(), rect.height()) ;

        if(m_isNeedInit) {
            SPDLOG_DEBUG("CvAutoTracker::onNewVideoFrame() m_isInitialized is True");
            m_tracker = cv::TrackerCSRT::create();
            m_tracker->init(mY, boundingBox);
            m_isNeedInit = false;
        }

        qint64 tick = QDateTime::currentMSecsSinceEpoch();
        bool isTracked = m_tracker->update(mY, boundingBox);
        SPDLOG_DEBUG("CvAutoTracker::onNewVideoFrame() isTracked : {}", isTracked);
        //SPDLOG_DEBUG("CvAutoTracker::onNewVideoFrame() tracking time:{}", QDateTime::currentMSecsSinceEpoch() - tick);
        if (isTracked) {
            NormalMaskRect resultRect;
            resultRect.normalize(boundingBox.x, boundingBox.y, boundingBox.width, boundingBox.height, videoWidth, videoHeight);
            QRectF qResultRectF(resultRect.normalX, resultRect.normalY, resultRect.normalWidth, resultRect.normalHeight);
            resultPair.first = m_trackerInfo.trackerId;
            resultPair.second = qResultRectF;
        }
    }
    wisenetBuffer->unmap();

    // emit signal (received frame and detected mask)
    emit newVideoFrame(cp.newVideo.frame, cp.newVideo.playbackTimeMsec, cp.newVideo.jpegColorSpace, resultPair);
#endif
#endif
}

void CvAutoTracker::checkCommandParam(CommandParam& commandParam)
{
    const std::lock_guard<std::mutex> lock(m_mutex);

    // check command
    if(m_isStartTrackingCommand) {
        m_isStartTrackingCommand = false;
        m_isTracking = true;
    }
    if(m_isStopTrackingCommand) {
        m_isStopTrackingCommand = false;
        m_isTracking = false;
    }
    if(m_isInitTrackerCommand) {
        m_isInitTrackerCommand = false;
        m_isNeedInit = true;
        m_isRest = false;
    }
    if(m_isRestTrackerCommand) {
        m_isRestTrackerCommand = false;
        m_isRest = true;
    }

    // check new video
    bool needToUpdateFrame = m_lastVideoFrame.frame.isValid() && m_isNewVideoFrame;
    if (!needToUpdateFrame)
        return;

    m_isNewVideoFrame = false;
    commandParam.isNewVideoFrame = true;
    commandParam.newVideo = m_lastVideoFrame;
}
