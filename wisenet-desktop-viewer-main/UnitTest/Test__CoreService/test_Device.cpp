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

bool Test__CoreService::loadDeviceList(const std::string& section, std::vector<DeviceInfo>& retDeviceList)
{
    try {
        const auto data = toml::parse(m_confPath);
        const auto deviceTable = toml::find<std::vector<toml::table>>(data, section);
        for (auto& ditem : deviceTable) {
            DeviceInfo deviceInfo;

            deviceInfo.host = ditem.at("ip").as_string();
            deviceInfo.port = ditem.at("port").as_integer();
            deviceInfo.user = ditem.at("user").as_string();
            deviceInfo.password = ditem.at("password").as_string();

            deviceInfo.deviceID = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8().constData();
            retDeviceList.push_back(deviceInfo);
        }
    }
    catch (std::exception &e) {
        std::string errMsg = "loadDeviceList() exception::" + std::string(e.what());
        QWARN(errMsg.c_str());
        return false;
    }
    return true;
}

bool Test__CoreService::addDevice(const std::vector<DeviceInfo> &deviceList)
{
    // Set Event
    auto deviceCount = deviceList.size();
    InitializeEventNotify();
    SetEventTestHandler([this, &deviceCount](const EventBaseSharedPtr& event)
    {
        if (event->EventTypeId() == Core::CoreEventType::AddDeviceEventType) {
            SPDLOG_INFO("addDevice Event::remain devic count = {}", deviceCount);
            deviceCount--;
            if (deviceCount == 0) {
                SPDLOG_INFO("EXPIRE ADD DEVICE EVENT SUCCESS");
                EventNotify(true);
            }
        }
    });

    // Add test
    for(auto& device : deviceList) {
        auto request = std::make_shared<Core::AddDeviceRequest>();
        request->userName = m_user.userID;
        request->deviceID = device.deviceID;
        request->connectionInfo.host= device.host;
        request->connectionInfo.port = device.port;
        request->connectionInfo.user = device.user;;
        request->connectionInfo.password = device.password;

        InitializeNotify();
        m_service->AddDevice(request, [this](const ResponseBaseSharedPtr& response){
            Notify(response->isSuccess());
        });

        if (!Wait())
            return false;
    }

    bool eventRet = EventWait();
    SetEventTestHandler(nullptr);
    return eventRet;
}

bool Test__CoreService::removeDevices(const std::vector<DeviceInfo> &deviceList)
{
    // Set Event
    InitializeEventNotify();
    SetEventTestHandler([this, deviceList](const EventBaseSharedPtr& event)
    {
        if (event->EventTypeId() == Core::CoreEventType::RemoveDevicesEventType) {
            auto removeEvent = std::static_pointer_cast<Wisenet::Core::RemoveDevicesEvent>(event);
            EventNotify(removeEvent->deviceIDs.size() == deviceList.size());
        }
    });

    auto removeRequest = std::make_shared<Core::RemoveDevicesRequest>();
    for(auto& device : deviceList) {
        removeRequest->deviceIDs.push_back(device.deviceID);
    }

    InitializeNotify();
    m_service->RemoveDevices(removeRequest, [this](const ResponseBaseSharedPtr& response){
        Notify(response->isSuccess());
    });

    if (!Wait())
        return false;

    bool eventRet = EventWait();
    SetEventTestHandler(nullptr);
    return eventRet;
}


void Test__CoreService::test_AddRemoveDevice()
{
    std::vector<DeviceInfo> retDeviceList;
    if (!loadDeviceList("newDevice", retDeviceList))
        QFAIL("Couldn't load device list.");

    // Add test
    if (!addDevice(retDeviceList)) {
        QFAIL("addDevice() failed");
    }


    // Remove test
    if (!removeDevices(retDeviceList)) {
        QFAIL("removeDevices() failed");
    }
}


void Test__CoreService::test_DeviceConnectDisconnect()
{

}

void Test__CoreService::test_DeviceMediaOpenClose()
{

}
