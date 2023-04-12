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

class IoAlarmOutputControl : public BaseCommand
{
public:
    explicit IoAlarmOutputControl(const std::string& logPrefix, DeviceSetAlarmOutputRequestSharedPtr request)
        :BaseCommand(nullptr,logPrefix,std::make_shared<Wisenet::ResponseBase>(),false,false),
          m_request(request)
     {

     }

    std::string RequestUri() override{
        SunapiSyntaxBuilder builder;
            std::string state = m_request->on? "On" : "Off";
            std::stringstream ss;
            ss << "AlarmOutput" << "." << m_request->deviceOutputID << "." << "State";

            builder.CreateCgi("io", "alarmoutput", "control")
                    .AddParameter(ss.str(), state);

            return builder.GetUrl();
    };

    ResponseBaseSharedPtr ProcessPost() override {
        m_responseBase->errorCode = Wisenet::ErrorCode::NoError;
        return m_responseBase;
    }

private:
    DeviceSetAlarmOutputRequestSharedPtr m_request;
};

void SunapiDeviceClientImpl::DeviceSetAlarmOutput(const DeviceSetAlarmOutputRequestSharedPtr& request,
    const ResponseBaseHandler& responseHandler)
{
    boost::asio::post(m_strand, WeakCallback(shared_from_this(), [this, request, responseHandler]()
        {
         
            auto command = std::make_shared<IoAlarmOutputControl>(m_logPrefix, request);

            //설정이 이미 되어있는지 확인해서 다르면 장비로 명령을 보낸다.
            auto itor = m_alarmOutputStatus.find(request->deviceOutputID);
            if (itor != m_alarmOutputStatus.end()) {
                if (itor->second == request->on) {
                     command->ResponseBase()->errorCode = Wisenet::ErrorCode::NoError;
                    if (responseHandler) {
                        responseHandler(command->ResponseBase());
                    }                    
                    return;
                }
            }

            asyncRequest(m_httpCommandSession,
                command,
                WeakCallback(shared_from_this(),
                    [this, request, responseHandler](const ResponseBaseSharedPtr& response) {

                        if (response->isSuccess()) {
                            //성공하였으면 업데이트를 한다.
                            auto itor = m_alarmOutputStatus.find(request->deviceOutputID);

                            if (itor == m_alarmOutputStatus.end()) {
                                m_alarmOutputStatus.emplace(request->deviceOutputID, request->on);
                            }
                            else {
                                m_alarmOutputStatus[request->deviceOutputID] = request->on;
                            }
                        }
                        
                        if (responseHandler) {
                            responseHandler(response);
                        }
                    
                    }),
                nullptr,
                        AsyncRequestType::HTTPGET, false);
        }));
}

void SunapiDeviceClientImpl::changeAlarmOuputStatus(const std::string& deviceOutputID, bool on)
{
    boost::asio::post(m_strand, WeakCallback(shared_from_this(), [this, deviceOutputID, on]()
    {
            auto itor = m_alarmOutputStatus.find(deviceOutputID);

            if (itor == m_alarmOutputStatus.end()) {
                m_alarmOutputStatus.emplace(deviceOutputID, on);
            }
            else {
                m_alarmOutputStatus[deviceOutputID] = on;
            }

    }));
}

}
}
