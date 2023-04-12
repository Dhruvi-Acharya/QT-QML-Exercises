#pragma once
#include "tst_test__coreservice.h"
#include <functional>
#include <boost/core/ignore_unused.hpp>

#include "DatabaseManager.h"
#include "LogManager.h"

static constexpr auto SAMPLE_UNIT_CONF = R"x(
[admin]
userName = "admin"
password = "5tkatjd!"

[userRole]
userRoleID = "1"
userRoleName = "user role"
accessAllResources = true
editCameraSettings = true
viewEventLog = true
viewArchive = true
exportArchive = true
viewBookmark = true
modifyBookmark = true
userInput = true

[user]
userName = "admin1"
password = "init123!!"
userRoleID = "1"
userType = "Admin" # Owner | Admin | Custom
fullName = "administrator-1"
description = "test description"
falseUserName = "emily"
falsePassword = "emily"

[[device]]
ip = "192.168.15.13"
port = 80
user = "admin"
password = "5tkatjd!"

[[device]]
ip = "192.168.80.11"
port = 80
user = "admin"
password = "5tkatjd!"

[newUser]
userName = "user"
password = "5tkatjd!"
userRoleID = "1"
userType = "Admin" # Owner | Admin | Custom
fullName = "helloWorld"
description = "description"
enabled = false

[newUserRole]
userRoleID = "2"
userRoleName = "user role2"
accessAllResources = true
editCameraSettings = false
viewEventLog = false
viewArchive = false
exportArchive = false
viewBookmark = false
modifyBookmark = false
userInput = false

[newLayout]
layoutID = "1"
userName = "user"
name = "layout1"
locked = true
cellWidth = 160
cellHeight = 90
cellSpacing = 0.3


[[newLayoutItem]]
itemType = "Camera" # Camera | Webpage | LocalResource | ZoomTarget
itemID = "1"
webPageID = ""
deviceID = "1"
channelID = "1"
resourcePath = ""
column = 0
columnCount = 160
row = 0
rowCount = 90
rotation = 90
displayInfo = true
zoomX = 0.1
zoomY = 0.1
zoomWidth = 0.5
zoomHeight = 0.5
zoomTargetID = "1"
brightness = 0.0
contrast = 0.0


[[newLayoutItem]]
itemType = "Webpage" # Camera | Webpage | LocalResource | ZoomTarget
itemID = "2"
webPageID = "1"
deviceID = "1"
channelID = "1"
resourcePath = ""
column = 0
columnCount = 160
row = 0
rowCount = 90
rotation = 90
displayInfo = true
zoomX = 0.1
zoomY = 0.1
zoomWidth = 0.5
zoomHeight = 0.5
zoomTargetID = "1"
brightness = 0.0
contrast = 0.0


[[newLayoutItem]]
itemType = "LocalResource" # Camera | Webpage | LocalResource | ZoomTarget
itemID = "2"
webPageID = "1"
deviceID = "1"
channelID = "1"
resourcePath = "C:\\localResource.png"
column = 0
columnCount = 160
row = 0
rowCount = 90
rotation = 90
displayInfo = true
zoomX = 0.1
zoomY = 0.1
zoomWidth = 0.5
zoomHeight = 0.5
zoomTargetID = "1"
brightness = 0.0
contrast = 0.0


[[newLayoutItem]]
itemType = "ZoomTarget" # Camera | Webpage | LocalResource | ZoomTarget
itemID = "2"
webPageID = "1"
deviceID = "1"
channelID = "1"
resourcePath = "C:\\localResource.png"
column = 0
columnCount = 160
row = 0
rowCount = 90
rotation = 90
displayInfo = true
zoomX = 0.1
zoomY = 0.1
zoomWidth = 0.5
zoomHeight = 0.5
zoomTargetID = "1"
brightness = 0.0
contrast = 0.0


[newSequenceLayout]
sequenceID = "1"
name = "Sequence1"
userName = "User"
settingManual = true

[[newSequenceLayoutItem]]
itemType = "Camera" # Camera | Webpage | LocalResource | Layout
sequencID = "1"
deviceID = "1"
channelID = "1"
layoutID = "2"
webpageID = "1"
localFilePath = "C:\\local.png"
delayMs = 10
order = 1

[[newSequenceLayoutItem]]
itemType = "Webpage" # Camera | Webpage | LocalResource | Layout
sequencID = "1"
deviceID = "1"
channelID = "1"
layoutID = "2"
webpageID = "1"
localFilePath = "C:\\local.png"
delayMs = 10
order = 2

[[newSequenceLayoutItem]]
itemType = "LocalResource" # Camera | Webpage | LocalResource | Layout
sequencID = "1"
deviceID = "1"
channelID = "1"
layoutID = "2"
webpageID = "1"
localFilePath = "C:\\local.png"
delayMs = 10
order = 3

[[newSequenceLayoutItem]]
itemType = "Layout" # Camera | Webpage | LocalResource | Layout
sequencID = "1"
deviceID = "1"
channelID = "1"
layoutID = "2"
webpageID = "1"
localFilePath = "C:\\local.png"
delayMs = 10
order = 4

[newWebpage]
webpageID = "2"
name = "WaveHome"
url = "http://wavevms.com"

[[newDevice]]
ip = "192.168.80.11"
port = 80
user = "admin"
password = "5tkatjd!"

)x";

using namespace std::placeholders;

void Test__CoreService::initTestCase()
{
    // Delete DB
    Core::DatabaseManager::Delete();
    Core::LogManager::Delete();

    Common::InitializeLogSettings("logs");
    spdlog::set_level(spdlog::level::info);

    m_confPath = QDir::currentPath().toStdString() + "/test_conf.txt";
    std::ifstream iconfs(m_confPath);
    if (!iconfs.is_open()) {
        std::ofstream oconfs(m_confPath);
        if (oconfs.is_open()) {
            oconfs << SAMPLE_UNIT_CONF;
        }
        oconfs.close();
    }

    try {
        const auto data = toml::parse(m_confPath);
        const auto& admin = toml::find(data, "admin");
        m_adminName = toml::find_or<std::string>(admin, "userName", "admin");
        m_adminPassword = toml::find<std::string>(admin, "password");
    }
    catch (std::exception &e) {
        std::string errMsg = "read login information() exception::" + std::string(e.what());
        QFAIL(errMsg.c_str());
    }

    if (!loadNewUser("user", m_user))
        QFAIL("failed to load user data");
    if (!loadNewUserRole("userRole", m_userRole))
        QFAIL("failed to load user role data");

    m_service = std::make_shared<Core::CoreService>();

    // service start
    m_service->Start();

    // service init
    test_GetServiceStatus_RequiredAddAdmin();

    // admin user login for setup
    SetupService();

    // normal user login
    if (!UserLogin(m_user.userID, m_user.password))
        QFAIL("UserLogin() failed");
}

void Test__CoreService::cleanupTestCase()
{
    if (m_user.userID.length() > 0)
        UserLogout(m_user.userID);
    m_service->Stop();
}


void Test__CoreService::init()
{

}

void Test__CoreService::cleanup()
{

}


void Test__CoreService::ResetService()
{
    m_service->Stop();
    Core::DatabaseManager::Delete();
    Core::LogManager::Delete();
    m_service->Start();
}


void Test__CoreService::SetupService()
{
    // admin login
    if (!UserLogin(m_adminName, m_adminPassword))
        QFAIL("UserLogin() admin failed");

    if (!loadDeviceList("device", m_registeredDevices))
        QFAIL("Couldn't load device list.");

    // Register devices
    if (!addDevice(m_registeredDevices)) {
        QFAIL("addDevice() failed");
    }

    // Register user role
    if (!saveUserRole(m_userRole)) {
        QFAIL("saveUserRole() failed");
    }

    // Register user
    if (!saveUser(m_user)) {
        QFAIL("saveUser() failed");
    }

    // admin logout
    if (!UserLogout(m_adminName))
        QFAIL("UserLogout() admin failed");
}

bool Test__CoreService::StartEventService()
{
    auto request = std::make_shared<Core::StartEventServiceRequest>();
    request->userName = m_user.userID;
    request->eventHandler = std::bind(&Test__CoreService::CoreEventHandler, this, _1);
    InitializeNotify();
    m_service->StartEventService(request, [this]
            (const ResponseBaseSharedPtr& response)
    {
        Notify(response->isSuccess());
    });
    bool ret = Wait();
    if (!ret)
        QWARN("StartEventService() failed");
    return ret;
}

void Test__CoreService::SetEventTestHandler(const EventBaseHandler& evtHandler)
{
    std::lock_guard<std::mutex> lock(m_eventHandlerMutex);
    m_eventTestHandler = evtHandler;
}

void Test__CoreService::CoreEventHandler(const EventBaseSharedPtr &event)
{
    SPDLOG_INFO("Receive Event from CoreService, event={}", event->EventType());
    std::lock_guard<std::mutex> lock(m_eventHandlerMutex);
    if (m_eventTestHandler)
        m_eventTestHandler(event);
}


bool Test__CoreService::UserLogin(const std::string& userName,
                                  const std::string& password)
{
    /* log in */
    auto request = std::make_shared<Core::LoginRequest>();
    request->userName = userName;
    request->password = password;

    InitializeNotify();
    m_service->Login(request, [this]
            (const ResponseBaseSharedPtr& response)
    {
        Notify(response->isSuccess());
    });
    if (!Wait()) {
        QWARN("UserLogin() failed");
        return false;
    }

    if (!StartEventService()) {
        QWARN("StartEventService() failed");
        return false;
    }

    return true;
}

bool Test__CoreService::UserLogout(const std::string& userName)
{
    /* log out */
    auto request = std::make_shared<Core::LogoutRequest>();
    request->userName = userName;
    InitializeNotify();
    m_service->Logout(request, [this]
            (const ResponseBaseSharedPtr& response)
    {
        Notify(response->isSuccess());
    });
    bool ret = Wait();
    if (!ret)
        QWARN("UserLogout() failed");
    return ret;
}

void Test__CoreService::InitializeNotify()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_canContinue = false;
    m_isSuccess = false;
}

void Test__CoreService::Notify(bool isSuccess)
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_canContinue = true;
        m_isSuccess = isSuccess;
    }
    m_condition.notify_one();
}

bool Test__CoreService::Wait(long timeoutSec)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    std::chrono::seconds second(timeoutSec);

    if (!m_condition.wait_for(lock, second, [this]() { return m_canContinue; })) {
        std::ostringstream msg;
        msg << "This request timed out. timeoutSec=" << second.count();
        QWARN(msg.str().c_str());
        return false;
    }

    if (!m_isSuccess) {
        QWARN("This request is failed.");
        return false;
    }

    return true;
}


void Test__CoreService::InitializeEventNotify()
{
    std::lock_guard<std::mutex> lock(m_eventMutex);
    m_canEventContinue = false;
    m_isEventSuccess = false;
}

void Test__CoreService::EventNotify(bool isSuccess)
{
    {
        std::lock_guard<std::mutex> lock(m_eventMutex);
        m_canEventContinue = true;
        m_isEventSuccess = isSuccess;
    }
    m_eventCondition.notify_one();
}

bool Test__CoreService::EventWait(long timeoutSec)
{
    std::unique_lock<std::mutex> lock(m_eventMutex);
    std::chrono::seconds second(timeoutSec);

    if (!m_eventCondition.wait_for(lock, second, [this]() { return m_canEventContinue; })) {
        std::ostringstream msg;
        msg << "This event condition timed out. timeoutSec=" << second.count();
        QWARN(msg.str().c_str());
        return false;
    }

    if (!m_isEventSuccess) {
        QWARN("This request is failed.");
        return false;
    }

    return true;
}

// debugging with standalone mode
QTEST_MAIN(Test__CoreService)

// run unit test
//QTEST_APPLESS_MAIN(Test__CoreService)

//#include "tst_test__coreservice.moc"
