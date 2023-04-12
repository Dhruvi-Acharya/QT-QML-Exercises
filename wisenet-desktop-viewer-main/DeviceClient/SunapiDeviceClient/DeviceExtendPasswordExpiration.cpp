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

class DeviceExtendPasswordExpirationAction : public BaseCommand, public IniParser
{

public:
    explicit DeviceExtendPasswordExpirationAction(const std::string& logPrefix, DeviceRequestBaseSharedPtr request)
        :BaseCommand(this,logPrefix,std::make_shared<Wisenet::ResponseBase>(),false,false),
          m_request(request)
    {
    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;
        builder.CreateCgi("security", "passwordexpiry", "set");
        builder.AddParameter("ExpiryDurationSettingInMonths", 0);

        return builder.GetUrl();
    };

    ResponseBaseSharedPtr ProcessPost() override {
        auto response = std::static_pointer_cast<Wisenet::ResponseBase>(m_responseBase);
        response->errorCode = Wisenet::ErrorCode::NoError;

        SPDLOG_DEBUG("DeviceExtendPasswordExpiration()::ProcessPost() response->errorCode={}", response->errorCode);

        return response;
    }

private:
    bool parseINI(NameValueText& iniDoc) override
    {
        return true;
    }
    DeviceRequestBaseSharedPtr m_request;
};

void SunapiDeviceClientImpl::DeviceExtendPasswordExpiration(const DeviceRequestBaseSharedPtr &request,
                                               const ResponseBaseHandler &responseHandler)
{
    SPDLOG_DEBUG("DeviceExtendPasswordExpiration():: deviceID={}", request->deviceID);

    asyncRequest(m_httpCommandSession,
                 std::make_shared<DeviceExtendPasswordExpirationAction>(m_logPrefix,request),
                 responseHandler,
                 nullptr,AsyncRequestType::HTTPGET,false);
}

}
}
