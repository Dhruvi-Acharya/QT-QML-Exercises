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

class DevicePasswordExpiryView : public BaseCommand, public IniParser
{

public:
    explicit DevicePasswordExpiryView(const std::string& logPrefix, DeviceRequestBaseSharedPtr request)
        :BaseCommand(this,logPrefix,std::make_shared<DeviceGetPasswordExpiryResponse>(),false,false),
          m_request(request)
    {
    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;
        builder.CreateCgi("security", "passwordexpiry", "view");

        return builder.GetUrl();
    };

    ResponseBaseSharedPtr ProcessPost() override {
        auto response = std::static_pointer_cast<DeviceGetPasswordExpiryResponse>(m_responseBase);

        response->isPasswordExpired = m_isPasswordExpired;
        response->expiryDurationSettingMonths = m_expiryDurationSettingMonths;

        response->errorCode = Wisenet::ErrorCode::NoError;
        return response;
    }

    bool m_isPasswordExpired = false;
    int m_expiryDurationSettingMonths = 0;

private:
    bool parseINI(NameValueText& iniDoc) override
    {
        std::vector<std::string> tokens;
        for (auto item : iniDoc.m_value)
        {
            tokens.clear();
            boost::split(tokens, item.first, boost::is_any_of("."), boost::token_compress_on);

//            SLOG_DEBUG("DevicePasswordExpiryView():: token - item.first={}", item.first);
//            SLOG_DEBUG("DevicePasswordExpiryView():: token - item.second={}", item.second);

            if(item.first == "PasswordExpired")
            {
                if(item.second == "False")
                {
                    m_isPasswordExpired = false;
                }
                else
                {
                    m_isPasswordExpired = true;
                }
            }

            if(item.first == "DefaultExpiryDurationSettingInMonths")
            {
                m_expiryDurationSettingMonths = try_stoi(item.second, 0);
            }
        }

//        SPDLOG_INFO("[DevicePasswordExpiryView] m_isPasswordExpired:{} m_expiryDurationSettingMonths:{}", m_isPasswordExpired, m_expiryDurationSettingMonths);

        return true;
    }

    DeviceRequestBaseSharedPtr m_request;
};

void SunapiDeviceClientImpl::DeviceGetPasswordExpiry(const DeviceRequestBaseSharedPtr &request,
                                               const ResponseBaseHandler &responseHandler)
{
    SLOG_DEBUG("DevicePasswordExpiryView():: deviceID={}", request->deviceID);

    asyncRequest(m_httpCommandSession,
                 std::make_shared<DevicePasswordExpiryView>(m_logPrefix,request),
                 responseHandler,
                 nullptr,AsyncRequestType::HTTPGET,false);
}

}
}
