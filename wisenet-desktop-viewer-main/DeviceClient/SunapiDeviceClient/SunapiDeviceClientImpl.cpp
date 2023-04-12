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
#include "SunapiDeviceClientLogSettings.h"
#include "cgis/BaseCommand.h"
#include <boost/lexical_cast.hpp>

#include "TimeUtil.h"

namespace Wisenet
{
namespace Device
{

static bool parseSSLPortFromLocation(const std::string &location, unsigned short& retSslPort)
{
    if (!boost::starts_with(location, "https://"))
        return false;

    retSslPort = 443;
    std::string url = location;
    boost::erase_first(url, "https://");
    std::vector<std::string>addresses;
    boost::split(addresses, url, boost::is_any_of("/"));
    if (addresses.size() > 0) {
        std::vector<std::string> ports;
        boost::split(ports, addresses[0], boost::is_any_of(":"), boost::token_compress_on);
        if (ports.size() == 2)        {
            try{
                unsigned short port = boost::lexical_cast<unsigned short>(ports[1]);
                retSslPort = port;
                return true;
            }
            catch (const boost::bad_lexical_cast&) {
                SPDLOG_DEBUG("ParseSSLPortFromUrl():: bad lexical cast()");
            }
        }
    }

    return false;
}

static Wisenet::ErrorCode HttpResponseToErrorCode(const HttpResponseSharedPtr &response)
{
    if (!response->failed)
        return Wisenet::ErrorCode::NoError;

     SPDLOG_DEBUG("Wisenet::ErrorCode HttpResponseToErrorCode():: {} {} {}", response->httpStatus, response->error.value(), response->error.message());

    if (response->httpStatus != 200) {
        boost::beast::http::status httpStatus = boost::beast::http::int_to_status(response->httpStatus);
        unsigned int nStatus = response->httpStatus;
        if (httpStatus == boost::beast::http::status::unknown) {
            switch (nStatus)
            {
            case 490:
                return Wisenet::ErrorCode::AccountBlocked;
            case 491:
                return Wisenet::ErrorCode::AccountBlocked;
            case 702 :
                return Wisenet::ErrorCode::SystemMenuUsed;
            default:
                return Wisenet::ErrorCode::NetworkError;
            }
        }
        else {
            switch (httpStatus)
            {
            case boost::beast::http::status::moved_permanently:
                return Wisenet::ErrorCode::MovedPermanently;
            case boost::beast::http::status::found:
                return Wisenet::ErrorCode::MovedPermanently;
            case boost::beast::http::status::unauthorized:
                return Wisenet::ErrorCode::UnAuthorized;
            case boost::beast::http::status::forbidden:
                return Wisenet::ErrorCode::UnAuthorized;
            default :
                return Wisenet::ErrorCode::NetworkError;
            }
        }
    }

    if (response->error) {
        if (response->error == boost::beast::error::timeout)
            return Wisenet::ErrorCode::NoResponse;
        else if(response->error.message() == "tlsv13 alert certificate required")
            return Wisenet::ErrorCode::MutualAuthenticationError;
    }
    return Wisenet::ErrorCode::NetworkError;
}

SunapiDeviceClientImpl::SunapiDeviceClientImpl(boost::asio::io_context& ioc,
                                               boost::asio::io_context& streamIoc,
                                               std::string deviceUUID,
                                               std::shared_ptr<Wisenet::Library::CloudService> cloudService,
                                               long long keepAliveTime,
                                               bool allowRelay,
                                               unsigned short s1DaemonPort)
    : m_strand(boost::asio::make_strand(ioc.get_executor()))
    , m_deviceIoc(ioc)
    , m_streamIoc(streamIoc)
    , m_deviceUUID(deviceUUID)
    , m_httpCommandSession(std::make_shared<HttpSession>(ioc))
    , m_httpEventSession(std::make_shared<HttpSession>(ioc))
    , m_httpPosEventSession(std::make_shared<HttpSession>(ioc))
    , m_httpPtzSession(std::make_shared<HttpSession>(ioc))
    , m_httpConfigSession(std::make_shared<HttpSession>(ioc))
    , m_httpSearchMetaDataSession(std::make_shared<HttpSession>(ioc))
    , m_httpChannelUpgradeSession(std::make_shared<HttpSession>(ioc))
    , m_repos(std::make_shared<DeviceObjectComposer>())
    , m_easyConnection(m_deviceUUID)
    , m_cloudService(cloudService)
    , m_keepAliveTimer(ioc)
    , m_changeConfigUriTimer(ioc)
    , m_aiSearchStatusTimer(ioc)
    , m_channelUpdateStatusTimer(ioc)
    , m_needToMonitorEventSessionStatus(true)
    , m_isRunningEventSession(false)
    , m_isConnecting(false)
    , m_isCompletedConnection(false)
    , m_isFwupdating(false)
    , m_keepAliveTime(keepAliveTime)
    , m_allowRelay(allowRelay)
    , m_startRelayMode(0)
    , m_isReleased(false)
    , m_s1DaemonPort(s1DaemonPort)
{
    m_logPrefix = "[SunapiClient]("+m_deviceUUID+") ";
    SLOG_DEBUG("Create a new SunapiDeviceClient instance");
}

SunapiDeviceClientImpl::~SunapiDeviceClientImpl()
{
    SLOG_DEBUG("Destroy a SunapiDeviceClient instance");
    m_eventHandler = nullptr;
}

bool SunapiDeviceClientImpl::IsReleased()
{
    return m_isReleased;
}

void SunapiDeviceClientImpl::SetDeviceEventHandler(DeviceEventBaseHandler const& eventHandler)
{
    m_eventHandler = eventHandler;

    StartKeepAliveTimer();
}

void SunapiDeviceClientImpl::eventHandler(const DeviceEventBaseSharedPtr &event)
{
    if (m_eventHandler) {
        m_eventHandler(event);
    }
}

void SunapiDeviceClientImpl::SetDeviceLatestStatusEventHandler(DeviceEventBaseHandler const& eventHandler)
{
    m_deviceLatestStatusEventHandler = eventHandler;
}

void SunapiDeviceClientImpl::deviceLatestStatusEventHandler(const DeviceEventBaseSharedPtr &event)
{
    if (m_deviceLatestStatusEventHandler) {
        m_deviceLatestStatusEventHandler(event);
    }
}

void SunapiDeviceClientImpl::Release()
{
    SLOG_INFO("SunapiDeviceClientImpl::Release start");
    m_isReleased = false;
    SLOG_INFO("SunapiDeviceClientImpl::Release StopKeepAliveTimer start");
    StopKeepAliveTimer();
    SLOG_INFO("SunapiDeviceClientImpl::Release StopKeepAliveTimer end");
    closeAll();
    SLOG_INFO("SunapiDeviceClientImpl::Release closeAll end");
    SLOG_INFO("SunapiDeviceClientImpl::Release end");
}

/**
 * @brief HTTP GET 요청을 처리합니다.
 *
 * 다음과 같은 순서로 처리됩니다.
 * 1.STRAND로 비동기로 처리를 시작합니다.
 * 2.ignoreConnectStatus 가 false인경우 연결중인지 확인하여 연결되지 않은 경우 에러처리합니다.
 * 3.HttpGetRequest 요청을 합니다.
 * 4.요청에 대한 응답을 받으면 STRAND로 비동기 처리를 합니다. (DEVICE CLIENT의 STRAND 내에서 처리가 되어야 동기화 이슈가 없습니다.)
 * 5.HTTP응답을 확인하고 BaseCommand::IgnoreHttpErrorResponse 가 false인경우, 에러가 발생된 경우면 에러응답 처리를 합니다.
 * 6.BaseCommand::ParseContent 로 컨텐츠를 파싱합니다.
 * 7.ParseContent의 결과를 확인하고 BaseCommand::IgnoreParsingErrorResponse()가 false인경우, 에러발가 생된 경우면 에러응처리답 를 합니다.
 * 8.BaseCommand::ProcessPost 로 ResponseBase객체가 리턴되면 응답처리를 합니다.
 * 9.processNextCommandFuncr가 nullptr이 아닌 경우, processNextCommandFunc를 호출하여 다음을 진행합니다.
 * @param httpSession HTTP Get요청을 할 세션
 * @param baseCommand HTTP 요청 응답처리를 할 명령
 * @param responseHandler 응답 Handler
 * @param processNextCommandFunc 응답 후 처리할 함수 포인터
 * @param ignoreConnectStatus 접속 상태를 확인하여 접속 상태경우가 아니면 에러 처리를 할지 결정하는 변수.
 */

void SunapiDeviceClientImpl::asyncRequest(std::shared_ptr<HttpSession> httpSession,
                     BaseCommandSharedPtr baseCommand,
                     ResponseBaseHandler const& responseHandler,
                     ProcessNextCommandFunc processNextCommandFunc,
                     AsyncRequestType requestType,
                     bool ignoreConnectStatus,
                     ProcessConnectFailFunc processConnectFailFunc,
                                          int timeout)
{
    boost::asio::post(m_strand, WeakCallback(shared_from_this(), [this, httpSession, baseCommand, responseHandler, processNextCommandFunc, requestType, ignoreConnectStatus,processConnectFailFunc, timeout]()
    {
        if (!ignoreConnectStatus && !isGood()) {
            SLOG_ERROR("Already disconnected :: url={})", baseCommand->RequestUri());

            baseCommand->ResponseBase()->errorCode = Wisenet::ErrorCode::NetworkError;
            if(responseHandler) {
                responseHandler(baseCommand->ResponseBase());
            }

            return;
        }

        auto responseFunc =  [this, httpSession, baseCommand, responseHandler, processNextCommandFunc, requestType, ignoreConnectStatus,processConnectFailFunc]
                (const HttpResponseSharedPtr& httpResponse)
        {
            asio::post(m_strand, WeakCallback(shared_from_this(), [this, httpSession, baseCommand, responseHandler, httpResponse, processNextCommandFunc, requestType, ignoreConnectStatus,processConnectFailFunc]() mutable
            {
//                SLOG_ERROR("(httpResponse->isContinue={})", httpResponse->isContinue);
//                SLOG_ERROR("(httpResponse->httpBody={})", httpResponse->httpBody);
                baseCommand->RenewResponse();
                baseCommand->IsContinue(httpResponse->isContinue);

                auto errorCode = HttpResponseToErrorCode(httpResponse);
                if (!baseCommand->IgnoreHttpErrorResponse() && (Wisenet::ErrorCode::NoError != errorCode)) {

                    if (errorCode == Wisenet::ErrorCode::MovedPermanently) {
                        unsigned short sslPort = 0;
                        if (parseSSLPortFromLocation(httpResponse->movedLocation, sslPort)) {
                            SLOG_INFO("Retry to connect with SSL port. {}", httpResponse->movedLocation);
                            httpSession->Close();
                            m_connectionInfo.isSSL = true;

                            // P2P 접속 중에는 LOCALHOST PORT를 사용해야 하므로 응답값의 SSLPORT를 사용하지 않는다.
                            // P2P 접속이 되면 sslPort는 실제 접속가능한 Port로 변경해 놓는다.
                            if (ConnectedType::P2P != m_connectionInfo.connectedType &&
                                    ConnectedType::P2P_RELAY != m_connectionInfo.connectedType &&
                                    ConnectedType::S1_SIP != m_connectionInfo.connectedType &&
                                    ConnectedType::S1_SIP_RELAY != m_connectionInfo.connectedType)
                            {
                                m_connectionInfo.sslPort = sslPort;
                            }
                            
                            setHttpConnection(httpSession, m_connectionInfo);
                            asyncRequest(httpSession, baseCommand, responseHandler, processNextCommandFunc, requestType, ignoreConnectStatus);
                            return;
                        }
                    }

                    SLOG_ERROR("Failed to request a command:: (url={}, error={}, status={})",
                                baseCommand->RequestUri(),
                                httpResponse->error.message(), httpResponse->httpStatus);

                    if(processConnectFailFunc){
                        SLOG_INFO("Proceed with additional actions for connection failure");
                        processConnectFailFunc(errorCode);
                        return;
                    }

                    baseCommand->ResponseBase()->errorCode = errorCode;
                    if(responseHandler) {
                        responseHandler(baseCommand->ResponseBase());
                    }
                    return;
                }

                //응답받은 시간을 업데이트.
                m_lastReveivedTime = std::chrono::steady_clock::now();

                auto ret = baseCommand->ParseContent(httpResponse->contentType,httpResponse->httpBody);

                if(!baseCommand->IgnoreParsingErrorResponse() && !ret){
                    SLOG_ERROR("Failed to find a value:: (url={}, error={}, status={})",
                                baseCommand->RequestUri(),
                                ret.errorDetails, ret.errorCode);
                    baseCommand->ResponseBase()->errorCode = ret.serviceErrorCode;
                    if(responseHandler) {
                        responseHandler(baseCommand->ResponseBase());
                    }
                    return;
                }

                auto response = baseCommand->ProcessPost();

                if(response) {
                    if(Wisenet::ErrorCode::NoError != response->errorCode) {
                        SLOG_ERROR("Failed to process PostProcessing:: (url={}, error={}, status={})",
                                   baseCommand->RequestUri(),
                                   ret.errorDetails, ret.errorCode);
                    }

                    if(responseHandler) {
                        responseHandler(baseCommand->ResponseBase());
                    }

                    if(Wisenet::ErrorCode::NoError != response->errorCode) {
                        return;
                    }
                }

                if(processNextCommandFunc){
                    processNextCommandFunc();
                }

            }));
        };

        if(AsyncRequestType::HTTPPOSTENCODEDDATA == requestType) {

            httpSession->HttpPostEncodedData(baseCommand->RequestUri(),
                                             baseCommand->GetPostData(),
                                             WeakCallback(shared_from_this(),std::move(responseFunc)));

        } else if(AsyncRequestType::HTTPGETDOWNLOAD == requestType) {

            httpSession->HttpGetDownload(baseCommand->RequestUri(),
                                             baseCommand->GetFilePath(),
                                             WeakCallback(shared_from_this(),std::move(responseFunc)));
        } else if(AsyncRequestType::HTTPPOSTDOWNLOAD == requestType) {

            httpSession->HttpPostDownload(baseCommand->RequestUri(),
                                             baseCommand->GetPostData(),
                                             baseCommand->GetFilePath(),
                                             WeakCallback(shared_from_this(),std::move(responseFunc)));
        }
        else if(AsyncRequestType::HTTPPOSTENCODEDUPLOAD == requestType) {

            httpSession->HttpPostEncodedUpload(baseCommand->RequestUri(),
                                             baseCommand->GetFilePath(),
                                             WeakCallback(shared_from_this(),std::move(responseFunc)));

        }
        else if(AsyncRequestType::HTTPPOSTENCODEDUPLOADPASSWORD == requestType) {

            httpSession->HttpPostEncodedUploadPassword(baseCommand->RequestUri(),
                                             baseCommand->GetPostData(),
                                             baseCommand->GetFilePath(),
                                             WeakCallback(shared_from_this(),std::move(responseFunc)));

        }
        else if(AsyncRequestType::HTTPFIRMWAREUPDATE == requestType) {

            httpSession->HttpPostUpdateFirmware(baseCommand->RequestUri(),
                                             baseCommand->GetFilePath(),
                                             WeakCallback(shared_from_this(),std::move(responseFunc)),120);

        } else if(AsyncRequestType::HTTPGET == requestType || AsyncRequestType::HTTPBIGGET == requestType) {
            SLOG_DEBUG("SunapiDeviceClientImpl::asyncRequest :: (url={}) timeout={}",baseCommand->RequestUri(), timeout);
            httpSession->HttpGetRequest(baseCommand->RequestUri(),
                                        baseCommand->NeedJsonResponse(),
                                        WeakCallback(shared_from_this(),std::move(responseFunc)), timeout,
                                        (requestType==AsyncRequestType::HTTPBIGGET)?true:false);

        }

        else {

            SLOG_ERROR("Unkown AsyncRequestType :: (url={})",baseCommand->RequestUri());
            baseCommand->ResponseBase()->errorCode = Wisenet::ErrorCode::InvalidRequest;
            if(responseHandler) {
                responseHandler(baseCommand->ResponseBase());
            }

            return;
        }

    }));
}



bool SunapiDeviceClientImpl::isGood()
{
    return (m_repos->status().isGood());
}

void SunapiDeviceClientImpl::StartKeepAliveTimer(long long keepAliveTime)
{
    long long expireTime = 0;
    if (0 == keepAliveTime) {
        expireTime = m_keepAliveTime; // 설정값으로 한다.
    }
    else {
        expireTime = keepAliveTime; // 인증 실패의 경우이다.
    }

    if (0 == expireTime) {
        SLOG_INFO("[KEEP ALIVE DEVICESESSION] a Keep Alive Timer for a device is aborted for settings.");
        return;
    }
    
    m_keepAliveTimer.expires_after(std::chrono::seconds(expireTime));
    m_keepAliveTimer.async_wait( WeakCallback(shared_from_this(),
                                              [this, expireTime](const boost::system::error_code& ec)
    {

        if (ec) {
            if (ec == boost::asio::error::operation_aborted)
                SLOG_DEBUG("[KEEP ALIVE DEVICESESSION] a Keep Alive Timer for a device is aborted");
            return;
        }

        boost::asio::post(m_strand, WeakCallback(shared_from_this(), [this, expireTime]()
        {
            SLOG_DEBUG("[KEEP ALIVE DEVICESESSION] Checking KeepAlive a device session");
            //1. 체크를 하지 않는 상태 (DeviceStatusType::DisconnectedByService 면 스킵)
            if (DeviceStatusType::DisconnectedByService == m_repos->status().status) {
                SLOG_DEBUG("[KEEP ALIVE DEVICESESSION] Skip because of the DisconnectedByService status.");
                StartKeepAliveTimer();
                return;
            }
            
            //1.1: 이벤트 세션이 끊어졌는지만 확인하여 끊어진 상태면 다시 접속을 하도록 한다.
            if (!m_isConnecting
                && !m_isRunningEventSession
                && m_needToMonitorEventSessionStatus
                && m_repos->status().isGood()) {
                SLOG_DEBUG("[KEEP ALIVE DEVICESESSION] The event session is disconnected and restarted.");
                m_httpEventSession->Close();
                startEventSession();
            }

            //2. 다른 명령이 처리된 시간이 10초 이내인지 확인하여 10초가 넘는 경우에만 추가로 접속 확인.
            auto currentTime = std::chrono::steady_clock::now();
            auto elapsedSec = std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_lastReveivedTime).count();

            if (elapsedSec < expireTime) {
                SLOG_DEBUG("[KEEP ALIVE DEVICESESSION] The elapsed time is less than {} seconds.", expireTime);
 
                // Restart keep alive timer
                StartKeepAliveTimer();
                return;
            }

            //3. 접속 중인 경우 스킵한다.
            if (m_isConnecting) {
                SLOG_DEBUG("[KEEP ALIVE DEVICESESSION] connecting to device.");
                // Restart keep alive timer
                StartKeepAliveTimer();
                return;
            }

            SLOG_DEBUG("[KEEP ALIVE DEVICESESSION] Starts the connection status check.");

            //4. 정상 상태인경우 System CGI를 보내본다.
            if (m_repos->status().isGood()) {

                //P2P 모드이면 접속 타입(P2P or P2P_RELAY을 확인한다. RELAY모드이면 시간을 확인한다. 5분이 경과되었는지 확인하여 접속 해제를 한다.
                if (needToFinishConnection()) {

                    if (m_isCompletedConnection) {
                        SPDLOG_ERROR("[KEEP ALIVE DEVICESESSION] Proceed to disconnect by relay restriction.");
                        disconnect(DeviceStatusType::DisconnectedWithRestriction);
                    }
                    return;
                }

                auto systemDeviceInfoView = std::make_shared<SystemDeviceInfoView>(m_logPrefix, std::make_shared<DeviceConnectResponse>());
                asyncRequest(m_httpCommandSession,
                    systemDeviceInfoView,
                    [this](const ResponseBaseSharedPtr& response) {

                        if (response->isSuccess()) {
                            //뒤의 람다에서 정상처리 되도록 리턴함.
                            return;
                        }

                        //에러 발생
                        SLOG_ERROR("[KEEP ALIVE DEVICESESSION] DeviceInfo was unsuccessful. ErrorCode = {}({})", response->errorString(), response->errorCode);
                        //4.1 인증이 안되고 상태가 Unauthorized가 아니면 Unauthorized로 상태 변경 이벤트를 보낸다.
                        if (ErrorCode::UnAuthorized == response->errorCode) {

                            if (DeviceStatusType::Unauthorized != m_repos->status().status) {
                                SLOG_ERROR("[KEEP ALIVE DEVICESESSION] Change the device status to Unauthorized status");
                                if (m_isCompletedConnection) {
                                    //접속 해제 처리를 하고 Disconnect 이벤트를 보낸다.
                                    SLOG_ERROR("[KEEP ALIVE DEVICESESSION] Proceed to disconnect.");
                                    disconnect(DeviceStatusType::Unauthorized);
                                }
                            }
                            StartKeepAliveTimer(60); //인증 실패의 경우에는 60초 후에 재확인을 한다.

                        }else if(ErrorCode::AccountBlocked == response->errorCode) {
                            //장비 패스워드가 변경되어 발생될 수 도 있고 다른 클라이언트로 인해 발생될 수도 있으므로 DisconnectedWithError 상태변경한다.
                            SLOG_ERROR("[KEEP ALIVE DEVICESESSION] Change the device status to DisconnectWithError status");
                            if (m_isCompletedConnection) {
                                SLOG_ERROR("[KEEP ALIVE DEVICESESSION] Proceed to disconnect.");
                                disconnect(DeviceStatusType::DisconnectedWithError);
                            }
                            StartKeepAliveTimer(60); //AccountBlocked 의 경우에는 60초 후에 재확인을 한다.

                        }else {
                            //4.2 접속이 안되고 상태가 DisconnectedWithError가 아니면 DisconnectWithError로 상태 변경 이벤트를 보낸다.
                            if (DeviceStatusType::DisconnectedWithError != m_repos->status().status) {
                                SLOG_ERROR("[KEEP ALIVE DEVICESESSION] Change the device status to DisconnectWithError status");
                                if (m_isCompletedConnection) {
                                    SLOG_ERROR("[KEEP ALIVE DEVICESESSION] Proceed to disconnect.");
                                    disconnect(DeviceStatusType::DisconnectedWithError);
                                }

                            }
                            StartKeepAliveTimer();
                        }
                    },
                    [this]() {
                        //정상 완료되어 다음 처리.
                        SLOG_DEBUG("[KEEP ALIVE DEVICESESSION] DeviceInfo was successful.");
                        StartKeepAliveTimer();

                    }, AsyncRequestType::HTTPGET, true, nullptr);
            }
            else {
                //4.3 접속이 정상적이고 기존 상태가 DisconnectedWithError/Unauthorized/WithRestrictions 상태였으면 Connect를 시도하도록 한다.
                SLOG_INFO("[KEEP ALIVE DEVICESESSION] Try to recover a device connection, ip={}", m_connectionInfo.ip);
                DeviceConnectRequestSharedPtr request = std::make_shared<DeviceConnectRequest>();
                request->deviceID = m_deviceUUID;
                request->connectionInfo = m_connectionInfo;
                //DDNS 설정인 경우 P2P 접속 고려하여 ConnectedType을 DDNS로 변경하여 DDNS부터 다시 접속할 수 있도록 변경한다.
                if (ConnectionType::WisenetDDNS == request->connectionInfo.connectionType) {
                    request->connectionInfo.connectedType = ConnectedType::WisenetDDNS;
                }
                request->addDevice = false;
                DeviceConnect(request
                    , [this](const ResponseBaseSharedPtr& response) {

                        if (response->isSuccess()) {
                            SPDLOG_INFO("[KEEP ALIVE DEVICESESSION] Connected");

                            DeviceConnectResponseSharedPtr connectResponse = std::static_pointer_cast<DeviceConnectResponse>(response);

                            auto deviceStatusEvent = std::make_shared<DeviceStatusEvent>();
                            deviceStatusEvent->deviceID = m_deviceUUID;
                            deviceStatusEvent->device = connectResponse->device;
                            deviceStatusEvent->deviceStatus = connectResponse->deviceStatus;
                            deviceStatusEvent->deviceProfileInfo = connectResponse->deviceProfileInfo;
                            eventHandler(deviceStatusEvent);

                            StartKeepAliveTimer();
                        }else {

                            SLOG_ERROR("[KEEP ALIVE DEVICESESSION] Connect Response Error : {}({}) ", response->errorString(), response->errorCode);

                            if (ErrorCode::UnAuthorized == response->errorCode) {

                                if (DeviceStatusType::Unauthorized != m_repos->status().status) {
                                    SLOG_ERROR("[KEEP ALIVE DEVICESESSION] Change the device status to Unauthorized status");
                                    if (m_isCompletedConnection) {
                                        //접속 해제 처리를 하고 Disconnect 이벤트를 보낸다.
                                        SLOG_ERROR("[KEEP ALIVE DEVICESESSION] Proceed to disconnect.");
                                        disconnect(DeviceStatusType::Unauthorized);
                                    }
                                }
                                StartKeepAliveTimer(60);
                            }else if(ErrorCode::AccountBlocked == response->errorCode) {
                                //패스워드가 틀려있는 상태에서 주기적으로 체크하다보면 다른 클라이언트에서 접속하거나 하면 account block이 생기고
                                //이 때 DeviceStatusType을 변경하지 않도록 한다. 변경하면 DeviceStatusType이 체크 때마다 바뀔 수 있다.
                                StartKeepAliveTimer(60); //AccountBlocked 의 경우에는 60초 후에 재확인을 한다.
                            }else {
                                if (DeviceStatusType::DisconnectedWithError != m_repos->status().status) {
                                    SLOG_ERROR("[KEEP ALIVE DEVICESESSION] Change the device status to DisconnectedWithError status");
                                    if (m_isCompletedConnection) {
                                        //접속 해제 처리를 하고 Disconnect 이벤트를 보낸다.
                                        SLOG_ERROR("[KEEP ALIVE DEVICESESSION] Proceed to disconnect.");
                                        disconnect(DeviceStatusType::DisconnectedWithError);
                                    }
                                }
                                StartKeepAliveTimer();
                            }

                        }
                });
            }

        }));
    }));
}
void SunapiDeviceClientImpl::StopKeepAliveTimer()
{
    SLOG_INFO("[KEEP ALIVE DEVICESESSION] StopKeepAliveTimer start");
    asio::dispatch(m_strand, WeakCallback(shared_from_this(), [this]()
    {
        m_keepAliveTimer.cancel();

    }));
    SLOG_INFO("[KEEP ALIVE DEVICESESSION] StopKeepAliveTimer end");
}

bool SunapiDeviceClientImpl::needToFinishConnection()
{
    if (m_allowRelay) {
        // Relay 허용 옵션이므로 종료 처리 하지 않음.
        m_startRelayMode = 0;
        return false;
    }
    // Relay 모드인지 체크한다. 
    SIPPROXY_CONNECTION_TYPE type = m_easyConnection.GetConnectedType();

    if (SIPPROXY_CONNECTION_TYPE::eRelay != type) {
        // Relay 모드가 아님.
        m_startRelayMode = 0;
        return false;
    }

    if (0 == m_startRelayMode) {
        // 처음으로 Relay모드로 들어옴. 
        m_startRelayMode = Wisenet::Common::currentUtcSecs();
        return false;
    }

    long long elapsed = Wisenet::Common::currentUtcSecs() - m_startRelayMode;

    if (elapsed < 300) {
        // 5분이 경과되지 않았음.
        return false;
    }

    //Relay Mode로 5분이 경과되어 접속 종료 필요.
    return true;
}

}
}



