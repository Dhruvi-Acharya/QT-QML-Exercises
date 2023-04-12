#pragma once
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>

#include <boost/core/ignore_unused.hpp>
#include <boost/algorithm/string.hpp>
#include "DeviceClient/DeviceRequestResponse.h"
#include "SunapiDeviceClientLogSettings.h"

namespace Wisenet
{
namespace Device
{

inline bool try_stoi(const std::string& str, int* p_value, int defValue = -1)
{
    try {
        *p_value = std::stoi(str);
        return true;
    }
    catch (const std::invalid_argument& ex) {
        boost::ignore_unused(ex);
        *p_value = defValue;
    }
    catch (const std::out_of_range& ex) {
        boost::ignore_unused(ex);
        *p_value = defValue;
    }
    catch (const std::exception& ex) {
        boost::ignore_unused(ex);
        *p_value = defValue;
    }
    return false;
}

inline int try_stoi(const std::string& str, int defValue = -1)
{
    try {
        return std::stoi(str);
    }
    catch (const std::exception& ex) {
        boost::ignore_unused(ex);
        return defValue;
    }
    return defValue;
}

inline float try_stof(const std::string& str, float defValue = -1.0f)
{
    try {
        return std::stof(str);
    }
    catch (const std::exception& ex) {
        boost::ignore_unused(ex);
        return defValue;
    }
    return defValue;
}


inline bool stob(const std::string& str)
{
    bool ret = false;
    std::istringstream(str) >> std::boolalpha >> ret;
    return ret;
}

// core service channel --> sunapi channel
static int toSunapiChannel(const std::string& serviceChannel)
{
    int channel = 1;
    try_stoi(serviceChannel, &channel, 1);
    return (channel-1);
}
static int toSunapiChannel(const int serviceChannel)
{
    return (serviceChannel > 0) ? (serviceChannel - 1) : 0;
}

// sunapi channel --> core service channel
static std::string  fromSunapiChannel(const int sunapiChannel)
{
    return std::to_string(sunapiChannel + 1);
}

static std::string fromSunapiChannel(const std::string& sunapiChannel)
{
    int channel = 0;
    try_stoi(sunapiChannel, &channel, 0);
    return std::to_string(channel + 1);
}

static bool isGreaterThanVersion(int majorVersion, int minorVersion, int patchVer,  const std::string& cgiVersion)
{
    std::string version = cgiVersion;
    std::vector<std::string> tokens;
    boost::split(tokens, version, boost::is_any_of("."), boost::token_compress_on);

    if(3 != tokens.size()){
        SPDLOG_ERROR("Not supported version syntax = {}", version);
        return false;
    }

    int minVersion[3] = {majorVersion,minorVersion,patchVer}; // minimum required version.

    for (size_t i = 0; i< 3; i++){
        int ver = 0;
        if (!try_stoi(tokens.at(i), &ver)) {

            return false;
        }
        if (minVersion[i] < ver) {
            SPDLOG_INFO("Supported version={}, mininum required version={}.{}.{}", version, majorVersion, minorVersion, patchVer);
            return true;
        }
        else if (minVersion[i] > ver) {
            SPDLOG_ERROR("Not supported version={}, mininum required version={}.{}.{}", version, majorVersion, minorVersion, patchVer);
            return false;
        }
    }

    SPDLOG_INFO("Supported version={}, mininum required version={}.{}.{}", version, majorVersion, minorVersion, patchVer);
    return true;
}

}
}

