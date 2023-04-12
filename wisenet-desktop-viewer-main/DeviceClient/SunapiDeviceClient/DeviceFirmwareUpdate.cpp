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

struct SystemFirmwareUpdateControlResult
{
    boost::optional<std::string>    firmwareModule; // Camera/Encoder only
    boost::optional<std::string>    status;
    boost::optional<int>           progress;
};

class SystemFirmwareUpdateControl : public BaseCommand, public IniParser
{
public:
    bool isProcessing = true;
     Wisenet::ErrorCode prevErrorCode;
    explicit SystemFirmwareUpdateControl(const std::string& logPrefix, DeviceFirmwareUpdateRequestSharedPtr request)
        :BaseCommand(this,logPrefix,std::make_shared<DeviceFirmwareUpdateResponse>(),false,false),
          prevErrorCode(Wisenet::ErrorCode::NoError),
          m_request(request)

    {
    }

    std::string RequestUri() override
    {
        return u8"/stw-cgi/system.cgi?msubmenu=firmwareupdate&action=control&Type=Normal";
    }

    std::string GetFilePath() override
    {
        return m_request->firmwareFilePath;
    }
    void RenewResponse() override
    {
        m_responseBase = std::make_shared<DeviceFirmwareUpdateResponse>();
    }
    void IsContinue(bool isContinue) override
    {
        auto response = std::static_pointer_cast<DeviceFirmwareUpdateResponse>(m_responseBase);
        response->isContinue = isContinue;
    }

    ResponseBaseSharedPtr ProcessPost() override {
        auto response = std::static_pointer_cast<DeviceFirmwareUpdateResponse>(m_responseBase);

        if(parseResult.status && parseResult.status.value()=="Alive")
            return nullptr;
        std::string tempFirmwareModule = parseResult.firmwareModule?parseResult.firmwareModule.value():"";
        std::string tempStatus = parseResult.status?parseResult.status.value():"";
        int tempProgress = parseResult.progress?parseResult.progress.value():0;

        response->errorCode = Wisenet::ErrorCode::NoError;
        response->progressStatus = DeviceFirmwareUpdateResponse::ProgressStatus::Updating;
        if(tempStatus=="Uploading"){
            response->progressStatus = DeviceFirmwareUpdateResponse::ProgressStatus::Uploading;
            response->progressPercent = tempProgress / 4;
        }
        if(tempStatus=="DownloadAck"){
            response->progressStatus = DeviceFirmwareUpdateResponse::ProgressStatus::UploadingAck;
            response->progressPercent = 25 + tempProgress / 4;
        }
        else if(tempStatus == "DownloadOK")
            response->progressPercent = 50;
        else if(tempStatus == "Start")
            response->progressPercent = 55;
        else if(tempStatus == "OK" || tempStatus == "Skip"){

            if(tempFirmwareModule == "Kernel")
                response->progressPercent = 60;
            else if(tempFirmwareModule == "App")
                response->progressPercent = 65;
            else if(tempFirmwareModule == "Web")
                response->progressPercent = 70;
            else if(tempFirmwareModule == "ISP")
                response->progressPercent = 95 ;
        }
        else if(tempStatus == "UpdatingISP"){
            response->progressPercent = 70 + tempProgress / 4;
        }
        else if(tempStatus == "End"){
            response->progressPercent = 100;
           // response->isContinue = false;
        }
        else if(tempStatus == "DownloadFail"){
            prevErrorCode =response->errorCode = Wisenet::ErrorCode::InCompatibleFile;
            isProcessing = false;
          //  response->isContinue = false;
        }
        else if(tempStatus == "Fail"){
            prevErrorCode = response->errorCode = Wisenet::ErrorCode::UpdateFail;
            isProcessing = false;
            //response->isContinue = false;
        }

        if(!isProcessing)
            response->errorCode = prevErrorCode;

        if(response->progressPercent != 0)
            prevProgress = response->progressPercent;
        if(response->progressPercent == 0 && prevProgress !=0)
            response->progressPercent = prevProgress;

        SPDLOG_DEBUG("ProcessPost() errorCode:{} percent:{} progressStatus:{}",
                     response->errorString(), response->progressPercent, response->progressStatus);

        return response;
    }

    SystemFirmwareUpdateControlResult parseResult;
    int prevProgress =0;

private:
    bool parseINI(NameValueText& iniDoc) override
    {
        try {

            parseResult.status = iniDoc.getStringValue("Status");
            parseResult.progress = iniDoc.getIntValue("Progress");
            parseResult.firmwareModule = iniDoc.getStringValue("FirmwareModule");

            if (spdlog::should_log(spdlog::level::debug)) {
                std::stringstream ss;
                if (parseResult.firmwareModule) {
                    ss << "::FirmwareModule=" << parseResult.firmwareModule.value();
                }
                if (parseResult.progress) {
                    ss << "::Progress=" << parseResult.progress.value();
                }
                if (parseResult.status) {
                    ss << "::Status=" << parseResult.status.value();
                }
                SPDLOG_DEBUG("Firmware Updating::{}", ss.str());
            }

        } catch(std::exception& e)
        {
            SPDLOG_ERROR("Firmware update Exception = {}", e.what());
        }


        return true;
    }

    DeviceFirmwareUpdateRequestSharedPtr m_request;
};

void SunapiDeviceClientImpl::DeviceFirmwareUpdate(const DeviceFirmwareUpdateRequestSharedPtr &request,
                                              const ResponseBaseHandler &responseHandler)
{
    asio::post(m_strand, WeakCallback(shared_from_this(),
                                      [this, request, responseHandler]()
    {
        if(request->firmwareStop){
            SLOG_DEBUG("DeviceFirmwareUpdate stop");
            m_isFwupdateCanceling = true;
            m_httpConfigSession->Close();
            return;
        }
        else{
            SLOG_DEBUG("DeviceFirmwareUpdate start");
            m_needToMonitorEventSessionStatus = false;
            m_isRunningEventSession = false;
            m_httpEventSession->Close();
            m_httpConfigSession->Close();

            m_isFwupdating = true;
            m_isFwupdateCanceling = false;

            closeAllMediaSession();
            sendFirmwareUpdateAlarm();
        }
        SLOG_DEBUG("DeviceFirmwareUpdate prev asyncRequest");

        asyncRequest(m_httpConfigSession,
                     std::make_shared<SystemFirmwareUpdateControl>(m_logPrefix,request),
                     [this, responseHandler, request](const ResponseBaseSharedPtr& response)
        {
            auto fwResponse = std::static_pointer_cast<DeviceFirmwareUpdateResponse>(response);

            SLOG_DEBUG("DeviceFirmwareUpdate -- success:{} -- errorCode:{}",
                         fwResponse->isSuccess(), fwResponse->errorString());

            if(!m_isFwupdating){
                SLOG_DEBUG("DeviceFirmwareUpdate Response !m_isFwupdating!!");
                return;
            }
            if(m_isFwupdateCanceling){
                m_isFwupdating = false;
                m_isFwupdateCanceling = false;
                startEventSession();
                fwResponse->errorCode = Wisenet::ErrorCode::FwupdateCanceled;
                SLOG_DEBUG("DeviceFirmwareUpdate Response canceled!!");
            }
            else if(fwResponse->isFailed()){
                m_isFwupdating = false;
                m_isFwupdateCanceling = false;
                startEventSession();
                SLOG_DEBUG("DeviceFirmwareUpdate fail!! - {}", fwResponse->errorCode);
            }
            responseHandler(response);

        }, nullptr,  AsyncRequestType::HTTPFIRMWAREUPDATE, false);
    }));
}

void SunapiDeviceClientImpl::sendFirmwareUpdateAlarm()
{
    DeviceAlarmEventSharedPtr alarm = std::make_shared<DeviceAlarmEvent>();
    alarm->serviceUtcTimeMsec = Wisenet::Common::currentUtcMsecs();
    alarm->deviceUtcTimeMsec = 0;
    alarm->type = "SystemEvent.CoreService.FirmwareUpgrade";

    alarm->isDevice = true;
    alarm->isChannel = false;

    alarm->deviceID = m_repos->device().deviceID;

    alarm->data = true;

    SLOG_DEBUG("DeviceAlarm : {}", alarm->ToString());

    eventHandler(alarm);
}



}
}
