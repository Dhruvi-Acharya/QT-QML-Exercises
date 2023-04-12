/*********************************************************************************
 * Copyright(c) 2023 Hanwha Vision Co., Ltd.  All rights reserved.
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
#include "AutoDiscoveryViewModel.h"
#include "LogSettings.h"
#include <QQmlEngine>
#include <set>
#include <QThread>
#include <QtConcurrent>
#include "../SetupCommon.h"
#include <boost/algorithm/string.hpp>


AutoDiscoveryViewModel::AutoDiscoveryViewModel()
    :m_isScanning(false), m_isIpv6Mode(false)

{
    m_columnNames << "Check" << "Model" <<"IsHttps" <<"Ip" << "Port" << "Mac address" << "Status";

    SPDLOG_DEBUG("AutoDiscoveryViewModel()");
}

AutoDiscoveryViewModel::~AutoDiscoveryViewModel()
{
    //clearData(true);
    SPDLOG_DEBUG("qml: ~AutoDiscoveryViewModel()");
}

int AutoDiscoveryViewModel::rowCount(const QModelIndex &) const
{
    return m_index.size();
}

int AutoDiscoveryViewModel::columnCount(const QModelIndex &) const
{
    return m_columnNames.size();
}

QVariant AutoDiscoveryViewModel::data(const QModelIndex &index, int role) const
{
    if(index.row() < 0 || index.row() > m_data.size())
        return QVariant();

    switch(role) {
    case Qt::DisplayRole:
        return getDataDisplayRole(index);
    case HighlightRole:
        if( m_highlightRows.count( index.row() ) )
            return true;
        return false;
    case HoveredRole:
        if(m_hoveredRow == index.row())
            return true;
        return false;
    default:
        break;
    }

    return QVariant();
}

QHash<int, QByteArray> AutoDiscoveryViewModel::roleNames() const
{
    return { {Qt::DisplayRole, "display"}, {HighlightRole, "highlightRole"}, {HoveredRole, "hoveredRole"}};
}

void AutoDiscoveryViewModel::sort(int column, Qt::SortOrder order)
{
    Q_UNUSED(column);
    updateSortData(column, order);
    beginResetModel();
    m_index = m_sorted;
    endResetModel();
}

void AutoDiscoveryViewModel::setHighlightRow(int rowNum, int modifiers)
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
            if((!pressedCtrl && !pressedShift)) {
                if(!m_highlightRows.empty()) {
                    minRow = *m_highlightRows.begin();
                    maxRow = *m_highlightRows.rbegin();
                    m_highlightRows.clear();
                }
            }
            if(!pressedShift || m_lastHighlightRow == -1){
                m_highlightRows.insert(rowNum);
            }
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
        emit highlightRowCountChanged();
        emit selectedRowCountChanged();

        if(minRow != -1) {
            QVector<int> role;
            role << Role::HighlightRole;
            emit dataChanged(QModelIndex(index(minRow, 0)), QModelIndex(index(maxRow, columnCount() -1)), role);
        }
    }
}

void AutoDiscoveryViewModel::setHoveredRow(int rowNum, bool hovered)
{
    QVector<int> role;
    role << Role::HoveredRole;

    int last = m_hoveredRow;
    if(hovered){
        m_hoveredRow = rowNum;
        if(last > m_hoveredRow)
            emit dataChanged(QModelIndex(index(m_hoveredRow, 0)), QModelIndex(index(last, columnCount() -1)), role);
        else
            emit dataChanged(QModelIndex(index(last, 0)), QModelIndex(index(m_hoveredRow, columnCount() -1)), role);
    }
    else{
        if(rowNum == m_hoveredRow){
            m_hoveredRow = -1;
            emit dataChanged(QModelIndex(index(rowNum, 0)), QModelIndex(index(rowNum, columnCount() -1)), role);
        }
    }
}

int AutoDiscoveryViewModel::highlightRow() const
{
    return m_highlightRow;
}

QVector<int> AutoDiscoveryViewModel::getHighlightRows() const
{
    QVector<int> toRet;
    for(auto curRow: m_highlightRows) toRet.push_back(curRow);
    return toRet;
}

QVariant AutoDiscoveryViewModel::getDataDisplayRole(const QModelIndex &index) const
{
    int row = m_index[index.row()];

    const QVariant& cellValue = m_data[row][index.column()];

    if(cellValue.isNull())
        return QVariant();

    return cellValue;
}
bool AutoDiscoveryViewModel::scanningStatus()
{
    return m_isScanning;
}

bool AutoDiscoveryViewModel::ipv6Mode()
{
    return m_isIpv6Mode;
}

int AutoDiscoveryViewModel::conflictIpCount()
{
    return m_conflictIpCount;
}

int AutoDiscoveryViewModel::needPasswordCount()
{
    return m_needPasswordCount;
}

void AutoDiscoveryViewModel::setScanningStatus(bool scanning)
{
    if (m_isScanning != scanning) {
        m_isScanning = scanning;
        emit scanningStatusChanged(scanning);
    }
}

void AutoDiscoveryViewModel::setIpv6Mode(bool isIpv6)
{
    if (m_isIpv6Mode != isIpv6) {
        m_isIpv6Mode = isIpv6;
        emit ipv6ModeChanged(isIpv6);
    }
}

void AutoDiscoveryViewModel::setConflictIpCount(int count)
{
    if (m_conflictIpCount != count) {
        m_conflictIpCount = count;
        emit conflictIpCountChanged(count);
    }
}

void AutoDiscoveryViewModel::setNeedPasswordCount(int count)
{
    if (m_needPasswordCount != count) {
        m_needPasswordCount = count;
        emit needPasswordCountChanged(count);
    }
}

void AutoDiscoveryViewModel::reset()
{
    SPDLOG_DEBUG("AutoDiscoveryViewModel::reset()");
    cancelDiscovery();
    m_index.clear();
    m_discoveredDevices.clear();
    m_data.clear();
    m_highlightRows.clear();
    m_highlightRow = -1;
    m_lastHighlightRow = -1;
    m_hoveredRow = -1;
    setConflictIpCount(0);
    setNeedPasswordCount(0);
    setDeviceCount(0);

    beginResetModel();
    endResetModel();
}

static void autoRegisterWorker(AutoDiscoveryViewModel* model, std::unordered_map<std::string, Wisenet::Core::DiscoveryDeviceInfo>& discoveredDevices, QString id, QString password)
{
    SPDLOG_DEBUG("AutoDiscoveryViewModel::onDeviceDiscoveryRegister() {} ",discoveredDevices.size());
    model->m_registerComplete = false;
    model->m_currentTryRegisterCount = 0;
    model->m_totalRegisterCount = (int)discoveredDevices.size();

    for(auto& device : discoveredDevices){
        auto request = std::make_shared<Wisenet::Core::AddDeviceRequest>();
        request->deviceID = QCoreServiceManager::Instance().CreateNewUUidString();

        request->connectionInfo.isSSL = device.second.isHttpsEnable;
        request->connectionInfo.host = device.second.ipAddress;

        request->connectionInfo.port = device.second.httpPort;
        request->connectionInfo.sslPort = device.second.httpsPort;
        request->connectionInfo.user = id.toStdString();
        request->connectionInfo.password = password.toStdString();
        request->connectionInfo.mac = boost::erase_all_copy(device.second.macAddress, ":");
        request->connectionForceInfo.forceRegister = true;
        request->connectionForceInfo.modelName = device.second.model;
        request->connectionForceInfo.macAddress = device.second.macAddress;
        request->connectionForceInfo.deviceType = device.second.deviceType;
    SPDLOG_DEBUG("AutoDiscoveryViewModel::onDeviceDiscoveryRegister() ip:{} ssl:{} http:{} https:{}",device.second.ipAddress, device.second.isHttpsEnable, device.second.httpPort, device.second.httpsPort);
        QCoreServiceManager::Instance().RequestToCoreService(
                    &Wisenet::Core::ICoreService::AddDevice,
                    model, request,
                    [model](const QCoreServiceReplyPtr& reply)
        {
            Q_UNUSED(reply);

            auto response = std::static_pointer_cast<Wisenet::Device::DeviceConnectResponse>(reply->responseDataPtr);
            //SPDLOG_DEBUG("AutoDiscoveryViewModel::startRegister result selectedRow={}", selectedRow);
            if (response->isFailed()) {
                if(!(response->errorCode == Wisenet::ErrorCode::AccountBlocked || response->errorCode == Wisenet::ErrorCode::UnAuthorized) ){
                    model->m_registerComplete = false;
                }
            }
            model->m_currentTryRegisterCount++;
            if(model->m_totalRegisterCount == model->m_currentTryRegisterCount){
                emit model->monitoringPageRegistering(false);
                if(model->m_registerComplete == false)
                    emit model->connectFailDeviceFound();
            }
        });
    }
    discoveredDevices.clear();
}

void AutoDiscoveryViewModel::startDiscoveryAndRegister(QString type, QString id, QString password)
{
    SPDLOG_DEBUG("startDiscoveryAndRegister:{}",type.toStdString());
    emit monitoringPageRegistering(true);
    setScanningStatus(true);
    m_discoveredDevices.clear();
    auto request = std::make_shared<Wisenet::Core::DiscoverDevicesRequest>();

    Wisenet::Device::DeviceType deviceType = Wisenet::Device::DeviceType::SunapiCamera;
    if(type == QString("Nvr"))
        deviceType = Wisenet::Device::DeviceType::SunapiRecorder;

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::DiscoverDevices,
                this, request,
                [this, deviceType, id, password](const QCoreServiceReplyPtr& reply)
    {
        if (!m_isScanning) {
            return;
        }

        auto response = std::static_pointer_cast<Wisenet::Core::DiscoverDevicesResponse>(reply->responseDataPtr);
        for (auto& item : response->discoveredDevices) {
            auto itr = m_discoveredDevices.find(item.macAddress);
            if (itr == m_discoveredDevices.end()) {

                Wisenet::Device::DeviceType tempType = item.deviceType;
                if(tempType == Wisenet::Device::DeviceType::SunapiEncoder)
                    tempType = Wisenet::Device::DeviceType::SunapiCamera;

                if(deviceType != tempType){
                    continue;
                }
                if(item.needToPasswordConfig)
                    continue;
                if(QCoreServiceManager::Instance().DB()->ContainsDeviceByMacAddress(item.macAddress))
                    continue;;

                m_discoveredDevices[item.macAddress] = item;
            }
        }

        if (!response->isContinue) {
            setScanningStatus(false);
            if(m_discoveredDevices.size() > 0){
                SPDLOG_DEBUG("DISCOVERY RESPONSE map count={}", m_discoveredDevices.size());

                QFutureWatcher<void> watcher;
                QObject::connect(&watcher, SIGNAL(finished()), this, SLOT(quit()));
                QFuture<void> result = QtConcurrent::run(autoRegisterWorker, this, m_discoveredDevices, id, password);
                watcher.setFuture(result);
            }
            else
                emit monitoringPageRegistering(false);
        }
    });
}

void AutoDiscoveryViewModel::clearData(bool clear){
    if(clear){
        m_discoveredDevices.clear();
        m_data.clear();
        m_index.clear();
        setConflictIpCount(0);
        setNeedPasswordCount(0);
    }
}

void AutoDiscoveryViewModel::startDiscovery(bool clear)
{
    setScanningStatus(true);
    clearData(clear);

    SPDLOG_DEBUG("AutoDiscoveryViewModel::startDiscovery() start:{} m_isIpv6Mode:{}",clear, m_isIpv6Mode.load());

    auto discoveryFp = &Wisenet::Core::ICoreService::DiscoverDevices;
    if(m_isIpv6Mode.load())
        discoveryFp = &Wisenet::Core::ICoreService::DiscoverIPv6Devices;

    auto request = std::make_shared<Wisenet::Core::DiscoverDevicesRequest>();
    QCoreServiceManager::Instance().RequestToCoreService(
                discoveryFp,
                this, request,
                [this](const QCoreServiceReplyPtr& reply)
    {
        if (!m_isScanning) {
            /* already canceled, ignore all responses.. */
            return;
        }

        SPDLOG_DEBUG("AutoDiscoveryViewModel::startDiscovery() reply()");


        auto response = std::static_pointer_cast<Wisenet::Core::DiscoverDevicesResponse>(reply->responseDataPtr);
        for (auto& item : response->discoveredDevices) {
            auto itr = m_discoveredDevices.find(item.macAddress);
#ifndef SUNAPI_CAMERA_SUPPORT
            if(item.deviceType != Wisenet::Device::DeviceType::SunapiRecorder)
                continue;
#endif
            if(item.ipVersion == Wisenet::Core::InternetProtocolVersion::IPv4 && m_isIpv6Mode.load())
                continue;
            if(item.ipVersion == Wisenet::Core::InternetProtocolVersion::IPv6 && !m_isIpv6Mode.load())
                continue;

            if (itr == m_discoveredDevices.end()) {
                QVector<QVariant> display;
                QString status = "";
                if(QCoreServiceManager::Instance().DB()->ContainsDeviceByMacAddress(item.macAddress)){
                    continue;
                }
                if(item.needToPasswordConfig)
                    status = "InitDevicePassword";

                display.push_back(item.isHttpsEnable);
                display.push_back(false);
                display.push_back(QString::fromStdString(item.model));
                display.push_back(QString::fromStdString(item.ipAddress));
                display.push_back(QString::fromStdString(item.macAddress));

                if(item.isHttpsEnable)
                    display.push_back(static_cast<unsigned int>(item.httpsPort));
                else
                    display.push_back(static_cast<unsigned int>(item.httpPort));


                SPDLOG_DEBUG("Discovery MAC:{} IP:{} Port:{} Https:{}",
                             item.macAddress, item.ipAddress, item.httpPort, item.isHttpsEnable);

                display.push_back(status);

                int i = m_data.size();
                beginInsertRows(QModelIndex(), i, i);

                m_data.push_back(display);
                m_index.push_back(m_index.size());
                m_discoveredDevices[item.macAddress] = item;
                endInsertRows();
            }
        }

        if (m_discoveredDevices.size() > 0) {
            setDeviceCount((int)m_discoveredDevices.size());
        }

        if (!response->isContinue) {
            m_needPasswordCount = 0;
            m_conflictIpCount= 0;

            std::set<std::string> conflictList;
            for (auto& device : m_discoveredDevices){
                for(auto & device2 : m_discoveredDevices){
                    if(device.first == device2.first)
                        continue;
                    if(device.second.ipAddress != device2.second.ipAddress)
                        continue;
                    if(device.second.isHttpsEnable != device2.second.isHttpsEnable)
                        continue;
                    if(device.second.isHttpsEnable && device.second.httpsPort != device2.second.httpsPort)
                        continue;
                    if(!device.second.isHttpsEnable && device.second.httpPort != device2.second.httpPort)
                        continue;
                    conflictList.insert(device2.first);
                }
            }
            for (int dataIndex = 0; dataIndex < m_data.size();dataIndex++) {
                auto mac = m_data[dataIndex][AutoDiscoveryMac].toString().toStdString();
                auto iter = conflictList.find(mac);
                if(m_data[dataIndex][AutoDiscoveryStatus] == "InitDevicePassword"){
                    m_needPasswordCount++;
                    continue;
                }

                if(iter != conflictList.end()){
                    m_data[dataIndex][AutoDiscoveryStatus] =  "IpConflict";
                }
            }
            m_conflictIpCount = conflictList.size();
            emit conflictIpCountChanged(m_conflictIpCount);
            emit needPasswordCountChanged(m_needPasswordCount);
            setScanningStatus(false);
        }

        m_roles.clear();
        m_roles << Qt::DisplayRole;
    });
}

void AutoDiscoveryViewModel::updateSortData(int column, Qt::SortOrder order)
{
    m_sorted.clear();
    m_sorted.fill(0, m_data.size());
    std::iota(m_sorted.begin(), m_sorted.end(), 0);

    std::sort(m_sorted.begin(),m_sorted.end(),[this, order,column](int indexA, int indexB){
        const QVariant& valueA = m_data[indexA][column];
        const QVariant& valueB = m_data[indexB][column];

        // ip sort
        if (column == AutoDiscoveryTitle::AutoDiscoveryIp) {
            uint a = ipv4StringToUint(valueA.toString());
            uint b = ipv4StringToUint(valueB.toString());
            if(Qt::SortOrder::AscendingOrder == order)
                return a > b;
            return a < b;
        }
        else {
            if(Qt::SortOrder::AscendingOrder == order)
                return valueA.toString().compare(valueB.toString(), Qt::CaseInsensitive) < 0;
            else
                return valueA.toString().compare(valueB.toString(), Qt::CaseInsensitive) > 0;
        }
    });
}

bool AutoDiscoveryViewModel::setCheckState(int rowNum, bool checked)
{
    SPDLOG_DEBUG("setCheckState row:{} checked:{}",rowNum, checked);

    int row = m_index[rowNum];
    m_data[row][AutoDiscoveryCheck] = !checked;
    emit dataChanged(QModelIndex(index(rowNum, AutoDiscoveryCheck)), QModelIndex(index(rowNum, AutoDiscoveryCheck)));

    return true;
}

bool AutoDiscoveryViewModel::setAllCheckState(bool checked)
{
    SPDLOG_DEBUG("setAllCheckState checked:{}",checked);

    for(auto& item : m_index)
        m_data[item][AutoDiscoveryCheck] = checked;
    emit dataChanged(QModelIndex(index(0, AutoDiscoveryCheck)), QModelIndex(index(m_index.length()-1, AutoDiscoveryCheck)));

    return true;
}

void AutoDiscoveryViewModel::cancelDiscovery()
{
    SPDLOG_DEBUG("Cancel to discovery");
    m_isScanning = false;
}

void AutoDiscoveryViewModel::startRegister(QString id, QString password)
{
    SPDLOG_DEBUG("Start to register selected devices");

    m_currentTryRegisterCount = 0;
    m_registerComplete= true;
    
    m_totalRegisterCount = 0;

    bool bFirst = true;
    for (int dataIndex = 0; dataIndex < m_data.size();dataIndex++) {
        if(m_data[dataIndex][AutoDiscoveryCheck].toBool() == false)
            continue;


        setCellValue(dataIndex, AutoDiscoveryStatus, "Registering");
        QString mac = m_data[dataIndex][AutoDiscoveryMac].toString();

        auto itr = m_discoveredDevices.find(mac.toStdString());
        if (itr != m_discoveredDevices.end())
        {
            auto device = itr->second;

            if(QCoreServiceManager::Instance().DB()->ContainsDeviceByMacAddress(device.macAddress)){
                setCellValue(dataIndex, AutoDiscoveryStatus, "Registered");
                continue;
            }

            if(bFirst){
                bFirst = false;
                emit deviceRegistering();
            }

            auto request = std::make_shared<Wisenet::Core::AddDeviceRequest>();
            request->deviceID = QCoreServiceManager::Instance().CreateNewUUidString();
            request->connectionInfo.connectionType = (device.ipVersion == Wisenet::Core::InternetProtocolVersion::IPv4)? 
                Wisenet::Device::ConnectionType::StaticIPv4: Wisenet::Device::ConnectionType::StaticIPv6;
            request->connectionInfo.host = device.ipAddress;
            request->connectionInfo.isSSL = device.isHttpsEnable;
            request->connectionInfo.sslPort = device.httpsPort;
            request->connectionInfo.port = device.httpPort;
            request->connectionInfo.user = id.toStdString();
            request->connectionInfo.password = password.toStdString();
            request->connectionInfo.mac = boost::erase_all_copy(device.macAddress, ":");

            m_totalRegisterCount++;

            QCoreServiceManager::Instance().RequestToCoreService(
                        &Wisenet::Core::ICoreService::AddDevice,
                        this, request,
                        [this, dataIndex](const QCoreServiceReplyPtr& reply)
            {
                auto response = std::static_pointer_cast<Wisenet::Device::DeviceConnectResponse>(reply->responseDataPtr);
                //SPDLOG_DEBUG("AutoDiscoveryViewModel::startRegister result selectedRow={}", selectedRow);
                if (response->isFailed()) {
                    setCellValue(dataIndex, AutoDiscoveryStatus, QString::fromStdString(response->errorString()));
                    m_registerComplete = false;
                }
                else {
                    setCellValue(dataIndex, AutoDiscoveryStatus, "Success");
                }
                m_currentTryRegisterCount++;

                if(m_totalRegisterCount == m_currentTryRegisterCount){
                    SPDLOG_DEBUG("Register completed = {}", m_registerComplete);
                    emit deviceRegisterCompleted(m_registerComplete);
                }
            });
        }
    }
}

void AutoDiscoveryViewModel::cancelRegister()
{
    SPDLOG_DEBUG("Cancel to register");
}

void AutoDiscoveryViewModel::startInitPasswordSetting(QString password)
{
    SPDLOG_DEBUG("Start to InitPasswordSetting selected devices");
    for (int dataIndex = 0; dataIndex < m_data.size();dataIndex++) {
        if(m_data[dataIndex][AutoDiscoveryCheck].toBool() == false)
            continue;
        setCellValue(dataIndex, AutoDiscoveryStatus, "InitDevicePasswordChanging");

        QString mac = m_data[dataIndex][AutoDiscoveryMac].toString();

        auto itr = m_discoveredDevices.find(mac.toStdString());
        if (itr != m_discoveredDevices.end())
        {
            auto device = itr->second;
            auto request = std::make_shared<Wisenet::Core::InitDevicePasswordRequest>();
            request->macAddress = device.macAddress;
            request->password = password.toStdString();

            QCoreServiceManager::Instance().RequestToCoreService(
                        &Wisenet::Core::ICoreService::InitDevicePassword,
                        this, request,
                        [this, dataIndex](const QCoreServiceReplyPtr& reply)
            {
                auto response = std::static_pointer_cast<Wisenet::ResponseBase>(reply->responseDataPtr);
                if (response->isFailed()) {
                    setCellValue(dataIndex, AutoDiscoveryStatus, QString::fromStdString(response->errorString()));
                }
                else {
                    setCellValue(dataIndex, AutoDiscoveryStatus, "Success");
                }
            });
        }
    }
}

void AutoDiscoveryViewModel::updateNonce()
{
    std::set<std::string> updatingNonceDevice;

    for (int dataIndex = 0; dataIndex < m_data.size();dataIndex++) {
        if(m_data[dataIndex][AutoDiscoveryCheck].toBool() == false)
            continue;
        QString mac = m_data[dataIndex][AutoDiscoveryMac].toString();

        auto itr = m_discoveredDevices.find(mac.toStdString());
        if (itr != m_discoveredDevices.end()  && itr->second.supportPasswordEncryption){
            updatingNonceDevice.insert(itr->first);
        }
    }

    if(updatingNonceDevice.size() > 0){
        setScanningStatus(true);

        InitializeNotify();
        auto request = std::make_shared<Wisenet::Core::DiscoverDevicesRequest>();
        QCoreServiceManager::Instance().RequestToCoreService(
                    &Wisenet::Core::ICoreService::DiscoverDevices,
                    this, request,
                    [this, updatingNonceDevice](const QCoreServiceReplyPtr& reply)
        {
            if (!m_isScanning) {
                return;
            }
            auto localNonce = updatingNonceDevice;

            auto response = std::static_pointer_cast<Wisenet::Core::DiscoverDevicesResponse>(reply->responseDataPtr);
            for (auto& item : response->discoveredDevices) {

                auto itr = localNonce.find(item.macAddress);
                if (itr != localNonce.end()) {

                    SPDLOG_DEBUG("update nonce mac={} ip={} nonce={}", item.macAddress, item.ipAddress, item.passwordNonce);
                    m_discoveredDevices[item.macAddress].passwordNonce = item.passwordNonce;
                    localNonce.erase(item.macAddress);
                }
            }
            if(localNonce.size() == 0){
                setScanningStatus(false);
                Notify(true);
            }
        });
        Wait(5);
    }
}

QString calculateIp(QString ip, int count){

    auto tokens = ip.split(".");
    QVector<int> numberTokens;
    for(auto& item : tokens){
        numberTokens.push_back(item.toInt());
    }
    numberTokens[3] += count;
    //console.log(numberTokens[3])

    int maxByte = 256;
    int maxIp = 255;

    if (numberTokens[3] > maxIp)
    {
        numberTokens[2] = numberTokens[2] + numberTokens[3] / maxByte;
        numberTokens[3] = numberTokens[3] % maxByte;

        if (numberTokens[2] > maxIp)
        {
            numberTokens[1] = numberTokens[1] + numberTokens[2] / maxByte;
            numberTokens[2] = numberTokens[2] % maxByte;
            if (numberTokens[1] > maxIp)
            {
                numberTokens[0] = numberTokens[0] + numberTokens[1] / maxByte;
                numberTokens[1] = numberTokens[1] % maxByte;
            }
        }
    }

    QString ret = QString::number(numberTokens[0]) + "." +
            QString::number(numberTokens[1]) + "." +
            QString::number(numberTokens[2]) + "." +
            QString::number(numberTokens[3]);

    SPDLOG_DEBUG("calculateIp result {}", ret.toStdString());
    return ret;

}

void AutoDiscoveryViewModel::startIpSetting( bool dhcp, QString id, QString password, QString port, QString startIp, QString subnet, QString gateway)
{
    SPDLOG_DEBUG("Start to startIpSetting selected devices");

    updateNonce();

    int count =0;
    for (int dataIndex = 0; dataIndex < m_data.size();dataIndex++) {
        if(m_data[dataIndex][AutoDiscoveryCheck].toBool() == false)
            continue;

        QString mac = m_data[dataIndex][AutoDiscoveryMac].toString();

        QString newIp = calculateIp(startIp, count++);

        auto itr = m_discoveredDevices.find(mac.toStdString());
        if (itr != m_discoveredDevices.end())
        {
            auto device = itr->second;
            auto request = std::make_shared<Wisenet::Core::IpSettingDeviceRequest>();
            request->ipSetting.macAddress = device.macAddress;
            request->ipSetting.password = password.toStdString();
            request->ipSetting.id = id.toStdString();

            if(dhcp){
                request->ipSetting.addressType = Wisenet::Core::DiscoverAddressType::DHCP;
                request->ipSetting.devicePort = device.devicePort;
                request->ipSetting.httpPort = port.toInt();
            }
            else{
                request->ipSetting.addressType = Wisenet::Core::DiscoverAddressType::StaticIP;
                request->ipSetting.ipAddress = newIp.toStdString();
                request->ipSetting.gatewayAddress = gateway.toStdString();
                request->ipSetting.subnetMaskAddress = subnet.toStdString();
                request->ipSetting.devicePort = device.devicePort;
                request->ipSetting.httpPort = port.toInt();
                if(device.supportPasswordEncryption){
                    request->ipSetting.passwordNonce = device.passwordNonce;
                    request->ipSetting.supportPasswordEncryption = device.supportPasswordEncryption;
                    SPDLOG_DEBUG("qml:startIpSetting() supportPasswordEncryption {} {}" ,device.supportPasswordEncryption, device.passwordNonce);
                }
            }
            QThread::msleep(500);
            QCoreServiceManager::Instance().RequestToCoreService(
                        &Wisenet::Core::ICoreService::IpSettingDevice,
                        this, request,
                        [this, dataIndex, newIp](const QCoreServiceReplyPtr& reply)
            {
                auto response = std::static_pointer_cast<Wisenet::ResponseBase>(reply->responseDataPtr);

                if (response->isFailed()) {
                    setCellValue(dataIndex, AutoDiscoveryStatus, QString::fromStdString(response->errorString()));
                }
                else {
                    setCellValue(dataIndex, AutoDiscoveryStatus, "Success");
                    setCellValue(dataIndex, AutoDiscoveryIp, newIp);

                    QString mac = m_data[dataIndex][AutoDiscoveryMac].toString();

                    if(QCoreServiceManager::Instance().DB()->ContainsDeviceByMacAddress(mac.toStdString())){
                        emit deviceIpChangeNeed(mac, newIp);
                    }

                }
            });
        }
    }
}

bool AutoDiscoveryViewModel::setCellValue(int rowNum, int columnNum, const QVariant &data)
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

bool AutoDiscoveryViewModel::checkInitDeviceSelect()
{
    for (int dataIndex = 0; dataIndex < m_data.size();dataIndex++) {
        if(m_data[dataIndex][AutoDiscoveryCheck].toBool() == false)
            continue;

        QString mac = m_data[dataIndex][AutoDiscoveryMac].toString();

        auto itr = m_discoveredDevices.find(mac.toStdString());
        if (itr != m_discoveredDevices.end() && itr->second.needToPasswordConfig){
            return true;
        }
    }
    return false;
}

int AutoDiscoveryViewModel::selectedRowCount(){
    int ret = 0;
    for (auto& item: m_data) {
        if(item[AutoDiscoveryCheck].toBool() == true)
            ret++;
    }
    return ret;
}

QString AutoDiscoveryViewModel::translate(QVariant display){
    std::string result = display.toString().toStdString();
    return QCoreApplication::translate("WisenetLinguist",result.c_str());

}

void AutoDiscoveryViewModel::InitializeNotify()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_canContinue = false;
    m_isSuccess = false;
}


void AutoDiscoveryViewModel::Notify(bool isSuccess)
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_canContinue = true;
        m_isSuccess = isSuccess;
    }
    m_condition.notify_one();
}

bool AutoDiscoveryViewModel::Wait(long timeoutSec)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    std::chrono::seconds second(timeoutSec);

    if (!m_condition.wait_for(lock, second, [this]() { return m_canContinue; })) {
        std::ostringstream msg;
        msg << "This request timed out. timeoutSec=" << second.count();

        return false;
    }

    if (!m_isSuccess) {

        return false;
    }

    return true;
}

QString AutoDiscoveryViewModel::getFirstItemIp(){
    for (auto& item: m_index) {
        if(m_data[item][AutoDiscoveryCheck].toBool() == true)
            return m_data[item][AutoDiscoveryIp].toString();
    }
    return "";
}
