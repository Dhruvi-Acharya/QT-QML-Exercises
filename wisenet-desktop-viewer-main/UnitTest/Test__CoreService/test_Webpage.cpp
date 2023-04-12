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
bool Test__CoreService::loadNewWebpage(const std::string& section, Core::Webpage& retWebpage)
{
    try {
        const auto data = toml::parse(m_confPath);
        const auto& newWebpage = toml::find(data, section);

        retWebpage.webpageID = toml::find<std::string>(newWebpage, "webpageID");
        retWebpage.name = toml::find<std::string>(newWebpage, "name");
        retWebpage.url = toml::find<std::string>(newWebpage, "url");
    }
    catch (std::exception &e) {
        std::string errMsg = "loadNewWebpage() exception::" + std::string(e.what());
        QWARN(errMsg.c_str());
        return false;
    }
    return true;
}

bool Test__CoreService::saveWebpage(const Core::Webpage& webpage)
{
    // SAVE TEST --------------------------------------------------
    // Set Event
    InitializeEventNotify();
    SetEventTestHandler([this](const EventBaseSharedPtr& event)
    {
        if (event->EventTypeId() == Core::CoreEventType::SaveWebpageEventType) {
            EventNotify(true);
        }
    });

    // save request and check response
    auto addRequest = std::make_shared<Core::SaveWebpageRequest>();
    addRequest->webpage = webpage;

    InitializeNotify();
    m_service->SaveWebpage(addRequest, [this](const ResponseBaseSharedPtr& response){
        Notify(response->isSuccess());
    });

    // wait response
    bool r = Wait();
    if (!r)
        QWARN("SaveWebpage() failed");

    // wait event
    bool r2 = EventWait();
    if (!r2)
        QWARN("SaveWebpageEvent failed");

    return (r && r2);
}

void Test__CoreService::test_SaveRemoveWebpage()
{
    Core::Webpage newWebpage;
    if (!loadNewWebpage("newWebpage", newWebpage)) {
        QFAIL("loadNewWebpage() failed");
    }
    if (!saveWebpage(newWebpage)) {
        QFAIL("SaveWebpage() failed");
    }

    // REMOVE TEST --------------------------------------------------
    // Set Event
    InitializeEventNotify();
    SetEventTestHandler([this, &newWebpage](const EventBaseSharedPtr& event)
    {
        if (event->EventTypeId() == Core::CoreEventType::RemoveWebpagesEventType) {
            auto removeWebpagesEvent = std::static_pointer_cast<Core::RemoveWebpagesEvent>(event);
            if((1 == removeWebpagesEvent->webpageIDs.size())
                    && (removeWebpagesEvent->webpageIDs[0] == newWebpage.webpageID)) {
                EventNotify(true);
            }else {
                EventNotify(false);
            }
        }
    });

    // save request and check response
    auto removeRequest = std::make_shared<Core::RemoveWebpagesRequest>();
    removeRequest->webpageIDs.push_back(newWebpage.webpageID);

    InitializeNotify();
    m_service->RemoveWebpages(removeRequest, [this](const ResponseBaseSharedPtr& response){
        Notify(response->isSuccess());
    });

    // wait response
    QVERIFY2(Wait(), "RemoveWebpages() failed");
    // wait event
    QVERIFY2(EventWait(), "RemoveWebpagesEvent failed");

    // RESTORE EVENT HANDLER ----------------------------------------
    SetEventTestHandler(nullptr);
}
