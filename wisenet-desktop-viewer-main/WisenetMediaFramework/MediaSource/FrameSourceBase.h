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
#include <QUuid>
#include "FFmpegStruct.h"
#include "WisenetVideoBuffer.h"
#include "WisenetMediaParam.h"
#include "MediaLayoutItemViewModel.h"
#include "PtzPresetItemModel.h"
#include "LogSettings.h"


class FrameSourceBase : public QObject
{
    Q_OBJECT
public:
    explicit FrameSourceBase(QObject *parent = nullptr)
        : QObject(parent)
        , m_status(0)
    {
        m_sourceId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    }
    virtual ~FrameSourceBase()
    {

    }

    enum PlayStatus
    {
        NoMedia,
        Loading,                    // loading (new open or stream changing)
        Loaded,                     // loaded
        ReadyToLoaded,              // ready to loading (pre loading status)
        Failed_InvalidMedia,        // failed, open failed (ResourceError, StreamTypeError)
        Failed_ChannelDisconnected, // channel disconnected
        Failed_MediaDisconnected,   // media session disconnected
        Failed_InvalidFormat,       // failed, open failed (FormatError)
        Failed_NoPermission,        // failed, PermissonError
        Failed_SessionFull,         // failed, SessionFullError
        Failed_PasswordRequired,    // failed, Password Required
        Failed_Timedout,            // failed, video loss for live, no recording data for playback
        Failed_FirmwareUpdating,    // failed, firmware updating
        EndOfMedia
    };
    Q_ENUM(PlayStatus)

    virtual void requestLastVideoFrame() {
        // 멀티쓰레드 FrameSource를 위해서 request후 쓰레드에서 마지막 프레임을 올려준다.
    }
    virtual void updatePlaybackStatus(const bool isPlay, const float speed, const bool playWait) {
        Q_UNUSED(isPlay); Q_UNUSED(speed); Q_UNUSED(playWait);
    }
    virtual void requestLastOriginalVideoImage() {
        // 마지막으로 디코딩 한 Video의 원본크기 이미지를 lastOriginalVideoImage(const QImage &image) signal로 올려준다.
    }

    void onNewVideoFrame(const VideoOutputFrameSharedPtr& videoOutput)
    {
        auto videoBuffer = new WisenetVideoBuffer();
        if(m_keepLastVideoOutput)
            videoBuffer->copyDataFrom(videoOutput); // copy data pointer to video buffer
        else
            videoBuffer->moveDataFrom(videoOutput); // move data pointer to video buffer

        QSize videoSize(videoOutput->width(), videoOutput->height());
        //SPDLOG_DEBUG("new video frame::{}, {}", videoSize.width(), videoBuffer->ptsTimeMsec());
        QVideoFrame frame(videoBuffer, videoSize, videoBuffer->pixelFormat());

        m_lastVideoFrame = frame;
        m_lastPlaybackTimeMsec = videoBuffer->ptsTimeMsec();
        m_lastJpegColorSpace = videoBuffer->useJpegColorSpace();

        if(m_keepLastVideoOutput) {
            m_lastVideoOutputPtr = videoOutput;
            m_lastImage = m_lastVideoFrame.image();
        }

        emit newVideoFrame(m_lastVideoFrame, m_lastPlaybackTimeMsec, m_lastJpegColorSpace);
        setVideoOutSize(videoSize);
    }

    void onLastVideoFrame()
    {
        if(m_keepLastVideoOutput && m_lastVideoOutputPtr != nullptr) {
            // create new video from m_lastVideoOutputPtr when keep last video output
            auto videoBuffer = new WisenetVideoBuffer();
            videoBuffer->copyDataFrom(m_lastVideoOutputPtr);
            QSize videoSize(m_lastVideoOutputPtr->width(), m_lastVideoOutputPtr->height());
            QVideoFrame frame(videoBuffer, videoSize, videoBuffer->pixelFormat());
            if(frame.isValid()) {
                emit newVideoFrame(frame, m_lastPlaybackTimeMsec, m_lastJpegColorSpace);
                return;
            }
        }

        if (m_lastVideoFrame.isValid()) {
            emit newVideoFrame(m_lastVideoFrame, m_lastPlaybackTimeMsec, m_lastJpegColorSpace);
        }
    }

    void onNewAudioFrame(const AudioOutputFrameSharedPtr& audioOutput)
    {
        emit newAudioFrame(audioOutput);
    }

    void onNewMetaFrame(const MetaFrameSharedPtr& metaData)
    {
        emit newMetaFrame(metaData);
    }

    QString sourceId() const { return m_sourceId; }
    QString mediaName() const { return m_mediaName; }

    bool supportVideoIn() const {return m_supportVideoIn;}
    bool supportAudioIn() const {return m_supportAudioIn;}
    bool supportAudioOut() const {return m_supportAudioOut;}
    bool supportDualStreamLive() const {return m_supportDualStreamLive;}
    bool supportDualStreamPlayback() const {return m_supportDualStreamPlayback;}
    bool supportPanTilt() const {return m_supportPanTilt;}
    bool supportZoom() const {return m_supportZoom;}
    bool supportZoom1x() const {return m_supportZoom1x;}
    bool supportAreaZoom() const {return m_supportAreaZoom;}
    bool supportPtzHome() const {return m_supportPtzHome;}
    bool supportPreset() const {return m_supportPreset;}
    bool supportSwing() const {return m_supportSwing;}
    bool supportGroup() const {return m_supportGroup;}
    bool supportTour() const {return m_supportTour;}
    bool supportTrace() const {return m_supportTrace;}
    bool supportSimpleFocus() const {return m_supportSimpleFocus;}
    bool supportAutoFocus() const {return m_supportAutoFocus;}
    bool supportResetFocus() const {return m_supportResetFocus;}
    bool supportDefog() const {return m_supportDefog;}
    bool supportFocus() const {return m_supportFocus;}
    bool supportIris() const {return m_supportIris;}
    bool supportLive() const {return m_supportLive;}
    bool supportPlayback() const {return m_supportPlayback;}
    bool supportBackup() const {return m_supportBackup;}
    bool supportDuration() const {return m_supportDuration;}
    bool supportTimeline() const {return m_supportTimeline;}
    bool supportMic() const {return m_supportMic;}
    bool enablePtz() const{return m_enablePtz;}
    bool supportSmartSearch() const { return m_supportSmartSearch;}
    bool supportSmartSearchFilter() const { return m_supportSmartSearchFilter; }
    bool supportFisheyeDewarp() const {return m_supportFisheyeDewarp;}
    bool fisheyeDewarpStatus() const {return m_fisheyeDewarpStatus;}
    int fisheyeLocation() const {return m_fisheyeLocation;}
    QString fisheyeLensType() const {return m_fisheyeLensType;}
    bool micStatus() const {return m_micStatus;}

    qint64 duration() const {return m_duration;}
    // preset
    int maxPtzPresets() const {return m_maxPtzPresets;}
    QList<PtzPresetItemModel*>& ptzPresets() {return m_ptzPresets;}
    // swing
    int swingType() const {return m_swingType;}
    int swingPanStartPreset() const {return m_swingPanStartPreset;}
    int swingPanEndPreset() const {return m_swingPanEndPreset;}
    int swingTiltStartPreset() const {return m_swingTiltStartPreset;}
    int swingTiltEndPreset() const {return m_swingTiltEndPreset;}
    int swingPanTiltStartPreset() const {return m_swingPanTiltStartPreset;}
    int swingPanTiltEndPreset() const {return m_swingPanTiltEndPreset;}
    // group
    QMap<int,QList<int>> groupPresets() const {return m_groupPresets;}
    // tour
    QList<int> tourPresets() const {return m_tourPresets;}
    // trace
    QList<int> tracePresets() const {return m_tracePresets;}

    QSize videoSize() const {return m_videoSize;}
    QSize videoOutSize() const {return m_videoOutSize;}
    QString videoCodec() const {return m_videoCodec;}
    QString audioCodec() const {return m_audioCodec;}
    float videoKbps() const {return m_videoKbps;}
    float videoFps() const {return m_videoFps;}
    QString videoProfile() const{return m_videoProfile;}
    int videoBufferingCount() const{return m_videoBufferingCount;}
    int status() const {return static_cast<int>(m_status);}
    bool enableAudioOut() const {return m_enableAudioOut;}
    float speed() const {return m_playSpeed;}
    int playbackState() const {return m_playbackState;}

    bool deviceUsingSetup() const {return m_deviceUsingSetup;}

    virtual void devicePtzContinuous(const int pan, const int tilt, const int zoom)
    { Q_UNUSED(pan); Q_UNUSED(tilt); Q_UNUSED(zoom);}
    virtual void devicePtzStop() {}
    virtual void devicePtzPresetGet() {}
    virtual void devicePtzPresetMove(const int presetNumber)
    {Q_UNUSED(presetNumber);}
    virtual void devicePtzPresetDelete(const int presetNumber)
    {Q_UNUSED(presetNumber);}
    virtual void devicePtzPresetAdd(const int presetNumber, const QString& presetName)
    {Q_UNUSED(presetNumber); Q_UNUSED(presetName);}
    // swing
    virtual void devicePtzSwingGet() {}
    virtual void devicePtzSwingMoveStop(const Wisenet::Device::SwingActionType swingActionType)
    {Q_UNUSED(swingActionType);}
    // group
    virtual void devicePtzGroupGet() {}
    virtual void devicePtzGroupMove(const int groupActionType)
    {Q_UNUSED(groupActionType);}
    virtual void devicePtzGroupStop(const int groupActionType)
    {Q_UNUSED(groupActionType);}
    // tour
    virtual void devicePtzTourGet() {}
    virtual void devicePtzTourMove() {}
    virtual void devicePtzTourStop() {}
    // trace
    virtual void devicePtzTraceGet() {}
    virtual void devicePtzTraceMove(const int traceNumber)
    {Q_UNUSED(traceNumber);}
    virtual void devicePtzTraceStop(const int traceNumber)
    {Q_UNUSED(traceNumber);}
    virtual void deviceFocusContinuous(const int command)
    { Q_UNUSED(command); }
    virtual void deviceFocusModeControl(const int focusMode)
    { Q_UNUSED(focusMode); }

    virtual void setOutputSize(const QSize size)
    { Q_UNUSED(size);}

    virtual void setKeepOriginalSize(const bool keep)
    { Q_UNUSED(keep);}
    virtual void deviceTalk(const bool on)
    { Q_UNUSED(on);}

    virtual void deviceUsingSetup(const bool deviceUsingSetup)
    { Q_UNUSED(deviceUsingSetup);}

    virtual void setOutPixelFormat(const VideoOutPixelFormat pixelFormat)
    { Q_UNUSED(pixelFormat);}

    virtual void setMaximumVideoSize(const int size)
    { Q_UNUSED(size);}

    void setSpeed(const float speed)
    {
        if (m_playSpeed != speed) {
            m_playSpeed = speed;
            emit speedChanged();
        }
    }
    void setPlaybackState(const int state)
    {
        m_playbackState = state;
        emit playbackStateChanged();
    }

    void setVideoProfile(const QString& videoProfile) {
        if (m_videoProfile != videoProfile) {
            m_videoProfile = videoProfile;
            emit videoProfileChanged();
        }
    }

    void setEnableAudioOut(const bool onOff)
    {
        if (m_enableAudioOut != onOff) {
            m_enableAudioOut = onOff;
        }
    }

    void setStatus(const PlayStatus status)
    {
        int rStatus = static_cast<int>(status);
        if (m_status != rStatus) {
            m_status = rStatus;
            emit statusChanged();
        }
    }

    void setMediaName(const QString& name) {
        if (m_mediaName != name) {
            m_mediaName = name;
            m_mediaStdName = m_mediaName.toStdString();
            emit mediaNameChanged();
        }
    }
    void setSupportVideoIn(const bool bSupport) {
        if (m_supportVideoIn != bSupport) {
            m_supportVideoIn = bSupport;
            emit supportVideoInChanged();
        }
    }
    void setSupportAudioIn(const bool bSupport) {
        if (m_supportAudioIn != bSupport) {
            m_supportAudioIn = bSupport;
            emit supportAudioInChanged();
        }
    }
    void setSupportAudioOut(const bool bSupport) {
        if (m_supportAudioOut != bSupport) {
            m_supportAudioOut = bSupport;
            emit supportAudioOutChanged();
        }
    }
    void setSupportDualStreamLive(const bool bSupport) {
        if (m_supportDualStreamLive != bSupport) {
            m_supportDualStreamLive = bSupport;
            emit supportDualStreamLiveChanged();
        }
    }
    void setSupportDualStreamPlayback(const bool bSupport) {
        if (m_supportDualStreamPlayback != bSupport) {
            m_supportDualStreamPlayback = bSupport;
            emit supportDualStreamPlaybackChanged();
        }
    }
    void setSupportPanTilt(const bool bSupport) {
        if (m_supportPanTilt != bSupport) {
            m_supportPanTilt = bSupport;
            emit supportPanTiltChanged();
        }
    }
    void setSupportZoom(const bool bSupport) {
        if (m_supportZoom != bSupport) {
            m_supportZoom = bSupport;
            emit supportZoomChanged();
        }
    }
    void setSupportZoom1x(const bool bSupport) {
        if (m_supportZoom1x != bSupport) {
            m_supportZoom1x = bSupport;
            emit supportZoom1xChanged();
        }
    }
    void setEnablePtz(const bool bEnable) {
        if (m_enablePtz != bEnable) {
            m_enablePtz = bEnable;
            emit enablePtzChanged();
        }
    }
    void setSupportAreaZoom(const bool bSupport) {
        if (m_supportAreaZoom != bSupport) {
            m_supportAreaZoom = bSupport;
            emit supportAreaZoomChanged();
        }
    }
    void setSupportPtzHome(const bool bSupport) {
        if (m_supportPtzHome != bSupport) {
            m_supportPtzHome = bSupport;
            emit supportPtzHomeChanged();
        }
    }
    void setSupportPreset(const bool bSupport) {
        if (m_supportPreset != bSupport) {
            m_supportPreset = bSupport;
            emit supportPresetChanged();
        }
    }
    void setSupportSwing(const bool bSupport) {
        if (m_supportSwing != bSupport) {
            m_supportSwing = bSupport;
            emit supportSwingChanged();
        }
    }
    void setSupportGroup(const bool bSupport) {
        if (m_supportGroup != bSupport) {
            m_supportGroup = bSupport;
            emit supportGroupChanged();
        }
    }
    void setSupportTour(const bool bSupport) {
        if (m_supportTour != bSupport) {
            m_supportTour = bSupport;
            emit supportTourChanged();
        }
    }
    void setSupportTrace(const bool bSupport) {
        if (m_supportTrace != bSupport) {
            m_supportTrace = bSupport;
            emit supportTraceChanged();
        }
    }
    void setSupportSimpleFocus(const bool bSupport) {
        if (m_supportSimpleFocus != bSupport) {
            m_supportSimpleFocus = bSupport;
            emit supportSimpleFocusChanged();
        }
    }
    void setSupportAutoFocus(const bool bSupport) {
        if (m_supportAutoFocus != bSupport) {
            m_supportAutoFocus = bSupport;
            emit supportAutoFocusChanged();
        }
    }
    void setSupportResetFocus(const bool bSupport) {
        if (m_supportResetFocus != bSupport) {
            m_supportResetFocus = bSupport;
            emit supportResetFocusChanged();
        }
    }
    void setSupportDefog(const bool bSupport) {
        if (m_supportDefog != bSupport) {
            m_supportDefog = bSupport;
            emit supportDefogChanged();
        }
    }
    void setSupportIris(const bool bSupport) {
        if (m_supportIris != bSupport) {
            m_supportIris = bSupport;
            emit supportIrisChanged();
        }
    }
    void setSupportFocus(const bool bSupport) {
        if (m_supportFocus != bSupport) {
            m_supportFocus = bSupport;
            emit supportFocusChanged();
        }
    }
    void setSupportLive(const bool bSupport) {
        if (m_supportLive != bSupport) {
            m_supportLive = bSupport;
            emit supportLiveChanged();
        }
    }
    void setSupportPlayback(const bool bSupport) {
        if (m_supportPlayback != bSupport) {
            m_supportPlayback = bSupport;
            emit supportPlaybackChanged();
        }
    }
    void setSupportBackup(const bool bSupport) {
        if (m_supportBackup != bSupport) {
            m_supportBackup = bSupport;
            emit supportBackupChanged();
        }
    }
    void setSupportDuration(const bool bSupport) {
        if (m_supportDuration != bSupport) {
            m_supportDuration = bSupport;
            emit supportDurationChanged();
        }
    }
    void setSupportTimeline(const bool bSupport) {
        if (m_supportTimeline != bSupport) {
            m_supportTimeline = bSupport;
            emit supportTimelineChanged();
        }
    }
    void setSupportMic(const bool bSupport) {
        if (m_supportMic != bSupport) {
            m_supportMic = bSupport;
            emit supportMicChanged();
        }
    }
    void setDuration(const qint64 duration) {
        if (m_duration != duration) {
            m_duration = duration;
            emit durationChanged();
        }
    }
    void setVideoSize(const QSize& size) {
        if (size != m_videoSize) {
            m_videoSize = size;
            emit videoSizeChanged();
        }
    }
    void setVideoSize(int width, int height) {
        if (width != m_videoSize.width() || height != m_videoSize.height()) {
            m_videoSize = QSize(width, height);
            emit videoSizeChanged();
        }
    }
    void setVideoOutSize(const QSize& size) {
        if (size != m_videoOutSize) {
            m_videoOutSize = size;
            SPDLOG_DEBUG("setVideoOutSize(), width={}x{}", size.width(), size.height());
            emit videoOutSizeChanged();
        }
    }
    void setVideoCodec(const QString& codec) {
        if (m_videoCodec != codec) {
            m_videoCodec = codec;
            emit videoCodecChanged();
        }
    }
    void setAudioCodec(const QString& codec) {
        if (m_audioCodec != codec) {
            m_audioCodec = codec;
            emit audioCodecChanged();
        }
    }

    void setVideoKbps(const float kbps)
    {
        if (m_videoKbps != kbps) {
            m_videoKbps = kbps;
            emit videoKbpsChanged();
        }
    }

    void setVideoFps(const float fps)
    {
        if (m_videoFps != fps) {
            m_videoFps = fps;
            emit videoFpsChanged();
        }
    }
    void setMaxPtzPresets(const int maxPresets)
    {
        if (m_maxPtzPresets != maxPresets) {
            m_maxPtzPresets = maxPresets;
            emit maxPtzPresetsChanged();
        }
    }

    void setVideoBufferingCount(const int bufferCount)
    {
        if (m_videoBufferingCount != bufferCount) {
            m_videoBufferingCount = bufferCount;
            emit videoBufferingCountChanged();
        }
    }

    bool permissionPlayback() const {return m_permissionPlayback;}
    bool permissionLocalRecording() const {return m_permissionLocalRecording;}
    bool permissionPtzControl() const {return m_permissionPtzControl;}
    bool permissionAudioIn() const {return m_permissionAudioIn;}
    bool permissionMic() const {return m_permissionMic;}
    void setPermissionPlayback(const bool support)
    {
        if (m_permissionPlayback != support) {
            m_permissionPlayback = support;
            emit permissionPlaybackChanged();
        }
    }
    void setPermissionLocalRecording(const bool support)
    {
        if (m_permissionLocalRecording != support) {
            m_permissionLocalRecording = support;
            emit permissionLocalRecordingChanged();
        }
    }
    void setPermissionPtzControl(const bool support)
    {
        if (m_permissionPtzControl != support) {
            m_permissionPtzControl = support;
            emit permissionPtzControlChanged();
        }
    }
    void setPermissionAudioIn(const bool support)
    {
        if (m_permissionAudioIn != support) {
            m_permissionAudioIn = support;
            emit permissionAudioInChanged();
        }
    }
    void setPermissionMic(const bool support)
    {
        if (m_permissionMic != support) {
            m_permissionMic = support;
            emit permissionMicChanged();
        }
    }

    WisenetMediaParam::StreamType streamType() const {return m_streamType;}
    void setStreamType(const WisenetMediaParam::StreamType type)
    {
        if (m_streamType != type) {
            m_streamType = type;
            emit streamTypeChanged();
        }
    }

    void setFisheyeParam(const bool support, const QString& lensType, const int location)
    {
        setSupportFisheyeDewarp(support);
        if (support) {
            setFisheyeLensType(lensType);
            setFisheyeLocation(location);
        }
        emit fisheyeParamChanged();
    }

    void setFisheyeDewarpStatus(const bool active)
    {
        if (m_fisheyeDewarpStatus != active) {
            m_fisheyeDewarpStatus = active;
            emit fisheyeDewarpStatusChanged();
        }
    }

    void setSupportFisheyeDewarp(const bool support)
    {
        if (m_supportFisheyeDewarp != support) {
            m_supportFisheyeDewarp = support;
            emit supportFisheyeDewarpChanged();
        }
    }

    void setFisheyeLocation(const int location)
    {
        if (m_fisheyeLocation != location) {
            m_fisheyeLocation = location;
            emit fisheyeLocationChanged();
        }
    }
    void setFisheyeLensType(const QString& lensType)
    {
        if (m_fisheyeLensType != lensType) {
            m_fisheyeLensType = lensType;
            emit fisheyeLensTypeChanged();
        }
    }
    void setMicStatus(const bool active)
    {
        if (m_micStatus != active) {
            m_micStatus = active;
            emit micStatusChanged();
        }
    }

    void setSupportSmartSearch(const bool bSupport) {
        if (m_supportSmartSearch != bSupport) {
            m_supportSmartSearch = bSupport;
            emit supportSmartSearchChanged();
        }
    }

    void setSupportSmartSearchFilter(const bool bSupport) {
        if (m_supportSmartSearchFilter != bSupport) {
            m_supportSmartSearchFilter = bSupport;
            emit supportSmartSearchFilterChanged();
        }
    }

    bool keepLastVideoOutput() const {return m_keepLastVideoOutput;}
    void setKeepLastVideoOutput(bool keepLastVideo) {
        if(m_keepLastVideoOutput != keepLastVideo) {
            m_keepLastVideoOutput = keepLastVideo;
            emit keepLastVideoOutputChanged();
        }
    }
    QImage getLastImage(){ return m_lastImage; }

signals:
    void newVideoFrame(const QVideoFrame &frame, const qint64 playbackTimeMsec, const bool jpegColorSpace = false);
    void newAudioFrame(const AudioOutputFrameSharedPtr& audioFrame);
    void newMetaFrame(const MetaFrameSharedPtr& metaFrame);
    void lastOriginalVideoImage(const QImage &image);

    void mediaNameChanged();
    void supportVideoInChanged();
    void supportAudioInChanged();
    void supportAudioOutChanged();
    void supportDualStreamLiveChanged();
    void supportDualStreamPlaybackChanged();
    void supportPanTiltChanged();
    void supportZoomChanged();
    void supportZoom1xChanged();
    void supportAreaZoomChanged();
    void supportPtzHomeChanged();
    void supportPresetChanged();
    void supportSwingChanged();
    void supportGroupChanged();
    void supportTourChanged();
    void supportTraceChanged();
    void supportSimpleFocusChanged();
    void supportAutoFocusChanged();
    void supportResetFocusChanged();
    void supportDefogChanged();
    void supportIrisChanged();
    void supportFocusChanged();
    void supportLiveChanged();
    void supportPlaybackChanged();
    void supportBackupChanged();
    void supportDurationChanged();
    void supportTimelineChanged();
    void supportMicChanged();
    void supportSmartSearchChanged();
    void supportSmartSearchFilterChanged();
    void durationChanged();
    void maxPtzPresetsChanged();
    void ptzPresetsChanged();
    // swing
    void swingPanStartPresetChanged();
    void swingPanEndPresetChanged();
    void swingTiltStartPresetChanged();
    void swingTiltEndPresetChanged();
    void swingPanTiltStartPresetChanged();
    void swingPanTiltEndPresetChanged();
    // group
    void groupPresetsChanged();
    // tour
    void tourPresetsChanged();
    // trace
    void tracePresetsChanged();
    void enablePtzChanged();
    void supportFisheyeDewarpChanged();
    void fisheyeDewarpStatusChanged();
    void fisheyeLocationChanged();
    void fisheyeLensTypeChanged();
    void fisheyeParamChanged();
    void micStatusChanged();

    void videoSizeChanged();
    void videoOutSizeChanged();
    void videoCodecChanged();
    void audioCodecChanged();
    void videoKbpsChanged();
    void videoFpsChanged();
    void videoProfileChanged();
    void videoBufferingCountChanged();
    void statusChanged();
    void streamTypeChanged();
    void speedChanged();
    void playbackStateChanged();

    void permissionPlaybackChanged();
    void permissionLocalRecordingChanged();
    void permissionPtzControlChanged();
    void permissionAudioInChanged();
    void permissionMicChanged();

    void deviceTalkResult(bool on, bool success, int errorCode);    //1 : already use , 2: mic in use

    void deviceUsingSetupChanged();
    void keepLastVideoOutputChanged();

protected:
    QString m_sourceId;

    // media name and capabilities
    QString m_mediaName;
    std::string m_mediaStdName;

    // video/audio
    bool m_supportVideoIn = true;
    bool m_supportAudioIn = true;
    bool m_supportAudioOut = false;

    // live/playback
    bool m_supportDualStreamLive = false;
    bool m_supportDualStreamPlayback = false;

    // device control
    bool m_supportPanTilt = false;
    bool m_supportZoom = false;
    bool m_supportZoom1x = false;
    bool m_supportAreaZoom = false;
    bool m_supportPtzHome = false;
    bool m_supportPreset = false;
    bool m_supportSwing = false;
    bool m_supportGroup = false;
    bool m_supportTour = false;
    bool m_supportTrace = false;
    bool m_supportSimpleFocus = false;
    bool m_supportResetFocus = false;
    bool m_supportAutoFocus = false;
    bool m_supportDefog = false;
    bool m_supportFocus = false;
    bool m_supportIris = false;
    bool m_enablePtz = false;

    bool m_supportLive = false;
    bool m_supportPlayback = false;
    bool m_supportBackup = false;
    bool m_supportDuration = false;
    bool m_supportTimeline = false;
    bool m_supportMic = false;
    bool m_supportSmartSearch = false;
    bool m_supportSmartSearchFilter = false;

    // user permission
    bool m_permissionPlayback = true;
    bool m_permissionLocalRecording = true;
    bool m_permissionPtzControl = true;
    bool m_permissionAudioIn = true;
    bool m_permissionMic = true;

    // fisheye dewarping parameter
    bool m_supportFisheyeDewarp = false; /* fisheye 렌즈로 설정한 상태 */
    int m_fisheyeLocation = (int)MediaLayoutItemViewModel::L_Ceiling;
    QString m_fisheyeLensType;
    bool m_fisheyeDewarpStatus = false; /* 드와핑 동작중인지 상태 */

    bool m_micStatus = false; /* mic동작중인지 상태 */

    qint64 m_duration = 0;

    // preset
    int m_maxPtzPresets = 0;
    QList<PtzPresetItemModel*> m_ptzPresets;
    // swing
    int m_swingType = 0;
    int m_swingPanStartPreset = 0;
    int m_swingPanEndPreset = 0;
    int m_swingTiltStartPreset = 0;
    int m_swingTiltEndPreset = 0;
    int m_swingPanTiltStartPreset = 0;
    int m_swingPanTiltEndPreset = 0;
    // group
    QMap<int,QList<int>> m_groupPresets;
    // tour
    QList<int> m_tourPresets;
    // trace
    QList<int> m_tracePresets;

    QSize m_videoSize;
    QSize m_videoOutSize;
    QString m_videoCodec = "None";
    QString m_audioCodec = "None";

    float m_videoKbps = 0;
    float m_videoFps = 0;
    QString m_videoProfile = "None";
    int m_videoBufferingCount = 0;
    bool m_enableAudioOut = false;
    float m_playSpeed = 1.0f;
    int m_playbackState = 0;
    bool m_deviceUsingSetup = false;

    WisenetMediaParam::StreamType m_streamType = WisenetMediaParam::StreamType::PlaybackHigh;

    std::atomic<int> m_status; //MediaControlBase::PlayStatus
    QVideoFrame m_lastVideoFrame;
    qint64 m_lastPlaybackTimeMsec = 0;
    bool m_lastJpegColorSpace = false;

    // for keep original image before masking
    bool m_keepLastVideoOutput = false;
    VideoOutputFrameSharedPtr m_lastVideoOutputPtr = nullptr;
    QImage m_lastImage;
};

Q_DECLARE_METATYPE(AudioOutputFrameSharedPtr)
Q_DECLARE_METATYPE(MetaFrameSharedPtr)
