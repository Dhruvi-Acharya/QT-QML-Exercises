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

bool Test__CoreService::loadNewLayout(const std::string& sectionLayout, const std::string& sectionLayoutItem, Core::Layout& retLayout)
{
    try {
        const auto data = toml::parse(m_confPath);
        const auto& newLayout = toml::find(data, sectionLayout);

        retLayout.layoutID = toml::find<std::string>(newLayout, "layoutID");
        retLayout.userName = toml::find<std::string>(newLayout, "userName");
        retLayout.name = toml::find<std::string>(newLayout, "name");
        retLayout.isLocked = toml::find_or<bool>(newLayout, "locked", true);
        retLayout.cellWidth = toml::find<float>(newLayout, "cellWidth");
        retLayout.cellHeight = toml::find<float>(newLayout, "cellHeight");
        retLayout.cellSpacing = toml::find<float>(newLayout, "cellSpacing");

        const auto layoutItemTable = toml::find<std::vector<toml::table>>(data, sectionLayoutItem);
        for (auto& layoutItem : layoutItemTable) {
            Core::LayoutItem item;
            std::string type = layoutItem.at("itemType").as_string();
            if(type == "Camera") {
                item.itemType = Core::LayoutItem::LayoutItemType::Camera;
            } else if(type == "Webpage"){
                item.itemType = Core::LayoutItem::LayoutItemType::Webpage;
            } else if(type == "LocalResource"){
                item.itemType = Core::LayoutItem::LayoutItemType::LocalResource;
            } else if(type == "ZoomTarget"){
                item.itemType = Core::LayoutItem::LayoutItemType::ZoomTarget;
            } else {
                QWARN("Unkown LayoutItemType");
                continue;
            }
            item.itemID = layoutItem.at("itemID").as_string();
            item.webpageID  = layoutItem.at("webPageID").as_string();
            item.deviceID = layoutItem.at("deviceID").as_string();
            item.channelID = layoutItem.at("channelID").as_string();
            item.resourcePath = layoutItem.at("resourcePath").as_string();
            item.column = layoutItem.at("column").as_integer();
            item.columnCount = layoutItem.at("columnCount").as_integer();
            item.row = layoutItem.at("row").as_integer();
            item.rowCount = layoutItem.at("rowCount").as_integer();
            item.rotation = layoutItem.at("rotation").as_integer();
            item.displayInfo = layoutItem.at("displayInfo").as_boolean();

            item.zoomTargetParam.normX = layoutItem.at("zoomX").as_floating();
            item.zoomTargetParam.normY = layoutItem.at("zoomY").as_floating();
            item.zoomTargetParam.normWidth = layoutItem.at("zoomWidth").as_floating();
            item.zoomTargetParam.normHeight = layoutItem.at("zoomHeight").as_floating();
            item.zoomTargetParam.zoomTargetID = layoutItem.at("zoomTargetID").as_string();

            item.bncParam.brightness = layoutItem.at("brightness").as_floating();
            item.bncParam.contrast = layoutItem.at("contrast").as_floating();

            retLayout.layoutItems.push_back(item);
        }
    }
    catch (std::exception &e) {
        std::string errMsg = "loadNewLayout() exception::" + std::string(e.what());
        QWARN(errMsg.c_str());
        return false;
    }
    return true;
}

bool Test__CoreService::saveLayout(const Core::Layout& retLayout)
{
    // SAVE TEST --------------------------------------------------
    // Set Event
    InitializeEventNotify();
    SetEventTestHandler([this](const EventBaseSharedPtr& event)
    {
        if (event->EventTypeId() == Core::CoreEventType::SaveLayoutEventType) {
            EventNotify(true);
        }
    });

    // save request and check response
    auto addRequest = std::make_shared<Core::SaveLayoutRequest>();
    addRequest->layout = retLayout;

    InitializeNotify();
    m_service->SaveLayout(addRequest, [this](const ResponseBaseSharedPtr& response){
        Notify(response->isSuccess());
    });

    // wait response
    bool r = Wait();
    if (!r)
        QWARN("saveLayout() failed");

    // wait event
    bool r2 = EventWait();
    if (!r2)
        QWARN("saveLayoutEvent failed");

    return (r && r2);
}

void Test__CoreService::test_SaveRemoveLayout()
{
    Core::Layout layout;
    if (!loadNewLayout("newLayout", "newLayoutItem", layout)) {
        QFAIL("loadNewLayout() failed");
    }
    if (!saveLayout(layout)) {
        QFAIL("saveNewLayout() failed");
    }

    // REMOVE TEST --------------------------------------------------
    // Set Event
    InitializeEventNotify();
    SetEventTestHandler([this, &layout](const EventBaseSharedPtr& event)
    {
        if (event->EventTypeId() == Core::CoreEventType::RemoveLayoutsEventType) {
            auto removeLayoutsEvent = std::static_pointer_cast<Core::RemoveLayoutsEvent>(event);
            if((1 == removeLayoutsEvent->layoutIDs.size()) &&
                    (removeLayoutsEvent->layoutIDs[0] == layout.layoutID)) {
                EventNotify(true);
            }else {
                EventNotify(false);
            }
        }
    });

    // save request and check response
    auto removeRequest = std::make_shared<Core::RemoveLayoutsRequest>();
    removeRequest->layoutIDs.push_back(layout.layoutID);

    InitializeNotify();
    m_service->RemoveLayouts(removeRequest, [this](const ResponseBaseSharedPtr& response){
        Notify(response->isSuccess());
    });

    // wait response
    QVERIFY2(Wait(), "RemoveLayouts() failed");
    // wait event
    QVERIFY2(EventWait(), "RemoveLayoutsEvent failed");

    // RESTORE EVENT HANDLER ----------------------------------------
    SetEventTestHandler(nullptr);
}
