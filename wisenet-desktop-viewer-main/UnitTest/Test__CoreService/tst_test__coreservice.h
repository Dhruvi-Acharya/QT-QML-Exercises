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

#include <QtTest>
#include <QDir>
#include <QString>

#include <memory>
#include <vector>
#include <toml.hpp>
#include <sstream>
#include "CoreService.h"
#include "LogSettings.h"
#include "CoreService/CoreServiceRequestResponse.h"

using namespace Wisenet;

struct DeviceInfo
{
    std::string deviceID;
    std::string host;
    int         port = 0;
    std::string user;
    std::string password;
};

class Test__CoreService : public QObject
{
    Q_OBJECT
public:
    Test__CoreService(){}
    ~Test__CoreService(){}

private:
    Core::ICoreServiceSharedPtr m_service;
    std::string m_confPath;

private:
    void LoadNewUserQtestData();

    // not used, just for debugging test
    void ResetService();

    // GetServiceInformation, SetupService
    void test_GetServiceStatus_RequiredAddAdmin();

    // Setup DB
    void SetupService();

    // Init user session
    bool UserLogin(const std::string& userName, const std::string& password);
    bool UserLogout(const std::string& userName);
    bool StartEventService();
    void CoreEventHandler(const EventBaseSharedPtr& event);

    // member function
    bool loadDeviceList(const std::string& section, std::vector<DeviceInfo>& retDeviceList);
    bool addDevice(const std::vector<DeviceInfo>& deviceList);
    bool removeDevices(const std::vector<DeviceInfo>& deviceList);
    bool loadNewUser(const std::string& section, Core::User& retUser);
    bool saveUser(const Core::User& newUser);
    bool loadNewUserRole(const std::string& section, Core::UserGroup& retUserRole);
    bool saveUserRole(const Core::UserGroup& newUserRole);
    bool loadNewWebpage(const std::string& section, Core::Webpage& retWebpage);
    bool saveWebpage(const Core::Webpage& webpage);
    bool loadNewLayout(const std::string& sectionLayout, const std::string& sectionLayoutItem, Core::Layout& retLayout);
    bool saveLayout(const Core::Layout& retLayout);
    bool loadNewSequenceLayout(const std::string& sectionSequence, const std::string& sectionSequenceLayoutItem, Core::SequenceLayout& retLSequenceayout);
    bool saveSequenceLayout(const Core::SequenceLayout& retSequenceLayout);
    void liveMedia();

    // Test case event intercept
    void SetEventTestHandler(const EventBaseHandler& evtHandler);

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Login, Logout
    void test_Login();

    // Device discovery & ip/password initialization using SUNAPI broadcast
    void test_DiscoverDevices();
    void test_DiscoverIPv6Devices();
    void test_IpSettingDevice_skip();
    void test_InitDevicePassword_skip();

    // DEVICE
    void test_AddRemoveDevice();
    void test_DeviceConnectDisconnect();
    void test_DeviceMediaOpenClose();

    // USER ROLE
    void test_SaveRemoveUserRole();

    // USER
    void test_SaveRemoveUser();

    // LAYOUT
    void test_SaveRemoveLayout();

    // SEQUENCE LAYOUT
    void test_SaveRemoveSequenceLayout();

    // WEBPAGE
    void test_SaveRemoveWebpage();

    // Audit Log
    void test_AuditLog();

    // Bookmark
    void test_SaveRemoveBookmark();

private:
    std::condition_variable m_condition;
    std::mutex m_mutex;
    bool m_canContinue = false; //Wait or Go 여부
    bool m_isSuccess = false;   //비동기 응답 성공 여부
    void InitializeNotify();     // 비동기 명령 호출 전 m_canContinue, m_isSuccess false로 초기화 함.
    void Notify(bool isSuccess); // 비동기 응답 처리가 완료된 경우 응답값의 정상 여부와 함께 호출.
    bool Wait(long timeoutSec = 1800); // true: timeout 시간 안에 Notify(true)가 호출된 경우, false: timeout이 지났거나, Nofity(false)가 호출된 경우.

    Core::User m_user;
    Core::UserGroup m_userRole;

    std::string m_adminName;
    std::string m_adminPassword;

    std::vector<DeviceInfo> m_registeredDevices;
    std::mutex m_eventHandlerMutex;
    EventBaseHandler m_eventTestHandler = nullptr;

    std::condition_variable m_eventCondition;
    std::mutex m_eventMutex;
    bool m_canEventContinue = false; //Wait or Go 여부
    bool m_isEventSuccess = false;   //비동기 응답 성공 여부
    void InitializeEventNotify();
    void EventNotify(bool isSuccess);
    bool EventWait(long timeoutSec = 1800);
};
