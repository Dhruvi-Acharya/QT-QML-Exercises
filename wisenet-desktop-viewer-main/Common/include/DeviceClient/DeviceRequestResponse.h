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

#include <boost/optional.hpp>
#include <memory>
#include <string>
#include <functional>
#include <map>

#include "DeviceClient/DeviceStructure.h"

namespace Wisenet
{
namespace Device
{


// All Device request structures should be derived from RequestBase
struct DeviceRequestBase : RequestBase
{
    uuid_string deviceID; // 00000000-0000-0000-0000-000000000000 (36 characters UUID)
};
typedef std::shared_ptr<DeviceRequestBase> DeviceRequestBaseSharedPtr;

// All Channel of a Device request structures should be derived from DeviceRequestBase
struct ChannelRequestBase : DeviceRequestBase
{
    std::string channelID; // starts with 1 ~ for SUNAPI Device
};
typedef std::shared_ptr<ChannelRequestBase> ChannelRequestBaseSharedPtr;

//==========> DeviceConnect()
struct DeviceConnectRequest : DeviceRequestBase
{
    bool addDevice = false;
    bool tryToDdnsInDDNSAndP2P = true;
    DeviceConnectInfo connectionInfo;
};
typedef std::shared_ptr<DeviceConnectRequest> DeviceConnectRequestSharedPtr;

// used only internally in CoreService and DeviceClient
// ResponseBase used in CoreServiceResponse
struct DeviceConnectResponse : ResponseBase
{
    Device               device;
    DeviceStatus         deviceStatus;
    DeviceProfileInfo    deviceProfileInfo;
};
typedef std::shared_ptr<DeviceConnectResponse> DeviceConnectResponseSharedPtr;

//==========> DeviceDisconnect()
struct DeviceDisconnectRequest : DeviceRequestBase
{
};
typedef std::shared_ptr<DeviceDisconnectRequest> DeviceDisconnectRequestSharedPtr;


//==========> DeviceMediaOpen()
struct DeviceMediaOpenRequest : ChannelRequestBase
{
    uuid_string mediaID;
    MediaRequestInfo mediaRequestInfo;
};
typedef std::shared_ptr<DeviceMediaOpenRequest> DeviceMediaOpenRequestSharedPtr;

//==========> DeviceMediaClose()
struct DeviceMediaCloseRequest : ChannelRequestBase
{
    uuid_string mediaID;
};
typedef std::shared_ptr<DeviceMediaCloseRequest> DeviceMediaCloseRequestSharedPtr;

//==========> DeviceMediaControl()
struct DeviceMediaControlRequest : ChannelRequestBase
{
    uuid_string mediaID;
    MediaControlInfo mediaControlInfo;
};
typedef std::shared_ptr<DeviceMediaControlRequest> DeviceMediaControlRequestSharedPtr;

//==========> DeviceTalk()
struct DeviceTalkRequest : ChannelRequestBase
{
    bool talkStart = false;
};
typedef std::shared_ptr<DeviceTalkRequest> DeviceTalkRequestSharedPtr;

//==========> DeviceSendTalkData()
struct DeviceSendTalkDataRequest : ChannelRequestBase
{
    Media::AudioSourceFrameSharedPtr audioFrame;
};
typedef std::shared_ptr<DeviceSendTalkDataRequest> DeviceSendTalkDataRequestSharedPtr;

//==========> DeviceGetOverlappedId()
struct DeviceGetOverlappedIdRequest : DeviceRequestBase
{
    RecordingOverlappedViewParam param;
};
typedef std::shared_ptr<DeviceGetOverlappedIdRequest> DeviceGetOverlappedIdRequestSharedPtr;

struct DeviceGetOverlappedIdResponse : ResponseBase
{
    RecordingOverlappedViewResult result;
};
typedef std::shared_ptr<DeviceGetOverlappedIdResponse> DeviceGetOverlappedIdResponseSharedPtr;

//==========> DeviceCalendarSearch()
struct DeviceCalendarSearchRequest : DeviceRequestBase
{
    RecordingCalendarSearchViewParam param;
};
typedef std::shared_ptr<DeviceCalendarSearchRequest> DeviceCalendarSearchRequestSharedPtr;

struct DeviceCalendarSearchResponse : ResponseBase
{
    RecordingCalendarSearchViewResult result;
};
typedef std::shared_ptr<DeviceCalendarSearchResponse> DeviceCalendarSearchResponseSharedPtr;

//==========> DeviceGetTimeline()
struct DeviceGetTimelineRequest : DeviceRequestBase
{
    RecordingTimelineViewParam param;
};
typedef std::shared_ptr<DeviceGetTimelineRequest> DeviceGetTimelineRequestSharedPtr;

struct DeviceGetTimelineResponse : ResponseBase
{
    RecordingTimelineViewResult result;
};
typedef std::shared_ptr<DeviceGetTimelineResponse> DeviceGetTimelineResponseSharedPtr;

//==========> DeviceGetRecordingPeriod()
struct DeviceGetRecordingPeriodRequest : DeviceRequestBase
{
};
typedef std::shared_ptr<DeviceGetRecordingPeriodRequest> DeviceGetRecordingPeriodRequestSharedPtr;

struct DeviceGetRecordingPeriodResponse : ResponseBase
{
    RecordingSearchRecordingPeriodViewResult result;
};
typedef std::shared_ptr<DeviceGetRecordingPeriodResponse> DeviceGetRecordingPeriodResponseSharedPtr;

//==========> DeviceGetAllPeriodTimeline()
struct DeviceGetAllPeriodTimelineRequest : DeviceRequestBase
{
    std::vector<std::string> ChannelIDList; /* Channel ID list */
    int64_t Month = 0;  /* RecordingPeriod 미지원 장비의 target month (UTC Msec) */
    int Period = 0; // target month 포함 최대 검색 범위 (단위 : month)
};
typedef std::shared_ptr<DeviceGetAllPeriodTimelineRequest> DeviceGetAllPeriodTimelineRequestSharedPtr;

struct DeviceGetAllPeriodTimelineResponse : ResponseBase
{
    RecordingSearchRecordingPeriodViewResult recordingPeriod;
    RecordingOverlappedViewResult overlappedId;
    RecordingTimelineViewResult timeline;
    std::unordered_map<std::string, std::vector<int>> channelOverlappedIdList;  // 채널 별 OverlappedId List (순서대로 최신 Track)
};
typedef std::shared_ptr<DeviceGetAllPeriodTimelineResponse> DeviceGetAllPeriodTimelineResponseSharedPtr;

//==========> DeviceChangeUserPassword()

// IIDeviceClient::DeviceChangeUserPassword()
struct DeviceChangeUserPasswordRequest : DeviceRequestBase
{
    std::string password; // device user password of admin Sunapi CGI Spec : MaxLength = 15
};
typedef std::shared_ptr<DeviceChangeUserPasswordRequest> DeviceChangeUserPasswordRequestSharedPtr;

//==========> DeviceSnapshot()
struct DeviceSnapshotRequest : ChannelRequestBase
{

};
typedef std::shared_ptr<DeviceSnapshotRequest> DeviceSnapshotRequestSharedPtr;

struct DeviceSnapshotResponse : ResponseBase
{
    std::string imageType = "image/jpeg";
    std::vector<char> snapshotData;
};
typedef std::shared_ptr<DeviceSnapshotResponse> DeviceSnapshotResponseSharedPtr;

//==========> DeviceThumbnail()
struct DeviceThumbnailRequest : ChannelRequestBase
{
    int64_t fromDate;
    int64_t toDate;
    int width;
    int height;
    int column;
    int row;
    int overlappedID;
};
typedef std::shared_ptr<DeviceThumbnailRequest> DeviceThumbnailRequestSharedPtr;

struct DeviceThumbnailResponse : ResponseBase
{
    std::vector<unsigned char> thumbnailData;
};
typedef std::shared_ptr<DeviceThumbnailResponse> DeviceThumbnailResponseSharedPtr;

//==========> DeviceConfigBackup()
struct DeviceConfigBackupRequest : DeviceRequestBase
{
    std::string password = "";
    std::string backupFilePath;
};
typedef std::shared_ptr<DeviceConfigBackupRequest> DeviceConfigBackupRequestSharedPtr;

//==========> DeviceConfigRestore()
struct DeviceConfigRestoreRequest : DeviceRequestBase
{
    bool excludeNetworkSettings = true;
    std::string password = "";
    std::string restoreFilePath;
};
typedef std::shared_ptr<DeviceConfigRestoreRequest> DeviceConfigRestoreRequestSharedPtr;

//==========> DeviceFirmwareUpdate()
struct DeviceFirmwareUpdateRequest : DeviceRequestBase
{
    std::string firmwareFilePath;
    bool firmwareStop = false;
};
typedef std::shared_ptr<DeviceFirmwareUpdateRequest> DeviceFirmwareUpdateRequestSharedPtr;

struct DeviceFirmwareUpdateResponse : ResponseBase
{
    enum class ProgressStatus
    {
        Uploading = 0,
        UploadingAck,
        Updating,
    };

    ProgressStatus progressStatus = ProgressStatus::Uploading;
    int progressPercent = 0;    // 0 ~ 100%
};
typedef std::shared_ptr<DeviceFirmwareUpdateResponse> DeviceFirmwareUpdateResponseSharedPtr;

//==========> DeviceChannelFirmwareUpdate()
struct DeviceChannelFirmwareUpdateRequest : DeviceRequestBase
{
    std::vector<UpdatingChannel> firmwareUpdatingList; // <channelID, FW path>
    bool firmwareStop = false;
};
typedef std::shared_ptr<DeviceChannelFirmwareUpdateRequest> DeviceChannelFirmwareUpdateRequestSharedPtr;

struct DeviceChannelFirmwareUpdateResponse : ResponseBase
{
    std::string status; // NONE, UPGRADING, SUCCESS/FAIL
    int percent = 0;
};
typedef std::shared_ptr<DeviceChannelFirmwareUpdateResponse> DeviceChannelFirmwareUpdateResponseSharedPtr;

//==========> GetDeviceUpgradeToken()
typedef std::shared_ptr<DeviceRequestBase> GetDeviceUpgradeTokenRequestSharedPtr;
struct GetDeviceUpgradeTokenResponse : ResponseBase
{
    std::string upgradeToken;
};
typedef std::shared_ptr<GetDeviceUpgradeTokenResponse> GetDeviceUpgradeTokenResponseSharedPtr;

//==========> DeviceUpgradeStatus()
struct DeviceUpgradeStatusRequest : DeviceRequestBase
{
    std::vector<UpdatingChannel> firmwareUpdatingList;
};
typedef std::shared_ptr<DeviceUpgradeStatusRequest> DeviceUpgradeStatusRequestSharedPtr;
struct DeviceUpgradeStatusResponse : ResponseBase
{
    std::vector<UpdateStatus> updateStatusList;
    std::string recorderUpgradeStatus;
};
typedef std::shared_ptr<DeviceUpgradeStatusResponse> DeviceUpgradeStatusResponseSharedPtr;

//==========> DevicePTZContinuous()
enum class ViewModeType
{
    Unknown = 0,
    Panorama,
    DoublePanorama
};

struct DevicePTZContinuousRequest : ChannelRequestBase
{
    int pan = 0;
    int tilt = 0;
    int zoom = 0;
    ViewModeType viewModeType = ViewModeType::Unknown;
    int subViewIndex = 0;
};
typedef std::shared_ptr<DevicePTZContinuousRequest> DevicePTZContinuousRequestSharedPtr;

//==========> DevicePTZFocus()
struct DevicePTZFocusRequest : ChannelRequestBase
{
    enum class Focus
    {
        Unknown = 0,
        Near,
        Far,
        Stop
    };
    Focus focus = Focus::Unknown;
};
typedef std::shared_ptr<DevicePTZFocusRequest> DevicePTZFocusRequestSharedPtr;

//==========> DevicePTZStop()
struct DevicePTZStopRequest : ChannelRequestBase
{
    ViewModeType viewModeType = ViewModeType::Unknown;
    int subViewIndex = 0;
};
typedef std::shared_ptr<DevicePTZStopRequest> DevicePTZStopRequestSharedPtr;

//==========> DeviceImageFocusMode()
struct DeviceImageFocusModeRequest : ChannelRequestBase
{
    enum class FocusMode
    {
        Unknown = 0,
        SimpleFocus,
        Reset,
        AutoFocus
    };
    FocusMode focusMode = FocusMode::Unknown;
};
typedef std::shared_ptr<DeviceImageFocusModeRequest> DeviceImageFocusModeRequestSharedPtr;

//==========> DeviceGetPreset()
struct DeviceGetPresetRequest : ChannelRequestBase
{
};
typedef std::shared_ptr<DeviceGetPresetRequest> DeviceGetPresetRequestSharedPtr;

struct DeviceGetPresetResponse : ResponseBase
{
    int maxPreset = 0;
    std::map<unsigned int, std::string> presets; // preset list of the device
};
typedef std::shared_ptr<DeviceGetPresetResponse> DeviceGetPresetResponseSharedPtr;

//==========> DeviceAddPreset()
struct DeviceAddPresetRequest : ChannelRequestBase
{
    unsigned int preset = 0;
    std::string name = "";
    int subViewIndex = 0;
};
typedef std::shared_ptr<DeviceAddPresetRequest> DeviceAddPresetRequestSharedPtr;

//==========> DeviceRemovePreset()
struct DeviceRemovePresetRequest : ChannelRequestBase
{
    unsigned int preset = 0;
};
typedef std::shared_ptr<DeviceRemovePresetRequest> DeviceRemovePresetRequestSharedPtr;

//==========> DeviceMovePreset()
struct DeviceMovePresetRequest : ChannelRequestBase
{
    unsigned int preset = 0;
    int subViewIndex = 0;
};
typedef std::shared_ptr<DeviceMovePresetRequest> DeviceMovePresetRequestSharedPtr;

/////// new feature of ptz --START--
//==========> DeviceGetSwing()
struct DeviceGetSwingResponse : ResponseBase
{
    int swingPanStartPreset = 0;
    int swingPanEndPreset = 0;
    int swingTiltStartPreset = 0;
    int swingTiltEndPreset = 0;
    int swingPanTiltStartPreset = 0;
    int swingPanTiltEndPreset = 0;
};
typedef std::shared_ptr<DeviceGetSwingResponse> DeviceGetSwingResponseSharedPtr;

//==========> DeviceMoveStopSwing()
struct DeviceMoveStopSwingRequest : ChannelRequestBase
{
    Wisenet::Device::SwingActionType swingActionType = Wisenet::Device::SwingActionType::Stop;
};
typedef std::shared_ptr<DeviceMoveStopSwingRequest> DeviceMoveStopSwingRequestSharedPtr;

//==========> DeviceGetGroup()
struct DeviceGetGroupResponse : ResponseBase
{
    std::map<int,std::vector<int> > groupPresets; // preset list of group
};
typedef std::shared_ptr<DeviceGetGroupResponse> DeviceGetGroupResponseSharedPtr;

//==========> DeviceMoveGroup()
struct DeviceMoveGroupRequest : ChannelRequestBase
{
    int groupNumber = 1;
};
typedef std::shared_ptr<DeviceMoveGroupRequest> DeviceMoveGroupRequestSharedPtr;

//==========> DeviceStopGroup()
struct DeviceStopGroupRequest : ChannelRequestBase
{
    int groupNumber = 1; // 1~ : group number
};
typedef std::shared_ptr<DeviceStopGroupRequest> DeviceStopGroupRequestSharedPtr;

//==========> DeviceGetTour()
struct DeviceGetTourResponse : ResponseBase
{
    std::vector<int> tourPresets; // preset list of Tour
};
typedef std::shared_ptr<DeviceGetTourResponse> DeviceGetTourResponseSharedPtr;

//==========> DeviceGetTrace()
struct DeviceGetTraceResponse : ResponseBase
{
    std::vector<int> tracePresets; // preset list of Trace
};
typedef std::shared_ptr<DeviceGetTraceResponse> DeviceGetTraceResponseSharedPtr;

//==========> DeviceMoveTrace()
struct DeviceMoveTraceRequest : ChannelRequestBase
{
    int traceNumber = 1; // 1~ : trace number
};
typedef std::shared_ptr<DeviceMoveTraceRequest> DeviceMoveTraceRequestSharedPtr;

//==========> DeviceStopTrace()
struct DeviceStopTraceRequest : ChannelRequestBase
{
    int traceNumber = 1; // 1~ : trace number
};
typedef std::shared_ptr<DeviceStopTraceRequest> DeviceStopTraceRequestSharedPtr;
/////// new feature of ptz --END--

//==========> DevicePTZAuxControl()
struct DevicePTZAuxControlRequest : ChannelRequestBase
{
    std::string command = "";
};
typedef std::shared_ptr<DevicePTZAuxControlRequest> DevicePTZAuxControlRequestSharedPtr;


//==========> DeviceAreaZoom()
struct DeviceAreaZoomRequest : ChannelRequestBase
{
    int profile = 0;
    int x1 = 0;
    int y1 = 0;
    int x2 = 0;
    int y2 = 0;
    int tileWidth = 0;
    int tileHeight = 0;
};
typedef std::shared_ptr<DeviceAreaZoomRequest> DeviceAreaZoomRequestSharedPtr;

//==========> DeviceGetAutoTracking()
struct DeviceGetAutoTrackingRequest : ChannelRequestBase
{
};
typedef std::shared_ptr<DeviceGetAutoTrackingRequest> DeviceGetAutoTrackingRequestSharedPtr;

struct DeviceGetAutoTrackingResponse : ResponseBase
{
    bool enable = false;
};
typedef std::shared_ptr<DeviceGetAutoTrackingResponse> DeviceGetAutoTrackingResponseSharedPtr;

//==========> DeviceSetAutoTracking()
struct DeviceSetAutoTrackingRequest : ChannelRequestBase
{
    bool enable = false;
};
typedef std::shared_ptr<DeviceSetAutoTrackingRequest>DeviceSetAutoTrackingRequestSharedPtr;

//==========> DeviceSetDigitalAutoTracking()
struct DeviceSetDigitalAutoTrackingRequest : ChannelRequestBase
{
    int profile = 0;
    bool enable = false;
};
typedef std::shared_ptr<DeviceSetDigitalAutoTrackingRequest>DeviceSetDigitalAutoTrackingRequestSharedPtr;

//==========> DeviceControlTargetLockCoodinate()
struct DeviceSetTargetLockCoordinateRequest : ChannelRequestBase
{
    unsigned int x = 0;
    unsigned int y = 0;
};
typedef std::shared_ptr<DeviceSetTargetLockCoordinateRequest> DeviceSetTargetLockCoordinateRequestSharedPtr;

//==========> DeviceGetDefog(), DeviceSetDefog()
enum class DefogMode{
    Off = 0,
    Auto,
    Manual
};

//==========> DeviceGetDefog()
struct DeviceGetDefogRequest : ChannelRequestBase
{
};
typedef std::shared_ptr<DeviceGetDefogRequest> DeviceGetDefogRequestSharedPtr;

struct DeviceGetDefogResponse : ResponseBase
{
    DefogMode defogMode = DefogMode::Off;
    int defogLevel = 0;
};
typedef std::shared_ptr<DeviceGetDefogResponse> DeviceGetDefogResponseSharedPtr;

//==========> DeviceSetDefog()
struct DeviceSetDefogRequest : ChannelRequestBase
{
    DefogMode defogMode = DefogMode::Off;
    int defogLevel = 0;
};
typedef std::shared_ptr<DeviceSetDefogRequest> DeviceSetDefogRequestSharedPtr;

//==========> DeviceSetAlarmOutput()

struct DeviceSetAlarmOutputRequest : DeviceRequestBase
{
    bool on = false;
    std::string deviceOutputID;
};
typedef std::shared_ptr<DeviceSetAlarmOutputRequest> DeviceSetAlarmOutputRequestSharedPtr;

//==========> DeviceMetaAttributeSearchRequest()
struct DeviceMetaAttributeSearchRequest : DeviceRequestBase
{
    int64_t fromDate = 0;   /* must be sent */
    int64_t toDate = 0;     /* must be sent */
    AiClassType classType;
    std::string ocrText;
    std::vector<std::string> channelIDList;     /* List of channels in which recordings to be searched. */
};
typedef std::shared_ptr<DeviceMetaAttributeSearchRequest> DeviceMetaAttributeSearchRequestSharedPtr;

struct DeviceMetaAttributeSearchResponse : ResponseBase
{
    uuid_string deviceId;
    int totalCount = 0;
    std::vector<MetaAttribute> results;
};
typedef std::shared_ptr<DeviceMetaAttributeSearchResponse> DeviceMetaAttributeSearchResponseSharedPtr;


//==========> DeviceUrlSnapShotRequest()
struct DeviceUrlSnapShotRequest : DeviceRequestBase
{
    std::vector<std::string> urlList;
};
typedef std::shared_ptr<DeviceUrlSnapShotRequest> DeviceUrlSnapShotRequestSharedPtr;

struct DeviceUrlSnapShotResponse : ResponseBase
{
    std::vector<UrlSnapShot> snapShots;
};
typedef std::shared_ptr<DeviceUrlSnapShotResponse> DeviceUrlSnapShotResponseSharedPtr;

//==========> DeviceSmartSearchRequest()
struct DeviceSmartSearchRequest : DeviceRequestBase
{
    int64_t fromDate = 0;   /* must be sent */
    int64_t toDate = 0;     /* must be sent */
    int OverlappedID = -1;
    std::string channel;
    std::vector<SmartSearchArea> areas;
    std::vector<SmartSearchLine> lines;
};
typedef std::shared_ptr<DeviceSmartSearchRequest> DeviceSmartSearchRequestSharedPtr;

struct DeviceSmartSearchResponse : ResponseBase
{
    int totalCount = 0;
    std::string searchToken;
    std::vector<SmartSearchResult> results;
};
typedef std::shared_ptr<DeviceSmartSearchResponse> DeviceSmartSearchResponseSharedPtr;

//==========> DeviceTextSearchRequest() // 2022.08.26. added
struct DeviceTextSearchRequest : DeviceRequestBase
{
    std::string mode;
    std::string metaDataType;
    std::string deviceIDList;
    std::string overlappedID;
    std::string keyword;
    int64_t fromDate = 0;
    int64_t toDate = 0;
    bool isWholeword = false;
    bool isCaseSensitive = false;
};
typedef std::shared_ptr<DeviceTextSearchRequest> DeviceTextSearchRequestSharedPtr;

struct DeviceTextSearchResponse: ResponseBase
{
    std::string searchToken;
};
typedef std::shared_ptr<DeviceTextSearchResponse> DeviceTextSearchResponseSharedPtr;

//==========> DeviceTextSearchStatusRequest() // 2022.08.29. added
struct DeviceTextSearchStatusRequest : DeviceRequestBase
{
    std::string type;
    std::string searchToken;
};
typedef std::shared_ptr<DeviceTextSearchStatusRequest> DeviceTextSearchStatusRequestSharedPtr;

struct DeviceTextSearchStatusResponse: ResponseBase
{
    std::string status;
    std::string totalCount;
    std::string intervalFrom;
    std::string intervalTo;
    bool timeOut = false;
};
typedef std::shared_ptr<DeviceTextSearchStatusResponse> DeviceTextSearchStatusResponseSharedPtr;

//==========> DeviceTextSearchResultRequest() // 2022.08.29. added
struct DeviceTextSearchResultRequest : DeviceRequestBase
{
    std::string type;
    std::string searchToken;
    std::string resultFromIndex;
    std::string maxResults;
};
typedef std::shared_ptr<DeviceTextSearchResultRequest> DeviceTextSearchResultRequestSharedPtr;

struct DeviceTextSearchResultResponse: ResponseBase
{
    std::vector<TextSearchDataResult> textData;

    std::string searchToken;
    std::string searchTokenExpirytime;

    std::string intervalFrom;
    std::string intervalTo;

    int totalResultsFound = 0;
    int totalCount = 0;
};
typedef std::shared_ptr<DeviceTextSearchResultResponse> DeviceTextSearchResultResponseSharedPtr;

//==========> DeviceTextPosConfRequest() // 2022.09.01. added
struct DeviceTextPosConfRequest : DeviceRequestBase
{
    ;
};
typedef std::shared_ptr<DeviceTextPosConfRequest> DeviceTextPosConfRequestSharedPtr;

struct DeviceTextPosConfResponse: ResponseBase
{
    std::vector<TextPosConfResult> confData;
};
typedef std::shared_ptr<DeviceTextPosConfResponse> DeviceTextPosConfResponseSharedPtr;

//==========> DeviceStorageInfoRequest() // 2022.10.20. added
struct DeviceStorageInfoRequest : DeviceRequestBase
{
};
typedef std::shared_ptr<DeviceStorageInfoRequest> DeviceStorageInfoRequestSharedPtr;

struct DeviceStorageInfoResponse : ResponseBase
{
    bool isNormal = true;
    int usedSpace = 0;
    int totalSpace = 0;
};
typedef std::shared_ptr<DeviceStorageInfoResponse> DeviceStorageInfoResponseSharedPtr;

//==========> DeviceGetPasswordExpiryRequest() // 2022.10.20. added
struct DeviceGetPasswordExpiryResponse : ResponseBase
{
    bool isPasswordExpired = false;
    int expiryDurationSettingMonths = 0;
};
typedef std::shared_ptr<DeviceGetPasswordExpiryResponse> DeviceGetPasswordExpiryResponseSharedPtr;

}
}
