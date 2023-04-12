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
#include <QVideoFrame>
#include "MediaRunnable.h"
#include "VideoFilterStruct.h"
#include "MaskItem.h"

#ifdef Q_OS_WINDOWS
#ifndef MEDIA_FILE_ONLY
#include <opencv2/tracking.hpp>

using namespace cv;
#endif
#endif

struct TrackerInfo
{
    int trackerId;
    NormalMaskRect initialRectPos;
};

class CvAutoTracker : public QObject, public MediaRunnable
{
    Q_OBJECT
public:
    explicit CvAutoTracker(QObject *parent = nullptr);
    ~CvAutoTracker();

protected:
    void loopTask() override;

signals:
    void newVideoFrame(const QVideoFrame &frame, const qint64 playbackTimeMsec, const bool jpegColorSpace, const QPair<int, QRectF> resultMap);

public:
    // public functions
    void InitTracker(int maskId, NormalMaskRect initialRect);
    void RestTracker();
    void onNewVideoFrame(const QVideoFrame &frame, const qint64 playbackTimeMsec, const bool jpegColorSpace = false);
    void StartTracking();
    void StopTracking();

private:
    // private functions
    void checkCommandParam(CommandParam& commandParam);

private:
    // private variables
    std::mutex      m_mutex;
    QVideoFrameInfo m_lastVideoFrame;
    bool              m_isNewVideoFrame = false;

#ifdef Q_OS_WINDOWS
#ifndef MEDIA_FILE_ONLY
    Ptr<TrackerCSRT> m_tracker;;
#endif
#endif
    TrackerInfo    m_trackerInfo; // will be updated when InitTracker() is called.
    bool            m_isTracking = false;
    bool            m_isNeedInit = false;
    bool            m_isRest = false;

    // command flags
    bool            m_isInitTrackerCommand = false;
    bool            m_isRestTrackerCommand = false;
    bool            m_isStartTrackingCommand = false;
    bool            m_isStopTrackingCommand = false;
};
