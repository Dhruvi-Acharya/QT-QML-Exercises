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

struct DefogInfo
{
    DefogMode defogMode = DefogMode::Off;
    int defogLevel = 0;
};

class ImageImageEnhancementsView : public BaseCommand, public IniParser
{

public:
    explicit ImageImageEnhancementsView(const std::string& logPrefix, DeviceGetDefogRequestSharedPtr request)
        :BaseCommand(this,logPrefix,std::make_shared<DeviceGetDefogResponse>(),false,false),
         m_request(request)
    {

    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;

        builder.CreateCgi("image", "imageenhancements", "view")
                .AddParameter("Channel", toSunapiChannel(m_request->channelID));

        return builder.GetUrl();
    };

    ResponseBaseSharedPtr ProcessPost() override {
        auto response = std::static_pointer_cast<DeviceGetDefogResponse>(m_responseBase);
        response->defogMode = m_parseResult.defogMode;
        response->defogLevel = m_parseResult.defogLevel;
        response->errorCode = Wisenet::ErrorCode::NoError;
        return response;
    }

    DefogInfo m_parseResult;

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
            if(tokens.size() < 3)
                continue;

            if (tokens.at(2) == "DefogMode"){
                std::string defogMode = item.second;

                if(defogMode == "Off")
                    m_parseResult.defogMode = DefogMode::Off;
                else if(defogMode == "Auto")
                    m_parseResult.defogMode = DefogMode::Auto;
                else if(defogMode == "Manual")
                    m_parseResult.defogMode = DefogMode::Manual;
            }
            else if (tokens.at(2) == "DefogLevel")
                try_stoi(item.second , &m_parseResult.defogLevel);
        }
        return true;
    }

    DeviceGetDefogRequestSharedPtr m_request;
};

void SunapiDeviceClientImpl::DeviceGetDefog(const DeviceGetDefogRequestSharedPtr &request,
                                        const ResponseBaseHandler &responseHandler)
{
    asyncRequest(m_httpCommandSession,
                 std::make_shared<ImageImageEnhancementsView>(m_logPrefix,request),
                 responseHandler,
                 nullptr,AsyncRequestType::HTTPGET,false);
}

class ImageImageEnhancementsSetDefog : public BaseCommand
{
public:
    explicit ImageImageEnhancementsSetDefog(const std::string& logPrefix, DeviceSetDefogRequestSharedPtr request)
        :BaseCommand(nullptr,logPrefix,std::make_shared<Wisenet::ResponseBase>(),false,false),
         m_request(request)
    {

    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;

        std::string defogMode = "";
        if(m_request->defogMode == DefogMode::Off)
            defogMode = "Off";
        else if(m_request->defogMode == DefogMode::Auto)
            defogMode = "Auto";
        else if(m_request->defogMode == DefogMode::Manual)
            defogMode = "Manual";

        if(m_request->defogMode == DefogMode::Off)
            builder.CreateCgi("image", "imageenhancements", "set")
                    .AddParameter("Channel", toSunapiChannel(m_request->channelID))
                    .AddParameter("DefogMode", defogMode);
        else
            builder.CreateCgi("image", "imageenhancements", "set")
                    .AddParameter("Channel", toSunapiChannel(m_request->channelID))
                    .AddParameter("DefogMode", defogMode)
                    .AddParameter("DefogLevel", m_request->defogLevel);

        return builder.GetUrl();
    };

    ResponseBaseSharedPtr ProcessPost() override {
        m_responseBase->errorCode = Wisenet::ErrorCode::NoError;
        return m_responseBase;
    }

private:
    DeviceSetDefogRequestSharedPtr m_request;
};

void SunapiDeviceClientImpl::DeviceSetDefog(const DeviceSetDefogRequestSharedPtr &request,
                                        const ResponseBaseHandler &responseHandler)
{
    asyncRequest(m_httpCommandSession,
                 std::make_shared<ImageImageEnhancementsSetDefog>(m_logPrefix,request),
                 responseHandler,
                 nullptr,AsyncRequestType::HTTPGET,false);
}

}
}
