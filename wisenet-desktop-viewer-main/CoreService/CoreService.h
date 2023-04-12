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
#include <QUuid>
#include <boost/asio/strand.hpp>
#include <boost/asio/io_context.hpp>

#include <memory>
#include <unordered_map>
#include <map>
#include "ThreadPool.h"
#include "CoreService/ICoreService.h"
#include "DeviceClient/IDeviceClient.h"

#include "CoreServiceConfig.h"
#include "CloudService.h"

namespace asio = boost::asio;
namespace device = Wisenet::Device;

class ThreadPool;

namespace Wisenet
{
namespace Core
{

class DeviceDiscovery;
class IPv6DeviceDiscovery;
class DatabaseManager;
class LogManager;
class UserSession;
class IpSetting;
class InitPassword;
class EventRuleManager;
class PTZLogManager;
class AlarmOutActionManager;
class LdapManager;
//class DashboardManager;

typedef std::unordered_map<uuid_string, Wisenet::Device::BaseDeviceClientSharedPtr> DeviceClientMap;

class CoreService : public ICoreService, public std::enable_shared_from_this<CoreService>
{
public:
    CoreService();
    ~CoreService();

public: // ICoreService
    void Start() override;
    void Stop() override;
    bool IsRunning() override;

    void SetS1DaemonPort(unsigned short port) override;

    /***** All comments are in ICoreService.h *****/
    void GetServiceInformation(RequestBaseSharedPtr const& request,
                               ResponseBaseHandler const& responseHandler) override;

    void SetupService(RequestBaseSharedPtr const& request,
                      ResponseBaseHandler const& responseHandler) override;

    void Login(RequestBaseSharedPtr const& request,
               ResponseBaseHandler const& responseHandler) override;

    void Logout(RequestBaseSharedPtr const& request,
                ResponseBaseHandler const& responseHandler) override;

    void StartEventService(RequestBaseSharedPtr const& request,
                           ResponseBaseHandler const& responseHandler) override;


    void SaveUserGroup(RequestBaseSharedPtr const& request,
                       ResponseBaseHandler const& responseHandler) override;

    void RemoveUserGroups(RequestBaseSharedPtr const& request,
                        ResponseBaseHandler const& responseHandler) override;


    void SaveUser(RequestBaseSharedPtr const& request,
                  ResponseBaseHandler const& responseHandler) override;

    void RemoveUsers(RequestBaseSharedPtr const& request,
                     ResponseBaseHandler const& responseHandler) override;

    void SaveGroup(RequestBaseSharedPtr const&request,
                   ResponseBaseHandler const&responseHandler) override;

    void RemoveGroups(RequestBaseSharedPtr const&request,
                      ResponseBaseHandler const&responseHandler) override;

    void AddGroupMapping(RequestBaseSharedPtr const& request,
                         ResponseBaseHandler const& responseHandler) override;

    void RemoveGroupMapping(RequestBaseSharedPtr const&request,
                            ResponseBaseHandler const&responseHandler) override;

    void UpdateGroupMapping(RequestBaseSharedPtr const&request,
                            ResponseBaseHandler const&responseHandler) override;

    void AddGroupWithUpdateMapping(RequestBaseSharedPtr const&request,
                                   ResponseBaseHandler const&responseHandler) override;

    void SaveLayout(RequestBaseSharedPtr const& request,
                    ResponseBaseHandler const& responseHandler) override;

    void RemoveLayouts(RequestBaseSharedPtr const& request,
                       ResponseBaseHandler const& responseHandler) override;

    void SaveSequenceLayout(RequestBaseSharedPtr const& request,
                            ResponseBaseHandler const& responseHandler) override;

    void RemoveSequenceLayouts(RequestBaseSharedPtr const& request,
                               ResponseBaseHandler const& responseHandler) override;

    void SaveWebpage(RequestBaseSharedPtr const& request,
                     ResponseBaseHandler const& responseHandler) override;

    void RemoveWebpages(RequestBaseSharedPtr const& request,
                        ResponseBaseHandler const& responseHandler) override;

    void AddDevice(RequestBaseSharedPtr const & request,
                   ResponseBaseHandler const & responseHandler) override;

    void RemoveDevices(RequestBaseSharedPtr const & request,
                       ResponseBaseHandler const & responseHandler) override;
    void UpdateDevices(RequestBaseSharedPtr const & request,
                       ResponseBaseHandler const & responseHandler) override;

    void DiscoverDevices(RequestBaseSharedPtr const& request,
                         ResponseBaseHandler const& responseHandler) override;

    void DiscoverIPv6Devices(RequestBaseSharedPtr const& request,
                         ResponseBaseHandler const& responseHandler) override;

    void IpSettingDevice(RequestBaseSharedPtr const& request,
                         ResponseBaseHandler const& responseHandler) override;

    void InitDevicePassword(RequestBaseSharedPtr const& request,
                            ResponseBaseHandler const& responseHandler) override;

    void GetAuditLog(RequestBaseSharedPtr const& request,
                     ResponseBaseHandler const& responseHandler) override;

    void GetEventLog(RequestBaseSharedPtr const& request,
                     ResponseBaseHandler const& responseHandler) override;

    void GetSystemLog(RequestBaseSharedPtr const& request,
                      ResponseBaseHandler const& responseHandler) override;

    void AddLog(RequestBaseSharedPtr const& request,
                ResponseBaseHandler const& responseHandler) override;

    void GetDeviceProfile(RequestBaseSharedPtr const& request,
                     ResponseBaseHandler const& responseHandler) override;

    void SaveBookmark(RequestBaseSharedPtr const& request,
                      ResponseBaseHandler const& responseHandler) override;

    void RemoveBookmarks(RequestBaseSharedPtr const& request,
                         ResponseBaseHandler const& responseHandler) override;

    void GetEventRule(RequestBaseSharedPtr const& request,
                      ResponseBaseHandler const& responseHandler) override;

    void SaveEventRule(RequestBaseSharedPtr const& request,
                       ResponseBaseHandler const& responseHandler) override;

    void RemoveEventRules(RequestBaseSharedPtr const& request,
                          ResponseBaseHandler const& responseHandler) override;

    void SaveEventSchedule(RequestBaseSharedPtr const& request,
                           ResponseBaseHandler const& responseHandler) override;

    void RemoveEventSchedules(RequestBaseSharedPtr const& request,
                          ResponseBaseHandler const& responseHandler) override;

    void SaveEventEmail(RequestBaseSharedPtr const& request,
                           ResponseBaseHandler const& responseHandler) override;

    void RemoveEventEmail(RequestBaseSharedPtr const& request,
                          ResponseBaseHandler const& responseHandler) override;

    /*void GetDashboardData(RequestBaseSharedPtr const& request,
                          ResponseBaseHandler const&responseHandler) override;*/

    void SaveChannels(RequestBaseSharedPtr const& request,
                         ResponseBaseHandler const& responseHandler) override;

    void SaveDevicesCredential(RequestBaseSharedPtr const& request,
                         ResponseBaseHandler const& responseHandler) override;

    void BackupService(RequestBaseSharedPtr const& request,
                         ResponseBaseHandler const& responseHandler) override;

    void RestoreService(RequestBaseSharedPtr const& request,
                         ResponseBaseHandler const& responseHandler) override;


    void ResetService(RequestBaseSharedPtr const& request,
                         ResponseBaseHandler const& responseHandler) override;

    void UpdateSoftwareService(RequestBaseSharedPtr const& request,
                         ResponseBaseHandler const& responseHandler) override;

    void UpdateLdapSetting(RequestBaseSharedPtr const& request,
                         ResponseBaseHandler const& responseHandler) override;

    void LdapConnect(RequestBaseSharedPtr const& request,
                     ResponseBaseHandler const& responseHandler) override;

    void UpdateStatistics(RequestBaseSharedPtr const& request,
                          ResponseBaseHandler const& responseHandler) override;

    void ActivateLicense(RequestBaseSharedPtr const& request,
                         ResponseBaseHandler const& responseHandler) override;

    /***** All comments are in IDeviceClient.h *****/
public:
    void DeviceConnect(RequestBaseSharedPtr const& request,
                       ResponseBaseHandler const& responseHandler) override;

    void DeviceDisconnect(RequestBaseSharedPtr const & request,
                          ResponseBaseHandler const & responseHandler) override;

    void DeviceMediaOpen(RequestBaseSharedPtr const & request,
                         ResponseBaseHandler const & responseHandler) override;

    void DeviceMediaClose(RequestBaseSharedPtr const & request,
                          ResponseBaseHandler const & responseHandler) override;

    void DeviceMediaControl(RequestBaseSharedPtr const & request,
                            ResponseBaseHandler const & responseHandler) override;

    void DeviceTalk( RequestBaseSharedPtr const& request,
                     ResponseBaseHandler const& responseHandler) override;

    void DeviceSendTalkData(RequestBaseSharedPtr const& request,
                            ResponseBaseHandler const& responseHandler) override;

    void DeviceGetOverlappedId(RequestBaseSharedPtr const & request,
                               ResponseBaseHandler const & responseHandler) override;

    void DeviceCalendarSearch(RequestBaseSharedPtr const & request,
                              ResponseBaseHandler const & responseHandler) override;

    void DeviceGetTimeline(RequestBaseSharedPtr const & request,
                           ResponseBaseHandler const & responseHandler) override;

    void DeviceGetRecordingPeriod(RequestBaseSharedPtr const & request,
                                  ResponseBaseHandler const & responseHandler) override;

    void DeviceGetAllPeriodTimeline(RequestBaseSharedPtr const & request,
                                  ResponseBaseHandler const & responseHandler) override;

    void DeviceChangeUserPassword(RequestBaseSharedPtr const& request,
                                  ResponseBaseHandler const& responseHandler) override;

    void DeviceSnapshot(RequestBaseSharedPtr const& request,
                        ResponseBaseHandler const& responseHandler ) override;

    void DeviceThumbnail(RequestBaseSharedPtr const& request,
                        ResponseBaseHandler const& responseHandler ) override;

    void DeviceConfigBackup(RequestBaseSharedPtr const& request,
                            ResponseBaseHandler const& responseHandler ) override;

    void DeviceConfigRestore(RequestBaseSharedPtr const& request,
                             ResponseBaseHandler const& responseHandler ) override;

    void DeviceFirmwareUpdate(RequestBaseSharedPtr const& request,
                              ResponseBaseHandler const& responseHandler ) override;

    void DeviceChannelFirmwareUpdate(RequestBaseSharedPtr const& request,
                              ResponseBaseHandler const& responseHandler ) override;

    void DevicePTZContinuous(RequestBaseSharedPtr const& request,
                             ResponseBaseHandler const& responseHandler) override;

    void DevicePTZFocus(RequestBaseSharedPtr const& request,
                        ResponseBaseHandler const& responseHandler) override;

    void DevicePTZStop(RequestBaseSharedPtr const& request,
                       ResponseBaseHandler const& responseHandler)  override;

    void DeviceImageFocusMode(RequestBaseSharedPtr const& request,
                              ResponseBaseHandler const& responseHandler)  override;

    void DeviceGetPreset(RequestBaseSharedPtr const& request,
                         ResponseBaseHandler const& responseHandler) override;

    void DeviceAddPreset(RequestBaseSharedPtr const& request,
                         ResponseBaseHandler const& responseHandler) override;

    void DeviceRemovePreset(RequestBaseSharedPtr const& request,
                            ResponseBaseHandler const& responseHandler) override;

    void DeviceMovePreset(RequestBaseSharedPtr const& request,
                          ResponseBaseHandler const& responseHandler) override;

    void DeviceGetSwing(RequestBaseSharedPtr const& request,
                         ResponseBaseHandler const& responseHandler) override;

    void DeviceMoveStopSwing(RequestBaseSharedPtr const& request,
                        ResponseBaseHandler const& responseHandler) override;

    void DeviceGetGroup(RequestBaseSharedPtr const& request,
                         ResponseBaseHandler const& responseHandler) override;

    void DeviceMoveGroup(RequestBaseSharedPtr const& request,
                        ResponseBaseHandler const& responseHandler) override;

    void DeviceStopGroup(RequestBaseSharedPtr const& request,
                             ResponseBaseHandler const& responseHandler) override;

    void DeviceGetTour(RequestBaseSharedPtr const& request,
                        ResponseBaseHandler const& responseHandler) override;

    void DeviceMoveTour(RequestBaseSharedPtr const& request,
                         ResponseBaseHandler const& responseHandler) override;

    void DeviceStopTour(RequestBaseSharedPtr const& request,
                         ResponseBaseHandler const& responseHandler) override;

    void DeviceGetTrace(RequestBaseSharedPtr const& request,
                       ResponseBaseHandler const& responseHandler) override;

    void DeviceMoveTrace(RequestBaseSharedPtr const& request,
                        ResponseBaseHandler const& responseHandler) override;

    void DeviceStopTrace(RequestBaseSharedPtr const& request,
                        ResponseBaseHandler const& responseHandler) override;

    void DevicePTZAuxControl(RequestBaseSharedPtr const& request,
                             ResponseBaseHandler const& responseHandler) override;


    void DeviceAreaZoom(RequestBaseSharedPtr const& request,
                        ResponseBaseHandler const& responseHandler ) override;

    void DeviceGetAutoTracking(RequestBaseSharedPtr const& request,
                               ResponseBaseHandler const& responseHandler ) override;

    void DeviceSetAutoTracking(RequestBaseSharedPtr const& request,
                               ResponseBaseHandler const& responseHandler ) override;

    void DeviceSetDigitalAutoTracking(RequestBaseSharedPtr const& request,
                                      ResponseBaseHandler const& responseHandler ) override;
    void DeviceSetTargetLockCoordinate(RequestBaseSharedPtr const& request,
                                      ResponseBaseHandler const& responseHandler ) override;

    void DeviceGetDefog(RequestBaseSharedPtr const& request,
                        ResponseBaseHandler const& responseHandler)  override;

    void DeviceSetDefog(RequestBaseSharedPtr const& request,
                        ResponseBaseHandler const& responseHandler)  override;

    void DeviceSetAlarmOutput(const RequestBaseSharedPtr& request,
                              const ResponseBaseHandler& responseHandler) override;
    void DeviceMetaAttributeSearch(const RequestBaseSharedPtr& request,
                              const ResponseBaseHandler& responseHandler) override;
    void DeviceAiSearchCancel(const RequestBaseSharedPtr& request,
                              const ResponseBaseHandler& responseHandler) override;
    void DeviceOcrSearch(const RequestBaseSharedPtr& request,
                              const ResponseBaseHandler& responseHandler) override;
    void DeviceUrlSnapShot(const RequestBaseSharedPtr& request,
                              const ResponseBaseHandler& responseHandler) override;
    void DeviceSmartSearch(const RequestBaseSharedPtr& request,
                              const ResponseBaseHandler& responseHandler) override;
    void DeviceSmartSearchCancel(const RequestBaseSharedPtr& request,
                              const ResponseBaseHandler& responseHandler) override;
    void DeviceTextSearch(const RequestBaseSharedPtr &request,
                          const ResponseBaseHandler &responseHandler) override;
    void DeviceTextPosConf(const RequestBaseSharedPtr &request,
                           const ResponseBaseHandler &responseHandler) override;
    void DeviceStorageInfo(const RequestBaseSharedPtr &request,
                           const ResponseBaseHandler &responseHandler) override;
    void DeviceGetPasswordExpiry(const RequestBaseSharedPtr &request,
                           const ResponseBaseHandler &responseHandler) override;
    void DeviceExtendPasswordExpiration(const RequestBaseSharedPtr &request,
                                 const ResponseBaseHandler &responseHandler) override;
    void DeviceTryConnectingS1DDNS(const RequestBaseSharedPtr &request,
                                   const ResponseBaseHandler &responseHandler) override;

private:
    void processEvent(const EventBaseSharedPtr& event);
    void SetAlertAlarmAction();
    void SetAlarmOutAction();
    void SetLayoutChangeAction();
    void SetEmailChangeAction();

    void SetWriteToLogAction();
    void connectAllDevices();
    device::BaseDeviceClientSharedPtr& getDevice(const std::string& deviceUUID);
    device::BaseDeviceClientSharedPtr  createDevice(const std::string& deviceUUID, const device::DeviceProtocolType deviceProtocolType);

    boost::asio::io_context& deviceIoContext();
    boost::asio::io_context& streamIoContext();
    boost::asio::io_context& discoveryIoContext();
    boost::asio::io_context& commandIoContext();

    void safeResponseHandler(const ResponseBaseSharedPtr& response,
                             ResponseBaseHandler const & responseHandler);

    void byPassToDevice(Wisenet::Device::memFuncType func,
                        device::DeviceRequestBaseSharedPtr const& request,
                        ResponseBaseHandler const& responseHandler);

    void restartBookmarkAutoRemoveTimer();
    void restartLogAutoRemoveTimer();
    void restartAlarmOutActionTimer();
    //void restartDashboardDataProduceTimer();

private:
    std::shared_ptr<ThreadPool> m_deviceIoContextPool;
    std::shared_ptr<ThreadPool> m_streamIoContextPool;
    std::shared_ptr<ThreadPool> m_discoveryIoContextPool;
    std::shared_ptr<ThreadPool> m_commandContextPool;
    std::shared_ptr<ThreadPool> m_logContextPool;
    std::shared_ptr<ThreadPool> m_bookmarkAutoRemoveTimerContext;
    std::shared_ptr<ThreadPool> m_logAutoRemoveTimerContext;
    std::shared_ptr<ThreadPool> m_alarmOutActionTimerContext;
    //std::shared_ptr<ThreadPool> m_dashboardDataProduceTimerContext;
    asio::strand<boost::asio::io_context::executor_type> m_strand;

    std::shared_ptr<DeviceDiscovery>    m_discovery;
    std::shared_ptr<IPv6DeviceDiscovery>    m_ipv6discovery;
    std::shared_ptr<IpSetting>          m_ipSetting;
    std::shared_ptr<InitPassword>       m_initPassword;
    std::shared_ptr<DatabaseManager>    m_databaseManager;
    std::shared_ptr<LogManager>         m_logManager;
    std::shared_ptr<EventRuleManager>   m_eventRuleManager;
    std::shared_ptr<PTZLogManager>      m_ptzLogManager;
    std::shared_ptr<AlarmOutActionManager> m_alarmOutputActionManager;
    //std::shared_ptr<DashboardManager>   m_dashboardManager;
    std::shared_ptr<LdapManager>        m_ldapManager;

    std::shared_ptr<Wisenet::Library::CloudService> m_cloudService;

    DeviceClientMap m_devices;
    device::BaseDeviceClientSharedPtr m_dummyDevice; // for error processing
    std::shared_ptr<UserSession> m_userSession;
    CoreServiceConfig m_config;

    boost::asio::steady_timer m_bookmarkAutoRemoveTimer;
    bool m_bookmarkAutoRemoveTimerStarted = false;

    boost::asio::steady_timer m_logAutoRemoveTimer;
    boost::asio::steady_timer m_alarmOutActionTimer;
    //boost::asio::steady_timer m_dashboardDataProduceTimer;
    //int m_latestDashboardSaveHour = 0;

    std::map<std::string, LoginBlockedInfo> m_loginBlockedInfoMap;
    unsigned short m_s1DaemonPort = 1881;
};

}
}
