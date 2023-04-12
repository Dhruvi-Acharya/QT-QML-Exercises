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
#include <memory>
#include <string>
#include <boost/core/ignore_unused.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/local_time/local_time.hpp>
#include "LogSettings.h"

#include "cgis/BaseCommand.h"
#include "cgis/SunapiSyntaxBuilder.h"


namespace Wisenet
{
namespace Device
{

//************************* SystemDeviceInfoView ******************************//

struct SystemDeviceInfoViewResult
{
    bool supportedVersion = true;
    bool supportedWebSocket = false;
    std::string model;
    std::string firmwareVersion;
    std::string deviceType;
    std::string connectedMacAddress;
    std::string cgiVersion;
    std::string name;
    std::string passwordStrength;
    DeviceType  enumDeviceType = DeviceType::Unknown;
};

static DeviceType toDeviceType(const std::string &deviceType)
{
    if (boost::iequals(deviceType, "NWC"))
        return DeviceType::SunapiCamera;
    else if (boost::iequals(deviceType, "Encoder"))
        return DeviceType::SunapiEncoder;
    else if (boost::iequals(deviceType, "NVR"))
        return DeviceType::SunapiRecorder;
    else if (boost::iequals(deviceType, "DVR"))
        return DeviceType::SunapiRecorder;
    else if (boost::iequals(deviceType, "Hybrid"))
        return DeviceType::SunapiRecorder;
    return DeviceType::Unknown;
}

static bool checkVersionAndType(SystemDeviceInfoViewResult& res)
{
    if(res.model.empty()) {
        SPDLOG_ERROR("Unknown Model Name.");
        return false;
    }

    if(res.cgiVersion.empty()) {
        SPDLOG_ERROR("Unknown CGI Version");
        return false;
    }

    if(!isGreaterThanVersion(2,3,1, res.cgiVersion)){
        return false;
    }

    res.enumDeviceType = toDeviceType(res.deviceType);
    if (res.enumDeviceType == DeviceType::Unknown)
        return false;

    return true;
}

static bool checkWebSocketVersion(SystemDeviceInfoViewResult& res)
{
    if(res.cgiVersion.empty()) {
        SPDLOG_ERROR("Unknown CGI Version");
        return false;
    }

    if(!isGreaterThanVersion(2,5,4,res.cgiVersion)){
        SPDLOG_ERROR("Not supported websocket version={}, mininum required version=2.5.4", res.cgiVersion);
        return false;
    }
    return true;
}

class SystemDeviceInfoView : public BaseCommand, public IniParser
{
public:
    explicit SystemDeviceInfoView(const std::string& logPrefix, ResponseBaseSharedPtr response)
        :BaseCommand(this, logPrefix, response, false, false)
    {
    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;
        builder.CreateCgi("system","deviceinfo", "view");
        return builder.GetUrl();
    };

    ResponseBaseSharedPtr ProcessPost() override {
        //응답처리를 하지 않으므로 nullptr로 리턴
        return nullptr;
    }

    SystemDeviceInfoViewResult parseResult;

private:
    bool parseINI(NameValueText& iniDoc) override
    {
        parseResult.model = iniDoc.getString("Model");
        parseResult.firmwareVersion = iniDoc.getString("FirmwareVersion");
        parseResult.deviceType = iniDoc.getString("DeviceType");
        parseResult.connectedMacAddress = iniDoc.getString("ConnectedMACAddress");
        parseResult.cgiVersion = iniDoc.getString("CGIVersion");
        parseResult.name = iniDoc.getString("DeviceName");
        parseResult.passwordStrength = iniDoc.getString("PasswordStrength");

        parseResult.supportedVersion = checkVersionAndType(parseResult);
        parseResult.supportedWebSocket = checkWebSocketVersion(parseResult);  //websocket 지원하는 버전부터 nvr digest 인증 지원

        if (!parseResult.supportedVersion) {
            m_serviceErrorCode = ErrorCode::InCompatibleDevice;
        }

        return parseResult.supportedVersion;
    }

};

//************************* SystemDateView ******************************//

struct SystemDateViewResult
{
    std::string localTime;
    std::string utcTime;
    bool        dstEnable = false;
    std::string posixTimeZone; /* todo timezone parsing */

    int utcTimeBias = 0;
    int dstBias = -60;
    int dstStartMonth = 0;
    int dstStartDay = 0;
    int dstStartHour = 0;
    int dstStartMinute = 0;
    int dstEndMonth = 0;
    int dstEndDay = 0;
    int dstEndHour = 0;
    int dstEndMinute = 0;
};

class SystemDateView : public BaseCommand, public IniParser
{
public:
    explicit SystemDateView(const std::string& logPrefix)
        :BaseCommand(this,logPrefix,std::make_shared<DeviceConnectResponse>(),false,false)
    {
    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;
        builder.CreateCgi("system","date", "view");
        return builder.GetUrl();
    };

    ResponseBaseSharedPtr ProcessPost() override {
        //응답처리 하지 않으므로 nullptr로 리턴함.
        return nullptr;
    }

    SystemDateViewResult parseResult;

private:
    /* todo */
    void parsePosixTimeZone(const std::string& posixTimezoneFormatString, const std::string& local_time, const bool dst_enable)
    {
        //boost::ignore_unused(posix_time);
        boost::ignore_unused(dst_enable);
        // posix_time Format : EST5EDT,M3.2.0/2:00:00,M11.1.0/2:00:00
        //                     지역,시작시간(월.일.요일/시간),끝시간(월.일.요일/시간)
        /* https://www.ibm.com/developerworks/aix/library/au-aix-posix/
        The format is TZ = local_timezone,date/time,date/time.
        Here, date is in the Mm.n.d format, where:
        Mm (1-12) for 12 months
        n (1-5) 1 for the first week and 5 for the last week in the month
        d (0-6) 0 for Sunday and 6 for Saturday
        */

        boost::posix_time::ptime lt(boost::posix_time::time_from_string(local_time));
        boost::local_time::time_zone_ptr tzPtr(new boost::local_time::posix_time_zone(posixTimezoneFormatString));
        boost::posix_time::time_duration offset = tzPtr->base_utc_offset();

        boost::posix_time::ptime dstStart = tzPtr->dst_local_start_time(lt.date().year());
        boost::posix_time::ptime dstEnd = tzPtr->dst_local_end_time(lt.date().year());

        parseResult.utcTimeBias = offset.hours() * 60 + offset.minutes();

        if(dst_enable)
        {
            parseResult.dstStartMonth = dstStart.date().month();
            parseResult.dstStartDay = dstStart.date().day();
            parseResult.dstStartHour = dstStart.time_of_day().hours();
            parseResult.dstStartMinute = dstStart.time_of_day().minutes();

            parseResult.dstEndMonth = dstEnd.date().month();
            parseResult.dstEndDay = dstEnd.date().day();
            parseResult.dstEndHour = dstEnd.time_of_day().hours();
            parseResult.dstEndMinute = dstEnd.time_of_day().minutes();
        }

        SPDLOG_INFO("[Timezone] offset H:{} M:{} {} {} ", offset.hours(), offset.minutes(), to_simple_string(dstStart), to_simple_string(dstEnd));
    }

    bool parseINI(NameValueText& iniDoc) override
    {
        parseResult.localTime = iniDoc.getString("LocalTime");
        parseResult.utcTime = iniDoc.getString("UTCTime");
        parseResult.posixTimeZone = iniDoc.getString("POSIXTimeZone");
        parseResult.dstEnable = iniDoc.getBool("DSTEnable");
        parsePosixTimeZone(parseResult.posixTimeZone, parseResult.utcTime, parseResult.dstEnable);
        return true;
    }
};


}
}

