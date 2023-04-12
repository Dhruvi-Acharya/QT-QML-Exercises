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
#include "DeviceListViewModel.h"
#include "QCoreServiceManager.h"
#include "LogSettings.h"
#include <QtCore>
#include "WisenetViewerDefine.h"
#include <QProcess>
#include <QDir>
#include "VersionManager.h"
#include <boost/algorithm/string.hpp>

DeviceListViewModel::DeviceListViewModel(QObject* parent)
{
    Q_UNUSED(parent);
    m_port = 80;

    qmlRegisterType<DeviceListViewModel>("Wisenet.Define",
                                         WisenetViewerDefine::WISENET_VIEWER_MAJOR_VERSION,
                                         WisenetViewerDefine::WISENET_VIEWER_MINER_VERSION,
                                         "DeviceIpType");

    reset();
    connect(&QCoreServiceManager::Instance(), &QCoreServiceManager::coreServiceEventTriggered,
            this, &DeviceListViewModel::coreServiceEventTriggered, Qt::QueuedConnection);
}

void DeviceListViewModel::deleteDevices()
{
    auto request = std::make_shared<Wisenet::Core::RemoveDevicesRequest>();

    for(auto &id : m_deviceGuidList){
        request->deviceIDs.push_back(id.toStdString());
    }

    if(request->deviceIDs.size()==0){
        emit resultMessage(false, QCoreApplication::translate("WisenetLinguist","Please select device"));
        return;
    }

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::RemoveDevices,
                this, request,
                [=](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::ResponseBase>(reply->responseDataPtr);

        qDebug() << "DeviceListViewModel::deleteDevice() response:" << QString::fromStdString(response->errorString());
        emit resultMessage(true, QCoreApplication::translate("WisenetLinguist","Success"));
        emit removedDevice();

    });
}
// TODO. IPv6 적용시 ComboBox 인덱스와 ConnectionType 값을 일치시켜야 한다.
DeviceListViewModel::DeviceIpType converIpType(bool ssl, Wisenet::Device::ConnectionType type){
    if (!ssl && type == Wisenet::Device::ConnectionType::StaticIPv4) return DeviceListViewModel::DeviceStaticIp;
    else if (ssl && type == Wisenet::Device::ConnectionType::StaticIPv4) return DeviceListViewModel::DeviceStaticIpSSL;
    else if (!ssl && type == Wisenet::Device::ConnectionType::HttpUrl) return DeviceListViewModel::DeviceUrl;
    else if (ssl && type == Wisenet::Device::ConnectionType::HttpUrl) return DeviceListViewModel::DeviceUrlSSL;
    else if (type == Wisenet::Device::ConnectionType::WisenetDDNS) return DeviceListViewModel::DeviceDdnsP2p;
    else if (!ssl && type == Wisenet::Device::ConnectionType::StaticIPv6) return DeviceListViewModel::DeviceStaticIpv6;
    else if (ssl && type == Wisenet::Device::ConnectionType::StaticIPv6) return DeviceListViewModel::DeviceStaticIpv6SSL;
    else if (type == Wisenet::Device::ConnectionType::S1_DDNS) return DeviceListViewModel::DeviceS1Ddns;
    else if (type == Wisenet::Device::ConnectionType::S1_SIP) return DeviceListViewModel::DeviceS1Sip;
    else return DeviceListViewModel::DeviceStaticIp;
}

Wisenet::Device::MediaProtocolType convertMediaProtocolType(int index){
    switch (index) {
    case 0: return Wisenet::Device::MediaProtocolType::RTP_RTSP;
    case 1: return Wisenet::Device::MediaProtocolType::RTP_UDP;
    case 2: return Wisenet::Device::MediaProtocolType::RTSP_HTTP;
    case 3: return Wisenet::Device::MediaProtocolType::RTSP_MULTICAST;
    default: return Wisenet::Device::MediaProtocolType::RTP_RTSP;
    }
}

// TODO. IPv6 적용시 ComboBox 인덱스와 ConnectionType 값을 일치시켜야 한다. (확인필요)
Wisenet::Device::ConnectionType convertIpType(int index, bool& bSSL)
{
#ifdef WISENET_S1_VERSION
    switch (index) {
    case 0: return Wisenet::Device::ConnectionType::StaticIPv4;
    case 1: { bSSL = true; return Wisenet::Device::ConnectionType::StaticIPv4;}
    case 2: return Wisenet::Device::ConnectionType::HttpUrl;
    case 3: { bSSL = true; return Wisenet::Device::ConnectionType::HttpUrl; }
    case 4: return Wisenet::Device::ConnectionType::S1_DDNS;
    case 5: return Wisenet::Device::ConnectionType::S1_SIP;
    default: return Wisenet::Device::ConnectionType::S1_DDNS;
    }
#else
#ifdef Q_OS_WINDOWS
    switch (index) {
    case 0: return Wisenet::Device::ConnectionType::StaticIPv4;
    case 1: { bSSL = true; return Wisenet::Device::ConnectionType::StaticIPv4;}
    case 2: return Wisenet::Device::ConnectionType::StaticIPv6;
    case 3: { bSSL = true; return Wisenet::Device::ConnectionType::StaticIPv6; }
    case 4: return Wisenet::Device::ConnectionType::HttpUrl;
    case 5: { bSSL = true; return Wisenet::Device::ConnectionType::HttpUrl; }
    case 6: return Wisenet::Device::ConnectionType::WisenetDDNS;
    default: return Wisenet::Device::ConnectionType::WisenetDDNS;
    }
#else
    switch (index) {
    case 0: return Wisenet::Device::ConnectionType::StaticIPv4;
    case 1: { bSSL = true; return Wisenet::Device::ConnectionType::StaticIPv4;}
    case 2: return Wisenet::Device::ConnectionType::HttpUrl;
    case 3: { bSSL = true; return Wisenet::Device::ConnectionType::HttpUrl; }
    case 4: return Wisenet::Device::ConnectionType::WisenetDDNS;
    default: return Wisenet::Device::ConnectionType::WisenetDDNS;
    }
#endif
#endif
}

QString convertConnectedType(Wisenet::Device::ConnectedType type){
    switch (type) {
    case Wisenet::Device::ConnectedType::StaticIPv4: return "StaticIPv4";
    case Wisenet::Device::ConnectedType::StaticIPv6: return "StaticIPv6";
    case Wisenet::Device::ConnectedType::WisenetDDNS: return "WisenetDDNS";
    case Wisenet::Device::ConnectedType::HttpUrl: return "HttpUrl";
    case Wisenet::Device::ConnectedType::P2P: return "P2P";
    case Wisenet::Device::ConnectedType::P2P_RELAY: return "P2P(RELAY)";
    case Wisenet::Device::ConnectedType::S1_DDNS: return "S1 DDNS";
    case Wisenet::Device::ConnectedType::S1_SIP: return "S1 SIP";
    case Wisenet::Device::ConnectedType::S1_SIP_RELAY: return "S1 SIP(RELAY)";
    default: return "StaticIPv4";
    }
}

void DeviceListViewModel::resetDeviceName()
{
    SPDLOG_DEBUG("DeviceListViewModel::resetDeviceName m_deviceGuid = {}",
                 m_deviceGuid.toStdString());

    auto updateDevicesRequest = std::make_shared<Wisenet::Core::UpdateDevicesRequest>();

    for(auto& deviceId : m_deviceGuidList){
        Wisenet::Device::Device device;
        if(QCoreServiceManager::Instance().DB()->FindDevice(deviceId, device)){
            device.name = device.deviceName;

            for(auto& channel : device.channels){
                channel.second.name = channel.second.channelName;
            }

            updateDevicesRequest->devices.push_back(device);
        }
    }

    if(updateDevicesRequest->devices.size() > 0){
        QCoreServiceManager::Instance().RequestToCoreService(
                    &Wisenet::Core::ICoreService::UpdateDevices,
                    this, updateDevicesRequest,
                    [=](const QCoreServiceReplyPtr& reply)
        {
            auto response = reply->responseDataPtr;
            if(response->isSuccess()){
                qDebug() << "DeviceListViewModel::resetDeviceName success";
                emit resultMessage(true, QCoreApplication::translate("WisenetLinguist","Success"));
            }
            else{
                qDebug() << "DeviceListViewModel::resetDeviceName fail";
                emit resultMessage(false, QCoreApplication::translate("WisenetLinguist","Failed"));
            }
        });
    }
}

void DeviceListViewModel::openBrowser()
{
    QStringList argvs = {m_url};
    QProcess::startDetached("C:\\Program Files (x86)\\Microsoft\\Edge\\Application\\msedge.exe", argvs);
}

void DeviceListViewModel::changeConnectInfo(bool connection, int ipTypeIndex, int streamingTypeIndex, int newPort, QString newIp, QString newName, bool dtls)
{
    SPDLOG_DEBUG("DeviceListViewModel::changeNetwork connection={} type={}, streaming={}, port={}, ip={}, dtls={}",
                 connection, ipTypeIndex, streamingTypeIndex, newPort, newIp.toStdString(), dtls);

    disconnectDevice();
    saveDevice(connection, ipTypeIndex, streamingTypeIndex, newPort, newIp, newName, dtls);
}

void DeviceListViewModel::changeCredential(QString id, QString pass)
{
    SPDLOG_DEBUG("DeviceListViewModel::changeCredential id={} pass={}",
                 id.toStdString(), pass.toStdString());

    disconnectDevice();
    saveDeviceCredential(id, pass);
}

QString DeviceListViewModel::translate(QVariant display){
    std::string result = display.toString().toStdString();
    return QCoreApplication::translate("WisenetLinguist",result.c_str());

}

void DeviceListViewModel::reset()
{
    setDeviceName("");
    setDeviceGuid("-");
    setModel("-");
    setVersion("-");
    setMacAddress("-");
    setHost("");
    setDeviceCertResult("-");
    setConnectedType("-");

    setId("");
    setPassword("");
    setIpType(DeviceIpType::DeviceStaticIp);
    setStreamingProtocol(0);
    setPort(80);
    setConnection(false);
    setDtls(false);
    m_deviceGuidList.clear();
}

QString DeviceListViewModel::makeUrl(Wisenet::Device::ConnectedType connectedType, bool supportWebSocket, std::string ip, int port, bool useSSL, Wisenet::Device::DeviceType dType)
{
    QString ret;
    if(useSSL){
        ret = "https://" + QString::fromStdString(ip);

        if(connectedType == Wisenet::Device::ConnectedType::P2P ||
           connectedType == Wisenet::Device::ConnectedType::P2P_RELAY)
            ret = "https://localhost";

        if(port != 443)
            ret += ":" + QString::number(port);
    }
    else{
        ret = "http://" + QString::fromStdString(ip);
        if(connectedType == Wisenet::Device::ConnectedType::P2P ||
           connectedType == Wisenet::Device::ConnectedType::P2P_RELAY)
            ret = "http://localhost";
        if(port != 80)
            ret += ":" + QString::number(port);

    }
    if(VersionManager::Instance()->s1Support()){
        std::string mac = boost::erase_all_copy(this->macAddress().toStdString(), ":");
        ret += ("/index.php?MAC=" + QString::fromStdString(mac) + "/setup/");
    }
    else if(dType == Wisenet::Device::DeviceType::SunapiRecorder && supportWebSocket)
        ret += "/uwa/#/setup";

    SPDLOG_DEBUG("MakeUrl:{} s1Support:{}",ret.toStdString(), VersionManager::Instance()->s1Support());
    return ret;
}

QString DeviceListViewModel::deviceCertResult(){

    if(m_deviceCertResult.size() ==0)
        return "-";
    else
        return m_deviceCertResult;

}

void DeviceListViewModel::selectDevices(QVector<QString> deviceIdList)
{
    QSet<QString> deviceSet;
    reset();
    m_deviceGuidList.clear();

    for(auto &id : deviceIdList){
        SPDLOG_DEBUG("DeviceListViewModel::selectDevices id={}", id.toStdString());
        Wisenet::Device::Device device;
        if(QCoreServiceManager::Instance().DB()->FindDevice(id, device)){
            if(!deviceSet.contains(id))
            {
                m_deviceGuidList.push_back(id);
                deviceSet.insert(id);
            }
        }
    }
    SPDLOG_DEBUG("DeviceListViewModel::selectDevices device:{}", m_deviceGuidList.size());//, m_channelGuidList.size());
    if(m_deviceGuidList.size()>0){
        setDeviceGuid(m_deviceGuidList.first());
        if(m_deviceGuidList.size() == 1){
            setDeviceGuid(m_deviceGuidList.at(0));
            Wisenet::Device::Device device;
            if(QCoreServiceManager::Instance().DB()->FindDevice(m_deviceGuidList.at(0), device)){
                setDeviceName(QString::fromStdString(device.name));
                setModel(QString::fromStdString(device.modelName));

                setMacAddress(QString::fromStdString(device.macAddress));

                SPDLOG_DEBUG("DeviceListViewModel::selectDevices(+) ip:{} -- {}",
                             device.connectionInfo.host, device.connectionInfo.ip);

                Wisenet::Device::DeviceStatus deviceStatus;
                if(QCoreServiceManager::Instance().DB()->FindDeviceStatus(device.deviceID,deviceStatus) &&
                        deviceStatus.isGood()){
                    setConnectedType(convertConnectedType(device.connectionInfo.connectedType));
                    setVersion(QString::fromStdString(device.firmwareVersion));
                    setDeviceCertResult(QString::fromStdString(device.deviceCertifcateResult));

                }
                else{

                    setConnectedType("-");
                    setVersion("-");
                    setDeviceCertResult("-");
                }

                if (device.connectionInfo.connectionType == Wisenet::Device::ConnectionType::StaticIPv6)
                {
                    // Delete "[" at string start and "]" at string end
                    auto itr = device.connectionInfo.host.find("[");
                    if (itr != std::string::npos) {
                        device.connectionInfo.host.erase(itr, 1);
                        auto itr2 = device.connectionInfo.host.find("]");
                        if (itr2 != std::string::npos) {
                            device.connectionInfo.host.erase(itr2, 1);
                        }
                    }
                    setHost(QString::fromStdString(device.connectionInfo.host));
                }
                else
                {
                    setHost(QString::fromStdString(device.connectionInfo.host));
                }
                SPDLOG_DEBUG("DeviceListViewModel::selectDevices ip:{}",
                             m_host.toStdString());
                setId(QString::fromStdString(device.connectionInfo.user));
                setPassword(QString::fromStdString(device.connectionInfo.password));
                setIpType(converIpType(device.connectionInfo.isSSL, device.connectionInfo.connectionType));
                setStreamingProtocol((int)device.connectionInfo.mediaProtocolType);
                setPort(device.connectionInfo.isSSL?device.connectionInfo.sslPort:device.connectionInfo.port);
                SPDLOG_DEBUG("DeviceListViewModel::selectDevices ssl:{} sslPort:{} port:{}",
                            device.connectionInfo.isSSL, device.connectionInfo.sslPort,device.connectionInfo.port);
                setConnection(device.connectionInfo.use);
                setSupportWebSocket(device.supportWebSocket);
                setDtls(device.connectionInfo.usageDtlsMode);

                std::string tempIp = device.connectionInfo.ip;
                if(tempIp.length() < 1)
                    tempIp = device.connectionInfo.host;

                setUrl(makeUrl(device.connectionInfo.connectedType ,device.supportWebSocket, tempIp,
                               m_port,
                               device.connectionInfo.isSSL, device.deviceType));
                setUrl2(makeUrl(device.connectionInfo.connectedType ,false, tempIp,
                               m_port,
                               device.connectionInfo.isSSL, device.deviceType));

            }
        }
        else{
            setDeviceName(QString::number(m_deviceGuidList.size()) + " devices");
        }
        emit deviceGuidListChanged();
    }

    emit deviceCountChanged();
}


void DeviceListViewModel::disconnectDevice()
{
    SPDLOG_DEBUG("DeviceListViewModel::DisconnectDevice device:{}", m_deviceGuidList.size());//, m_channelGuidList.size());
    for(auto& deviceId : m_deviceGuidList){
        Wisenet::Device::Device device;
        if(QCoreServiceManager::Instance().DB()->FindDevice(deviceId, device)){
            if(!device.connectionInfo.use)
                continue;

            auto disconnectRequest = std::make_shared<Wisenet::Device::DeviceDisconnectRequest>();

            disconnectRequest->deviceID = deviceId.toStdString();

            QCoreServiceManager::Instance().RequestToCoreService(
                        &Wisenet::Core::ICoreService::DeviceDisconnect,
                        this, disconnectRequest,
                        [=](const QCoreServiceReplyPtr& reply)
            {
                auto response = reply->responseDataPtr;
                if(response->isSuccess()){
                    qDebug() << "DeviceListViewModel::DisconnectDevice success";
                }
                else
                    qDebug() << "DeviceListViewModel::DisconnectDevice fail";
            });
        }
    }
}

void DeviceListViewModel::saveDeviceCredential(QString id_, QString pass)
{
    SPDLOG_DEBUG("DeviceListViewModel::saveDeviceCredential device:{}", m_deviceGuidList.size());//, m_channelGuidList.size());
    SPDLOG_DEBUG("DeviceListViewModel::saveDeviceCredential id={} pass={}",
                 id_.toStdString(), pass.toStdString());
    auto devicesCredential = std::make_shared<Wisenet::Core::SaveDevicesCredentialRequest>();

    std::string localId = id_.toStdString();
    std::string localPass = pass.toStdString();

    for(auto& deviceId : m_deviceGuidList){
        Wisenet::Device::Device device;
        if(QCoreServiceManager::Instance().DB()->FindDevice(deviceId, device)){
            device.connectionInfo.user = localId;
            device.connectionInfo.password = localPass;
            devicesCredential->devices.push_back(device);

            qDebug() << "DeviceListViewModel::saveDeviceCredential 4";
        }
    }
    if(devicesCredential->devices.size() > 0){
        QCoreServiceManager::Instance().RequestToCoreService(
                    &Wisenet::Core::ICoreService::SaveDevicesCredential,
                    this, devicesCredential,
                    [=](const QCoreServiceReplyPtr& reply)
        {
            auto response = std::static_pointer_cast<Wisenet::Core::SaveDevicesCredentialResponse>(reply->responseDataPtr);
            qDebug() << "DeviceListViewModel::saveDeviceCredential 5" ;
            if(response->isSuccess()){
                qDebug() << "DeviceListViewModel::saveDeviceCredential success";
                //emit resultMessage(true, QCoreApplication::translate("WisenetLinguist","Success"));
                m_id = id_;
                m_password = pass;
                connectDevice(response->devices);
            }
            else{
                qDebug() << "DeviceListViewModel::saveDeviceCredential fail";
                //emit resultMessage(false, QCoreApplication::translate("WisenetLinguist","Failed"));
            }
        });
    }
}

void DeviceListViewModel::saveDevice(bool use, int ipTypeIndex, int streamingTypeIndex, int newPort, QString newIp, QString newName, bool dtls)
{
    SPDLOG_DEBUG("DeviceListViewModel::SaveDevice device:{}", m_deviceGuidList.size());//, m_channelGuidList.size());
    SPDLOG_DEBUG("DeviceListViewModel::SaveDevice connection={} type={}, streaming={}, port={}, ip={} dtls={}",
                 use, ipTypeIndex, streamingTypeIndex, newPort, newIp.toStdString(), dtls);
    auto updateDevicesRequest = std::make_shared<Wisenet::Core::UpdateDevicesRequest>();

    auto localMediaStreaming = convertMediaProtocolType(streamingTypeIndex);
    bool bSSL = false;
    auto localIpType = convertIpType(ipTypeIndex, bSSL);

    for(auto& deviceId : m_deviceGuidList){
        Wisenet::Device::Device device;
        if(QCoreServiceManager::Instance().DB()->FindDevice(deviceId, device)){
            device.connectionInfo.use = use;
            device.connectionInfo.mediaProtocolType = localMediaStreaming;
            device.connectionInfo.usageDtlsMode = dtls;
            if(m_deviceGuidList.size() ==1){
                device.connectionInfo.connectionType = localIpType;
                device.connectionInfo.isSSL = bSSL;
                device.connectionInfo.host = newIp.toStdString();
                if(device.connectionInfo.connectionType != Wisenet::Device::ConnectionType::WisenetDDNS){
                    if(bSSL)
                        device.connectionInfo.sslPort = newPort;
                    else
                        device.connectionInfo.port = newPort;
                }
                device.name = newName.toStdString();
            }
            updateDevicesRequest->devices.push_back(device);
        }
    }
    if(updateDevicesRequest->devices.size() > 0){
        QCoreServiceManager::Instance().RequestToCoreService(
                    &Wisenet::Core::ICoreService::UpdateDevices,
                    this, updateDevicesRequest,
                    [=](const QCoreServiceReplyPtr& reply)
        {
            auto response = std::static_pointer_cast<Wisenet::Core::UpdateDevicesResponse>(reply->responseDataPtr);

            if(response->isSuccess()){
                qDebug() << "DeviceListViewModel::SaveDevice success";
                emit resultMessage(true, QCoreApplication::translate("WisenetLinguist","Success"));
                connectDevice(response->devices);
            }
            else{
                qDebug() << "DeviceListViewModel::SaveDevice fail";
                emit resultMessage(false, QCoreApplication::translate("WisenetLinguist","Failed"));
            }
        });
    }
}

void DeviceListViewModel::connectDevice(std::vector<Wisenet::Device::Device> devices)
{
    SPDLOG_DEBUG("DeviceListViewModel::ConnectDevice device:{}", devices.size());//, m_channelGuidList.size());
    for(auto& device : devices){
        auto connectRequest = std::make_shared<Wisenet::Device::DeviceConnectRequest>();
        if(!device.connectionInfo.use)
            continue;
        connectRequest->deviceID = device.deviceID;
        connectRequest->connectionInfo = device.connectionInfo;

        QCoreServiceManager::Instance().RequestToCoreService(
                    &Wisenet::Core::ICoreService::DeviceConnect,
                    this, connectRequest,
                    [](const QCoreServiceReplyPtr& reply)
        {
            auto response = reply->responseDataPtr;
            if(response->isSuccess())
                qDebug() << "DeviceListViewModel::ConnectDevice success";
            else
                qDebug() << "DeviceListViewModel::ConnectDevice fail";
        });
    }
}

void DeviceListViewModel::setDeviceGuid(QString deviceGuid)
{
    if(deviceGuid != m_deviceGuid){
        m_deviceGuid = deviceGuid;

        emit this->deviceGuidChanged();
    }
}

void DeviceListViewModel::setDeviceName(QString deviceName)
{
    if(deviceName != m_deviceName){
        m_deviceName = deviceName;

        SPDLOG_DEBUG("DeviceListViewModel::setDeviceName = {}", m_deviceName.toStdString());

        emit this->deviceNameChanged();
    }
}

void DeviceListViewModel::setModel(QString model)
{
    if(model != m_model){
        m_model = model;
        emit this->modelChanged();
    }
}

void DeviceListViewModel::setVersion(QString version)
{
    if(version != m_version){
        m_version = version;
        emit this->versionChanged();
    }
}

void DeviceListViewModel::setMacAddress(QString mac)
{
    if(mac != m_macAddress){
        m_macAddress = mac;
        emit this->macAddressChanged();
    }
}

void DeviceListViewModel::setDeviceCertResult(QString result)
{
    if(result != m_deviceCertResult){
        m_deviceCertResult = result;
        emit this->deviceCertResultChanged();
    }
}

void DeviceListViewModel::setConnectedType(QString type)
{
    if(type != m_connectedType){
        m_connectedType = type;
        emit this->connectedTypeChanged();
    }
}

void DeviceListViewModel::setIpType(DeviceListViewModel::DeviceIpType type)
{
    if(type != m_ipType){
        m_ipType = type;
        emit this->ipTypeChanged();
    }
}

void DeviceListViewModel::setHost(QString host)
{
    if(host != m_host){
        m_host = host;
        emit this->hostChanged();
    }
}

void DeviceListViewModel::setPort(int port)
{
    if(port != m_port){
        m_port = port;
        emit this->portChanged();
    }
}

void DeviceListViewModel::setStreamingProtocol(int type)
{
    if(type != m_streamingProtocol){
        m_streamingProtocol = type;
        emit this->streamingProtocolChanged();
    }
}

void DeviceListViewModel::setId(QString id)
{
    if(id != m_id){
        m_id = id;
        emit this->idChanged();
    }
}

void DeviceListViewModel::setPassword(QString password)
{
    if(password != m_password){
        m_password = password;
        emit this->passwordChanged();
    }
}

void DeviceListViewModel::setConnection(bool connect)
{
    if(connect != m_connection){
        m_connection = connect;
        emit this->connectionChanged();
    }
}

void DeviceListViewModel::setDtls(bool param)
{
    if(param != m_dtls){
        m_dtls = param;
        emit this->dtlsChanged();
    }
}

void DeviceListViewModel::setUrl(QString url)
{
    if(url != m_url){
        m_url = url;
        emit this->urlChanged();
    }
}

void DeviceListViewModel::setUrl2(QString url)
{
    if(url != m_url2){
        m_url2 = url;
        emit this->url2Changed();
    }
}

void DeviceListViewModel::setSupportWebSocket(bool webSocket)
{
    if(webSocket != m_supportWebSocket){
        m_supportWebSocket = webSocket;
        emit this->supportWebSocketChanged();
    }
}

void DeviceListViewModel::coreServiceEventTriggered(QCoreServiceEventPtr event)
{
    auto serviceEvent = event->eventDataPtr;

    switch(serviceEvent->EventTypeId())
    {
    case Wisenet::Core::UpdateDevicesEventType: {
        break;
    }
    case Wisenet::Core::SaveDevicesCredentialEventType:{
        break;
    }
    case Wisenet::Device::DeviceStatusEventType:{
        selectDevices(m_deviceGuidList);
        break;
    }
    }
}

