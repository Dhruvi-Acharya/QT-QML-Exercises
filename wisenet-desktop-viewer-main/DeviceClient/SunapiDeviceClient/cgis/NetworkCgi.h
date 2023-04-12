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

#include "cgis/BaseCommand.h"
#include "cgis/SunapiSyntaxBuilder.h"
#include "LogSettings.h"

namespace Wisenet
{
namespace Device
{

struct NetworkRtspViewResult
{
    unsigned short rtspPort = 554;
};

class NetworkRtspView : public BaseCommand, public IniParser
{
public:
    explicit NetworkRtspView(const std::string& logPrefix)
        :BaseCommand(this,logPrefix,std::make_shared<DeviceConnectResponse>(),false,false)
    {
    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;
        builder.CreateCgi("network","rtsp", "view");
        return builder.GetUrl();
    };

    ResponseBaseSharedPtr ProcessPost() override {
        //응답처리를 하지 않으므로 nullptr로 리턴
        return nullptr;
    }

    NetworkRtspViewResult parseResult;

private:
    bool parseINI(NameValueText& iniDoc) override
    {
        parseResult.rtspPort = static_cast<unsigned short>(iniDoc.getInt("Port"));
        return true;
    }
};

//************************* NetworkInterfaceView ******************************//
struct NetworkInterfaceViewResult
{
    struct NetworkInterface
    {
        std::string ipAddress;

    };

    std::map<std::string, NetworkInterface> networkInterfaces;
};


class NetworkInterfaceView : public BaseCommand, public IniParser
{
public:
    explicit NetworkInterfaceView(const std::string& logPrefix)
        :BaseCommand(this,logPrefix,std::make_shared<DeviceConnectResponse>(),true,true)
    {

    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;
        builder.CreateCgi("network","interface", "view");
        return builder.GetUrl();
    }

    ResponseBaseSharedPtr ProcessPost() override {
        return nullptr;
    }
    NetworkInterfaceViewResult parseResult;

private:
    bool parseINI(NameValueText& iniDoc) override
    {
        // Parse full response
        //int i =0;
        std::string mac;
        for (auto &item : iniDoc.m_listValue) {
            SPDLOG_DEBUG("NetworkInterfaceView first:{} second:{} ",
                                 item.first, item.second);
           if(item.first == "MACAddress"){
               mac = item.second;
           }
           else if(item.first == "IPv4Address"){
               NetworkInterfaceViewResult::NetworkInterface networkInterface;
               networkInterface.ipAddress = item.second;
               parseResult.networkInterfaces.emplace(mac, networkInterface);
           }

        }
        return true;
    }
};
}
}
