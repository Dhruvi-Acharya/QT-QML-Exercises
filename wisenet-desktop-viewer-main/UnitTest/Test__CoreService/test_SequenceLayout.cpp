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

bool Test__CoreService::loadNewSequenceLayout(const std::string& sectionSequence, const std::string& sectionSequenceLayoutItem, Core::SequenceLayout& retSequenceLayout)
{
    try {
        const auto data = toml::parse(m_confPath);
        const auto& newLayout = toml::find(data, sectionSequence);

        retSequenceLayout.sequenceLayoutID = toml::find<std::string>(newLayout, "sequenceID");
        retSequenceLayout.name = toml::find<std::string>(newLayout, "name");
        retSequenceLayout.userName = toml::find<std::string>(newLayout, "userName");
        retSequenceLayout.settingsManual = toml::find_or<bool>(newLayout, "settingManual", true);

        const auto layoutItemTable = toml::find<std::vector<toml::table>>(data, sectionSequenceLayoutItem);
        for (auto& layoutItem : layoutItemTable) {
            Core::SequenceLayoutItem item;            

            item.itemID = layoutItem.at("itemID").as_string();
            item.layoutID  = layoutItem.at("layoutID").as_string();
            item.delayMs = layoutItem.at("delayMs").as_integer();
            item.order = layoutItem.at("order").as_integer();

            retSequenceLayout.sequenceLayoutItems.push_back(item);
        }
    }
    catch (std::exception &e) {
        std::string errMsg = "loadNewSequenceLayout() exception::" + std::string(e.what());
        QWARN(errMsg.c_str());
        return false;
    }
    return true;
}

bool Test__CoreService::saveSequenceLayout(const Core::SequenceLayout& retSequenceLayout)
{
    // SAVE TEST --------------------------------------------------
    // Set Event
    InitializeEventNotify();
    SetEventTestHandler([this](const EventBaseSharedPtr& event)
    {
        if (event->EventTypeId() == Core::CoreEventType::SaveSequenceLayoutEventType) {
            EventNotify(true);
        }
    });

    // save request and check response
    auto addRequest = std::make_shared<Core::SaveSequenceLayoutRequest>();
    addRequest->sequenceLayout = retSequenceLayout;

    InitializeNotify();
    m_service->SaveSequenceLayout(addRequest, [this](const ResponseBaseSharedPtr& response){
        Notify(response->isSuccess());
    });

    // wait response
    bool r = Wait();
    if (!r)
        QWARN("saveSequenceLayout() failed");

    // wait event
    bool r2 = EventWait();
    if (!r2)
        QWARN("saveSequenceLayoutEvent failed");

    return (r && r2);
}

void Test__CoreService::test_SaveRemoveSequenceLayout()
{
    Core::SequenceLayout layout;
    if (!loadNewSequenceLayout("newSequenceLayout", "newSequenceLayoutItem", layout)) {
        QFAIL("loadNewSequenceLayout() failed");
    }
    if (!saveSequenceLayout(layout)) {
        QFAIL("saveLayout() failed");
    }

    // REMOVE TEST --------------------------------------------------
    // Set Event
    InitializeEventNotify();
    SetEventTestHandler([this, &layout](const EventBaseSharedPtr& event)
    {
        if (event->EventTypeId() == Core::CoreEventType::RemoveSequenceLayoutsEventType) {
            auto removeLayoutsEvent = std::static_pointer_cast<Core::RemoveSequenceLayoutsEvent>(event);
            if((1 == removeLayoutsEvent->sequenceLayoutIDs.size()) &&
                    (removeLayoutsEvent->sequenceLayoutIDs[0] == layout.sequenceLayoutID)) {
                EventNotify(true);
            }else {
                EventNotify(false);
            }
        }
    });

    // save request and check response
    auto removeRequest = std::make_shared<Core::RemoveSequenceLayoutsRequest>();
    removeRequest->sequenceLayoutIDs.push_back(layout.sequenceLayoutID);

    InitializeNotify();
    m_service->RemoveSequenceLayouts(removeRequest, [this](const ResponseBaseSharedPtr& response){
        Notify(response->isSuccess());
    });

    // wait response
    QVERIFY2(Wait(), "RemoveSequenceLayouts() failed");
    // wait event
    QVERIFY2(EventWait(), "RemoveSequenceLayoutsEvent failed");

    // RESTORE EVENT HANDLER ----------------------------------------
    SetEventTestHandler(nullptr);
}
