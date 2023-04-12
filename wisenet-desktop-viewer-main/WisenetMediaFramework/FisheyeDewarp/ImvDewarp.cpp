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
#include "ImvDewarp.h"
#include "LogSettings.h"
#include "WisenetVideoBuffer.h"
#include <cmath>

#undef SPDLOG_LOGGER_CALL
#define SPDLOG_LOGGER_CALL(logger, level, ...) (logger)->log(spdlog::source_loc{__FILE__, __LINE__, "[DEWARP] "}, level, __VA_ARGS__)



const long long PTZ_ANIMATION_USEC = 300000; // point이동 혹은 areaZoom시 애니메이션 시간 (300msec)

static int Align4(int value)
{
    return (((value + 3) >> 2) << 2);
}


ImvDewarp::ImvDewarp(QObject *parent)
    : QObject(parent)
{
    if (Wisenet::Common::statLoggerRaw()->should_log(spdlog::level::debug)) {
        m_checkFrameCount = 300;
    }

    SPDLOG_DEBUG("ImvDewarp::ImvDewarp()");
    std::memset(&m_inputBuffer, 0x00, sizeof(m_inputBuffer));
    std::memset(&m_outputBuffer, 0x00, sizeof(m_outputBuffer));
}

ImvDewarp::~ImvDewarp()
{
    SPDLOG_DEBUG("ImvDewarp::~ImvDewarp()");
    stop();
    disconnect();
}

void ImvDewarp::setup(const int location,
                      const QString &mediaName,
                      const int viewMode,
                      const QString &cameraLensType,
                      const qreal screenPixelRatio,
                      const int outWidth, const int outHeight,
                      const bool isViewPortMode)
{
    m_mediaName = mediaName.toStdString();
    m_lensType = cameraLensType;
    m_location = location;
    m_screenPixelRatio = screenPixelRatio;
    m_outWidth = outWidth;
    m_outHeight = outHeight;
    m_viewMode = isViewPortMode ? (int)MediaLayoutItemViewModel::V_Single : viewMode ;
    m_isViewPortMode = isViewPortMode;
    if (isViewPortMode) m_mediaName += "(Zoom)";
}


// CBC : B4QQV
// AMBARELLA : B6SST
// BOOWONOPTICALS ? B6SST : B5SST

// SNF-8010/8010VM : C7SST
// PNF-9010R/RV/RVM : C8WWT
// XNF-8010R/RV/RVM : D1SST
// HCF-8010V : D4SST
// QNF-8010/KAN : D9SSV
// QNF-9010 : E3VVV
// XNF-9010RV, TNF-9010, XNF-9013 : E5VVT
static const char* getRplNumber(const QString& lensType)
{
    if (lensType == "SNF-8010")
        return "C7SST";
    if (lensType == "PNF-9010")
        return "C8WWT";
    if (lensType == "XNF-8010")
        return "D1SST";
    if (lensType == "HCF-8010")
        return "D4SST";
    if (lensType == "QNF-8010")
        return "D9SSV";
    if (lensType == "QNF-9010")
        return "E3VVV";
    if(lensType == "XNF-9010" || lensType == "TNF-9010"|| lensType == "XNF-9013")
        return "E5VVT";
    return "E5VVT";
}
static unsigned long getViewMode(const int viewMode)
{
    if (viewMode == MediaLayoutItemViewModel::V_Quad)
        return IMV_Defs::E_VTYPE_QUAD;
    if (viewMode == MediaLayoutItemViewModel::V_Panorama)
        return IMV_Defs::E_VTYPE_PERI;
    return IMV_Defs::E_VTYPE_PTZ;
}
static unsigned long getLocation(const int location)
{
    if (location == (int)MediaLayoutItemViewModel::L_Ground)
        return IMV_Defs::E_CPOS_GROUND;
    else if (location == (int)MediaLayoutItemViewModel::L_Wall)
        return IMV_Defs::E_CPOS_WALL;
    return IMV_Defs::E_CPOS_CEILING;
}

static unsigned long getColorFormat(const QVideoFrame::PixelFormat qPixelFormat)
{
    if (qPixelFormat == QVideoFrame::PixelFormat::Format_NV12)
        return IMV_Defs::E_YUV_NV12_STD;
    else if (qPixelFormat == QVideoFrame::PixelFormat::Format_BGRA32)
        return IMV_Defs::E_BGRA_32_STD;
    return IMV_Defs::E_YUV_I420_STD;
}

static AVPixelFormat getAvPixelFormat(const QVideoFrame::PixelFormat qPixelFormat)
{
    if (qPixelFormat == QVideoFrame::PixelFormat::Format_NV12)
        return AVPixelFormat::AV_PIX_FMT_NV12;
    else if (qPixelFormat == QVideoFrame::PixelFormat::Format_BGRA32)
        return AVPixelFormat::AV_PIX_FMT_BGRA;
    return AVPixelFormat::AV_PIX_FMT_YUV420P;
}

struct PtzRange
{
    float min = 0;
    float max = 0;
};

// https://stackoverflow.com/questions/54427870/how-to-overflow-a-value-out-of-a-certain-range-like-30-100
static float overflowRange(const PtzRange &range, const float fValue)
{
    float ret = fValue;
    bool needToChange = false;
    if (fValue < range.min || fValue > range.max) {
        needToChange = true;
        float size = range.max - range.min;
        ret -= range.min;
        ret = std::fmod(ret, size);
        if (ret < 0) {
            ret += size;
        }
        ret += range.min;
    }
#ifdef DEWARP_TEST_LOG
    SPDLOG_DEBUG("overflowRange, {} ==> {} {}", fValue, ret, (needToChange? "****" : "OK"));
#endif

    return ret;
}


static PtzRange getPanRange(const int location)
{
    // -180 ~ 180
    PtzRange range;
    range.min = -180; range.max = 180;

    // -90 ~ 90
    if (location == MediaLayoutItemViewModel::L_Wall) {
        range.min = -90; range.max = 90;
    }
    return range;
}

static PtzRange getTiltRange(const int location)
{
    // -90 ~ 90
    PtzRange range;
    range.min = -90; range.max = 90;

    // 0 ~ 90
    if (location == MediaLayoutItemViewModel::L_Ceiling) {
        range.min = 0; range.max = 90;
    }
    // -90 ~ 0
    else if (location == MediaLayoutItemViewModel::L_Ground) {
        range.min = -90; range.max = 0;
    }
    return range;
}

// 아 수학은 어렵다. ㅠ.ㅠ;;
static float calcuateDistance(const float from, const float to, const PtzRange& range)
{
    float halfSize = std::fabs(range.max - range.min)/2;
    float distance = to - from;

    // 360도 회전각도인 경우에만 계산
//    SPDLOG_DEBUG("calcuateDistance() from={}, to={}, halfSize={}, distance={},{}",
//                 from, to, halfSize, distance, std::fabs(distance));
    if (halfSize > 179 && std::fabs(distance) > halfSize) {
        float gap = distance > 0 ? (range.max-range.min)*-1 : (range.max-range.min);
        float newDistance = distance + gap;
        SPDLOG_DEBUG("calcuateDistance() direction change, distance={}, halfSize={}, newDistance={}",
                     distance, halfSize, newDistance);
        distance = newDistance;
    }
    return distance;
}

void ImvDewarp::setViewPort(const QRectF nRect)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isSetViewPortCommand = true;
    m_lastViewPort = nRect;
}

void ImvDewarp::setViewMode(const int viewMode)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isSetViewModeCommand = true;
    m_lastViewMode = viewMode;
    SPDLOG_DEBUG("setViewMode()::{}", viewMode);
}

void ImvDewarp::setFisheyeDewarpOutputParam(const int width, const int height)
{
    SPDLOG_DEBUG("setFisheyeDewarpOutputParam, mediaName={}, width={}, height={}", m_mediaName, width, height);
    const std::lock_guard<std::mutex> lock(m_mutex);
    if (m_lastWidth != width || m_lastHeight != height) {
        m_isUpdateOutputParamCommand = true;
        m_lastWidth = width;
        m_lastHeight = height;
    }
}

void ImvDewarp::setLensParam(const QString &lensType, const int location)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isSetLensParamCommand = true;
    m_lastLensType = lensType;
    m_lastLensLocation = location;
}

void ImvDewarp::setViewPosition(const QVector<qreal> positions)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    //SPDLOG_DEBUG("setViewPosition(), positions length={}", positions.length());
    m_isSetViewPositionCommand = true;
    m_lastSetViewPosition = positions;
}

void ImvDewarp::onNewVideoFrame(const QVideoFrame &frame, const qint64 playbackTimeMsec, const bool jpegColorSpace)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    //SPDLOG_DEBUG("onNewVideoFrame(), resolution={}x{}", frame.width(), frame.height());
    m_isNewVideoFrame = true;
    m_lastVideoFrame.frame = frame;
    m_lastVideoFrame.playbackTimeMsec = playbackTimeMsec;
    m_lastVideoFrame.jpegColorSpace = jpegColorSpace;
}

// original view에서는 dewarping을 하지 않고 마지막 프레임만 저장해 놓는다. (low fps나 paused status대응)
void ImvDewarp::onLastVideoFrame(const QVideoFrame &frame, const qint64 playbackTimeMsec, const bool jpegColorSpace)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_lastVideoFrame.frame = frame;
    m_lastVideoFrame.playbackTimeMsec = playbackTimeMsec;
    m_lastVideoFrame.jpegColorSpace = jpegColorSpace;
}

void ImvDewarp::ptzContinuous(const int viewIndex, const int pan, const int tilt, const int zoom)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    SPDLOG_DEBUG("ptzContinous, viewIndex={}, pan={}, tilt={}, zoom={}", viewIndex, pan, tilt, zoom);
    m_isPtzCommand = true;
    m_lastPtzParam.act = true;
    m_lastPtzParam.pan = pan;
    m_lastPtzParam.tilt = tilt;
    m_lastPtzParam.zoom = zoom;
    m_lastPtzParam.viewIndex = viewIndex;
}

void ImvDewarp::ptzStop(const int viewIndex)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    SPDLOG_DEBUG("ptzStop, viewIndex={}", viewIndex);

    m_isPtzCommand = true;
    m_lastPtzParam = PtzParam();
    m_lastPtzParam.viewIndex = viewIndex;
}

void ImvDewarp::ptzZoom1x(const int viewIndex)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    SPDLOG_DEBUG("ptzZoom1x, viewIndex={}", viewIndex);
    m_isPtzZoom1xCommand = true;
    m_lastPtzAbsoluteParam.viewIndex = viewIndex;
}

void ImvDewarp::ptzPointMove(const int viewIndex, const qreal x, const qreal y)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    SPDLOG_DEBUG("ptzPointMove, viewIndex={}, x={}, y={}", viewIndex, x, y);
    m_isPtzPointMoveCommand = true;
    m_lastPtzAbsoluteParam.viewIndex = viewIndex;
    m_lastPtzAbsoluteParam.pointX = x;
    m_lastPtzAbsoluteParam.pointY = y;
}

void ImvDewarp::ptzAreaMove(const int viewIndex, const qreal x, const qreal y, const qreal width, const qreal height)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    SPDLOG_DEBUG("ptzAreaMove, viewIndex={}, x={}, y={}, width={}, height={}", viewIndex, x, y, width, height);
    m_isPtzAreaMoveCommand = true;
    m_lastPtzAbsoluteParam.viewIndex = viewIndex;
    m_lastPtzAbsoluteParam.pointX = x;
    m_lastPtzAbsoluteParam.pointY = y;
    m_lastPtzAbsoluteParam.pointW = width;
    m_lastPtzAbsoluteParam.pointH = height;
}

void ImvDewarp::ptzAbsZoom(const int viewIndex, const qreal zoomValue)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    //SPDLOG_DEBUG("ptzAbsZoom, viewIndex={}, zoom={}", viewIndex, zoomValue);

    m_isPtzAbsZoomCommand = true;
    m_lastPtzAbsoluteParam.viewIndex = viewIndex;
    m_lastPtzAbsoluteParam.absZoom = zoomValue;
}

void ImvDewarp::sleep(const bool isOn)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isSleepCommand = true;
    m_newSleepStatus = isOn;
}

/**
 * @brief ImvDewarp::reset
 * 초기셋업함수
 * 첫번째 input video가 들어왔을 때 호출한다.
 */
void ImvDewarp::reset(QVideoFrameInfo& videoIn)
{
    SPDLOG_DEBUG("reset() entered");
    // input 버퍼는 원본영상 데이터 할당.
    m_inputBuffer.width = videoIn.frame.width();
    m_inputBuffer.height = videoIn.frame.height();
    m_inputBuffer.frameX = 0;
    m_inputBuffer.frameY = 0;
    m_inputBuffer.frameWidth = m_inputBuffer.width;
    m_inputBuffer.frameHeight = m_inputBuffer.height;
    videoIn.frame.map(QAbstractVideoBuffer::ReadOnly);
    m_inputBuffer.data = videoIn.frame.bits();
    videoIn.frame.unmap();

    // output 버퍼는 출력화면만큼 사이즈를 잡는다.
    int outWidth = 0;
    int outHeight = 0;
    calculateOutSize(m_inputBuffer.width, m_inputBuffer.height, outWidth, outHeight);
    m_outputBuffer.width = outWidth;
    m_outputBuffer.height = outHeight;
    m_outputBuffer.frameX = 0;
    m_outputBuffer.frameY = 0;
    m_outputBuffer.frameWidth = m_outputBuffer.width;
    m_outputBuffer.frameHeight = m_outputBuffer.height;

    m_pixelFormat = videoIn.frame.pixelFormat();
    AVPixelFormat ffPixelFormat = getAvPixelFormat(m_pixelFormat);
    m_outFrame = std::make_shared<VideoOutputFrame>(m_outputBuffer.width, m_outputBuffer.height, ffPixelFormat);
    m_outputBuffer.data = m_outFrame->data(0);

    auto lensType = getRplNumber(m_lensType);
    int ret = m_cam.SetLens((char*)lensType);
    SPDLOG_DEBUG("setLensType(), type={}, ret={}", lensType, ret);

    setThreadCount(m_inputBuffer.width);
    m_setupResult = m_cam.SetVideoParams(&m_inputBuffer, &m_outputBuffer,
                                    getColorFormat(m_pixelFormat),
                                    getViewMode(m_viewMode),
                                    getLocation(m_location));
    if (m_setupResult != IMV_Defs::E_ERR_OK) {
        if(m_setupResult != IMV_Defs::E_ERR_NOTPANOMORPH) {
            // 일반 카메라에 Fisheye 설정 시 E_ERR_NOTPANOMORPH가 계속 발생하여 로그 남기지 않음
            SPDLOG_WARN("SetVideoParams() failed, mediaName={}, Resolution={}x{}, RPL={}, result={}/{}",
                        m_mediaName, m_inputBuffer.frameWidth, m_inputBuffer.frameHeight,
                        lensType,
                        m_setupResult, m_cam.GetErrorString(m_setupResult));
        }
        // lensType == "HCF-8010" 인 경우에 초기 셋업이 실패하여 30회 반복 retry시도, 10회에서 성공하는듯.
        // TODO:: IMV사에 문의하여 내용 확인중
        if (m_lensType == "HCF-8010") {
            int retryCount = 0;
            while (retryCount < 30 && m_setupResult != IMV_Defs::E_ERR_OK) {
                m_setupResult = m_cam.SetVideoParams(&m_inputBuffer, &m_outputBuffer,
                                                getColorFormat(m_pixelFormat),
                                                getViewMode(m_viewMode),
                                                getLocation(m_location));
                retryCount++;
                SPDLOG_DEBUG("SetVideoParams() for HCF-8010 retry!!, retryCount={}, ret={}", retryCount, m_setupResult);
            }
        }
    }
    else {
        SPDLOG_INFO("SetVideoParams(), mediaName={}, Resolution={}x{}, Ouput={}x{}, RPL={}",
                    m_mediaName, m_inputBuffer.frameWidth, m_inputBuffer.frameHeight,
                    m_outputBuffer.width, m_outputBuffer.height,
                    lensType);
        m_cam.SetZoomLimits(20.f, 180.f);
        /*
         * 특정 영상에서 드와핑 delay발생 및 전체 드와핑 속도 개선
         * IMV 사에 따르면 QR Code Marker Detection이 자동으로 동작하고, 이로 인한 드와핑 지연이 발생할 수 있음.
         */
        char rpl[128]={};
        int nbBytes;
        bool hasRpl = (m_cam.GetMarkersInfo((char*)"RPL", (void*)rpl, &nbBytes) == IMV_Defs::E_ERR_OK);
        if (!hasRpl) {
            auto res = m_cam.SetPrivateSTG((char*)"marks_disable");
            SPDLOG_DEBUG("SetVideoParams(), marks_diable!, mediaName={}, res={}/{}", m_mediaName, res, m_cam.GetErrorString(res));
        }
    }
    if (m_setupResult != IMV_Defs::E_ERR_OK) {
        std::memset(&m_inputBuffer, 0x00, sizeof(m_inputBuffer));
        std::memset(&m_outputBuffer, 0x00, sizeof(m_outputBuffer));
    }
    /*
    float pan = 0;
    float tilt = 0;
    float roll = 0;
    m_cam.GetCameraRotation(&pan, &tilt, &roll);
    roll = 45;
    m_cam.SetCameraRotation(&pan, &tilt, &roll);   // CameraRotation의 roll만 45도로 설정

    m_cam.SetNavigationType(IMV_Defs::E_NAV_ROLL_ZOOMOUT_FREE);
    */
    emit setupResult(m_setupResult);
}

void ImvDewarp::resetInputVideoParam(QVideoFrameInfo &videoIn)
{
    // HCF-8010 버그로 여러번 반복해야 성공하는 케이스가 있어, 예외처리.
    if (m_lensType == "HCF-8010") {
        reset(videoIn);
        return;
    }
    // input 버퍼는 원본영상 데이터 할당.
    m_inputBuffer.width = videoIn.frame.width();
    m_inputBuffer.height = videoIn.frame.height();
    m_inputBuffer.frameX = 0;
    m_inputBuffer.frameY = 0;
    m_inputBuffer.frameWidth = m_inputBuffer.width;
    m_inputBuffer.frameHeight = m_inputBuffer.height;
    videoIn.frame.map(QAbstractVideoBuffer::ReadOnly);
    m_inputBuffer.data = videoIn.frame.bits();
    videoIn.frame.unmap();
    setThreadCount(m_inputBuffer.width);
    auto res = m_cam.SetInputVideoParams(&m_inputBuffer);
    if (res != IMV_Defs::E_ERR_OK) {
        SPDLOG_INFO("SetInputVideoParams() failed, res={}/{}", res, m_cam.GetErrorString(res));
        return;
    }
    SPDLOG_DEBUG("SetInputVideoParams() success, size={}/{}", m_inputBuffer.width, m_inputBuffer.height);
}

void ImvDewarp::setThreadCount(const int width)
{
    int res = IMV_Defs::E_ERR_OK;
    int threadCount = width/960;
    if (threadCount > 8)
        threadCount = 8;
    else if (threadCount < 1)
        threadCount = 1;

    m_ptzMovementUsec = (width <= 2048) ? 33000 : 40000;
    res = m_cam.SetThreadCount(threadCount);
    SPDLOG_DEBUG("setThreadCount(), mediaName={}, threadCount={}, res={}", m_mediaName, threadCount, m_cam.GetErrorString(res));
}

void ImvDewarp::resetOutputVideoParam()
{
    int outWidth = 0;
    int outHeight = 0;
    calculateOutSize(m_inputBuffer.width, m_inputBuffer.height, outWidth, outHeight);
    if (m_outputBuffer.data && outWidth == m_outputBuffer.width && outHeight == m_outputBuffer.height) {
        SPDLOG_DEBUG("resetOutputVideoParam() no need to reset output video param. skip, width={}, height={}",
                     outWidth, outHeight);
        return;
    }
    m_outputBuffer.width = outWidth;
    m_outputBuffer.height = outHeight;
    m_outputBuffer.frameX = 0;
    m_outputBuffer.frameY = 0;
    m_outputBuffer.frameWidth = m_outputBuffer.width;
    m_outputBuffer.frameHeight = m_outputBuffer.height;

    AVPixelFormat ffPixelFormat = getAvPixelFormat(m_pixelFormat);
    m_outFrame = std::make_shared<VideoOutputFrame>(m_outputBuffer.width, m_outputBuffer.height, ffPixelFormat);
    m_outputBuffer.data = m_outFrame->data(0);
    auto res = m_cam.SetOutputVideoParams(&m_outputBuffer);
    if (res != IMV_Defs::E_ERR_OK) {
        SPDLOG_INFO("SetOutputVideoParams() failed, res={}/{}", res, m_cam.GetErrorString(res));
        return;
    }
    SPDLOG_DEBUG("SetOutputVideoParams() success, mediaName={}, size={}/{}",
                 m_mediaName, m_outputBuffer.width, m_outputBuffer.height);
}

void ImvDewarp::resetViewPosition()
{
    SPDLOG_DEBUG("resetViewPosition(), mediaName={}", m_mediaName);

    // 2022.12.28. coverity
    float pan = 0.0f;
    float tilt = 0.0f;
    float zoom = 0.0f;

    pan = 0.0f, tilt = 45.0f, zoom = 100.0f;
    m_cam.SetPosition(&pan, &tilt, &zoom, IMV_Defs::E_COOR_ABSOLUTE, 1);
    pan = 90.0f, tilt = 45.0f, zoom = 100.0f;
    m_cam.SetPosition(&pan, &tilt, &zoom, IMV_Defs::E_COOR_ABSOLUTE, 2);
    pan = 270.0f, tilt = 45.0f, zoom = 100.0f;
    m_cam.SetPosition(&pan, &tilt, &zoom, IMV_Defs::E_COOR_ABSOLUTE, 3);
    pan = 180.0f, tilt = 45.0f, zoom = 100.0f;
    m_cam.SetPosition(&pan, &tilt, &zoom, IMV_Defs::E_COOR_ABSOLUTE, 4);
}


void ImvDewarp::calculateOutSize(const int inWidth, const int inHeight,
                                 int &outWidth, int &outHeight)
{
    qreal pixelWidth = m_screenPixelRatio*m_outWidth;
    qreal pixelHeight = m_screenPixelRatio*m_outHeight;

    // viewport mode(줌영역)인 경우에는 1:1로 output을 잡는다.
    // 타일 사이즈를 사용하고 zoom area의 컨트롤 rect만 정사각형으로 고정
#if 0
    if (m_isViewPortMode) {
        if (pixelWidth > pixelHeight) {
            pixelWidth = pixelHeight;
        }
        else {
            pixelHeight = pixelWidth;
        }
    }
#endif

    // 소스 이미지보다 output 사이즈가 큰 경우에는 사이즈를 재조정한다.
    if (pixelWidth > inWidth || pixelHeight > inHeight) {
        if (pixelWidth >= pixelHeight) {
            qreal ratio = pixelHeight / pixelWidth;
            pixelWidth = inWidth;
            pixelHeight = pixelWidth * ratio;
        }
        else {
            qreal ratio = pixelWidth / pixelHeight;
            pixelHeight = inHeight;
            pixelWidth = pixelHeight * ratio;
        }
    }

    /* IMV SDK bug? Perimeter&NonWall 에서는 화면이 늘려서 표시되므로 16:9로 고정 */
    if (m_viewMode == MediaLayoutItemViewModel::V_Panorama &&
        m_location != MediaLayoutItemViewModel::L_Wall) {
        pixelHeight = pixelWidth * 9 / 16;
    }

    outWidth = Align4(int(pixelWidth));
    outHeight = Align4(int(pixelHeight));

    SPDLOG_DEBUG("calculateOutSize(), mediaName={}, input={}/{}, output={}/{}, m_out={}/{}, screenRatio={}",
                 m_mediaName, inWidth, inHeight, outWidth, outHeight, m_outWidth, m_outHeight,
                 m_screenPixelRatio);
}

/**
 * @brief ImvDewarp::checkCommandParam
 * 외부 조건 변경 체크
 * - 새로운 명령
 * - 새로운 비디오 프레임
 * - PTZ 애니메이션 타임
 * @param commandParam
 */
void ImvDewarp::checkCommandParam(const std::chrono::steady_clock::time_point& now,
                                  CommandParam &commandParam)
{
    const std::lock_guard<std::mutex> lock(m_mutex);

    if (m_isSleepCommand) {
        m_isSleepCommand = false;
        if (m_isSleep != m_newSleepStatus) {
            SPDLOG_DEBUG("sleep mode changed::{}", m_newSleepStatus);
            m_isSleep = m_newSleepStatus;
        }
    }

    bool needToUpdateFrame = m_lastVideoFrame.frame.isValid() &&
            (m_isNewVideoFrame ||
             m_isSetViewPortCommand ||
             m_isSetViewModeCommand ||
             m_isSetLensParamCommand ||
             m_isSetViewPositionCommand ||
             m_isUpdateOutputParamCommand ||
             m_isPtzCommand ||
             m_isPtzZoom1xCommand ||
             m_isPtzPointMoveCommand ||
             m_isPtzAreaMoveCommand ||
             m_isPtzAbsZoomCommand
             );


    bool needToPtzUpdate = (m_currentPtzParam.act || m_currentPtzAbsoluteParam.act) && m_lastVideoFrame.frame.isValid();

    // ptz 동작 수행중인 경우에는 33ms에 맞춰서 ptz act명령을 수행하도록 한다.
    if (needToPtzUpdate) {
        auto elapsedUsec = std::chrono::duration_cast<std::chrono::microseconds>(now - m_lastPtzActPoint).count();
        auto calcElapsedUsec = elapsedUsec + m_lastPtzClockGap;

        // frame이 계속 들어오는 중이면 ptz 동작을 함께 수행한다.
        // frame이 들어오고 있지 않는 경우에는 33ms주기로 업데이트 체크한다.
        if (calcElapsedUsec >= m_ptzMovementUsec || m_isNewVideoFrame) {
#ifdef DEWARP_TEST_LOG
            SPDLOG_DEBUG("needToUpdateFrame!!, elasped={}, calc={}, clockGap={}, updateFlag={}",
                         elapsedUsec, calcElapsedUsec, m_lastPtzClockGap, needToUpdateFrame);
#endif
            m_lastPtzClockGap = m_isNewVideoFrame ? 0 : (calcElapsedUsec - m_ptzMovementUsec);
            if (m_lastPtzClockGap > m_ptzMovementUsec) {
                m_lastPtzClockGap = m_ptzMovementUsec;
            }

            m_lastPtzActPoint = now;
            needToUpdateFrame = true;

            if (m_currentPtzParam.act) {
                commandParam.isPtzCommand = true;
                commandParam.newPtzSpeed = (float)elapsedUsec/1000000;
            }
            else if (m_currentPtzAbsoluteParam.act) {
                commandParam.isPtzAbsoluteCommand = true;
                m_currentPtzAbsoluteParam.elaspedUsec += calcElapsedUsec;
            }
        }
    }

    if (!needToUpdateFrame)
        return;

    m_isNewVideoFrame = false;
    commandParam.isNewVideoFrame = true;
    commandParam.newVideo = m_lastVideoFrame;

    // ZoomTarget의 view port 변경
    if (m_isSetViewPortCommand) {
        m_isSetViewPortCommand = false;
        commandParam.isSetViewPortCommand = true;
        commandParam.newViewPort = m_lastViewPort;
    }
    // 뷰모드 변경(qual/single/perimeter/original)
    if (m_isSetViewModeCommand) {
        m_isSetViewModeCommand = false;
        m_viewMode = m_lastViewMode;
        commandParam.isSetViewModeCommand = true;
    }
    // 렌즈 파라미터 변경 (lensType, lensLocation)
    if (m_isSetLensParamCommand) {
        m_isSetLensParamCommand = false;
        m_lensType = m_lastLensType;
        m_location = m_lastLensLocation;
        commandParam.isSetLensParamCommand = true;
    }
    // 저장된 PTZ 위치로 이동
    if (m_isSetViewPositionCommand) {
        m_isSetViewPositionCommand = false;
        m_setViewPosition = m_lastSetViewPosition;
        commandParam.isSetViewPositionCommand = true;
    }

    // Output 크기 변경
    if (m_isUpdateOutputParamCommand) {
        m_isUpdateOutputParamCommand = false;
        commandParam.isSetUpdateOutputParam = true;
        m_outWidth = m_lastWidth;
        m_outHeight = m_lastHeight;
    }
    // Continuous PTZ 명령 확인
    if (m_isPtzCommand) {
        m_isPtzCommand = false;
        commandParam.isPtzCommand = true;
        m_currentPtzParam = m_lastPtzParam;
        if (m_currentPtzParam.act) {
            m_lastPtzClockGap = 0;
            m_lastPtzActPoint = now;
        }
    }
    // Absolute PTZ 명령 확인
    if (m_isPtzZoom1xCommand) {
        m_isPtzZoom1xCommand = false;
        commandParam.isPtzZoom1xCommand = true;
        m_currentPtzAbsoluteParam = m_lastPtzAbsoluteParam;
    }
    if (m_isPtzPointMoveCommand) {
        m_isPtzPointMoveCommand = false;
        commandParam.isPtzPointMoveCommand = true;
        m_currentPtzAbsoluteParam = m_lastPtzAbsoluteParam;
    }
    if (m_isPtzAreaMoveCommand) {
        m_isPtzAreaMoveCommand = false;
        commandParam.isPtzAreaMoveCommand = true;
        m_currentPtzAbsoluteParam = m_lastPtzAbsoluteParam;
    }
    if (m_isPtzAbsZoomCommand) {
        m_isPtzAbsZoomCommand = false;
        commandParam.isPtzAbsZoomCommand = true;
        m_currentPtzAbsoluteParam = m_lastPtzAbsoluteParam;
    }
}



///////////////////////////////////////////// START OF THRED MAIN
//#define DEWARP_TEST_LOG
//#define DEWARP_TEST_PERF

/**
 * @brief ImvDewarp::loopTask
 * 드와핑 프로세싱 쓰레드의 main loop
 */
void ImvDewarp::loopTask()
{
#ifdef DEWARP_TEST_PERF
    m_checkFrameCount = 30;
#endif

    const int waitMsec = m_isSleep ? 33 : 1;
    std::this_thread::sleep_for(std::chrono::milliseconds(waitMsec));

    auto now = std::chrono::steady_clock::now();

    /* check new frame and command */
    CommandParam cp;
    bool updateViewPositionInfo = false;

    // 새로운 명령 혹은 비디오가 들어왔는지 체크한다.
    checkCommandParam(now, cp);
    if (!cp.isNewVideoFrame || !cp.newVideo.frame.isValid() || m_isSleep) {
        if (m_isSleep) {
            SPDLOG_DEBUG("sleep mode ON!!");
        }
        return;
    }

    // 데이터 초기화
    if (!m_inputBuffer.data || !m_outputBuffer.data) {
        reset(cp.newVideo);
    }

    // pixel format이 변경된 경우에는 reset해야 한다.
    if (m_pixelFormat != cp.newVideo.frame.pixelFormat()) {
        SPDLOG_DEBUG("pixel format changed, reset, old={}, new={}",
                     m_pixelFormat, cp.newVideo.frame.pixelFormat());
        reset(cp.newVideo);
    }

    // setup이 실패한 경우에는 원본 영상 그대로 올려준다.
    // todo 실패 시그널
    if (m_setupResult != IMV_Defs::E_ERR_OK) {
        emit newVideoFrame(cp.newVideo.frame, cp.newVideo.playbackTimeMsec, cp.newVideo.jpegColorSpace);
        return;
    }

    // output size가 변경된 경우 output 버퍼를 다시 생성해야 한다.
    if (cp.isSetUpdateOutputParam) {
        resetOutputVideoParam();
    }

    // input 해상도가 변경된 경우 input 버퍼를 다시 생성해야 한다.
    if (cp.newVideo.frame.width() != (int)m_inputBuffer.width || cp.newVideo.frame.height() != (int)m_inputBuffer.height) {
        resetInputVideoParam(cp.newVideo);
        resetOutputVideoParam();    // input이 바뀌면 output도 reset
    }

    cp.newVideo.frame.map(QAbstractVideoBuffer::ReadOnly);
    m_inputBuffer.data = cp.newVideo.frame.bits();
    cp.newVideo.frame.unmap();

    // Lens parameter 변경
    if (cp.isSetLensParamCommand) {
        SPDLOG_DEBUG("SetLens() and SetCameraPosition(), lens={}, position={}", m_lensType.toStdString(), m_location);
        auto result = m_cam.SetLens((char*)getRplNumber(m_lensType));
        if (result != IMV_Defs::E_ERR_OK) {
            SPDLOG_WARN("SetLens() failed, mediaName={},result={}/{}",
                        m_mediaName, m_setupResult, m_cam.GetErrorString(m_setupResult));
        }
        result = m_cam.SetCameraPosition(getLocation(m_location));
        if (result != IMV_Defs::E_ERR_OK) {
            SPDLOG_WARN("SetCameraPosition() failed, mediaName={},result={}/{}",
                        m_mediaName, m_setupResult, m_cam.GetErrorString(m_setupResult));
        }
        resetOutputVideoParam();
    }

    if (cp.isSetViewModeCommand && m_viewMode != MediaLayoutItemViewModel::V_Original) {
        SPDLOG_DEBUG("SetViewType()::viewMode={}/{}", m_viewMode, getViewMode(m_viewMode));
        m_cam.SetViewType(getViewMode(m_viewMode));
        resetOutputVideoParam();
        updateViewPositionInfo = true;
    }

    // 줌타겟의 View Port변경
    if (cp.isSetViewPortCommand) {
        setPositionWithViewPort(cp.newViewPort, cp.newVideo);
    }

    // Set View Position
    if (cp.isSetViewPositionCommand) {
        if (m_setViewPosition.length() == 0) {
            resetViewPosition();
            updateViewPositionInfo = true;
        }
        else {
            setPositionWithViewMode(m_setViewPosition);
        }
    }

    // original view에서는 dewarping frame을 처리하지 않는다.
    if (m_viewMode == MediaLayoutItemViewModel::V_Original) {
        // original view로 변경된 경우에는 마지막 저장프레임을 한번 올려준다.
        // 즉시 변경을 위해서, eg. PAUSE
        if (cp.isSetViewModeCommand) {
            emit newVideoFrame(cp.newVideo.frame,
                               cp.newVideo.playbackTimeMsec,
                               cp.newVideo.jpegColorSpace);
        }
        return;
    }

    // Continuous PTZ 명령 실행
    if (cp.isPtzCommand && m_currentPtzParam.act) {
        updateViewPositionInfo = true;
        float pan = m_currentPtzParam.pan * cp.newPtzSpeed;
        float tilt = m_currentPtzParam.tilt * cp.newPtzSpeed;
        float zoom = m_currentPtzParam.zoom * -cp.newPtzSpeed;
        int viewIndex = m_currentPtzParam.viewIndex;
        m_cam.SetPosition(&pan, &tilt, &zoom, IMV_Defs::E_COOR_RELATIVE, viewIndex);
    }

    // Absolute PTZ 초기화
    if (cp.isPtzZoom1xCommand || cp.isPtzPointMoveCommand || cp.isPtzAreaMoveCommand) {
        cp.isPtzAbsoluteCommand = true;
        initAbsolutePtz(now, cp.isPtzPointMoveCommand, cp.isPtzZoom1xCommand);
    }

    // Absolute PTZ 이동 보간(애니메이션)
    if (cp.isPtzAbsoluteCommand && m_currentPtzAbsoluteParam.act) {
        updateViewPositionInfo = true;
        moveAbsolutePtz();
    }

    // Absolute Zoom 명령은 애니메이션 없이 즉시 처리
    if (cp.isPtzAbsZoomCommand) {
        updateViewPositionInfo = true;

        // 2022.12.28. coverity
        float pan = 0.0f;
        float tilt = 0.0f;
        float zoom = 0.0f;

        int viewIndex = m_currentPtzAbsoluteParam.viewIndex;
        m_cam.GetPosition(&pan, &tilt, &zoom, viewIndex);
        pan = 0; tilt = 0;
        zoom = m_currentPtzAbsoluteParam.absZoom - zoom;
        //SPDLOG_DEBUG("PtzAbsZoomCommand, pan={}, tilt={}, zoom={}, viewIndex={}", pan, tilt, zoom, viewIndex);
        m_cam.SetPosition(&pan, &tilt, &zoom, IMV_Defs::E_COOR_RELATIVE, viewIndex);
    }

    // OUTPUT VIDEO FRAME 생성 및 IMV output buffer update
    AVPixelFormat ffPixelFormat = getAvPixelFormat(m_pixelFormat);
    VideoOutputFrameSharedPtr newOutFrame = std::make_shared<VideoOutputFrame>(m_outputBuffer.width, m_outputBuffer.height, ffPixelFormat);
    m_outputBuffer.data = newOutFrame->data(0);

    // IMV UPDATE ////////////////////////////
    auto result = m_cam.Update();
    m_outputBuffer.data = m_outFrame->data(0);
    if (result != IMV_Defs::E_ERR_OK) {
        if (m_updateFailCount == 0) {
            SPDLOG_WARN("Update() failed, res={}/{}", result, m_cam.GetErrorString(result));
        }
        m_updateFailCount = m_updateFailCount >= 30 ? 0 : (m_updateFailCount+1);
        return;
    }
    m_updateFailCount = 0;

    // Create QVideoFrame and Emit Dewarped Frame to VideoOutput
    auto videoBuffer = new WisenetVideoBuffer();
    videoBuffer->moveDataFrom(newOutFrame);
    QSize videoSize(newOutFrame->width(), newOutFrame->height());
#if 0 /* m_outFrame을 copy하는 비용을 줄이기 위해서 newOutFrame을 만들고 IMV::outputBuffer의 포인터를 변경 */
    videoBuffer->copyDataFrom(m_outFrame);
    QSize videoSize(m_outFrame->width(), m_outFrame->height());
#endif

    QVideoFrame dewarpFrame(videoBuffer, videoSize, videoBuffer->pixelFormat());
    emit newVideoFrame(dewarpFrame, cp.newVideo.playbackTimeMsec, cp.newVideo.jpegColorSpace);

    // PTZ 동작을 수행한 경우, 현재 좌표정보를 업데이트 한다.
    if (updateViewPositionInfo) {
        updateCurrentViewPosition();
    }

    // Zoom 영역의 ViewPort가 변경된 경우, 현재 뷰포트 정보를 업데이트 한다.
    if (cp.isSetViewPortCommand) {
        updateCurrentInputViewPort();
    }

    // PERFORMANCE CHECK
    auto end = std::chrono::steady_clock::now();

    int elapsedMsec = std::chrono::duration_cast<std::chrono::milliseconds>(end - now).count();
    if (elapsedMsec > 33) {
        STATLOG_DEBUG("Too much time to IMV::Update(), MediaName={}, elapsed={}, videoMsec={}",
                     m_mediaName, elapsedMsec, cp.newVideo.playbackTimeMsec);
    }

    // Statistics for video decoding time
    if (m_maxDewarpTimeMsec < elapsedMsec)
        m_maxDewarpTimeMsec = elapsedMsec;
    m_dewarpCount++;
    m_dewarpTimeMsec += elapsedMsec;
    if (m_dewarpCount >= m_checkFrameCount) {
        float frameAvg = (float)m_dewarpTimeMsec / m_dewarpCount;
        STATLOG_INFO("{} frames avg elapsed MediaName={}, AVG={:.2f} / MAX={} ms, RES={}x{}/OUT={}x{}",
                     m_checkFrameCount, m_mediaName, frameAvg, m_maxDewarpTimeMsec,
                     cp.newVideo.frame.width(), cp.newVideo.frame.height(),
                     m_outFrame->width(), m_outFrame->height());
        m_dewarpCount = 0;
        m_dewarpTimeMsec = 0;
        m_maxDewarpTimeMsec = 0;
    }

}
///////////////////////////////////////////// END OF THRED MAIN

void ImvDewarp::setPositionWithViewPort(const QRectF& newViewPort,
                                        const QVideoFrameInfo& newVideo)
{
    int x[4], y[4];
    // left/top
    x[0] = (int)(newViewPort.x()*newVideo.frame.width());
    y[0] = (int)(newViewPort.y()*newVideo.frame.height());
    // left/bottom
    x[1] = x[0];
    y[1] = y[0] + (int)(newViewPort.height()*newVideo.frame.height());
    // right/bottom
    x[2] = x[0] + (int)(newViewPort.width()*newVideo.frame.width());
    y[2] = y[1];
    // right/top
    x[3] = x[2];
    y[3] = y[0];
    //SPDLOG_DEBUG("viewPort::{},{},{},{}", newViewPort.x(), newViewPort.y(), newViewPort.width(), newViewPort.height());
    //SPDLOG_DEBUG("poligon:: {}/{},{}/{},{}/{},{}/{}", x[0], y[0], x[1], y[1], x[2], y[2], x[3], y[3]);

    // 2022.12.28. coverity
    float pan = 0.0f;
    float tilt = 0.0f;
    float zoom = 0.0f;

    auto result = m_cam.GetPositionFromInputVideoPolygon(4, x, y,
                                                      m_outputBuffer.frameWidth,
                                                      m_outputBuffer.frameHeight,
                                                      &pan, &tilt, &zoom);
    if (result == IMV_Defs::E_ERR_OK) {
        //SPDLOG_DEBUG("SetPosition, pan={}, tilt={}, zoom={}", pan, tilt, zoom);
        m_cam.SetPosition(&pan, &tilt, &zoom);
    }
    else {
        SPDLOG_DEBUG("GetPositionFromInputVideoPolygon() failed, res={}/{}", result, m_cam.GetErrorString(result));
    }
}


void ImvDewarp::updateCurrentInputViewPort()
{
    constexpr int kLinePoint = 4;
    const int kPointCount = kLinePoint*4;

    float pan = 0, tilt = 0;
    int outX[kPointCount]={0};
    int outY[kPointCount]={0};
    int inX[kPointCount]={0};
    int inY[kPointCount]={0};
    float nX[kPointCount]={0.f};
    float nY[kPointCount]={0.f};

    int widthPTZView = m_outputBuffer.width;
    int heightPTZView = m_outputBuffer.height;
    float incWidthPTZView = widthPTZView/kLinePoint;
    float incHeightPTZView = heightPTZView/kLinePoint;
    int index = 0;

    for (int pt = 0 ; pt < kLinePoint ; pt++) {
        outX[index] = pt*incWidthPTZView;
        outY[index] = 0;
        index++;
    }
    for (int pt = 0 ; pt < kLinePoint ; pt++) {
        outX[index] = widthPTZView-1;
        outY[index] = pt*incHeightPTZView;
        index++;
    }
    for (int pt = 0 ; pt < kLinePoint ; pt++) {
        outX[index] = widthPTZView-1-pt*incWidthPTZView;
        outY[index] = heightPTZView-1;
        index++;
    }
    for (int pt = 0 ; pt < kLinePoint ; pt++) {
        outX[index] = 0;
        outY[index] = heightPTZView-1-pt*incHeightPTZView;
        index++;
    }

    QVector<qreal> points;
    for (int i = 0 ; i < kPointCount ; i++) {
        m_cam.GetPositionFromOutputVideoPoint(outX[i], outY[i], &pan, &tilt);
        m_cam.GetInputVideoPointFromPosition(pan, tilt, &inX[i], &inY[i]);
        nX[i] = (float)inX[i] / m_inputBuffer.width;
        nY[i] = (float)inY[i] / m_inputBuffer.height;

//        SPDLOG_DEBUG("[{}]::outXY={},{}, PT={},{}, inXY={},{}/{}/{}",
//                     i, outX[i], outY[i], pan, tilt, inX[i], inY[i], nX[i], nY[i]);
        points << nX[i];
        points << nY[i];
    }
    emit fisheyeViewPortChanged(points);
}

void ImvDewarp::setPositionWithViewMode(const QVector<qreal> &viewPosition)
{
    /* 3,6,9,12 */
    if (viewPosition.length() % 3 != 0 || viewPosition.length() > 12) {
        SPDLOG_DEBUG("setPositionWithViewMode(), but invalid viewPosition values, length={}", viewPosition.length());
        return;
    }

    int viewIndex = 1;
    for (int i = 0 ; i < viewPosition.length() ; i=i+3) {
        float p = viewPosition[i];
        float t = viewPosition[i+1];
        float z = viewPosition[i+2];
        SPDLOG_DEBUG("setPositionWithViewMode(), set position, ptz={}/{}/{}, index={}",
                     p, t, z, viewIndex);
        m_cam.SetPosition(&p, &t, &z, IMV_Defs::E_COOR_ABSOLUTE, viewIndex);
        viewIndex++;
    }
}

void ImvDewarp::updateCurrentViewPosition()
{
    QVector<qreal> currentPositions;

    int indexCount = (m_viewMode == MediaLayoutItemViewModel::V_Quad) ? 4 : 1;
    for (int i = 1 ; i <= indexCount ; i++) {
        float pan = 0;
        float tilt = 0;
        float zoom = 0;
        m_cam.GetPosition(&pan, &tilt, &zoom, i);
//        SPDLOG_DEBUG("updateCurrentViewPosition(), pan={},tilt={},zoom={},index={}",
//                     pan, tilt, zoom, i);
        currentPositions.append(pan);
        currentPositions.append(tilt);
        currentPositions.append(zoom);
    }

    emit fisheyeViewPositionChanged(currentPositions);
}


void ImvDewarp::initAbsolutePtz(const std::chrono::steady_clock::time_point& now,
                                const bool isPtzPointMoveCommand,
                                const bool isPtzZoom1xCommand)
{
    int result = IMV_Defs::E_ERR_OK;
    m_lastPtzClockGap = 0;
    m_lastPtzActPoint = now;

    float pan = 0, tilt = 0, zoom = 0;
    m_currentPtzAbsoluteParam.act = true;
    m_currentPtzAbsoluteParam.animationUsec = PTZ_ANIMATION_USEC;
    m_currentPtzAbsoluteParam.elaspedUsec = 0;

    // save current position
    m_cam.GetPosition(&pan, &tilt, &zoom, m_currentPtzAbsoluteParam.viewIndex);
    m_currentPtzAbsoluteParam.fromPan = pan;
    m_currentPtzAbsoluteParam.fromTilt = tilt;
    m_currentPtzAbsoluteParam.fromZoom = zoom;

    PtzRange panRange = getPanRange(m_location);
    PtzRange tiltRange = getTiltRange(m_location);

    if (isPtzZoom1xCommand) {
        m_currentPtzAbsoluteParam.toPan = -0;
        m_currentPtzAbsoluteParam.toTilt = 0;
        m_currentPtzAbsoluteParam.toZoom = 180;
        //m_currentPtzAbsoluteParam.animationUsec = 10000000;
    }
    else if (isPtzPointMoveCommand) {
        float pointX = m_currentPtzAbsoluteParam.pointX * m_outputBuffer.width;
        float pointY = m_currentPtzAbsoluteParam.pointY * m_outputBuffer.height;
        result = m_cam.GetPositionFromOutputVideoPoint((int)pointX,
                                                       (int)pointY,
                                                       &pan, &tilt);
        pan = overflowRange(panRange, pan);
        tilt = overflowRange(tiltRange, tilt);
        m_currentPtzAbsoluteParam.toPan = pan;
        m_currentPtzAbsoluteParam.toTilt = tilt;
        m_currentPtzAbsoluteParam.toZoom = zoom;

        // ceil mode에서 tilt 오류 수정 (천장으로 계속 찍으면 바닥으로 이동하는 문제, imv 버그 같음..
        if (m_location == MediaLayoutItemViewModel::L_Ceiling  && m_currentPtzAbsoluteParam.pointY < 0.5 && tilt < 45) {
            m_currentPtzAbsoluteParam.toTilt = m_currentPtzAbsoluteParam.fromTilt;
        }
        else if (m_location == MediaLayoutItemViewModel::L_Ground  && m_currentPtzAbsoluteParam.pointY > 0.5 && tilt > -45) {
            m_currentPtzAbsoluteParam.toTilt = m_currentPtzAbsoluteParam.fromTilt;
        }

        SPDLOG_DEBUG("GetPositionFromOutputVideoPoint(), pointX={}, pointY={}, pan={}, tilt={}, res={}",
                     pointX, pointY, pan, tilt,
                     m_cam.GetErrorString(result));
    }
    else { /* isPtzAreaMoveCommand */
        float pointX = m_currentPtzAbsoluteParam.pointX * m_outputBuffer.width;
        float pointY = m_currentPtzAbsoluteParam.pointY * m_outputBuffer.height;
        float pointW = m_currentPtzAbsoluteParam.pointW * m_outputBuffer.width;
        float pointH = m_currentPtzAbsoluteParam.pointH * m_outputBuffer.height;
        int x[4], y[4];
        // left/top
        x[0] = (int)pointX;
        y[0] = (int)pointY;
        // left/bottom
        x[1] = x[0];
        y[1] = y[0] + (int)pointH;
        // right/bottom
        x[2] = x[0] + (int)pointW;
        y[2] = y[1];
        // right/top
        x[3] = x[2];
        y[3] = y[0];
        result = m_cam.GetPositionFromOutputVideoPolygon(4, x, y, &pan, &tilt, &zoom);
        SPDLOG_DEBUG("GetPositionFromOutputVideoPolygon(), pan={}, tilt={}, zoom={}, res={}",
                     pan, tilt, zoom, m_cam.GetErrorString(result));

        m_currentPtzAbsoluteParam.toPan = overflowRange(panRange, pan);
        m_currentPtzAbsoluteParam.toTilt = overflowRange(tiltRange, tilt);
        m_currentPtzAbsoluteParam.toZoom = zoom;
    }

    // 이동 거리 계산
    m_currentPtzAbsoluteParam.diffPan = calcuateDistance(m_currentPtzAbsoluteParam.fromPan,
                                                         m_currentPtzAbsoluteParam.toPan,
                                                         panRange);
    m_currentPtzAbsoluteParam.diffTilt = calcuateDistance(m_currentPtzAbsoluteParam.fromTilt,
                                                          m_currentPtzAbsoluteParam.toTilt,
                                                          tiltRange);
    m_currentPtzAbsoluteParam.diffZoom = m_currentPtzAbsoluteParam.toZoom - m_currentPtzAbsoluteParam.fromZoom;

    SPDLOG_DEBUG("PTZ absolute position param, from={}/{}/{}, to={}/{}/{}, diff={}/{}/{}",
                  m_currentPtzAbsoluteParam.fromPan,  m_currentPtzAbsoluteParam.fromTilt,  m_currentPtzAbsoluteParam.fromZoom,
                  m_currentPtzAbsoluteParam.toPan,  m_currentPtzAbsoluteParam.toTilt,  m_currentPtzAbsoluteParam.toZoom,
                  m_currentPtzAbsoluteParam.diffPan,  m_currentPtzAbsoluteParam.diffTilt,  m_currentPtzAbsoluteParam.diffZoom);
}

void ImvDewarp::moveAbsolutePtz()
{
    PtzRange panRange = getPanRange(m_location);
    PtzRange tiltRange = getTiltRange(m_location);
    float pan = m_currentPtzAbsoluteParam.toPan;
    float tilt = m_currentPtzAbsoluteParam.toTilt;
    float zoom = m_currentPtzAbsoluteParam.toZoom;
    int viewIndex = m_currentPtzAbsoluteParam.viewIndex;

    if (m_currentPtzAbsoluteParam.animationUsec <= 0 ||
        m_currentPtzAbsoluteParam.elaspedUsec >= m_currentPtzAbsoluteParam.animationUsec) {
        m_currentPtzAbsoluteParam = PtzAbsoluteParam(); // final reset
    }
    else {
        float elapsedUsec = m_currentPtzAbsoluteParam.elaspedUsec <= 0 ? 1.0f : (float)m_currentPtzAbsoluteParam.elaspedUsec;
        float fspeed = elapsedUsec / m_currentPtzAbsoluteParam.animationUsec;
        pan =  m_currentPtzAbsoluteParam.fromPan + (m_currentPtzAbsoluteParam.diffPan * fspeed);
        tilt =  m_currentPtzAbsoluteParam.fromTilt + (m_currentPtzAbsoluteParam.diffTilt * fspeed);
        zoom =  m_currentPtzAbsoluteParam.fromZoom + (m_currentPtzAbsoluteParam.diffZoom * fspeed);
        pan = overflowRange(panRange, pan);
        tilt = overflowRange(tiltRange, tilt);
    }
    //SPDLOG_DEBUG("moveAbsolutePtz(), ptz={}/{}/{}", pan, tilt, zoom);
    auto result = m_cam.SetPosition(&pan, &tilt, &zoom, IMV_Defs::E_COOR_ABSOLUTE, viewIndex);
}
