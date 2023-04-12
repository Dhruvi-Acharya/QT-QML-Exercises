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

bool Test__CoreService::loadNewUser(const std::string& section, Core::User& retUser)
{
    try {
        const auto data = toml::parse(m_confPath);
        const auto& newUser = toml::find(data, section);

        retUser.loginID = toml::find<std::string>(newUser, "userName");
        retUser.userGroupID = toml::find_or<std::string>(newUser, "userGroupID", "0");
        retUser.password = toml::find<std::string>(newUser, "password");
        const auto userType = toml::find_or<std::string>(newUser, "userType", "Admin");
        retUser.name = toml::find<std::string>(newUser, "fullName");
        retUser.description = toml::find<std::string>(newUser, "description");

        if (userType == "Admin")
            retUser.userType = Core::User::UserType::Admin;
        else
            retUser.userType = Core::User::UserType::Custom;
    }
    catch (std::exception &e) {
        std::string errMsg = "loadNewUser() exception::" + std::string(e.what());
        QWARN(errMsg.c_str());
        return false;
    }
    return true;
}

bool Test__CoreService::saveUser(const Core::User& newUser)
{
    // SAVE TEST --------------------------------------------------
    // Set Event
    InitializeEventNotify();
    SetEventTestHandler([this](const EventBaseSharedPtr& event)
    {
        if (event->EventTypeId() == Core::CoreEventType::SaveUserEventType) {
            EventNotify(true);
        }
    });

    // save request and check response
    auto addRequest = std::make_shared<Core::SaveUserRequest>();
    addRequest->user = newUser;

    InitializeNotify();
    m_service->SaveUser(addRequest, [this](const ResponseBaseSharedPtr& response){
        Notify(response->isSuccess());
    });

    // wait response
    bool r = Wait();
    if (!r)
        QWARN("SaveUser() failed");

    // wait event
    bool r2 = EventWait();
    if (!r2)
        QWARN("SaveUserEvent failed");

    return (r && r2);
}

void Test__CoreService::test_SaveRemoveUser()
{
    Core::User newUser;
    if (!loadNewUser("newUser", newUser)) {
        QFAIL("loadNewUser() failed");
    }
    if (!saveUser(newUser)) {
        QFAIL("saveUser() failed");
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
}
