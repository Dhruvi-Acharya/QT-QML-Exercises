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

class PTZControlAreaZoomControl : public BaseCommand
{
public:
    explicit PTZControlAreaZoomControl(const std::string& logPrefix, DeviceAreaZoomRequestSharedPtr request, bool isProfileSupported)
        :BaseCommand(nullptr,logPrefix,std::make_shared<Wisenet::ResponseBase>(),false,false),
         m_request(request),
         m_isProfileSupported(isProfileSupported)
    {

    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;
        builder.CreateCgi("ptzcontrol","areazoom", "control")
                .AddParameter("Channel", toSunapiChannel(m_request->channelID))
                .AddParameter("Type", "ZoomIn")
                .AddParameter("X1", m_request->x1)
                .AddParameter("Y1", m_request->y1)
                .AddParameter("X2", m_request->x2)
                .AddParameter("Y2", m_request->y2)
                .AddParameter("TileWidth", m_request->tileWidth)
                .AddParameter("TileHeight", m_request->tileHeight);

        if(m_isProfileSupported && (0 < m_request->profile)) {
            builder.AddParameter("Profile", m_request->profile);
        }

        return builder.GetUrl();
    };

    ResponseBaseSharedPtr ProcessPost() override {
        m_responseBase->errorCode = Wisenet::ErrorCode::NoError;
        return m_responseBase;
    }

private:
    DeviceAreaZoomRequestSharedPtr m_request;
    bool m_isProfileSupported;
};

void SunapiDeviceClientImpl::DeviceAreaZoom(DeviceAreaZoomRequestSharedPtr const& request, ResponseBaseHandler const& responseHandler)
{
    asyncRequest(m_httpCommandSession,
                std::make_shared<PTZControlAreaZoomControl>(m_logPrefix, request, m_repos->attributes().ptzControlCgi.areazoom.profile),
                responseHandler,
                nullptr,
                AsyncRequestType::HTTPGET, false);
}

}
}
