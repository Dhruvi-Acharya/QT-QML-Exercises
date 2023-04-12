#include "SunapiDeviceClientImpl.h"

#include "iPolisDDNS.h"

#include "../RtspClient/RtspSession/RtspSocket.h"

#include "VersionManager.h"
#include "S1DDNS.h"
#include "S1SerialConverter.h"
#include <QDebug>

namespace Wisenet
{
namespace Device
{

static Wisenet::ErrorCode CloudErrorCodeToErrorCode(const Wisenet::Library::CloudErrorCode& errorCode)
{
    switch(errorCode){
    case Wisenet::Library::CloudErrorCode::NoError:
        return Wisenet::ErrorCode::NoError;
    case Wisenet::Library::CloudErrorCode::NetworkError:
        return Wisenet::ErrorCode::CloudConnectError;
    case Wisenet::Library::CloudErrorCode::UnAuthorizedDevice:
        return Wisenet::ErrorCode::UnAuthorized;
    case Wisenet::Library::CloudErrorCode::InvalidValue:
        return Wisenet::ErrorCode::CloudInvalidServerValue;
    case Wisenet::Library::CloudErrorCode::AddingUserError:
        return Wisenet::ErrorCode::CloudAddingUserError;
    case Wisenet::Library::CloudErrorCode::AddingDeviceError:
        return Wisenet::ErrorCode::CloudAddingDeviceError;
    }

    return Wisenet::ErrorCode::CloudConnectError;
}

void SunapiDeviceClientImpl::setHttpConnection(const std::shared_ptr<HttpSession>& httpSession,
                                               const Wisenet::Device::DeviceConnectInfo& connInfo,
                                               const int connectionTimeoutSec)
{
    m_logPrefix = "[SunapiClient]("+m_deviceUUID+")("+connInfo.host+") ";
    SLOG_DEBUG("setHttpConnection ip:{}, isSSL:{}, port:{}, sslPort:{}, id:{}, mac:{}",
                 connInfo.host, connInfo.isSSL, connInfo.port, connInfo.sslPort, connInfo.user, connInfo.mac);

    auto port = connInfo.isSSL ? connInfo.sslPort : connInfo.port;
    httpSession->SetConnection(connInfo.ip,
                               port,
                               connInfo.user,
                               connInfo.password,
                               connInfo.mac,
                               connInfo.isSSL,
                               connectionTimeoutSec);
}

void SunapiDeviceClientImpl::DeviceConnect(DeviceConnectRequestSharedPtr const& request,
                                           ResponseBaseHandler const& responseHandler)
{
    m_deviceRegistrationStartTime = QDateTime::currentMSecsSinceEpoch();

    asio::post(m_strand, WeakCallback(shared_from_this(),
        [this, request, responseHandler]()
        {
            SLOG_INFO("DeviceConnect() :: (uuid={} host={}:{} isSSL:{}  mediaProtocolType:{} mac:{})",
                request->deviceID, request->connectionInfo.host, request->connectionInfo.port, request->connectionInfo.isSSL, request->connectionInfo.mediaProtocolType,request->connectionInfo.mac);

            if (m_isConnecting) {
                SLOG_INFO("DeviceConnect() :: (uuid={}, host={}:{} isSSL:{}) Failed because connection is in progress.",
                    request->deviceID, request->connectionInfo.host, request->connectionInfo.port, request->connectionInfo.isSSL);
                //현재 접속 진행 중이므로 예외처리 한다.
                auto response = std::make_shared<ResponseBase>();
                response->errorCode = Wisenet::ErrorCode::InvalidRequest;
                if (responseHandler) {
                    responseHandler(response);
                }
                return;
            }

            m_isConnecting = true;
            m_isFwupdating = false;
            if((DeviceStatusType::DisconnectedByService == m_repos->status().status)
                    || (DeviceStatusType::Connected == m_repos->status().status)){
                m_repos->setDisconnected(DeviceStatusType::DisconnectedWithError); // DisconnectedWithError로 바꾸어 놓아야 Retry처리를 한다.

            }

            if (ConnectionType::WisenetDDNS == request->connectionInfo.connectionType) {

                if (EasyConnection::isValidProductP2PID(request->connectionInfo.host)) {
                    request->connectionInfo.connectedType = ConnectedType::P2P;
                    request->connectionInfo.productP2PID = request->connectionInfo.host;
                }

                if (ConnectedType::P2P == request->connectionInfo.connectedType 
                    || ConnectedType::P2P_RELAY == request->connectionInfo.connectedType ) {

                    DeviceConnectViaP2P(request, responseHandler, 1);
                }
                else {
                    DeviceConnectViaDDNS(request, responseHandler, 1);
                }
            }
            else if (ConnectionType::S1_DDNS == request->connectionInfo.connectionType)
                DeviceConnectViaS1DDNS(request, responseHandler, 1);
            else if (ConnectionType::S1_SIP == request->connectionInfo.connectionType)
            {
                // 마지막에 DDNS로 접속을 성공했을 경우 DDNS로 접속함.
                if(ConnectedType::S1_DDNS == request->connectionInfo.connectedType)
                    DeviceConnectViaS1DDNS(request, responseHandler, 1);
                else // 최초 등록 시 == ConnectedType이 StaticIPv4 (default값)
                    DeviceConnectViaS1SIP(request, responseHandler, 1);
            }
            else {
                DeviceConnectViaURL(request, responseHandler);
            }

        }));
}

void SunapiDeviceClientImpl::DeviceConnectViaURL(DeviceConnectRequestSharedPtr const& request,
                   ResponseBaseHandler const& responseHandler)
{
    asio::post(m_strand, WeakCallback(shared_from_this(),
                                      [this, request, responseHandler]()
    {
        m_connectionInfo = request->connectionInfo;

        bool bRet = true;
        if(m_connectionInfo.connectionType == ConnectionType::StaticIPv4){
            m_connectionInfo.ip = m_connectionInfo.host;
            m_connectionInfo.connectedType =ConnectedType::StaticIPv4;
        }
        else if (m_connectionInfo.connectionType == ConnectionType::StaticIPv6) {
            
            if (m_connectionInfo.host.at(0) != '[')
                m_connectionInfo.ip = "[";
            m_connectionInfo.ip += m_connectionInfo.host;
            if (m_connectionInfo.host.at(m_connectionInfo.host.length()-1) != ']')
                m_connectionInfo.ip += "]";

            m_connectionInfo.connectedType = ConnectedType::StaticIPv6;
        }
        else if(m_connectionInfo.connectionType == ConnectionType::HttpUrl){
            IpolisDDNS iPolis(m_deviceIoc);
            std::string ip;
            bRet = iPolis.GetUrlAddress(m_connectionInfo.host, ip, m_connectionInfo.isSSL);
            m_connectionInfo.connectedType =ConnectedType::HttpUrl;
            if(bRet)
                m_connectionInfo.ip = ip;
        }else{
            SLOG_ERROR("Not supported Connection Type in this function(DeviceConnectViaURL). type={}", m_connectionInfo.connectionType);
            bRet = false;
        }

        if(bRet){
            setHttpConnection(m_httpCommandSession, m_connectionInfo);
            m_httpCommandSession->SetCheckDeviceCert(true);
            getSystemDeviceInfo(request, responseHandler, nullptr);
        }
        else{
            auto systemDeviceInfoView = std::make_shared<SystemDeviceInfoView>(m_logPrefix, std::make_shared<DeviceConnectResponse>());
            systemDeviceInfoView->ResponseBase()->errorCode = Wisenet::ErrorCode::DDNSError;
            if(responseHandler) {
                responseHandler(systemDeviceInfoView->ResponseBase());
            }
            m_isConnecting = false;
        }
    }));
}

void SunapiDeviceClientImpl::DeviceConnectViaS1DDNS(DeviceConnectRequestSharedPtr const& request,
                                                    ResponseBaseHandler const& responseHandler,
                                                    int tryToConnectCount)
{
    asio::post(m_strand, WeakCallback(shared_from_this(),
                                      [this, request, responseHandler, tryToConnectCount]()
    {
        SLOG_INFO("S1DDNS - Start");
        if (tryToConnectCount == 1) {
            //처음 접속인 경우에만 request에서 복사하여 사용.
            m_connectionInfo = request->connectionInfo;
        }

        S1DDNS s1ddns(m_deviceIoc);

        std::string peerAddress;
        unsigned short http = 0;
        unsigned short https = 0;
        unsigned short rtsp = 0;


        QString hostName = QString::fromStdString(request->connectionInfo.host);
        if(hostName.length() != 12)
        {
            hostName = Wisenet::Device::ConvertSerialToMAC(hostName);
        }
        m_connectionInfo.mac = hostName.toStdString();

        if(s1ddns.GetPeerAddress(hostName.toStdString(), peerAddress, http, https, rtsp))
        {
            SLOG_INFO( "S1DDNS Get connection info success - peerAddress:{}, http:{}, https:{}, rtsp:{}", peerAddress, http, https, rtsp);
            m_connectionInfo.connectedType = ConnectedType::S1_DDNS;
            m_connectionInfo.ip = peerAddress;
            m_connectionInfo.port = http;
            m_connectionInfo.sslPort = https;
            m_connectionInfo.rtspPort = rtsp;

            setHttpConnection(m_httpCommandSession, m_connectionInfo);
            getSystemDeviceInfo(request, responseHandler,
                                [this, request, responseHandler, tryToConnectCount](Wisenet::ErrorCode& errorCode)
            {
                if(tryToConnectCount < 2)
                {
                    DeviceConnectViaS1SIP(request, responseHandler, 2);
                }
                else
                {
                    auto systemDeviceInfoView = std::make_shared<SystemDeviceInfoView>(m_logPrefix, std::make_shared<DeviceConnectResponse>());
                    systemDeviceInfoView->ResponseBase()->errorCode = errorCode;
                    if(responseHandler) {
                        responseHandler(systemDeviceInfoView->ResponseBase());
                    }
                    m_isConnecting = false;
                }
            });
        }
        else
        {
            if(tryToConnectCount < 2)
            {
                DeviceConnectViaS1SIP(request, responseHandler, 2);
            }
            else
            {
                // 첫 등록 요청이 SIP이기 때문에 P2P 에러 코드
                auto systemDeviceInfoView = std::make_shared<SystemDeviceInfoView>(m_logPrefix, std::make_shared<DeviceConnectResponse>());
                systemDeviceInfoView->ResponseBase()->errorCode = Wisenet::ErrorCode::P2PConnectError;
                if(responseHandler) {
                    responseHandler(systemDeviceInfoView->ResponseBase());
                }
                m_isConnecting = false;
            }
        }
    }));
}

void SunapiDeviceClientImpl::DeviceConnectViaS1SIP(DeviceConnectRequestSharedPtr const& request,
                                                   ResponseBaseHandler const& responseHandler,
                                                   int tryToConnectCount)
{
    asio::post(m_strand, WeakCallback(shared_from_this(),
                                      [this, request, responseHandler, tryToConnectCount]()
    {
        SLOG_INFO("S1 SIP - Start");
        if (tryToConnectCount == 1) {
            //처음 접속인 경우에만 request에서 복사하여 사용.
            m_connectionInfo = request->connectionInfo;
        }

        std::string peerAddress;
        unsigned short http = 0;
        unsigned short https = 0;
        short connectionType = -1;

        QString hostName = QString::fromStdString(request->connectionInfo.host);
        if(hostName.length() != 12)
        {
            hostName = Wisenet::Device::ConvertSerialToMAC(hostName);
        }
        m_connectionInfo.mac = hostName.toStdString();

        if(m_s1sip == nullptr)
            m_s1sip = std::make_shared<S1Sip>(m_deviceIoc, m_s1DaemonPort);

        // SIP Request
        m_s1sip->GetPort(m_connectionInfo.mac, http, https, connectionType);

        if(connectionType != -1)
        {
            SLOG_INFO("S1 SIP Connection info http:{}, https:{}, ConnectionType:{}", http, https, connectionType);
            m_connectionInfo.connectedType = connectionType == 0 ? ConnectedType::S1_SIP : ConnectedType::S1_SIP_RELAY;
            m_connectionInfo.ip = "127.0.0.1";
            m_connectionInfo.port = http;
            m_connectionInfo.sslPort = https;

            setHttpConnection(m_httpCommandSession, m_connectionInfo);
            getSystemDeviceInfo(request, responseHandler,
                                [this, request, responseHandler, tryToConnectCount](Wisenet::ErrorCode& errorCode)
            {
                if(tryToConnectCount < 2)
                {
                    DeviceConnectViaS1DDNS(request, responseHandler, 2);
                }
                else
                {
                    auto systemDeviceInfoView = std::make_shared<SystemDeviceInfoView>(m_logPrefix, std::make_shared<DeviceConnectResponse>());
                    systemDeviceInfoView->ResponseBase()->errorCode = errorCode;
                    if(responseHandler) {
                        responseHandler(systemDeviceInfoView->ResponseBase());
                    }
                    m_isConnecting = false;
                }
            });
        }
        else
        {
            if(tryToConnectCount < 2)
            {
                DeviceConnectViaS1DDNS(request, responseHandler, 2);
            }
            else
            {
                // 첫 등록 요청이 DDNS이기 때문에 DDNS 에러 코드
                auto systemDeviceInfoView = std::make_shared<SystemDeviceInfoView>(m_logPrefix, std::make_shared<DeviceConnectResponse>());
                systemDeviceInfoView->ResponseBase()->errorCode = Wisenet::ErrorCode::DDNSError;
                if(responseHandler) {
                    responseHandler(systemDeviceInfoView->ResponseBase());
                }
                m_isConnecting = false;
            }
        }
    }));
}

void SunapiDeviceClientImpl::DeviceTryConnectingS1DDNS(DeviceRequestBaseSharedPtr const& request,
                                                       ResponseBaseHandler const& responseHandler)
{
    asio::post(m_strand, WeakCallback(shared_from_this(),
                                      [this, request, responseHandler]()
    {
        S1DDNS s1ddns(m_deviceIoc);

        std::string peerAddress;

        unsigned short http = 0;
        unsigned short https = 0;
        unsigned short rtsp = 0;

        auto responseBase = std::make_shared<ResponseBase>();
        if (s1ddns.GetPeerAddress(m_connectionInfo.mac, peerAddress, http, https, rtsp))
        {
            SLOG_INFO("DeviceTryConnectingS1DDNS() - peerAddress:{}, http:{}, https:{}, rtsp:{}", peerAddress, http, https, rtsp);
        }
        else
            responseBase->errorCode = Wisenet::ErrorCode::DDNSError;

        if(responseHandler) {
            responseHandler(responseBase);
        }
    }));
}

void SunapiDeviceClientImpl::DeviceConnectViaDDNS(DeviceConnectRequestSharedPtr const& request,
                   ResponseBaseHandler const& responseHandler, int tryToConnectCount)
{
    asio::post(m_strand, WeakCallback(shared_from_this(),
                                      [this, request, responseHandler, tryToConnectCount]()
    {
        SLOG_INFO("WisenetDDNS - Start");
        if (tryToConnectCount == 1) {
            //처음 접속인 경우에만 request에서 복사하여 사용.
            m_connectionInfo = request->connectionInfo;
        }

        IpolisDDNS iPolis(m_deviceIoc);
        std::string productP2PID("");
        unsigned int status = 0;
        if((m_connectionInfo.connectionType != ConnectionType::WisenetDDNS)
                || !iPolis.GetDDNSAddress(m_connectionInfo.host, m_connectionInfo.ip, m_connectionInfo.port, productP2PID, status)){
            SLOG_ERROR("WisenetDDNS - connectionType isn't WisenetDDNS or can't connect to Wisenet DDNS.");
            //DDNS 접속 타입이 아니고 DDNS 실패인 경우 실패 처리.
            auto systemDeviceInfoView = std::make_shared<SystemDeviceInfoView>(m_logPrefix, std::make_shared<DeviceConnectResponse>());
            if(400 == status){
                systemDeviceInfoView->ResponseBase()->errorCode = Wisenet::ErrorCode::NotExistID;
            }else{
                systemDeviceInfoView->ResponseBase()->errorCode = Wisenet::ErrorCode::DDNSError;
            }

            if(responseHandler) {
                responseHandler(systemDeviceInfoView->ResponseBase());
            }
            m_isConnecting = false;
            return;
        }

        SLOG_INFO("WisenetDDNS - Connection info ip={}, port={}, productP2PID={}", m_connectionInfo.ip, m_connectionInfo.port, productP2PID);

        m_connectionInfo.connectedType = ConnectedType::WisenetDDNS;
        m_connectionInfo.isSSL = false; //DDNS로 붙는 경우는 무조건 HTTP 로 시작해야 한다.
        setHttpConnection(m_httpCommandSession, m_connectionInfo);

        if(!EasyConnection::isValidProductP2PID(productP2PID)){
            // productID가 없으면 P2P를 지원하지 않으므로 접속 실패시 P2P 접속을 하지 않도록 함.
            SLOG_INFO("WisenetDDNS - Only DDNS Mode");
            m_httpCommandSession->SetCheckDeviceCert(true);
            getSystemDeviceInfo(request, responseHandler, nullptr);

        }else{
            SLOG_INFO("WisenetDDNS - Both DDNS & P2P Mode");
            m_connectionInfo.productP2PID = productP2PID;

            getSystemDeviceInfo(request, responseHandler,[this, request, responseHandler, tryToConnectCount](Wisenet::ErrorCode& errorCode){

                SLOG_INFO("WisenetDDNS - Not connected a device via DDNS.");
                //접속 실패시 실행되는 부분임.

                if(tryToConnectCount < 2 && EasyConnection::isValidProductP2PID(m_connectionInfo.productP2PID)){
                    // 첫번째 시도에서 실패한 경우이고 productP2PID가 존재하므로 추가로 P2P 접속 시도.
                    SLOG_INFO("WisenetDDNS - Try to connect P2P");
                    DeviceConnectViaP2P(request, responseHandler,2);

                }else{
                    // DDNS, P2P 모두 실패한 경우 또는 Product ID가 없는 경우이니 실패 처리.
                    SLOG_INFO("WisenetDDNS - Failed to connect a device. ");
                    auto systemDeviceInfoView = std::make_shared<SystemDeviceInfoView>(m_logPrefix, std::make_shared<DeviceConnectResponse>());
                    systemDeviceInfoView->ResponseBase()->errorCode = errorCode;
                    if(responseHandler) {
                        responseHandler(systemDeviceInfoView->ResponseBase());
                    }
                    m_isConnecting = false;
                }
            });
        }

    }));
}
void SunapiDeviceClientImpl::DeviceConnectViaP2P(DeviceConnectRequestSharedPtr const& request,
                   ResponseBaseHandler const& responseHandler, int tryToConnectCount)
{
    asio::post(m_strand, WeakCallback(shared_from_this(),
                                      [this, request, responseHandler, tryToConnectCount]()
    {
        SLOG_INFO("Start WisenetP2P");
        if (tryToConnectCount == 1) {
            //처음 접속인 경우에만 request에서 복사하여 사용.
            m_connectionInfo = request->connectionInfo;
        }
        
        if((m_connectionInfo.connectionType != ConnectionType::WisenetDDNS)
                || !EasyConnection::isValidProductP2PID(m_connectionInfo.productP2PID)){

            SLOG_ERROR("WisenetDDNS - connectionType isn't WisenetDDNS or can't connect to Wisenet DDNS or productP2PID is invaild.");
            //실패 처리.
            auto systemDeviceInfoView = std::make_shared<SystemDeviceInfoView>(m_logPrefix, std::make_shared<DeviceConnectResponse>());
            systemDeviceInfoView->ResponseBase()->errorCode = Wisenet::ErrorCode::InvalidRequest;
            if(responseHandler) {
                responseHandler(systemDeviceInfoView->ResponseBase());
            }
            m_isConnecting = false;            
            return;
        }

        SLOG_INFO("WisenetP2P - Connection info ip={}, port={}, productP2PID={}", m_connectionInfo.ip, m_connectionInfo.port, m_connectionInfo.productP2PID);
        m_connectionInfo.connectedType = ConnectedType::P2P;

        //Try to query a P2P Connection Information on WisenetCloud.
        getP2PInfoFromCloud(request,responseHandler, m_connectionInfo.productP2PID, tryToConnectCount);

    }));
}

void SunapiDeviceClientImpl::getP2PInfoFromCloud(const DeviceConnectRequestSharedPtr &request, const ResponseBaseHandler &responseHandler, const std::string& productP2PID, int tryToConnectCount)
{
    Wisenet::Library::CloudDevice device;
    device.productP2PID = productP2PID;
    device.userName = m_connectionInfo.user;
    device.password = m_connectionInfo.password;

    SLOG_INFO("WisenetP2P - Try to get P2PInfo from Wisenet Cloud");

    m_cloudService->AsyncGetP2PInfo(device,
                                    WeakCallback(shared_from_this(),[this, request, responseHandler, productP2PID, tryToConnectCount](const Wisenet::Library::CloudErrorCode& errorCode, const Wisenet::Library::CloudP2PInfo& p2pInfo){

        asio::post(m_strand, WeakCallback(shared_from_this(),
                                          [this, request, responseHandler, productP2PID, tryToConnectCount, errorCode, p2pInfo]()
        {


            if(Wisenet::Library::CloudErrorCode::NoError != errorCode){
                SLOG_INFO("WisenetP2P - Issued a error in Wisenet Cloude. errorCode = {}", errorCode);
                if(tryToConnectCount < 2){
                    SLOG_INFO("WisenetP2P - Try to connect a device vis Wisenet DDNS.");
                    DeviceConnectViaDDNS(request, responseHandler,2);
                }else{
                    SLOG_INFO("WisenetP2P - Failed to connect a device.");
                    //실패 처리.
                    auto systemDeviceInfoView = std::make_shared<SystemDeviceInfoView>(m_logPrefix, std::make_shared<DeviceConnectResponse>());
                    systemDeviceInfoView->ResponseBase()->errorCode = CloudErrorCodeToErrorCode(errorCode);
                    if(responseHandler) {
                        responseHandler(systemDeviceInfoView->ResponseBase());
                    }
                    m_isConnecting = false;
                }
                return;
            }

            //Start EasyConnection
            SLOG_INFO("WisenetP2P - Started to connect a device via P2P");
            m_connectionInfo.productP2PID = productP2PID; //P2P서버 등록이 완료되었으므로 DB에 기록하기 위해 productP2PID를 복사한다.

            // EasyConnection이 정상적으로 연결이 안되거나, 릴레이 허가 옵션이 아니고 장치 추가 중이 아닌 경우 RELAY모드에서는 접속을 중단한다. 
            if(!m_easyConnection.GetIpAndPort(m_connectionInfo, p2pInfo)
                || (!m_allowRelay && !request->addDevice && (ConnectedType::P2P_RELAY == m_connectionInfo.connectedType))){

                //easyConnection을 Stop한다.
                m_easyConnection.StopP2PService();

                if(tryToConnectCount < 2 && !EasyConnection::isValidProductP2PID(m_connectionInfo.host)){
                    DeviceConnectViaDDNS(request, responseHandler,2);
                }else{
                    //실패 처리.
                    auto systemDeviceInfoView = std::make_shared<SystemDeviceInfoView>(m_logPrefix, std::make_shared<DeviceConnectResponse>());
                    systemDeviceInfoView->ResponseBase()->errorCode = Wisenet::ErrorCode::P2PConnectError;
                    if(responseHandler) {
                        responseHandler(systemDeviceInfoView->ResponseBase());
                    }
                    m_isConnecting = false;
                }
                return;
            }

            setHttpConnection(m_httpCommandSession, m_connectionInfo);

            getSystemDeviceInfo(request, responseHandler,
                                WeakCallback(shared_from_this(),[this, request, responseHandler, tryToConnectCount](Wisenet::ErrorCode& errorCode){

                //접속 실패시 실행되는 부분임.
                if(tryToConnectCount < 2 && !EasyConnection::isValidProductP2PID(m_connectionInfo.host)){
                    SLOG_INFO("WisenetP2P - Try to connect a device vis Wisenet DDNS.");
                    DeviceConnectViaDDNS(request, responseHandler,2);
                }else{
                    SLOG_INFO("WisenetP2P - Failed to connect a device.");
                    auto systemDeviceInfoView = std::make_shared<SystemDeviceInfoView>(m_logPrefix, std::make_shared<DeviceConnectResponse>());
                    systemDeviceInfoView->ResponseBase()->errorCode = errorCode;
                    if(responseHandler) {
                        responseHandler(systemDeviceInfoView->ResponseBase());
                    }
                    m_isConnecting = false;
                }

            }));

        }));

    }));
}

// Get Device information
void SunapiDeviceClientImpl::getSystemDeviceInfo(DeviceConnectRequestSharedPtr const& request,
                                                 const ResponseBaseHandler& responseHandler,
                                                 ProcessConnectFailFunc processConnectFailFunc)
{
    auto systemDeviceInfoView = std::make_shared<SystemDeviceInfoView>(m_logPrefix, std::make_shared<DeviceConnectResponse>());
    asyncRequest(m_httpCommandSession,
                 systemDeviceInfoView,
                [this, responseHandler](const ResponseBaseSharedPtr& responseBase) {

                    if (responseHandler) {
                        responseHandler(responseBase);
                    }
                    m_isConnecting = false;

                },
                [this, systemDeviceInfoView, request, responseHandler](){

        auto oc = std::make_shared<DeviceObjectComposer>();
        oc->setSystemDeviceInfoView(systemDeviceInfoView->parseResult);
        //oc->setDeviceCertificateResult(systemDeviceInfoView->parseResult.connectedMacAddress, m_httpCommandSession->GetDeviceCertMac(), m_httpCommandSession->GetDeviceCertResult());

        //SPDLOG_DEBUG("SunapiDeviceClientImpl::getSystemDeviceInfo cert mac:{} result:{}",
        //             m_httpCommandSession->GetDeviceCertMac(), m_httpCommandSession->GetDeviceCertResult());

        if (request->addDevice &&
                systemDeviceInfoView->parseResult.enumDeviceType != DeviceType::SunapiRecorder) {

            // create low profile for camera
            SLOG_DEBUG("Try to create low video profile");
            createVideoProfile(oc, responseHandler);
            return;
        }

        getNetworkInterface(oc, responseHandler);

    }, AsyncRequestType::HTTPGET, true, processConnectFailFunc);
}

std::string capitalizeString(std::string s)
{
    transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c){ return toupper(c); });
    return s;
}

// Get Network interface
void SunapiDeviceClientImpl::getNetworkInterface(const DeviceObjectComposerPtr &oc, const ResponseBaseHandler &responseHandler)
{
    auto networkInterfaceView = std::make_shared<NetworkInterfaceView>(m_logPrefix);
    asyncRequest(m_httpCommandSession,
                 networkInterfaceView,
                [this, responseHandler](const ResponseBaseSharedPtr& responseBase) {

                    if (responseHandler) {
                        responseHandler(responseBase);
                    }
                    m_isConnecting = false;
    },
    [this, oc, networkInterfaceView, responseHandler](){
        oc->setNetworkInterfaceView(networkInterfaceView->parseResult);

        //std::string mac = networkInterfaceView->parseResult.networkInterfaces.begin()->first;
         auto certMac = capitalizeString(m_httpCommandSession->GetDeviceCertMac());
         bool bSameMac = false;

        for(auto &item : networkInterfaceView->parseResult.networkInterfaces){
            std::string tempMac = item.first;
            tempMac.erase(remove(tempMac.begin(), tempMac.end(), ':'), tempMac.end());
            if(certMac == capitalizeString(tempMac)){
                bSameMac = true;
                break;
            }
        }

        oc->setDeviceCertificateResult(bSameMac, m_httpCommandSession->GetDeviceCertResult());
        SPDLOG_DEBUG("SunapiDeviceClientImpl::getNetworkInterface cert mac:{}/{} result:{}",
                             bSameMac, m_httpCommandSession->GetDeviceCertMac(), m_httpCommandSession->GetDeviceCertResult());
        getSystemDate(oc, responseHandler);

    }, AsyncRequestType::HTTPGET, true);
}

// Get Device date
void SunapiDeviceClientImpl::getSystemDate(const DeviceObjectComposerPtr& oc,
                                           const ResponseBaseHandler& responseHandler)
{
    auto systemDataView = std::make_shared<SystemDateView>(m_logPrefix);
    asyncRequest(m_httpCommandSession,
                 systemDataView,
                [this, responseHandler](const ResponseBaseSharedPtr& responseBase) {

                    if (responseHandler) {
                        responseHandler(responseBase);
                    }
                    m_isConnecting = false;
                }, 
                [this, oc, systemDataView, responseHandler](){

        oc->setSystemDateView(systemDataView->parseResult);

        if (ConnectedType::P2P == m_connectionInfo.connectedType 
            || ConnectedType::P2P_RELAY == m_connectionInfo.connectedType) {
            // P2P 연결인 경우 RTSP 포트는 LOCAL PORT로 연결되어야 하므로 getAttributes로 이동.
            getAttributes(oc, responseHandler);

        }else {

            getNetworkRtspPort(oc, responseHandler);
        }

    }, AsyncRequestType::HTTPGET, true);
}

// Get Device RTSP Port
void SunapiDeviceClientImpl::getNetworkRtspPort(const DeviceObjectComposerPtr& oc,
                                                const ResponseBaseHandler& responseHandler)
{
    auto networkRtspView = std::make_shared<NetworkRtspView>(m_logPrefix);
    asyncRequest(m_httpCommandSession,
                 networkRtspView,
                [this, responseHandler](const ResponseBaseSharedPtr& responseBase) {

                    if (responseHandler) {
                        responseHandler(responseBase);
                    }
                    m_isConnecting = false;
                },
                [this, oc, networkRtspView, responseHandler](){

        
                m_connectionInfo.rtspPort = networkRtspView->parseResult.rtspPort;

        getAttributes(oc, responseHandler);
    }, AsyncRequestType::HTTPGET, true);
}

// Get Device Attributes
void SunapiDeviceClientImpl::getAttributes(const DeviceObjectComposerPtr& oc,
                                           const ResponseBaseHandler& responseHandler)
{
    auto attributesParser = std::make_shared<AttributesParser>(m_logPrefix, std::make_shared<DeviceConnectResponse>());
    asyncRequest(m_httpCommandSession,
                 attributesParser,
                [this, responseHandler](const ResponseBaseSharedPtr& responseBase) {

                    if (responseHandler) {
                        responseHandler(responseBase);
                    }
                    m_isConnecting = false;
                },
                 [this, oc, attributesParser, responseHandler](){

        oc->setAttributes(attributesParser->parseResult);

        // 등록 과정이라 mediaProtocol Type이 결정되지 않았으면 결정한다.
        if (MediaProtocolType::UNKNOWN == m_connectionInfo.mediaProtocolType) {

            if (ConnectedType::WisenetDDNS == m_connectionInfo.connectedType) {
                // RTSP 포트로 접속을 시도하고 결정한다.
                checkRtspPort(oc, responseHandler);
            }
            else if(ConnectedType::S1_DDNS == m_connectionInfo.connectedType || ConnectedType::S1_SIP == m_connectionInfo.connectedType)
            {
                m_connectionInfo.mediaProtocolType = MediaProtocolType::RTSP_HTTP;
                getMediaVideoSource(oc, responseHandler);
            }
            else {
                m_connectionInfo.mediaProtocolType = MediaProtocolType::RTP_RTSP;
                getMediaVideoSource(oc, responseHandler);
            }
        }
        else {
            getMediaVideoSource(oc, responseHandler);
        }        
    }, AsyncRequestType::HTTPGET, true);
}

void SunapiDeviceClientImpl::checkRtspPort(const DeviceObjectComposerPtr& oc, ResponseBaseHandler const& responseHandler)
{
    asio::post(m_strand, WeakCallback(shared_from_this(),
        [this, oc, responseHandler]()
        {
            SPDLOG_DEBUG("checkRtspPort() : Start.");
            // 성공하면 mediaProtocolType은 RTP_RTSP 이고, 실패하면 mediaProtocolType은 HTTP로 설정한다.
            std::string rtspUrl = "rtsp://" + m_connectionInfo.ip + ":" + boost::lexical_cast<std::string>(m_connectionInfo.rtspPort);

            //NVR
            if (oc->isRecorder()) {
                rtspUrl += "/LiveChannel/0/media.smp";
            }
            else {
                if (oc->attributes().systemAttribute.maxChannel > 1) {
                    rtspUrl += "/H.264/media.smp";
                }
                else {
                    rtspUrl += "/0/H.264/media.smp";
                }
            }
            
            std::shared_ptr<Wisenet::Rtsp::RtspSocket> rtspSocket = std::make_shared<Wisenet::Rtsp::RtspSocket>(m_deviceIoc, rtspUrl, m_connectionInfo.user, m_connectionInfo.password);

            rtspSocket->SetEventCallback([this](const Wisenet::Rtsp::RtspEventSharedPtr& e) {

                SPDLOG_DEBUG("checkRtspPort() : Event Callback.");
            });

            rtspSocket->Initialize(10, m_connectionInfo.rtspPort, Wisenet::Rtsp::TransportMethod::RTP_RTSP);

            std::string bindIP("");
            rtspSocket->SetBindIp(bindIP);
            
            std::string optionCommand = "OPTIONS";
            std::string contents("");
            rtspSocket->Write(optionCommand, rtspUrl, contents, WeakCallback(shared_from_this(), 
                [this, oc, responseHandler, rtspSocket](const Wisenet::Rtsp::RtspSocketResponsePtr& response) {

                SPDLOG_DEBUG("checkRtspPort() : Response of OPTION");

                asio::post(m_strand, WeakCallback(shared_from_this(),
                    [this, oc, response, responseHandler, rtspSocket]()
                {
                        SPDLOG_DEBUG("checkRtspPort() : response {}",response->ErrorDetails);

                        if (response->IsSuccess) {
                            m_connectionInfo.mediaProtocolType = MediaProtocolType::RTP_RTSP;
                            SPDLOG_INFO("checkRtspPort() : Changed mediProtocolType to RTP_RTSP");
                        }
                        else {
                            m_connectionInfo.mediaProtocolType = MediaProtocolType::RTSP_HTTP;
                            SPDLOG_INFO("checkRtspPort() : Changed mediProtocolType to RTSP_HTTP");
                        }

                        rtspSocket->Close();
                        getMediaVideoSource(oc, responseHandler);

                }));
            }));

        }));
}
// Get Video Source
void SunapiDeviceClientImpl::getMediaVideoSource(const DeviceObjectComposerPtr& oc,
                                                 const ResponseBaseHandler& responseHandler)
{
    auto mediaVideoSourceView = std::make_shared<MediaVideoSourceView>(m_logPrefix);
    asyncRequest(m_httpCommandSession,
                 mediaVideoSourceView,
                [this, responseHandler](const ResponseBaseSharedPtr& responseBase) {

                    if (responseHandler) {
                        responseHandler(responseBase);
                    }
                    m_isConnecting = false;
                },
                [this, oc, mediaVideoSourceView, responseHandler](){

        oc->setMediaVideoSourceView(mediaVideoSourceView->parseResult);
        getMediaVideoProfile(oc, responseHandler);

    }, AsyncRequestType::HTTPGET, true);
}

// Get Video profile
void SunapiDeviceClientImpl::getMediaVideoProfile(const DeviceObjectComposerPtr& oc,
                                                  const ResponseBaseHandler& responseHandler)
{
    auto mediaVideoProfileView = std::make_shared<MediaVideoProfileView>(m_logPrefix,std::make_shared<DeviceConnectResponse>());
    asyncRequest(m_httpCommandSession,
                 mediaVideoProfileView,
                [this, responseHandler](const ResponseBaseSharedPtr& responseBase) {

                    if (responseHandler) {
                        responseHandler(responseBase);
                    }
                    m_isConnecting = false;
                },
                 [this, oc, mediaVideoProfileView, responseHandler](){

        oc->setMediaVideoProfileView(mediaVideoProfileView->parseResult);
        getMediaVideoProfilePolicy(oc, responseHandler);

    }, AsyncRequestType::HTTPGET,true);
}

// Get Video profile policy
void SunapiDeviceClientImpl::getMediaVideoProfilePolicy(const DeviceObjectComposerPtr& oc,
                                                        const ResponseBaseHandler& responseHandler)
{
    auto mediaVideoProfilePolicyView =
            std::make_shared<MediaVideoProfilePolicyView>(m_logPrefix,
                                                          std::make_shared<DeviceConnectResponse>());
    asyncRequest(m_httpCommandSession,
                 mediaVideoProfilePolicyView,
                [this, responseHandler](const ResponseBaseSharedPtr& responseBase) {

                    if (responseHandler) {
                        responseHandler(responseBase);
                    }
                    m_isConnecting = false;
                },
                 [this, oc, mediaVideoProfilePolicyView, responseHandler](){

        oc->setMediaVideoProfilePolicyView(mediaVideoProfilePolicyView->parseResult);
        getRecordingStorage(oc, responseHandler);
    }, AsyncRequestType::HTTPGET, true);
}

// Get Recording storage
void SunapiDeviceClientImpl::getRecordingStorage(const DeviceObjectComposerPtr& oc,
                                                 const ResponseBaseHandler& responseHandler)
{
    auto recordingStorageView =
            std::make_shared<RecordingStorageView>(m_logPrefix,
                                                   std::make_shared<DeviceConnectResponse>(),
                                                   true, true); // ignore failure
    asyncRequest(m_httpCommandSession,
                 recordingStorageView,
                [this, responseHandler](const ResponseBaseSharedPtr& responseBase) {

                    if (responseHandler) {
                        responseHandler(responseBase);
                    }
                    m_isConnecting = false;
                },
                 [this, oc, recordingStorageView, responseHandler](){

        oc->setRecordingStorageView(recordingStorageView->parseResult);
        getRecordingGeneral(oc, responseHandler);
    }, AsyncRequestType::HTTPGET, true);
}


// Get Recording general setup
void SunapiDeviceClientImpl::getRecordingGeneral(const DeviceObjectComposerPtr& oc,
                                                 const ResponseBaseHandler& responseHandler)
{
    auto recordingGeneralView =
            std::make_shared<RecordingGeneralView>(m_logPrefix,
                                                   std::make_shared<DeviceConnectResponse>(),
                                                   true, true); // ignore failure
    asyncRequest(m_httpCommandSession,
                 recordingGeneralView,
                [this, responseHandler](const ResponseBaseSharedPtr& responseBase) {

                    if (responseHandler) {
                        responseHandler(responseBase);
                    }
                    m_isConnecting = false;
                },
                 [this, oc, recordingGeneralView, responseHandler](){

        oc->setRecordingGeneralView(recordingGeneralView->parseResult);
        getMediaAudioOutput(oc, responseHandler);
    }, AsyncRequestType::HTTPGET, true);
}

// Get audio output
void SunapiDeviceClientImpl::getMediaAudioOutput(const DeviceObjectComposerPtr &oc, const ResponseBaseHandler &responseHandler)
{
    auto mediaAudioOutputView =
            std::make_shared<MediaAudioOutputView>(m_logPrefix,
                                                   std::make_shared<DeviceConnectResponse>(),
                                                   true, true); // ignore failure
    asyncRequest(m_httpCommandSession,
                 mediaAudioOutputView,
                [this, responseHandler](const ResponseBaseSharedPtr& responseBase) {

                    if (responseHandler) {
                        responseHandler(responseBase);
                    }
                    m_isConnecting = false;
                },
                 [this, oc, mediaAudioOutputView, responseHandler](){

        oc->setMediaAudioOutputView(mediaAudioOutputView->parseResult);
        getEventSourcesAlarmInput(oc, responseHandler);
    }, AsyncRequestType::HTTPGET, true);
}

// Get alarm inputs
void SunapiDeviceClientImpl::getEventSourcesAlarmInput(const DeviceObjectComposerPtr &oc, const ResponseBaseHandler &responseHandler)
{
    auto alarmInputView =
            std::make_shared<EventSourcesAlarmInputView>(m_logPrefix,
                                                         std::make_shared<DeviceConnectResponse>(),
                                                         true, true); // ignore failure
    asyncRequest(m_httpCommandSession,
                 alarmInputView,
                [this, responseHandler](const ResponseBaseSharedPtr& responseBase) {

                    if (responseHandler) {
                        responseHandler(responseBase);
                    }
                    m_isConnecting = false;
                },
                 [this, oc, alarmInputView, responseHandler](){

        oc->setAlarmInputView(alarmInputView->parseResult);
        getIoAlarmOutput(oc, responseHandler);
    }, AsyncRequestType::HTTPGET, true);
}

// Get alarm outputs
void SunapiDeviceClientImpl::getIoAlarmOutput(const DeviceObjectComposerPtr &oc, const ResponseBaseHandler &responseHandler)
{
    auto alarmOutputView =
            std::make_shared<IoAlarmOutputView>(m_logPrefix,
                                                std::make_shared<DeviceConnectResponse>(),
                                                true, true); // ignore failure
    asyncRequest(m_httpCommandSession,
                 alarmOutputView,
                [this, responseHandler](const ResponseBaseSharedPtr& responseBase) {

                    if (responseHandler) {
                        responseHandler(responseBase);
                    }
                    m_isConnecting = false;
                },
                 [this, oc, alarmOutputView, responseHandler](){

        oc->setAlarmOutputView(alarmOutputView->parseResult);

        // if schemaBased, Get EventStatusSchema ///////////////////
        if(oc->attributes().eventStatusCgi.monitorDiff.schemaBased) {
            getEventStatusSchema(oc, responseHandler);
            return;
        }

        // Or if NVR, Get sessionKey ///////////////////
        if (oc->attributes().mediaCgi.sessionKey) {
            getMediaSesseionKey(oc, responseHandler);
            return;
        }

        // Or go to final process //////////////////
        asio::post(m_strand, WeakCallback(shared_from_this(), [this, oc, responseHandler]()
        {
            DeviceConnectFinal(oc, responseHandler);
        }));
    }, AsyncRequestType::HTTPGET, true);
}

void SunapiDeviceClientImpl::getEventStatusSchema(const DeviceObjectComposerPtr& oc, ResponseBaseHandler const& responseHandler)
{
    auto eventStatusSchemaView =
            std::make_shared<EventStatusSchemaView>(m_logPrefix,
                                                std::make_shared<DeviceConnectResponse>(),
                                                true, true); // ignore failure
    asyncRequest(m_httpCommandSession,
                 eventStatusSchemaView,
                [this, responseHandler](const ResponseBaseSharedPtr& responseBase) {

                    if (responseHandler) {
                        responseHandler(responseBase);
                    }
                    m_isConnecting = false;
                },
                 [this, oc, eventStatusSchemaView, responseHandler](){

        oc->setEventStatusSchema(eventStatusSchemaView->parseResult);

        // if NVR, Get sessionKey ///////////////////
        if (oc->attributes().mediaCgi.sessionKey) {
            getMediaSesseionKey(oc, responseHandler);
            return;
        }

        // Or go to final process //////////////////
        asio::post(m_strand, WeakCallback(shared_from_this(), [this, oc, responseHandler]()
        {
            DeviceConnectFinal(oc, responseHandler);
        }));
    }, AsyncRequestType::HTTPGET, true);
}
// Get Media Session Key for Recorder
void SunapiDeviceClientImpl::getMediaSesseionKey(const DeviceObjectComposerPtr& oc, const ResponseBaseHandler& responseHandler)
{
    auto mediaSessionkeyView = std::make_shared<MediaSessionkeyView>(m_logPrefix);

    /* 
      발생조건 : S1 NVR + DDNS(외부망) + HTTPS + Client(사내망) 
      동일 세션으로 sessionkey 여러개 받아오는 경우 stream truncated 문제 발생.
      http session 초기화 하는 문제로 해당 문제 개선
    */
    setHttpConnection(m_httpCommandSession, m_connectionInfo);

    asyncRequest(m_httpCommandSession,
                 mediaSessionkeyView,
                [this, responseHandler](const ResponseBaseSharedPtr& responseBase) {

                    if (responseHandler) {
                        responseHandler(responseBase);
                    }
                    m_isConnecting = false;
                },
                 [this, oc, mediaSessionkeyView, responseHandler]()
    {
        constexpr int kMaxSessionNum = 5;
        DevcieSessionKey deviceSessionKey(mediaSessionkeyView->parseResult, "");
        oc->setSessionKey(deviceSessionKey);

        // Continue to request session key
        if (oc->sessionKeys().size() < kMaxSessionNum) {
            getMediaSesseionKey(oc, responseHandler);
            return;
        }

        getMediaCameraRegister(oc, responseHandler);

    }, AsyncRequestType::HTTPGET, true);
}

// Get camera register info for Recorder
void SunapiDeviceClientImpl::getMediaCameraRegister(const DeviceObjectComposerPtr &oc, const ResponseBaseHandler &responseHandler)
{
    auto mediaCameraRegisterView =
            std::make_shared<MediaCameraRegisterView>(m_logPrefix,
                                                      std::make_shared<DeviceConnectResponse>(),
                                                      true, true); // ignore failure
    asyncRequest(m_httpCommandSession,
                 mediaCameraRegisterView,
                [this, responseHandler](const ResponseBaseSharedPtr& responseBase) {

                    if (responseHandler) {
                        responseHandler(responseBase);
                    }
                    m_isConnecting = false;
                },
                 [this, oc, mediaCameraRegisterView, responseHandler](){

        oc->setMediaCameraRegisterView(mediaCameraRegisterView->parseResult);
        getMediaCameraUpgrade(oc, responseHandler);
    }, AsyncRequestType::HTTPGET, true);
}

// Get camera upgrade info for Recorder
void SunapiDeviceClientImpl::getMediaCameraUpgrade(const DeviceObjectComposerPtr &oc, const ResponseBaseHandler &responseHandler)
{
    auto mediaCameraUpgradeView =
            std::make_shared<MediaCameraUpgradeView>(m_logPrefix,
                                                      std::make_shared<DeviceConnectResponse>(),
                                                      true, true); // ignore failure
    asyncRequest(m_httpCommandSession,
                 mediaCameraUpgradeView,
                [this, responseHandler](const ResponseBaseSharedPtr& responseBase) {

                    if (responseHandler) {
                        responseHandler(responseBase);
                    }
                    m_isConnecting = false;
                },
                 [this, oc, mediaCameraUpgradeView, responseHandler](){

        oc->setMediaCameraUpgradeView(mediaCameraUpgradeView->parseResult);
        getEventSourcesNetworkAlarmInput(oc, responseHandler);
    }, AsyncRequestType::HTTPGET, true);
}

// Get network alarm input for Recorder
void SunapiDeviceClientImpl::getEventSourcesNetworkAlarmInput(const DeviceObjectComposerPtr &oc, const ResponseBaseHandler &responseHandler)
{
    auto networkAlarmInputView =
            std::make_shared<EventSourcesNetworkAlarmInputView>(m_logPrefix,
                                                                std::make_shared<DeviceConnectResponse>(),
                                                                true, true); // ignore failure
    asyncRequest(m_httpCommandSession,
                 networkAlarmInputView,
                [this, responseHandler](const ResponseBaseSharedPtr& responseBase) {

                    if (responseHandler) {
                        responseHandler(responseBase);
                    }
                    m_isConnecting = false;
                },
                 [this, oc, networkAlarmInputView, responseHandler](){

        oc->setNetworkAlarmInputView(networkAlarmInputView->parseResult);

        // if has POS, Get Pos Conf ///////////////////
        if (oc->attributes().systemAttribute.maxPOS > 0) {
            getRecordingPosConf(oc, responseHandler);
            return;
        }

        // go to final process //////////////////
        asio::post(m_strand, WeakCallback(shared_from_this(), [this, oc, responseHandler]()
        {
            DeviceConnectFinal(oc, responseHandler);
        }));
    }, AsyncRequestType::HTTPGET, true);
}

void SunapiDeviceClientImpl::getRecordingPosConf(const DeviceObjectComposerPtr &oc, const ResponseBaseHandler &responseHandler)
{
    auto recordingTextPosConfView = std::make_shared<RecordingTextPosConfView>(m_logPrefix,
                                                                               nullptr,
                                                                               true, true); // ignore failure

    asyncRequest(m_httpCommandSession,
                 recordingTextPosConfView,
                 [this, responseHandler](const ResponseBaseSharedPtr& responseBase) {
                     if (responseHandler) {
                         responseHandler(responseBase);
                     }
                     m_isConnecting = false;
                 },
                 [this, oc, recordingTextPosConfView, responseHandler]() {

        oc->setRecordingTextPosConfView(recordingTextPosConfView->m_confData);

        // go to final process //////////////////
        asio::post(m_strand, WeakCallback(shared_from_this(), [this, oc, responseHandler]()
        {
            DeviceConnectFinal(oc, responseHandler);
        }));
    },
    AsyncRequestType::HTTPGET, true);
}

void SunapiDeviceClientImpl::DeviceConnectFinal(const DeviceObjectComposerPtr& oc, ResponseBaseHandler const& responseHandler)
{
    SLOG_DEBUG("DeviceConnectFinal()");
    int registrationMsec = QDateTime::currentMSecsSinceEpoch() - this->m_deviceRegistrationStartTime;
    qDebug() << "registrationMsec" << registrationMsec;

    // assign new object composer
    m_repos = oc;
    m_repos->rebuildDevice(m_deviceUUID, m_connectionInfo);

    // response handler
    if (responseHandler) {
        auto deviceResponse = std::make_shared<DeviceConnectResponse>();
        deviceResponse->device = m_repos->device();
        deviceResponse->deviceStatus = m_repos->status();
        deviceResponse->deviceProfileInfo = m_repos->profileInfo();
#ifdef SUNAPI_CAMERA_SUPPORT
        responseHandler(deviceResponse);
#else
        if(deviceResponse->device.connectionInfo.registrationMsec == 0)
            deviceResponse->device.connectionInfo.registrationMsec = registrationMsec;

        if(m_repos->isRecorder()){
            responseHandler(deviceResponse);
        }

        else{
            SPDLOG_WARN("CURRENT VERSION DOES NOT SUPPORT SUNAPI CAMERA");
            deviceResponse->errorCode = Wisenet::ErrorCode::InCompatibleDevice;
            m_httpCommandSession->Close();
            m_isConnecting = false;
            responseHandler(deviceResponse);
            return;
        }
#endif
    }

    if (!m_allowRelay && ConnectedType::P2P_RELAY == m_connectionInfo.connectedType) {
        // 릴레이 허가 옵션이 아니고 등록 중이라 RELAY 모드로 여기까지 온 것임. 접속을 끊는다.
        asio::post(m_strand, WeakCallback(shared_from_this(), [this]()
            {
                SPDLOG_INFO("Disconnect by RELAY MODE in adding device.");                
                disconnect(DeviceStatusType::DisconnectedWithRestriction);
                m_startRelayMode = 0;
            }));

    }else {

        m_httpCommandSession->Close();

        // setup ptz session
        setHttpConnection(m_httpPtzSession, m_connectionInfo);
        setHttpConnection(m_httpSearchMetaDataSession, m_connectionInfo);
        // setup confg session
        setHttpConnection(m_httpConfigSession, m_connectionInfo, 60);
        // setup channel upgrade session
        setHttpConnection(m_httpChannelUpgradeSession, m_connectionInfo);

        startEventSession();
        //startPosEventSession();
    }

    // 항상 완료처리가 되어야 함.
    m_isConnecting = false;
    m_isCompletedConnection = true;

}

void SunapiDeviceClientImpl::startEventSession()
{
    //Timer에서 EventSession상태를 모니터할 수있도록 추가.
    m_needToMonitorEventSessionStatus = true;
    m_isRunningEventSession = true;

    // setup and connect to event session
    setHttpConnection(m_httpEventSession, m_connectionInfo);

    std::shared_ptr<IEventStatusDispatcher> eventStatusDispatcher;

    auto needJsonResponse = false;
    if(m_repos->attributes().eventStatusCgi.monitorDiff.schemaBased){
        eventStatusDispatcher = std::make_shared<SchemaBasedEventStatusDispatcher>(m_logPrefix);
        needJsonResponse = true;
    }else{
        eventStatusDispatcher = std::make_shared<EventStatusDispatcher>(m_logPrefix);
    }

    eventStatusDispatcher->SetEventHandler(
                std::bind(&SunapiDeviceClientImpl::eventHandler, shared_from_this(), std::placeholders::_1),
                std::bind(&SunapiDeviceClientImpl::changeConfigUri, shared_from_this(), std::placeholders::_1),
                std::bind(&SunapiDeviceClientImpl::changeAlarmOuputStatus, shared_from_this(), std::placeholders::_1, std::placeholders::_2));

    eventStatusDispatcher->SetDeviceLatestEventStatusHandler(
                std::bind(&SunapiDeviceClientImpl::deviceLatestStatusEventHandler, shared_from_this(), std::placeholders::_1));

    eventStatusDispatcher->ResetObjectComposer(m_repos);

    m_httpEventSession->HttpMonitorRequest(needJsonResponse, true,
                                           WeakCallback(shared_from_this(), [this, eventStatusDispatcher]
                                                        (const HttpResponseSharedPtr& response)
    {
        asio::post(m_strand, WeakCallback(shared_from_this(), [this, eventStatusDispatcher, response]()
        {
            if (response->failed) {
                if (response->error == boost::asio::error::operation_aborted) {
                    SLOG_DEBUG("HttpMonitorRequest operation aborted..");
                }
                else {
                    SLOG_INFO("Failed to connect event session of SunapiDeviceClient :: error={}, status={}",
                              response->error.message(), response->httpStatus);
                }

                m_isRunningEventSession = false;
            }
            else {
                SLOG_TRACE("Successfully received SUNAPI events :: body={}{}",
                           SPDLOG_EOL, response->httpBody);

                m_lastReveivedTime = std::chrono::steady_clock::now();
                eventStatusDispatcher->ParseNewEvent(response->httpBody);
            }
        }));
    }));
}

void SunapiDeviceClientImpl::startPosEventSession()
{
    if(m_repos->attributes().systemAttribute.maxPOS <= 0 || !m_repos->isPosEnabled()) {
        // POS 미지원 or 활성화 된 POS가 없는 경우 Session Close 후 리턴
        m_needToMonitorPosEventSession = false;
        m_isRunningPosEventSession = false;
        m_httpPosEventSession->Close();
        return;
    }

    //Timer에서 Pos Event Session상태를 모니터할 수있도록 추가.
    m_needToMonitorPosEventSession = true;
    m_isRunningPosEventSession = true;

    // setup and connect to event session
    setHttpConnection(m_httpPosEventSession, m_connectionInfo);

    std::shared_ptr<PosDataDispatcher> posDataDispatcher = std::make_shared<PosDataDispatcher>(m_logPrefix);
    posDataDispatcher->SetEventHandler(std::bind(&SunapiDeviceClientImpl::eventHandler, shared_from_this(), std::placeholders::_1));
    posDataDispatcher->ResetObjectComposer(m_repos);

    m_httpPosEventSession->HttpPosMonitorRequest(WeakCallback(shared_from_this(), [this, posDataDispatcher](const HttpResponseSharedPtr& response)
    {
        asio::post(m_strand, WeakCallback(shared_from_this(), [this, posDataDispatcher, response]()
        {
            if (response->failed) {
                if (response->error == boost::asio::error::operation_aborted) {
                    SLOG_DEBUG("HttpPosMonitorRequest operation aborted..");
                }
                else {
                    SLOG_INFO("Failed to connect POS event session of SunapiDeviceClient :: error={}, status={}",
                              response->error.message(), response->httpStatus);
                }

                m_isRunningPosEventSession = false;
            }
            else {
                SLOG_TRACE("Successfully received POS events :: body={}{}",
                           SPDLOG_EOL, response->httpBody);
                //m_lastReveivedTime = std::chrono::steady_clock::now();
                posDataDispatcher->ParseNewEvent(response->httpBody);
            }
        }));
    }));
}

}
}
