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

//************************* MediaChannelFirmwareUpdateControl ******************************//

struct MediaChannelFirmwareUpdateControlResult
{
    boost::optional<std::string> status;
    boost::optional<int> percent = 0;
};

class MediaChannelFirmwareUpdateControl : public BaseCommand, public IniParser
{
public:
     Wisenet::ErrorCode prevErrorCode;
    explicit MediaChannelFirmwareUpdateControl(const std::string& logPrefix, DeviceChannelFirmwareUpdateRequestSharedPtr request, const bool& downloadFromServer, const std::string& upgradeToken, const int& currentIndex)
        :BaseCommand(this,logPrefix,std::make_shared<DeviceChannelFirmwareUpdateResponse>(),false,false),
          prevErrorCode(Wisenet::ErrorCode::NoError),
          m_request(request),
          m_downloadFromServer(downloadFromServer),
          m_upgradeToken(upgradeToken),
          m_currentIndex(currentIndex)

    {
    }

    std::string RequestUri() override
    {
        std::string filePath = m_request->firmwareUpdatingList[m_currentIndex].firmwarePath;
        std::string firmwareName;
        // folder separator
    #ifdef _WIN32
        static const char folder_sep = '\\';
    #else
        static const char folder_sep = '/';
    #endif
        std::string path = filePath;
        std::replace(path.begin(), path.end(), '/', folder_sep);
        auto pos = path.find_last_of(folder_sep);
        if (pos == std::string::npos)
            firmwareName = path;
        else
            firmwareName = path.substr(pos + 1, path.length() - pos);

        SPDLOG_DEBUG("MediaChannelFirmwareUpdateControl : m_request->firmwareUpdatingList[m_currentIndex].channelID = {}",m_request->firmwareUpdatingList[m_currentIndex].channelID);

        SunapiSyntaxBuilder builder;
        builder.CreateCgi("media","cameraupgrade", "set");
        builder.AddParameter("Mode", "Start");
        builder.AddParameter("Channel", /*채널번호*/toSunapiChannel(m_request->firmwareUpdatingList[m_currentIndex].channelID));
        builder.AddParameter("downloadFromServer", /*서버에서 fw다운로드할지 여부*/m_downloadFromServer);
        builder.AddParameter("UpgradeToken", /*업그레이드 토큰*/m_upgradeToken);
        bool extraChannel = true;
        if(m_currentIndex == m_request->firmwareUpdatingList.size()-1) {
            extraChannel = false;
        }
        builder.AddParameter("Extra", /*업그레이드할 채널 중 마지막 순서인지 여부(마지막 채널이라면 False)*/extraChannel);
        builder.AddParameter("IgnoreMultipartResponse", true);
        builder.AddParameter("FileName", /*업그레이드할 펌웨어 이름*/firmwareName);
        return builder.GetUrl();
    }

    std::string GetFilePath() override
    {
        if(m_request->firmwareUpdatingList.size() > 0) {
            return m_request->firmwareUpdatingList[m_currentIndex].firmwarePath;
        }
        else {
            return "";
        }
    }
    void RenewResponse() override
    {
        m_responseBase = std::make_shared<DeviceChannelFirmwareUpdateResponse>();
    }
    void IsContinue(bool isContinue) override
    {
        auto response = std::static_pointer_cast<DeviceChannelFirmwareUpdateResponse>(m_responseBase);
        response->isContinue = isContinue;
    }

    ResponseBaseSharedPtr ProcessPost() override {
        auto response = std::static_pointer_cast<DeviceChannelFirmwareUpdateResponse>(m_responseBase);

        std::string status = parseResult.status?parseResult.status.value():"-";
        int percent = parseResult.percent?parseResult.percent.value():0;

        response->status = status;
        response->percent = percent;

        SPDLOG_DEBUG("MediaChannelFirmwareUpdateControl::ProcessPost() errorCode:{} status:{} percent:{}", response->errorString(), response->status, response->percent);

        return response;
    }

    MediaChannelFirmwareUpdateControlResult parseResult;

private:
    bool parseINI(NameValueText& iniDoc) override
    {
        try {

            parseResult.status = iniDoc.getStringValue("Status");
            parseResult.percent.value() = std::stoi(iniDoc.getStringValue("Progress").value());

            if (spdlog::should_log(spdlog::level::debug)) {
                std::stringstream ss;
                if (parseResult.status) {
                    ss << "::Status=" << parseResult.status.value();
                }

                if (parseResult.percent) {
                    ss << "::Progress=" << parseResult.percent.value();
                }
                SPDLOG_DEBUG("FW update {}", ss.str());
            }

        } catch(std::exception& e)
        {
            SPDLOG_ERROR("FW update Exception = {}", e.what());
        }

        return true;
    }

    DeviceChannelFirmwareUpdateRequestSharedPtr m_request;
    bool m_downloadFromServer = false;
    std::string m_upgradeToken = "";
    int m_currentIndex = 0;
    bool m_extra = true;
};


//************************* MediaCameraUpgradeRecorderStatusView ******************************//

struct MediaCameraUpgradeRecorderStatusViewResult
{
    std::vector<UpdateStatus> updateStatusList;
    std::string recorderUpgradeStatus;
};

class MediaCameraUpgradeRecorderStatusView : public BaseCommand, public JsonParser
{
public:
    Wisenet::ErrorCode prevErrorCode;
    explicit MediaCameraUpgradeRecorderStatusView(const std::string& logPrefix, DeviceUpgradeStatusRequestSharedPtr request, const std::string upgradeToken)
        :BaseCommand(this,logPrefix,std::make_shared<DeviceUpgradeStatusResponse>(),false,false),
          prevErrorCode(Wisenet::ErrorCode::NoError),
          m_request(request),
          m_upgradeToken(upgradeToken)
    {
    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;
        builder.CreateCgi("media","cameraupgrade", "view");
        builder.AddParameter("SkipUpgradeServerCheck", true);
        builder.AddParameter("UpgradeToken", m_upgradeToken);
        return builder.GetUrl();
    }

    ResponseBaseSharedPtr ProcessPost() override {
        auto response = std::static_pointer_cast<DeviceUpgradeStatusResponse>(m_responseBase);

        response->updateStatusList.clear();
        response->updateStatusList.assign(parseResult.updateStatusList.begin(), parseResult.updateStatusList.end());

        std::string recorderUpgradeStatus = parseResult.recorderUpgradeStatus;

        response->recorderUpgradeStatus = recorderUpgradeStatus;

        return response;
    }

    MediaCameraUpgradeRecorderStatusViewResult parseResult;

private:
    bool parseJson(rapidjson::Document& jsonDoc) override
    {
        auto statusItr = jsonDoc.FindMember("Status");
        if(statusItr != jsonDoc.MemberEnd()) {
//            SPDLOG_DEBUG("DeviceUpgradeStatus statusItr={}", statusItr->value.GetString());
            auto upgradeStatusItr = statusItr->value.FindMember("UpgradeStatus");
            if(upgradeStatusItr != statusItr->value.MemberEnd()) {
                std::string totalUpgradeStatus = upgradeStatusItr->value.GetString();
                parseResult.recorderUpgradeStatus = totalUpgradeStatus;
                SPDLOG_DEBUG("DeviceUpgradeStatus recorderUpgradeStatus={}", totalUpgradeStatus);
            }
        }

        for(auto channel : m_request->firmwareUpdatingList) {
            auto channelId = std::to_string(stoi(channel.channelID)-1);

            auto dataItr = jsonDoc.FindMember("Data");
            if (dataItr != jsonDoc.MemberEnd()) {
                auto &results = dataItr->value;
                for(auto &channelResult : results.GetArray()) {
//                    SPDLOG_DEBUG("DeviceUpgradeStatus channelResult={} channelId={}", channelResult.GetString(), channelId);

                    auto camItr = channelResult.FindMember("Channel");
//                    SPDLOG_DEBUG("DeviceUpgradeStatus channelResult={} channelId={}", std::to_string(camItr->value.GetInt()), channelId);
                    if(camItr != channelResult.MemberEnd() && std::to_string(camItr->value.GetInt()) == channelId) {

                        auto currentFwVersionItr = channelResult.FindMember("Status");
                        if(currentFwVersionItr != channelResult.MemberEnd()) {
                            std::string camStatus = currentFwVersionItr->value.GetString();
                            parseResult.updateStatusList.push_back(UpdateStatus(channel.channelID, camStatus));
                            SPDLOG_DEBUG("DeviceUpgradeStatus ch({})status={}", channel.channelID, camStatus);
                        }
                    }
                }
            }
        }

        return true;
    }

    DeviceUpgradeStatusRequestSharedPtr m_request;
    std::string m_upgradeToken = "";
};

//************************* MediaCameraUpgradeRecorderStatusRequestTokenControl ******************************//

struct MediaCameraUpgradeRequestTokenControlResult
{
    boost::optional<std::string> upgradeToken;
};

class MediaCameraUpgradeRequestTokenControl : public BaseCommand, public IniParser
{
public:
    Wisenet::ErrorCode prevErrorCode;
    explicit MediaCameraUpgradeRequestTokenControl(const std::string& logPrefix, GetDeviceUpgradeTokenRequestSharedPtr request)
        :BaseCommand(this,logPrefix,std::make_shared<GetDeviceUpgradeTokenResponse>(),false,false),
          prevErrorCode(Wisenet::ErrorCode::NoError),
          m_request(request)

    {
    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;
        builder.CreateCgi("media","cameraupgrade", "control");
        builder.AddParameter("Mode", "Start");
        return builder.GetUrl();
    }

    ResponseBaseSharedPtr ProcessPost() override {
        auto response = std::static_pointer_cast<GetDeviceUpgradeTokenResponse>(m_responseBase);
        std::string upgradeToken = parseResult.upgradeToken?parseResult.upgradeToken.value():"-";

        response->upgradeToken = upgradeToken;

        return response;
    }

    MediaCameraUpgradeRequestTokenControlResult parseResult;

private:
    bool parseINI(NameValueText& iniDoc) override
    {
        try {

            parseResult.upgradeToken = iniDoc.getStringValue("UpgradeToken");

            if (spdlog::should_log(spdlog::level::debug)) {
                std::stringstream ss;
                if (parseResult.upgradeToken) {
                    ss << "::UpgradeToken=" << parseResult.upgradeToken.value();
                }
                SPDLOG_DEBUG("Getting Upgrade Token{}", ss.str());
            }

        } catch(std::exception& e)
        {
            SPDLOG_ERROR("Getting Upgrade Token Exception = {}", e.what());
        }

        return true;
    }

    GetDeviceUpgradeTokenRequestSharedPtr m_request;
};

//************************* MediaRenewUpgradeTokenControl ******************************//

class MediaRenewUpgradeTokenControl : public BaseCommand, public IniParser
{
public:
    Wisenet::ErrorCode prevErrorCode;
    explicit MediaRenewUpgradeTokenControl(const std::string& logPrefix, const std::string upgradeToken)
        :BaseCommand(this,logPrefix, std::make_shared<Wisenet::ResponseBase>(),false,false), //std::shared_ptr<ResponseBase>(), false , false),
          prevErrorCode(Wisenet::ErrorCode::NoError),
          m_upgradeToken(upgradeToken)

    {
    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;
        builder.CreateCgi("media","cameraupgrade", "control");
        builder.AddParameter("Mode", "Renew");
        builder.AddParameter("UpgradeToken", m_upgradeToken);
        return builder.GetUrl();
    }

    ResponseBaseSharedPtr ProcessPost() override {
        return nullptr;
    }

private:
    bool parseINI(NameValueText& iniDoc) override
    {
        return true;
    }

    std::string m_upgradeToken = "";
};

void SunapiDeviceClientImpl::DeviceChannelFirmwareUpdate(const DeviceChannelFirmwareUpdateRequestSharedPtr &request,
                                              const ResponseBaseHandler &responseHandler)
{
    auto upgradeTokenRequest = std::make_shared<Wisenet::Device::DeviceRequestBase>();
    upgradeTokenRequest->deviceID = request->deviceID;

    SPDLOG_DEBUG("DeviceChannelFirmwareUpdate::DeviceChannelFirmwareUpdate start!! request->firmwareStop={}", request->firmwareStop);

    if(request->firmwareStop){
        SLOG_DEBUG("DeviceChannelFirmwareUpdate stop");
        m_isChannelFwUpdateCanceling = true;
        m_httpConfigSession->Close();
        return;
    }

//    m_uploadingWaitCount = 0;
    // request upgrade token
    asyncRequest(m_httpConfigSession,
                 std::make_shared<MediaCameraUpgradeRequestTokenControl>(m_logPrefix,upgradeTokenRequest),
                 [this, responseHandler, request, upgradeTokenRequest](const ResponseBaseSharedPtr& response)
    {
        auto tokenResponse = std::static_pointer_cast<GetDeviceUpgradeTokenResponse>(response);

        if(tokenResponse->isSuccess()){
//            SPDLOG_DEBUG("DeviceChannelFirmwareUpdate::GetDeviceUpgradeToken success!! - {}", tokenResponse->upgradeToken);
            // TO-DO : 업그레이드 토큰 갱신 필요

            // upgrade post
            asio::post(m_strand, WeakCallback(shared_from_this(),
                                              [=]()
            {
                if(request->firmwareStop){
                    SLOG_DEBUG("DeviceChannelFirmwareUpdate stop");
                    m_isChannelFwUpdateCanceling = true;
                    m_httpConfigSession->Close();
                    return;
                }
                else{
                    SLOG_DEBUG("DeviceChannelFirmwareUpdate start");
                    m_needToMonitorEventSessionStatus = false;
                    m_isRunningEventSession = false;
                    m_httpEventSession->Close();
                    m_httpConfigSession->Close();

                    m_isChannelFwUpdating = true;
                    m_isChannelFwUpdateCanceling = false;

                    closeAllMediaSession();
                    sendFirmwareUpdateAlarm();
                }
                SLOG_DEBUG("DeviceChannelFirmwareUpdate::firmwareUpdatingList.size={}", request->firmwareUpdatingList.size());

                // 업그레이드 토큰 갱신을 위한 renew 30초 주기 실행
                m_isChannelFwUploading = true;
                StartChannelUpdateTimer(tokenResponse->upgradeToken, request, responseHandler, 30000);

                ContinueChannelUpgrade(tokenResponse->upgradeToken, request, responseHandler, 0);
            }));
        }
        else if(tokenResponse->isFailed()){
            SPDLOG_DEBUG("GetDeviceUpgradeToken fail!! - {}", tokenResponse->errorCode);
            StopChannelUpdateTimer();
        }

    }, nullptr, AsyncRequestType::HTTPGET, false);
}

void SunapiDeviceClientImpl::ContinueChannelUpgrade(const std::string upgradeToken,
                                                    const DeviceChannelFirmwareUpdateRequestSharedPtr &request,
                                                    const ResponseBaseHandler &responseHandler,
                                                    const int idx)
{
    if(idx > request->firmwareUpdatingList.size()) {
        return;
    }

    if(idx == request->firmwareUpdatingList.size()) {
        m_isChannelFwUploading = false;
        DeviceChannelUpgradeStatusView(upgradeToken, request, responseHandler);
        return;
    }

    asio::post(m_strand, WeakCallback(shared_from_this(),
                                      [=]()
    {

        bool downloadFromServer = false; // TO-DO : 서버로부터 다운로드 받는 경우가 추가된다면 아래 downloadFromServer 유무 판별 구현
//        SLOG_DEBUG("DeviceChannelFirmwareUpdate::asyncRequest loop ChIdx({})",idx);

        // request update
        asyncRequest(m_httpConfigSession,
                     std::make_shared<MediaChannelFirmwareUpdateControl>(m_logPrefix, request, downloadFromServer, upgradeToken, idx),
                     [=](const ResponseBaseSharedPtr& response)
        {
            auto updateResponse = std::static_pointer_cast<DeviceChannelFirmwareUpdateResponse>(response);
            SPDLOG_DEBUG("DeviceChannelFirmwareUpdate::ChannelFirmwareUpdateControl - success={} idx[{}] chID={} path={}", updateResponse->isSuccess(), idx, request->firmwareUpdatingList[idx].channelID, request->firmwareUpdatingList[idx].firmwarePath);

            if(updateResponse->isSuccess()){
                SLOG_DEBUG("DeviceChannelFirmwareUpdate::Uploading...({}%)",updateResponse->percent);

                if(updateResponse->percent == 100) {
                    // continue upgrade for next channel of same recorder
                    SLOG_DEBUG("DeviceChannelFirmwareUpdate::ChannelFirmwareUpdateControl - upgrade request({})", idx+1);
                    ContinueChannelUpgrade(upgradeToken, request, responseHandler, idx+1);
                }
            }
            else if(!m_isChannelFwUpdating){
                SLOG_DEBUG("DeviceChannelFirmwareUpdate Response - !m_isFwupdating!!");
                return;
            }
            if(m_isChannelFwUpdateCanceling){
                SLOG_DEBUG("DeviceChannelFirmwareUpdate Response - m_isChannelFwUpdateCanceling!!");
                m_isChannelFwUpdating = false;
                m_isChannelFwUpdateCanceling = false;
                startEventSession();
                updateResponse->errorCode = Wisenet::ErrorCode::FwupdateCanceled;

                StopChannelUpdateTimer();
            }
            else if(updateResponse->isFailed()){
                SLOG_DEBUG("DeviceChannelFirmwareUpdate fail!! - {}", updateResponse->errorString());
                m_isChannelFwUpdating = false;
                m_isChannelFwUpdateCanceling = false;
                startEventSession();

                auto statResponse = std::static_pointer_cast<DeviceUpgradeStatusResponse>(response);
                statResponse->errorCode = Wisenet::ErrorCode::NetworkError;
                responseHandler(statResponse);
            }

        }, nullptr,  AsyncRequestType::HTTPFIRMWAREUPDATE, false);
    }));
}

void SunapiDeviceClientImpl::DeviceChannelUpgradeStatusView(const std::string upgradeToken,
                                                            const DeviceChannelFirmwareUpdateRequestSharedPtr &upgradeRequest,
                                                            const ResponseBaseHandler &responseHandler)
{
    SLOG_DEBUG("DeviceChannelUpgradeStatusView start");

    if(m_isChannelFwUpdateCanceling) {
        return;
    }

    auto updateStatusRequest = std::make_shared<Wisenet::Device::DeviceUpgradeStatusRequest>();
    updateStatusRequest->deviceID = upgradeRequest->deviceID;
    updateStatusRequest->firmwareUpdatingList = upgradeRequest->firmwareUpdatingList;

    asyncRequest(m_httpChannelUpgradeSession,
                 std::make_shared<MediaCameraUpgradeRecorderStatusView>(m_logPrefix, updateStatusRequest, upgradeToken),
                 [=](const ResponseBaseSharedPtr& response)
    {
        for(auto channel : updateStatusRequest->firmwareUpdatingList) {
            SLOG_DEBUG("DeviceChannelFirmwareUpdate::DeviceChannelUpgradeStatusView - request : channel ID={} fwPath={}", channel.channelID, channel.firmwarePath);
        }
        auto statResponse = std::static_pointer_cast<DeviceUpgradeStatusResponse>(response);

        for(auto channelStat : statResponse->updateStatusList) {
            SLOG_DEBUG("DeviceChannelFirmwareUpdate::DeviceChannelUpgradeStatusView - statResponse : channel ID={} status={}", channelStat.channelID, channelStat.status);
        }

        if(statResponse->isSuccess()){
            SLOG_DEBUG("DeviceChannelFirmwareUpdate::DeviceChannelUpgradeStatusView - request success!!");

            if(statResponse->recorderUpgradeStatus != "DONE") {
//                if(statResponse->recorderUpgradeStatus == "NONE") {
//                    ++m_uploadingWaitCount;
//                    SLOG_DEBUG("DeviceChannelFirmwareUpdate::DeviceChannelUpgradeStatusView - statResponse->recorderUpgradeStatus is NONE uploadingWaitCount:{}", m_uploadingWaitCount);
//                    if(m_uploadingWaitCount == m_uploadingFailCount) {
//                        statResponse->errorCode = Wisenet::ErrorCode::NetworkError;
//                        m_uploadingWaitCount = 0;
//                        responseHandler(statResponse);
//                        return;
//                    }
//                }

                // 타이머 : 주기적으로 status view 요청
                SLOG_DEBUG("DeviceChannelFirmwareUpdate::DeviceChannelUpgradeStatusView - upgrading..({})", statResponse->recorderUpgradeStatus);
                // 중간 결과를 ViewModel에 전달
                responseHandler(statResponse);
                // 5초 뒤에 status 요청
                StartChannelUpdateTimer(upgradeToken, upgradeRequest, responseHandler, 5000);
            }
            else {
                SLOG_DEBUG("DeviceChannelFirmwareUpdate::DeviceChannelUpgradeStatusView - upgrade completed!!");
                responseHandler(statResponse);
            }
        }

        if(statResponse->isFailed()) {
            // status view 요청 실패
            SLOG_DEBUG("DeviceChannelFirmwareUpdate::DeviceChannelUpgradeStatusView fail!!");
            responseHandler(statResponse);
            // 10초 뒤에 다시 status 요청
//            StartChannelUpdateTimer(upgradeToken, upgradeRequest, responseHandler, 10000);
        }
    }, nullptr, AsyncRequestType::HTTPGET, false);
}

void SunapiDeviceClientImpl::StartChannelUpdateTimer(const std::string upgradeToken,
                                                           const DeviceChannelFirmwareUpdateRequestSharedPtr &upgradeRequest,
                                                           const ResponseBaseHandler &responseHandler,
                                                           const long long durationMsec)
{
    SLOG_DEBUG("StartChannelUpdateTimer start");
    m_channelUpdateStatusTimer.cancel();
    m_channelUpdateStatusTimer.expires_after(std::chrono::milliseconds(durationMsec));
    m_channelUpdateStatusTimer.async_wait( WeakCallback(shared_from_this(),
                                              [=](const boost::system::error_code& ec) {
        if (ec) {
            return;
        }

        boost::asio::post(m_strand, WeakCallback(shared_from_this(), [=]() {
            SLOG_DEBUG("StartChannelUpdateTimer({}msec)",durationMsec);

            if(m_isChannelFwUploading) {
                DeviceRenewUpgradeToken(upgradeToken, upgradeRequest, responseHandler);
            }
            else {
                DeviceChannelUpgradeStatusView(upgradeToken, upgradeRequest, responseHandler);
            }
        }));
    }));
}

void SunapiDeviceClientImpl::DeviceRenewUpgradeToken(const std::string upgradeToken,
                                                     const DeviceChannelFirmwareUpdateRequestSharedPtr &upgradeRequest,
                                                     const ResponseBaseHandler &responseHandler)
{
    SLOG_DEBUG("DeviceRenewUpgradeToken start");

    if(m_isChannelFwUpdateCanceling) {
        return;
    }

    asyncRequest(m_httpChannelUpgradeSession,
                 std::make_shared<MediaRenewUpgradeTokenControl>(m_logPrefix, upgradeToken),
                 [=](const ResponseBaseSharedPtr& response)
    {
        auto renewTokenResponse = std::static_pointer_cast<ResponseBase>(response);

        if(renewTokenResponse->isSuccess()){
            SLOG_DEBUG("DeviceChannelFirmwareUpdate::DeviceRenewUpgradeToken - request success!!");
            // renew 성공 -> 30초 뒤에 renew 요청
            StartChannelUpdateTimer(upgradeToken, upgradeRequest, responseHandler, 30000);
        }

        if(renewTokenResponse->isFailed()) {
            // status view 요청 실패
            SLOG_DEBUG("DeviceChannelFirmwareUpdate::DeviceRenewUpgradeToken fail!!");
            // 30초 뒤에 renew 요청
            StartChannelUpdateTimer(upgradeToken, upgradeRequest, responseHandler, 30000);
        }
    }, nullptr, AsyncRequestType::HTTPGET, false);
}

void SunapiDeviceClientImpl::StopChannelUpdateTimer()
{
    asio::dispatch(m_strand, WeakCallback(shared_from_this(), [this]()
    {
        m_channelUpdateStatusTimer.cancel();
    }));
}

}
}
