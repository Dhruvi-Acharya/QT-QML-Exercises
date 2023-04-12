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
#include "tst_test__coreservice.h"

#include <thread>
#include <chrono>

void Test__CoreService::liveMedia()
{
    if(m_registeredDevices.empty()){
        QFAIL("No registered device.");
    }

    InitializeEventNotify();
    bool isNotifyed = false;
    auto streamHandler = [this, &isNotifyed](const Wisenet::Media::MediaSourceFrameBaseSharedPtr& mediaSourceFrame)
    {
        if(!isNotifyed && (mediaSourceFrame->getMediaType() == Wisenet::Media::MediaType::VIDEO)) {
            EventNotify(true);
            isNotifyed = false;
        }
    };

    std::string mediaID = "test_media_id";
    auto deviceInfo = m_registeredDevices[0];

    {
        //MediaOpen
        auto request = std::make_shared<Device::DeviceMediaOpenRequest>();
        request->deviceID = deviceInfo.deviceID;
        request->channelID = "1";
        request->mediaID = mediaID;
        request->mediaRequestInfo.streamType = Device::StreamType::liveHigh;
//        request->mediaRequestInfo.profileId = "-1";
        request->mediaRequestInfo.startTime = 0;
        request->mediaRequestInfo.endTime = 0;
        request->mediaRequestInfo.trackId = 0;
        request->mediaRequestInfo.streamHandler = streamHandler;

        InitializeNotify();
        m_service->DeviceMediaOpen(request, [this](const ResponseBaseSharedPtr& response){
            Notify(response->isSuccess());
        });

        // wait response
        QVERIFY2(Wait(), "DeviceMediaOpen() failed");
        QVERIFY2(EventWait(), "Media Receive failed");

    }

    std::this_thread::sleep_for(std::chrono::seconds(10));

    {
        // MediaClose
        auto request = std::make_shared<Device::DeviceMediaCloseRequest>();
        request->deviceID = deviceInfo.deviceID;
        request->channelID = "1";
        request->mediaID = mediaID;

        InitializeNotify();
        m_service->DeviceMediaClose(request, [this](const ResponseBaseSharedPtr& response){
            Notify(response->isSuccess());
        });

        // wait response
        QVERIFY2(Wait(), "DeviceMediaOpen() failed");
    }

}

void Test__CoreService::test_AuditLog()
{
    // logout first
    if (!UserLogout(m_user.userID))
        QFAIL("UserLogout() failed");

    std::string id, password, falseUser, falsePassword;

    try {
        const auto data = toml::parse(m_confPath);
        const auto& user = toml::find(data, "user");
        id = toml::find<std::string>(user, "userName");
        password = toml::find<std::string>(user, "password");
        falseUser = toml::find<std::string>(user, "falseUserName");
        falsePassword = toml::find<std::string>(user, "falsePassword");

        SPDLOG_INFO("CoreService user id={}, pw={}, falseId={}, falsePw={}",
                    id, password, falseUser, falsePassword);
    }
    catch (std::exception &e) {
        std::string errMsg = "LoadLoginQtestData() exception::" + std::string(e.what());
        QFAIL(errMsg.c_str());
    }

    QVERIFY(id.length() > 0);
    QVERIFY(password.length() > 0);
    QVERIFY(falseUser.length() > 0);
    QVERIFY2(falsePassword.length() > 0, "falsePassword is empty!!");

    // false test
    QVERIFY2(UserLogin(falseUser, falsePassword) == false, "False login test failed");

    // true test
    QVERIFY2(UserLogin(id, password) == true, "login test failed");


    // Save User and Remove User Test

    Core::User newUser;
    if (!loadNewUser("newUser", newUser)) {
        QFAIL("loadNewUser() failed");
    }
    if (!saveUser(newUser)) {
        QFAIL("saveUser() failed");
    }

    newUser.description = "update test";
    if (!saveUser(newUser)) {
        QFAIL("saveUser() failed - update");
    }

    // REMOVE TEST --------------------------------------------------

    // Set Event
    InitializeEventNotify();
    SetEventTestHandler([this](const EventBaseSharedPtr& event)
    {
        if (event->EventTypeId() == Core::CoreEventType::RemoveUsersEventType) {
            EventNotify(true);
        }
    });

    // save request and check response
    auto removeRequest = std::make_shared<Core::RemoveUsersRequest>();
    removeRequest->userIDs.push_back(newUser.userID);

    InitializeNotify();
    m_service->RemoveUsers(removeRequest, [this](const ResponseBaseSharedPtr& response){
        Notify(response->isSuccess());
    });

    // wait response
    QVERIFY2(Wait(), "RemoveUsers() failed");
    // wait event
    QVERIFY2(EventWait(), "RemoveUsersEvent failed");

    // RESTORE EVENT HANDLER ----------------------------------------
    SetEventTestHandler(nullptr);

    // Add Remove Device
    test_AddRemoveDevice();

    // Live Media
    liveMedia();

    // GetAuditLog TEST --------------------------------------------------

    std::this_thread::sleep_for(std::chrono::seconds(15));

    // Set Event
    //InitializeEventNotify();
    SetEventTestHandler([](const EventBaseSharedPtr& event)
    {
        boost::ignore_unused(event);
        //if (event->EventTypeId() == Core::CoreEventType::RemoveUsersEventType) {
        //    EventNotify(true);
        //}
    });

    // save request and check response
    auto auditLog = std::make_shared<Core::GetAuditLogRequest>();
    auditLog->toTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    auditLog->fromTime = auditLog->toTime - 3600;

    InitializeNotify();
    m_service->GetAuditLog(auditLog, [this](const ResponseBaseSharedPtr& response){
        auto getAuditLogResponse = std::static_pointer_cast<Core::GetAuditLogResponse>(response);
        if(getAuditLogResponse->isSuccess()) {
            Notify(true);
        } else {
            Notify(false);
        }
    });

    // wait response
    QVERIFY2(Wait(), "GetAuditLog() failed");
    // wait event
    //QVERIFY2(EventWait(), "RemoveUsersEvent failed");

    // RESTORE EVENT HANDLER ----------------------------------------
    SetEventTestHandler(nullptr);
}
