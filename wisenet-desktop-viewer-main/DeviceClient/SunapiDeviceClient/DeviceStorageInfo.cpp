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

class SystemStorageInfoView : public BaseCommand, public IniParser
{

public:
    explicit SystemStorageInfoView(const std::string& logPrefix, DeviceStorageInfoRequestSharedPtr request)
        :BaseCommand(this,logPrefix,std::make_shared<DeviceStorageInfoResponse>(),false,false),
          m_request(request)
    {
    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;
        builder.CreateCgi("system", "storageinfo", "view");

        return builder.GetUrl();
    };

    ResponseBaseSharedPtr ProcessPost() override {
        auto response = std::static_pointer_cast<DeviceStorageInfoResponse>(m_responseBase);

        response->isNormal = m_isNormal;
        response->usedSpace = m_usedSpace;
        response->totalSpace = m_totalSpace;

        response->errorCode = Wisenet::ErrorCode::NoError;
        return response;
    }

    int m_usedSpace = 0;
    int m_totalSpace = 0;
    bool m_isNormal = true;

private:
    bool parseINI(NameValueText& iniDoc) override
    {
        std::vector<std::string> tokens;
        for (auto item : iniDoc.m_value)
        {
            if (item.first.find("Storage") == std::string::npos)
                continue;

            tokens.clear();
            boost::split(tokens, item.first, boost::is_any_of("."), boost::token_compress_on);
            if(tokens.size() < 3)
                continue;

            if (tokens.at(2) == "UsedSpace")
                m_usedSpace += try_stoi(item.second, 0);
            else if (tokens.at(2) == "TotalSpace")
                m_totalSpace += try_stoi(item.second, 0);
            else if (tokens.at(2) == "Status" && m_isNormal)
            {
                if(item.second != "Normal")
                    m_isNormal = false;
            }
        }

        SPDLOG_INFO("[StorageInfoView] Used:{} Total:{} isNormal:{}", m_usedSpace, m_totalSpace, m_isNormal);

        return true;
    }

    DeviceStorageInfoRequestSharedPtr m_request;
};

void SunapiDeviceClientImpl::DeviceStorageInfo(const DeviceStorageInfoRequestSharedPtr &request,
                                               const ResponseBaseHandler &responseHandler)
{
    SLOG_DEBUG("DeviceStorageInfo():: channel={}", request->deviceID);

    asyncRequest(m_httpCommandSession,
                 std::make_shared<SystemStorageInfoView>(m_logPrefix,request),
                 responseHandler,
                 nullptr,AsyncRequestType::HTTPGET,false);
}

}
}
