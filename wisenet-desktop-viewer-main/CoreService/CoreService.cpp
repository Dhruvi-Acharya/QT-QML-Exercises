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

#include "CoreService.h"
#include <QCoreApplication>
#include <QDir>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/algorithm/string.hpp>

#include "CoreServiceLogSettings.h"
#include "WeakCallback.h"
#include "DummyDevice.h"
#include "ThreadPool.h"
#include "DeviceDiscovery.h"
#include "IPv6DeviceDiscovery.h"
#include "IpSetting.h"
#include "InitPassword.h"
#include "CoreServiceUtil.h"

#include "DatabaseManager.h"
#include "EmailManager.h"
#include "LogManager.h"
#include "UserSession.h"
#include "EventRuleManager.h"
#include "PTZLogManager.h"
#include "AlarmOutActionManager.h"
#include "ResourceMonitor.h"
//#include "DashboardManager.h"
#include "LdapManager.h"
#include "TimeUtil.h"

#include "DeviceCertificate.h"
#include "EasyConnectionLogger.h"
#include "AppLocalDataLocation.h"
#include "DigestAuthenticator.h"

#ifdef WISENET_S1_VERSION
#include "S1Sip.h"
#endif

namespace Wisenet
{
namespace Core
{
using namespace std::placeholders;
CoreService::CoreService()
    : m_deviceIoContextPool(std::make_shared<ThreadPool>(16))
    , m_streamIoContextPool(std::make_shared<ThreadPool>(16))
    , m_discoveryIoContextPool(std::make_shared<ThreadPool>(2))
    , m_commandContextPool(std::make_shared<ThreadPool>(4))
    , m_logContextPool(std::make_shared<ThreadPool>(2))
    , m_bookmarkAutoRemoveTimerContext(std::make_shared<ThreadPool>(2))
    , m_logAutoRemoveTimerContext(std::make_shared<ThreadPool>(1))
    , m_alarmOutActionTimerContext(std::make_shared<ThreadPool>(1))
    //, m_dashboardDataProduceTimerContext(std::make_shared<ThreadPool>(1))
    , m_strand(m_commandContextPool->ioContext().get_executor())
    , m_discovery(std::make_shared<DeviceDiscovery>(m_discoveryIoContextPool->ioContext()))
    , m_ipv6discovery(std::make_shared<IPv6DeviceDiscovery>(m_discoveryIoContextPool->ioContext()))
    , m_ipSetting(std::make_shared<IpSetting>(m_discoveryIoContextPool->ioContext()))
    , m_initPassword(std::make_shared<InitPassword>(m_discoveryIoContextPool->ioContext()))
    , m_databaseManager(std::make_shared<DatabaseManager>())
    , m_logManager(std::make_shared<LogManager>(m_logContextPool->ioContext()))
    , m_eventRuleManager(std::make_shared<EventRuleManager>(*m_databaseManager))
    , m_ptzLogManager(std::make_shared<PTZLogManager>(*m_databaseManager,*m_logManager,m_logContextPool->ioContext()))
    , m_alarmOutputActionManager(std::make_shared<AlarmOutActionManager>())
    , m_cloudService(std::make_shared<Wisenet::Library::CloudService>())
    , m_dummyDevice(std::make_shared<device::DummyDevice>(m_commandContextPool->ioContext()))
    , m_userSession(std::make_shared<UserSession>(*m_databaseManager, *m_logManager))
    , m_bookmarkAutoRemoveTimer(m_bookmarkAutoRemoveTimerContext->ioContext())
    , m_logAutoRemoveTimer(m_logAutoRemoveTimerContext->ioContext())
    , m_alarmOutActionTimer(m_alarmOutActionTimerContext->ioContext())
    , m_ldapManager(std::make_shared<LdapManager>(*m_databaseManager))
    //, m_dashboardManager(std::make_shared<DashboardManager>())
    //, m_dashboardDataProduceTimer(m_dashboardDataProduceTimerContext->ioContext())

{
    SPDLOG_INFO("Create a new CoreService instance");
}

CoreService::~CoreService()
{
    SPDLOG_INFO("Destroy a CoreService instance");
    if (IsRunning())
        Stop();
}


boost::asio::io_context &CoreService::deviceIoContext()
{
    return m_deviceIoContextPool->ioContext();
}
boost::asio::io_context &CoreService::streamIoContext()
{
    return m_streamIoContextPool->ioContext();
}
boost::asio::io_context &CoreService::discoveryIoContext()
{
    return m_discoveryIoContextPool->ioContext();
}
boost::asio::io_context &CoreService::commandIoContext()
{
    return m_commandContextPool->ioContext();
}

void CoreService::safeResponseHandler(const ResponseBaseSharedPtr &response,
                                      const ResponseBaseHandler &responseHandler)
{
    if (responseHandler)
        responseHandler(response);
}


void CoreService::Start()
{
    SPDLOG_INFO("<==============START CORE SERVICE=============>");
    SPDLOG_INFO("Start to CoreService, check thread count:: app_name={}, device_io={}, streamming_io={}, command_io={}, discovery_io={}",
                QCoreApplication::applicationName().toStdString(),
                m_deviceIoContextPool->threadCount(),
                m_streamIoContextPool->threadCount(),
                m_commandContextPool->threadCount(),
                m_discoveryIoContextPool->threadCount());

    m_deviceIoContextPool->Start();
    m_streamIoContextPool->Start();
    m_discoveryIoContextPool->Start();
    m_commandContextPool->Start();
    m_bookmarkAutoRemoveTimerContext->Start();
    m_logAutoRemoveTimerContext->Start();
    m_alarmOutActionTimerContext->Start();
    //m_dashboardDataProduceTimerContext->Start();

    m_config.Read(GetAppDataLocation());

    if(!m_databaseManager->Open()) {
        SPDLOG_ERROR("Failed to open a management database.");
    }
    m_logContextPool->Start();
    if(!m_logManager->Open()) {
        SPDLOG_ERROR("Failed to open a log database.");
    }

    /*
    if(!m_dashboardManager->Open()) {
        SPDLOG_ERROR("Failed to open a log database.");
    }
    */

    m_ptzLogManager->Start();

    std::vector<std::string> rootCA;

    std::string defaultPath = QCoreApplication::applicationDirPath().toLocal8Bit().toStdString();

    SPDLOG_INFO("Login to CoreService--- defaultPath:{}", defaultPath);
    rootCA.push_back( defaultPath+ "/RootCA/HTW_rootca.crt");
    rootCA.push_back(defaultPath + "/RootCA/HTW_rootca_2.crt");
    rootCA.push_back(defaultPath + "/RootCA/HTW_rootca_3.crt");

    DeviceCertificate::getInstance().SetRootCA(rootCA);
    DeviceCertificate::getInstance().CheckRootCA();

    EmailManager::Initialize();
    EmailManager::Instance()->StartTask();

    //EventAction 설정.
    SetAlertAlarmAction();
    SetAlarmOutAction();
    SetWriteToLogAction();
    SetLayoutChangeAction();
    SetEmailChangeAction();

    //EventRule 설정.
    m_eventRuleManager->SetEventRules(m_databaseManager->GetEventRules());

    //CloudUser 설정.
    auto cloudInformation = m_databaseManager->GetCloudInformation();
    Wisenet::Library::CloudUser cloudUser;
    cloudUser.cloudUserID = cloudInformation.cloudID;
    cloudUser.userName = cloudInformation.userName;
    cloudUser.password = cloudInformation.password;

    //CloudService Start
    m_cloudService->Start(m_config.GetCloudConfig(),cloudUser, [this](const Wisenet::Library::CloudUser& user){

        SPDLOG_DEBUG("Changed User Inforation of Cloud");

        GetCloudInformationResponse res;
        res.cloudID = user.cloudUserID;
        res.userName = user.userName;
        res.password = user.password;

        asio::post(m_strand,
                   WeakCallback(shared_from_this(),
                                [this, res]()
        {
            m_databaseManager->SaveCloudInformation(res);

        }));

    });

    //EasyConnectionLogger 설정
    unsigned int logLevel = m_config.GetEasyConnectionLogLevel();
    Wisenet::Device::EasyConnectionLogger::getInstance().SetLog(logLevel, WeakCallback(shared_from_this(),
                                                                                       [logLevel](char* log)
    {
        if(logLevel > 0){
            spdlog::debug("[EASYCONNECTION] ---- {}",log);
        }
    }));

    // 북마크 자동 삭제 타이머 시작
    restartBookmarkAutoRemoveTimer();

    // 로그 삭제 타이머 시작
    restartLogAutoRemoveTimer();

    // 알람 아웃 액션 타이머 시작
    restartAlarmOutActionTimer();

    // Dashboard data 저장 타이머 시작
    //restartDashboardDataProduceTimer();

    // DB에 등록된 디아비스 접속 시작.
    connectAllDevices();

    SPDLOG_DEBUG("CoreService has been started successfully");
}

void CoreService::connectAllDevices()
{
    SPDLOG_INFO(">> Start to connect all devices");

    asio::post(m_strand,
               WeakCallback(shared_from_this(),
                            [this]()
    {
        // 로그인 성공 시 등록 된 모든 장비에 접속 (Test Code)
        auto allDevices = m_databaseManager->GetDevices();
        for(auto& device : allDevices)
        {
            auto itr = m_devices.find(device.first);
            if (itr != m_devices.end())
                continue;

            auto clientPtr = createDevice(device.first, device.second.connectionInfo.deviceProtocolType);
            if (clientPtr == nullptr)
                continue;

            m_devices[device.first] = clientPtr;

            if(!device.second.connectionInfo.use)
                continue;

            // try to connect to the device
            auto connectRequest = std::make_shared<Device::DeviceConnectRequest>();
            connectRequest->deviceID = device.first;
            connectRequest->connectionInfo = device.second.connectionInfo;
            connectRequest->addDevice = false;
            connectRequest->tryToDdnsInDDNSAndP2P = false;
            connectRequest->connectionInfo.usageDtlsMode = device.second.connectionInfo.usageDtlsMode;

            SPDLOG_DEBUG("connectAllDevices() dtls={}", device.second.connectionInfo.usageDtlsMode);

            // CoreService의 DeviceConnect를 호출하는 것으로 코드 정리
            DeviceConnect(connectRequest, nullptr);
        }
    }));
}
void CoreService::Stop()
{
    SPDLOG_INFO("Stop CoreService");

    m_ptzLogManager->Stop();

    SPDLOG_INFO("Stopped PTZLogManager");

    m_userSession->LogOut();

    SPDLOG_INFO("LogOut Usersession");

    m_bookmarkAutoRemoveTimer.cancel();

    SPDLOG_INFO("Canceled bookmarkAutoRemoveTimer");

    m_logAutoRemoveTimer.cancel();

    SPDLOG_INFO("Canceled logAutoRemoveTimer");

    m_alarmOutActionTimer.cancel();

    SPDLOG_INFO("Canceled alarmoutActionTimer");

    //m_dashboardDataProduceTimer.cancel();

    //SPDLOG_INFO("Canceled dashboardDataProduceTimer");

    m_logManager->Close();
    SPDLOG_INFO("Closed m_logManager");

    m_logContextPool->Stop();
    SPDLOG_INFO("Closed m_logContextPool");

    for (const auto& kv : m_devices) {
        kv.second->Release();
    }

    bool isAllReleased = false;
    auto startTimeSecs = Wisenet::Common::currentUtcSecs();
    while(!isAllReleased) {

        isAllReleased = true;
        for (const auto& kv : m_devices) {
            if (!kv.second->IsReleased()) {
                isAllReleased = false;
                break;
            }
        }

        if (!isAllReleased) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        auto elapsed = Wisenet::Common::currentUtcSecs() - startTimeSecs;
        
        if ((elapsed > 30) || (elapsed < 0)) {
            SPDLOG_INFO("Waiting Time exceeded in all device release. Elapsed Time Sec({})",elapsed);
            break;
        }
    };

    EmailManager::Instance()->StopTask();

    SPDLOG_INFO("Released m_devices");
    m_devices.clear();
    SPDLOG_INFO("Cleared m_devices");

#ifdef WISENET_S1_VERSION
    S1Sip sip(m_deviceIoContextPool->ioContext(), m_s1DaemonPort);
    sip.StopService();
#endif

    m_deviceIoContextPool->Stop();
    SPDLOG_INFO("Stoped m_deviceIoContextPool");

    m_streamIoContextPool->Stop();
    SPDLOG_INFO("Stoped m_streamIoContextPool");

    m_discoveryIoContextPool->Stop();
    SPDLOG_INFO("Stoped m_discoveryIoContextPool");

    m_commandContextPool->Stop();
    SPDLOG_INFO("Stoped m_commandContextPool");

    m_bookmarkAutoRemoveTimerContext->Stop();
    SPDLOG_INFO("Stoped m_bookmarkAutoRemoveTimerContext");

    m_logAutoRemoveTimerContext->Stop();
    SPDLOG_INFO("Stoped m_logAutoRemoveTimerContext");

    m_alarmOutActionTimerContext->Stop();
    SPDLOG_INFO("Stoped m_alarmOutActionTimerContext");

    //m_dashboardDataProduceTimerContext->Stop();
    //SPDLOG_INFO("Stoped m_dashboardDataProduceTimerContext");

    m_databaseManager->Close();
    SPDLOG_INFO("Closed m_databaseManager");

    //m_dashboardManager->Close();
    //SPDLOG_INFO("Closed m_dashboardManager");

    m_cloudService->Stop();
    SPDLOG_DEBUG("CoreService has stopped successfully");
    SPDLOG_INFO("<------------------STOP CORE SERVICE------------------->");
}

bool CoreService::IsRunning()
{
    return m_commandContextPool->IsRunning();
}

void CoreService::SetS1DaemonPort(unsigned short port)
{
    m_s1DaemonPort = port;
}

void CoreService::GetServiceInformation(const RequestBaseSharedPtr &request,
                                        const ResponseBaseHandler &responseHandler)
{
    SPDLOG_DEBUG("GetServiceInformation");
    auto req = std::static_pointer_cast<GetServiceInformationRequest>(request);
    auto res = std::make_shared<GetServiceInformationResponse>();

    asio::post(m_strand,
               WeakCallback(shared_from_this(),
                            [this, req, res, responseHandler]()
    {
        *res = m_databaseManager->GetServiceInformation();
        safeResponseHandler(res, responseHandler);
    }));
}

void CoreService::SetupService(const RequestBaseSharedPtr &request,
                               const ResponseBaseHandler &responseHandler)
{
    SPDLOG_DEBUG("SetupService");
    auto req = std::static_pointer_cast<SetupServiceRequest>(request);
    auto res = std::make_shared<ResponseBase>();

    asio::post(m_strand,
               WeakCallback(shared_from_this(),
                            [this, req, res, responseHandler]()
    {
        res->errorCode = m_databaseManager->SetupService(req->userID, req->newPassword);

        safeResponseHandler(res, responseHandler);

    }));
}


void CoreService::Login(RequestBaseSharedPtr const& request,
                        ResponseBaseHandler const& responseHandler)
{
    auto req = std::static_pointer_cast<LoginRequest>(request);
    auto res = std::make_shared<LoginResponse>();

    SPDLOG_INFO("Login to CoreService, Login ID={} ", req->userName);

    asio::post(m_strand,
               WeakCallback(shared_from_this(),
                            [this, req, res, responseHandler]()
    {
        // Locked 처리
        if(m_loginBlockedInfoMap.find(req->userName) != m_loginBlockedInfoMap.end())
        {
            LoginBlockedInfo& info = m_loginBlockedInfoMap[req->userName];

            long long now = std::time(nullptr);
            SPDLOG_INFO("Account id={} now={} timeStamp={} blocked={}", req->userName, now, info.blockEndTimeStamp, info.blockedCount);

            if(info.blockEndTimeStamp > now && info.blockedCount >= 5)
            {
                res->errorCode = ErrorCode::AccountBlocked;
                res->lockedSeconds = info.blockEndTimeStamp - now + 1; // 바로하면 29초로 출력돼서 1더함.
                safeResponseHandler(res, responseHandler);

                return;
            }
        }

        res->errorCode = m_databaseManager->LogIn(res->userId, req->userName,  req->password, req->isHashed);

        // LDAP 유저일 경우 ldapManager로 로그인 시도
        if (res->errorCode == ErrorCode::LdapUser) {
            SPDLOG_INFO("Account id={} is a LDAP user", req->userName);
            res->isLdapUser = true;
            Wisenet::ErrorCode errorCode = m_ldapManager->Login(req->userName, req->password);
            if (errorCode == ErrorCode::LdapSuccess)
                res->errorCode = ErrorCode::NoError;
            else
                res->errorCode = errorCode;
        }

        if (res->isSuccess()) {
            // 로그인이 되는 경우 UserSession 생성.
            m_userSession = std::make_shared<UserSession>(*m_databaseManager, *m_logManager, req->userName);

            if(m_loginBlockedInfoMap.find(req->userName) != m_loginBlockedInfoMap.end())
            {
                m_loginBlockedInfoMap.erase(req->userName);
            }
        }
        else {
            // 로그인이 안되는 경우 AuditLog 생성.
            AuditLog log;
            log.auditLogID = boost::uuids::to_string(boost::uuids::random_generator()());
            log.sessionID = boost::uuids::to_string(boost::uuids::random_generator()());
            if(m_databaseManager->IsExistUser(req->userName)) {
                log.userName = req->userName;
            }else {
                log.userName = ""; // 등록된 user가 아닌 경우 공란으로 넣는다.
            }

            log.host = "127.0.0.1";
            log.actionType = "User Login";
            log.serviceUtcTimeMsec = Wisenet::Common::currentUtcMsecs();
            log.itemType = AuditLog::ItemType::Service;

            AuditLogLogInActionDetail detail;
            detail.isSuccess = false;
            detail.startUtcTimeMsec = log.serviceUtcTimeMsec;
            detail.endUtcTimeMsec = log.serviceUtcTimeMsec;

            JsonWriter writer;
            writer & detail;
            log.actionDetail = writer.GetString();

            m_logManager->SaveAuditLogInMemory(log);

            // Lock status 추가
            if(m_loginBlockedInfoMap.find(req->userName) != m_loginBlockedInfoMap.end())
            {
                long long result = std::time(nullptr);

                LoginBlockedInfo& info = m_loginBlockedInfoMap[req->userName];

                info.blockedCount++;
                if(info.blockedCount >= 5)
                    info.blockEndTimeStamp = result + 30;

                SPDLOG_INFO("Lock id={} timeStamp={} blocked={}", req->userName, info.blockEndTimeStamp, info.blockedCount);
            }
            else
            {
                LoginBlockedInfo info;
                info.blockedCount++;

                m_loginBlockedInfoMap.emplace(req->userName, info);
            }
        }
        safeResponseHandler(res, responseHandler);
    }));
}

void CoreService::Logout(RequestBaseSharedPtr const& request,
                         ResponseBaseHandler const& responseHandler)
{
    SPDLOG_INFO("Logout to CoreService Login ID={}", request->userName);

    auto req = std::static_pointer_cast<LogoutRequest>(request);
    auto res = std::make_shared<ResponseBase>();

    asio::dispatch(m_strand,
                   WeakCallback(shared_from_this(),
                                [this, res, responseHandler]()
    {
        m_userSession = std::make_shared<UserSession>(*m_databaseManager, *m_logManager);
        safeResponseHandler(res, responseHandler);
    }));
}


void CoreService::StartEventService(RequestBaseSharedPtr const& request,
                                    ResponseBaseHandler const& responseHandler)
{
    SPDLOG_DEBUG("StartEventService");
    auto req = std::static_pointer_cast<StartEventServiceRequest>(request);
    auto res = std::make_shared<ResponseBase>();

    asio::post(m_strand,
               WeakCallback(shared_from_this(),
                            [this, req, res, responseHandler]()
    {
        m_userSession->SetUserEventHandler(req->eventHandler);
        safeResponseHandler(res, responseHandler);
        asio::post(m_strand, WeakCallback(shared_from_this(), [this]()
        {
            m_userSession->ProcessFullInfoEvent();
        }));
    }));
}


void CoreService::SaveUserGroup(const RequestBaseSharedPtr &request,
                                const ResponseBaseHandler &responseHandler)
{
    auto req = std::static_pointer_cast<SaveUserGroupRequest>(request);
    asio::post(m_strand,
               WeakCallback(shared_from_this(),
                            [this, req, responseHandler]()
    {
        auto res = m_userSession->SaveUserGroup(req);
        safeResponseHandler(res, responseHandler);
        if (res->isSuccess()) {
            auto event = std::make_shared<SaveUserGroupEvent>();
            event->userGroup = req->userGroup;
            processEvent(event);
        }
    }));
}

void CoreService::RemoveUserGroups(const RequestBaseSharedPtr &request,
                                   const ResponseBaseHandler &responseHandler)
{
    auto req = std::static_pointer_cast<RemoveUserGroupsRequest>(request);
    asio::post(m_strand,
               WeakCallback(shared_from_this(),
                            [this, req, responseHandler]()
    {
        std::vector<uuid_string> removedUserIDs;
        std::vector<uuid_string> removedLayoutIDs;
        std::vector<uuid_string> removedSequenceIDs;

        auto res = m_userSession->RemoveUserGroups(req,
                                                   removedUserIDs,
                                                   removedLayoutIDs,
                                                   removedSequenceIDs);
        safeResponseHandler(res, responseHandler);

        if(removedLayoutIDs.size() > 0 ){

            auto removeLayoutsEvent = std::make_shared<RemoveLayoutsEvent>();

            removeLayoutsEvent->layoutIDs = removedLayoutIDs;
            processEvent(removeLayoutsEvent);
        }

        if(removedUserIDs.size() > 0 ){

            auto removeUsersEvent = std::make_shared<RemoveUsersEvent>();

            removeUsersEvent->userIDs = removedUserIDs;
            processEvent(removeUsersEvent);
        }

        if (res->isSuccess()) {
            auto event = std::make_shared<RemoveUserGroupsEvent>();
            event->userGroupIDs = req->userGroupIDs;
            processEvent(event);
        }
    }));
}

void CoreService::SaveUser(RequestBaseSharedPtr const& request,
                           ResponseBaseHandler const& responseHandler)
{
    auto req = std::static_pointer_cast<SaveUserRequest>(request);
    asio::post(m_strand,
               WeakCallback(shared_from_this(),
                            [this, req, responseHandler]()
    {
        auto res = m_userSession->SaveUser(req);
        safeResponseHandler(res, responseHandler);
        if (res->isSuccess()) {
            auto event = std::make_shared<SaveUserEvent>();
            event->user = req->user;
            processEvent(event);
        }
    }));
}

void CoreService::RemoveUsers(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto req = std::static_pointer_cast<RemoveUsersRequest>(request);
    asio::post(m_strand,
               WeakCallback(shared_from_this(),
                            [this, req, responseHandler]()
    {
        std::vector<uuid_string> removedLayoutIDs;
        std::vector<uuid_string> removedSequenceIDs;
        auto res = m_userSession->RemoveUsers(req,
                                              removedLayoutIDs,
                                              removedSequenceIDs);
        safeResponseHandler(res, responseHandler);

        if(removedLayoutIDs.size() > 0 ){

            auto removeLayoutsEvent = std::make_shared<RemoveLayoutsEvent>();

            removeLayoutsEvent->layoutIDs = removedLayoutIDs;
            processEvent(removeLayoutsEvent);
        }

        if (res->isSuccess()) {
            auto event = std::make_shared<RemoveUsersEvent>();
            event->userIDs = req->userIDs;
            processEvent(event);
        }
    }));
}

void CoreService::SaveGroup(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto req = std::static_pointer_cast<SaveGroupRequest>(request);
    asio::post(m_strand,
               WeakCallback(shared_from_this(),
                            [this, req, responseHandler]()
    {
        auto res = m_userSession->SaveGroup(req);
        safeResponseHandler(res, responseHandler);
        if (res->isSuccess()) {
            auto event = std::make_shared<SaveGroupEvent>();
            event->group = req->group;
            processEvent(event);
        }
    }));
}

void CoreService::RemoveGroups(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto req = std::static_pointer_cast<RemoveGroupsRequest>(request);
    asio::post(m_strand,
               WeakCallback(shared_from_this(),
                            [this, req, responseHandler]()
    {
        std::vector<uuid_string> removedGroups;
        std::map<uuid_string, std::vector<std::string>> removedMappings;

        auto res = m_userSession->RemoveGroups(req, removedGroups, removedMappings);
        safeResponseHandler(res, responseHandler);
        if (res->isSuccess()) {

            for (auto& kvp : removedMappings)
            {
                auto mappingEvent = std::make_shared<RemoveGroupMappingEvent>();
                mappingEvent->channelIDs = kvp.second;
                mappingEvent->groupID = kvp.first;

                if(mappingEvent->channelIDs.size() > 0)
                    processEvent(mappingEvent);
            }

            auto event = std::make_shared<RemoveGroupsEvent>();
            event->groupIDs = req->groupIDs;
            processEvent(event);
        }
    }));
}

void CoreService::AddGroupMapping(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto req = std::static_pointer_cast<AddGroupMappingRequest>(request);
    asio::post(m_strand,
               WeakCallback(shared_from_this(),
                            [this, req, responseHandler]()
    {
        auto res = m_userSession->AddGroupMapping(req);
        safeResponseHandler(res, responseHandler);
        if (res->isSuccess()) {
            auto event = std::make_shared<AddGroupMappingEvent>();
            event->groupID = req->groupID;
            event->channels = req->channels;
            processEvent(event);
        }
    }));
}

void CoreService::RemoveGroupMapping(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto req = std::static_pointer_cast<RemoveGroupMappingRequest>(request);
    asio::post(m_strand,
               WeakCallback(shared_from_this(),
                            [this, req, responseHandler]()
    {
        auto res = m_userSession->RemoveGroupMapping(req);
        safeResponseHandler(res, responseHandler);
        if (res->isSuccess()) {
            auto event = std::make_shared<RemoveGroupMappingEvent>();
            event->groupID = req->groupID;
            event -> channelIDs = req->channelIDs;
            processEvent(event);
        }
    }));
}

void CoreService::UpdateGroupMapping(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto req = std::static_pointer_cast<UpdateGroupMappingRequest>(request);
    asio::post(m_strand,
               WeakCallback(shared_from_this(),
                            [this, req, responseHandler]()
    {
        auto res = m_userSession->UpdateGroupMapping(req);
        safeResponseHandler(res, responseHandler);
        if (res->isSuccess()) {
            auto event = std::make_shared<UpdateGroupMappingEvent>();
            event->from = req->from;
            event->to = req->to;
            event->channelIDs = req->channelIDs;
            processEvent(event);
        }
    }));
}

void CoreService::AddGroupWithUpdateMapping(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto req = std::static_pointer_cast<AddGroupWithUpdateMappingRequest>(request);
    asio::post(m_strand,
               WeakCallback(shared_from_this(),
                            [this, req, responseHandler]()
    {
        auto res = m_userSession->AddGroupWithUpdateMapping(req);
        safeResponseHandler(res, responseHandler);

        QUuid emptyUuid;
        std::string emptyUuidString = emptyUuid.toString(QUuid::WithoutBraces).toStdString();

        if (res->isSuccess()) {
            auto groupAddEvent = std::make_shared<SaveGroupEvent>();
            groupAddEvent->group = req->group;
            processEvent(groupAddEvent);

            for (auto& kvp : req->groupChannelMapping)
            {
                if(kvp.first == emptyUuidString)
                {
                    auto addMappingEvent = std::make_shared<AddGroupMappingEvent>();
                    addMappingEvent->groupID = req->group.groupID;
                    addMappingEvent->channels = kvp.second;
                    processEvent(addMappingEvent);
                }
                else
                {
                    auto updateMappingEvent = std::make_shared<UpdateGroupMappingEvent>();
                    updateMappingEvent->from = kvp.first;
                    updateMappingEvent->to = req->group.groupID;

                    for(auto& item : kvp.second)
                    {
                        updateMappingEvent->channelIDs.push_back(item.id);
                    }
                    processEvent(updateMappingEvent);
                }
            }
        }
    }));
}

void CoreService::SaveLayout(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto req = std::static_pointer_cast<SaveLayoutRequest>(request);
    asio::post(m_strand,
               WeakCallback(shared_from_this(),
                            [this, req, responseHandler]()
    {
        auto res = m_userSession->SaveLayout(req);
        safeResponseHandler(res, responseHandler);
        if (res->isSuccess()) {
            auto event = std::make_shared<SaveLayoutEvent>();
            event->layout = req->layout;
            processEvent(event);
        }
    }));
}

void CoreService::RemoveLayouts(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto req = std::static_pointer_cast<RemoveLayoutsRequest>(request);
    asio::post(m_strand,
               WeakCallback(shared_from_this(),
                            [this, req, responseHandler]()
    {
        std::map<uuid_string, SequenceLayout> changedSequenceLayouts;
        std::map<uuid_string,UserGroup> changedUserGroups;
        auto res = m_userSession->RemoveLayouts(req,
                                                changedSequenceLayouts,
                                                changedUserGroups);
        safeResponseHandler(res, responseHandler);
        if (res->isSuccess()) {
            auto event = std::make_shared<RemoveLayoutsEvent>();
            event->layoutIDs = req->layoutIDs;
            processEvent(event);

            for (auto& kvp : changedSequenceLayouts) {
                auto saveSequenceEvent = std::make_shared<SaveSequenceLayoutEvent>();
                saveSequenceEvent->sequenceLayout = kvp.second;

                processEvent(saveSequenceEvent);
            }
        }
    }));
}

void CoreService::SaveSequenceLayout(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto req = std::static_pointer_cast<SaveSequenceLayoutRequest>(request);
    asio::post(m_strand,
               WeakCallback(shared_from_this(),
                            [this, req, responseHandler]()
    {
        auto res = m_userSession->SaveSequenceLayout(req);
        safeResponseHandler(res, responseHandler);
        if (res->isSuccess()) {
            auto event = std::make_shared<SaveSequenceLayoutEvent>();
            event->sequenceLayout = req->sequenceLayout;
            processEvent(event);
        }
    }));
}

void CoreService::RemoveSequenceLayouts(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto req = std::static_pointer_cast<RemoveSequenceLayoutsRequest>(request);
    asio::post(m_strand,
               WeakCallback(shared_from_this(),
                            [this, req, responseHandler]()
    {
        auto res = m_userSession->RemoveSequenceLayouts(req);
        safeResponseHandler(res, responseHandler);
        if (res->isSuccess()) {
            auto event = std::make_shared<RemoveSequenceLayoutsEvent>();
            event->sequenceLayoutIDs = req->sequenceLayoutIDs;
            processEvent(event);
        }
    }));
}

void CoreService::SaveWebpage(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto req = std::static_pointer_cast<SaveWebpageRequest>(request);
    asio::post(m_strand,
               WeakCallback(shared_from_this(),
                            [this, req, responseHandler]()
    {
        auto res = m_userSession->SaveWebpage(req);
        safeResponseHandler(res, responseHandler);
        if (res->isSuccess()) {
            auto event = std::make_shared<SaveWebpageEvent>();
            event->webpage = req->webpage;
            processEvent(event);
        }
    }));
}

void CoreService::RemoveWebpages(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto req = std::static_pointer_cast<RemoveWebpagesRequest>(request);
    asio::post(m_strand,
               WeakCallback(shared_from_this(),
                            [this, req, responseHandler]()
    {
        std::map<uuid_string, UserGroup> changedUserGroups;
        std::map<uuid_string, Layout> changedLayouts;
        std::map<uuid_string, std::vector<std::string>> removedGroupChannelMappings;
        auto res = m_userSession->RemoveWebpages(req,
                                                 changedUserGroups,
                                                 changedLayouts,
                                                 removedGroupChannelMappings);
        safeResponseHandler(res, responseHandler);
        if (res->isSuccess()) {
            // User group -- webpage 할당 시나리오가 없어 별 동작하지 않음. 추후 사용 가능
            if(changedUserGroups.size() > 0){
                for(auto& kvp : changedUserGroups){
                    auto saveUserGroupEvent = std::make_shared<SaveUserGroupEvent>();

                    saveUserGroupEvent->userGroup = kvp.second;
                    processEvent(saveUserGroupEvent);
                }
            }

            // Layout
            if(changedLayouts.size() > 0 ){
                auto saveLayoutsEvent = std::make_shared<SaveLayoutsEvent>();

                saveLayoutsEvent->layouts = changedLayouts;
                processEvent(saveLayoutsEvent);
            }

            // mapping 삭제 이벤트
            for(auto& kvp : removedGroupChannelMappings)
            {
                auto removeMappingEvent = std::make_shared<RemoveGroupMappingEvent>();
                removeMappingEvent->groupID = kvp.first;
                removeMappingEvent->channelIDs = kvp.second;
                processEvent(removeMappingEvent);
            }

            // webpage 삭제 이벤트
            auto event = std::make_shared<RemoveWebpagesEvent>();
            event->webpageIDs = req->webpageIDs;
            processEvent(event);
        }
    }));
}

void CoreService::DiscoverDevices(RequestBaseSharedPtr const& request,
                                  ResponseBaseHandler const& responseHandler)
{
    SPDLOG_INFO("Start to discover devices..");

    auto req = std::static_pointer_cast<DiscoverDevicesRequest>(request);
    asio::post(m_strand,
               WeakCallback(shared_from_this(),
                            [this, req, responseHandler]()
    {
        m_discovery->StartDiscovery(req, responseHandler);
    }));
}

void CoreService::DiscoverIPv6Devices(RequestBaseSharedPtr const& request,
                                      ResponseBaseHandler const& responseHandler)
{
    SPDLOG_INFO("Start to discover IPv6 devices..");

    auto req = std::static_pointer_cast<DiscoverDevicesRequest>(request);
    asio::post(m_strand,
               WeakCallback(shared_from_this(),
                            [this, req, responseHandler]()
    {
        m_ipv6discovery->StartDiscoveryIPv6(req, responseHandler);
    }));
}

void CoreService::IpSettingDevice(RequestBaseSharedPtr const& request,
                                  ResponseBaseHandler const& responseHandler)
{
    SPDLOG_INFO("Start to Ip Setting");

    auto req = std::static_pointer_cast<IpSettingDeviceRequest>(request);
    asio::post(m_strand, WeakCallback(shared_from_this(),
                                      [this, req, responseHandler]()
    {
        m_ipSetting->Change(req, responseHandler);
    }));
}

void CoreService::InitDevicePassword(RequestBaseSharedPtr const& request,
                                     ResponseBaseHandler const& responseHandler)
{
    SPDLOG_INFO("Start to Set Init Password");

    auto req = std::static_pointer_cast<InitDevicePasswordRequest>(request);
    asio::post(m_strand, WeakCallback(shared_from_this(), [this, req, responseHandler]()
    {
        m_initPassword->Set(req, responseHandler);
    }));
}

void CoreService::GetAuditLog(RequestBaseSharedPtr const& request,
                              ResponseBaseHandler const&responseHandler)
{
    SPDLOG_INFO("Start to get AuditLog");

    auto req = std::static_pointer_cast<GetAuditLogRequest>(request);
    asio::post(m_strand, WeakCallback(shared_from_this(), [this, req, responseHandler]()
    {
        m_userSession->GetAuditLog(req, responseHandler);
    }));
}

void CoreService::GetEventLog(RequestBaseSharedPtr const& request,
                              ResponseBaseHandler const&responseHandler)
{
    SPDLOG_INFO("Start to get EventLog");

    auto req = std::static_pointer_cast<GetEventLogRequest>(request);
    asio::post(m_strand, WeakCallback(shared_from_this(), [this, req, responseHandler]()
    {
        m_userSession->GetEventLog(req, responseHandler);
    }));
}

void CoreService::GetSystemLog(RequestBaseSharedPtr const& request,
                               ResponseBaseHandler const&responseHandler)
{
    SPDLOG_INFO("Start to get SystemLog");

    auto req = std::static_pointer_cast<GetSystemLogRequest>(request);
    asio::post(m_strand, WeakCallback(shared_from_this(), [this, req, responseHandler]()
    {
        m_userSession->GetSystemLog(req, responseHandler);
    }));
}

void CoreService::AddLog(RequestBaseSharedPtr const& request,
                         ResponseBaseHandler const& responseHandler)
{
    auto req = std::static_pointer_cast<AddLogRequest>(request);
    asio::post(m_strand,
               WeakCallback(shared_from_this(),
                            [this, req, responseHandler]()
    {
        for(auto eventLog : req->eventLogs)
        {
            auto eventLogPtr = std::make_shared<EventLog>();
            eventLogPtr->eventLogID = boost::uuids::to_string(boost::uuids::random_generator()());
            eventLogPtr->serviceUtcTimeMsec = eventLog.serviceUtcTimeMsec;
            eventLogPtr->type = eventLog.type;
            eventLogPtr->isService = eventLog.isService;
            eventLogPtr->parameters.data = eventLog.parameters.data;
            m_logManager->AddSystemLog(eventLogPtr);

            auto showSystemAlarmPanelEvent = std::make_shared<ShowSystemAlarmPanelEvent>();
            showSystemAlarmPanelEvent->serviceUtcTimeMsec = eventLog.serviceUtcTimeMsec;
            showSystemAlarmPanelEvent->intervalSeconds = 0;
            showSystemAlarmPanelEvent->eventLog = eventLog;
            m_userSession->ProcessEvent(showSystemAlarmPanelEvent);
        }
    }));
}

void CoreService::SaveBookmark(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto req = std::static_pointer_cast<SaveBookmarkRequest>(request);
    asio::post(m_strand,
               WeakCallback(shared_from_this(),
                            [this, req, responseHandler]()
    {
        auto res = m_userSession->SaveBookmark(req);
        safeResponseHandler(res, responseHandler);
        if (res->isSuccess()) {
            auto event = std::make_shared<SaveBookmarkEvent>();
            event->bookmark = req->bookmark;
            processEvent(event);
        }
    }));
}

void CoreService::RemoveBookmarks(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto req = std::static_pointer_cast<RemoveBookmarksRequest>(request);
    asio::post(m_strand,
               WeakCallback(shared_from_this(),
                            [this, req, responseHandler]()
    {
        auto res = m_userSession->RemoveBookmarks(req);
        safeResponseHandler(res, responseHandler);
        if (res->isSuccess()) {
            auto event = std::make_shared<RemoveBookmarksEvent>();
            event->bookmarkIDs = req->bookmarkIDs;
            processEvent(event);
        }
    }));
}

void CoreService::GetEventRule(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    SPDLOG_INFO("Start to get EventRule");

    auto req = std::static_pointer_cast<GetEventRuleRequest>(request);
    asio::post(m_strand, WeakCallback(shared_from_this(), [this, req, responseHandler]()
    {
        auto res = m_userSession->GetEventRule();
        safeResponseHandler(res, responseHandler);
    }));
}

void CoreService::SaveEventRule(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto req = std::static_pointer_cast<SaveEventRuleRequest>(request);

    SPDLOG_INFO("Start to save EventRule id:{} name:{}", req->eventRule.eventRuleID, req->eventRule.name);

    asio::post(m_strand, WeakCallback(shared_from_this(), [this, req, responseHandler]()
    {
        auto res = m_userSession->SaveEventRule(req);
        safeResponseHandler(res, responseHandler);
        if(res->isSuccess()) {

            m_eventRuleManager->SaveEventRule(req->eventRule);

            auto event = std::make_shared<SaveEventRuleEvent>();
            event->eventRule = req->eventRule;
            processEvent(event);
        }
    }));
}

void CoreService::RemoveEventRules(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    SPDLOG_INFO("Start to remove EventRules");

    auto req = std::static_pointer_cast<RemoveEventRulesRequest>(request);
    asio::post(m_strand, WeakCallback(shared_from_this(), [this, req, responseHandler]()
    {
        auto res = m_userSession->RemoveEventRules(req);
        safeResponseHandler(res, responseHandler);
        if(res->isSuccess()) {

            m_eventRuleManager->RemoveEventRule(req->eventRuleIDs);

            auto event = std::make_shared<RemoveEventRulesEvent>();
            event->eventRuleIDs = req->eventRuleIDs;
            processEvent(event);
        }
    }));
}

void CoreService::SaveEventSchedule(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto req = std::static_pointer_cast<SaveEventScheduleRequest>(request);

    SPDLOG_INFO("Start to save EventSchedule id:{} name:{}", req->eventSchedule.scheduleID, req->eventSchedule.scheduleName);

    asio::post(m_strand, WeakCallback(shared_from_this(), [this, req, responseHandler]()
    {
        auto res = m_userSession->SaveEventSchedule(req);
        safeResponseHandler(res, responseHandler);
        if(res->isSuccess()) {
            auto event = std::make_shared<SaveEventScheduleEvent>();
            event->eventSchedule = req->eventSchedule;
            processEvent(event);
        }
    }));

}

void CoreService::RemoveEventSchedules(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    SPDLOG_INFO("Start to remove EventRules");

    auto req = std::static_pointer_cast<RemoveEventSchedulesRequest>(request);
    asio::post(m_strand, WeakCallback(shared_from_this(), [this, req, responseHandler]()
    {
        auto res = m_userSession -> RemoveEventSchedules(req);
        safeResponseHandler(res, responseHandler);
        if(res->isSuccess()) {
            auto event = std::make_shared<RemoveEventSchedulesEvent>();
            event->eventScheduleIDs = req->eventScheduleIDs;
            processEvent(event);
        }
    }));
}

void CoreService::SaveEventEmail(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto req = std::static_pointer_cast<SaveEventEmailRequest>(request);

    SPDLOG_INFO("Start to save EventEmail Sender: {}", req->eventEmail->sender);

    asio::post(m_strand, WeakCallback(shared_from_this(), [this, req, responseHandler]()
    {
        auto res = m_userSession->SaveEventEmail(req);
        safeResponseHandler(res, responseHandler);
        if(res->isSuccess()) {
            auto event = std::make_shared<SaveEventEmailEvent>();
            event->eventEmail = req->eventEmail;
            processEvent(event);
        }
    }));
}

void CoreService::RemoveEventEmail(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    SPDLOG_INFO("Start to remove EventEmail");

    auto req = std::static_pointer_cast<RemoveEventEmailRequest>(request);
    asio::post(m_strand, WeakCallback(shared_from_this(), [this, req, responseHandler]()
    {
        auto res = m_userSession -> RemoveEventEmail(req);
        safeResponseHandler(res, responseHandler);
        if(res->isSuccess()) {
            auto event = std::make_shared<RemoveEventEmailEvent>();
            processEvent(event);
        }
    }));
}

void CoreService::UpdateLdapSetting(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto req = std::static_pointer_cast<UpdateLdapSettingRequest>(request);
    SPDLOG_INFO("Start to save Ldap Setting serverUrl:{}", req->ldapSetting.serverUrl);

    asio::post(m_strand, WeakCallback(shared_from_this(), [this, req, responseHandler]()
    {
        auto res = m_userSession->UpdateLdapSetting(req);
        safeResponseHandler(res, responseHandler);
        if(res->isSuccess()) {
            auto event = std::make_shared<UpdateLdapSettingEvent>();
            event->ldapSetting = req->ldapSetting;
            processEvent(event);
        }
    }));
}

/*
void CoreService::GetDashboardData(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    SPDLOG_INFO("Get DashboardData");

    auto req = std::static_pointer_cast<GetDashboardDataRequest>(request);

    asio::post(m_strand, WeakCallback(shared_from_this(), [this, req, responseHandler]()
    {
        auto res = std::make_shared<GetDashboardDataResponse>();

        //DB
        m_dashboardManager->GetDashboardData(res->dashboardData);


        //----------- for test
        QDateTime now = QDateTime::currentDateTime();
        QDateTime lastWeek = now.addDays(-6);
        QTime midnight = QTime(0,0,0,0);
        lastWeek.setTime(midnight);

        int nowSinceEpoch = (int)now.toSecsSinceEpoch();
        int lastWeekMidnightSinceEpoch = lastWeek.toSecsSinceEpoch();

        int skipTest = 0;
        while(lastWeekMidnightSinceEpoch < nowSinceEpoch)
        {
            std::srand(std::time(nullptr));
            DashboardData dashboardData;
            dashboardData.saveTime = lastWeekMidnightSinceEpoch;
            dashboardData.powerErrorCount = Wisenet::Core::random(1,5);
            dashboardData.diskErrorCount = Wisenet::Core::random(1,5);
            dashboardData.overloadErrorCount = Wisenet::Core::random(1,5);
            dashboardData.connectionErrorCount = Wisenet::Core::random(1,5);
            dashboardData.fanErrorCount = Wisenet::Core::random(1,5);
            dashboardData.disconnectedCameraCount = Wisenet::Core::random(30,60);

            lastWeekMidnightSinceEpoch += (3 * 60 * 60);

            if(skipTest < 5 || skipTest > 10)
                res->dashboardData.emplace(dashboardData.saveTime, dashboardData);
            skipTest++;
        }
        //----------- for test


        safeResponseHandler(res, responseHandler);
    }));
}
*/

void CoreService::SaveDevicesCredential(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    SPDLOG_INFO("SaveDevicesCredential");
    auto req = std::static_pointer_cast<SaveDevicesCredentialRequest>(request);
    asio::post(m_strand,
               WeakCallback(shared_from_this(),
                            [this, req, responseHandler]()
    {
        auto res = std::make_shared<SaveDevicesCredentialResponse>();
        auto resBase = m_userSession->SaveDevicesCredential(req);

        res->devices = req->devices;
        res->errorCode = resBase->errorCode;
        safeResponseHandler(res, responseHandler);
        if (res->isSuccess()) {
            SPDLOG_INFO("SaveDevicesCredential++");
            auto event = std::make_shared<SaveDevicesCredentialEvent>();
            SPDLOG_INFO("SaveDevicesCredential+++");
            event->devices = req->devices;
            processEvent(event);
            SPDLOG_INFO("SaveDevicesCredential++++");
        }
    }));
}

void CoreService::UpdateDevices(RequestBaseSharedPtr const & request, ResponseBaseHandler const & responseHandler)
{
    SPDLOG_INFO("UpdateDevices");
    auto req = std::static_pointer_cast<UpdateDevicesRequest>(request);
    asio::post(m_strand,
               WeakCallback(shared_from_this(),
                            [this, req, responseHandler]()
    {
        auto res = std::make_shared<UpdateDevicesResponse>();
        auto resBase = m_userSession->SaveDevices(req);

        res->devices = req->devices;
        res->errorCode = resBase->errorCode;

        safeResponseHandler(res, responseHandler);
        if (res->isSuccess()) {
            SPDLOG_INFO("UpdateDevices++");
            auto event = std::make_shared<UpdateDevicesEvent>();
            SPDLOG_INFO("UpdateDevices+++");
            event->devices = req->devices;
            processEvent(event);
            SPDLOG_INFO("UpdateDevices++++");
        }
    }));
}




void CoreService::SaveChannels(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    SPDLOG_INFO("SaveChannel");
    auto req = std::static_pointer_cast<SaveChannelsRequest>(request);
    asio::post(m_strand,
               WeakCallback(shared_from_this(),
                            [this, req, responseHandler]()
    {
        std::map<uuid_string, UserGroup> changedUserGroups;
        std::map<uuid_string, Layout> changedLayouts;
        std::vector<uuid_string> removedBookmarks;

        auto res = m_userSession->SaveChannels(req->channels,
                                               changedUserGroups,
                                               changedLayouts,
                                               removedBookmarks);

        safeResponseHandler(res, responseHandler);

        if (res->isSuccess()) {
            // User group
            if(changedUserGroups.size() > 0){

                for(auto& kvp : changedUserGroups){
                    auto saveUserGroupEvent = std::make_shared<SaveUserGroupEvent>();

                    saveUserGroupEvent->userGroup = kvp.second;
                    processEvent(saveUserGroupEvent);
                }
            }

            // Layout
            if(changedLayouts.size() > 0 ){
                auto saveLayoutsEvent = std::make_shared<SaveLayoutsEvent>();

                saveLayoutsEvent->layouts = changedLayouts;
                processEvent(saveLayoutsEvent);
            }

            // Bookmark
            if(removedBookmarks.size() > 0){
                auto removeBookmarksEvent = std::make_shared<RemoveBookmarksEvent>();

                removeBookmarksEvent->bookmarkIDs = removedBookmarks;
                processEvent(removeBookmarksEvent);
            }

            // save channels event
            auto event = std::make_shared<SaveChannelsEvent>();
            event->channels = req->channels;
            processEvent(event);
        }
    }));
}

void CoreService::GetDeviceProfile(const RequestBaseSharedPtr &request,
                                   ResponseBaseHandler const&responseHandler)
{
    SPDLOG_INFO("Start to get ProfileInfo");

    auto req = std::static_pointer_cast<GetDeviceProfileRequest>(request);
    asio::post(m_strand, WeakCallback(shared_from_this(), [this, req, responseHandler]()
    {
        auto res = std::make_shared<GetDeviceProfileResponse>();

        auto deviceProfiles = m_userSession->GetDeviceProfile();

        auto iter = deviceProfiles.find(req->deviceID);
        if(iter != deviceProfiles.end())
            res->profileInfo = iter->second;

        SPDLOG_INFO("GetDeviceProfile, deviceUUID={}", req->deviceID);

        safeResponseHandler(res, responseHandler);

    }));

}

/* TODO */
void CoreService::AddDevice(RequestBaseSharedPtr const & request,
                            ResponseBaseHandler const & responseHandler)
{
    auto req = std::static_pointer_cast<AddDeviceRequest>(request);
    auto res = std::make_shared<ResponseBase>();

    asio::post(m_strand,
               WeakCallback(shared_from_this(),
                            [this, req, res, responseHandler]()
    {
        SPDLOG_DEBUG("Add new device, deviceUUID={}", req->deviceID);

        if (!m_userSession->CheckAdminAuthority()) {
            /* TODO */
        }

        auto itr = m_devices.find(req->deviceID);
        if (itr != m_devices.end()) {
            SPDLOG_WARN("Already exist deviceUUID={}, do not register the device.",
                        req->deviceID);
            res->errorCode = Wisenet::ErrorCode::InvalidRequest;
            safeResponseHandler(res, responseHandler);
            return;
        }

        auto clientPtr = createDevice(req->deviceID, req->connectionInfo.deviceProtocolType);
        if (clientPtr == nullptr) {
            SPDLOG_WARN("Invalid deviceType deviceUUID={}, deviceProtocolType={}, do not register the device.",
                        req->deviceID, req->connectionInfo.deviceProtocolType);

            res->errorCode = Wisenet::ErrorCode::InvalidRequest;
            safeResponseHandler(res, responseHandler);
            return;
        }

        // try to connect to the device with a new low profile
        auto connectRequest = std::make_shared<Device::DeviceConnectRequest>();
        connectRequest->deviceID = req->deviceID;
        connectRequest->connectionInfo = req->connectionInfo;
        connectRequest->connectionInfo.mediaProtocolType = Wisenet::Device::MediaProtocolType::UNKNOWN; // 추가 중 MediaProtocolType을 결정한다.
        connectRequest->addDevice = true;

        clientPtr->DeviceConnect(connectRequest, WeakCallback(shared_from_this(),
                                                              [this, req, res, responseHandler, clientPtr]
                                                              (const ResponseBaseSharedPtr& response)
        {
            asio::dispatch(m_strand, WeakCallback(shared_from_this(),
                                                  [this, response, req, res, responseHandler, clientPtr]()
            {
                auto addDeviceReq = std::static_pointer_cast<AddDeviceRequest>(req);

                if (response->isFailed() &&
                        ((addDeviceReq->connectionForceInfo.forceRegister == false) ||
                         ((addDeviceReq->connectionForceInfo.forceRegister == true) && !(response->errorCode == Wisenet::ErrorCode::UnAuthorized))))
                {
                    clientPtr->Release();
                    res->errorCode = response->errorCode;
                    safeResponseHandler(res, responseHandler);
                    return;
                }

                Device::DeviceConnectResponseSharedPtr connectResponse =
                        std::static_pointer_cast<Device::DeviceConnectResponse>(response);

                if(response->isFailed()){
                    connectResponse->device.deviceID = addDeviceReq->deviceID;
                    connectResponse->device.connectionInfo = addDeviceReq->connectionInfo;
                    connectResponse->device.modelName = addDeviceReq->connectionForceInfo.modelName;
                    connectResponse->device.macAddress = addDeviceReq->connectionForceInfo.macAddress;
                    Device::Device::Channel channel;
                    channel.deviceID = connectResponse->device.deviceID;
                    channel.channelID = "1";

                    connectResponse->device.channels.emplace(channel.channelID, channel);

                    Device::DeviceStatus deviceStatus;
                    deviceStatus.deviceID = connectResponse->device.deviceID;
                    deviceStatus.status = Device::DeviceStatusType::Unauthorized;
                    Device::ChannelStatus channelStatus;
                    channelStatus.deviceID = connectResponse->device.deviceID;
                    channelStatus.channelID = "1";
                    channelStatus.status = Device::ChannelStatusType::Disconnected;
                    deviceStatus.channelsStatus.emplace(channelStatus.channelID, channelStatus);
                    connectResponse->deviceStatus = deviceStatus;
                }


                if(response->isSuccess())
                    AssignChannelProfileInfo(connectResponse->deviceProfileInfo,
                                             connectResponse->deviceStatus,
                                             connectResponse->device);

                res->errorCode = m_userSession->AddDevice(connectResponse);
                if (res->isFailed()) {
                    clientPtr->Release();
                    safeResponseHandler(res, responseHandler);
                    return;
                }

                m_devices[req->deviceID] = clientPtr;
                SPDLOG_INFO("Added new device successfully, deviceUUID={}", req->deviceID);

                // Response
                connectResponse->errorCode = res->errorCode;
                safeResponseHandler(connectResponse, responseHandler);

                // Event
                auto addEvent = std::make_shared<AddDeviceEvent>();
                addEvent->device = connectResponse->device;
                addEvent->deviceStatus = connectResponse->deviceStatus;
                processEvent(addEvent);


                // Make default group
                auto saveGroupRequest = std::make_shared<SaveGroupRequest>();
                QUuid emptyUuid;
                saveGroupRequest->group.parentGroupID = emptyUuid.toString(QUuid::WithoutBraces).toUtf8().constData();
                saveGroupRequest->group.groupID = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8().constData();
                saveGroupRequest->group.name = "Group_" + connectResponse->device.name;

                auto saveGroupResponse = m_userSession->SaveGroup(saveGroupRequest);
                if (saveGroupResponse->isSuccess()) {
                    auto event = std::make_shared<SaveGroupEvent>();
                    event->group = saveGroupRequest->group;
                    processEvent(event);
                }

                // Add group mapping
                auto addGroupMappingRequest = std::make_shared<AddGroupMappingRequest>();
                addGroupMappingRequest->groupID = saveGroupRequest->group.groupID;
                for (auto& channel : connectResponse->device.channels)
                {
                    if (channel.second.use) {
                        Group::MappingItem item;

                        item.id = channel.second.deviceID + "_" + channel.second.channelID;
                        item.mappingType = Group::MappingType::Camera;

                        addGroupMappingRequest->channels.push_back(item);
                    }
                }

                auto addGroupMappingResponse = m_userSession->AddGroupMapping(addGroupMappingRequest);
                if(addGroupMappingResponse->isSuccess()) {
                    auto event = std::make_shared<AddGroupMappingEvent>();
                    event->groupID = addGroupMappingRequest->groupID;
                    event->channels = addGroupMappingRequest->channels;
                    processEvent(event);
                }

                if(connectResponse->device.connectionInfo.connectedType == Wisenet::Device::ConnectedType::S1_SIP ||
                        connectResponse->device.connectionInfo.connectedType == Wisenet::Device::ConnectedType::S1_SIP_RELAY)
                {
                    auto tryConnectingS1DdnsRequest = std::make_shared<Wisenet::RequestBase>();
                    clientPtr->DeviceTryConnectingS1DDNS(tryConnectingS1DdnsRequest,
                                                         [=](const ResponseBaseSharedPtr& reply)
                    {
                        if(reply->isSuccess())
                        {
                            SPDLOG_INFO("AddDevice(): Initially ConnectedType S1_SIP, but S1DDNS getting success, change connectedType to S1DDNS");
                            auto s1DeviceConnectionTypeChangeEvent = std::make_shared<Wisenet::Device::S1DeviceConnectionTypeChangeEvent>();
                            s1DeviceConnectionTypeChangeEvent->device = connectResponse->device;
                            s1DeviceConnectionTypeChangeEvent->device.connectionInfo.connectedType = Wisenet::Device::ConnectedType::S1_DDNS;
                            processEvent(s1DeviceConnectionTypeChangeEvent);
                        }
                    });
                }
            }));
        }));
    }));
}

void CoreService::RemoveDevices(RequestBaseSharedPtr const & request,
                                ResponseBaseHandler const & responseHandler)
{
    auto req = std::static_pointer_cast<RemoveDevicesRequest>(request);
    auto res = std::make_shared<ResponseBase>();

    asio::post(m_strand,
               WeakCallback(shared_from_this(),
                            [this, req, res, responseHandler]()
    {
        std::map<uuid_string,UserGroup> changedUserGroups;
        std::map<uuid_string, Layout> changedLayouts;
        std::vector<uuid_string> removedBookmarks;

        // P2P 연결 장비 목록을 획득한다.
        std::vector<std::string> removeProductP2PIDs;
        for (auto deviceID : req->deviceIDs) {
            Wisenet::Device::Device device = m_userSession->GetDevice(deviceID);
            if (!device.connectionInfo.productP2PID.empty()) {
                removeProductP2PIDs.emplace_back(device.connectionInfo.productP2PID);
            }
        }

        res->errorCode = m_userSession->RemoveDevices(req->deviceIDs,
                                                      changedUserGroups,
                                                      changedLayouts,
                                                      removedBookmarks);
        if (res->isFailed()) {
            safeResponseHandler(res, responseHandler);
            return;
        }

        //P2P 제거 목록이 있으면 Cloud에 연결하여 제거한다.
        m_cloudService->AsyncDeleteDevice(removeProductP2PIDs);
        
        for (auto& removeID : req->deviceIDs) {
            auto itr = m_devices.find(removeID);
            if (itr != m_devices.end()) {
                SPDLOG_INFO("Remove device, deviceUUID={}", removeID);
                itr->second->Release();
                m_devices.erase(itr);
            }
            else {
                SPDLOG_WARN("Remove device error, can not find deviceUUID={}", removeID);
            }
        }

        // Response
        safeResponseHandler(res, responseHandler);

        // User group
        if(changedUserGroups.size() > 0){

            for(auto& kvp : changedUserGroups){
                auto saveUserGroupEvent = std::make_shared<SaveUserGroupEvent>();

                saveUserGroupEvent->userGroup = kvp.second;
                processEvent(saveUserGroupEvent);
            }
        }

        // Layout
        if(changedLayouts.size() > 0 ){
            auto saveLayoutsEvent = std::make_shared<SaveLayoutsEvent>();

            saveLayoutsEvent->layouts = changedLayouts;
            processEvent(saveLayoutsEvent);
        }

        // Bookmark
        if(removedBookmarks.size() > 0){
            auto removeBookmarksEvent = std::make_shared<RemoveBookmarksEvent>();

            removeBookmarksEvent->bookmarkIDs = removedBookmarks;
            processEvent(removeBookmarksEvent);
        }

        // Event
        auto removesEvent = std::make_shared<RemoveDevicesEvent>();
        removesEvent->deviceIDs = req->deviceIDs;
        processEvent(removesEvent);
    }));
}

void CoreService::BackupService(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    SPDLOG_INFO("BackupService");
    auto req = std::static_pointer_cast<BackupServiceRequest>(request);
    asio::post(m_strand,
               WeakCallback(shared_from_this(),
                            [this, req, responseHandler]()
    {
        auto res = m_userSession->BackupService(req);
        SPDLOG_INFO("BackupService result:", res->isSuccess());
        safeResponseHandler(res, responseHandler);
    }));
}

void CoreService::RestoreService(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    SPDLOG_INFO("RestoreService");
    auto req = std::static_pointer_cast<RestoreServiceRequest>(request);
    asio::post(m_strand,
               WeakCallback(shared_from_this(),
                            [this, req, responseHandler]()
    {
        auto res = m_userSession->RestoreService(req);
        safeResponseHandler(res, responseHandler);
        SPDLOG_INFO("RestoreService result:", res->isSuccess());
        if (res->isSuccess()) {
            auto event = std::make_shared<RestoreServiceEvent>();
            processEvent(event);
            SPDLOG_INFO("RestoreServiceEvent");
        }
    }));
}

void CoreService::ResetService(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    SPDLOG_INFO("ResetService");
    auto req = std::static_pointer_cast<ResetServiceRequest>(request);
    asio::post(m_strand,
               WeakCallback(shared_from_this(),
                            [this, req, responseHandler]()
    {
        auto res = m_userSession->ResetService(req);
        safeResponseHandler(res, responseHandler);
        if (res->isSuccess()) {
            auto event = std::make_shared<ResetServiceEvent>();
            processEvent(event);
            SPDLOG_INFO("ResetServiceEvent");
        }
    }));
}

void CoreService::LdapConnect(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    SPDLOG_INFO("LdapConnect");
    auto req = std::static_pointer_cast<LdapConnectRequest>(request);
    asio::post(m_strand,
               WeakCallback(shared_from_this(),
                            [this, req, responseHandler]()
    {
        auto res = m_ldapManager->LdapConnect(req);
        safeResponseHandler(res, responseHandler);
    }));
}

void CoreService::UpdateStatistics(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    SPDLOG_INFO("UpdateStatistics");
    auto req = std::static_pointer_cast<UpdateStatisticsRequest>(request);
    asio::post(m_strand,
               WeakCallback(shared_from_this(),
                            [this, req, responseHandler]()
    {
        auto res = m_userSession->UpdateStatistics(req);
        safeResponseHandler(res, responseHandler);
    }));
}

void CoreService::ActivateLicense(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    SPDLOG_INFO("ActivateLicense");
    auto req = std::static_pointer_cast<ActivateLicenseRequest>(request);
    asio::post(m_strand,
               WeakCallback(shared_from_this(),
                            [this, req, responseHandler]()
    {
        auto res = m_userSession->ActivateLicense(req);
        safeResponseHandler(res, responseHandler);

        // Event
        auto activateLicenseEvent = std::make_shared<ActivateLicenseEvent>();
        activateLicenseEvent->licenseInfo = m_userSession->GetLicenseInfo();
        processEvent(activateLicenseEvent);
    }));
}

void CoreService::UpdateSoftwareService(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    SPDLOG_INFO("UpdateSoftwareService");
    auto req = std::static_pointer_cast<UpdateSoftwareServiceRequest>(request);
    asio::post(m_strand,
               WeakCallback(shared_from_this(),
                            [this, req, responseHandler]()
    {
        auto res = m_userSession->UpdateSoftwareService(req);
        safeResponseHandler(res, responseHandler);
    }));
}

void CoreService::restartBookmarkAutoRemoveTimer()
{
    if(!m_bookmarkAutoRemoveTimerStarted)
    {
        m_bookmarkAutoRemoveTimer.expires_after(std::chrono::seconds(600));
        m_bookmarkAutoRemoveTimerStarted = true;
    }
    else
    {
        m_bookmarkAutoRemoveTimer.expires_after(std::chrono::seconds(3600));
    }

    m_bookmarkAutoRemoveTimer.async_wait( WeakCallback(shared_from_this(),
                                                       [this](const boost::system::error_code& ec)
    {
        if (ec)
            return;

        boost::asio::post(m_strand, WeakCallback(shared_from_this(), [this]()
        {
            std::set<uuid_string> deviceIDs; // 북마크가 하나라도 있는 Device ID set
            for(auto& bookmark : m_databaseManager->GetBookmarks())
                deviceIDs.insert(bookmark.second.deviceID);

            // 각 Device의 RecordingPeriod 획득
            for(uuid_string deviceId : deviceIDs) {
                auto request = std::make_shared<Wisenet::Device::DeviceGetRecordingPeriodRequest>();
                request->deviceID = deviceId;

                DeviceGetRecordingPeriod(request, [this, request](const ResponseBaseSharedPtr& response)
                {
                    if(response->isSuccess()) {
                        auto recordingPeriodResponse = std::static_pointer_cast<Wisenet::Device::DeviceGetRecordingPeriodResponse>(response);

                        boost::asio::post(m_strand, WeakCallback(shared_from_this(), [this, request, recordingPeriodResponse]()
                        {
                            // 삭제 할 녹화 시작 시간 이전의 북마크 필터링
                            std::vector<uuid_string> bookmarkIDs;
                            for(auto& bookmark : m_databaseManager->GetBookmarks()) {
                                if(bookmark.second.deviceID == request->deviceID && bookmark.second.endTime < recordingPeriodResponse->result.startTime)
                                    bookmarkIDs.push_back(bookmark.first);
                            }

                            // 삭제 할 북마크가 있으면 DB에서 삭제 후 이벤트 전송
                            if(bookmarkIDs.size() > 0) {
                                Wisenet::ErrorCode res = m_databaseManager->RemoveBookmarks(bookmarkIDs);
                                if (res == Wisenet::ErrorCode::NoError) {
                                    SPDLOG_DEBUG("restartBookmarkAutoRemoveTimer : bookmark reomved count({})", bookmarkIDs.size());
                                    auto event = std::make_shared<RemoveBookmarksEvent>();
                                    event->bookmarkIDs = bookmarkIDs;
                                    processEvent(event);
                                }
                            }
                        }));
                    }
                });
            }

            // 타이머 함수 재시작
            restartBookmarkAutoRemoveTimer();
        }));
    }));
}

void CoreService::restartLogAutoRemoveTimer()
{
    m_logAutoRemoveTimer.expires_after(std::chrono::seconds(60));

    m_logAutoRemoveTimer.async_wait( WeakCallback(shared_from_this(),
                                                  [this](const boost::system::error_code& ec)
    {
        if (ec)
            return;
        boost::asio::post(m_strand, WeakCallback(shared_from_this(), [this]()
        {
            auto toTimeMsec = Wisenet::Common::currentUtcMsecs();
            toTimeMsec -= m_databaseManager->GetLogRetensionTimeMsec();

            SPDLOG_DEBUG("Remove To {} in log database.", toTimeMsec);
            m_logManager->RemoveTo(toTimeMsec);

            restartLogAutoRemoveTimer();
        }));
    }));

}

void CoreService::restartAlarmOutActionTimer()
{
    m_alarmOutActionTimer.expires_after(std::chrono::seconds(1));

    m_alarmOutActionTimer.async_wait( WeakCallback(shared_from_this(),
                                                   [this](const boost::system::error_code& ec)
    {
        if (ec)
            return;

        boost::asio::post(m_strand, WeakCallback(shared_from_this(), [this]()
        {
            m_alarmOutputActionManager->ReduceActionInterval();

            restartAlarmOutActionTimer();
        }));
    }));
}

/*
void CoreService::restartDashboardDataProduceTimer()
{
    m_dashboardDataProduceTimer.expires_after(std::chrono::seconds(15));
    m_dashboardDataProduceTimer.async_wait( WeakCallback(shared_from_this(),
                                                         [this](const boost::system::error_code& ec)
    {
        if (ec)
            return;

        boost::asio::post(m_strand, WeakCallback(shared_from_this(), [this]()
        {
            // Save every 6 hours
            QDateTime now = QDateTime::currentDateTime();

            if(now.time().hour() % 3 == 0 && now.time().hour() != m_latestDashboardSaveHour)
            {
                // minites, seconds delete
                QTime onlyHour = QTime(now.time().hour(), 0, 0, 0);
                now.setTime(onlyHour);

                // UNIX Timestamp, 마지막 저장 시간과 비교
                int nowSinceEpoch = (int)now.toSecsSinceEpoch();
                int latestSaveTime = m_dashboardManager->GetDashboardLatestTime();

                SPDLOG_INFO("CoreService::restartDashboardDataProduceTimer() nowSinceEpoch: {}, latestSaveTime: {}", nowSinceEpoch, latestSaveTime);

                if(latestSaveTime != nowSinceEpoch)
                {
                    QDateTime lastWeek = now.addDays(-6);
                    QTime midnight = QTime(0,0,0,0);
                    lastWeek.setTime(midnight);

                    // Midnight of 6 days ago
                    int lastWeekMidnightSinceEpoch = lastWeek.toSecsSinceEpoch();

                    //QDateTime convertBack = QDateTime::fromSecsSinceEpoch((int)now.toSecsSinceEpoch(), Qt::LocalTime, utcTime);
                    //qDebug() << "restartDashboardDataProduceTimer timenow*" << now << nowSinceEpoch;
                    //qDebug() << "restartDashboardDataProduceTimer convertback" << convertBack;
                    //qDebug() << "restartDashboardDataProduceTimer lastWeek" << lastWeek << lastWeekMidnightSinceEpoch;
                    SPDLOG_INFO("CoreService::restartDashboardDataProduceTimer() timenow  {} timestamp {}", now.toString().toUtf8().constData(), nowSinceEpoch);
                    SPDLOG_INFO("CoreService::restartDashboardDataProduceTimer() lastWeek {} timestamp {}", lastWeek.toString().toUtf8().constData(), lastWeekMidnightSinceEpoch);

                    m_latestDashboardSaveHour = now.time().hour();

                    DashboardData dashboardData;
                    dashboardData.saveTime = nowSinceEpoch;
                    m_databaseManager->GetDashboardData(dashboardData);
                    m_dashboardManager->SaveDashboardData(dashboardData, lastWeekMidnightSinceEpoch - 1);

                    auto event = std::make_shared<DashboardDataUpdatedEvent>();
                    processEvent(event);
                }
            }

            restartDashboardDataProduceTimer();
        }));
    }));
}
*/

}
}
