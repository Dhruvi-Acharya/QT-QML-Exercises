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

//************** DeviceGetAutoTracking ****************************//

class EventsourcesAutoTrackingView : public BaseCommand, public IniParser
{
public:
    explicit EventsourcesAutoTrackingView(const std::string& logPrefix, DeviceGetAutoTrackingRequestSharedPtr request)
        :BaseCommand(this,logPrefix,std::make_shared<DeviceGetAutoTrackingResponse>(),false,false),
         m_request(request)
    {

    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;
        builder.CreateCgi("eventsources","autotracking", "view")
                .AddParameter("Channel", toSunapiChannel(m_request->channelID));

        return builder.GetUrl();
    };

    ResponseBaseSharedPtr ProcessPost() override {
        auto response = std::static_pointer_cast<DeviceGetAutoTrackingResponse>(m_responseBase);
        auto itor = m_enables.find(m_request->channelID);

        if(itor == m_enables.end()) {
            response->errorCode = Wisenet::ErrorCode::InvalidRequest;
            return response;
        }

        response->enable = itor->second;
        response->errorCode = Wisenet::ErrorCode::NoError;
        return response;
    }

    std::map<std::string, bool> m_enables; // key: channel, value: status of enable
private:
    bool parseINI(NameValueText& iniDoc) override {

        for(auto& itor : iniDoc.m_value) {

            //key Parsing
            //Channel.0.Enable=True 형식으로 입력됨.
            std::vector<std::string> keySplit;
            boost::split(keySplit, itor.first, boost::is_any_of("."));

            if(3 > keySplit.size()) {
                SPDLOG_ERROR("Failed to find a exact value : {}", itor.first);
                continue;
            }

            //Enable이 아닌 키는 필요 없다.
            if(0 != keySplit[2].compare("Enable")){
                continue;
            }

            int channel = 0;
            try_stoi(keySplit[1],&channel,0);
            m_enables.emplace(fromSunapiChannel(channel),iniDoc.getBool(itor.first));
            SPDLOG_INFO("AutoTracking View : channel={}, Enable={}", channel , iniDoc.getBool(itor.first));
        }

         return true;
    };

    DeviceGetAutoTrackingRequestSharedPtr m_request;
};

void SunapiDeviceClientImpl::DeviceGetAutoTracking(DeviceGetAutoTrackingRequestSharedPtr const& request, ResponseBaseHandler const& responseHandler )
{
    asyncRequest(m_httpCommandSession,
                 std::make_shared<EventsourcesAutoTrackingView>(m_logPrefix, request),
                 responseHandler,
                 nullptr,
                 AsyncRequestType::HTTPGET, false);
}

//************** DeviceSetAutoTracking ****************************//

class EventsourcesAutoTrackingSet : public BaseCommand
{
public:
    explicit EventsourcesAutoTrackingSet(const std::string& logPrefix, DeviceSetAutoTrackingRequestSharedPtr request)
        :BaseCommand(nullptr,logPrefix,std::make_shared<Wisenet::ResponseBase>(),false,false),
          m_request(request)
    {

    };
    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;
        builder.CreateCgi("eventsources","autotracking", "set")
                .AddParameter("Channel", toSunapiChannel(m_request->channelID))
                .AddParameter("Enable", m_request->enable);

        return builder.GetUrl();
    };

    ResponseBaseSharedPtr ProcessPost() override {
        m_responseBase->errorCode = Wisenet::ErrorCode::NoError;
        return m_responseBase;
    }

private:
    DeviceSetAutoTrackingRequestSharedPtr m_request;
};


void SunapiDeviceClientImpl::DeviceSetAutoTracking(DeviceSetAutoTrackingRequestSharedPtr const& request, ResponseBaseHandler const& responseHandler )
{
    asyncRequest(m_httpCommandSession,
                 std::make_shared<EventsourcesAutoTrackingSet>(m_logPrefix,request),
                 responseHandler,
                 nullptr,
                 AsyncRequestType::HTTPGET, false);
}

//************** DeviceControlTargetLockCoodinate ****************************//

class EventsourcesAutoTrackingTargetLockCoordinateControl : public BaseCommand
{
public:
    explicit EventsourcesAutoTrackingTargetLockCoordinateControl(const std::string& logPrefix,
                                                                 DeviceSetTargetLockCoordinateRequestSharedPtr request)
        :BaseCommand(nullptr,logPrefix,std::make_shared<Wisenet::ResponseBase>(),false,false),
          m_request(request)
    {

    };

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;
        builder.CreateCgi("eventsources","autotracking", "control")
                .AddParameter("Channel", toSunapiChannel(m_request->channelID));

        std::stringstream ss;
        ss << m_request->x << "," << m_request->y;

        builder.AddParameter("TargetLockCoordinate", ss.str());

        return builder.GetUrl();
    };

    ResponseBaseSharedPtr ProcessPost() override {
        m_responseBase->errorCode = Wisenet::ErrorCode::NoError;
        return m_responseBase;
    }

private:
    DeviceSetTargetLockCoordinateRequestSharedPtr m_request;
};

void SunapiDeviceClientImpl::DeviceSetTargetLockCoordinate(DeviceSetTargetLockCoordinateRequestSharedPtr const& request, ResponseBaseHandler const& responseHandler )
{
    asyncRequest(m_httpCommandSession,
                 std::make_shared<EventsourcesAutoTrackingTargetLockCoordinateControl>(m_logPrefix,request),
                 responseHandler,
                 nullptr,
                 AsyncRequestType::HTTPGET, false);
}

}
}
