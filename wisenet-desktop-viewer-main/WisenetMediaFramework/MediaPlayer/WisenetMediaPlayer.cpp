/*********************************************************************************
 * Copyright(c) 2023 Hanwha Vision Co., Ltd.  All rights reserved.
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
#include "WisenetMediaPlayer.h"
#include "LogSettings.h"
#include "QCoreServiceManager.h"
#include "QLocaleManager.h"
#include "PosMetaManager.h"
#include "AudioOutputManager.h"
#include "AudioInputManager.h"
#include "ControllerManager.h"
#include "FFmpegMediaControl.h"
#include "WnmMediaControl.h"
#include "SecMediaControl.h"
#include "CoreServiceMediaControl.h"
#include "MediaLayoutViewModel.h"
#include "WisenetMediaParam.h"
#include "MediaThreadPool.h"
#include "OpenLayoutIItem.h"
#include "DragItemListModel.h"
#include "FileWriteManager.h"
#include <QQmlEngine>
#include <QPointer>
#include <QtDebug>
#include <QtConcurrent>
#ifdef Q_OS_WINDOWS
#include <mmsystem.h>
#pragma comment(lib, "winmm")
#endif

#undef SPDLOG_LOGGER_CALL
#define SPDLOG_LOGGER_CALL(logger, level, ...) (logger)->log(spdlog::source_loc{__FILE__, __LINE__, "[MediaPlayer] "}, level, __VA_ARGS__)


WisenetMediaPlayer::WisenetMediaPlayer(QObject *parent)
    : QObject(parent)
    , m_surfaceFormat(QSize(1,1), QVideoFrame::PixelFormat::Format_YUV420P)
{
    if (m_mediaParam == nullptr) {
        m_mediaParam = new WisenetMediaParam(this);
    }

    // init mask painter options for test
    m_maskPainter.setInclude(true);
    m_maskPainter.setPaintType(WisenetMaskType::MaskPaintType::Mosaic);
    m_maskPainter.setRGBColor(255, 0, 255);
    m_maskPainter.setMosaicCellSize(10);
    //m_maskPainter.setPixelRatio(1280.0f/1920.0f);
}

WisenetMediaPlayer::~WisenetMediaPlayer()
{
    if(!m_maskItemListModel.isNull())
    {
        qDebug() << "WisenetMediaPlayer::~WisenetMediaPlayer() clear m_maskItemListModel";
        m_maskItemListModel->clear();
    }
}

void WisenetMediaPlayer::registerQml()
{
#ifdef Q_OS_WINDOWS
    timeBeginPeriod(1);
#endif
    qmlRegisterType<MediaLayoutViewModel>("WisenetMediaFramework", 1, 0, "MediaLayoutViewModel");
    qmlRegisterType<MediaLayoutItemViewModel>("WisenetMediaFramework", 1, 0, "MediaLayoutItemViewModel");
    qmlRegisterType<WisenetMediaPlayer>("WisenetMediaFramework", 1, 0, "WisenetMediaPlayer");
    qmlRegisterType<WisenetMediaParam>("WisenetMediaFramework", 1, 0, "WisenetMediaParam");
    qmlRegisterType<DragItemListModel>("WisenetMediaFramework", 1, 0, "DragItemListModel");
    qmlRegisterType<PtzPresetItemModel>("WisenetMediaFramework", 1, 0, "PtzPresetItemModel");
    qmlRegisterType<PosMetaManager>("WisenetMediaFramework", 1, 0, "PosMetaManager");
    qmlRegisterType<AudioOutputManager>("WisenetMediaFramework", 1, 0, "AudioOutputManager");
    qmlRegisterType<AudioOutputManager>("WisenetMediaFramework", 1, 0, "AudioInputManager");
    qmlRegisterType<ControllerManager>("WisenetMediaFramework", 1, 0, "ControllerManager");

    qRegisterMetaType<MediaLayoutItemViewModel::ItemType>("MediaLayoutItemViewModel::ItemType");
    qRegisterMetaType<MediaLayoutItemViewModel::ItemFillMode>("MediaLayoutItemViewModel::ItemFillMode");
    qRegisterMetaType<MediaLayoutItemViewModel::ItemProfile>("MediaLayoutItemViewModel::ItemProfile");
    qRegisterMetaType<MediaLayoutItemViewModel::FisheyeViewMode>("MediaLayoutItemViewModel::FisheyeViewMode");
    qRegisterMetaType<MediaLayoutItemViewModel::FisheyeLensLocation>("MediaLayoutItemViewModel::FisheyeLensLocation");

    qRegisterMetaType<WisenetMediaParam::SourceType>("WisenetMediaParam::SourceType");
    qRegisterMetaType<WisenetMediaParam::StreamType>("WisenetMediaParam::StreamType");

    qRegisterMetaType<MediaControlBase::Error>("MediaControlBase::Error");
    qRegisterMetaType<WisenetMediaPlayer::Error>("WisenetMediaPlayer::Error");

    qRegisterMetaType<WisenetMediaPlayer::PlaybackState>("WisenetMediaPlayer::PlaybackState");
    qRegisterMetaType<WisenetMediaPlayer::PlayStatus>("WisenetMediaPlayer::PlayStatus");

    qRegisterMetaType<AudioOutputFrameSharedPtr>();
    qRegisterMetaType<OpenLayoutItem>();

    qmlRegisterType<ControllerModel>("WisenetMediaFramework", 1, 0, "ControllerModel");
    typedef ControllerModel::ControllerMode ControllerMode;
    qRegisterMetaType<ControllerMode>("ControllerMode");

    qmlRegisterType<ControllerSpc2000>("WisenetMediaFramework", 1, 0, "ControllerSpc2000");
    typedef ControllerSpc2000::ControllerButtonAction ControllerButtonAction;
    qRegisterMetaType<ControllerButtonAction>("ControllerButtonAction");

    MediaThreadPool::Initialize();
    AudioOutputManager::Initialize();
    AudioInputManager::Initialize();
    PosMetaManager::Initialize();
}

QAbstractVideoSurface *WisenetMediaPlayer::videoSurface() const
{
    return m_surface;
}

void WisenetMediaPlayer::setVideoSurface(QAbstractVideoSurface *videoSurface)
{
    SPDLOG_DEBUG("setVideoSurface()");
    if (m_surface && m_surface != videoSurface  && m_surface->isActive()) {
        SPDLOG_DEBUG("setVideoSurface(), stop surface");
        m_surface->stop();
    }

    m_surface = videoSurface;
}

/**
 * @brief WisenetMediaPlayer::onNewVideoFrame
 * FrameSource로부터 비디오 프레임을 받는 슬롯
 * @param frame
 * @param playbackTimeMsec
 * @param jpegColorSpace
 */
void WisenetMediaPlayer::onNewVideoFrame(const QVideoFrame &frame,
                                         const qint64 playbackTimeMsec,
                                         const bool jpegColorSpace)
{
    // Dewarping 중인 경우
    if (m_dewarpCam) {
        // original view가 아닌 경우에는 dewarping 영상을 최종 영상으로 전달한다.
        if (m_isRefSourceType || m_fisheyeViewMode != MediaLayoutItemViewModel::V_Original) {
            m_dewarpCam->onNewVideoFrame(frame, playbackTimeMsec, jpegColorSpace);
            return;
        }
        // original view인 경우에는 마지막 영상만 keep한다.
        m_dewarpCam->onLastVideoFrame(frame, playbackTimeMsec, jpegColorSpace);
    }
#ifdef Q_OS_WINDOWS
    else if(m_enableMaskingMode) {
        if(!m_objectTracker) {
            // create Object tracker
            resetObjectTracker();
        }
        if(m_objectTracker) {
            // send frame to object tracker
            m_objectTracker->onNewVideoFrame(frame, playbackTimeMsec, jpegColorSpace);
            return;
        }
    }
#endif

    present(frame, playbackTimeMsec, jpegColorSpace);
}

/**
 * @brief WisenetMediaPlayer::onFilteredNewVideoFrame
 * ImvDewarp로부터 Fisheye Dewarping 영상을 받는 슬롯
 * @param frame
 * @param playbackTimeMsec
 * @param jpegColorSpace
 */
void WisenetMediaPlayer::onFilteredNewVideoFrame(const QVideoFrame &frame,
                                                 const qint64 playbackTimeMsec,
                                                 const bool jpegColorSpace)
{
    present(frame, playbackTimeMsec, jpegColorSpace);
}

/**
 * @brief WisenetMediaPlayer::onTrackedNewVideoFrame
 * Slot receive video and mask data from Object Tracker
 * @param frame : same with original frame
 * @param playbackTimeMsec
 * @param jpegColorSpace
 * @param resultMap : tracking result mask rectangle
 */
void WisenetMediaPlayer::onTrackedNewVideoFrame(const QVideoFrame &frame,
                                                const qint64 playbackTimeMsec,
                                                const bool jpegColorSpace,
                                                const QPair<int, QRectF> resultPair)
{
    NormalMaskRect maskRect;
    QVariantMap maskRectMap = m_maskItemListModel->getMaskRectMap(playbackTimeMsec, m_isTracking);    // key:(int)maskId, value:(QRectF)maskRect
    //SPDLOG_DEBUG("WisenetMediaPlayer::onTrackedNewVideoFrame getMaskRectMap count : {},", maskRectMap.count());

    if(m_isTracking && m_currentMaskingType == (int)WisenetMaskType::MaskType::Auto) {
        if(resultPair.first >= 0) { // 트래킹 성공
            // SPDLOG_DEBUG("WisenetMediaPlayer::onTrackedNewVideoFrame NEW TRACKED RESULT {} {} {} {}", resultPair.second.x(),resultPair.second.y(), resultPair.second.width(), resultPair.second.height());
            maskRect = NormalMaskRect(resultPair.second.x(),resultPair.second.y(), resultPair.second.width(), resultPair.second.height());
            m_maskItemListModel->AddTempMaskIndicator(MaskIndicator(maskRect, playbackTimeMsec));
            maskRectMap.insert(QString::number(resultPair.first), QRectF(resultPair.second.x(), resultPair.second.y(), resultPair.second.width(), resultPair.second.height()));
        }
        else { // 트래킹 실패
            SPDLOG_DEBUG("WisenetMediaPlayer::onTrackedNewVideoFrame Tracking FAILED");
            stopTracking();
            pause();
            emit showTrackingFailMessage();
        }
    }


    // send maskRectPositionChanged signal
    emit maskRectPositionChanged(maskRectMap);

#ifdef Q_OS_WINDOWS
    // paint mask data on video frame before present
    if(maskPreviewOn()) {
        QList<NormalMaskRect> maskRectList = m_maskItemListModel->getMaskRectList(playbackTimeMsec, m_isTracking);
        m_maskPainter.paint(frame, maskRectList);
    }
#endif
    // present masked frame
    present(frame, playbackTimeMsec, jpegColorSpace);
}

/**
 * @brief WisenetMediaPlayer::onLastOriginalVideoImage
 * FrameSource의 마지막 VideoFrame의 원본사이즈 Image를 받는 슬롯
 * @param image
 */
void WisenetMediaPlayer::onLastOriginalVideoImage(const QImage &image)
{
    imageToLocalFile(image);    // 이미지 파일로 저장 (캡쳐)
}

void WisenetMediaPlayer::onDewarpSetupResult(int errorCode)
{
    // 일반 카메라에 Fisheye 설정 시 E_ERR_NOTPANOMORPH가 계속 발생하여 로그 남기지 않음
    if(errorCode != 10)
        SPDLOG_DEBUG("onDewarpSetupResult(), errorCode={}", errorCode);
    emit dewarpSetupResult(errorCode);
}



void WisenetMediaPlayer::present(const QVideoFrame &frame,
                                 const qint64 playbackTimeMsec,
                                 const bool jpegColorSpace)
{
    if (m_surface) {
        setPosition(playbackTimeMsec);
        checkSurfaceFormat(frame, jpegColorSpace);
        m_surface->present(frame);

        if (m_isRefSourceType || m_fisheyeViewMode != MediaLayoutItemViewModel::V_Original) {
            m_lastVideoFrame = frame;
            if(m_fisheyeViewMode != MediaLayoutItemViewModel::V_Original
                    && !m_lastVideoFrame.image().isNull()
                    && m_lastVideoFrame.image().size() != m_fisheyeOutputSize) {
                // Fisheye Dewarp 영상의 OutputSize를 VideoOutSize 사용하도록 수정
                m_fisheyeOutputSize = m_lastVideoFrame.image().size();
                emit videoOutSizeChanged();
            }
        }
        if (m_isFirstFrame) {
            m_isFirstFrame = false;
            emit firstVideoFrame();
        }
    }
}


/**
 * @brief WisenetMediaPlayer::onNewAudioFrame
 * 오디오 프레임을 받는 슬롯
 * @param audioFrame
 */
void WisenetMediaPlayer::onNewAudioFrame(const AudioOutputFrameSharedPtr &audioFrame)
{
    if (m_enableAudioOut && m_mediaSource && !m_isSequence) {
        AudioOutputManager::Instance()->onNewAudioFrame(audioFrame);
    }
}

/**
 * @brief WisenetMediaPlayer::onNewMetaFrame
 * 메타 프레임을 받는 슬롯
 * @param metaFrame
 */
void WisenetMediaPlayer::onNewMetaFrame(const MetaFrameSharedPtr& metaFrame)
{
    if (metaFrame == nullptr || PosMetaManager::Instance() == nullptr) {
        return;
    }

    if (metaFrame->dataSize() > 0) {
        PosMetaManager::Instance()->onNewMetaFrame(metaFrame);
    }
}

void WisenetMediaPlayer::checkSurfaceFormat(const QVideoFrame &frame,
                                            const bool jpegColorSpace)
{
    QVideoSurfaceFormat::YCbCrColorSpace yCbCrColorSpace =
            jpegColorSpace ? QVideoSurfaceFormat::YCbCr_JPEG : QVideoSurfaceFormat::YCbCr_Undefined;

    //testing
    //QVideoSurfaceFormat::YCbCrColorSpace yCbCrColorSpace =QVideoSurfaceFormat::YCbCr_Undefined;
    //qDebug() << "colorSpace=" << m_surfaceFormat.yCbCrColorSpace();
    if (!m_surface->isActive() ||
            m_surfaceFormat.frameWidth() != frame.width() ||
            m_surfaceFormat.frameHeight() != frame.height() ||
            m_surfaceFormat.pixelFormat() != frame.pixelFormat() ||
            m_surfaceFormat.yCbCrColorSpace() != yCbCrColorSpace) {
        SPDLOG_DEBUG("checkSurfaceFormat() change surface format!!, {}x{},{}, isRef={}",
                     frame.width(), frame.height(), frame.pixelFormat(), m_isRefSourceType);

        // stop surface
        if (m_surface->isActive()) {
            SPDLOG_DEBUG("STOP SURFACE");
            m_surface->stop();
        }

        QSize videoSize(frame.width(), frame.height());
        m_surfaceFormat = QVideoSurfaceFormat(videoSize, frame.pixelFormat());
        if (yCbCrColorSpace != QVideoSurfaceFormat::YCbCr_Undefined) {
            m_surfaceFormat.setYCbCrColorSpace(yCbCrColorSpace);
        }

        if (m_isRefSourceType && !m_dewarpCam) {
            QRect vp;
            vp.setX(m_viewPort.x()*frame.width());
            vp.setY(m_viewPort.y()*frame.height());
            vp.setWidth(m_viewPort.width()*frame.width());
            vp.setHeight(m_viewPort.height()*frame.height());
            //            SPDLOG_DEBUG("checkSurfaceFormat() setViewPort() called, x={}, y={}, width ={}, height={}",
            //                         vp.x(), vp.y(), vp.width(), vp.height());
            m_surfaceFormat.setViewport(vp);
        }

        // restart surface
        m_surface->start(m_surfaceFormat);
        //SPDLOG_DEBUG("RESTART SURFACE={}", success);
    }
}

/**
 * @brief WisenetMediaPlayer::updateSurfaceViewPort
 * 줌타겟인 경우, 뷰포트를 변경한다.
 * @param normalizedRect
 */
void WisenetMediaPlayer::updateSurfaceViewPort(const QRectF normalizedRect)
{
    if (m_isRefSourceType) {
        setViewPort(normalizedRect);
        if (m_dewarpCam) {
            return;
        }

        if (m_surface && m_surface->isActive() && m_lastVideoFrame.isValid()) {

            m_surface->stop();

            QRect vp;
            vp.setX(m_viewPort.x()*m_lastVideoFrame.width());
            vp.setY(m_viewPort.y()*m_lastVideoFrame.height());
            vp.setWidth(m_viewPort.width()*m_lastVideoFrame.width());
            vp.setHeight(m_viewPort.height()*m_lastVideoFrame.height());
            m_surfaceFormat.setViewport(vp);
            m_surface->start(m_surfaceFormat);

            m_surface->present(m_lastVideoFrame);

            emit requestVideoOutputUpdate();
        }
    }
}

/**
 * @brief WisenetMediaPlayer::setRefSourceType
 * 줌타겟 아이템 초기화
 * @param source
 */
void WisenetMediaPlayer::setRefSourceType(FrameSourceBase* source)
{
    qDebug() << "setRefSourceType()" << source;
    if (m_mediaControl != nullptr) {
        SPDLOG_WARN("setRefSourceType() ignored since already has it's own source!!");
        return;
    }

    if (m_mediaSource != source) {
        m_mediaParam->setSourceType(WisenetMediaParam::RefSource);
        m_isRefSourceType = true;

        // 이전 연결을 끊고 새로운 media source와 연결을 맺는다.(TODO: 이럴 필요까지?)
        if (m_mediaSource != nullptr) {
            disconnect(m_mediaSource, nullptr, this, nullptr);
        }

        m_mediaSource = source;
        if (m_mediaSource != nullptr) {
            connect(m_mediaSource, &FrameSourceBase::destroyed, this, [=] (){
                SPDLOG_DEBUG("setRefSourceType():: destroyed refSource target!!");
                m_mediaSource = nullptr;
            });

            connectToSource();
            resetFisheyeDewarp();
        }
        emit mediaSourceChanged();
        emit mediaNameChanged();
        emit statusChanged();
        emit streamTypeChanged();

        if (m_mediaSource != nullptr) {
            // Pause상태에서 RefSourceType지정시에는 빈영상이 표시되는 문제를 해결
            if (m_mediaSource->playbackState() == PlaybackState::PausedState) {
                m_mediaSource->requestLastVideoFrame();
            }
        }
    }
}


FrameSourceBase *WisenetMediaPlayer::mediaSource() const
{
    return m_mediaSource;
}

void WisenetMediaPlayer::setLocalResourceType(const QUrl localResource)
{
    m_mediaParam->setSourceType(WisenetMediaParam::LocalResource);
    m_mediaParam->setStreamType(WisenetMediaParam::PlaybackHigh);
    m_mediaParam->setLocalResource(localResource);
    setSeekable(m_mediaParam->seekable());
}

void WisenetMediaPlayer::setCameraType(const WisenetMediaParam::StreamType streamType,
                                       const QString& serviceId,
                                       const QString& deviceId,
                                       const QString& channelId,
                                       const QString& sessionId,
                                       const QString& profileId,
                                       const qint64 startTime,
                                       const qint64 endTime,
                                       const int trackId,
                                       const float speed,
                                       const bool paused,
                                       const QString& posId,
                                       const bool isPosPlayback)
{
    m_lastCameraReqStreamType = streamType;

    auto _streamType = streamTypeFromProfile(streamType, m_itemProfile);
    m_mediaParam->setSourceType(WisenetMediaParam::Camera);
    m_mediaParam->setStreamType(_streamType);
    m_mediaParam->setServiceId(serviceId);
    m_mediaParam->setDeviceId(deviceId);
    m_mediaParam->setChannelId(channelId);
    m_mediaParam->setSessionId(sessionId);
    m_mediaParam->setProfileId(profileId);
    m_mediaParam->setStartTime(startTime);
    m_mediaParam->setEndTime(endTime);
    m_mediaParam->setTrackId(trackId);
    m_mediaParam->setSpeed(speed);
    m_mediaParam->setPausedOpen(paused);
    m_mediaParam->setPosId(posId);

    if (streamType == WisenetMediaParam::StreamType::PlaybackAuto)
    {
        if (isPosPlayback) {
            m_mediaParam->setPosPlayback(true);
        }
        else
            m_mediaParam->setPosPlayback(false);
    }

    setSeekable(m_mediaParam->seekable());
}

void WisenetMediaPlayer::connectToSource()
{
    connect(m_mediaSource, &FrameSourceBase::newVideoFrame, this, &WisenetMediaPlayer::onNewVideoFrame, Qt::QueuedConnection);
    connect(m_mediaSource, &FrameSourceBase::newAudioFrame, this, &WisenetMediaPlayer::onNewAudioFrame, Qt::QueuedConnection);
    connect(m_mediaSource, &FrameSourceBase::lastOriginalVideoImage, this, &WisenetMediaPlayer::onLastOriginalVideoImage, Qt::QueuedConnection);
    connect(m_mediaSource, &FrameSourceBase::statusChanged, this, &WisenetMediaPlayer::statusChanged, Qt::QueuedConnection);

    connect(m_mediaSource, &FrameSourceBase::mediaNameChanged, this, &WisenetMediaPlayer::mediaNameChanged, Qt::QueuedConnection);
    connect(m_mediaSource, &FrameSourceBase::supportVideoInChanged, this, &WisenetMediaPlayer::supportVideoInChanged, Qt::QueuedConnection);
    connect(m_mediaSource, &FrameSourceBase::supportAudioInChanged, this, &WisenetMediaPlayer::supportAudioInChanged, Qt::QueuedConnection);
    connect(m_mediaSource, &FrameSourceBase::supportAudioOutChanged, this, &WisenetMediaPlayer::supportAudioOutChanged, Qt::QueuedConnection);
    connect(m_mediaSource, &FrameSourceBase::supportDualStreamLiveChanged, this, &WisenetMediaPlayer::supportDualStreamLiveChanged, Qt::QueuedConnection);
    connect(m_mediaSource, &FrameSourceBase::supportDualStreamPlaybackChanged, this, &WisenetMediaPlayer::supportDualStreamPlaybackChanged, Qt::QueuedConnection);
    connect(m_mediaSource, &FrameSourceBase::supportPanTiltChanged, this, &WisenetMediaPlayer::supportPanTiltChanged, Qt::QueuedConnection);
    connect(m_mediaSource, &FrameSourceBase::supportZoomChanged, this, &WisenetMediaPlayer::supportZoomChanged, Qt::QueuedConnection);
    connect(m_mediaSource, &FrameSourceBase::supportZoom1xChanged, this, &WisenetMediaPlayer::supportZoom1xChanged, Qt::QueuedConnection);
    connect(m_mediaSource, &FrameSourceBase::supportAreaZoomChanged, this, &WisenetMediaPlayer::supportAreaZoomChanged, Qt::QueuedConnection);
    connect(m_mediaSource, &FrameSourceBase::supportPtzHomeChanged, this, &WisenetMediaPlayer::supportPtzHomeChanged, Qt::QueuedConnection);
    connect(m_mediaSource, &FrameSourceBase::supportPresetChanged, this, &WisenetMediaPlayer::supportPresetChanged, Qt::QueuedConnection);
    connect(m_mediaSource, &FrameSourceBase::supportSwingChanged, this, &WisenetMediaPlayer::supportSwingChanged, Qt::QueuedConnection);
    connect(m_mediaSource, &FrameSourceBase::supportGroupChanged, this, &WisenetMediaPlayer::supportGroupChanged, Qt::QueuedConnection);
    connect(m_mediaSource, &FrameSourceBase::supportTourChanged, this, &WisenetMediaPlayer::supportTourChanged, Qt::QueuedConnection);
    connect(m_mediaSource, &FrameSourceBase::supportTraceChanged, this, &WisenetMediaPlayer::supportTraceChanged, Qt::QueuedConnection);
    connect(m_mediaSource, &FrameSourceBase::supportDefogChanged, this, &WisenetMediaPlayer::supportDefogChanged, Qt::QueuedConnection);
    connect(m_mediaSource, &FrameSourceBase::supportSimpleFocusChanged, this, &WisenetMediaPlayer::supportSimpleFocusChanged, Qt::QueuedConnection);
    connect(m_mediaSource, &FrameSourceBase::supportAutoFocusChanged, this, &WisenetMediaPlayer::supportAutoFocusChanged, Qt::QueuedConnection);
    connect(m_mediaSource, &FrameSourceBase::supportResetFocusChanged, this, &WisenetMediaPlayer::supportResetFocusChanged, Qt::QueuedConnection);
    connect(m_mediaSource, &FrameSourceBase::supportFocusChanged, this, &WisenetMediaPlayer::supportFocusChanged, Qt::QueuedConnection);
    connect(m_mediaSource, &FrameSourceBase::supportIrisChanged, this, &WisenetMediaPlayer::supportIrisChanged, Qt::QueuedConnection);
    connect(m_mediaSource, &FrameSourceBase::supportDurationChanged, this, &WisenetMediaPlayer::supportDurationChanged, Qt::QueuedConnection);
    connect(m_mediaSource, &FrameSourceBase::supportTimelineChanged, this, &WisenetMediaPlayer::supportTimelineChanged, Qt::QueuedConnection);
    connect(m_mediaSource, &FrameSourceBase::supportSmartSearchChanged, this, &WisenetMediaPlayer::supportSmartSearchChanged, Qt::QueuedConnection);
    connect(m_mediaSource, &FrameSourceBase::supportSmartSearchFilterChanged, this, &WisenetMediaPlayer::supportSmartSearchFilterChanged, Qt::QueuedConnection);

    connect(m_mediaSource, &FrameSourceBase::durationChanged, this, &WisenetMediaPlayer::durationChanged, Qt::QueuedConnection);
    connect(m_mediaSource, &FrameSourceBase::ptzPresetsChanged, this, &WisenetMediaPlayer::ptzPresetsChanged, Qt::QueuedConnection);
    connect(m_mediaSource, &FrameSourceBase::maxPtzPresetsChanged, this, &WisenetMediaPlayer::maxPtzPresetsChanged, Qt::QueuedConnection);

    connect(m_mediaSource, &FrameSourceBase::swingPanStartPresetChanged, this, &WisenetMediaPlayer::swingPanStartPresetChanged, Qt::QueuedConnection);
    connect(m_mediaSource, &FrameSourceBase::groupPresetsChanged, this, &WisenetMediaPlayer::groupPresetsChanged, Qt::QueuedConnection);
    connect(m_mediaSource, &FrameSourceBase::tourPresetsChanged, this, &WisenetMediaPlayer::tourPresetsChanged, Qt::QueuedConnection);
    connect(m_mediaSource, &FrameSourceBase::tracePresetsChanged, this, &WisenetMediaPlayer::tracePresetsChanged, Qt::QueuedConnection);

    connect(m_mediaSource, &FrameSourceBase::enablePtzChanged, this, &WisenetMediaPlayer::enablePtzChanged, Qt::QueuedConnection);
    connect(m_mediaSource, &FrameSourceBase::speedChanged, this, &WisenetMediaPlayer::speedChanged, Qt::QueuedConnection);
    connect(m_mediaSource, &FrameSourceBase::playbackStateChanged, this, &WisenetMediaPlayer::playbackStateChanged, Qt::QueuedConnection);

    connect(m_mediaSource, &FrameSourceBase::videoSizeChanged, this, &WisenetMediaPlayer::onVideoSizeChanged, Qt::QueuedConnection);
    connect(m_mediaSource, &FrameSourceBase::videoOutSizeChanged, this, &WisenetMediaPlayer::onVideoOutSizeChanged, Qt::QueuedConnection);
    connect(m_mediaSource, &FrameSourceBase::videoCodecChanged, this, &WisenetMediaPlayer::videoCodecChanged, Qt::QueuedConnection);
    connect(m_mediaSource, &FrameSourceBase::audioCodecChanged, this, &WisenetMediaPlayer::audioCodecChanged, Qt::QueuedConnection);
    connect(m_mediaSource, &FrameSourceBase::videoKbpsChanged, this, &WisenetMediaPlayer::videoKbpsChanged, Qt::QueuedConnection);
    connect(m_mediaSource, &FrameSourceBase::videoFpsChanged, this, &WisenetMediaPlayer::videoFpsChanged, Qt::QueuedConnection);
    connect(m_mediaSource, &FrameSourceBase::videoProfileChanged, this, &WisenetMediaPlayer::videoProfileChanged, Qt::QueuedConnection);
    connect(m_mediaSource, &FrameSourceBase::videoBufferingCountChanged, this, &WisenetMediaPlayer::videoBufferingCountChanged, Qt::QueuedConnection);
    connect(m_mediaSource, &FrameSourceBase::streamTypeChanged, this, &WisenetMediaPlayer::streamTypeChanged, Qt::QueuedConnection);

    /* user permission */
    connect(m_mediaSource, &FrameSourceBase::permissionPlaybackChanged, this, &WisenetMediaPlayer::permissionPlaybackChanged, Qt::QueuedConnection);
    connect(m_mediaSource, &FrameSourceBase::permissionLocalRecordingChanged, this, &WisenetMediaPlayer::permissionLocalRecordingChanged, Qt::QueuedConnection);
    connect(m_mediaSource, &FrameSourceBase::permissionPtzControlChanged, this, &WisenetMediaPlayer::permissionPtzControlChanged, Qt::QueuedConnection);
    connect(m_mediaSource, &FrameSourceBase::permissionAudioInChanged, this, &WisenetMediaPlayer::permissionAudioInChanged, Qt::QueuedConnection);
    connect(m_mediaSource, &FrameSourceBase::permissionMicChanged, this, &WisenetMediaPlayer::permissionMicChanged, Qt::QueuedConnection);

    /* fisheye dewarp */
    connect(m_mediaSource, &FrameSourceBase::fisheyeDewarpStatusChanged, this, &WisenetMediaPlayer::onFisheyeDewarpStatusChanged, Qt::QueuedConnection);
    connect(m_mediaSource, &FrameSourceBase::fisheyeParamChanged, this, &WisenetMediaPlayer::onFisheyeParamChanged, Qt::QueuedConnection);
    connect(m_mediaSource, &FrameSourceBase::supportFisheyeDewarpChanged, this, &WisenetMediaPlayer::supportFisheyeDewarpChanged, Qt::QueuedConnection);
    connect(m_mediaSource, &FrameSourceBase::fisheyeLocationChanged, this, &WisenetMediaPlayer::fisheyeLocationChanged, Qt::QueuedConnection);
    connect(m_mediaSource, &FrameSourceBase::fisheyeLensTypeChanged, this, &WisenetMediaPlayer::fisheyeLensTypeChanged, Qt::QueuedConnection);

    connect(m_mediaSource, &FrameSourceBase::supportMicChanged, this, &WisenetMediaPlayer::supportMicChanged, Qt::QueuedConnection);
    connect(m_mediaSource, &FrameSourceBase::micStatusChanged, this, &WisenetMediaPlayer::onMicStatusChanged, Qt::QueuedConnection);
    connect(m_mediaSource, &CoreServiceFrameSource::deviceTalkResult, this, &WisenetMediaPlayer::onTalkResult, Qt::QueuedConnection);

    connect(m_mediaSource, &FrameSourceBase::deviceUsingSetupChanged, this, &WisenetMediaPlayer::deviceUsingSetupChanged, Qt::QueuedConnection);
}


void WisenetMediaPlayer::open()
{
    if (m_mediaSource == nullptr && m_mediaControl == nullptr) {
        // 미디어 소스 생성
        auto sourceType = m_mediaParam->sourceType();
        SPDLOG_DEBUG("open(), create new media source, type={}, deviceId={}, channelId={}, outRes={}x{}, screenRatio={}",
                     sourceType, m_mediaParam->deviceId().toStdString(),
                     m_mediaParam->channelId().toStdString(),
                     m_outWidth, m_outHeight, m_screenPixelRatio);

        if (sourceType == WisenetMediaParam::SourceType::Camera) {
            m_mediaControl = new CoreServiceMediaControl(this, m_isSequence);
            connect(m_mediaControl, &MediaControlBase::localRecordingTimeout, this, &WisenetMediaPlayer::onLocalRecordingTimeout, Qt::QueuedConnection);
        }
        else if (sourceType  == WisenetMediaParam::SourceType::LocalResource) {
            QString filePath = m_mediaParam->localResource().toLocalFile();
            QFileInfo fileInfo(filePath);
            QString fileSuffix = fileInfo.suffix();
            if(fileSuffix.toLower() == "wnm")
                m_mediaControl = new WnmMediaControl(this);
            else if(fileSuffix.toLower() == "sec")
                m_mediaControl = new SecMediaControl(this);
            else
                m_mediaControl = new FFmpegMediaControl(this, m_isSequence);
        }

        if(m_mediaControl == nullptr)
            return;

        m_mediaSource = m_mediaControl->source();
        m_mediaSource->setEnableAudioOut(m_enableAudioOut);
        if(m_enableMaskingMode) {
            m_mediaSource->setOutPixelFormat(VideoOutPixelFormat::YUV420P); // fix out pixel format to YUV420P
            m_mediaSource->setMaximumVideoSize(1280);   // fix maximum out video width or height to 1280
            m_mediaSource->setKeepLastVideoOutput(true);    // keep last video output
            m_mediaParam->setPausedOpen(true);
        }

        connectToSource();
        connect(m_mediaControl, &MediaControlBase::openResult, this, &WisenetMediaPlayer::onOpenResult, Qt::QueuedConnection);
        connect(m_mediaControl, &MediaControlBase::mediaIdChanged, this, &WisenetMediaPlayer::mediaIdChanged, Qt::QueuedConnection);

        m_mediaControl->setMediaParam(m_mediaParam);
        m_mediaControl->updateOutputParam(m_screenPixelRatio, m_outWidth, m_outHeight);
        m_mediaControl->setKeepOriginalSize(m_keepOriginalSize);
        m_mediaControl->open();

        emit mediaSourceChanged();
    }
    else {
        SPDLOG_DEBUG("open(), it would be refsource, do nothing!");
    }

    // 2022.10.12. added
    if (m_mediaParam->isPosPlayback()) {
        if (m_posMediaSource == nullptr && m_posMediaControl == nullptr) {
            // (포스) 미디어 소스 생성
            auto sourceType = m_mediaParam->sourceType();
            SPDLOG_INFO("pos open(), create new media source, type={}, deviceId={}, posId={}, outRes={}x{}, screenRatio={}",
                        sourceType, m_mediaParam->deviceId().toStdString(),
                        m_mediaParam->posId().toStdString(),
                        m_outWidth, m_outHeight, m_screenPixelRatio);

            m_posMediaControl = new PosMediaControl(this);

            if (m_posMediaControl != nullptr) {
                m_posMediaSource = m_posMediaControl->source();
            }

            connect(m_posMediaControl, &PosMediaControl::newMetaFrame, this, &WisenetMediaPlayer::onNewMetaFrame, Qt::QueuedConnection);

            m_posMediaControl->setMediaParam(m_mediaParam);
            m_posMediaControl->updateOutputParam(m_screenPixelRatio, m_outWidth, m_outHeight);
            m_posMediaControl->setKeepOriginalSize(m_keepOriginalSize);
            m_posMediaControl->open();
        }
    }
}

void WisenetMediaPlayer::onOpenResult(MediaControlBase::Error error)
{
    SPDLOG_DEBUG("onOpenResult(), error={}", error);
    emit openResult(error);
}


void WisenetMediaPlayer::close()
{
    if (m_mediaControl) {
        SPDLOG_DEBUG("close(), deviceId={}, channelId={}", m_mediaParam->deviceId().toStdString(), m_mediaParam->channelId().toStdString());
        m_mediaControl->close();
        delete m_mediaControl;
        m_mediaControl = nullptr;
        m_mediaSource = nullptr;

        emit statusChanged();
        emit closed();
        if (m_surface) {
            m_surface->stop();
        }
    }
    else {
        SPDLOG_DEBUG("close(), no mediaControl");
    }

    // 2022.10.12. added
    if (m_posMediaControl) {
        SPDLOG_INFO("pos media close(), deviceId={}, posId={}", m_mediaParam->deviceId().toStdString(), m_mediaParam->posId().toStdString());
        m_posMediaControl->close();
        delete m_posMediaControl;
        m_posMediaControl = nullptr;
        m_posMediaSource = nullptr;

        emit statusChanged();
        emit closed();
        if (m_surface) {
            m_surface->stop();
        }
    }
}


WisenetMediaParam::StreamType WisenetMediaPlayer::streamTypeFromProfile(const WisenetMediaParam::StreamType streamType,
                                                                        const ItemProfile profile)
{
    // streamtype이 auto인 경우, profile 타입이 auto가 아닌지 체크해서 변환을 한번 해준다.
    if (streamType == WisenetMediaParam::StreamType::LiveAuto) {
        if (profile == ItemProfile::High) {
            return WisenetMediaParam::StreamType::LiveHigh;
        }
        else if (profile == ItemProfile::Low) {
            return WisenetMediaParam::StreamType::LiveLow;
        }
    }
    else if (streamType == WisenetMediaParam::StreamType::PlaybackAuto) {
        if (profile == ItemProfile::High) {
            return WisenetMediaParam::StreamType::PlaybackHigh;
        }
        else if (profile == ItemProfile::Low) {
            return WisenetMediaParam::StreamType::PlaybackLow;
        }
    }
    return streamType;
}


void WisenetMediaPlayer::changeCameraStream(
        const WisenetMediaParam::StreamType streamType,
        const QString &profileId,
        const qint64 startTime,
        const qint64 endTime,
        const int trackId,
        const float speed,
        const bool paused)
{
    SPDLOG_DEBUG("changeCameraStream(), streamType={}, before={}", streamType, m_mediaParam->streamType());

    if (m_mediaControl == nullptr) {
        SPDLOG_INFO("changeCameraStream(), invalid media control");
        return;
    }

    m_lastCameraReqStreamType = streamType;
    auto _streamType = streamTypeFromProfile(streamType, m_itemProfile);

    if (_streamType != m_mediaParam->streamType() || trackId != m_mediaParam->trackId()) {
        m_mediaParam->setSourceType(WisenetMediaParam::Camera);
        m_mediaParam->setStreamType(_streamType);
        m_mediaParam->setProfileId(profileId);
        m_mediaParam->setStartTime(startTime);
        m_mediaParam->setEndTime(endTime);
        m_mediaParam->setTrackId(trackId);

        /*
        // track id가 바뀐 경우에는 speed를 1배속으로 변경한다. (paused open이 어렵다.)
        if (trackId != m_mediaParam->trackId()) {
            m_mediaParam->setSpeed(1.0);
        }
        */
        // 초기 speed와 paused 상태를 입력받도록 수정
        m_mediaParam->setSpeed(speed);
        m_mediaParam->setPausedOpen(paused);

        if (m_mediaControl != nullptr) {
            m_mediaControl->switchingOpen(m_mediaParam);
        }
        setSeekable(m_mediaParam->seekable());
    }
    else {
        SPDLOG_INFO("same stream type change request, do nothing!, {}/{},{}",
                    streamType, _streamType, m_mediaParam->streamType());
    }
}

/**
 * @brief WisenetMediaPlayer::checkDualStream
 * High/Low 프로파일 체크
 */
void WisenetMediaPlayer::checkDualStream()
{
    if (m_mediaControl != nullptr) {
        if (m_mediaParam->sourceType() != WisenetMediaParam::Camera) {
            return;
        }

        auto _streamType = streamTypeFromProfile(m_lastCameraReqStreamType, m_itemProfile);
        //SPDLOG_DEBUG("checkDualStream(), streamType=new:{}, before:{}, profile:{}", _streamType, m_mediaParam->streamType(), m_itemProfile);

        if (_streamType != m_mediaParam->streamType()) {
            m_mediaParam->setStreamType(_streamType);
            if (m_mediaControl->changeStreamType(_streamType)) {
                SPDLOG_DEBUG("checkDualStream(), change streamType, new:{}, before:{}", _streamType, m_mediaParam->streamType());
            }
        }
        else if (WisenetMediaParam::isAuto(_streamType)) {
            if (m_mediaControl != nullptr) {
                if (m_mediaControl->checkDualStream()) {
                    SPDLOG_DEBUG("checkDualStream(), change auto stream, streamType={}", _streamType);
                }
            }
        }
    }
}


WisenetMediaParam* WisenetMediaPlayer::mediaParam() const
{
    return m_mediaParam;
}

void WisenetMediaPlayer::setPosition(qint64 pos)
{
    if (m_position != pos) {
        m_position = pos;
        //SPDLOG_DEBUG("m_position={}", pos);
        emit positionChanged();

        // senconds value
        auto positionSeconds = pos/1000;
        if (m_positionSeconds != positionSeconds) {
            auto gap = abs(m_positionSeconds - positionSeconds);

            m_positionSeconds = positionSeconds;
            emit positionSecondsChanged();

            if (m_seekable && gap >= 3) { // playback 3초이상 차이가 나면
                emit playbackTimeChanged();
            }
        }
    }
}

void WisenetMediaPlayer::setSeekable(const bool seekable)
{
    if (m_seekable != seekable) {
        m_seekable = seekable;
        emit seekableChanged();
    }
}

qint64 WisenetMediaPlayer::position() const
{
    return m_position;
}

qint64 WisenetMediaPlayer::positionSeconds() const
{
    return m_positionSeconds;
}

float WisenetMediaPlayer::speed() const
{
    if (m_mediaSource) {
        return m_mediaSource->speed();
    }
    return 1.0f;
}

void WisenetMediaPlayer::setSpeed(const float playbackSpeed)
{
    if (m_mediaControl != nullptr) {
        m_mediaControl->setSpeed(playbackSpeed, m_position);
    }

    if (m_posMediaControl != nullptr) {
        m_posMediaControl->setSpeed(playbackSpeed, m_position);
    }
}

float WisenetMediaPlayer::seekable() const
{
    if (m_mediaParam != nullptr) {
        return m_mediaParam->seekable();
    }

    return 0.0f;
}

WisenetMediaPlayer::ItemProfile WisenetMediaPlayer::itemProfile() const
{
    return m_itemProfile;
}

void WisenetMediaPlayer::setItemProfile(const ItemProfile profile)
{
    if (m_itemProfile != profile) {
        m_itemProfile = profile;
        emit itemProfileChanged();
    }
}


void WisenetMediaPlayer::updateOutputParam(const qreal screenPixelRatio,
                                           const int width, const int height)
{
    SPDLOG_DEBUG("updateOutputParam()::pixelRatio={}, width={}, height={}",
                 screenPixelRatio, width, height);

    m_outWidth = width;
    m_outHeight = height;
    if (screenPixelRatio > 0) {
        m_screenPixelRatio = screenPixelRatio;
    }

    if (m_mediaControl != nullptr) {
        m_mediaControl->updateOutputParam(m_screenPixelRatio, m_outWidth, m_outHeight);
    }
}

void WisenetMediaPlayer::setLocalFileFisheyeSettings(const bool enable,
                                                     const QString lensType,
                                                     const int lensLocation)
{
    if (m_mediaSource != nullptr) {
        if (m_mediaParam->sourceType() == WisenetMediaParam::SourceType::LocalResource) {
            m_mediaSource->setFisheyeParam(enable, lensType, lensLocation);
        }
    }
}

bool WisenetMediaPlayer::fisheyeDewarpStatus() const
{
    if (m_mediaSource) {
        return m_mediaSource->fisheyeDewarpStatus();
    }
    return false;
}

bool WisenetMediaPlayer::micStatus() const
{
    return m_micStatus;
}

void WisenetMediaPlayer::setMicStatus(const bool enable)
{
    SPDLOG_DEBUG("setMicStatus(), enable={}", enable, m_micStatus);
    if(m_mediaControl){
        if (m_micStatus != enable) {

            m_micStatus = enable;
            emit micStatusChanged();
        }
    }
}


void WisenetMediaPlayer::udpateFisheyeDewarpOutputParam(const int width, const int height)
{
    if (m_dewarpCam) {
        m_dewarpCam->setFisheyeDewarpOutputParam(width, height);
    }
}

void WisenetMediaPlayer::dewarpPtzContinuous(const int viewIndex, const int pan, const int tilt, const int zoom)
{
    if (m_dewarpCam) {
        m_dewarpCam->ptzContinuous(viewIndex, pan, tilt, zoom);
    }
}

void WisenetMediaPlayer::dewarpPtzStop(const int viewIndex)
{
    if (m_dewarpCam) {
        m_dewarpCam->ptzStop(viewIndex);
    }
}

void WisenetMediaPlayer::dewarpPtzZoom1x(const int viewIndex)
{
    if (m_dewarpCam) {
        m_dewarpCam->ptzZoom1x(viewIndex);
    }
}

void WisenetMediaPlayer::dewarpPtzPointMove(const int viewIndex, const qreal x, const qreal y)
{
    if (m_dewarpCam) {
        m_dewarpCam->ptzPointMove(viewIndex, x, y);
    }
}

void WisenetMediaPlayer::dewarpPtzAreaMove(const int viewIndex, const qreal x, const qreal y, const qreal width, const qreal height)
{
    if (m_dewarpCam) {
        m_dewarpCam->ptzAreaMove(viewIndex, x, y, width, height);
    }
}

void WisenetMediaPlayer::dewarpPtzAbsZoom(const int viewIndex, const qreal zoomValue)
{
    if (m_dewarpCam) {
        m_dewarpCam->ptzAbsZoom(viewIndex, zoomValue);
    }
}

void WisenetMediaPlayer::setFisheyeViewMode(const int mode)
{
    if (m_fisheyeViewMode != mode) {
        m_fisheyeViewMode = mode;
        m_fisheyeOutputSize = QSize(0,0);
        emit videoOutSizeChanged();
        if (m_dewarpCam) {
            m_dewarpCam->setViewMode(m_fisheyeViewMode);
            m_dewarpCam->resetViewPosition();
        }
        emit fisheyeViewModeChanged();
    }
}

void WisenetMediaPlayer::deviceTalk(const bool on)
{
    if (m_mediaSource != nullptr) {
        m_mediaSource->deviceTalk(on);
    }
}

void WisenetMediaPlayer::onFisheyeViewPositionChanged(const QVector<qreal> positions)
{
    //SPDLOG_DEBUG("onFisheyeViewPositionChanged(), position size={}", positions.size());
    m_fisheyeViewPositions = positions;
    emit fisheyeViewPositionChanged();
}

QVector<qreal> WisenetMediaPlayer::fisheyeViewPosition() const
{
    return m_fisheyeViewPositions;
}

void WisenetMediaPlayer::setFisheyeViewPosition(const QVector<qreal> positions)
{
    if (m_dewarpCam) {
        SPDLOG_DEBUG("setFisheyeViewPosition(), position size={}", positions.size());
        m_dewarpCam->setViewPosition(positions);
    }
}

QList<int> WisenetMediaPlayer::getGroupKeyList()
{
    if(!groupPresets().empty())
    {
        return groupPresets().keys();
    }
    else
    {
        return QList<int>();
    }
}

QList<int> WisenetMediaPlayer::getGroupPresetList(const int groupNumberKey)
{
    if(groupPresets().empty()) {
        return QList<int>();
    }

    if(groupPresets().find(groupNumberKey) != groupPresets().end())
    {
        return groupPresets().find(groupNumberKey).value();
    }
    else
    {
        return QList<int>();
    }
}

void WisenetMediaPlayer::setEnableFisheyeDewarp(const bool enable)
{
    SPDLOG_DEBUG("setEnableFisheyeDewarp(), enable={}", enable, m_enableFisheyeDewarp);
    // Fisheye Dewarping On/Off 상태
    if (m_enableFisheyeDewarp != enable) {
        m_enableFisheyeDewarp = enable;
        if (m_mediaSource) {
            resetFisheyeDewarp();
        }
        else {
            SPDLOG_DEBUG("setEnableFisheyeDewarp() called, but mediaSource is not ready!");
        }
        emit enableFisheyeDewarpChanged();
    }
}

void WisenetMediaPlayer::onFisheyeDewarpStatusChanged()
{
    // 원본 소스의 DewarpStatus가 변경되면 드와핑 ON/OFF 재확인
    if (m_isRefSourceType) {
        resetFisheyeDewarp();
    }
    emit fisheyeDewarpStatusChanged();
}


void WisenetMediaPlayer::onFisheyeParamChanged()
{
    if (m_mediaSource != nullptr) {
        // 드와핑중 설정정보만 변경시
        if (m_dewarpCam && m_mediaSource->supportFisheyeDewarp()) {
            m_dewarpCam->setLensParam(m_mediaSource->fisheyeLensType(),
                                      m_mediaSource->fisheyeLocation());
        }
        else if (!m_isRefSourceType) {
            resetFisheyeDewarp();
        }
    }
}

void WisenetMediaPlayer::resetFisheyeDewarp()
{
    if (m_mediaSource == nullptr) {
        SPDLOG_INFO("resetFisheyeDewarp() call, but mediaSource is null");
        return;
    }

    bool dewarpOn = m_isRefSourceType ? (m_mediaSource->fisheyeDewarpStatus()) :
                                        (m_mediaSource->supportFisheyeDewarp() && m_enableFisheyeDewarp);

    SPDLOG_DEBUG("resetFisheyeDewarp(), dewarpOn={}", dewarpOn);

    if (dewarpOn) {
        setKeepOriginalSize(true);
        m_dewarpCam.reset(new ImvDewarp(nullptr));
        m_dewarpCam->setup(m_mediaSource->fisheyeLocation(),
                           m_mediaSource->mediaName(),
                           m_fisheyeViewMode,
                           m_mediaSource->fisheyeLensType(),
                           m_screenPixelRatio, m_outWidth, m_outHeight,
                           m_isRefSourceType);
        if (m_isRefSourceType) {
            m_dewarpCam->setViewPort(m_viewPort);
            connect(m_dewarpCam.data(), &ImvDewarp::fisheyeViewPortChanged, this, &WisenetMediaPlayer::fisheyeViewPortChanged, Qt::QueuedConnection);
        }
        connect(m_dewarpCam.data(), &ImvDewarp::newVideoFrame, this, &WisenetMediaPlayer::onFilteredNewVideoFrame, Qt::QueuedConnection);
        connect(m_dewarpCam.data(), &ImvDewarp::setupResult, this, &WisenetMediaPlayer::onDewarpSetupResult, Qt::QueuedConnection);
        connect(m_dewarpCam.data(), &ImvDewarp::fisheyeViewPositionChanged, this, &WisenetMediaPlayer::onFisheyeViewPositionChanged, Qt::QueuedConnection);

        m_dewarpCam->start();
    }
    else {
        m_dewarpCam.reset();
    }

    // mediaSource의 fisheye dewarp 상태는 원본 아이템에서 변경한다.
    if (!m_isRefSourceType) {
        m_mediaSource->setFisheyeDewarpStatus(dewarpOn);
    }

    updateSurfaceViewPort(m_viewPort);

    // Pause상태에서 Dewarp on/off 상태 변경시에는 마지막 영상을 다시 source로 요청해서 불러온다.
    if (m_mediaSource->playbackState() == PlaybackState::PausedState) {
        m_mediaSource->requestLastVideoFrame();
    }
}

void WisenetMediaPlayer::onMicStatusChanged()
{
    emit micStatusChanged();
}

void WisenetMediaPlayer::onTalkResult(bool on, bool success, int errorCode)
{
    SPDLOG_DEBUG("WisenetMediaPlayer::onTalkResult errorCode={}", errorCode);
    if(!success)
        emit talkErrorResult(errorCode);
    else
        setMicStatus(on);
}

void WisenetMediaPlayer::onMicUnavilable()
{
    SPDLOG_DEBUG("WisenetMediaPlayer::onMicUnavilable");
}

void WisenetMediaPlayer::onVideoSizeChanged()
{
    if(m_enableMaskingMode) {
        m_maskPainter.setPixelRatio((float)videoOutSize().width() / (float)videoSize().width());
    }

    emit videoSizeChanged();
}

void WisenetMediaPlayer::onVideoOutSizeChanged()
{
    if(m_enableMaskingMode) {
        m_maskPainter.setPixelRatio((float)videoOutSize().width() / (float)videoSize().width());
    }

    emit videoOutSizeChanged();
}

bool WisenetMediaPlayer::keepOriginalSize() const
{
    return m_keepOriginalSize;
}

void WisenetMediaPlayer::setKeepOriginalSize(const bool keep)
{
    if (m_keepOriginalSize != keep) {
        SPDLOG_DEBUG("setKeepOriginalSize():: keep={}, isRefSource={}, controlReady={}",
                     keep, m_isRefSourceType, (m_mediaControl?true:false));
        m_keepOriginalSize = keep;
        emit keepOriginalSizeChanged();
    }

    if (m_mediaControl != nullptr) {
        m_mediaControl->setKeepOriginalSize(keep);
    }

    if (m_posMediaControl != nullptr) {
        m_posMediaControl->setKeepOriginalSize(keep);
    }
}


void WisenetMediaPlayer::seek(const qint64 playbackTimeMsec)
{
    if (m_mediaControl != nullptr) {
        m_mediaControl->seek(playbackTimeMsec);
    }

    if (m_posMediaControl != nullptr) {
        m_posMediaControl->seek(playbackTimeMsec);
    }
}

void WisenetMediaPlayer::pause()
{
    if (m_mediaControl != nullptr) {
        m_mediaControl->pause();
    }

    if (m_posMediaControl != nullptr) {
        m_posMediaControl->pause();
    }

    if(m_isTracking) {
        stopTracking();
    }
}

void WisenetMediaPlayer::play()
{
    if (m_mediaControl != nullptr) {
        m_mediaControl->play(m_position);
    }

    if (m_posMediaControl != nullptr) {
        m_posMediaControl->play(m_position);
    }
}

void WisenetMediaPlayer::step(const bool isForward)
{
    if (m_mediaControl != nullptr) {
        m_mediaControl->step(isForward, m_position);
    }

    if (m_posMediaControl != nullptr) {
        m_posMediaControl->step(isForward, m_position);
    }
    if (m_enableMaskingMode) {
        // TODO
    }
}

WisenetMediaPlayer::PlaybackState WisenetMediaPlayer::playbackState() const
{
    if (m_mediaSource != nullptr) {
        return static_cast<PlaybackState>(m_mediaSource->playbackState());
    }
    return PlaybackState::PausedState;
}

WisenetMediaPlayer::PlayStatus WisenetMediaPlayer::status() const
{
    if (m_mediaSource != nullptr) {
        return static_cast<PlayStatus>(m_mediaSource->status());
    }
    return PlayStatus::NoMedia;
}

QString WisenetMediaPlayer::mediaName() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->mediaName();
    }
    return "";
}

bool WisenetMediaPlayer::supportVideoIn() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->supportVideoIn();
    }
    return false;
}

bool WisenetMediaPlayer::supportAudioIn() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->supportAudioIn();
    }
    return false;
}

bool WisenetMediaPlayer::supportAudioOut() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->supportAudioOut();
    }
    return false;
}

bool WisenetMediaPlayer::supportDualStreamLive() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->supportDualStreamLive();
    }
    return false;
}

bool WisenetMediaPlayer::supportDualStreamPlayback() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->supportDualStreamPlayback();
    }
    return false;
}

bool WisenetMediaPlayer::supportPanTilt() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->supportPanTilt();
    }
    return false;
}

bool WisenetMediaPlayer::supportZoom() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->supportZoom();
    }
    return false;
}

bool WisenetMediaPlayer::supportZoom1x() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->supportZoom1x();
    }
    return false;
}

bool WisenetMediaPlayer::supportAreaZoom() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->supportAreaZoom();
    }
    return false;
}

bool WisenetMediaPlayer::supportPtzHome() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->supportPtzHome();
    }
    return false;
}

bool WisenetMediaPlayer::supportPreset() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->supportPreset();
    }
    return false;
}

bool WisenetMediaPlayer::supportSwing() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->supportSwing();
    }
    return false;
}

bool WisenetMediaPlayer::supportGroup() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->supportGroup();
    }
    return false;
}

bool WisenetMediaPlayer::supportTour() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->supportTour();
    }
    return false;
}

bool WisenetMediaPlayer::supportTrace() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->supportTrace();
    }
    return false;
}

bool WisenetMediaPlayer::supportSimpleFocus() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->supportSimpleFocus();
    }
    return false;
}

bool WisenetMediaPlayer::supportAutoFocus() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->supportAutoFocus();
    }
    return false;
}

bool WisenetMediaPlayer::supportResetFocus() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->supportResetFocus();
    }
    return false;
}

bool WisenetMediaPlayer::supportDefog() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->supportDefog();
    }
    return false;
}

bool WisenetMediaPlayer::supportFocus() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->supportFocus();
    }
    return false;
}

bool WisenetMediaPlayer::supportIris() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->supportIris();
    }
    return false;
}

bool WisenetMediaPlayer::supportDuration() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->supportDuration();
    }
    return false;
}

bool WisenetMediaPlayer::supportTimeline() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->supportTimeline();
    }
    return false;
}

bool WisenetMediaPlayer::supportMic() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->supportMic();
    }
    return false;
}

bool WisenetMediaPlayer::supportSmartSearch() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->supportSmartSearch();
    }
    return false;
}

bool WisenetMediaPlayer::supportSmartSearchFilter() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->supportSmartSearchFilter();
    }
    return false;
}

bool WisenetMediaPlayer::enablePtz() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->enablePtz();
    }
    return false;
}

bool WisenetMediaPlayer::permissionPlayback() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->permissionPlayback();
    }
    return true;
}

bool WisenetMediaPlayer::permissionLocalRecording() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->permissionLocalRecording();
    }
    return true;
}

bool WisenetMediaPlayer::permissionPtzControl() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->permissionPtzControl();
    }
    return true;
}

bool WisenetMediaPlayer::permissionAudioIn() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->permissionAudioIn();
    }
    return true;
}

bool WisenetMediaPlayer::permissionMic() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->permissionMic();
    }
    return true;
}

bool WisenetMediaPlayer::supportFisheyeDewarp() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->supportFisheyeDewarp();
    }
    return false;
}

bool WisenetMediaPlayer::enableFisheyeDewarp() const
{
    return m_enableFisheyeDewarp;
}

int WisenetMediaPlayer::fisheyeLocation() const
{
    if (m_mediaSource != nullptr) {
        return (int)m_mediaSource->fisheyeLocation();
    }
    return (int)MediaLayoutItemViewModel::L_Ceiling;
}

QString WisenetMediaPlayer::fisheyeLensType() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->fisheyeLensType();
    }
    return "";
}

int WisenetMediaPlayer::fisheyeViewMode() const
{
    return m_fisheyeViewMode;
}

qint64 WisenetMediaPlayer::duration() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->duration();
    }
    return 0;
}

QMap<qint64, qint64> WisenetMediaPlayer::getTimeline() const
{
    if (m_mediaControl != nullptr) {
        return m_mediaControl->getTimeline();
    }
    return QMap<qint64, qint64>();
}

QSize WisenetMediaPlayer::videoSize() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->videoSize();
    }
    return QSize(0,0);
}

QSize WisenetMediaPlayer::videoOutSize() const
{
    if(m_fisheyeViewMode != MediaLayoutItemViewModel::V_Original && !m_lastVideoFrame.image().isNull()) {
        return m_lastVideoFrame.image().size();
    }
    else if (m_mediaSource != nullptr) {
        return m_mediaSource->videoOutSize();
    }
    return QSize(0,0);
}

QString WisenetMediaPlayer::videoCodec() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->videoCodec();
    }
    return "None";
}

QString WisenetMediaPlayer::audioCodec() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->audioCodec();
    }
    return "None";
}

float WisenetMediaPlayer::videoKbps() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->videoKbps();
    }
    return 0;
}

float WisenetMediaPlayer::videoFps() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->videoFps();
    }
    return 0;
}

QString WisenetMediaPlayer::videoProfile() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->videoProfile();
    }
    return "None";
}

int WisenetMediaPlayer::videoBufferingCount() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->videoBufferingCount();
    }
    return 0;
}

QString WisenetMediaPlayer::mediaId() const
{
    if (m_mediaControl != nullptr) {
        return m_mediaControl->mediaId();
    }
    return "";
}

bool WisenetMediaPlayer::isSequence() const
{
    return m_isSequence;
}

bool WisenetMediaPlayer::deviceUsingSetup() const
{
    return m_deviceUsingSetup;
}

void WisenetMediaPlayer::setIsSequence(bool isSequence)
{
    m_isSequence = isSequence;
    isSequenceChanged(isSequence);
}

QRectF WisenetMediaPlayer::viewPort() const
{
    return m_viewPort;
}

void WisenetMediaPlayer::setViewPort(const QRectF normalizedRect)
{
    if (m_viewPort != normalizedRect) {
        m_viewPort = normalizedRect;

        if (m_isRefSourceType && m_dewarpCam) {
            m_dewarpCam->setViewPort(normalizedRect);
        }
        emit viewPortChanged();
    }
}

bool WisenetMediaPlayer::enableAudioOut() const
{
    return m_enableAudioOut;
}

void WisenetMediaPlayer::setEnableAudioOut(const bool on)
{
    if (m_enableAudioOut != on) {
        m_enableAudioOut = on;

        // 오디오 디코더 on/off
        if (m_mediaSource != nullptr) {
            if (m_mediaSource && !m_isRefSourceType) {
                m_mediaSource->setEnableAudioOut(m_enableAudioOut);
            }
        }

        emit enableAudioOutChanged();
    }
}

void WisenetMediaPlayer::setPlaybackState(const PlaybackState playbackState)
{
    if (m_mediaSource != nullptr) {
        m_mediaSource->setPlaybackState(static_cast<int>(playbackState));
    }
}

bool WisenetMediaPlayer::localRecordingStatus() const
{
    return m_localRecordingStatus;
}

void WisenetMediaPlayer::setLocalRecording(const bool recordingStart, QString fileName)
{
#ifndef MEDIA_FILE_ONLY
    if (m_mediaControl) {
        if (m_localRecordingStatus != recordingStart) {
            QString fileDir = QCoreServiceManager::Instance().Settings()->localRecordingPath();
            if (fileDir.isEmpty()) {
                fileDir = QDir::currentPath();
                SPDLOG_WARN("setLocalRecording(), Output dir is empty, use current directory, dir={}", fileDir.toStdString());
            }
            int saveDuration = QCoreServiceManager::Instance().Settings()->localRecordingTime();
            int localRecordingFormat = QCoreServiceManager::Instance().Settings()->localRecordingFormat();
            fileName += localRecordingFormat == 0 ? ".mkv" :
                                                    localRecordingFormat == 1 ? ".wnm" : "";

            // 2022.12.28. coveriry
            if (m_mediaControl == nullptr) {
                return;
            }

            bool ret = m_mediaControl->setLocalRecording(recordingStart, fileDir, fileName, saveDuration);
            SPDLOG_DEBUG("fileDir={}, name={}, duration={} min", fileDir.toStdString(), fileName.toStdString(), saveDuration);
            if(m_localRecordingStatus != ret)
            {
                if(ret)
                {
                    m_localRecordingPath = fileDir;
                    emit localRecordingPathChanged();
                }
                m_localRecordingStatus = ret;
                emit localRecordingStatusChanged();
            }
        }
    }
#endif
}

void WisenetMediaPlayer::onLocalRecordingTimeout()
{
    FileWriteManager::getInstance()->requestUpdateLocalDir();
    if(m_localRecordingStatus) {
        m_localRecordingStatus = false;
        emit localRecordingStatusChanged();
    }
}

QString WisenetMediaPlayer::localRecordingPath() const
{
    return m_localRecordingPath;
}

void WisenetMediaPlayer::copyWisenetPlayer()
{
    FileWriteManager::getInstance()->copyWisenetPlayer(m_localRecordingPath);
}

void WisenetMediaPlayer::saveSnapshot(QImage image)
{
    if(m_isRefSourceType && !m_lastVideoFrame.image().isNull()) {
        QRect viewPort = m_surfaceFormat.viewport();
        SPDLOG_DEBUG("saveSnapshot(1) ZoomTarget. viewPort {} {} {} {}", viewPort.x(), viewPort.y(), viewPort.width(), viewPort.height());
        QImage cropImage = m_lastVideoFrame.image().copy(viewPort);
        imageToLocalFile(cropImage);
    }
    else if(m_fisheyeViewMode != MediaLayoutItemViewModel::V_Original && !m_lastVideoFrame.image().isNull()) {
        SPDLOG_DEBUG("saveSnapshot(2) FisheyeDewarp.");
        imageToLocalFile(m_lastVideoFrame.image());
    }
    else if(m_mediaSource != nullptr) {
        // mediaSource에 원본사이즈 이미지 요청
        SPDLOG_DEBUG("saveSnapshot(3) Original image.");
        m_mediaSource->requestLastOriginalVideoImage();
    }
    else {
        // qml에서 grap한 이미지 저장
        SPDLOG_DEBUG("saveSnapshot(4) Grapped image.");
        imageToLocalFile(image);
    }
}

void WisenetMediaPlayer::imageToLocalFile(QImage image)
{
    SPDLOG_DEBUG("imageToLocalFile() Entered");
    QFuture<void> future = QtConcurrent::run([=]() {
        QString fileDir = "";
#ifndef MEDIA_FILE_ONLY
        // 2022.12.28. coverity
        auto setting = QCoreServiceManager::Instance().Settings();
        if (setting != nullptr) {
            fileDir = setting->localRecordingPath();
        }
#else
        QString fileName = m_mediaParam->localResource().toLocalFile();
        QFileInfo fileInfo = QFileInfo(fileName);
        fileDir = fileInfo.absolutePath();
#endif

        if (fileDir.isEmpty()) {
            fileDir = QDir::currentPath();
        }

        QString name = mediaName();
        while(name.length() > 0 && name.at(0) == '.') {
            name.replace(0, 1, ""); // 첫 문자가 . 이 되지 않도록.
        }
        name.replace("\\", ""); // \ 문자 제거
        name = name.remove(QRegExp("[/:*?<>|\"]")); // 나머지 특수문자 제거
#ifndef MEDIA_FILE_ONLY
        if(m_mediaParam) {
            Wisenet::Device::Device device;
            if(QCoreServiceManager::Instance().DB()->FindDevice(m_mediaParam->deviceId(), device)) {
                name = name + "(" + QString::fromStdString(device.connectionInfo.ip) + ")";
            }
        }
#endif

        QDateTime now = QDateTime::currentDateTime();
        QString timeStr = now.toString("yyyyMMdd_hhmmss_zzz");

        QString filePath = fileDir + "/" + name + "_" + timeStr + ".png";

        SPDLOG_INFO("imageToLocalFile(), file={}",
                    filePath.toStdString());

        bool result = image.save(filePath);
        if (!result) {
            SPDLOG_WARN("imageToLocalFile(), failed to save, file={}",
                        filePath.toStdString());
        }

        emit imageSaved(result, fileDir);
        FileWriteManager::getInstance()->requestUpdateLocalDir();
    });
}

void WisenetMediaPlayer::devicePtzContinuous(const int pan, const int tilt, const int zoom)
{
    if (m_mediaSource != nullptr) {
        m_mediaSource->devicePtzContinuous(pan, tilt, zoom);
    }
}

void WisenetMediaPlayer::devicePtzStop()
{
    if (m_mediaSource != nullptr) {
        m_mediaSource->devicePtzStop();
    }
}

void WisenetMediaPlayer::devicePtzPresetGet()
{
    if (m_mediaSource != nullptr) {
        m_mediaSource->devicePtzPresetGet();
    }
}

void WisenetMediaPlayer::devicePtzPresetMove(const int presetNumber)
{
    if (m_mediaSource != nullptr) {
        m_mediaSource->devicePtzPresetMove(presetNumber);
    }
}

void WisenetMediaPlayer::devicePtzPresetDelete(const int presetNumber)
{
    if (m_mediaSource != nullptr) {
        m_mediaSource->devicePtzPresetDelete(presetNumber);
    }
}

void WisenetMediaPlayer::devicePtzPresetAdd(const int presetNumber, const QString &presetName)
{
    if (m_mediaSource != nullptr) {
        m_mediaSource->devicePtzPresetAdd(presetNumber, presetName);
    }
}

void WisenetMediaPlayer::deviceFocusContinuous(const int command)
{
    if (m_mediaSource != nullptr) {
        m_mediaSource->deviceFocusContinuous(command);
    }
}

void WisenetMediaPlayer::deviceFocusModeControl(const int focusMode)
{
    if (m_mediaSource != nullptr) {
        m_mediaSource->deviceFocusModeControl(focusMode);
    }
}

QQmlListProperty<PtzPresetItemModel> WisenetMediaPlayer::ptzPresets()
{
    if (m_mediaSource != nullptr) {
        QList<PtzPresetItemModel*>& tempList = m_mediaSource->ptzPresets();
        return QQmlListProperty<PtzPresetItemModel>(m_mediaSource, &tempList);
    }
    else {
        QList<PtzPresetItemModel> tempList;
        return QQmlListProperty<PtzPresetItemModel>();
    }
}

WisenetMediaParam::StreamType WisenetMediaPlayer::streamType() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->streamType();
    }
    //SPDLOG_DEBUG("streamType() is not ready... maybe bug!");
    return WisenetMediaParam::PlaybackHigh;
}

QString WisenetMediaPlayer::getDateTime24h(long long utcTimeMsec)
{
    if (supportDuration()) {
        auto utcTimeSec = utcTimeMsec/1000;
        auto durationSec = m_mediaSource->duration()/1000;
        QTime currentTime((utcTimeSec / 3600) % 60, (utcTimeSec / 60) % 60,
                          utcTimeSec % 60, (utcTimeSec * 1000) % 1000);
        QTime totalTime((durationSec / 3600) % 60, (durationSec / 60) % 60,
                        durationSec % 60, (durationSec * 1000) % 1000);
        QString format = durationSec > 3600 ? "hh:mm:ss" : "mm:ss";
        QString tStr = currentTime.toString(format) + " / " + totalTime.toString(format);
        return tStr;
    }
    return QLocaleManager::Instance()->getDateTime24h(utcTimeMsec);
}

bool WisenetMediaPlayer::hasControl()
{
    if (m_mediaControl != nullptr)
        return true;
    return false;
}

int WisenetMediaPlayer::maxPtzPresets() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->maxPtzPresets();
    }
    return 0;
}

// swing
void WisenetMediaPlayer::devicePtzSwingGet()
{
    if (m_mediaSource != nullptr) {
        m_mediaSource->devicePtzSwingGet();
    }
}

void WisenetMediaPlayer::devicePtzSwingMoveStop(const int actionType)
{
    // int to SwingActionType 형변환
    const auto swingActionType = static_cast<Wisenet::Device::SwingActionType>(actionType);

    if (m_mediaSource != nullptr) {
        m_mediaSource->devicePtzSwingMoveStop(swingActionType);
    }
}
int WisenetMediaPlayer::swingType() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->swingType();
    }
    return 0;
}

int WisenetMediaPlayer::swingPanStartPreset() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->swingPanStartPreset();
    }
    return 0;
}

int WisenetMediaPlayer::swingPanEndPreset() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->swingPanEndPreset();
    }
    return 0;
}

int WisenetMediaPlayer::swingTiltStartPreset() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->swingTiltStartPreset();
    }
    return 0;
}

int WisenetMediaPlayer::swingTiltEndPreset() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->swingTiltEndPreset();
    }
    return 0;
}

int WisenetMediaPlayer::swingPanTiltStartPreset() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->swingPanTiltStartPreset();
    }
    return 0;
}

int WisenetMediaPlayer::swingPanTiltEndPreset() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->swingPanTiltEndPreset();
    }
    return 0;
}

// group
QMap<int,QList<int>> WisenetMediaPlayer::groupPresets() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->groupPresets();
    }
    return QMap<int,QList<int>>();
}

void WisenetMediaPlayer::devicePtzGroupGet()
{
    if (m_mediaSource != nullptr) {
        m_mediaSource->devicePtzGroupGet();
    }
}

void WisenetMediaPlayer::devicePtzGroupMove(const int groupNumber)
{
    if (m_mediaSource != nullptr) {
        m_mediaSource->devicePtzGroupMove(groupNumber);
    }
}

void WisenetMediaPlayer::devicePtzGroupStop(const int groupNumber)
{
    if (m_mediaSource != nullptr) {
        m_mediaSource->devicePtzGroupStop(groupNumber);
    }
}

// tour
QList<int> WisenetMediaPlayer::tourPresets() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->tourPresets();
    }
    return QList<int>();
}

void WisenetMediaPlayer::devicePtzTourGet()
{
    if (m_mediaSource != nullptr) {
        m_mediaSource->devicePtzTourGet();
    }
}

void WisenetMediaPlayer::devicePtzTourMove()
{
    if (m_mediaSource != nullptr) {
        m_mediaSource->devicePtzTourMove();
    }
}

void WisenetMediaPlayer::devicePtzTourStop()
{
    if (m_mediaSource != nullptr) {
        m_mediaSource->devicePtzTourStop();
    }
}

// trace
QList<int> WisenetMediaPlayer::tracePresets() const
{
    if (m_mediaSource != nullptr) {
        return m_mediaSource->tracePresets();
    }
    return QList<int>();
}

void WisenetMediaPlayer::devicePtzTraceGet()
{
    if (m_mediaSource != nullptr) {
        m_mediaSource->devicePtzTraceGet();
    }
}

void WisenetMediaPlayer::devicePtzTraceMove(const int traceNumber)
{
    if (m_mediaSource != nullptr) {
        m_mediaSource->devicePtzTraceMove(traceNumber);
    }
}

void WisenetMediaPlayer::devicePtzTraceStop(const int traceNumber)
{
    if (m_mediaSource != nullptr) {
        m_mediaSource->devicePtzTraceStop(traceNumber);
    }
}

void WisenetMediaPlayer::checkPassword(QString password)
{
    m_password = password;
    if (m_mediaControl != nullptr) {
        m_mediaControl->checkPassword(password);
    }
}

void WisenetMediaPlayer::sleep(const bool isOn)
{
    if (m_mediaControl != nullptr) {
        m_mediaControl->sleep(isOn);
    }

    if (m_dewarpCam != nullptr) {
        m_dewarpCam->sleep(isOn);
    }
}

// source의 speed property만 변경하고 실제 동작수행은 하지 않는다.(for NVR동기재생)
void WisenetMediaPlayer::updatePlaybackStatus(const PlaybackState state,
                                              const float speed,
                                              const int trackId,
                                              const bool playWait)
{
    const bool isPlay = (state==PlaybackState::PlayingState);
    m_mediaParam->setTrackId(trackId);

    if (m_mediaControl != nullptr) {
        m_mediaControl->setMediaParam(m_mediaParam);
        m_mediaControl->updatePlaybackStatus(isPlay, speed);
    }

    if (m_mediaSource != nullptr) {
        m_mediaSource->setPlaybackState(static_cast<int>(state));
        m_mediaSource->setSpeed(speed);
        m_mediaSource->updatePlaybackStatus(isPlay, speed, playWait);
    }
}

MaskItemListModel* WisenetMediaPlayer::maskItemListModel()
{
    if(m_maskItemListModel.isNull())
    {
        qDebug() << "WisenetMediaPlayer::maskItemListModel() new MaskItemListModel";
        m_maskItemListModel = new MaskItemListModel();
    }

    return m_maskItemListModel;
}

bool WisenetMediaPlayer::enableMaskingMode() const
{
    return m_enableMaskingMode;
}

void WisenetMediaPlayer::setEnableMaskingMode(const bool enable)
{
    SPDLOG_DEBUG("setEnableMaskingMode(), enable={}", enable);
    // Object Tracker On/Off status
    if (m_enableMaskingMode != enable) {
        m_enableMaskingMode = enable;
        if (m_mediaSource) {
            resetObjectTracker();
        }
        else {
            SPDLOG_DEBUG("setEnableMaskingMode() called, but mediaSource is not ready!");
        }
        emit enableMaskingModeChanged();
    }
}

bool WisenetMediaPlayer::maskPreviewOn() const
{
    return m_maskPreviewOn;
}

void WisenetMediaPlayer::setMaskPreviewOn(const bool enable)
{
    SPDLOG_DEBUG("setMaskPreviewOn(), enable={}", enable);
    if (m_maskPreviewOn != enable) {
        m_maskPreviewOn = enable;
        emit maskPreviewOnChanged();

        refreshMaskResult();
    }
}

MaskingExportProgressModel* WisenetMediaPlayer::maskingExportProgressModel()
{
    return m_maskingExportProgressModel.data();
}

void WisenetMediaPlayer::resetObjectTracker()
{
#ifdef Q_OS_WINDOWS
    if (m_mediaSource == nullptr) {
        SPDLOG_INFO("resetObjectTracker() call, but mediaSource is null");
        return;
    }

    SPDLOG_DEBUG("resetObjectTracker(), trackingOn={}", m_enableMaskingMode);

    if (m_enableMaskingMode) {
        m_objectTracker.reset(new CvAutoTracker(this));
        m_manualTrackingTimer.setInterval(100);
        connect(&m_manualTrackingTimer, &QTimer::timeout, this, &WisenetMediaPlayer::onAddManualTrackingResult, Qt::QueuedConnection);
        connect(m_objectTracker.data(), &CvAutoTracker::newVideoFrame, this, &WisenetMediaPlayer::onTrackedNewVideoFrame, Qt::QueuedConnection);

        m_objectTracker->start();
    }
    else {
        m_objectTracker.reset();
    }
#endif

    // Pause상태에서 on/off 상태 변경시에는 마지막 영상을 다시 source로 요청해서 불러온다.
    if (m_mediaSource->playbackState() == PlaybackState::PausedState) {
        m_mediaSource->requestLastVideoFrame();
    }
}

bool WisenetMediaPlayer::isTracking() const
{
    return m_isTracking;
}

void WisenetMediaPlayer::addMask(const int maskId, const int maskType, const QVector<qreal> rectCoordinates)
{
#ifdef Q_OS_WINDOWS
    if(rectCoordinates.size() != 4)
        return;

    // convert normalized rectCoordinates to real size
    int videoWidth = videoOutSize().width();
    int videoHeight = videoOutSize().height();
    int left = rectCoordinates[0] * videoWidth;
    int top = rectCoordinates[1] * videoHeight;
    int maskWidth = rectCoordinates[2] * videoWidth;
    int maskHeight = rectCoordinates[3] * videoHeight;
    NormalMaskRect maskRect(rectCoordinates[0], rectCoordinates[1], rectCoordinates[2], rectCoordinates[3]);

    SPDLOG_DEBUG("WisenetMediaPlayer::addMask() id:{}, type:{}, x:{}, y:{}, width:{}, height:{}",
                 maskId, maskType, left, top, maskWidth, maskHeight);

    QString log = QString("id:%1, type:%2, x:%3, y:%4, width:%5, height:%6").arg(maskId).arg(maskType).arg(left).arg(top).arg(maskWidth).arg(maskHeight);
    qDebug() << "WisenetMediaPlayer::addMask()" << log;

    // create and append new MaskItem
    MaskItem* maskItem = new MaskItem(maskId, (WisenetMaskType::MaskType)maskType);

    if(maskType == (int)WisenetMaskType::MaskType::Fixed) {
        auto timeLineMap = getTimeline();
        maskItem->AddKeyFrameMaskIndicator(MaskIndicator(maskRect, timeLineMap.firstKey()));
        maskItem->AddKeyFrameMaskIndicator(MaskIndicator(maskRect, timeLineMap.last()));
    }
    else {
        maskItem->AddKeyFrameMaskIndicator(MaskIndicator(maskRect, m_position));
    }

    maskItem->cropImage = m_mediaSource->getLastImage().copy(left,top,maskWidth,maskHeight);
    maskItem->fullImage = m_mediaSource->getLastImage();
    m_maskItemListModel->append(maskItem);

    refreshMaskResult();
#endif
}
void WisenetMediaPlayer::removeMask(const int maskId, const int maskType)
{
#ifdef Q_OS_WINDOWS
    SPDLOG_DEBUG("WisenetMediaPlayer::removeMask() id:{}, type:{}", maskId, maskType);

    QString log = QString("id:%1, type:%2").arg(maskId).arg(maskType);
    qDebug() << "WisenetMediaPlayer::removeMask()" << log;

    // remove MaskItem
    m_maskItemListModel->removeByMaskId(maskId);

    refreshMaskResult();
#endif
}

// Auto, Fixed : UI에서 마스킹 박스의 위치나 사이즈가 변경되었을때 실행되는 함수
// Manual : UI에서 마스킹 박스를 클릭하고 있던 마우스가 release될 때 실행 or 다른 매뉴얼 아이템을 클릭했을 때 실행됨
void WisenetMediaPlayer::updateMask(const int maskId, const int maskType, const QVector<qreal> rectCoordinates)
{
#ifdef Q_OS_WINDOWS
    if(rectCoordinates.size() != 4)
        return;

    // convert normalized rectCoordinates to real size
    int videoWidth = videoOutSize().width();
    int videoHeight = videoOutSize().height();
    int left = rectCoordinates[0] * videoWidth;
    int top = rectCoordinates[1] * videoHeight;
    int maskWidth = rectCoordinates[2] * videoWidth;
    int maskHeight = rectCoordinates[3] * videoHeight;
    NormalMaskRect maskRect(rectCoordinates[0], rectCoordinates[1], rectCoordinates[2], rectCoordinates[3]);

    SPDLOG_DEBUG("WisenetMediaPlayer::updateMask() id:{}, type:{}, x:{}, y:{}, width:{}, height:{}",
                 maskId, maskType, left, top, maskWidth, maskHeight);

    QString log = QString("id:%1, type:%2, x:%3, y:%4, width:%5, height:%6").arg(maskId).arg(maskType).arg(left).arg(top).arg(maskWidth).arg(maskHeight);
    qDebug() << "WisenetMediaPlayer::updateMask()" << log;

    // Update MaskItemList's Crop Image
    if(m_maskItemListModel->isNeedUpdateCrop(maskId, m_position)) {
        SPDLOG_DEBUG("WisenetMediaPlayer::updateMask() updateCropImage()");       
        m_maskItemListModel->updateCropImage(m_mediaSource->getLastImage(), maskId, left, top, maskWidth, maskHeight);
    }

    // Update Fixed MaskItem
    if(maskType == (int)WisenetMaskType::MaskType::Fixed) {
        SPDLOG_DEBUG("FIXED ITEM - WisenetMediaPlayer::updateMask() updateCropImage()");
        m_maskItemListModel->UpdateFixedMaskItem(maskId, maskRect);
    }
    else { // Update Manual & Auto MaskItem
        m_maskItemListModel->AddKeyFrameMaskIndicator(maskId, MaskIndicator(maskRect, m_position));
    }

    refreshMaskResult();
#endif
}

// statTracking 버튼을 눌렀을 때 실행되는 함수
void WisenetMediaPlayer::startTracking()
{
    SPDLOG_DEBUG("WisenetMediaPlayer::startTracking()");

#ifdef Q_OS_WINDOWS
    m_isTracking = true;
    emit isTrackingChanged();

    m_maskItemListModel->initTempMaskIndicatorList();

    if(m_objectTracker) {
        m_currentMaskingType = m_maskItemListModel->getSelectedMaskItemType();
        QPair<int, NormalMaskRect> selectedMaskItem = m_maskItemListModel->getSelectedMaskItemRect(m_position); //key: maskId, value: NormalMaskRect
        m_currentTrackingID = selectedMaskItem.first;
        if (m_currentMaskingType == (int)WisenetMaskType::MaskType::Auto) {
            SPDLOG_DEBUG("WisenetMediaPlayer::startTracking() with InitTracker maskId: {}", selectedMaskItem.first);
            m_objectTracker->InitTracker(selectedMaskItem.first, selectedMaskItem.second);
        }
        else { // Manual Mask Item
            SPDLOG_DEBUG("WisenetMediaPlayer::startTracking() with RestTracker");
            m_objectTracker->RestTracker();

            m_manualTrackingRect = std::make_pair(selectedMaskItem.first, m_maskItemListModel->getManualMaskRect(m_position));
            m_manualTrackingTimer.start();
            SPDLOG_DEBUG("WisenetMediaPlayer::startTracking AddKeyFrameMaskIndicator Fixed or Manual Item, id: {} position {} {} {} {} ",
                         m_manualTrackingRect.first, m_manualTrackingRect.second.normalX, m_manualTrackingRect.second.normalY, m_manualTrackingRect.second.normalWidth, m_manualTrackingRect.second.normalHeight);
            m_maskItemListModel->AddTempMaskIndicator(MaskIndicator(m_manualTrackingRect.second, m_position));
        }
        m_objectTracker->StartTracking();
    }
#endif
}

void WisenetMediaPlayer::stopTracking()
{
    SPDLOG_DEBUG("WisenetMediaPlayer::stopTracking()");

#ifdef Q_OS_WINDOWS
    m_isTracking = false;
    emit isTrackingChanged();

    if(m_objectTracker) {
        m_objectTracker->StopTracking();
    }

    QPair<int, NormalMaskRect> selectedMaskItem = m_maskItemListModel->getSelectedMaskItemRect(m_position); // key: maskId, value: NormalMaskRect
    SPDLOG_DEBUG("WisenetMediaPlayer::stopTracking() m_currentTrackingID : {}, current selected mask ID : {}", m_currentTrackingID, selectedMaskItem.first );
    if (m_currentTrackingID != selectedMaskItem.first) { // 다른 아이템이 선택되어 stopTracking이 실행된 경우
        // m_currentTrackingID로 마스크 아이템 정보 가져오기
        selectedMaskItem = m_maskItemListModel->getMaskItembyId(m_currentTrackingID, m_position);
    }

    if (m_currentMaskingType == (int)WisenetMaskType::MaskType::Manual) {
        if(m_isUpdatingManualRect) { // if user makes change after mask creation (addMask())
            SPDLOG_DEBUG("WisenetMediaPlayer::stopTracking() {}", m_manualTrackingRect.first);
            m_maskItemListModel->AddTempMaskIndicator(MaskIndicator(m_manualTrackingRect.second, m_position));
            m_isUpdatingManualRect = false;
        }
        else {  // if user doesn't make any changes after mask creation
            m_maskItemListModel->AddKeyFrameMaskIndicator(selectedMaskItem.first, MaskIndicator(selectedMaskItem.second, m_position));
        }
    }

    m_manualTrackingTimer.stop();
    m_maskItemListModel->updateMaskIndicator(selectedMaskItem.first);
#endif
}
void WisenetMediaPlayer::startMaskingExport(const qint64 startTime, const qint64 endTime, const QString filePath, const QString fileName,
                                            const bool usePassword, const QString password, const bool useDigitalSignature)
{
    SPDLOG_DEBUG("WisenetMediaPlayer::startMaskingExport() startTime:{}, endTime:{}, filePath:{}, fileName:{}",
                 startTime, endTime, filePath.toStdString(), fileName.toStdString());

    m_maskingExportProgressModel.reset(new MaskingExportProgressModel());
    emit maskingExportProgressModelChanged();
    m_maskingExportProgressModel->setFilePath(filePath);

    m_maskingTranscoder.reset(new MaskingTranscoder());
    m_maskingTranscoder->start();
    m_maskingTranscoder->setMaskItemList(m_maskItemListModel->getMaskItemList());
    m_maskingTranscoder->setMaskPainter(m_maskPainter);
    WnmInitalizeParameter initParam;
    initParam.usePassword = usePassword;
    initParam.password = password;
    initParam.useDigitalSignature = useDigitalSignature;
    m_maskingTranscoder->setWnmInitalizeParameter(initParam);

    connect(m_maskingTranscoder.data(), &MaskingTranscoder::progressChanged,
            m_maskingExportProgressModel.data(), &MaskingExportProgressModel::onProgressChanged, Qt::QueuedConnection);
    connect(m_maskingTranscoder.data(), &MaskingTranscoder::writeFinished,
            m_maskingExportProgressModel.data(), &MaskingExportProgressModel::onWriteFinished, Qt::QueuedConnection);

    m_maskingTranscoder->open(startTime, endTime, m_mediaParam->localResource(), filePath, fileName, m_password);
}

void WisenetMediaPlayer::stopMaskingExport()
{
    if(m_maskingTranscoder)
        m_maskingTranscoder->close();

    if(m_maskingExportProgressModel && m_maskingExportProgressModel->progress() != 100) {
        m_maskingExportProgressModel->setCanceled(true);
    }
}

void WisenetMediaPlayer::updateManualTrackingRect(const int maskId, const int maskType, const QVector<qreal> rectCoordinates)
{
    if(maskType != (int)WisenetMaskType::MaskType::Manual || rectCoordinates.size() != 4l)
        return;

    m_isUpdatingManualRect = true;
    m_manualTrackingRect.first = maskId;
    m_manualTrackingRect.second = NormalMaskRect(rectCoordinates[0], rectCoordinates[1], rectCoordinates[2], rectCoordinates[3]);
}

void WisenetMediaPlayer::onAddManualTrackingResult()
{
    if(m_isUpdatingManualRect) {
        SPDLOG_DEBUG("WisenetMediaPlayer::onAddManualTrackingResult()");
        m_maskItemListModel->AddTempMaskIndicator(MaskIndicator(m_manualTrackingRect.second, m_position));
    }
}

void WisenetMediaPlayer::setMaskPaintType(int paintType)
{
    qDebug() << "WisenetMediaPlayer::setMaskPaintType" << paintType;
    m_maskPainter.setPaintType((WisenetMaskType::MaskPaintType)paintType);
    refreshMaskResult();
}

void WisenetMediaPlayer::setMaskInclude(bool include)
{
    qDebug() << "WisenetMediaPlayer::setMaskInclude" << include;
    m_maskPainter.setInclude(include);
    refreshMaskResult();
}

void WisenetMediaPlayer::setMaskRGBColor(const QString& hexColor)
{
    QColor color(hexColor);
    qDebug() << "WisenetMediaPlayer::setMaskRGBColor" << color.red() << color.green() << color.blue();
    m_maskPainter.setRGBColor((uchar)color.red(), (uchar)color.green(), (uchar)color.blue());
    refreshMaskResult();
}

void WisenetMediaPlayer::setMaskMosaicCellSize(uchar size)
{
    qDebug() << "WisenetMediaPlayer::setMaskMosaicCellSize" << size;
    m_maskPainter.setMosaicCellSize(size);
    refreshMaskResult();
}

// re-present last video with last mask result
void WisenetMediaPlayer::refreshMaskResult()
{
    if(!m_enableMaskingMode)
        return;

    // request last video frame for refresh mask when paused state
    if (m_mediaSource != nullptr && m_mediaSource->playbackState() == PlaybackState::PausedState)
        m_mediaSource->requestLastVideoFrame();
}
