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


bool Test__CoreService::loadNewUserRole(const std::string& section, Core::UserGroup& retUserGroup)
{
    try {
        const auto data = toml::parse(m_confPath);
        const auto& newUserRole = toml::find(data, section);

        retUserGroup.userGroupID = toml::find<std::string>(newUserRole, "userGroupID");
        retUserGroup.name = toml::find<std::string>(newUserRole, "name");
        retUserGroup.accessAllResources = toml::find<bool>(newUserRole, "accessAllResources");
        retUserGroup.userPermission.playback = toml::find<bool>(newUserRole, "playback");
        retUserGroup.userPermission.exportVideo = toml::find<bool>(newUserRole, "exportVideo");
        retUserGroup.userPermission.ptzControl = toml::find<bool>(newUserRole, "ptzControl");
        retUserGroup.userPermission.deviceControl = toml::find<bool>(newUserRole, "deviceControl");
        retUserGroup.userPermission.localRecording = toml::find<bool>(newUserRole, "captureLocalRecording");
        retUserGroup.userPermission.audio = toml::find<bool>(newUserRole, "audio");
        retUserGroup.userPermission.mic = toml::find<bool>(newUserRole, "mic");

    }
    catch (std::exception &e) {
        std::string errMsg = "loadNewUser() exception::" + std::string(e.what());
        QWARN(errMsg.c_str());
        return false;
    }
    return true;
}

bool Test__CoreService::saveUserRole(const Core::UserGroup& newGroup)
{
    // SAVE TEST --------------------------------------------------
    // Set Event
    InitializeEventNotify();
    SetEventTestHandler([this](const EventBaseSharedPtr& event)
    {
        if (event->EventTypeId() == Core::CoreEventType::SaveUserGroupEventType) {
            EventNotify(true);
        }
    });

    // save request and check response
    auto addRequest = std::make_shared<Core::SaveUserGroupRequest>();
    addRequest->userGroup= newGroup;

    InitializeNotify();
    m_service->SaveUserGroup(addRequest, [this](const ResponseBaseSharedPtr& response){
        Notify(response->isSuccess());
    });

    // wait response
    bool r = Wait();
    if (!r)
        QWARN("saveUserRole() failed");

    // wait event
    bool r2 = EventWait();
    if (!r2)
        QWARN("saveUserRoleEvent failed");

    return (r && r2);
}

void Test__CoreService::test_SaveRemoveUserRole()
{
    Core::UserGroup newUserGroup;
    if(!loadNewUserRole("newUserGroup",newUserGroup)) {
        QFAIL("loadNewUser() failed");
    }
    if(!saveUserRole(newUserGroup)) {
        QFAIL("saveUserRole() failed");
    }

    // REMOVE TEST --------------------------------------------------
    // Set Event
    InitializeEventNotify();
    SetEventTestHandler([this, &newUserGroup](const EventBaseSharedPtr& event)
    {
        if (event->EventTypeId() == Core::CoreEventType::RemoveUserGroupsEventType) {
            EventNotify(true);
        }
    });

    // save request and check response
    auto removeRequest = std::make_shared<Core::RemoveUserGroupsRequest>();
    removeRequest->userGroupIDs.push_back(newUserGroup.userGroupID);

    InitializeNotify();
    m_service->RemoveUserGroups(removeRequest, [this](const ResponseBaseSharedPtr& response) {
        Notify(response->isSuccess());
    });

    //wait response
    QVERIFY2(Wait(), "RemoveUserRole() failed");
    //wait event
    QVERIFY2(EventWait(), "RemoveUserRoleEvent failed");

    //RESTOR EVENT HANDLER -----------------------------------------------------------
    SetEventTestHandler(nullptr);
}
