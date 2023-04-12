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
#include <list>

#include <functional>

#include <boost/asio.hpp>
#include <boost/core/ignore_unused.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "SunapiDeviceClientUtil.h"
#include "SunapiDeviceClientLogSettings.h"

#include "DeviceClient/IDeviceClient.h"
#include "WeakCallback.h"

#include "HttpSession.h"
#include "MediaAgent.h"
#include "EventStatusDispatcher.h"
#include "PosDataDispatcher.h"
#include "DeviceObjectComposer.h"
#include "ConfigUriComposer.h"
#include "EasyConnection.h"
#include "CloudService.h"
#include "S1Sip.h"
#include "SecurityManager.h"
#include "TimeUtil.h"

#include "cgis/BaseCommand.h"
#include "cgis/SunapiSyntaxBuilder.h"
#include "cgis/SecurityCgi.h"
#include "cgis/SystemCgi.h"
#include "cgis/NetworkCgi.h"
#include "cgis/MediaCgi.h"
#include "cgis/RecordingCgi.h"
#include "cgis/AttributesCgi.h"
#include "cgis/EventStatusCgi.h"

namespace Wisenet
{
namespace Device
{

typedef std::function<void(void)> ProcessNextCommandFunc;
typedef std::function<void(Wisenet::ErrorCode&)> ProcessConnectFailFunc;

class SunapiDeviceClientImpl : public std::enable_shared_from_this<SunapiDeviceClientImpl>
{
public:
    SunapiDeviceClientImpl(boost::asio::io_context& ioc,
                           boost::asio::io_context& streamIoc,
                           std::string deviceUUID,
                           std::shared_ptr<Wisenet::Library::CloudService> cloudService,
                           long long keepAliveTime,
                           bool allowRelay,
                           unsigned short s1DaemonPort);

    ~SunapiDeviceClientImpl();

    bool IsReleased();

    void SetDeviceEventHandler(DeviceEventBaseHandler const& eventHandler);
    void SetDeviceLatestStatusEventHandler(DeviceEventBaseHandler const& eventHandler);
    void Release();
    void DeviceConnect(DeviceConnectRequestSharedPtr const& request,
                       ResponseBaseHandler const& responseHandler);
    void DeviceDisconnect(DeviceDisconnectRequestSharedPtr const& request,
                          ResponseBaseHandler const& responseHandler);
    void DeviceMediaOpen(DeviceMediaOpenRequestSharedPtr const& request,
                         ResponseBaseHandler const& responseHandler);
    void DeviceMediaClose(DeviceMediaCloseRequestSharedPtr const& request,
                          ResponseBaseHandler const& responseHandler);
    void DeviceMediaControl(DeviceMediaControlRequestSharedPtr const& request,
                            ResponseBaseHandler const& responseHandler );
    void DeviceTalk(DeviceTalkRequestSharedPtr const& request,
                    ResponseBaseHandler const& responseHandler);
    void DeviceSendTalkData(DeviceSendTalkDataRequestSharedPtr const& request);
    void DeviceGetOverlappedId(DeviceGetOverlappedIdRequestSharedPtr const& request,
                               ResponseBaseHandler const& responseHandler );
    void DeviceCalendarSearch(DeviceCalendarSearchRequestSharedPtr const& request,
                              ResponseBaseHandler const& responseHandler );
    void DeviceGetTimeline(DeviceGetTimelineRequestSharedPtr const& request,
                           ResponseBaseHandler const& responseHandler );
    void DeviceGetRecordingPeriod(DeviceGetRecordingPeriodRequestSharedPtr const& request,
                                  ResponseBaseHandler const& responseHandler);
    void DeviceGetAllPeriodTimeline(DeviceGetAllPeriodTimelineRequestSharedPtr const& request,
                                    ResponseBaseHandler const& responseHandler);
    void DeviceGetPreset(DeviceGetPresetRequestSharedPtr const& request,
                         ResponseBaseHandler const& responseHandler);
    void DeviceAddPreset(DeviceAddPresetRequestSharedPtr const& request,
                         ResponseBaseHandler const& responseHandler);
    void DeviceRemovePreset(DeviceRemovePresetRequestSharedPtr const& request,
                            ResponseBaseHandler const& responseHandler);
    void DeviceMovePreset(DeviceMovePresetRequestSharedPtr const& request,
                          ResponseBaseHandler const& responseHandler);
    // swing
    void DeviceGetSwing(ChannelRequestBaseSharedPtr const& request,
                        ResponseBaseHandler const& responseHandler);
    void DeviceMoveStopSwing(DeviceMoveStopSwingRequestSharedPtr const& request,
                             ResponseBaseHandler const& responseHandler);
    // group
    void DeviceGetGroup(ChannelRequestBaseSharedPtr const& request,
                        ResponseBaseHandler const& responseHandler);
    void DeviceMoveGroup(DeviceMoveGroupRequestSharedPtr const& request,
                             ResponseBaseHandler const& responseHandler);
    void DeviceStopGroup(DeviceStopGroupRequestSharedPtr const& request,
                             ResponseBaseHandler const& responseHandler);
    // tour
    void DeviceGetTour(ChannelRequestBaseSharedPtr const& request,
                        ResponseBaseHandler const& responseHandler);
    void DeviceMoveTour(ChannelRequestBaseSharedPtr const& request,
                         ResponseBaseHandler const& responseHandler);
    void DeviceStopTour(ChannelRequestBaseSharedPtr const& request,
                         ResponseBaseHandler const& responseHandler);
    // trace
    void DeviceGetTrace(ChannelRequestBaseSharedPtr const& request,
                       ResponseBaseHandler const& responseHandler);
    void DeviceMoveTrace(DeviceMoveTraceRequestSharedPtr const& request,
                        ResponseBaseHandler const& responseHandler);
    void DeviceStopTrace(DeviceStopTraceRequestSharedPtr const& request,
                        ResponseBaseHandler const& responseHandler);
    void DevicePTZContinuous(DevicePTZContinuousRequestSharedPtr const& request,
                             ResponseBaseHandler const& responseHandler);
    void DevicePTZFocus(DevicePTZFocusRequestSharedPtr const& request,
                        ResponseBaseHandler const& responseHandler);
    void DevicePTZStop(DevicePTZStopRequestSharedPtr const& request,
                       ResponseBaseHandler const& responseHandler);
    void DeviceImageFocusMode(DeviceImageFocusModeRequestSharedPtr const& request,
                              ResponseBaseHandler const& responseHandler);
    void DevicePTZAuxControl(DevicePTZAuxControlRequestSharedPtr const& request,
                             ResponseBaseHandler const& responseHandler);
    void DeviceSnapshot(DeviceSnapshotRequestSharedPtr const& request,
                        ResponseBaseHandler const& responseHandler );
    void DeviceThumbnail(DeviceThumbnailRequestSharedPtr const& request,
                        ResponseBaseHandler const& responseHandler );
    void DeviceConfigBackup(DeviceConfigBackupRequestSharedPtr const& request,
                            ResponseBaseHandler const& responseHandler );
    void DeviceConfigRestore(DeviceConfigRestoreRequestSharedPtr const& request,
                             ResponseBaseHandler const& responseHandler );
    void DeviceFirmwareUpdate(DeviceFirmwareUpdateRequestSharedPtr const& request,
                              ResponseBaseHandler const& responseHandler );
    void DeviceChannelFirmwareUpdate(DeviceChannelFirmwareUpdateRequestSharedPtr const& request,
                                     ResponseBaseHandler const& responseHandler);
    void ContinueChannelUpgrade(const std::string upgradeToken,
                                const DeviceChannelFirmwareUpdateRequestSharedPtr &request,
                                const ResponseBaseHandler &responseHandler,
                                const int idx);
    void DeviceChannelUpgradeStatusView(const std::string upgradeToken,
                                        const DeviceChannelFirmwareUpdateRequestSharedPtr &upgradeRequest,
                                        const ResponseBaseHandler &responseHandler);
    void StartChannelUpdateTimer(const std::string upgradeToken,
                                 const DeviceChannelFirmwareUpdateRequestSharedPtr &upgradeRequest,
                                 const ResponseBaseHandler &responseHandler,
                                 const long long durationMsec);
    void DeviceRenewUpgradeToken(const std::string upgradeToken,
                                 const DeviceChannelFirmwareUpdateRequestSharedPtr &upgradeRequest,
                                 const ResponseBaseHandler &responseHandler);
    void StopChannelUpdateTimer();
    void DeviceChangeUserPassword(DeviceChangeUserPasswordRequestSharedPtr const& request,
                                  ResponseBaseHandler const& responseHandler);
    void DeviceSetAlarmOutput(const DeviceSetAlarmOutputRequestSharedPtr& request,
                              const ResponseBaseHandler& responseHandler);
    void DeviceAreaZoom(DeviceAreaZoomRequestSharedPtr const& request,
                        ResponseBaseHandler const& responseHandler );
    void DeviceGetDefog(DeviceGetDefogRequestSharedPtr const& request,
                        ResponseBaseHandler const& responseHandler);
    void DeviceSetDefog(DeviceSetDefogRequestSharedPtr const& request,
                        ResponseBaseHandler const& responseHandler);

    void DeviceGetAutoTracking(DeviceGetAutoTrackingRequestSharedPtr const& request,
                               ResponseBaseHandler const& responseHandler );
    void DeviceSetAutoTracking(DeviceSetAutoTrackingRequestSharedPtr const& request,
                               ResponseBaseHandler const& responseHandler );
    void DeviceSetTargetLockCoordinate(DeviceSetTargetLockCoordinateRequestSharedPtr const& request,
                                       ResponseBaseHandler const& responseHandler );
    void DeviceSetDigitalAutoTracking(DeviceSetDigitalAutoTrackingRequestSharedPtr const& request,
                                      ResponseBaseHandler const& responseHandler );
    void DeviceMetaAttributeSearch( DeviceMetaAttributeSearchRequestSharedPtr const& request,
                                              ResponseBaseHandler const& responseHandler );
    void DeviceAiSearchCancel( DeviceRequestBaseSharedPtr const& request,
                                              ResponseBaseHandler const& responseHandler );
    void DeviceOcrSearch( DeviceMetaAttributeSearchRequestSharedPtr const& request,
                                              ResponseBaseHandler const& responseHandler );
    void DeviceUrlSnapShot( DeviceUrlSnapShotRequestSharedPtr const& request,
                                              ResponseBaseHandler const& responseHandler );
    void DeviceSmartSearch( DeviceSmartSearchRequestSharedPtr const& request,
                                              ResponseBaseHandler const& responseHandler );
    void DeviceSmartSearchCancel( DeviceRequestBaseSharedPtr const& request,
                                              ResponseBaseHandler const& responseHandler );
    void DeviceTextSearch(DeviceTextSearchRequestSharedPtr const& request,
                          ResponseBaseHandler const& responseHandler );
    void DeviceTextPosConf(DeviceTextPosConfRequestSharedPtr const& request,
                           ResponseBaseHandler const& responseHandler );
    void DeviceStorageInfo(DeviceStorageInfoRequestSharedPtr const& request,
                           ResponseBaseHandler const& responseHandler );
    void DeviceGetPasswordExpiry(DeviceRequestBaseSharedPtr const& request,
                           ResponseBaseHandler const& responseHandler );
    void DeviceExtendPasswordExpiration(DeviceRequestBaseSharedPtr const& request,
                                 ResponseBaseHandler const& responseHandler );
    void DeviceTryConnectingS1DDNS(DeviceRequestBaseSharedPtr const& request,
                                   ResponseBaseHandler const& responseHandler);

private:
    /* common http request processing */
    enum class AsyncRequestType {
        UNKNOWN,
        HTTPGET,
        HTTPGETDOWNLOAD,
        HTTPPOSTENCODEDUPLOAD,
        HTTPPOSTENCODEDDATA,
        HTTPFIRMWAREUPDATE,
        HTTPBIGGET,
        HTTPPOSTDOWNLOAD,
        HTTPPOSTENCODEDUPLOADPASSWORD,
    };

    void asyncRequest(std::shared_ptr<HttpSession> httpSession,
                      BaseCommandSharedPtr baseCommand,
                      ResponseBaseHandler const& responseHandler,
                      ProcessNextCommandFunc processNextCommandFunc,
                      AsyncRequestType requestType,
                      bool ignoreConnectStatus,
                      ProcessConnectFailFunc processConnectFailFunc = nullptr,
                      int timeout =10);

    void eventHandler(const DeviceEventBaseSharedPtr& event);
    void deviceLatestStatusEventHandler(const DeviceEventBaseSharedPtr &event);


    /* used in DeviceConnect  */
    void DeviceConnectViaURL(DeviceConnectRequestSharedPtr const& request,
                             ResponseBaseHandler const& responseHandler);
    void DeviceConnectViaDDNS(DeviceConnectRequestSharedPtr const& request,
                       ResponseBaseHandler const& responseHandler, int tryToConnectCount);
    void DeviceConnectViaP2P(DeviceConnectRequestSharedPtr const& request,
                       ResponseBaseHandler const& responseHandler, int tryToConnectCount);
    void getP2PInfoFromCloud(const DeviceConnectRequestSharedPtr &request,
                                            const ResponseBaseHandler &responseHandler,
                                            const std::string& productP2PID,
                                            int tryToConnectCount);

    // S1 DDNS & SIP
    void DeviceConnectViaS1DDNS(DeviceConnectRequestSharedPtr const& request,
                                ResponseBaseHandler const& responseHandler, int tryToConnectCount);
    void DeviceConnectViaS1SIP(DeviceConnectRequestSharedPtr const& request,
                                ResponseBaseHandler const& responseHandler, int tryToConnectCount);

    void getSystemDeviceInfo(DeviceConnectRequestSharedPtr const& request,
                             ResponseBaseHandler const& responseHandler,
                             ProcessConnectFailFunc processConnectFailFunc);

    void getSystemDate(const DeviceObjectComposerPtr& oc, ResponseBaseHandler const& responseHandler);
    void getNetworkInterface(const DeviceObjectComposerPtr& oc, ResponseBaseHandler const& responseHandler);
    void getNetworkRtspPort(const DeviceObjectComposerPtr& oc, ResponseBaseHandler const& responseHandler);
    void getAttributes(const DeviceObjectComposerPtr& oc, ResponseBaseHandler const& responseHandler);
    void checkRtspPort(const DeviceObjectComposerPtr& oc, ResponseBaseHandler const& responseHandler);
    void getMediaVideoSource(const DeviceObjectComposerPtr& oc, ResponseBaseHandler const& responseHandler);
    void getMediaVideoProfile(const DeviceObjectComposerPtr& oc, ResponseBaseHandler const& responseHandler);
    void getMediaVideoProfilePolicy(const DeviceObjectComposerPtr& oc, ResponseBaseHandler const& responseHandler);
    void getRecordingStorage(const DeviceObjectComposerPtr& oc, ResponseBaseHandler const& responseHandler);
    void getRecordingGeneral(const DeviceObjectComposerPtr& oc, ResponseBaseHandler const& responseHandler);
    void getMediaAudioOutput(const DeviceObjectComposerPtr& oc, ResponseBaseHandler const& responseHandler);
    void getEventSourcesAlarmInput(const DeviceObjectComposerPtr& oc, ResponseBaseHandler const& responseHandler);
    void getIoAlarmOutput(const DeviceObjectComposerPtr& oc, ResponseBaseHandler const& responseHandler);
    void getEventStatusSchema(const DeviceObjectComposerPtr& oc, ResponseBaseHandler const& responseHandler);

    // for NVR only
    void getMediaSesseionKey(const DeviceObjectComposerPtr& oc, ResponseBaseHandler const& responseHandler);
    void getMediaCameraRegister(const DeviceObjectComposerPtr& oc, ResponseBaseHandler const& responseHandler);
    void getMediaCameraUpgrade(const DeviceObjectComposerPtr &oc, ResponseBaseHandler const&responseHandler);
    void getEventSourcesNetworkAlarmInput(const DeviceObjectComposerPtr& oc, ResponseBaseHandler const& responseHandler);
    void getRecordingPosConf(const DeviceObjectComposerPtr& oc, ResponseBaseHandler const& responseHandler);

    // final
    void DeviceConnectFinal(const DeviceObjectComposerPtr& oc, ResponseBaseHandler const& responseHandler);

    // EventSession
    void startEventSession();
    void startPosEventSession();

    void getMediaVideoProfile(const std::string& channelID, MediaVideoProfilePolicyViewResult& parseResult,
                              ResponseBaseHandler const& responseHandler);


    // create low profile for camera
    void createVideoProfile(const DeviceObjectComposerPtr& oc,
                            const ResponseBaseHandler& responseHandler);
    void createVideoProfileFinal(const DeviceObjectComposerPtr& oc,
                                 const ResponseBaseHandler& responseHandler);

    /* change user password --> */
    void getUser(std::string publicKey, DeviceChangeUserPasswordRequestSharedPtr const& request, ResponseBaseHandler const& responseHandler);
    void updateUserPassword(const std::string& index, const std::string& password, DeviceChangeUserPasswordRequestSharedPtr const& request, ResponseBaseHandler const& responseHandler);
    void updateEncryptedUserPassword(const std::string& index, const std::string& password, DeviceChangeUserPasswordRequestSharedPtr const& request, ResponseBaseHandler const& responseHandler);
    /* <-- change user password */

    /* used in ptzControl  --> */
    void SetFocusURL(std::string& Url, DevicePTZContinuousRequestSharedPtr const& request);
    void SetPTZURL(std::string& Url, DevicePTZContinuousRequestSharedPtr const& request);

    void DeviceConfigBackupInternal(DeviceConfigBackupRequestSharedPtr const& request,
                            ResponseBaseHandler const& responseHandler );
    void DeviceConfigRestoreInternal(DeviceConfigRestoreRequestSharedPtr const& request,
                            ResponseBaseHandler const& responseHandler );
    void DeviceConfigBackupGetRsa(DeviceConfigBackupRequestSharedPtr const& request,
                            ResponseBaseHandler const& responseHandler );
    void DeviceConfigRestoreGetRsa(DeviceConfigRestoreRequestSharedPtr const& request,
                            ResponseBaseHandler const& responseHandler );

    void closeAll();
    void closeAllMediaSession();
    void disconnect(const DeviceStatusType disconnType);

    /* used in Device Media Open */
    void rtspMediaOpen(MediaRequestInfo mediaRequest, std::string& channel, MediaAgentPtr const& mediaAgent);
    std::string getCameraRtspUrl(MediaRequestInfo mediaRequest, Rtsp::TransportMethod transportMethod, std::string& channel);
    std::string getNvrRtspUrl(MediaRequestInfo mediaRequest, Rtsp::TransportMethod transportMethod, std::string& channel);
    Wisenet::ErrorCode convertRtspStatusToErrorcode(unsigned short statusCode);
    std::string getSessionKey(StreamType streamType, std::string playbackSessionId);
    void checkSessionKeyRelease(std::string playbackSessionId);
    MediaAgentPtr getMediaAgent(const std::string& mediaUUID);
    int getPlaybackChannelCount();

    /* used in DevcieGetAllPeriodTimeline */
    void getRecentTimeline(DeviceGetAllPeriodTimelineRequestSharedPtr const& request,
                           ResponseBaseHandler const& responseHandler);
    void getAllPeriodTimeline(DeviceGetAllPeriodTimelineRequestSharedPtr const& request,
                              ResponseBaseHandler const& responseHandler);
    void getAllPeriodOverlappedId(DeviceGetAllPeriodTimelineRequestSharedPtr const& request,
                                  ResponseBaseHandler const& responseHandler,
                                  DeviceGetAllPeriodTimelineResponseSharedPtr const& response);
    void getAllPeriodOverlappedTimeline(DeviceGetAllPeriodTimelineRequestSharedPtr const& request,
                                        ResponseBaseHandler const& responseHandler,
                                        DeviceGetAllPeriodTimelineResponseSharedPtr const& response,
                                        unsigned int overlappedIdIndex,
                                        unsigned int channeldIndex,
                                        int64_t startTime,
                                        int64_t endTime);
    void getRecordingPeriodFromCalendarSearch(DeviceGetAllPeriodTimelineRequestSharedPtr const& request,
                                              ResponseBaseHandler const& responseHandler,
                                              DeviceGetAllPeriodTimelineResponseSharedPtr const& response,
                                              unsigned int periodIndex);
    void updateRecordingPeriodFromCalendarSearch(DeviceGetAllPeriodTimelineResponseSharedPtr const& finalResponse,
                                                 DeviceCalendarSearchResponseSharedPtr const calendarResponse,
                                                 int64_t monthMsec);
    void getAllOverlappedIdTimeline(DeviceGetTimelineRequestSharedPtr const& request,
                                    ResponseBaseHandler const& responseHandler);

    /* meta attribute search */
    void metaAttributeSearchViewStatus(std::string seachToken,
                                 DeviceMetaAttributeSearchResponseSharedPtr finalResponse,
                                 ResponseBaseHandler const& responseHandler);

    void metaAttributeSearchViewResult(std::string searchToken,
                                 int startIndex, int maxCount,
                                 DeviceMetaAttributeSearchResponseSharedPtr finalResponse,
                                 ResponseBaseHandler const& responseHandler);

    void ocrSearchViewStatus(std::string seachToken,
                                 DeviceMetaAttributeSearchResponseSharedPtr finalResponse,
                                 ResponseBaseHandler const& responseHandler);

    void ocrSearchViewResult(std::string searchToken,
                                 int startIndex, int maxCount,
                                 DeviceMetaAttributeSearchResponseSharedPtr finalResponse,
                                 ResponseBaseHandler const& responseHandler);

    void urlSnapShotResult(DeviceUrlSnapShotRequestSharedPtr const& request,
                           DeviceUrlSnapShotResponseSharedPtr finalResponse,
                           ResponseBaseHandler const& responseHandler);

    void smartSearchViewStatus(std::string seachToken,
                                 DeviceSmartSearchResponseSharedPtr finalResponse,
                                 ResponseBaseHandler const& responseHandler);

    void smartSearchViewResult(std::string searchToken,
                                 DeviceSmartSearchResponseSharedPtr finalResponse,
                                 ResponseBaseHandler const& responseHandler);
    void smartSearchCancelInternal(std::string searchToken);

    // 2022.08.29. added
    void textSearchViewStatus(std::string deviceID,
                              std::string seachToken,
                              DeviceTextSearchResponseSharedPtr finalResponse,
                              ResponseBaseHandler const& responseHandler);
    void textSearchViewResult(std::string deviceID,
                              std::string searchToken, std::string requestCount,
                              DeviceTextSearchResponseSharedPtr finalResponse,
                              ResponseBaseHandler const& responseHandler);

    void setHttpConnection(const std::shared_ptr<HttpSession>& httpSession,
                           const Wisenet::Device::DeviceConnectInfo& connInfo,
                           const int connectionTimeoutSec = 10);

    /* Change Config URI*/
    void changeConfigUri(const std::string& configUri);
    void dispatchConfigUri(const ConfigUriComposerPtr& cuc, const unsigned int configCap);
    void reloadAttribute(const ConfigUriComposerPtr& cuc, const unsigned int configCap);
    void reloadSystemDate(const ConfigUriComposerPtr& cuc, const unsigned int configCap);
    void reloadRecordingGeneral(const ConfigUriComposerPtr& cuc, const unsigned int configCap);
    void reloadMediaVideoProfile(const ConfigUriComposerPtr& cuc, const unsigned int configCap);
    void reloadMediaVideoSource(const ConfigUriComposerPtr& cuc, const unsigned int configCap);
    void reloadMediaVideoProfilePolicy(const ConfigUriComposerPtr& cuc, const unsigned int configCap);
    void reloadRecordingStorage(const ConfigUriComposerPtr& cuc, const unsigned int configCap);
    void reloadNetworkRtsp(const ConfigUriComposerPtr& cuc, const unsigned int configCap);

    /* Check Device status */
    bool isGood();
    void StartKeepAliveTimer(long long keepAliveTime = 0);
    void StopKeepAliveTimer();

    /* Check Event Change config uri */
    void StartChangeConfigUriTimer(long long durationMsec = 500);
    void StopChangeConfigUriTimer();

    /* Check Ai/Smart search status */
    void StartAiSearchStatusTimer(std::string searchToken,
                                  DeviceMetaAttributeSearchResponseSharedPtr finalResponse,
                                  ResponseBaseHandler const& responseHandler, long long durationMsec = 500);
    void StartOcrSearchStatusTimer(std::string searchToken,
                                  DeviceMetaAttributeSearchResponseSharedPtr finalResponse,
                                  ResponseBaseHandler const& responseHandler, long long durationMsec = 500);
    void StartSmartSearchStatusTimer(std::string searchToken,
                                  DeviceSmartSearchResponseSharedPtr finalResponse,
                                  ResponseBaseHandler const& responseHandler, long long durationMsec = 1000);
    void StartTextSearchStatusTimer(std::string deviceID,
                                    std::string searchToken,
                                    DeviceTextSearchResponseSharedPtr finalResponse,
                                    ResponseBaseHandler const& responseHandler, long long durationMsec = 1000);
    void textSearchCancelInternal(std::string searchToken);

    void StopAiSearchStatusTimer();

    /* Relay Mode */
    bool needToFinishConnection();
    void sendFirmwareUpdateAlarm();

    /* AlarmOutput Status*/
    void changeAlarmOuputStatus(const std::string& deviceOutputID, bool on);

private:
    boost::asio::strand<boost::asio::io_context::executor_type> m_strand;
    boost::asio::io_context&    m_deviceIoc;
    boost::asio::io_context&    m_streamIoc;
    std::string                 m_deviceUUID;
    std::string                 m_logPrefix;
    DeviceEventBaseHandler      m_eventHandler = nullptr;
    DeviceEventBaseHandler      m_deviceLatestStatusEventHandler = nullptr;

    /* HTTP SESSIONS */
    std::shared_ptr<HttpSession>    m_httpCommandSession;   // command session
    std::shared_ptr<HttpSession>    m_httpEventSession;     // event session
    std::shared_ptr<HttpSession>    m_httpPosEventSession;  // POS event session
    std::shared_ptr<HttpSession>    m_httpPtzSession;       // ptz session
    std::shared_ptr<HttpSession>    m_httpConfigSession;    // firmware update, config backup, config restore session
    std::shared_ptr<HttpSession>    m_httpSearchMetaDataSession; // search metadata session
    std::shared_ptr<HttpSession>    m_httpChannelUpgradeSession; // renew upgrade token session

    /* Media Agents */
    std::vector<Wisenet::Device::MediaAgentPtr> m_mediaAgentList;
    std::map<std::string, Rtsp::RtspClientPtr> m_TalkClientMap;

    /* Device attributes & information repository */
    DeviceObjectComposerPtr m_repos;

    /* Device connection info */
    DeviceConnectInfo       m_connectionInfo;

    /* EasyConnection - WISENET DDNS & P2P */
    EasyConnection          m_easyConnection;
    std::shared_ptr<Wisenet::Library::CloudService> m_cloudService;

    /* S1 P2P */
    std::shared_ptr<S1Sip> m_s1sip;
    unsigned short m_s1DaemonPort = 1881;

    /* Check a device session and recovery a device session */
    boost::asio::steady_timer m_keepAliveTimer;
    boost::asio::steady_timer m_changeConfigUriTimer;
    boost::asio::steady_timer m_aiSearchStatusTimer;
    boost::asio::steady_timer m_channelUpdateStatusTimer;

    unsigned int              m_lastChangeConfigUriCap = 0;
    std::chrono::steady_clock::time_point m_lastReveivedTime; // 마지막으로 받은 응답의 시간
    bool m_needToMonitorEventSessionStatus = false; // 이벤트 세션이 끊겼는지 확인하여 다시 실행을 할지 여부를 확인하는 변수
    bool m_isRunningEventSession = false; // 이벤트 세션이 동작 중인지 확인하는 변수
    bool m_needToMonitorPosEventSession = false; // POS 이벤트 세션이 끊겼는지 확인하여 다시 실행을 할지 여부를 확인하는 변수
    bool m_isRunningPosEventSession = false; // POS 이벤트 세션이 동작 중인지 확인하는 변수
    bool m_isConnecting = false; //접속 중인지 확인하는 변수
    bool m_isCompletedConnection = false; //접속 과정이 완료되어 객체 구성이 완료되었는지 확인하는 변수.
    bool m_isFwupdating = false;
    bool m_isFwupdateCanceling = false;
    long long m_keepAliveTime; // KeepAliveTime
    bool m_allowRelay = false;
    std::atomic<bool> m_runningAiSearch{false};
    std::atomic<bool> m_isReleased{false};
    bool m_isFwUpdateStatusChecking = false;
    bool m_isChannelFwUpdating = false;
    bool m_isChannelFwUpdateCanceling = false;
    bool m_isChannelFwUploading = false;

    /* Relay Connection */
    long long m_startRelayMode; // Relay 모드가 시작된 시간을 기록하는 변수.

    /* AlarmOut status*/
    std::map<std::string, bool> m_alarmOutputStatus;

    long long m_deviceRegistrationStartTime = 0;

//    unsigned short m_uploadingWaitCount = 0;
//    const int m_uploadingFailCount = 12;
};

}
}
