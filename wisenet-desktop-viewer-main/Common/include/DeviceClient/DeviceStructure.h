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

#include <memory>
#include <string>
#include <functional>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include "BaseStructure.h"
#include "Media/MediaSourceFrame.h"

namespace Wisenet
{
namespace Device
{
constexpr char LIVE_PROFILE_RECORER[] = "Live";
constexpr char RECORD_PROFILE_RECORER[] = "Record";
constexpr char NETWORK_PROFILE_RECORER[] = "Remote";
constexpr char LIVE4NVR_PROFILE[] = "Live4NVR";
constexpr char LOW4SSM_PROFILE[] = "Low4SSM";
constexpr char WAVEPRIMARY_PROFILE[] = "WAVEPrimary";
constexpr char WAVESECONDARY_PROFILE[] = "WAVESecondary";
constexpr char MOBILE_PROFILE[] = "MOBILE";

enum SwingActionType
{
    Pan = 0,
    Tilt,
    PanTilt,
    Stop,
};

enum class DeviceType
{
    Unknown = 0,
    SunapiCamera,
    SunapiRecorder,
    SunapiEncoder
};

enum class ChannelStatusType
{
    Disconnected = 0,   // NVR channel off, videoloss
    Unauthorized,       // unauthorized
    Connected,          // normal
    Deactivated,        // deactive, covert1, covert2
    Recording           // connected and recording */
};

enum class DeviceStatusType
{
    DisconnectedByService = 0,  // Ready or disconnected by DeviceDisconnect() command
    DisconnectedWithError,      // Device disconnected abnormally
    DisconnectedWithRestriction, // Device disconnected by connection restriction of the Relay Mode.
    Unauthorized,               // Device disconnected abnormally with authorization
    Connected,                  // Device connected
    ConnectedAndInfoChanged,
};

enum class DeviceProtocolType
{
    SunapiGeneric = 0
};

enum class MediaProtocolType
{
    RTP_RTSP = 0,
    RTP_UDP,
    RTSP_HTTP,
    RTSP_MULTICAST,
    UNKNOWN,
};

enum class ConnectionType
{
    StaticIPv4 = 0,
    WisenetDDNS,
    HttpUrl, // http:// or https://url:448
    StaticIPv6,
    S1_DDNS,
    S1_SIP,
};

enum class ConnectedType
{
    StaticIPv4 = 0,
    WisenetDDNS,
    HttpUrl, // http:// or https://url:448
    P2P,
    P2P_RELAY,
    StaticIPv6,
    S1_DDNS,
    S1_SIP,
    S1_SIP_RELAY,
};

enum class RecordingType : unsigned int
{
    None = 0x00000000,
    Normal = 0x00000001,
    Event = 0x00000002,
    AlarmInput = 0x00000004,
    VideoAnalysis = 0x00000008,
    MotionDetection = 0x00000010,
    NetworkDisconnect = 0x00000020,
    FaceDetection = 0x00000040,
    TamperingDetection = 0x00000080,
    AudioDetection = 0x00000100,
    Tracking = 0x00000200,
    Manual = 0x00000400,
    UserInput = 0x00000800,
    DefocusDetection = 0x00001000,
    FogDetection = 0x00002000,
    AudioAnalysis = 0x00004000,
    QueueEvent = 0x00008000,
    videoloss = 0x00010000,
    EmergencyTrigger = 0x00020000,
    InternalHDDWarmup = 0x00040000,
    GSensorEvent = 0x00080000,
    ShockDetection = 0x00100000,
    TemperatureChangeDetection = 0x00200000,
    BoxTemperatureDetection = 0x00400000,
    ObjectDetectionVehicle = 0x00800000,
    ObjectDetectionFace = 0x01000000,
    ObjectDetectionPerson = 0x02000000,
    FaceRecognition = 0x04000000,
    ObjectDetectionLicensePlate = 0x08000000,
    All = 0xFFFFFFFF,
};

inline RecordingType operator |(RecordingType a, RecordingType b)
{
    return static_cast<RecordingType>(static_cast<unsigned int>(a) | static_cast<unsigned int>(b));
}

inline RecordingType operator &(RecordingType a, RecordingType b)
{
    return static_cast<RecordingType>(static_cast<unsigned int>(a) & static_cast<unsigned int>(b));
}

inline RecordingType& operator |=(RecordingType& a, RecordingType b)
{
    return a= a |b;
}

inline RecordingType convertRecordingType(std::string strType)
{
    if(strType == "Normal") return RecordingType::Normal;
    else if(strType == "AlarmInput") return RecordingType::AlarmInput;
    else if(strType == "VideoAnalysis") return RecordingType::VideoAnalysis;
    else if(strType == "MotionDetection") return RecordingType::MotionDetection;
    else if(strType == "NetworkDisconnect") return RecordingType::NetworkDisconnect;
    else if(strType == "FaceDetection") return RecordingType::FaceDetection;
    else if(strType == "TamperingDetection") return RecordingType::TamperingDetection;
    else if(strType == "AudioDetection") return RecordingType::AudioDetection;
    else if(strType == "Tracking") return RecordingType::Tracking;
    else if(strType == "Manual") return RecordingType::Manual;
    else if(strType == "UserInput") return RecordingType::UserInput;
    else if(strType == "DefocusDetection") return RecordingType::DefocusDetection;
    else if(strType == "FogDetection") return RecordingType::FogDetection;
    else if(strType == "AudioAnalysis") return RecordingType::AudioAnalysis;
    else if(strType == "QueueEvent") return RecordingType::QueueEvent;
    else if(strType == "videoloss") return RecordingType::videoloss;
    else if(strType == "EmergencyTrigger") return RecordingType::EmergencyTrigger;
    else if(strType == "InternalHDDWarmup") return RecordingType::InternalHDDWarmup;
    else if(strType == "GSensorEvent") return RecordingType::GSensorEvent;
    else if(strType == "ShockDetection") return RecordingType::ShockDetection;
    else if(strType == "TemperatureChangeDetection") return RecordingType::TemperatureChangeDetection;
    else if(strType == "BoxTemperatureDetection") return RecordingType::BoxTemperatureDetection;
    else if(strType == "ObjectDetection.Vehicle") return RecordingType::ObjectDetectionVehicle;
    else if(strType == "ObjectDetection.Face") return RecordingType::ObjectDetectionFace;
    else if(strType == "ObjectDetection.Person") return RecordingType::ObjectDetectionPerson;
    else if(strType == "FaceRecognition") return RecordingType::FaceRecognition;
    else if(strType == "ObjectDetection.LicensePlate") return RecordingType::ObjectDetectionLicensePlate;

    else return RecordingType::None;
}

struct DeviceConnectInfo
{
    DeviceProtocolType deviceProtocolType = DeviceProtocolType::SunapiGeneric;
    MediaProtocolType mediaProtocolType = MediaProtocolType::RTP_RTSP;
    ConnectionType connectionType = ConnectionType::StaticIPv4;
    ConnectedType connectedType = ConnectedType::StaticIPv4;
    std::string host; // WisenetDDNS or httpUrl or ipv4
    std::string ip;   // 실제 접속에 사용할 IP주소
    unsigned short port = 80;
    std::string user;
    std::string password;
    bool isSSL = false;
    unsigned short sslPort = 443;
    unsigned short rtspPort = 554;
    bool use = true;
    std::string productP2PID;
    bool usageDtlsMode = true;
    std::string mac; // for S1
    int registrationMsec = 0;
};

struct DeviceConnectForceInfo
{
    // Login fail device register start
    bool forceRegister = false;
    std::string modelName;
    std::string macAddress;
    DeviceType deviceType;
    // Login fail device register End
};

struct Resolution
{
    int width = 0;
    int height = 0;
};

struct Device
{
    struct Channel
    {
        struct ChannelInput
        {
            uuid_string deviceID;
            std::string channelID;
            std::string inputID;
        };

        struct MediaCapability
        {
            bool live = false;
            bool playback = false;
            bool backup = false;
            bool audioIn = false;
            bool audioOut = false;
            bool recordingSearch = false;
            bool recordingDualTrack = false;
            bool personSearch = false;
            bool faceSearch = false;
            bool vehicleSearch = false;
            bool ocrSearch = false;
            bool smartSearch = false;
            bool smartSearchAiFilter = false;
        };

        struct PtzCapability
        {
            bool continousPanTilt = false;
            bool continousZoom = false;
            bool continous3Axis = false;
            bool continousFocus = false;
            bool continousIris = false;
            bool home = false;
            bool areaZoom = false;
            bool autoTracking = false;
            bool preset = false;
            bool swing = false;
            bool group = false;
            bool tour = false;
            bool trace = false;
            std::vector<std::string> auxCommands;
        };

        struct ImageCapability
        {
            bool simpleFocus = false;
            bool resetFocus = false;
            bool autoFocus = false;
            bool defog = false;
        };
        struct FisheyeSettings
        {
            bool fisheyeEnable = false;
            FisheyeLensLocation fisheyeLensLocation = FisheyeLensLocation::Ceiling;
            std::string fisheyeLensType;
        };

        uuid_string deviceID;
        std::string channelID;
        std::string name; // DB name
        std::string channelName;
        std::string highProfile;
        std::string lowProfile;
        std::string recordingProfile;
        bool use = false;
        bool ptzEnable = false;
        FisheyeSettings fisheyeSettings;

        std::string  channelModelName;
        std::string ip; // ch IP
        std::string channelCurVersion = "-"; // ch current FW Version
        bool isServerUpgradeEnabled = false;

        MediaCapability     mediaCapabilities;
        PtzCapability       ptzCapabilities;
        ImageCapability     imageCapabilities;

        Resolution highResolution;
        Resolution lowResolution;
        Resolution recordingResolution;

        int highFramerate = 0;
        int lowFramerate = 0;
        int recordingFramerate = 0;

        Wisenet::Media::VideoCodecType highCodecType = Wisenet::Media::VideoCodecType::UNKNOWN;
        Wisenet::Media::VideoCodecType lowCodecType = Wisenet::Media::VideoCodecType::UNKNOWN;
        Wisenet::Media::VideoCodecType recordingCodecType = Wisenet::Media::VideoCodecType::UNKNOWN;

        std::map<std::string, ChannelInput>  channelInputs;
        bool hasDBInfo = false;
    };

    struct DeviceInput
    {
        uuid_string deviceID;
        std::string deviceInputID;
    };

    struct DeviceOutput
    {
        enum class OutputType
        {
            AlarmOut = 0,
            Beep
        };

        uuid_string deviceID;
        std::string deviceOutputID;
        OutputType  outputType = OutputType::AlarmOut;
    };

    struct PosConfiguration
    {
        uuid_string deviceID;
        std::string posID;
        std::string posName;
        bool enable = false;
        std::string encodingType;
        std::vector<std::string> channelIDList;
    };

    struct DeviceCapability
    {
        bool configBackup = false;
        bool configBackupEncrypt = false;
        bool configRestore = false;
        bool configRestoreDecrypt = false;
        bool firmwareUpdate = false;
        bool changePassword = false;
        bool recordingManual = false;
        bool recordingSearch = false;
        bool recordingSearchPeriod = false;
        bool recordingDualTrack = false;
        bool playback = false;
        bool playbackSync = false;
        bool playbackChannel = false;
        bool thumbnail = false;
        bool quickPlay = false;
        std::vector<float> playbackSpeeds;
        std::vector<std::string> eventNames;
        std::vector<RecordingType> recordingTypes;
    };

    struct DeviceTimeZone{
        int utcTimeBias = 0;
        int dstBias = -60;
        bool useDst = false;
        int dstStartMonth = 0;
        int dstStartDay = 0;
        int dstStartHour = 0;
        int dstStartMinute = 0;
        int dstEndMonth = 0;
        int dstEndDay = 0;
        int dstEndHour = 0;
        int dstEndMinute = 0;
    };

    typedef std::pair<uuid_string, Device::Channel> ChannelPair;

    uuid_string deviceID;
    DeviceConnectInfo connectionInfo;
    std::string modelName;
    std::string name; // DB name
    std::string deviceName;
    std::string macAddress;
    std::string firmwareVersion;
    DeviceType  deviceType = DeviceType::Unknown;
    std::string deviceCertifcateResult;
    bool supportWebSocket = false;

    DeviceTimeZone                          deviceTimeZone;
    DeviceCapability                        deviceCapabilities;
    std::map<std::string, Channel>          channels;
    std::map<std::string, DeviceInput>      deviceInputs;
    std::map<std::string, DeviceOutput>     deviceOutputs;
    std::map<std::string, PosConfiguration> posConfigurations;

    static bool channelComp(const ChannelPair& a, const ChannelPair& b)
    {
        int ak = -1;
        int bk = -1;
        try {
            ak = std::stoi(a.first);
            bk = std::stoi(b.first);
        }
        catch(const std::exception& ex){
            (void)ex;
        }
        return ak < bk;
    }

    std::vector<ChannelPair> ToOrderedChannels() const
    {
        std::vector<ChannelPair> vec(channels.begin(), channels.end());
        std::sort(vec.begin(), vec.end(), Device::channelComp);
        return vec;
    }

    std::vector<std::string>            dynamicEvents;

    bool hasDBInfo = false;
};



struct ChannelStatus
{
    uuid_string         deviceID;
    std::string         channelID;
    ChannelStatusType   status = ChannelStatusType::Disconnected;

    /*
    채널 상태가 Connected or Recording 상태로 변경된 경우에는 채널 정보가 함께 올라온다
    ChannelStatusEvent 에서만 유효함
    DeviceStatusEvent에서는 포함하지 않음.
    */
    boost::optional<Device::Channel>    channel;

    bool isGood() const
    {
        return (status == ChannelStatusType::Connected ||
                status == ChannelStatusType::Recording);
    }
};

struct DeviceStatus
{
    uuid_string                 deviceID;
    DeviceStatusType            status = DeviceStatusType::DisconnectedByService;
    std::map<std::string, ChannelStatus> channelsStatus;
    bool isGood() const
    {
        return (status == DeviceStatusType::Connected || status == DeviceStatusType::ConnectedAndInfoChanged);
    }
};


struct VideoProfile
{
    uuid_string deviceID;
    std::string channelID;
    std::string profileID;
    std::string profileName;
    Wisenet::Media::VideoCodecType codecType = Wisenet::Media::VideoCodecType::UNKNOWN;
    Resolution resolution;
    int framerate = 0;
    int bitrate = 0;
    int compressionLevel = 0;

    // H.264, H.265
    int h26xGovLength = 0;
    std::string h26xProfile;
    std::string h26xBitrateControlType;

    // for recorder Only 
    std::string aliasID; // if camera, same to profileID
    std::string aliasName; // if camera, same to profileName
};

struct DeviceProfileInfo
{
    uuid_string deviceID;
    // first key :: channelID
    // second key :: profileID
    std::map<std::string, std::map<std::string, VideoProfile>> videoProfiles;
};


struct DevcieSessionKey
{
    std::string sessionKey = "";        // get from device
    uuid_string playbackSessionId = ""; // get from client

    DevcieSessionKey(std::string key, uuid_string id)
    {
        sessionKey = key;
        playbackSessionId = id;
    }
};


enum class StreamType
{
    none = 0,
    liveHigh,
    liveLow,
    playbackHigh,
    playbackLow,
    backupHigh,
    backupLow,
    posLive,
    posPlayback,
    posBackup
};

enum class ControlType
{
    pause,
    play,
    stepForward,
    stepBackward,
};

struct MediaRequestInfo
{
    StreamType streamType = StreamType::none;
    std::string profileId;
    Wisenet::Media::MediaSourceFrameHandler streamHandler = nullptr;
    int64_t startTime = 0;
    int64_t endTime = 0;
    int trackId = -1;
    std::string playbackSessionId;
    float speed = 1.0f;
    bool isPausedOpen = false;

    bool isLive()
    {
        return (streamType == StreamType::liveHigh || streamType == StreamType::liveLow);
    }
    bool isPlayback()
    {
        return (streamType == StreamType::playbackHigh || streamType == StreamType::playbackLow);
    }
    bool isBackup()
    {
        return (streamType == StreamType::backupHigh || streamType == StreamType::backupLow);
    }
    bool isPosLive()
    {
        return (streamType == StreamType::posLive);
    }
    bool isPosPlayback()
    {
        return (streamType == StreamType::posPlayback);
    }
    bool isPosBackup()
    {
        return (streamType == StreamType::posBackup);
    }
};

struct MediaControlInfo
{
    ControlType controlType = ControlType::pause;
    float speed = 0;
    int64_t time = 0;
};

struct RecordingOverlappedViewResult
{
    std::vector<int> overlappedIDList;
    std::unordered_map<std::string, std::vector<int>> channelOverlappedIDList; /* MULTI DIRECTIONAL CAMERA ONLY */
};

struct RecordingOverlappedViewParam
{
    std::vector<std::string> ChannelIDList; /* Channel ID list */

    int64_t FromDate = 0;           /* The start date and time for when the recording occurred (UTC Msec)
                                       Must be sent for the view action. */

    int64_t ToDate = 0;             /* The end date and time for when the recording occurred (UTC Msec)
                                       Must be sent for the view action. */
};

struct MonthResultArray
{
    bool dayRecordExist[31] = {false};
};

struct RecordingCalendarSearchViewResult
{
    MonthResultArray result;    /* This parameter provides a response only when IgnoreChannelBasedResults is set to True in the request. (NVR only) */
    std::unordered_map<std::string, MonthResultArray> channelReslut;    /* key : channelId, value:result */
};

struct RecordingCalendarSearchViewParam
{
    int64_t Month = 0;                          /* Target month for searching. (UTC Msec).
                                                   Month must be sent together with the view action. */

    std::vector<std::string> ChannelIDList;     /* List of channels in which recordings to be searched. */

    std::string PrimaryDeviceIPAddress = "";    /* IP address of primary device to which recording to be searched.
                                                   Applicable only if FailOver feature is supported. (NVR only) */

    bool IgnoreChannelBasedResults = false;     /* If true, consolidated results will be given for all channels (NVR only) */
};

struct ChannelTimeline
{
    uint64_t startTime = 0;
    uint64_t endTime = 0;
    RecordingType type = RecordingType::None;
};

struct OneDayTimeline {
    std::unordered_map<int, std::vector<ChannelTimeline>> timelineList;     // key: Overlapped ID, value: Timeline List
    std::unordered_map<int, ChannelTimeline> recordingPeriodMap;            // key: Overlapped ID, value: RecordingPeriod
};

struct RecordingTimelineViewResult
{
    // Primary key : Channel ID
    // Second key : Overlapped ID
    // value : Timeline List
    std::unordered_map<std::string, std::unordered_map<int, std::vector<ChannelTimeline>>> channelTimelineList;

    // for Oneday Search
    // Primary key : Channel ID
    // Second key : Date (MSec)
    // value : oneday Timeline
    std::unordered_map<std::string, std::map<int64_t, OneDayTimeline>> onedayTimelineList;
};

struct RecordingTimelineViewParam
{
    //RecordingType Type = RecordingType::None; // Type은 항상 All로 처리
    int64_t FromDate = 0;   /* must be sent */
    int64_t ToDate = 0;     /* must be sent */
    int OverlappedID = -1;
    std::vector<std::string> ChannelIDList;     /* must be sent */
    std::string PrimaryDeviceIPAddress = "";
};

struct RecordingSearchRecordingPeriodViewResult
{
    int64_t startTime = 0;
    int64_t endTime = 0;
};

enum class AiClassType
{
    person = 0,
    vehicle,
    face,
    ocr
};

struct BoundingBox{
    float left = 0;
    float top = 0;
    float right = 0;
    float bottom = 0;
};

struct AiBaseAttribute{
    uuid_string deviceID;
    bool isSuccess = false;
    int index = 0;
    int channelId = 0;
    int64_t dateTime = 0;
    std::string imageUrl;
    int imageWidth = 0;
    int imageHeight = 0;
};

struct MetaAttribute{
    AiBaseAttribute aiBaseAttribute;
    AiClassType attributeType = AiClassType::person;
    int objectId = 0;

    std::vector<std::string> personGender; // Any, Male, Female
    std::vector<std::string> personClothingTopsColor;   //Any, Black, Gray, White, Red, Orange, Yellow, Green, Blue, Purple
    std::vector<std::string> personClothingBottomsColor;
    std::vector<std::string> personBelongingBag;    //Any, Wear, No

    std::vector<std::string> faceGender; // Any, Male, Female
    std::vector<std::string> faceAgeType; // Any, Young, Adult, Middle, Senior
    std::vector<std::string> faceHat; //Any, Wear, No
    std::vector<std::string> faceOpticals; //Any, Wear, No
    std::vector<std::string> faceMask; //Any, Wear, No

    std::vector<std::string> vehicleType; // Any, Car, Bus, Truck, Motorcycle, Bicycle
    std::vector<std::string> vehicleColor;//Any, Black, Gray, White, Red, Orange, Yellow, Green, Blue, Purple
    std::string ocrText;
};

struct MetaFilter{
    std::set<std::string> personGender; // Any, Male, Female
    std::set<std::string> personClothingTopsColor;   //Any, Black, Gray, White, Red, Orange, Yellow, Green, Blue, Purple
    std::set<std::string> personClothingBottomsColor;
    std::set<std::string> personBelongingBag;    //Any, Wear, No

    std::set<std::string> faceGender; // Any, Male, Female
    std::set<std::string> faceAgeType; // Any, Young, Adult, Middle, Senior
    std::set<std::string> faceHat; //Any, Wear, No
    std::set<std::string> faceOpticals; //Any, Wear, No
    std::set<std::string> faceMask; //Any, Wear, No

    std::set<std::string> vehicleType; // Any, Car, Bus, Truck, Motorcycle, Bicycle
    std::set<std::string> vehicleColor;//Any, Black, Gray, White, Red, Orange, Yellow, Green, Blue, Purple

    std::string ocrText;
};


struct UrlSnapShot
{
    bool isSuccess = false;
    std::string url;
    std::vector<unsigned char> snapshotData;
};

enum class SmartSearchAreaEventType
{
    motion = 0,
    enter,
    exit,
};

enum class SmartSearchAIType
{
    person = 0,
    vehicle,
    unknown,
};

enum class SmartSearchLineEventType
{
    bothDirection = 0,
    leftDirection ,
    rightDirection,
};

enum class SmartSearchAreaType{     //only area search
    inside = 0,
    outside,
};

enum class SmartSearchResultType{
    motion = 0, // Area
    enter,
    exit,
    pass,       // Line
};

struct SmartSearchLine{
    int index=1;  //base 1
    SmartSearchLineEventType eventType = SmartSearchLineEventType::bothDirection;
    std::vector<float> coordinates; // 2 points(line)
};

struct SmartSearchArea{
    int index=1;  //base 1
    std::vector<SmartSearchAreaEventType> eventType;
    std::vector<SmartSearchAIType> aiType;
    SmartSearchAreaType areaType = SmartSearchAreaType::inside;
    std::vector<float> coordinates; // 4 points(area)
};

struct SmartSearchResult{
    std::string channel;
    int index = 0;
    int64_t dateTime = 0;
    SmartSearchResultType type = SmartSearchResultType::motion;
};

// 2022.08.29. added
struct TextSearchDataResult {
    std::string uuid;
    std::string date;                                           /* Result date */
    std::string playTime;                                       /* Result play time */
    int deviceID = 0;                                               /* POS ID */
    std::string textData;                                       /* POS receipt */
    std::string keywordsMatched;                                /* Keywords found in POS receipt */
    std::string channelIDList;                                  /* Result channel ID List */
};

// 2022.09.01. added
struct TextPosConfResult {
    std::string deviceId = "";
    std::string deviceName = "";
    bool enable = false;
    std::string encodingType = "";
    std::string channelIDList = "";
};

struct UpdatingDevice {
    UpdatingDevice(std::string fwPath)
        : firmwarePath(fwPath)
    {}
    std::string firmwarePath;
};

struct UpdatingChannel {
    UpdatingChannel(std::string chID, std::string fwPath)
        : channelID(chID), firmwarePath(fwPath)
    {}
    std::string channelID;
    std::string firmwarePath;
};

struct UpdateStatus {
    UpdateStatus(std::string chID, std::string stat)
        : channelID(chID), status(stat)
    {}
    std::string channelID;
    std::string status; // NONE, UPGRADING, SUCCESS/FAIL
};

}
}
