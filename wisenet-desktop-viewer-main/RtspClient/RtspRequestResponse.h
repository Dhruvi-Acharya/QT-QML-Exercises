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


#include <memory>
#include <string>
#include <functional>

namespace Wisenet
{
namespace Rtsp
{

const unsigned int kDefaultRtspConnectionTimeout = 10;
const float RTSP_PLAY_PB_SUNAPI_NVR_F1 =  100000.0F;
const float RTSP_PLAY_PB_SUNAPI_NVR_B1 = -100000.0F;

enum TransportMethod
{
    RTP_UDP_UNICAST,
    RTP_UDP_MULTICAST,
    RTP_RTSP,
    RTP_RTSP_HTTP,
    RTP_RTSP_HTTPS
};

enum ResponseStatusCode : unsigned short
{
    RTSP_Continue = 100,
    RTSP_OK = 200,
    RTSP_MultipleChoices = 300,
    RTSP_MovedPermanently = 301,
    RTSP_Found = 302,
    RTSP_SeeOther = 303,
    RTSP_UseProxy = 305,

    RTSP_BadRequest = 400,
    RTSP_Unauthorized = 401,
    RTSP_PaymentRequired = 402,
    RTSP_Forbidden = 403,
    RTSP_NotFound = 404,
    RTSP_MethodNotAllowed = 405,
    RTSP_NotAcceptable = 406,
    RTSP_ProxyAuthRequired = 407,
    RTSP_RequestTimeout = 408,
    RTSP_Conflict = 409,
    RTSP_Gone = 410,
    RTSP_LengthRequired = 411,
    RTSP_PreconditionFailed = 412,
    RTSP_RequestEntityTooLarge = 413,
    RTSP_RequestUriTooLarge = 414,
    RTSP_UnsupportedMediaType = 415,
    RTSP_RangeNotSatisfiable = 416,

    RTSP_InvalidParameter = 451,
    RTSP_IllegalConferenceIdentifier = 452,
    RTSP_NotEnoughBandwidth = 453,
    RTSP_SessionNotFound = 454,
    RTSP_MethodNotValidInThisState = 455,
    RTSP_HeaderFieldNotValid = 456,
    RTSP_InvalidRange = 457,
    RTSP_ParameterIsReadOnly = 458,
    RTSP_AggregateOperationNotAllowed = 459,
    RTSP_OnlyAggregateOperationAllowed = 460,
    RTSP_UnsupportedTransport = 461,
    RTSP_DestinationUnreachable = 462,

    RTSP_InternalError = 500,
    RTSP_NotImplemented = 501,
    RTSP_BadGateway = 502,
    RTSP_ServiceUnavailable = 503,
    RTSP_GatewayTimeout = 504,
    RTSP_VersionNotSupported = 505,
    RTSP_OptionNotSupported = 551,
    RTSP_MaxUserHasExceeded = 560, /*NVR::The user has exceeded the maximum*/
    RTSP_MaxConcurrentStreamsForCodecReached = 561,
    RTSP_MaxConcurrentStreamsReached = 562,
    /* Add new error code for NVR, 2022-11-24 */
    RTSP_ChannelVideoOff = 570,
    RTSP_ChannelCovert1 = 571,
    RTSP_ChannelCovert2 = 572,
    RTSP_ChannelDeactive = 573,

    RTSP_UserOperationWasCancelled = 10001,
    RTSP_UserOperationWasTimedout = 10002,
    RTSP_UserConnectionWasClosed = 10003
};


struct RtspSetupParam
{
    bool VideoOn;
    bool AudioOn;
    bool OnvifMetaDataOn;
    bool MetaImageOn;

    RtspSetupParam()
        : VideoOn(true)
        , AudioOn(true)
        , OnvifMetaDataOn(true)
        , MetaImageOn(false)
    {}
};


enum RtspPlaybackType
{
    SUNAPI_CAMERA,
    SUNAPI_NVR,
    ONVIF,
    SSM,
};

enum RtspVideoFrameType
{
    FRAME_DEFAULT = 0,
    FRAME_ALL,
    FRAME_INTRA,
    FRAME_PREDICTED
};

struct RtspPlaybackParam
{
    /* ONVIF-Streaming Spec
    Unless the Rate-Control header is
    set to “no” (see below), the Scale parameter is used in the manner described in [RFC 2326]. If
    Rate-Control is set to “no”, the Scale parameter, if it is present, shall be either 1.0 or –1.0, to
    indicate forward or reverse playback respectively. If it is not present, forward playback is
    assumed.
    */
    bool	RateControl;

    /* ONVIF-Streaming Spec
    a PLAY command received when replay is already in
    progress will not take effect until the existing play operation has completed. This specification
    adds a new RTSP header, “Immediate”, which overrides this behaviour for the PLAY
    command that it is used with:
    */
    bool	Immidiate;


    time_t	StartTime;	/* utc-time, 0 means ntp=now- */
    time_t	EndTime;	/* utc-time, 0 means no end time */

    /*
    PAUSE 후 PLAY시 P프레임부터 재생하기 위해서, SUNAPI CAMERA인 경우에는
    Resume 필드를 통해 재생 명령을 조정해야 함.
    */
    bool	Resume;

    /* RFC-2326
    A scale value of 1 indicates normal play or record at the normal
    forward viewing rate. If not 1, the value corresponds to the rate
    with respect to normal viewing rate. For example, a ratio of 2
    indicates twice the normal viewing rate ("fast forward") and a ratio
    of 0.5 indicates half the normal viewing rate. In other words, a
    ratio of 2 has normal play time increase at twice the wallclock rate.
    For every second of elapsed (wallclock) time, 2 seconds of content
    will be delivered. A negative value indicates reverse direction.

    SUNAPI NVR에서
    Scale == RTSP_PLAY_PB_SUNAPI_NVR_F1인 경우에는 정방향 영상 한장 요청
    Scale == RTSP_PLAY_PB_SUNAPI_NVR_B1인 경우에는 역방향 영상 한장 요청
    */
    float	Scale;


    /* ONVIF Streaming Spec
    6.4.3 Frames header field
    The Frames header field may be used to reduce the number of frames that are transmitted,
    for example to lower bandwidth or processing load. Three modes are possible:
    1. Intra frames only. This is indicated using the value “intra”, optionally followed by a
    minimum interval between successive intra frames in the stream. The latter can be
    used to limit the number of frames received even in the presence of “I-frame storms”
    caused by many receivers requesting frequent I-frames.
    2. Intra frames and predicted frames only. This is indicated using the value “predicted”.
    This value can be used to eliminate B-frames if the stream includes them.
    3. All frames. This is the default.
    */
    RtspVideoFrameType FrameType;

    /* ONVIF Streaming Spec
    If playback reaches a point after which there is no further data in one or more of the streams
    being sent, it stops transmitting data but does not enter the “paused” state. If the server
    resumes recording after this has happened, delivery will resume with the new data as it is
    received.
    */
    RtspPlaybackParam()
        : RateControl(true)
        , StartTime(0)
        , EndTime(0)
        , Scale(0)
        , FrameType(FRAME_DEFAULT)
        , Resume(false)
        , Immidiate(true)
    {}
};

enum RtspEventType
{
    SOCKET_CLOSED,
};

struct RtspEvent
{
    RtspEventType EventType;
};

struct RtspResponse
{
    bool			IsSuccess;
    unsigned char	CSeq;
    unsigned short	StatusCode;
    std::string		ErrorDetails;
};

typedef std::shared_ptr<RtspEvent> RtspEventSharedPtr;
typedef std::shared_ptr<RtspResponse> RtspResponseSharedPtr;
typedef std::function <void(const RtspResponseSharedPtr&)>  RtspResponseHandler;
typedef std::function <void(const RtspEventSharedPtr& e) > RtspEventCallbackFunctor;

enum class RtspDeviceType
{
    SunapiCamera = 0,
    SunapiRecorder,
    OnvifStandard
};

enum class RtspPlayControlType
{
    RTSP_PLAY_LIVE,                 // generic live
    RTSP_PLAY_AUDIO_BACKCHANNEL,    // audio backchannel only
    RTSP_PLAY_PB_SUNAPI_CAMERA,     // sd card playback support utc time
    RTSP_PLAY_PB_SUNAPI_CAMERA_LC,  // legacy sd card playback not support utc time
    RTSP_PLAY_PB_SUNAPI_NVR,        // wisenet nvr
    RTSP_PLAY_PB_ONVIF,             // onvif playback
    RTSP_PLAY_PB_SSM                // ssm, but not support now
};


struct RtspInfoBase
{
    std::string rtspUrl;
};

struct RtspOpenRequest : RtspInfoBase
{
    TransportMethod     transferMethod;
    RtspDeviceType      deviceType;
    std::string         user;
    std::string         password;
};


struct RtspOpenResponse : RtspInfoBase
{

};


struct RtspEventNotification : RtspInfoBase
{

};

typedef std::shared_ptr<RtspOpenRequest>        RtspOpenRequestSharedPtr;
typedef std::shared_ptr<RtspOpenResponse>       RtspOpenResponseSharedPtr;
typedef std::shared_ptr<RtspEventNotification>  RtspEventNotificationSharedPtr;

typedef std::function <void(const RtspOpenResponseSharedPtr&)>      RtspOpenResponseHandler;
typedef std::function <void(const RtspEventNotificationSharedPtr&)> RtspEventNotificationHandler;

}
}
