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
#include "ManualDiscoveryViewModel.h"
#include "LogSettings.h"


ManualDiscoveryViewModel::ManualDiscoveryViewModel()
    :m_highlightRow(-1),
      m_lastHighlightRow(-1)
{
    m_columnNames << "Ip" << "Port" << "Status";
    SPDLOG_DEBUG("ManualDiscoveryViewModel()");
}

ManualDiscoveryViewModel::~ManualDiscoveryViewModel()
{
    SPDLOG_DEBUG("qml: ~ManualDiscoveryViewModel()");
    qDebug() << "qml: ~ManualDiscoveryViewModel()";
}

int ManualDiscoveryViewModel::rowCount(const QModelIndex &) const
{
    return m_index.size();
}

int ManualDiscoveryViewModel::columnCount(const QModelIndex &) const
{
    return m_columnNames.size();
}

QVariant ManualDiscoveryViewModel::data(const QModelIndex &index, int role) const
{
    switch(role) {
    case Qt::DisplayRole:
        return getDataDisplayRole(index);
    case HighlightRole:
        if( m_highlightRows.count( index.row() ) )
            return true;
        return false;
    default:
        break;
    }

    return QVariant();
}

QString ManualDiscoveryViewModel::translate(QVariant display){
    std::string result = display.toString().toStdString();
    return QCoreApplication::translate("WisenetLinguist",result.c_str());

}

QHash<int, QByteArray> ManualDiscoveryViewModel::roleNames() const
{
    return { {Qt::DisplayRole, "display"}, {HighlightRole, "highlightRole"}, {HoveredRole, "hoveredRole"}};
}

void ManualDiscoveryViewModel::sort(int column, Qt::SortOrder order)
{
    Q_UNUSED(column);
    UpdateSortData(column, order);
    beginResetModel();
    m_index = m_sorted;
    endResetModel();
}

int GetLastOctet(std::string ip)
{
    std::stringstream sstream(ip);
    int a,b,c,d; //to store the 4 ints
    char ch; //to temporarily store the '.'
    sstream >> a >> ch >> b >> ch >> c >> ch >> d;
    return d;
}

void ManualDiscoveryViewModel::setHighlightRow(int rowNum, int modifiers)
{
    bool pressedCtrl = modifiers & Qt::ControlModifier;
    bool pressedShift = modifiers & Qt::ShiftModifier;

    bool fireRowChanged(false);
    int minRow(-1), maxRow(-1);

    if(m_highlightRows.count(rowNum)) {
        if(pressedCtrl || m_highlightRows.size() == 1) {
            //deselect it
            m_highlightRows.erase(rowNum);
            if(m_highlightRows.empty())
                m_highlightRow = -1;
            else
                m_highlightRow = *m_highlightRows.begin();
            fireRowChanged = true;
            minRow = rowNum; maxRow = rowNum;
            m_lastHighlightRow = -1;
        }
        else {
            minRow = *m_highlightRows.begin();
            maxRow = *m_highlightRows.rbegin();
            m_highlightRows.clear();
            m_highlightRows.insert(rowNum);
            m_highlightRow = rowNum;
            m_lastHighlightRow = rowNum;
            fireRowChanged = true;
        }
    }
    else {
        if(rowNum == -1) {
            //select none
            if(!m_highlightRows.empty()) {
                //clear selection
                minRow = *m_highlightRows.begin();
                maxRow = *m_highlightRows.rbegin();
                m_highlightRows.clear();
                m_highlightRow = -1;
                m_lastHighlightRow = -1;
                fireRowChanged = true;
            }
        }
        else if(rowNum == -2) {
            //select all
            for(int irow=0; irow<m_index.size(); irow++){
                m_highlightRows.insert(irow);
            }
            fireRowChanged = true;
            m_highlightRow = m_index.size()-1;
            m_lastHighlightRow = m_index.size()-1;
            if(!m_highlightRows.empty()) {
                minRow = *m_highlightRows.begin();
                maxRow = *m_highlightRows.rbegin();
            }
        }
        else {
            if(!pressedCtrl && !pressedShift) {
                if(!m_highlightRows.empty()) {
                    minRow = *m_highlightRows.begin();
                    maxRow = *m_highlightRows.rbegin();
                    m_highlightRows.clear();
                }
            }
            if(!pressedShift || m_lastHighlightRow == -1)
                m_highlightRows.insert(rowNum);
            else {
                int fromRow = m_lastHighlightRow;
                int toRow = rowNum;
                if(fromRow > toRow) std::swap(fromRow, toRow);
                minRow = fromRow;
                maxRow = toRow;
                while(fromRow <= toRow) {
                    m_highlightRows.insert(fromRow);
                    fromRow++;
                }
            }

            m_highlightRow = rowNum;
            m_lastHighlightRow = rowNum;
            fireRowChanged = true;
            if(minRow < 0 || rowNum < minRow) minRow = rowNum;
            if(maxRow < 0 || rowNum > maxRow) maxRow = rowNum;
        }
    }

    if(fireRowChanged) {
        emit highlightRowChanged();
        if(minRow != -1) {
            QVector<int> role;
            role << Role::HighlightRole;
            emit dataChanged(QModelIndex(index(minRow, 0)), QModelIndex(index(maxRow, 2)), role);
        }
    }
}

int ManualDiscoveryViewModel::highlightRow() const
{
    return m_highlightRow;
}

int ManualDiscoveryViewModel::getCurrentRow(int row){
    for(int i=0; i< m_index.size();i++){
        if(m_index[i] == row)
            return i;
    }
    return 0;
}

bool ManualDiscoveryViewModel::setCellValue(int rowNum, int columnNum, const QVariant &data)
{
    if(rowNum < 0 || rowNum >= m_data.size())
        return false;

    m_data[rowNum][columnNum] = data;
    m_roles.clear();
    m_roles << Qt::DisplayRole;

    auto modelIndex = QModelIndex(index(getCurrentRow(rowNum), columnNum));
    emit dataChanged(modelIndex, modelIndex, m_roles);
    return true;
}

void ManualDiscoveryViewModel::startP2pRegister(QString productId, QString id, QString password, bool usageDtlsMode)
{
    SPDLOG_DEBUG("ManualDiscoveryViewModel::startP2pRegister productId:{} id:{} usageDtlsMode:{}", productId.toStdString(), id.toStdString(), usageDtlsMode);

    emit monitoringPageRegistering(true);
    auto request = std::make_shared<Wisenet::Core::AddDeviceRequest>();
    request->deviceID = QCoreServiceManager::Instance().CreateNewUUidString();
    request->connectionInfo.connectionType = Wisenet::Device::ConnectionType::WisenetDDNS;

    request->connectionInfo.host = productId.trimmed().toStdString();
    request->connectionInfo.user = id.trimmed().toStdString();
    request->connectionInfo.password = password.trimmed().toStdString();
    request->connectionInfo.usageDtlsMode = usageDtlsMode;

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::AddDevice,
                this, request,
                [this](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::Device::DeviceConnectResponse>(reply->responseDataPtr);
        if (response->isFailed()) {
            emit connectFailDevice(QString::fromStdString(response->errorString()));
        }
        else{
            if(response->device.connectionInfo.connectedType == Wisenet::Device::ConnectedType::P2P_RELAY)
                emit connectFailDeviceRelayMode();
        }
        emit monitoringPageRegistering(false);
    });
}

void ManualDiscoveryViewModel::startS1Register(QString productId, QString id, QString password, bool isSIP)
{
    SPDLOG_DEBUG("ManualDiscoveryViewModel::startS1Register productId:{} id:{} isSIP:{}", productId.toStdString(), id.toStdString(), isSIP);

    emit monitoringPageRegistering(true);
    auto request = std::make_shared<Wisenet::Core::AddDeviceRequest>();
    request->deviceID = QCoreServiceManager::Instance().CreateNewUUidString();
    request->connectionInfo.connectionType = isSIP ? Wisenet::Device::ConnectionType::S1_SIP : Wisenet::Device::ConnectionType::S1_DDNS;

    request->connectionInfo.host = productId.trimmed().toStdString();
    request->connectionInfo.user = id.trimmed().toStdString();
    request->connectionInfo.password = password.trimmed().toStdString();

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::AddDevice,
                this, request,
                [this](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::Device::DeviceConnectResponse>(reply->responseDataPtr);
        if (response->isFailed()) {
            emit connectFailDevice(QString::fromStdString(response->errorString()));
        }
        else{
            if(response->device.connectionInfo.connectedType == Wisenet::Device::ConnectedType::S1_SIP_RELAY)
                emit connectFailDeviceRelayMode();
        }
        emit monitoringPageRegistering(false);
    });
}

void ManualDiscoveryViewModel::registerDevice(
        QVector<Wisenet::Device::DeviceConnectInfo> devices)
{
    m_data.clear();
    m_index.clear();

    m_currentTryRegisterCount = 0;
    m_registerComplete= true;

    m_totalRegisterCount = 0;

    beginResetModel();
    for (auto &device : devices) {
        QVector<QVariant> display;

        display.push_back(QString::fromStdString(device.host));
        if(device.connectionType == Wisenet::Device::ConnectionType::WisenetDDNS)
            display.push_back("");
        else
            display.push_back(device.port);
        display.push_back(QString::fromStdString("Registering"));
        m_data.push_back(display);
        m_index.push_back(m_index.size());
    }
    endResetModel();

    m_roles.clear();
    m_roles << Qt::DisplayRole;

    int row = 0;
    bool bFirst = true;
    for (auto &device : devices) {
        auto request = std::make_shared<Wisenet::Core::AddDeviceRequest>();
        request->deviceID = QCoreServiceManager::Instance().CreateNewUUidString();
        request->connectionInfo.connectionType = device.connectionType;
        request->connectionInfo.host = device.host;
        request->connectionInfo.port = device.port;
        request->connectionInfo.isSSL = device.isSSL;
        request->connectionInfo.usageDtlsMode = device.usageDtlsMode;
        request->connectionInfo.mac = device.mac;

        if(request->connectionInfo.isSSL)
            request->connectionInfo.sslPort = device.sslPort;

        request->connectionInfo.user = device.user;
        request->connectionInfo.password = device.password;
        if(device.connectionType == Wisenet::Device::ConnectionType::StaticIPv4 &&
                QCoreServiceManager::Instance().DB()->ContainsDeviceByIp(device.host, device.port)){
            setCellValue(row, ManualRegisterStatus, QString("Already Registered"));
            SPDLOG_DEBUG("Already Exist {} {}", row, device.host);
        }
        else if (device.connectionType == Wisenet::Device::ConnectionType::StaticIPv6 &&
                 QCoreServiceManager::Instance().DB()->ContainsDeviceByIp(device.host, device.port)) {
            setCellValue(row, ManualRegisterStatus, QString("Already Registered"));
            SPDLOG_DEBUG("Already Exist {} {}", row, device.host);
        }
        else{

            if(bFirst){
                bFirst = false;
                emit deviceRegistering();
            }

            m_totalRegisterCount++;
            QCoreServiceManager::Instance().RequestToCoreService(
                        &Wisenet::Core::ICoreService::AddDevice,
                        this, request,
                        [this, row](const QCoreServiceReplyPtr& reply)
            {
                auto response = std::static_pointer_cast<Wisenet::Device::DeviceConnectResponse>(reply->responseDataPtr);
                if (response->isFailed()) {
                    setCellValue(row, ManualRegisterStatus, QString::fromStdString(response->errorString()));
                    m_registerComplete = false;
                }
                else {
                    setCellValue(row, ManualRegisterStatus, QString("Success"));
                    SPDLOG_DEBUG("Already Exist {} {}", row, "registered");
                }

                m_currentTryRegisterCount++;

                if(m_totalRegisterCount == m_currentTryRegisterCount){
                    SPDLOG_DEBUG("Register completed = {}", m_registerComplete);
                    emit deviceRegisterCompleted(m_registerComplete);
                }
                if(response->isSuccess() && response->device.connectionInfo.connectedType == Wisenet::Device::ConnectedType::P2P_RELAY)
                    emit connectFailDeviceRelayModeManual();
            });
        }
        row++;
    }
}

void ManualDiscoveryViewModel::staticDevice(bool isSSL, std::string id, std::string password,
                                            std::string startIP, std::string endIP, int port)
{
    QVector<Wisenet::Device::DeviceConnectInfo> devices;
    int start = GetLastOctet(startIP);

    std::stringstream sstream(endIP);
    int a,b,c,d; //to store the 4 ints
    char ch; //to temporarily store the '.'
    sstream >> a >> ch >> b >> ch >> c >> ch >> d ;

    int end = d;
    std::stringstream stream;

    SPDLOG_DEBUG("start = {} end= {}", start, end);
    for (auto i = start; i <= end; i++) {
        Wisenet::Device::DeviceConnectInfo device;
        stream.str("");
        stream << a << ch << b << ch << c << ch << i;
        device.connectionType = Wisenet::Device::ConnectionType::StaticIPv4;
        device.host = stream.str();
        device.isSSL = isSSL;
        device.port = port;
        if(device.isSSL)
            device.sslPort = port;
        device.user = id;
        device.password = password;

        devices.push_back(device);
    }
    registerDevice(devices);
}

void ManualDiscoveryViewModel::staticIPv6Device(bool isSSL, std::string id, std::string password,
                                                std::string startIP, int port)
{
    QVector<Wisenet::Device::DeviceConnectInfo> devices;

    Wisenet::Device::DeviceConnectInfo device;
    device.connectionType = Wisenet::Device::ConnectionType::StaticIPv6;
    device.host = startIP;
    device.isSSL = isSSL;
    device.port = port;
    if (device.isSSL)
        device.sslPort = port;
    device.user = id;
    device.password = password;

    devices.push_back(device);

    registerDevice(devices);
}

void ManualDiscoveryViewModel::ddnsUrlDevice(bool isSSL, Wisenet::Device::ConnectionType type, std::string id, std::string password, std::string host, int port, bool dtls)
{
    beginResetModel();

    QVector<Wisenet::Device::DeviceConnectInfo> devices;
    Wisenet::Device::DeviceConnectInfo device;
    device.host = host;
    device.isSSL = isSSL;
    if(device.isSSL)
        device.sslPort = port;
    device.connectionType = type;
    device.port = port;
    device.user = id;
    device.password = password;
    device.usageDtlsMode = dtls;

    devices.push_back(device);

    SPDLOG_DEBUG("ip = {}", device.host);
    endResetModel();
    registerDevice(devices);
}

void ManualDiscoveryViewModel::reset()
{
    SPDLOG_DEBUG("ManualDiscoveryViewModel::reset()");
    m_index.clear();
    m_data.clear();
    m_highlightRows.clear();
    m_highlightRow = -1;
    m_lastHighlightRow = -1;

    beginResetModel();
    endResetModel();
}

void ManualDiscoveryViewModel::startRegister( QString _type, QString _id, QString _password,
                                              QString _host, QString _endIP, QString _port, bool _usageDtlsMode)
{
    std::string id = _id.trimmed().toStdString();
    std::string password = _password.trimmed().toStdString();
    std::string host = _host.trimmed().toStdString();

    int port = _port.toUInt();

    m_data.clear();
    m_index.clear();

    m_highlightRows.clear();
    m_highlightRow = -1;
    m_lastHighlightRow = -1;

    std::string endIP = _endIP.toStdString();

    if(_type =="IPv4")
        staticDevice(false, id, password, host, endIP, port);
    else if(_type =="IPv4+SSL")
        staticDevice(true, id, password, host, endIP, port);
    else if(_type =="IPv6")
        staticIPv6Device(false, id, password, host, port);
    else if(_type =="IPv6+SSL")
        staticIPv6Device(true, id, password, host, port);
    else if(_type == "S1 DDNS")
        ddnsUrlDevice(false, Wisenet::Device::ConnectionType::S1_DDNS, id, password, host, port, _usageDtlsMode);
    else if(_type == "S1 SIP")
        ddnsUrlDevice(false, Wisenet::Device::ConnectionType::S1_SIP, id, password, host, port, _usageDtlsMode);
    else {
        Wisenet::Device::ConnectionType type = (_type == "DDNS/P2P" ) ? Wisenet::Device::ConnectionType::WisenetDDNS :
                                                                        Wisenet::Device::ConnectionType::HttpUrl;
        bool isSSL =  (_type =="URL+SSL")?true:false;

        ddnsUrlDevice(isSSL, type ,id, password, host, port, _usageDtlsMode);
    }
}

void ManualDiscoveryViewModel::cancelRegister()
{
    SPDLOG_DEBUG("Cancel to register");

}

QVariant ManualDiscoveryViewModel::getDataDisplayRole(const QModelIndex &index) const
{
    int row = m_index[index.row()];

    const QVariant& cellValue = m_data[row][index.column()];

    if(cellValue.isNull())
        return QVariant(QVariant::String);

    return cellValue;
}

void ManualDiscoveryViewModel::UpdateSortData(int column, Qt::SortOrder order)
{
    m_sorted.clear();

    m_sorted.fill(0, m_data.size());
    std::iota(m_sorted.begin(), m_sorted.end(), 0);

    std::sort(m_sorted.begin(),m_sorted.end(),[this, order,column](int indexA, int indexB){
        const QVariant& valueA = m_data[indexA][column];
        const QVariant& valueB = m_data[indexB][column];

        if(Qt::SortOrder::AscendingOrder == order)
            return valueA.toString().compare(valueB.toString(), Qt::CaseInsensitive) < 0;
        else
            return valueA.toString().compare(valueB.toString(), Qt::CaseInsensitive) > 0;
    });
}
