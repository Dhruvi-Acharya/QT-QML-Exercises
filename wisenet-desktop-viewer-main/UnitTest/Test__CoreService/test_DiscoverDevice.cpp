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

void Test__CoreService::test_DiscoverDevices()
{
    auto request = std::make_shared<Core::DiscoverDevicesRequest>();

    std::vector<Core::DiscoveryDeviceInfo> discoveredDevices;

    InitializeNotify();
    m_service->DiscoverDevices(request, [this, &discoveredDevices]
                               (const ResponseBaseSharedPtr& response)
    {
        auto discoverRes = std::static_pointer_cast<Core::DiscoverDevicesResponse>(response);
        if (!response->isContinue) {
            Notify(response->isSuccess());
        }

        for(auto &info : discoverRes->discoveredDevices) {
            SPDLOG_DEBUG("test_DiscoverDevices()::{}", info.ipAddress);
            discoveredDevices.push_back(info);
        }
    });
    QVERIFY2(Wait(), "test_DiscoverDevices() can not find device");
    QVERIFY(discoveredDevices.size() > 0);
}

void Test__CoreService::test_DiscoverIPv6Devices()
{
    auto request = std::make_shared<Core::DiscoverDevicesRequest>();

    std::vector<Core::DiscoveryDeviceInfo> discoveredDevices;

    InitializeNotify();
    m_service->DiscoverIPv6Devices(request, [this, &discoveredDevices]
    (const ResponseBaseSharedPtr& response)
        {
            auto discoverRes = std::static_pointer_cast<Core::DiscoverDevicesResponse>(response);
            if (!response->isContinue) {
                Notify(response->isSuccess());
            }

            for (auto& info : discoverRes->discoveredDevices) {
                SPDLOG_DEBUG("test_DiscoverDevices()::{}", info.ipAddress);
                discoveredDevices.push_back(info);
            }
        });
    QVERIFY2(Wait(), "test_DiscoverDevices() can not find device");
    QVERIFY(discoveredDevices.size() > 0);
}

void Test__CoreService::test_IpSettingDevice_skip()
{
    // remove or comments below line to test
    QSKIP("test_IpSettingDevice_skip() TEST SKIP, must be used for debugging");

    std::string user = "admin";
    std::string password = "5tkatjd!";
    std::string source_ip = "192.168.880.10";
    std::string source_gateway; // save it from response automatically
    Core::DiscoveryDeviceInfo sourceInfo;

    std::string target_ip = "192.168.80.11";
    std::string target_gateway = "192.168.80.1";


    auto discoverRequest = std::make_shared<Core::DiscoverDevicesRequest>();

    InitializeNotify();
    m_service->DiscoverDevices(discoverRequest, [this, &sourceInfo, source_ip]
                               (const ResponseBaseSharedPtr& response)
    {
        auto discoverRes = std::static_pointer_cast<Core::DiscoverDevicesResponse>(response);
        if (!response->isContinue) {
            Notify(response->isSuccess() && !sourceInfo.ipAddress.empty());
            return;
        }

        for (auto& info : discoverRes->discoveredDevices) {
            SPDLOG_DEBUG("test_DiscoverDevices()::{}", info.ipAddress);
            if (info.ipAddress == source_ip){
                sourceInfo = info;
            }
        }
    });

    QVERIFY2(Wait(), "IpSettingDevice() can not find device");
    source_gateway = sourceInfo.gatewayAddress;

    auto ipSettingRequest = std::make_shared<Core::IpSettingDeviceRequest>();
    ipSettingRequest->ipSetting.ipAddress = target_ip;
    ipSettingRequest->ipSetting.gatewayAddress = target_gateway;
    ipSettingRequest->ipSetting.id = user;          //??
    ipSettingRequest->ipSetting.password = password;//??
    ipSettingRequest->ipSetting.subnetMaskAddress = sourceInfo.subnetMaskAddress;
    ipSettingRequest->ipSetting.macAddress = sourceInfo.macAddress;
    ipSettingRequest->ipSetting.devicePort = sourceInfo.devicePort;
    ipSettingRequest->ipSetting.httpPort = sourceInfo.httpPort;
    ipSettingRequest->ipSetting.passwordNonce = sourceInfo.passwordNonce;
    ipSettingRequest->ipSetting.supportPasswordEncryption = sourceInfo.supportPasswordEncryption;

    InitializeNotify();
    m_service->IpSettingDevice(ipSettingRequest, [this] (const ResponseBaseSharedPtr& response)
    {
        Notify(response->isSuccess());
    });

    if (!Wait()) {
        std::ostringstream msg;
        msg << "IpSettingDevice() ip change fail sourceIP=" << source_ip << ", targetIP=" << target_ip;
        QFAIL(msg.str().c_str());
    }

    // Wait device booting time
    std::this_thread::sleep_for(std::chrono::seconds(10));

    sourceInfo = {};
    InitializeNotify();
    m_service->DiscoverDevices(discoverRequest, [this, &sourceInfo, ipSettingRequest]
                               (const ResponseBaseSharedPtr& response)
    {
        auto discoverRes = std::static_pointer_cast<Core::DiscoverDevicesResponse>(response);
        if (!response->isContinue) {
            Notify(response->isSuccess() && !sourceInfo.ipAddress.empty());
            return;
        }

        for (auto& info : discoverRes->discoveredDevices) {
            SPDLOG_DEBUG("test_DiscoverDevices()::{}", info.ipAddress);
            if (info.macAddress == ipSettingRequest->ipSetting.macAddress) {
                sourceInfo = info;
            }
        }
    });

    QVERIFY2(Wait(), "IpSettingDevice() can not find taret device");

    ipSettingRequest->ipSetting.ipAddress = source_ip;
    ipSettingRequest->ipSetting.gatewayAddress = source_gateway;

    InitializeNotify();
    m_service->IpSettingDevice(ipSettingRequest, [this](const ResponseBaseSharedPtr& response)
    {
        Notify(response->isSuccess());
    });

    if (!Wait()) {
        std::ostringstream msg;
        msg << "IpSettingDevice() ip change fail sourceIP=" << source_ip << ", targetIP=" << target_ip;
        QFAIL(msg.str().c_str());
    }
}


void Test__CoreService::test_InitDevicePassword_skip()
{
    // remove or comments below line to test
    QSKIP("test_InitDevicePassword_skip() TEST SKIP, must be used for debugging");

    std::string source_ip = "192.168.880.10";
    Core::DiscoveryDeviceInfo sourceInfo;
    std::string targetPassword = "5tkatjd!";

    auto discoverRequest = std::make_shared<Core::DiscoverDevicesRequest>();
    InitializeNotify();

    m_service->DiscoverDevices(discoverRequest, [this, &sourceInfo, source_ip](const ResponseBaseSharedPtr& response)
    {
        auto discoverRes = std::static_pointer_cast<Core::DiscoverDevicesResponse>(response);
        if (!response->isContinue) {
            Notify(response->isSuccess() && !sourceInfo.ipAddress.empty());
            return;
        }

        for (auto& info : discoverRes->discoveredDevices) {
            if (info.needToPasswordConfig) {
                SPDLOG_DEBUG("test_InitPassword device{}", info.ipAddress);

                if (info.ipAddress == source_ip){
                    sourceInfo = info;
                }
            }
        }
    });

    QVERIFY2(Wait(), "DiscoverDevices() can not find device");

    auto passwordRequest = std::make_shared<Core::InitDevicePasswordRequest>();
    passwordRequest->macAddress = sourceInfo.macAddress;
    passwordRequest->password = targetPassword;

    InitializeNotify();
    m_service->InitDevicePassword(passwordRequest, [this](const ResponseBaseSharedPtr& response)
    {
        Notify(response->isSuccess());
    });

    QVERIFY2(Wait(), "Init Password() failed");
}


