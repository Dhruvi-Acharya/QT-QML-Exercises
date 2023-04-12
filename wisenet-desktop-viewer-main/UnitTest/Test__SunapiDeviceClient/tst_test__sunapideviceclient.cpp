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
#include "tst_test__sunapideviceclient.h"
#include <boost/core/ignore_unused.hpp>

static constexpr auto SAMPLE_UNIT_CONF = R"x(
ip = "192.168.183.34"
port = 80
user = "admin"
password = "5rlarudejr!"
)x";


void Test__SunapiDeviceClient::initConfig()
{
    auto a = QDir::currentPath().toStdString();
    const auto confPath = QDir::currentPath().toStdString() + "/test_conf.txt";
    std::ifstream iconfs(confPath);
    if (!iconfs.is_open()) {
        std::ofstream oconfs(confPath);
        if (oconfs.is_open()) {
            oconfs << SAMPLE_UNIT_CONF;
        }
        oconfs.close();
    }
    iconfs.close();
    try {
        const auto data = toml::parse(confPath);
        m_ip = toml::find_or(data, "ip", "192.168.183.34");
        m_port = toml::find_or(data, "port", 80);
        m_user = toml::find_or(data, "user", "admin");
        m_password = toml::find_or(data, "password", "5rlarudejr!");
        m_audioSamplePath = QDir::currentPath().toStdString() + "/" + toml::find_or(data, "audio_sample_path", "");
    }
    catch (std::exception& e) {
        // do nothing
        boost::ignore_unused(e);
    }
}

void Test__SunapiDeviceClient::initTestCase()
{
    Wisenet::Common::InitializeLogSettings("logs");
    initConfig();

    SPDLOG_INFO("Initialize()");
    m_threadPool.Start();
    m_streamIoContextPool.Start();
}

void Test__SunapiDeviceClient::cleanupTestCase()
{
    SPDLOG_INFO("Finalize()");
    m_threadPool.Stop();
    m_streamIoContextPool.Stop();
}

bool Test__SunapiDeviceClient::connectInternal()
{
    if (!m_deviceClient) {
        m_deviceClient = std::make_shared<Wisenet::Device::SunapiDeviceClient>(m_threadPool.ioContext(),
                                                                               m_streamIoContextPool.ioContext(),
                                                                               m_deviceUuid,
                                                                               m_cloudService, 30, false, 1831);
    }

    m_deviceClient->SetDeviceEventHandler([](const DeviceEventBaseSharedPtr& event)
        {
            SPDLOG_DEBUG("DEVICE EVNET: type = {}", event->EventType());
        });

    auto request = std::make_shared<DeviceConnectRequest>();
    request->deviceID = m_deviceUuid;
    request->connectionInfo.host = m_ip;
    request->connectionInfo.port = m_port;
    request->connectionInfo.user = m_user;
    request->connectionInfo.password = m_password;


    InitializeNotify();
    m_deviceClient->DeviceConnect(request,
                                  [this, request]
                                  (const ResponseBaseSharedPtr& response)
    {
        if (response->isFailed()) {
            SPDLOG_WARN("connectInternal() failed, error_string={}", response->errorString());
            Notify(false);
        }
        else {
           Notify(true);
        }
    });

    if(!Wait(30)) {
        return false;
    }

    return true;
}

bool Test__SunapiDeviceClient::disconnectInternal()
{
    if (m_deviceClient) {
        auto request = std::make_shared<DeviceDisconnectRequest>();
        m_deviceClient->DeviceDisconnect(request, nullptr);
        m_deviceClient = nullptr;
    }
    return true;
}

//void Test__SunapiDeviceClient::init()
//{
//    SPDLOG_DEBUG("Test__SunapiDeviceClient::init() :: Connect to the device");
//    QVERIFY2(connectInternal(), "Can not connect to the device..");
//}

//void Test__SunapiDeviceClient::cleanup()
//{
//    SPDLOG_DEBUG("Test__SunapiDeviceClient::cleanup() :: Disconnect to the device");
//    QVERIFY2(disconnectInternal(), "Can not disconnect to the device..");
//}


void Test__SunapiDeviceClient::InitializeNotify()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_canContinue = false;
    m_isSuccess = false;
}
void Test__SunapiDeviceClient::Notify(bool isSuccess)
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_canContinue = true;
        m_isSuccess = isSuccess;
    }
    m_condition.notify_one();
}

bool Test__SunapiDeviceClient::Wait(long timeout)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    std::chrono::seconds second(timeout);

    if(!m_condition.wait_for(lock, second, [this](){ return m_canContinue;})){
        SPDLOG_ERROR("This request timed out.");
        return false;
    }

    if(!m_isSuccess) {
        SPDLOG_ERROR("This request is failed.");
        return false;
    }

    return true;
}

QTEST_MAIN(Test__SunapiDeviceClient)

