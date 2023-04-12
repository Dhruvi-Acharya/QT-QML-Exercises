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
#pragma once

#include <QString>
#include <string>
#include <map>

namespace Wisenet
{
namespace Core
{
struct SunapiDeviceClientOptions
{
    long long keepAliveTime = 10;
    bool allowRelay = false;
};

class CoreServiceConfig
{
public:
    CoreServiceConfig();

    bool Read(const QString& path);

    const std::map<std::string,std::string>& GetCloudConfig() { return m_cloudConfig; };
    unsigned int GetEasyConnectionLogLevel(){ return m_easyConnectionLogLevel; };
    const SunapiDeviceClientOptions& GetSunapiDeviceClientOption() { return m_sunapiDeviceClientOptions; };

private:
    bool writeDefaultValue(const std::string& filePath);
    void setDefaultValue();
    void setDefaultCloudSection();
    void setDefaultEasyConnectionSection();
    void setDefaultSunapiDeviceClientSection();

    static const QString CONFIG_NAME;

    std::map<std::string,std::string> m_cloudConfig;
    unsigned int m_easyConnectionLogLevel;
    SunapiDeviceClientOptions m_sunapiDeviceClientOptions;
};

}
}
