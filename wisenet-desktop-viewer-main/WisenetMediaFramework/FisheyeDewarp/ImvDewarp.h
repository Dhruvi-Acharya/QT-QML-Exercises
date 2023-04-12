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
#include <chrono>
#include "MediaRunnable.h"
#include "FFmpegStruct.h"
#include "FrameSourceBase.h"
#include "MediaLayoutItemViewModel.h"
#include "IMV1.h"
#include "VideoFilterStruct.h"


struct PtzParam
{
    bool act = false;
    int pan = 0;
    int tilt = 0;
    int zoom = 0;
    int viewIndex = 1;
};


struct PtzAbsoluteParam
{
    bool act = false;
    float pointX = 0;
    float pointY = 0;
    float pointW = 0;
    float pointH = 0;
    float absZoom = 0;

    long long animationUsec = 0;
    long long elaspedUsec = 0;
    float fromPan = 0;
    float fromTilt = 0;
    float fromZoom = 0;
    float toPan = 0;
    float toTilt = 0;
    float toZoom = 0;
    float diffPan = 0;
    float diffTilt = 0;
    float diffZoom = 0;
    int viewIndex = 1;
};


class ImvDewarp : public QObject, public MediaRunnable
{
    Q_OBJECT
public:
    explicit ImvDewarp(QObject *parent = nullptr);
    ~ImvDewarp();

    void setup(const int location,
               const QString &mediaName,
               const int viewMode,
               const QString &cameraLensType,
               const qreal screenPixelRatio,
               const int outWidth, const int outHeight,
               const bool isViewPortMode);
    void setViewPort(const QRectF nRect);
    void setViewMode(const int viewMode);
    void setFisheyeDewarpOutputParam(const int width, const int height);
    void setLensParam(const QString& lensType, const int location);
    void setViewPosition(const QVector<qreal> positions);

    void onNewVideoFrame(const QVideoFrame &frame, const qint64 playbackTimeMsec, const bool jpegColorSpace = false);
    void onLastVideoFrame(const QVideoFrame &frame, const qint64 playbackTimeMsec, const bool jpegColorSpace = false);

    /* ptz action */
    void ptzContinuous(const int viewIndex, const int pan, const int tilt, const int zoom);
    void ptzStop(const int viewIndex);
    void ptzZoom1x(const int viewIndex);
    void ptzPointMove(const int viewIndex, const qreal x, const qreal y);
    void ptzAreaMove(const int viewIndex, const qreal x, const qreal y, const qreal width, const qreal height);
    void ptzAbsZoom(const int viewIndex, const qreal zoomValue);
    void sleep(const bool isOn);
protected:
    void loopTask() override;

signals:
    void newVideoFrame(const QVideoFrame &frame, const qint64 playbackTimeMsec, const bool jpegColorSpace = false);
    void setupResult(int errorCode);
    void fisheyeViewPositionChanged(const QVector<qreal> positions);
    void fisheyeViewPortChanged(const QVector<qreal> positions);

public:
    void resetViewPosition();

private:
    void reset(QVideoFrameInfo& videoIn);
    void resetInputVideoParam(QVideoFrameInfo& videoIn);
    void resetOutputVideoParam();

    void setThreadCount(const int width);
    void calculateOutSize(const int inWidth, const int inHeight,
                          int &outWidth, int &outHeight);
    void checkCommandParam(const std::chrono::steady_clock::time_point& now,
                           CommandParam& commandParam);
    void initAbsolutePtz(const std::chrono::steady_clock::time_point& now,
                         const bool isPtzPointMoveCommand,
                         const bool isPtzZoom1xCommand);
    void moveAbsolutePtz();
    void setPositionWithViewPort(const QRectF& newViewPort, const QVideoFrameInfo& newVideo);
    void setPositionWithViewMode(const QVector<qreal>&viewPosition);
    void updateCurrentViewPosition();
    void updateCurrentInputViewPort();
private:
    std::mutex      m_mutex;
    bool            m_isActive = false;
    bool            m_isNewVideoFrame = false;
    bool            m_isSetViewPortCommand = false;
    bool            m_isSetViewModeCommand = false;
    bool            m_isSetLensParamCommand = false;
    bool            m_isUpdateOutputParamCommand = false;
    bool            m_isPtzCommand = false;
    bool            m_isPtzZoom1xCommand = false;
    bool            m_isPtzPointMoveCommand = false;
    bool            m_isPtzAreaMoveCommand = false;
    bool            m_isPtzAbsZoomCommand = false;
    bool            m_isSetViewPositionCommand = false;
    bool            m_isSleepCommand = false;

    bool            m_isViewPortMode = false;
    QVideoFrameInfo m_lastVideoFrame;
    QRectF          m_lastViewPort;
    int             m_lastViewMode = (int)MediaLayoutItemViewModel::V_Original;
    int             m_lastWidth = 0;
    int             m_lastHeight = 0;
    PtzParam        m_lastPtzParam;
    PtzAbsoluteParam m_lastPtzAbsoluteParam;
    QString         m_lastLensType;
    int             m_lastLensLocation = (int)MediaLayoutItemViewModel::L_Ceiling;
    QVector<qreal>  m_lastSetViewPosition;
    bool            m_newSleepStatus = false;
    bool            m_isSleep = false;

    std::string     m_mediaName;
    IMV_Buffer      m_inputBuffer;
    IMV_Buffer      m_outputBuffer;
    IMV_CameraInterface m_cam;
    VideoOutputFrameSharedPtr m_outFrame;
    QString         m_lensType;
    int             m_location = (int)MediaLayoutItemViewModel::L_Ceiling;
    int             m_viewMode = (int)MediaLayoutItemViewModel::V_Original;
    qreal           m_screenPixelRatio = 1.0f;
    int             m_outWidth = 100;
    int             m_outHeight = 100;
    QVideoFrame::PixelFormat   m_pixelFormat = QVideoFrame::PixelFormat::Format_Invalid;

    QVector<qreal>  m_setViewPosition;

    /* PTZ Parameter */
    PtzParam        m_currentPtzParam;
    std::chrono::steady_clock::time_point m_lastPtzActPoint;
    long long       m_lastPtzClockGap = 0;
    int             m_updateFailCount = 0;
    int             m_setupResult = IMV_Defs::E_ERR_OK;

    PtzAbsoluteParam m_currentPtzAbsoluteParam;

    /* performance check */
    int             m_ptzMovementUsec = 33000;
    int             m_checkFrameCount = 1800; // 30fps 기준 1분 (info level)
    size_t          m_dewarpCount = 0;
    size_t          m_dewarpTimeMsec = 0;
    size_t          m_maxDewarpTimeMsec = 0;

};

