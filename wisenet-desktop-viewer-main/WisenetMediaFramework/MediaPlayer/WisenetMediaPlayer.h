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
#include <QAbstractVideoSurface>
#include <QVideoSurfaceFormat>
#include <QSize>
#include <QAudioOutput>
#include <QQmlListProperty>
#include <QPointer>
#include "WisenetMediaParam.h"
#include "FFmpegStruct.h"
#include "MediaControlBase.h"
#include "PosMediaControl.h" // 2022.10.12. added
#include "FrameSourceBase.h"
#include "MediaLayoutItemViewModel.h"
#include "PtzPresetItemModel.h"
#include "ImvDewarp.h"
#include "Masking/CvAutoTracker.h"
#include "Masking//MaskItemListModel.h"
#include "Masking/CvMaskPainter.h"
#include "Masking/MaskingTranscoder.h"
#include "Masking/MaskingExportProgressModel.h"

class WisenetMediaPlayer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QAbstractVideoSurface* videoSurface READ videoSurface WRITE setVideoSurface)
    Q_PROPERTY(FrameSourceBase* mediaSource READ mediaSource NOTIFY mediaSourceChanged)
    Q_PROPERTY(WisenetMediaParam* mediaParam READ mediaParam)
    Q_PROPERTY(QString mediaName READ mediaName NOTIFY mediaNameChanged)
    Q_PROPERTY(bool supportVideoIn READ supportVideoIn NOTIFY supportVideoInChanged)
    Q_PROPERTY(bool supportAudioIn READ supportAudioIn NOTIFY supportAudioInChanged)
    Q_PROPERTY(bool supportAudioOut READ supportAudioOut NOTIFY supportAudioOutChanged)
    Q_PROPERTY(bool supportDualStreamLive READ supportDualStreamLive NOTIFY supportDualStreamLiveChanged)
    Q_PROPERTY(bool supportDualStreamPlayback READ supportDualStreamPlayback NOTIFY supportDualStreamPlaybackChanged)
    Q_PROPERTY(bool supportPanTilt READ supportPanTilt NOTIFY supportPanTiltChanged)
    Q_PROPERTY(bool supportZoom READ supportZoom NOTIFY supportZoomChanged)
    Q_PROPERTY(bool supportZoom1x READ supportZoom1x NOTIFY supportZoom1xChanged)
    Q_PROPERTY(bool supportAreaZoom READ supportAreaZoom NOTIFY supportAreaZoomChanged)
    Q_PROPERTY(bool supportPtzHome READ supportPtzHome NOTIFY supportPtzHomeChanged)
    Q_PROPERTY(bool supportPreset READ supportPreset NOTIFY supportPresetChanged)
    Q_PROPERTY(bool supportSwing READ supportSwing NOTIFY supportSwingChanged)
    Q_PROPERTY(bool supportGroup READ supportGroup NOTIFY supportGroupChanged)
    Q_PROPERTY(bool supportTour READ supportTour NOTIFY supportTourChanged)
    Q_PROPERTY(bool supportTrace READ supportTrace NOTIFY supportTraceChanged)
    Q_PROPERTY(bool supportDefog READ supportDefog NOTIFY supportDefogChanged)
    Q_PROPERTY(bool supportSimpleFocus READ supportSimpleFocus NOTIFY supportSimpleFocusChanged)
    Q_PROPERTY(bool supportAutoFocus READ supportAutoFocus NOTIFY supportAutoFocusChanged)
    Q_PROPERTY(bool supportResetFocus READ supportResetFocus NOTIFY supportResetFocusChanged)
    Q_PROPERTY(bool supportFocus READ supportFocus NOTIFY supportFocusChanged)
    Q_PROPERTY(bool supportIris READ supportIris NOTIFY supportIrisChanged)
    Q_PROPERTY(bool supportDuration READ supportDuration NOTIFY supportDurationChanged)
    Q_PROPERTY(bool supportTimeline READ supportTimeline NOTIFY supportTimelineChanged)
    Q_PROPERTY(bool supportSmartSearch READ supportSmartSearch NOTIFY supportSmartSearchChanged)
    Q_PROPERTY(bool supportSmartSearchFilter READ supportSmartSearchFilter NOTIFY supportSmartSearchFilterChanged)

    Q_PROPERTY(bool enablePtz READ enablePtz NOTIFY enablePtzChanged)
    Q_PROPERTY(bool permissionPlayback READ permissionPlayback NOTIFY permissionPlaybackChanged)
    Q_PROPERTY(bool permissionLocalRecording READ permissionLocalRecording NOTIFY permissionLocalRecordingChanged)
    Q_PROPERTY(bool permissionPtzControl READ permissionPtzControl NOTIFY permissionPtzControlChanged)
    Q_PROPERTY(bool permissionAudioIn READ permissionAudioIn NOTIFY permissionAudioInChanged)
    Q_PROPERTY(bool permissionMic READ permissionMic NOTIFY permissionMicChanged)

    Q_PROPERTY(bool supportFisheyeDewarp READ supportFisheyeDewarp NOTIFY supportFisheyeDewarpChanged)
    Q_PROPERTY(bool enableFisheyeDewarp READ enableFisheyeDewarp WRITE setEnableFisheyeDewarp NOTIFY enableFisheyeDewarpChanged)
    Q_PROPERTY(bool fisheyeDewarpStatus READ fisheyeDewarpStatus NOTIFY fisheyeDewarpStatusChanged)
    Q_PROPERTY(int fisheyeViewMode READ fisheyeViewMode WRITE setFisheyeViewMode NOTIFY fisheyeViewModeChanged)
    Q_PROPERTY(int fisheyeLocation READ fisheyeLocation NOTIFY fisheyeLocationChanged)

    Q_PROPERTY(bool supportMic READ supportMic NOTIFY supportMicChanged)

    Q_PROPERTY(ItemProfile itemProfile READ itemProfile WRITE setItemProfile NOTIFY itemProfileChanged)

    // preset
    Q_PROPERTY(int maxPtzPresets READ maxPtzPresets  NOTIFY maxPtzPresetsChanged)
    Q_PROPERTY(QQmlListProperty<PtzPresetItemModel> ptzPresets READ ptzPresets NOTIFY ptzPresetsChanged)
    // swing
    Q_PROPERTY(int swingType READ swingType  NOTIFY swingTypeChanged)
    Q_PROPERTY(int swingPanStartPreset READ swingPanStartPreset  NOTIFY swingPanStartPresetChanged)
    Q_PROPERTY(int swingPanEndPreset READ swingPanEndPreset  NOTIFY swingPanEndPresetChanged)
    Q_PROPERTY(int swingTiltStartPreset READ swingTiltStartPreset  NOTIFY swingTiltStartPresetChanged)
    Q_PROPERTY(int swingTiltEndPreset READ swingTiltEndPreset  NOTIFY swingTiltEndPresetChanged)
    Q_PROPERTY(int swingPanTiltStartPreset READ swingPanTiltStartPreset  NOTIFY swingPanTiltStartPresetChanged)
    Q_PROPERTY(int swingPanTiltEndPreset READ swingPanTiltEndPreset  NOTIFY swingPanTiltEndPresetChanged)
    // group
    Q_PROPERTY(QMap<int,QList<int>> groupPresets READ groupPresets NOTIFY groupPresetsChanged)
    // tour
    Q_PROPERTY(QList<int> tourPresets READ tourPresets NOTIFY tourPresetsChanged)
    // trace
    Q_PROPERTY(QList<int> tracePresets READ tracePresets NOTIFY tracePresetsChanged)

    // 현재 플레이하는 위치 (utc millisecond or video play time)
    Q_PROPERTY(qint64 position READ position NOTIFY positionChanged)
    // 현재 플레이하는 위치 (utc second or video play time)
    Q_PROPERTY(qint64 positionSeconds READ positionSeconds NOTIFY positionSecondsChanged)

    // 비디오 파일인 경우 비디오 재생시간
    Q_PROPERTY(qint64 duration READ duration NOTIFY durationChanged)

    // 현재 재생 속도
    Q_PROPERTY(float speed READ speed WRITE setSpeed NOTIFY speedChanged)
    Q_PROPERTY(bool seekable READ seekable NOTIFY seekableChanged)


    // 디코딩시 스케일 하지 않음.
    Q_PROPERTY(bool keepOriginalSize READ keepOriginalSize WRITE setKeepOriginalSize NOTIFY keepOriginalSizeChanged)

    Q_PROPERTY(PlaybackState playbackState READ playbackState WRITE setPlaybackState NOTIFY playbackStateChanged)
    Q_PROPERTY(PlayStatus status READ status NOTIFY statusChanged)
    Q_PROPERTY(QRectF viewPort READ viewPort WRITE setViewPort NOTIFY viewPortChanged)

    Q_PROPERTY(bool enableAudioOut READ enableAudioOut WRITE setEnableAudioOut NOTIFY enableAudioOutChanged)
    Q_PROPERTY(bool localRecordingStatus READ localRecordingStatus NOTIFY localRecordingStatusChanged)
    Q_PROPERTY(QString localRecordingPath READ localRecordingPath NOTIFY localRecordingPathChanged)

    Q_PROPERTY(bool micStatus READ micStatus NOTIFY micStatusChanged)

    // video status information
    Q_PROPERTY(QSize videoSize READ videoSize NOTIFY videoSizeChanged)
    Q_PROPERTY(QSize videoOutSize READ videoOutSize NOTIFY videoOutSizeChanged)

    Q_PROPERTY(QString videoCodec READ videoCodec NOTIFY videoCodecChanged)
    Q_PROPERTY(QString audioCodec READ audioCodec NOTIFY audioCodecChanged)
    Q_PROPERTY(float   videoKbps READ videoKbps NOTIFY videoKbpsChanged)
    Q_PROPERTY(float   videoFps READ videoFps NOTIFY videoFpsChanged)
    Q_PROPERTY(QString videoProfile READ videoProfile NOTIFY videoProfileChanged)
    Q_PROPERTY(int videoBufferingCount READ videoBufferingCount NOTIFY videoBufferingCountChanged)
    Q_PROPERTY(WisenetMediaParam::StreamType streamType READ streamType NOTIFY streamTypeChanged)

    // mediaID
    Q_PROPERTY(QString mediaId READ mediaId NOTIFY mediaIdChanged)

    Q_PROPERTY(bool isSequence READ isSequence WRITE setIsSequence NOTIFY isSequenceChanged)

    // 장비 설정 창 진입 유무
    Q_PROPERTY(bool deviceUsingSetup READ deviceUsingSetup NOTIFY deviceUsingSetupChanged)

    // Masking & Object Tracker
    Q_PROPERTY(MaskItemListModel* maskItemListModel READ maskItemListModel NOTIFY maskItemListModelChanged)
    Q_PROPERTY(bool enableMaskingMode READ enableMaskingMode WRITE setEnableMaskingMode NOTIFY enableMaskingModeChanged)
    Q_PROPERTY(bool maskPreviewOn READ maskPreviewOn WRITE setMaskPreviewOn NOTIFY maskPreviewOnChanged)
    Q_PROPERTY(MaskingExportProgressModel* maskingExportProgressModel READ maskingExportProgressModel NOTIFY maskingExportProgressModelChanged)
    Q_PROPERTY(bool isTracking READ isTracking NOTIFY isTrackingChanged)

public:
    // openResult()의 에러코드 (MediaControlBase::Error의 1:1 매핑)
    enum Error
    {
        NoError = MediaControlBase::NoError,
        FormatError = MediaControlBase::FormatError,
        ResourceError = MediaControlBase::ResourceError,
        StreamTypeError = MediaControlBase::StreamTypeError,
        PermissonError = MediaControlBase::PermissionError,
        SessionFullError = MediaControlBase::SessionFullError,
        PasswordRequired = MediaControlBase::PasswordRequired,
    };
    enum PlaybackState
    {
        PlayingState = MediaControlBase::PlayingState,
        PausedState = MediaControlBase::PausedState
    };
    // 미디어 상태 (FameSourceBase::PlayStatus의 1:1 매핑)
    enum PlayStatus
    {
        NoMedia = FrameSourceBase::NoMedia,
        Loading = FrameSourceBase::Loading,
        Loaded = FrameSourceBase::Loaded,
        ReadyToLoaded = FrameSourceBase::ReadyToLoaded,
        Failed_InvalidMedia = FrameSourceBase::Failed_InvalidMedia,
        Failed_ChannelDisconnected = FrameSourceBase::Failed_ChannelDisconnected,
        Failed_MediaDisconnected = FrameSourceBase::Failed_MediaDisconnected,
        Failed_InvalidFormat = FrameSourceBase::Failed_InvalidFormat,
        Failed_NoPermission = FrameSourceBase::Failed_NoPermission,
        Failed_SessionFull = FrameSourceBase::Failed_SessionFull,
        Failed_PasswordRequired = FrameSourceBase::Failed_PasswordRequired,
        Failed_Timedout = FrameSourceBase::Failed_Timedout,
        Failed_FirmwareUpdating = FrameSourceBase::Failed_FirmwareUpdating,
        EndOfMedia  = FrameSourceBase::EndOfMedia
    };
    enum ItemProfile // Same to MediaLayoutItemViewModel::ItemProfile
    {
        Auto,
        High,
        Low
    };

    Q_ENUM(Error)
    Q_ENUM(PlaybackState)
    Q_ENUM(PlayStatus)
    Q_ENUM(ItemProfile)


    explicit WisenetMediaPlayer(QObject *parent = nullptr);
    ~WisenetMediaPlayer();

    static void registerQml();

    QAbstractVideoSurface *videoSurface() const;
    void setVideoSurface(QAbstractVideoSurface *videoSurface);

    FrameSourceBase* mediaSource() const;    

    WisenetMediaParam* mediaParam() const;
    qint64 position() const;
    qint64 positionSeconds() const;

    float speed() const;
    void setSpeed(const float playbackSpeed);

    float seekable() const;

    ItemProfile itemProfile() const;
    void setItemProfile(const ItemProfile profile);

    bool keepOriginalSize() const;
    void setKeepOriginalSize(const bool keep);

    PlaybackState playbackState() const;
    void setPlaybackState(const PlaybackState playbackState);

    PlayStatus status() const;

    QString mediaName() const;
    bool supportVideoIn() const;
    bool supportAudioIn() const;
    bool supportAudioOut() const;
    bool supportDualStreamLive() const;
    bool supportDualStreamPlayback() const;
    bool supportPanTilt() const;
    bool supportZoom() const;
    bool supportZoom1x() const;
    bool supportAreaZoom() const;
    bool supportPtzHome() const;
    bool supportPreset() const;
    bool supportSwing() const;
    bool supportGroup() const;
    bool supportTour() const;
    bool supportTrace() const;
    bool supportSimpleFocus() const;
    bool supportAutoFocus() const;
    bool supportResetFocus() const;
    bool supportDefog() const;
    bool supportFocus() const;
    bool supportIris() const;
    bool supportDuration() const;
    bool supportTimeline() const;
    bool supportMic() const;
    bool supportSmartSearch() const;
    bool supportSmartSearchFilter() const;
    bool enablePtz() const;
    bool permissionPlayback() const;
    bool permissionLocalRecording() const;
    bool permissionPtzControl() const;
    bool permissionAudioIn() const;
    bool permissionMic() const;
    bool supportFisheyeDewarp() const;
    bool fisheyeDewarpStatus() const;
    bool enableFisheyeDewarp() const;
    bool micStatus() const;
    int fisheyeLocation() const;
    QString fisheyeLensType() const;
    int fisheyeViewMode() const;
    void setFisheyeViewMode(const int mode);
    void setEnableFisheyeDewarp(const bool enable);

    qint64 duration() const;
    QMap<qint64, qint64> getTimeline() const;

    QSize videoSize() const;
    QSize videoOutSize() const;
    QString videoCodec() const;
    QString audioCodec() const;
    float   videoKbps() const;
    float   videoFps()  const;
    QString videoProfile() const;
    int videoBufferingCount() const;
    QString mediaId() const;

    bool isSequence() const;
    void setIsSequence(bool isSequence);

    bool deviceUsingSetup() const;

    QRectF viewPort() const;
    void setViewPort(const QRectF normalizedRect);

    bool enableAudioOut() const;
    void setEnableAudioOut(const bool on);

    bool localRecordingStatus() const;
    QString localRecordingPath() const;    

    // preset
    int maxPtzPresets() const;
    QQmlListProperty<PtzPresetItemModel> ptzPresets();
    // swing
    int swingType() const;
    int swingPanStartPreset() const;
    int swingPanEndPreset() const;
    int swingTiltStartPreset() const;
    int swingTiltEndPreset() const;
    int swingPanTiltStartPreset() const;
    int swingPanTiltEndPreset() const;
    // group
    QMap<int,QList<int>> groupPresets() const;
    // tour
    QList<int> tourPresets() const;
    // trace
    QList<int> tracePresets() const;

    WisenetMediaParam::StreamType streamType() const;

    Q_INVOKABLE QString getDateTime24h(long long utcTimeMsec);
    Q_INVOKABLE bool hasControl();
    Q_INVOKABLE QVector<qreal> fisheyeViewPosition() const;
    Q_INVOKABLE void setFisheyeViewPosition(const QVector<qreal> positions);
    Q_INVOKABLE QList<int> getGroupKeyList();
    Q_INVOKABLE QList<int> getGroupPresetList(const int groupNumberKey);

    // Masking & Object tracker
    MaskItemListModel* maskItemListModel();
    bool enableMaskingMode() const;
    void setEnableMaskingMode(const bool enable);
    bool maskPreviewOn() const;
    void setMaskPreviewOn(const bool enable);
    MaskingExportProgressModel* maskingExportProgressModel();
    bool isTracking() const;

signals:
    void mediaSourceChanged();
    void positionChanged();
    void positionSecondsChanged();
    void playbackTimeChanged();
    void speedChanged();
    void seekableChanged();
    void keepOriginalSizeChanged();
    void openResult(MediaControlBase::Error error);
    void closed();
    void playbackStateChanged();

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
    void supportTraceChanged();
    void supportTourChanged();
    void supportSimpleFocusChanged();
    void supportAutoFocusChanged();
    void supportResetFocusChanged();
    void supportDefogChanged();
    void supportIrisChanged();
    void supportFocusChanged();
    void supportDurationChanged();
    void supportTimelineChanged();
    void supportMicChanged();
    void enablePtzChanged();
    void durationChanged();
    void supportFisheyeDewarpChanged();
    void enableFisheyeDewarpChanged();
    void fisheyeDewarpStatusChanged();
    void fisheyeLocationChanged();
    void fisheyeViewModeChanged();
    void fisheyeLensTypeChanged();
    void fisheyeViewPositionChanged();
    void fisheyeViewPortChanged(const QVector<qreal> positions);
    void micStatusChanged();
    void supportSmartSearchChanged();
    void supportSmartSearchFilterChanged();

    void permissionPlaybackChanged();
    void permissionLocalRecordingChanged();
    void permissionPtzControlChanged();
    void permissionAudioInChanged();
    void permissionMicChanged();

    void statusChanged();
    void viewPortChanged();
    void enableAudioOutChanged();
    void localRecordingStatusChanged();
    void localRecordingPathChanged();
    void imageSaved(bool result, QString path);

    void videoSizeChanged();
    void videoOutSizeChanged();
    void videoCodecChanged();
    void audioCodecChanged();
    void videoKbpsChanged();
    void videoFpsChanged();
    void videoProfileChanged(); // 현재 profile 이름 (high/low)
    void videoBufferingCountChanged();
    void itemProfileChanged();  // profile 설정정보
    void ptzPresetsChanged(); // 프리셋 정보 획득시
    void maxPtzPresetsChanged();
    // swing
    void swingTypeChanged();
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

    void streamTypeChanged();
    void mediaIdChanged();

    void requestVideoOutputUpdate(); // 연결된 VideoOutput update 요청
    void firstVideoFrame();
    void dewarpSetupResult(int errorCode);
    void talkErrorResult(int errorCode);

    void isSequenceChanged(int isSequence);

    void maskItemListModelChanged();
    void deviceUsingSetupChanged();
    void enableMaskingModeChanged();
    void maskPreviewOnChanged();
    void maskRectPositionChanged(QVariantMap maskRectMap);  // key:(int)maskId, value:(QRectF)maskRect
    void getCurrentBoxID();
    void maskingExportProgressModelChanged();
    void isTrackingChanged();
    void showTrackingFailMessage();

private:
    void setPosition(qint64 pos);
    void setSeekable(const bool seekable);
    void checkSurfaceFormat(const QVideoFrame &frame, const bool jpegColorSpace);

    WisenetMediaParam::StreamType streamTypeFromProfile(const WisenetMediaParam::StreamType streamType,
                                                        const ItemProfile profile);

    void present(const QVideoFrame &frame,
                 const qint64 playbackTimeMsec,
                 const bool jpegColorSpace);

    void resetFisheyeDewarp();
    void resetObjectTracker();

public slots:
    // 줌타겟의 영역 변경시 qml에서 호출
    void updateSurfaceViewPort(const QRectF normalizedRect);

    // 로컬 파일
    void setLocalResourceType(const QUrl localResource);

    // 타겟 소스
    void setRefSourceType(FrameSourceBase* mediaSource);

    // 카메라 라이브 / 플레이백 / 백업 / 포스 플레이백
    void setCameraType(const WisenetMediaParam::StreamType streamType,
                       const QString& serviceId,
                       const QString& deviceId,
                       const QString& channelId,
                       const QString& sessionId = "",
                       const QString& profileId = "",
                       const qint64 startTime = 0,
                       const qint64 endTime = 0,
                       const int trackId = -1,
                       const float speed = 1,
                       const bool paused = false,
                       const QString& posId = "",
                       const bool isPosPlayback = false);

    void connectToSource();
    void open();
    void seek(const qint64 playbackTimeMsec);
    void pause();
    void play();
    void step(const bool isForward);
    void close();

    /* slots for mediaPlayer from mediaControl, mediaSource, dewarp filter ==> */
    void onNewVideoFrame(const QVideoFrame &frame,
                         const qint64 playbackTimeMsec,
                         const bool jpegColorSpace);
    void onFilteredNewVideoFrame(const QVideoFrame &frame,
                                 const qint64 playbackTimeMsec,
                                 const bool jpegColorSpace);
    void onTrackedNewVideoFrame(const QVideoFrame &frame,
                                const qint64 playbackTimeMsec,
                                const bool jpegColorSpace,
                                const QPair<int, QRectF> resultPair);
    void onLastOriginalVideoImage(const QImage &image);
    void onDewarpSetupResult(int errorCode);
    void onNewAudioFrame(const AudioOutputFrameSharedPtr& audioFrame);
    void onNewMetaFrame(const MetaFrameSharedPtr& metaFrame);
    void onOpenResult(MediaControlBase::Error error);
    void onLocalRecordingTimeout();
    void onFisheyeDewarpStatusChanged();
    void onFisheyeParamChanged();
    void onFisheyeViewPositionChanged(const QVector<qreal> positions);
    void onMicStatusChanged();
    void onTalkResult(bool on, bool result, int);
    void onMicUnavilable();
    void onVideoSizeChanged();
    void onVideoOutSizeChanged();
    /* <== slots for mediaPlayer from mediaControl */

    void changeCameraStream(const WisenetMediaParam::StreamType streamType,
                            const QString& profileId = "",
                            const qint64 startTime = 0,
                            const qint64 endTime = 0,
                            const int trackId = -1,
                            const float speed = 1,
                            const bool paused = false);
    void checkDualStream();
    void setLocalRecording(const bool recordingStart, QString fileName);
    void copyWisenetPlayer();
    void setMicStatus(const bool enable);

    void saveSnapshot(QImage image);
    void imageToLocalFile(QImage image);
    void devicePtzContinuous(const int pan, const int tilt, const int zoom);
    void devicePtzStop();
    void devicePtzPresetGet();
    void devicePtzPresetMove(const int presetNumber);
    void devicePtzPresetDelete(const int presetNumber);
    void devicePtzPresetAdd(const int presetNumber, const QString &presetName);
    void devicePtzSwingGet();
    void devicePtzSwingMoveStop(const int actionType);
    void devicePtzGroupGet();
    void devicePtzGroupMove(const int groupNumber);
    void devicePtzGroupStop(const int groupNumber);
    void devicePtzTourGet();
    void devicePtzTourMove();
    void devicePtzTourStop();
    void devicePtzTraceGet();
    void devicePtzTraceMove(const int traceNumber);
    void devicePtzTraceStop(const int traceNumber);
    void deviceFocusContinuous(const int command); /* 0 : stop, 1: near, 2: far */
    void deviceFocusModeControl(const int focusMode); /* 0 : simpleFocus, 1: reset, 2: autoFocus */
    void checkPassword(QString password);
    void sleep(const bool isOn);
    void updatePlaybackStatus(const PlaybackState state, const float speed, const int trackId, const bool playWait);
    void updateOutputParam(const qreal screenPixelRatio, const int width, const int height);
    void setLocalFileFisheyeSettings(const bool enable,
                                     const QString lensType,
                                     const int lensLocation);
    void udpateFisheyeDewarpOutputParam(const int width, const int height);
    void dewarpPtzContinuous(const int viewIndex, const int pan, const int tilt, const int zoom);
    void dewarpPtzStop(const int viewIndex);
    void dewarpPtzZoom1x(const int viewIndex);
    void dewarpPtzPointMove(const int viewIndex, const qreal x, const qreal y);
    void dewarpPtzAreaMove(const int viewIndex, const qreal x, const qreal y, const qreal width, const qreal height);
    void dewarpPtzAbsZoom(const int viewIndex, const qreal zoomValue);
    void deviceTalk(const bool on);

    // Masking
    void addMask(const int maskId, const int maskType, const QVector<qreal> rectCoordinates);
    void removeMask(const int maskId, const int maskType);
    void updateMask(const int maskId, const int maskType, const QVector<qreal> rectCoordinates);
    void startTracking();
    void stopTracking();
    void startMaskingExport(const qint64 startTime, const qint64 endTime, const QString filePath, const QString fileName,
                            const bool usePassword, const QString password, const bool useDigitalSignature);
    void stopMaskingExport();
    void updateManualTrackingRect(const int maskId, const int maskType, const QVector<qreal> rectCoordinates);
    void onAddManualTrackingResult();
    void setMaskPaintType(int paintType);
    void setMaskInclude(bool include);
    void setMaskRGBColor(const QString& hexColor);    
    void setMaskMosaicCellSize(uchar size);
    void refreshMaskResult();

private:
    QAbstractVideoSurface *m_surface = nullptr;
    QVideoSurfaceFormat m_surfaceFormat;
    QPointer<MediaControlBase>  m_mediaControl;
    QPointer<FrameSourceBase>   m_mediaSource;
    QPointer<PosMediaControl>   m_posMediaControl; // 2022.10.12. added
    QPointer<FrameSourceBase>   m_posMediaSource;  // 2022.10.12. added
    WisenetMediaParam* m_mediaParam = nullptr;
    bool m_isRefSourceType = false;
    qint64 m_position = 0;
    qint64 m_positionSeconds = 0;
    bool m_seekable = false;
    int m_outWidth = 1920;
    int m_outHeight = 1080;
    qreal m_screenPixelRatio = 1.0f;
    QRectF m_viewPort = QRectF(0,0,1.0,1.0);

    WisenetMediaParam::StreamType m_lastCameraReqStreamType = WisenetMediaParam::StreamType::LiveAuto;
    ItemProfile m_itemProfile = ItemProfile::Auto;

    bool m_localRecordingStatus = false;
    QString m_localRecordingPath = "";
    bool m_enableAudioOut = false;
    bool m_keepOriginalSize = false;

    // 줌타겟인 경우 마지막 프레임을 저장 (영역 변경시 업데이트 용)
    // 그외 목적의 마지막 원본 영상은 framesource에서 가지고 있는다.
    QVideoFrame m_lastVideoFrame;
    bool m_isFirstFrame = true;

    // Immervision Dewarp instance
    bool m_enableFisheyeDewarp = false; /* 외부에서 내려온 enable/disable flag */
    int m_fisheyeViewMode = (int)MediaLayoutItemViewModel::V_Original;
    QVector<qreal>  m_fisheyeViewPositions;
    QScopedPointer<ImvDewarp> m_dewarpCam;
    QSize m_fisheyeOutputSize = QSize(0,0);

    bool m_micStatus = false; /* 외부에서 내려온 mic on/off flag */

    bool m_isSequence = false;
    bool m_deviceUsingSetup = false;

    // Masking & OpenCV object tracker instance
    QPointer<MaskItemListModel> m_maskItemListModel;
    bool m_enableMaskingMode = false; /* 외부에서 내려온 enable/disable flag */
    bool m_maskPreviewOn = false;

#ifdef Q_OS_WINDOWS
    QScopedPointer<CvAutoTracker> m_objectTracker;
#endif
    CvMaskPainter m_maskPainter;
    QScopedPointer<MaskingTranscoder> m_maskingTranscoder;
    QScopedPointer<MaskingExportProgressModel> m_maskingExportProgressModel;
    QString m_password = "";
    bool m_isTracking = false;
    bool m_isUpdatingManualRect = false;
    std::pair<int, NormalMaskRect> m_manualTrackingRect; // manualTracking 중일 때 manualRect 값을 저장하는 변수
    QTimer m_manualTrackingTimer;
    int m_currentTrackingID = -1;
    int m_currentMaskingType = (int)WisenetMaskType::MaskType::Auto;
};
