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
#include "SunapiDeviceClientImpl.h"

namespace Wisenet
{
namespace Device
{

class PTZConfigPresetView : public BaseCommand, public IniParser
{
public:
    explicit PTZConfigPresetView(const std::string& logPrefix, DeviceGetPresetRequestSharedPtr request, int maxPreset)
        :BaseCommand(this,logPrefix,std::make_shared<DeviceGetPresetResponse>(),false,false),
         m_request(request), m_maxPreset(maxPreset)
    {

    }
    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;

        builder.CreateCgi("ptzconfig", "preset", "view")
               .AddParameter("Channel", toSunapiChannel(m_request->channelID));

        return builder.GetUrl();
    }

    ResponseBaseSharedPtr ProcessPost() override {
        auto response = std::static_pointer_cast<DeviceGetPresetResponse>(m_responseBase);
        response->maxPreset = m_maxPreset;
        response->presets = parseResult;
        response->errorCode = Wisenet::ErrorCode::NoError;
        return response;
    }

    std::map<unsigned int, std::string> parseResult;
private:
    bool parseINI(NameValueText& iniDoc) override
    {
        std::vector<std::string> tokens;
        for (auto item : iniDoc.m_value)
        {
            if (item.first.find("Channel") == std::string::npos)
                continue;

            tokens.clear();
            boost::split(tokens, item.first, boost::is_any_of("."), boost::token_compress_on);
            if(tokens.size() < 5)
                continue;

            int presetNumber = 0;
            if (!try_stoi(tokens.at(3), &presetNumber))
                continue;

            std::string name = iniDoc.getString(item.first);
            parseResult.insert(std::pair<unsigned int, std::string>(presetNumber, name));
        }
        return true;
    }

    DeviceGetPresetRequestSharedPtr m_request;
    int m_maxPreset = 0;
};

void SunapiDeviceClientImpl::DeviceGetPreset(DeviceGetPresetRequestSharedPtr const& request,
                                         ResponseBaseHandler const& responseHandler)
{
    int index = toSunapiChannel(request->channelID);
    int maxPreset = 0;
    auto& attr = m_repos->attributes().ptzSupportAttribute;
    auto ch = attr.channels.find(index);
    if (ch != attr.channels.end()) {
        maxPreset = ch->second.maxPreset;
    }
    if (maxPreset == 0) {
        maxPreset = 255;
    }

    asyncRequest(m_httpPtzSession,
                 std::make_shared<PTZConfigPresetView>(m_logPrefix,request, maxPreset),
                 responseHandler,
                 nullptr,AsyncRequestType::HTTPGET,false);
}

class PTZConfigPresetAdd : public BaseCommand
{
public:
    explicit PTZConfigPresetAdd(const std::string& logPrefix,DeviceAddPresetRequestSharedPtr request)
        :BaseCommand(nullptr,logPrefix,std::make_shared<Wisenet::ResponseBase>(),false,false),
         m_request(request)
    {

    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;

        builder.CreateCgi("ptzconfig", "preset", "add")
               .AddParameter("Channel", toSunapiChannel(m_request->channelID))
               .AddParameter("Preset", m_request->preset)
               .AddParameter("Name", m_request->name);

        if(m_request->subViewIndex != 0)
            builder.AddParameter("SubViewIndex", m_request->subViewIndex);

        return builder.GetUrl();
    };

    ResponseBaseSharedPtr ProcessPost() override {
        m_responseBase->errorCode = Wisenet::ErrorCode::NoError;
        return m_responseBase;
    }

private:
    DeviceAddPresetRequestSharedPtr m_request;
};

//add and update preset
void SunapiDeviceClientImpl::DeviceAddPreset(DeviceAddPresetRequestSharedPtr const& request, ResponseBaseHandler const& responseHandler)
{
    asyncRequest(m_httpPtzSession,
                 std::make_shared<PTZConfigPresetAdd>(m_logPrefix,request),
                 responseHandler,
                 nullptr,AsyncRequestType::HTTPGET,false);
}

class PTZConfigPresetRemove : public BaseCommand
{
public:
    explicit PTZConfigPresetRemove(const std::string& logPrefix, DeviceRemovePresetRequestSharedPtr request)
        :BaseCommand(nullptr,logPrefix,std::make_shared<Wisenet::ResponseBase>(),false,false),
         m_request(request)
    {

    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;

        builder.CreateCgi("ptzconfig", "preset", "remove")
               .AddParameter("Channel", toSunapiChannel(m_request->channelID))
               .AddParameter("Preset", m_request->preset);

        return builder.GetUrl();
    };

    ResponseBaseSharedPtr ProcessPost() override {
        m_responseBase->errorCode = Wisenet::ErrorCode::NoError;
        return m_responseBase;
    }

private:
    DeviceRemovePresetRequestSharedPtr m_request;
};

//Remove preset
void SunapiDeviceClientImpl::DeviceRemovePreset(DeviceRemovePresetRequestSharedPtr const& request, ResponseBaseHandler const& responseHandler)
{
    asyncRequest(m_httpPtzSession,
                 std::make_shared<PTZConfigPresetRemove>(m_logPrefix,request),
                 responseHandler,
                 nullptr,AsyncRequestType::HTTPGET,false);
}

class PTZControlPresetMove : public BaseCommand
{
public:
    explicit PTZControlPresetMove(const std::string& logPrefix, DeviceMovePresetRequestSharedPtr request)
        :BaseCommand(nullptr,logPrefix,std::make_shared<Wisenet::ResponseBase>(),false,false),
         m_request(request)
    {

    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;

        builder.CreateCgi("ptzcontrol", "preset", "control")
               .AddParameter("Channel", toSunapiChannel(m_request->channelID))
               .AddParameter("Preset", m_request->preset);

        if(m_request->subViewIndex != 0)
            builder.AddParameter("SubViewIndex", m_request->subViewIndex);

        return builder.GetUrl();
    };

    ResponseBaseSharedPtr ProcessPost() override {
        m_responseBase->errorCode = Wisenet::ErrorCode::NoError;
        return m_responseBase;
    }

private:
    DeviceMovePresetRequestSharedPtr m_request;
};

//move to preset
void SunapiDeviceClientImpl::DeviceMovePreset(DeviceMovePresetRequestSharedPtr const& request, ResponseBaseHandler const& responseHandler)
{
    asyncRequest(m_httpPtzSession,
                 std::make_shared<PTZControlPresetMove>(m_logPrefix,request),
                 responseHandler,
                 nullptr,AsyncRequestType::HTTPGET,false);
}

}
}
