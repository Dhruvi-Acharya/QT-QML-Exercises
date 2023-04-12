/*********************************************************************************
 * Copyright(c) 2023 Hanwha Vision Co., Ltd. All rights reserved.
 *
 * This software is copyrighted by, and is the sole property of Hanwha Vision.
 * All rigths, title, ownership, or other interests in the software remain the
 * property of Hanwha Vision. This software may only be used in accordance with
 * the corresponding license agreement. Any unauthorized use, duplication,
 * transmission, distribution, or disclosure of this software is expressly
 * forbidden.
 *
 * This Copyright notice may not be removed or modified without prior written
 * consent of Hanwha Vision.
 *
 * Hanwha Vision reserves the right to modify this software without notice.
 *
 * Hanwha Vision Co., Ltd.
 * KOREA
 * https://www.hanwhavision.com/
 *********************************************************************************/
#include "CoreServiceConfig.h"

#include <QDir>

#include "toml.hpp"
#include "CoreServiceLogSettings.h"

namespace Wisenet
{
namespace Core
{

static constexpr auto CORESERVICE_CONFIG = R"x(
# a connection information of a Wisenet Cloud 
# domain by environment: (develop: dev, QA: qa, Real: prod, Hofix: hf)
# if useSTUN is true, you can use a P2P connection.
# if useTURN is true, you can user a RELAY connection.   
[cloud]
iothuburl = "mqtt.prod.wra1.wisenetcloud.com:5000"
apiurl = "api.prod.wra1.wisenetcloud.com"
authurl = "auth.prod.wra1.wisenetcloud.com"
turnurl = "turn.prod.wra1.wisenetcloud.com"
port = "443"
useSTUN = "true"
useTURN = "true"
useNewCloudAPI = "true"

# loglevel: None = 0,App = 1,Fatal = 2, Error = 4, Debug = 8, Warn = 16, Info = 32,
#           Print = 64, Cmd = 128, Sdk = 256, Profile= 512, eAll = 0xffff
[easyconnection]
loglevel=0

[sunapideviceclient]
keepalivetime=10
allowRelay=false

)x";

const QString CoreServiceConfig::CONFIG_NAME = "coreservice_settings.conf";

CoreServiceConfig::CoreServiceConfig()
    :m_easyConnectionLogLevel(8)
{
}

bool CoreServiceConfig::Read(const QString &path)
{
    QString nativeFilePath = QDir::toNativeSeparators(path + QDir::separator() + CONFIG_NAME);
    //const auto configPath = nativeFilePath.toStdString();
    //fix local file path for korean
    const auto configPath = nativeFilePath.toLocal8Bit().toStdString();

    SPDLOG_INFO("CoreService Configuration Path={}", configPath);

    std::ifstream iconfs(configPath);

    if (!iconfs.is_open()) {
        iconfs.close();

        if(!writeDefaultValue(configPath)){
            SPDLOG_WARN("Read a config in a memroy.");
            return false;
        }
        SPDLOG_INFO("Writed a defult config.");
        return true;
    }

    try {
        const auto data = toml::parse(configPath);

        try {
            const auto& cloud = toml::find(data, "cloud");

            // set a cloud configuration.
            m_cloudConfig.emplace("iothuburl", toml::find_or(cloud, "iothuburl", "mqtt.prod.wra1.wisenetcloud.com:5000"));
            m_cloudConfig.emplace("apiurl", toml::find_or(cloud, "apiurl", "api.prod.wra1.wisenetcloud.com"));
            m_cloudConfig.emplace("authurl", toml::find_or(cloud, "authurl", "auth.prod.wra1.wisenetcloud.com"));
            m_cloudConfig.emplace("turnurl", toml::find_or(cloud, "turnurl", "turn.prod.wra1.wisenetcloud.com"));
            m_cloudConfig.emplace("port", toml::find_or(cloud, "port", "443"));
            m_cloudConfig.emplace("useSTUN", toml::find_or(cloud, "useSTUN", "true"));
            m_cloudConfig.emplace("useTURN", toml::find_or(cloud, "useTURN", "true"));
            m_cloudConfig.emplace("useNewCloudAPI", toml::find_or(cloud, "useNewCloudAPI", "true"));
       
        }catch (std::exception& e) {
            SPDLOG_ERROR("Failed to read a cloud section in a coreservice_settings.conf. Path={}, Exception={}", configPath, e.what());
            setDefaultCloudSection();
            return false;
        }
        
        try{
            const auto& easyConnection = toml::find(data, "easyconnection");

            // set a easyConnection configuration.
            m_easyConnectionLogLevel = toml::find_or(easyConnection, "loglevel", 0);

        }catch (std::exception& e) {
            SPDLOG_ERROR("Failed to read a easyconnection section in a coreservice_settings.conf. Path={}, Exception={}", configPath, e.what());
            setDefaultEasyConnectionSection();
            return false;
        }

        try {
            const auto& sunapideviceclient = toml::find(data, "sunapideviceclient");

            // set a easyConnection configuration.
            m_sunapiDeviceClientOptions.keepAliveTime = toml::find_or(sunapideviceclient, "keepalivetime", 10);
            m_sunapiDeviceClientOptions.allowRelay = toml::find_or(sunapideviceclient, "allowRelay", false);
        }
        catch (std::exception& e) {
            SPDLOG_ERROR("Failed to read a sunapideviceclient section in a coreservice_settings.conf. Path={}, Exception={}", configPath, e.what());
            setDefaultSunapiDeviceClientSection();
            return false;
        }

    }  catch (std::exception &e) {
        SPDLOG_ERROR("Failed to read a coreservice_settings.conf. Path={}, Exception={}", configPath, e.what());
        setDefaultValue();
        return false;
    }

    SPDLOG_INFO("Successfully read a coreservice configuration.");
    return true;
}

bool CoreServiceConfig::writeDefaultValue(const std::string &filePath)
{
    std::ofstream oconfs(filePath);

    if (!oconfs.is_open()) {
        SPDLOG_ERROR("Failed to write a defult config file. Path={}", filePath);
        setDefaultValue();
        oconfs.close();
        return false;
    }

    setDefaultValue();
    oconfs << CORESERVICE_CONFIG;
    oconfs.close();

    return true;
}

void CoreServiceConfig::setDefaultValue()
{
    setDefaultCloudSection();
    setDefaultEasyConnectionSection();
}
void CoreServiceConfig::setDefaultCloudSection()
{
    // default cloud configuration.
    m_cloudConfig.clear();
    m_cloudConfig.emplace("iothuburl", "mqtt.prod.wra1.wisenetcloud.com:5000");
    m_cloudConfig.emplace("apiurl", "api.prod.wra1.wisenetcloud.com");
    m_cloudConfig.emplace("authurl", "auth.prod.wra1.wisenetcloud.com");
    m_cloudConfig.emplace("turnurl", "turn.prod.wra1.wisenetcloud.com");
    m_cloudConfig.emplace("port", "443");
    m_cloudConfig.emplace("useSTUN", "true");
    m_cloudConfig.emplace("useTURN", "true");
    m_cloudConfig.emplace("useNewCloudAPI", "true");
}
void CoreServiceConfig::setDefaultEasyConnectionSection()
{
    m_easyConnectionLogLevel = 0;
}

void CoreServiceConfig::setDefaultSunapiDeviceClientSection()
{
    m_sunapiDeviceClientOptions.keepAliveTime = 10;
    m_sunapiDeviceClientOptions.allowRelay = false;
}

}
}
