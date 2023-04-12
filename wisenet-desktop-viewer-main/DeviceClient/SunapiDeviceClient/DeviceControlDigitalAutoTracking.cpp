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

class PTZControlDigitalAutoTrackingControl : public BaseCommand
{
public:
    explicit PTZControlDigitalAutoTrackingControl(const std::string& logPrefix,DeviceSetDigitalAutoTrackingRequestSharedPtr request, bool isProfileSupported)
        :BaseCommand(nullptr,logPrefix,std::make_shared<Wisenet::ResponseBase>(),false,false),
         m_request(request),
         m_isProfileSupported(isProfileSupported)
    {

    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;
        builder.CreateCgi("ptzcontrol","digitalautotracking", "control")
                .AddParameter("Channel", toSunapiChannel(m_request->channelID));

        if(m_isProfileSupported) {
            builder.AddParameter("Profile", m_request->profile);
        }


        if(m_request->enable) {
            builder.AddParameter("Mode", "Start");
        }else {
            builder.AddParameter("Mode", "Stop");
        }

        return builder.GetUrl();
    };

    ResponseBaseSharedPtr ProcessPost() override {
        m_responseBase->errorCode = Wisenet::ErrorCode::NoError;
        return m_responseBase;
    }

private:
    DeviceSetDigitalAutoTrackingRequestSharedPtr m_request;
    bool m_isProfileSupported;
};

void SunapiDeviceClientImpl::DeviceSetDigitalAutoTracking(DeviceSetDigitalAutoTrackingRequestSharedPtr const& request, ResponseBaseHandler const& responseHandler )
{
    asyncRequest(m_httpCommandSession,
                 std::make_shared<PTZControlDigitalAutoTrackingControl>(m_logPrefix, request, m_repos->attributes().ptzControlCgi.digitalAutoTracking.profile),
                 responseHandler,
                 nullptr,AsyncRequestType::HTTPGET,false);
}

}
}
