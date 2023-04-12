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
#include "DeviceConfigViewModel.h"
#include <future>
#include "QCoreServiceManager.h"
#include "LogSettings.h"
#include "../SetupCommon.h"
#include <algorithm>
#include <QtConcurrent>

DeviceConfigViewModel::DeviceConfigViewModel(QObject* parent)
    : m_hoveredRow(-1),
      m_highlightRow(-1),
      m_lastHighlightRow(-1),
      m_viewType(DeviceConfigViewType::FirmwareUpdateViewType)
{
    SPDLOG_DEBUG("DeviceConfigViewModel::DeviceConfigViewModel - m_viewType={}",m_viewType);
    Q_UNUSED(parent);

    QStringList commonCol;
    commonCol << "Device Id" << "Check" << "Model" << "Device" ;

    m_columnNames << commonCol << "Channel Id" << "Ch Model" << "Ch Name" << "IP" << "Current Version"<< "Upgrade Version" << "Server Update"<<"Status" ;
    commonCol << "IP" ;

    m_columnConfigBackupNames << commonCol << "Version"<< "Status" ;
    m_columnChangePasswordNames << commonCol << "Status";
    m_paintRow = -1;
    m_lastPaintRow = -1;
}

int DeviceConfigViewModel::rowCount(const QModelIndex &) const
{
    return m_index.size();
}

int DeviceConfigViewModel::columnCount(const QModelIndex &) const
{
    if(m_viewType == PasswordChangeViewType)
        return m_columnChangePasswordNames.size();
    else if(m_viewType == ConfigBackupViewType)
        return m_columnConfigBackupNames.size();
    return m_columnNames.size();
}

QVariant DeviceConfigViewModel::data(const QModelIndex &index, int role) const
{
    if(index.row() < 0 || index.row() > m_data.count())
        return QVariant();

    //    SPDLOG_DEBUG("deviceConfigViewModel::data index.row={} index.column={} m_data.size={} role={}", index.row(), index.column(), m_data.size(), role);

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
    case PaintRole:
        if( m_paintRows.count( index.row() ) )
            return true;
        return false;
    case ServerUpdateRole:
        return getServerUpdateRole(index);
    default:
        break;
    }

    return QVariant();
}

QHash<int, QByteArray> DeviceConfigViewModel::roleNames() const
{
    return { {Qt::DisplayRole, "display"}, {HighlightRole, "highlightRole"}, {HoveredRole, "hoveredRole"}, {PaintRole, "paintRole"}, {ServerUpdateRole, "serverUpdateRole"}} ;
}

void DeviceConfigViewModel::sort(int column, Qt::SortOrder order)
{
    qDebug() << "DeviceConfigViewModel::sort";
    Q_UNUSED(column);
    UpdateSortData(column, order);
    beginResetModel();
    m_index = m_sorted;
    endResetModel();

    // 정렬할 때 마다 업데이트 필요한 장치 표시
    /*if(m_viewType == DeviceConfigViewType::FirmwareUpdateViewType)
    {
        m_paintRows.clear();
        m_paintRow = -1;
        m_lastPaintRow = -1;
        for (int dataIndex = 0; dataIndex < m_data.size(); ++dataIndex)
        {
            int rowNum = m_index[dataIndex];
            // setup 페이지의 테이블에서 녹화장비(ex.NVR)의 deviceId 가져옴
            QString deviceModelName = m_data[rowNum][DeviceConfigModel].toString();
            QString channelModelName = m_data[rowNum][DeviceConfigChannelModel].toString();
            QString currentVersion = m_data[rowNum][DeviceConfigVersion].toString();
            Wisenet::Core::FirmwareUpdateStatus isLatest = Wisenet::Core::FirmwareUpdateStatus::Latest;

            // NVR device
            if(channelModelName == "-")
            {
                isLatest = QCoreServiceManager::Instance().checkLatestVersion(deviceModelName, currentVersion);
            }
            else // Channel cam
            {
                isLatest = QCoreServiceManager::Instance().checkLatestVersion(channelModelName, currentVersion);
            }

            // 최신 펌웨어가 아니라면(NeedToUpdate) 페인트 표시
            if(isLatest == Wisenet::Core::FirmwareUpdateStatus::NeedToUpdate)
            {
                qDebug() << "device need to update - dataIndex" << dataIndex << "m_index[dataIndex]"<<m_index[dataIndex];
                setPaintRow(dataIndex, Qt::ControlModifier);
            }
            else
            {
                qDebug() << "device is already latest - dataIndex" << dataIndex << "m_index[dataIndex]"<<m_index[dataIndex];
            }
        }
    }*/
}

void DeviceConfigViewModel::setHighlightRow(int rowNum, int modifiers)
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

void DeviceConfigViewModel::setHoveredRow(int rowNum, bool hovered)
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

void DeviceConfigViewModel::setPaintRow(int rowNum, int modifiers)
{
    qDebug() << "DeviceConfigViewModel::setPaintRow - rowNum:" << rowNum;
    bool pressedCtrl = modifiers & Qt::ControlModifier;
    bool pressedShift = modifiers & Qt::ShiftModifier;

    bool fireRowChanged(false);
    int minRow(-1), maxRow(-1);

    if(m_paintRows.count(rowNum)) {
        if(pressedCtrl || m_paintRows.size() == 1) {
            //deselect it
            m_paintRows.erase(rowNum);
            if(m_paintRows.empty())
                m_paintRow = -1;
            else
                m_paintRow = *m_paintRows.begin();
            fireRowChanged = true;
            minRow = rowNum; maxRow = rowNum;
            m_lastPaintRow = -1;
        }
        else {
            minRow = *m_paintRows.begin();
            maxRow = *m_paintRows.rbegin();
            m_paintRows.clear();
            m_paintRows.insert(rowNum);
            m_paintRow = rowNum;
            m_lastPaintRow = rowNum;
            fireRowChanged = true;
        }
    }
    else {
        if(rowNum == -1) {
            //select none
            if(!m_paintRows.empty()) {
                //clear selection
                minRow = *m_paintRows.begin();
                maxRow = *m_paintRows.rbegin();
                m_paintRows.clear();
                m_paintRow = -1;
                m_lastPaintRow = -1;
                fireRowChanged = true;
            }
        }
        else if(rowNum == -2) {
            //select all
            for(int irow=0; irow<m_index.size(); irow++){
                m_paintRows.insert(irow);
            }
            fireRowChanged = true;
            m_paintRow = m_index.size()-1;
            m_lastPaintRow = m_index.size()-1;
            if(!m_paintRows.empty()) {
                minRow = *m_paintRows.begin();
                maxRow = *m_paintRows.rbegin();
            }
        }
        else {
            if((!pressedCtrl && !pressedShift)) {
                if(!m_paintRows.empty()) {
                    minRow = *m_paintRows.begin();
                    maxRow = *m_paintRows.rbegin();
                    m_paintRows.clear();
                }
            }
            if(!pressedShift || m_lastPaintRow == -1){
                m_paintRows.insert(rowNum);
            }
            else {
                int fromRow = m_lastPaintRow;
                int toRow = rowNum;
                if(fromRow > toRow) std::swap(fromRow, toRow);
                minRow = fromRow;
                maxRow = toRow;
                while(fromRow <= toRow) {
                    m_paintRows.insert(fromRow);
                    fromRow++;
                }
            }

            m_paintRow = rowNum;
            m_lastPaintRow = rowNum;
            fireRowChanged = true;
            if(minRow < 0 || rowNum < minRow) minRow = rowNum;
            if(maxRow < 0 || rowNum > maxRow) maxRow = rowNum;
        }
    }

    if(fireRowChanged) {
        emit paintRowChanged();
        emit paintRowCountChanged();
        emit selectedRowCountChanged();

        qDebug() << "DeviceConfigViewModel::setPaintRow - m_paintRows";
        for(auto p : m_paintRows) {
            qDebug() << "p:"<<p << "m_index[p]:"<<m_index[p];
        }

        if(minRow != -1) {
            QVector<int> role;
            role << Role::PaintRole;
            emit dataChanged(QModelIndex(index(minRow, 0)), QModelIndex(index(maxRow, columnCount() -1)), role);
        }
    }
}

int DeviceConfigViewModel::highlightRow() const
{
    return m_highlightRow;
}

QVector<int> DeviceConfigViewModel::getHighlightRows() const
{
    QVector<int> toRet;
    for(auto curRow: m_highlightRows) toRet.push_back(curRow);
    return toRet;
}


QVariant DeviceConfigViewModel::getDataDisplayRole(const QModelIndex &index) const
{
    int row = m_index[index.row()];
    //    SPDLOG_DEBUG("DeviceConfigViewModel::getDataDisplayRole - row={} index.column()={}", row, index.column());

    const QVariant& cellValue = m_data[row][index.column()];

    if(cellValue.isNull() || !cellValue.isValid())
        return QVariant();
    //    qDebug() << "DeviceConfigViewModel::getDataDisplayRole - row="<< row<< "cellValue="  << cellValue.toString();

    return cellValue;
}

QVariant DeviceConfigViewModel::getServerUpdateRole(const QModelIndex &index) const
{
    if(m_viewType != FirmwareUpdateViewType)
        return QVariant();

    int row = m_index[index.row()];
    //    SPDLOG_DEBUG("DeviceConfigViewModel::getDataDisplayRole - row={} index.column()={}", row, index.column());

    const QVariant& cellValue = m_data[row][DeviceConfigServerUpdate];

    if(cellValue.isNull() || !cellValue.isValid())
        return QVariant();
    //    qDebug() << "DeviceConfigViewModel::getDataDisplayRole - row="<< row<< "cellValue="  << cellValue.toString();

    return cellValue;
}

void DeviceConfigViewModel::getUpdateFwInfoFile()
{

    if(m_future.isRunning()){
        return;
    }

    m_future = QtConcurrent::run([=]() {
        QCoreServiceManager::Instance().getUpdateFwInfoFile();

        m_paintRows.clear();
        m_paintRow = -1;
        m_lastPaintRow = -1;

        // getUpdateFwInfoFile()에서 얻은 데이터를 가지고 펌웨어 업그레이드 탭의 테이블 값 세팅
        for (int dataIndex = 0; dataIndex < m_data.size(); ++dataIndex)
        {
            int rowNum = m_index[dataIndex];
            // setup 페이지의 테이블에서 녹화장비(ex.NVR)의 deviceId 가져옴
            QString deviceModelName = m_data[rowNum][DeviceConfigModel].toString();
            QString channelModelName = m_data[rowNum][DeviceConfigChannelModel].toString();
            QString currentVersion = m_data[rowNum][DeviceConfigVersion].toString();
            QString latestVersion = "-";
            Wisenet::Core::FirmwareUpdateStatus isLatest = Wisenet::Core::FirmwareUpdateStatus::Latest;

            // NVR device
            if(channelModelName == "-")
            {
                latestVersion = getDeviceRecentVersion(deviceModelName).second;
                isLatest = QCoreServiceManager::Instance().checkLatestVersion(deviceModelName, currentVersion);
            }
            else // Channel cam
            {
                latestVersion = getDeviceRecentVersion(channelModelName).second;
                isLatest = QCoreServiceManager::Instance().checkLatestVersion(channelModelName, currentVersion);
            }
            if(isLatest == Wisenet::Core::FirmwareUpdateStatus::NeedToUpdate)
            {
                setPaintRow(dataIndex, Qt::ControlModifier);
            }

            setCellValue(rowNum, DeviceConfigPath, latestVersion);
            setCellValue(rowNum, DeviceConfigServerUpdate, true, true);
        }
         SPDLOG_DEBUG("DeviceConfigViewModel::getUpdateFwInfoFile() end");
    });
}

// 디바이스 유지관리 설정이 열릴 때 각 컬럼 내용 값 세팅
void DeviceConfigViewModel::reset(DeviceConfigViewType type)
{
    SPDLOG_DEBUG("DeviceMaintenanceViewModel::reset type:{}", type);
    m_viewType = type;

    m_data.clear();
    m_index.clear();
    m_highlightRows.clear();
    m_highlightRow = -1;
    m_lastHighlightRow = -1;
    m_hoveredRow = -1;

    // 펌웨어 업그레이드 관리 탭에 필요한 데이터 세팅
    if(m_viewType == DeviceConfigViewType::FirmwareUpdateViewType){
        m_row = 0;
        m_cancelUpdating = false;
        setFirmwareDownloading(false);
    }

    auto deviceList = QCoreServiceManager::Instance().DB()->GetDevices();
    auto deviceStatusList = QCoreServiceManager::Instance().DB()->GetDevicesStatus();

    int i = 0;

    for(auto& iter : deviceList){
        QVector<QVariant> display;
        m_deviceServerUpdateModeMap[QString::fromStdString(iter.second.deviceID)] = true;

        SPDLOG_DEBUG("DeviceMaintenanceViewModel::reset model:{} ip:{}", iter.second.modelName, iter.second.connectionInfo.host);

        display.push_back(QString::fromStdString(iter.second.deviceID)); // NVR device ID

        display.push_back(false);
        display.push_back(QString::fromStdString(iter.second.modelName)); // NVR model
        if(type == DeviceConfigViewType::FirmwareUpdateViewType) {
            display.push_back(QString::fromStdString(iter.second.modelName) + " (" + QString::fromStdString(iter.second.name) + ")"); // NVR model + name
        }
        else {
            display.push_back(QString::fromStdString(iter.second.name)); // NVR name (Device)
        }

        if(type == DeviceConfigViewType::FirmwareUpdateViewType || type == DeviceConfigViewType::ConfigRestoreViewType) {
            display.push_back(QString("-")); // Ch Id
            display.push_back(QString("-")); // Ch model
            display.push_back(QString("-")); // Ch name
        }

        display.push_back(QString::fromStdString(iter.second.connectionInfo.host)); // NVR IP

        if(type != DeviceConfigViewType::PasswordChangeViewType)
            display.push_back(QString::fromStdString(iter.second.firmwareVersion)); // NVR Current version

        if(type == DeviceConfigViewType::FirmwareUpdateViewType || type == DeviceConfigViewType::ConfigRestoreViewType) {
            display.push_back(QString("-")); // NVR Latest version
            display.push_back(false);
        }

        display.push_back(QString(" ")); //status

        m_data.push_back(display);
        m_index.push_back(i++);

        // NVR의 하위 카메라 탐색
        if(type == DeviceConfigViewType::FirmwareUpdateViewType) {
            for(auto& camIter : iter.second.channels) {
                m_deviceServerUpdateModeMap[QString::fromStdString(iter.second.deviceID) + "_" + QString::fromStdString(camIter.second.channelID)] = true;

                // NVR의 deviceStatusList 가져옴
                auto nvrStatusIter = deviceStatusList.find(camIter.second.deviceID);

                // 업그레이드가 불가능한(연결끊김 등) 하위 장비는 표시하지 않도록 필터링
                if(nvrStatusIter != deviceStatusList.end()) {
                    auto camStatusIter = nvrStatusIter->second.channelsStatus.find(camIter.second.channelID);
                    if(camStatusIter != nvrStatusIter->second.channelsStatus.end()) {
                        if(camStatusIter->second.isGood() == false) {
                            continue;
                        }
                    }
                }

                // SUNAPI의 media/camupgrade를 지원하지 않는 카메라 필터링
                if(camIter.second.channelCurVersion == "-") {
                    continue;
                }

                QVector<QVariant> display;

                SPDLOG_DEBUG("DeviceMaintenanceViewModel::reset cam model:{} ip:{}", camIter.second.channelModelName, camIter.second.ip);

                display.push_back(QString::fromStdString(camIter.second.deviceID)); // NVR device ID
                display.push_back(false);
                display.push_back(QString::fromStdString(iter.second.modelName)); // NVR model
                display.push_back(QString::fromStdString(iter.second.modelName) + " (" + QString::fromStdString(iter.second.name) + ")"); // NVR model + name


                display.push_back(QString::fromStdString(camIter.second.deviceID) + "_" + QString::fromStdString(camIter.second.channelID)); // Ch Id
                display.push_back(QString::fromStdString(camIter.second.channelModelName)); // Ch model
                display.push_back(QString::fromStdString(camIter.second.channelModelName) + " (" + QString::fromStdString(camIter.second.channelName) + ")"); // Ch model + name

                display.push_back(QString::fromStdString(camIter.second.ip)); // Ch IP

                display.push_back(QString::fromStdString(camIter.second.channelCurVersion)); // Ch Current version

                display.push_back(QString("-")); // Ch Latest version
                display.push_back(false);
                display.push_back(QString(" ")); // Ch status

                m_data.push_back(display);
                m_index.push_back(i++);
            }
        }
    }
    sort(DeviceConfigModel);

#ifdef WISENET_S1_VERSION
    if(type == DeviceConfigViewType::PasswordChangeViewType)
    {
        checkAllDevicePasswordExpiry();
    }
#endif
    // 펌웨어 업그레이드 관리 탭의 최신 펌웨어 정보를 비동기로 요청하여 테이블 채움
    if(type == DeviceConfigViewType::FirmwareUpdateViewType)
    {
        getUpdateFwInfoFile();
    }
}

void DeviceConfigViewModel::UpdateSortData(int column, Qt::SortOrder order)
{
    m_sorted.clear();
    m_sorted.fill(0, m_data.size());
    std::iota(m_sorted.begin(), m_sorted.end(), 0);

    //    SPDLOG_DEBUG("DeviceConfigViewModel::UpdateSortData - m_viewType={} column={} order={} m_sorted.size={} m_data.size={}",m_viewType,column,order,m_sorted.size(),m_data.size());

    std::sort(m_sorted.begin(),m_sorted.end(),[this, order,column](int indexA, int indexB){
        const QVariant& valueA = m_data[indexA][column];
        const QVariant& valueB = m_data[indexB][column];

        if(valueA.isNull() || valueB.isNull()) {
            SPDLOG_DEBUG("DeviceConfigViewModel::UpdateSortData - isNull of value");
            return true;
        }

        if(!valueA.isValid() || !valueB.isValid()) {
            SPDLOG_DEBUG("DeviceConfigViewModel::UpdateSortData - isNotValid of value");
            return true;
        }

        // ip sort
        if (column == DeviceConfigViewModel::DeviceConfigIp) {
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

bool DeviceConfigViewModel::setCheckState(int rowNum, bool checked)
{
    SPDLOG_DEBUG("setCheckState row:{} checked:{}",rowNum, checked);

    int row = m_index[rowNum];
    m_data[row][DeviceConfigCheck] = !checked;
    emit dataChanged(QModelIndex(index(rowNum, DeviceConfigCheck)), QModelIndex(index(rowNum, DeviceConfigCheck)));

    return true;
}

bool DeviceConfigViewModel::setAllCheckState(bool checked)
{
    SPDLOG_DEBUG("setAllCheckState checked:{}",checked);

    for(auto& item : m_index)
        m_data[item][DeviceConfigCheck] = checked;
    emit dataChanged(QModelIndex(index(0, DeviceConfigCheck)), QModelIndex(index(m_index.length()-1, DeviceConfigCheck)));

    return true;
}

bool DeviceConfigViewModel::setCellValue(int rowNum, int columnNum, const QVariant &data, bool isServerUpdate)
{
    if(rowNum < 0 || rowNum >= m_data.size())
        return false;

    int localCol = columnNum;
    if(columnNum == DeviceConfigStatus)
        localCol = columnCount() -1;

    SPDLOG_DEBUG("setCellValue--1 rowNum:{} localCol:{}",rowNum,localCol);
    m_data[rowNum][localCol] = data;
    SPDLOG_DEBUG("setCellValue--2 rowNum:{} localCol:{}",rowNum,localCol);
    m_roles.clear();
    if(isServerUpdate){
        m_roles << ServerUpdateRole;
        localCol--;
    }
    else
        m_roles << Qt::DisplayRole;

    auto modelIndex = QModelIndex(index(getCurrentRow(rowNum), localCol));
    emit dataChanged(modelIndex, modelIndex, m_roles);
    return true;
}

int DeviceConfigViewModel::selectedRowCount(){
    int ret = 0;
    for (auto& item: m_data) {
        if(item[DeviceConfigCheck].toBool() == true)
            ret++;
    }
    return ret;
}

bool DeviceConfigViewModel::isFirmwareUpdating(){
    SPDLOG_DEBUG("DeviceConfigViewModel::isFirmwareUpdating: m_firmwareDownloading({}) device({}), channel({})", m_firmwareDownloading, m_deviceFirmwareUpdatingMap.size(), m_channelFirmwareUpdatingMap.size());

    bool ret = m_firmwareDownloading || m_deviceFirmwareUpdatingMap.size() > 0 || m_channelFirmwareUpdatingMap.size() > 0;

    if(ret == false) {
        removeDownloadedFirmware();
    }

    return ret;
}

QPair<QString,QString> DeviceConfigViewModel::getDeviceRecentVersion(QString deviceModelName) {
    // 디바이스가 펌웨어 업데이트 대상인지 체크
    QPair<QString,QString> recentVersionInfo = QCoreServiceManager::Instance().getDeviceRecentVersion(deviceModelName);

    SPDLOG_DEBUG("DeviceConfigViewModel::checkUpdate - deviceModelName={} modelCode={} recentUpdateVersionName={}", deviceModelName.toStdString(), recentVersionInfo.first.toStdString(), recentVersionInfo.second.toStdString());
    return recentVersionInfo;
}

std::unique_ptr<QFile> DeviceConfigViewModel::openFileForWrite(const QString &fileName)
{
    std::unique_ptr<QFile> file(new QFile(fileName));
    if (!file->open(QIODevice::WriteOnly)) {
        qDebug() << "DeviceConfigViewModel::openFileForWrite() failed";
        return nullptr;
    }
    return file;
}

void DeviceConfigViewModel::httpReadyRead()
{
    if (file)
    {
        file->write(reply->readAll());
    }
}

void DeviceConfigViewModel::networkReplyProgress(qint64 bytesRead, qint64 totalBytes)
{
    if(totalBytes)
    {
        qint64 downloadProgress = (bytesRead*100)/totalBytes;
        QVariant vdownloadProgress(downloadProgress);
        setDownloadProgress(vdownloadProgress);
    }
}
void DeviceConfigViewModel::setDownloadProgress(QVariant &downloadProgress)
{
    m_downloadProgress = downloadProgress;
    qDebug() << "DeviceConfigViewModel::setDownloadProgress() - downloadProgress=" << m_downloadProgress << "%";
}

void DeviceConfigViewModel::httpFinished()
{
    SPDLOG_DEBUG("DeviceConfigViewModel::httpFinished()");

    if(!file) {
        SPDLOG_DEBUG("DeviceConfigViewModel::httpFinished() - file is null");
        return;
    }

    QFileInfo fi;
    if (file) {
        fi.setFile(file->fileName());
        file->close();
        file.reset();
    }

    if (reply->error()) {
        qDebug() << "DeviceConfigViewModel::reply->error()";
        if(QFile::exists(fi.absoluteFilePath())){
            QFile::remove(fi.absoluteFilePath());
        }
        reply->deleteLater();
        reply = nullptr;
        QVariant zero(0);
        setDownloadProgress(zero);

        std::string deviceId = m_data[m_row][DeviceConfigDeviceId].toString().toStdString();
        if(m_deviceFirmwareUpdatingMap.find(deviceId) != m_deviceFirmwareUpdatingMap.end())
        {
            m_deviceFirmwareUpdatingMap.erase(deviceId);
        }
        if(m_channelFirmwareUpdatingMap.find(deviceId) != m_channelFirmwareUpdatingMap.end())
        {
            m_channelFirmwareUpdatingMap.erase(deviceId);
        }

        setCellValue(m_row, DeviceConfigStatus,  translate("Download failed"));
        emit isFirmwareUpdatingChanged();
        startUpdate(m_row+1);
        return;
    }
    reply->deleteLater();
    reply = nullptr;

    if(getDownloadProgress() == 100 && fi.size() > 1024*1024)
    {
        SPDLOG_DEBUG("DeviceConfigViewModel::httpFinished() row{} - getDownloadProgress=100% fi.size>1MB", m_row);
        // insert to map (m_deviceFirmwareUpdatingMap or m_channelFirmwareUpdatingMap)
        QString deviceId = m_data[m_row][DeviceConfigDeviceId].toString();
        QString channelId = m_data[m_row][DeviceConfigChannelId].toString();
        QString path = fi.filePath();

        if(m_data[m_row][DeviceConfigChannelModel] == "-") {
            // NVR
            m_deviceFirmwareUpdatingMap[deviceId.toStdString()] = std::make_shared<Wisenet::Device::UpdatingDevice>(path.toLocal8Bit().toStdString());
            SPDLOG_DEBUG("DeviceConfigViewModel::httpFinished() - m_deviceFirmwareUpdatingMap row{} path={}", m_row, m_deviceFirmwareUpdatingMap[deviceId.toStdString()]->firmwarePath);
        }
        else {
            // 카메라

            QString channelNumber;
            QStringList channel = channelId.split("_");
            if(channel.size() == 2) {
                channelNumber = channel[1];
            }

            if(m_channelFirmwareUpdatingMap.find(deviceId.toStdString()) != m_channelFirmwareUpdatingMap.end()) {
                m_channelFirmwareUpdatingMap[deviceId.toStdString()].push_back(std::make_shared<Wisenet::Device::UpdatingChannel>(channelNumber.toStdString(), path.toLocal8Bit().toStdString()));
            }
            else {
                std::vector<std::shared_ptr<Wisenet::Device::UpdatingChannel>> channelList;
                channelList.push_back(std::make_shared<Wisenet::Device::UpdatingChannel>(channelNumber.toStdString(), path.toLocal8Bit().toStdString()));
                m_channelFirmwareUpdatingMap[deviceId.toStdString()] = channelList;
            }

            for(auto channel : m_channelFirmwareUpdatingMap[deviceId.toStdString()]) {
                SPDLOG_DEBUG("DeviceConfigViewModel::httpFinished() - m_channelFirmwareUpdatingMap row{} path={}", m_row, channel->firmwarePath);
            }
        }
        //        SPDLOG_DEBUG("DeviceConfigViewModel::httpFinished() - m_deviceFirmwareUpdatingMap.size={}, m_channelFirmwareUpdatingMap.size={}", m_deviceFirmwareUpdatingMap.size(), m_channelFirmwareUpdatingMap.size());
    }
    else
    {
        QVariant zero(0);
        setDownloadProgress(zero);
        SPDLOG_DEBUG("DeviceConfigViewModel::httpFinished()1 - m_deviceFirmwareUpdatingMap.size={}, m_channelFirmwareUpdatingMap.size={}", m_deviceFirmwareUpdatingMap.size(), m_channelFirmwareUpdatingMap.size());
        setCellValue(m_row, DeviceConfigStatus,  translate("The download could not be completed. Please try again later."));
    }

    startUpdate(m_row+1);
}

void DeviceConfigViewModel::startUpdate(int row)
{
    // 업그레이드 탭에서 취소버튼 클릭시 더이상 진행하지 않음
    if(m_cancelUpdating) {
        m_cancelUpdating = false;
        m_firmwareDownloading = false;
        return;
    }

    m_row = row;
    setFirmwareDownloading(true);

    // 업데이트 시작시 초기 세팅
    if(row == 0) {
        m_deviceFirmwareUpdatingMap.clear();
        m_channelFirmwareUpdatingMap.clear();

        // UI의 status 초기(Uploading or Downloading) 표시
        for (int dataIndex = 0; dataIndex < m_data.size(); ++dataIndex) {
            if(m_data[dataIndex][DeviceConfigCheck].toBool() == false) {
                setCellValue(dataIndex, DeviceConfigStatus, QString("0"));
                continue;
            }

            if(internalServerUpdateMode(dataIndex)) {
                setCellValue(dataIndex, DeviceConfigStatus,  translate("Downloading"));
            }
            else {
                setCellValue(dataIndex, DeviceConfigStatus,  translate("Uploading"));
            }
        }
    }

    // 모든 row 처리 완료
    if(row >= m_data.size()) {
        m_row = 0;
        m_cancelUpdating = false;
        SPDLOG_DEBUG("DeviceConfigViewModel::startUpdate() is done - m_row={}",m_row);
        SPDLOG_DEBUG("DeviceConfigViewModel::startUpdate() - m_deviceFirmwareUpdatingMap.size={}, m_channelFirmwareUpdatingMap.size={}", m_deviceFirmwareUpdatingMap.size(), m_channelFirmwareUpdatingMap.size());
        setFirmwareDownloading(false);
        startFirmwareUpdate();
        return;
    }

    // 체크되어있지 않은 장치는 넘어감
    if(m_data[row][DeviceConfigCheck].toBool() == false) {
        SPDLOG_DEBUG("DeviceConfigViewModel::startUpdate() - checked={}",m_data[row][DeviceConfigCheck].toBool());
        setCellValue(row, DeviceConfigStatus, QString("0"));

        startUpdate(row+1);
        return;
    }

    // 자동 업데이트가 불가한 디바이스에 대해 선택 및 업데이트 진행시 예외처리
    if(m_data[row][DeviceConfigPath].toString() == "-") {
        std::string deviceId = m_data[row][DeviceConfigDeviceId].toString().toStdString();
        if(m_deviceFirmwareUpdatingMap.find(deviceId) != m_deviceFirmwareUpdatingMap.end())
        {
            m_deviceFirmwareUpdatingMap.erase(deviceId);
        }
        if(m_channelFirmwareUpdatingMap.find(deviceId) != m_channelFirmwareUpdatingMap.end())
        {
            m_channelFirmwareUpdatingMap.erase(deviceId);
        }

        setCellValue(row, DeviceConfigStatus,  translate("Download failed"));
        emit isFirmwareUpdatingChanged();
        startUpdate(row+1);
        return;
    }

     if(internalServerUpdateMode(row) == false) {
        startUpdate(row+1);
        return;
    }

    QString updateServerUrl = QCoreServiceManager::Instance().Settings()->deviceUpdateSeverURL();
    qDebug() << "DeviceConfigViewModel::updateServerUrl:" << updateServerUrl;

    QString updateFilename = "-";
    QPair<QString,QString> recentVersionInfo;
    if(m_data[row][DeviceConfigChannelModel] == "-") {
        // NVR
        recentVersionInfo = getDeviceRecentVersion(m_data[row][DeviceConfigModel].toString());
        updateFilename = recentVersionInfo.second + ".img";
    }
    else {
        // 카메라
        recentVersionInfo = getDeviceRecentVersion(m_data[row][DeviceConfigChannelModel].toString());
        updateFilename = recentVersionInfo.second + ".img";
    }

    qDebug() << "DeviceConfigViewModel::updateFilename:" << updateFilename;

    QUrl strUrl = QString("-");
    if(m_data[row][DeviceConfigChannelModel] == "-") {
        // NVR
        strUrl = updateServerUrl + "NVR/" + recentVersionInfo.first + "/" + updateFilename;
    }
    else {
        // 카메라
        strUrl = updateServerUrl + "NW_Camera/" + recentVersionInfo.first + "/" + updateFilename;
    }

    qDebug() << "DeviceConfigViewModel::strUrl:" << strUrl;
    QString fileName = QCoreApplication::applicationDirPath() + "/" + updateFilename;

    file = openFileForWrite(fileName);
    qDebug() << "DeviceConfigViewModel::fileName():" << file->fileName();
    if (!file) {
        std::string deviceId = m_data[row][DeviceConfigDeviceId].toString().toStdString();
        if(m_deviceFirmwareUpdatingMap.find(deviceId) != m_deviceFirmwareUpdatingMap.end())
        {
            m_deviceFirmwareUpdatingMap.erase(deviceId);
        }
        if(m_channelFirmwareUpdatingMap.find(deviceId) != m_channelFirmwareUpdatingMap.end())
        {
            m_channelFirmwareUpdatingMap.erase(deviceId);
        }

        setCellValue(row, DeviceConfigStatus,  translate("Download failed"));
        emit isFirmwareUpdatingChanged();
        startUpdate(row+1);
        return;
    }

    m_deviceUpdatefileName = fileName;

    reply = qnam.get(QNetworkRequest(strUrl));

    connect(reply, &QNetworkReply::finished, this, &DeviceConfigViewModel::httpFinished);
    connect(reply, &QIODevice::readyRead, this, &DeviceConfigViewModel::httpReadyRead);
    connect(reply, &QNetworkReply::downloadProgress, this, &DeviceConfigViewModel::networkReplyProgress);
}

void DeviceConfigViewModel::startFirmwareUpdate()
{
    SPDLOG_DEBUG("DeviceConfigViewModel::startFirmwareUpdate()");
    auto deviceList = QCoreServiceManager::Instance().DB()->GetDevices();
    auto deviceStatusList = QCoreServiceManager::Instance().DB()->GetDevicesStatus();

    for (int dataIndex = 0; dataIndex < m_data.size(); ++dataIndex) {
        // 체크되어있지 않은 채널은 넘어감
        if(m_data[dataIndex][DeviceConfigCheck].toBool() == false){
            setCellValue(dataIndex, DeviceConfigStatus, QString("0"));
            continue;
        }

        // setup 페이지의 테이블에서 녹화장비(ex.NVR)의 deviceId와 펌웨어가 위치한 path 가져옴
        QString deviceId = m_data[dataIndex][DeviceConfigDeviceId].toString();
        QString path = m_data[dataIndex][DeviceConfigPath].toString();

        //        for(auto channel : m_channelFirmwareUpdatingMap[deviceId.toStdString()]) {
        //            SPDLOG_DEBUG("DeviceConfigViewModel::startFirmwareUpdate() m_channelFirmwareUpdatingMap - row{} path={}", dataIndex, channel->firmwarePath);
        //        }

        // setup 페이지의 테이블에서 장비에 연결된 채널 id 정보 가져옴
        QString channelId = m_data[dataIndex][DeviceConfigChannelId].toString();

        QString channelNumber;
        QStringList channel = channelId.split("_");
        if(channel.size() == 2) {
            channelNumber = channel[1];
        }

        // fw upgrade 창에서 선택한 장비의 deviceId로 DB 검색
        auto nvrIter = deviceList.find(deviceId.toStdString());

        // 채널 카메라 업그레이드
        if(nvrIter != deviceList.end()){
            auto nvrStatusIter = deviceStatusList.find((deviceId.toStdString()));
            if(nvrStatusIter != deviceStatusList.end()){
                if(channelId != "-"){
                    if(m_deviceServerUpdateModeMap.find(channelId) != m_deviceServerUpdateModeMap.end()) {
                        SPDLOG_DEBUG("channel fw update on server - updatingMapSize={} m_deviceServerUpdateModeMap[channelId]={}", m_channelFirmwareUpdatingMap[nvrIter->second.deviceID].size(), m_deviceServerUpdateModeMap[channelId]);
                        for(auto channel : m_channelFirmwareUpdatingMap[nvrIter->second.deviceID]) {
                            SPDLOG_DEBUG("DeviceConfigViewModel::startFirmwareUpdate() channel - row{} path={}", dataIndex, channel->firmwarePath);
                        }
                        if(m_deviceServerUpdateModeMap[channelId] == true) {
                            continue;
                        }
                    }
                    // channel(하위 카메라) fw update
                    SPDLOG_DEBUG("channel fw update on local");
                    auto camStatusIter = nvrStatusIter->second.channelsStatus.find(channelNumber.toStdString());
                    if(camStatusIter != nvrStatusIter->second.channelsStatus.end()) {
                        if(camStatusIter->second.isGood()) {
                            if(m_channelFirmwareUpdatingMap.find(nvrIter->second.deviceID) != m_channelFirmwareUpdatingMap.end()) {
                                m_channelFirmwareUpdatingMap[nvrIter->second.deviceID].push_back(std::make_shared<Wisenet::Device::UpdatingChannel>(camStatusIter->second.channelID, path.toLocal8Bit().toStdString()));
                            }
                            else {
                                std::vector<std::shared_ptr<Wisenet::Device::UpdatingChannel>> channelList;
                                channelList.push_back(std::make_shared<Wisenet::Device::UpdatingChannel>(camStatusIter->second.channelID, path.toLocal8Bit().toStdString()));
                                m_channelFirmwareUpdatingMap[nvrIter->second.deviceID] = channelList;
                            }
                            //                            emit isFirmwareUpdatingChanged();
                        }
                        else
                            setCellValue(dataIndex, DeviceConfigStatus, QString("Camera disconnected"));
                    }
                }
            }
        }
    }

    // NVR 장비 업그레이드
    for (int dataIndex = 0; dataIndex < m_data.size(); ++dataIndex) {
        // 체크되어있지 않은 NVR은 넘어감
        if(m_data[dataIndex][DeviceConfigCheck].toBool() == false){
            setCellValue(dataIndex, DeviceConfigStatus, QString("0"));
            continue;
        }

        // setup 페이지의 테이블에서 녹화장비(ex.NVR)의 deviceId와 펌웨어가 위치한 path 가져옴
        QString deviceId = m_data[dataIndex][DeviceConfigDeviceId].toString();
        QString path = m_data[dataIndex][DeviceConfigPath].toString();

        // setup 페이지의 테이블에서 장비에 연결된 채널 id 정보 가져옴
        QString channelId = m_data[dataIndex][DeviceConfigChannelId].toString();

        // fw upgrade 창에서 선택한 장비의 deviceId로 DB 검색
        auto nvrIter = deviceList.find(deviceId.toStdString());

        if(nvrIter != deviceList.end()){
            auto nvrStatusIter = deviceStatusList.find((deviceId.toStdString()));
            if(nvrStatusIter != deviceStatusList.end()){
                if(channelId == "-"){
                    SPDLOG_DEBUG("device fw update");
                    // device(녹화장비) fw update
                    if(nvrStatusIter->second.isGood()){
                        if(internalServerUpdateMode(dataIndex)) {
                            SPDLOG_DEBUG("device fw update on server");
                        }
                        else {
                            SPDLOG_DEBUG("device fw update on local");
                            m_deviceFirmwareUpdatingMap[nvrIter->second.deviceID] = std::make_shared<Wisenet::Device::UpdatingDevice>(path.toLocal8Bit().toStdString());
                        }
//                        if(m_channelFirmwareUpdatingMap.find(nvrIter->second.deviceID) == m_channelFirmwareUpdatingMap.end()) {
                            if(m_deviceFirmwareUpdatingMap.find(nvrIter->second.deviceID) != m_deviceFirmwareUpdatingMap.end()) {
                                SPDLOG_DEBUG("DeviceConfigViewModel::startFirmwareUpdate() - nvrIter->second.deviceID:{} firmwarePath:{}", nvrIter->second.deviceID, m_deviceFirmwareUpdatingMap[nvrIter->second.deviceID]->firmwarePath);
                                deviceFirmwareUpdate(nvrIter->second.deviceID, m_deviceFirmwareUpdatingMap[nvrIter->second.deviceID]->firmwarePath, dataIndex);
                            }
//                        }
                    }
                    else
                        setCellValue(dataIndex, DeviceConfigStatus, QString("Recorder disconnected"));
                }
            }
        }
    }

    if(m_channelFirmwareUpdatingMap.size() > 0) {
        SPDLOG_DEBUG("channel fw update begin");
        channelFirmwareUpdate();
    }

    emit isFirmwareUpdatingChanged();
}

void DeviceConfigViewModel::cancelFirmwareUpdate()
{
    m_cancelUpdating = true;
    httpFinished();

    for (int dataIndex = 0; dataIndex < m_data.size(); ++dataIndex) {
        QString deviceId = m_data[dataIndex][DeviceConfigDeviceId].toString();
        SPDLOG_DEBUG("cancelFirmwareUpdate - dataIndex={}", dataIndex);

        if(m_deviceFirmwareUpdatingMap.find(deviceId.toStdString()) != m_deviceFirmwareUpdatingMap.end()){
            SPDLOG_DEBUG(" deviceConfigViewModel::cancelFirmwareUpdate id:{}", deviceId.toStdString());
            deviceFirmwareUpdate(deviceId.toStdString(), "", dataIndex, true);
        }
    }

    channelFirmwareUpdate(true);
    m_channelFirmwareUpdatingMap.clear();
    emit isFirmwareUpdatingChanged();
}

void DeviceConfigViewModel::removeDownloadedFirmware()
{
    // 펌웨어 업데이트 메뉴에서만 사용
    if(m_viewType != DeviceConfigViewType::FirmwareUpdateViewType)
    {
        return;
    }

    // 더이상 사용하지 않는 펌웨어 삭제
    for (int dataIndex = 0; dataIndex < m_data.size(); ++dataIndex)
    {
        bool isChecked = m_data[dataIndex][DeviceConfigCheck].toBool();
        QString firmwareName = m_data[dataIndex][DeviceConfigPath].toString();

        if(isChecked && internalServerUpdateMode(dataIndex))
        {
            QString filePath = QCoreApplication::applicationDirPath() + "/" + firmwareName + ".img";
            if(QFile::exists(filePath))
            {
                QFile::remove(filePath);
            }
        }
    }
}

bool DeviceConfigViewModel::isServerUpdateMode(int rowNum)
{
    int row = m_index[rowNum];
    bool isServerUpdate  = m_data[row][DeviceConfigServerUpdate].toBool();
    SPDLOG_DEBUG("DeviceConfigViewModel::isServerUpdateMode row:{} - isServerUpdate:{}", row, isServerUpdate);
    return isServerUpdate;
}

bool DeviceConfigViewModel::internalServerUpdateMode(int row)
{
    bool isServerUpdate  = m_data[row][DeviceConfigServerUpdate].toBool();
    SPDLOG_DEBUG("DeviceConfigViewModel::internalServerUpdateMode row:{} - isServerUpdate:{}", row, isServerUpdate);
    return isServerUpdate;
}

void DeviceConfigViewModel::setServerUpdateMode(int rowNum, bool isServerUpdateMode)
{
    int row = m_index[rowNum];

    QString deviceId = m_data[row][DeviceConfigDeviceId].toString();
    QString channelId = m_data[row][DeviceConfigChannelId].toString();
    QString model = m_data[row][DeviceConfigModel].toString();

    if(channelId == "-") {
        // device (NVR)에 대한 처리
        m_deviceServerUpdateModeMap[deviceId] = isServerUpdateMode;
        SPDLOG_DEBUG("DeviceConfigViewModel::setServerUpdateMode() device - row={} isServerUpdateMode={}", row, m_deviceServerUpdateModeMap[deviceId]);
    }
    else {
        // channel (Cam)에 대한 처리
        m_deviceServerUpdateModeMap[channelId] = isServerUpdateMode;
        SPDLOG_DEBUG("DeviceConfigViewModel::setServerUpdateMode() channel - row={} isServerUpdateMode={}", row, m_deviceServerUpdateModeMap[channelId]);
    }
    emit dataChanged(QModelIndex(index(row, 0)), QModelIndex(index(row, 0)));
}

void DeviceConfigViewModel::fillRecentFwVersion(int rowNum)
{
    int row = m_index[rowNum];

    QString deviceModelName = m_data[row][DeviceConfigModel].toString();
    QString channelModelName = m_data[row][DeviceConfigChannelModel].toString();
    QString recentFwVersion = "-";

    if(channelModelName == "-") {
        // device (NVR)에 대한 처리
        recentFwVersion = getDeviceRecentVersion(deviceModelName).second;
    }
    else {
        // channel (Cam)에 대한 처리
        recentFwVersion = getDeviceRecentVersion(channelModelName).second;
    }

    SPDLOG_DEBUG("DeviceConfigViewModel::fillRecentFwVersion() - row={} recentFwVersion={}", row, recentFwVersion.toStdString());
    if(recentFwVersion != "-")
        setCellValue(row, DeviceConfigServerUpdate, true, true);

    setCellValue(row, DeviceConfigPath, recentFwVersion);
}

// device(녹화장비) fw update
void DeviceConfigViewModel::deviceFirmwareUpdate(std::string deviceId, std::string path, int row, bool stop)
{
    SPDLOG_DEBUG("DeviceConfigViewModel::deviceFirmwareUpdate deviceId={} path={}",deviceId, path);
    auto request = std::make_shared<Wisenet::Device::DeviceFirmwareUpdateRequest>();
    request->deviceID = deviceId;
    request->firmwareFilePath = path;
    request->firmwareStop = stop;

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::DeviceFirmwareUpdate,
                this, request,
                [this, deviceId, row](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::Device:: DeviceFirmwareUpdateResponse>(reply->responseDataPtr);

        SPDLOG_DEBUG("DeviceConfigViewModel::deviceFirmwareUpdate id:{} -- success:{} -- errorCode:{} -- percent:{}",
                     deviceId, response->isSuccess(), response->errorString(), response->progressPercent);

        if(response->isFailed()){
            if(m_viewType == FirmwareUpdateViewType) {
                setCellValue(row, DeviceConfigStatus, translate(QString::fromStdString(response->errorString())));
            }

            if(m_deviceServerUpdateModeMap.find(QString::fromStdString(deviceId)) != m_deviceServerUpdateModeMap.end()) {
                if(m_deviceFirmwareUpdatingMap.find(deviceId) != m_deviceFirmwareUpdatingMap.end()) {
                    m_deviceFirmwareUpdatingMap.erase(deviceId);
                    emit isFirmwareUpdatingChanged();
                }
            }
        }
        else if (response->isContinue) {
            if(m_viewType == FirmwareUpdateViewType) {
                setCellValue(row, DeviceConfigStatus, QString::number(response->progressPercent));
            }
        }
        else if(response->isSuccess() && response->progressPercent == 100){
            if(m_viewType == FirmwareUpdateViewType) {
                setCellValue(row, DeviceConfigStatus,  translate("Success"));
            }
            m_deviceFirmwareUpdatingMap.erase(deviceId);
            emit isFirmwareUpdatingChanged();
        }
    });

    if(stop) {
        SPDLOG_DEBUG(" DeviceConfigViewModel::deviceFirmwareUpdate - Stop Updating about {}", deviceId);
        if(m_deviceServerUpdateModeMap.find(QString::fromStdString(deviceId)) != m_deviceServerUpdateModeMap.end()) {
            if(m_deviceFirmwareUpdatingMap.find(deviceId) != m_deviceFirmwareUpdatingMap.end()) {
                m_deviceFirmwareUpdatingMap.erase(deviceId);
                emit isFirmwareUpdatingChanged();
            }
        }
    }
}

// channel fw update
void DeviceConfigViewModel::channelFirmwareUpdate(bool stop)
{
    SPDLOG_DEBUG(" DeviceConfigViewModel::channelFirmwareUpdate");

    std::map<std::string, std::vector<std::shared_ptr<Wisenet::Device::UpdatingChannel>> > chFwUpdatingMap;
    chFwUpdatingMap.insert(m_channelFirmwareUpdatingMap.begin(), m_channelFirmwareUpdatingMap.end());

    for(auto& mapItr : chFwUpdatingMap) {

        auto chUpgradeRequest = std::make_shared<Wisenet::Device::DeviceChannelFirmwareUpdateRequest>();
        std::vector<std::shared_ptr<Wisenet::Device::UpdatingChannel>> channelList = mapItr.second; // copy firmwareUpdatingList
        SPDLOG_DEBUG(" DeviceConfigViewModel::channelFirmwareUpdate - mapItr.first:{} mapItr.first.size:{} mapItr.second.size:{}", mapItr.first, mapItr.first.size(), mapItr.second.size());

        chUpgradeRequest->deviceID = mapItr.first; // copy deviceID
        SPDLOG_DEBUG(" DeviceConfigViewModel::channelFirmwareUpdate - m_channelFirmwareUpdatingMap.size:{} chFwUpdatingMap.size:{} mapItr.second.size:{} channelList.size:{}",m_channelFirmwareUpdatingMap.size(), chFwUpdatingMap.size(), mapItr.second.size(), channelList.size());

        for(auto& channel : channelList) {
            SPDLOG_DEBUG(" DeviceConfigViewModel::channelFirmwareUpdate - chUpgradeRequest->firmwareUpdatingList.push_back : channelID={} fwPath={}",channel->channelID,channel->firmwarePath);
            chUpgradeRequest->firmwareUpdatingList.push_back(Wisenet::Device::UpdatingChannel(channel->channelID, channel->firmwarePath));
        }
        chUpgradeRequest->firmwareStop = stop;

        if(stop == false) {
            for(auto channelStatus : chUpgradeRequest->firmwareUpdatingList) {
                if(m_viewType == FirmwareUpdateViewType) {
                    for (int dataIndex = 0; dataIndex < m_data.size(); ++dataIndex) {
                        QString channelId = m_data[dataIndex][DeviceConfigChannelId].toString();

                        QString channelNumber;
                        QString devId;
                        QStringList channel = channelId.split("_");
                        if(channel.size() == 2) {
                            devId = channel[0];
                            channelNumber = channel[1];
                        }

                        if(devId.toStdString() == chUpgradeRequest->deviceID && channelNumber == QString::fromStdString(channelStatus.channelID)) {
                            setCellValue(dataIndex, DeviceConfigStatus,  translate("Uploading"));
                            emit isFirmwareUpdatingChanged();
                            break;
                        }
                    }
                }
            }
        }

        // upgrade request
        QCoreServiceManager::Instance().RequestToCoreService(
                    &Wisenet::Core::ICoreService::DeviceChannelFirmwareUpdate,
                    this, chUpgradeRequest,
                    [=](const QCoreServiceReplyPtr& reply)
        {
            auto response = std::static_pointer_cast<Wisenet::Device::DeviceUpgradeStatusResponse>(reply->responseDataPtr);
            auto deviceId = chUpgradeRequest->deviceID;

            SPDLOG_DEBUG(" DeviceConfigViewModel::channelFirmwareUpdate id:{} -- success:{} -- errorCode:{}",
                         deviceId, response->isSuccess(), response->errorString());

            SPDLOG_DEBUG("DeviceConfigViewModel::channelFirmwareUpdate - recorderUpgradeStatus={}", response->recorderUpgradeStatus);
//            for(auto channelStatus : response->updateStatusList) {
//                SPDLOG_DEBUG("DeviceConfigViewModel::channelFirmwareUpdate - channelID={} status={}",channelStatus.channelID,channelStatus.status);
//            }

            // 업그레이드 요청 실패
            if(response->isFailed()){
                SPDLOG_DEBUG("DeviceConfigViewModel::channelFirmwareUpdate failed - response->error:{}", response->errorString());

                if(response->errorCode == Wisenet::ErrorCode::NetworkError) {
                    if(m_viewType == FirmwareUpdateViewType) {
                        for (int dataIndex = 0; dataIndex < m_data.size(); ++dataIndex) {
                            QString channelId = m_data[dataIndex][DeviceConfigChannelId].toString();
                            bool isUpdating = m_data[dataIndex][DeviceConfigCheck].toBool();

                            if(isUpdating) {
                                std::string deviceId = m_data[dataIndex][DeviceConfigDeviceId].toString().toStdString();
                                if(m_deviceFirmwareUpdatingMap.find(deviceId) != m_deviceFirmwareUpdatingMap.end())
                                {
                                    m_deviceFirmwareUpdatingMap.erase(deviceId);
                                }
                                if(m_channelFirmwareUpdatingMap.find(deviceId) != m_channelFirmwareUpdatingMap.end())
                                {
                                    m_channelFirmwareUpdatingMap.erase(deviceId);
                                }

                                setCellValue(dataIndex, DeviceConfigStatus,  translate("Network error"));
                            }
                        }

                        emit isFirmwareUpdatingChanged();
                        return;
                    }
                }

                for(auto channelStatus : response->updateStatusList) {
                    if(m_viewType == FirmwareUpdateViewType) {
                        for (int dataIndex = 0; dataIndex < m_data.size(); ++dataIndex) {
                            QString channelId = m_data[dataIndex][DeviceConfigChannelId].toString();

                            QString channelNumber;
                            QString devId;
                            QStringList channel = channelId.split("_");
                            if(channel.size() == 2) {
                                devId = channel[0];
                                channelNumber = channel[1];
                            }

                            if(devId.toStdString() == deviceId && channelNumber == QString::fromStdString(channelStatus.channelID)) {
                                setCellValue(dataIndex, DeviceConfigStatus,  translate("Failed"));
                                break;
                            }
                        }
                    }
                }
                if(response->recorderUpgradeStatus == "DONE") {
                    if(m_deviceFirmwareUpdatingMap.count(deviceId)) {
                        for (int dataIndex = 0; dataIndex < m_data.size(); ++dataIndex) {
                            QString newDeviceId = m_data[dataIndex][DeviceConfigDeviceId].toString();

                            if(newDeviceId.toStdString() == deviceId) {
                                deviceFirmwareUpdate(deviceId, m_deviceFirmwareUpdatingMap[deviceId]->firmwarePath, dataIndex);
                                break;
                            }
                        }
                    }
                    m_channelFirmwareUpdatingMap.erase(deviceId);
                }
                emit isFirmwareUpdatingChanged();
            }

            // 업그레이드 요청 성공
            if(response->isSuccess()){
                for(auto channelStatus : response->updateStatusList) {
                    if(m_viewType == FirmwareUpdateViewType) {
                        for (int dataIndex = 0; dataIndex < m_data.size(); ++dataIndex) {
                            QString channelId = m_data[dataIndex][DeviceConfigChannelId].toString();

                            QString channelNumber;
                            QString devId;
                            QStringList channel = channelId.split("_");
                            if(channel.size() == 2) {
                                devId = channel[0];
                                channelNumber = channel[1];
                            }

                            if(devId.toStdString() == deviceId && channelNumber == QString::fromStdString(channelStatus.channelID)) {
                                if(channelStatus.status == "NONE") {
                                    channelStatus.status = "Uploading";
                                }

                                if(channelStatus.status == "UPGRADING") {
                                    channelStatus.status = "Upgrading";
                                }

                                if(channelStatus.status == "SUCCESS") {
                                    channelStatus.status = "Success";
                                }

                                if(channelStatus.status == "FAIL") {
                                    channelStatus.status = "Failed";
                                }
                                setCellValue(dataIndex, DeviceConfigStatus,  translate(QString::fromStdString(channelStatus.status)));
                                break;
                            }
                        }
                    }
                }
                if(response->recorderUpgradeStatus == "DONE") {
                    if(m_deviceFirmwareUpdatingMap.count(deviceId)) {
                        for (int dataIndex = 0; dataIndex < m_data.size(); ++dataIndex) {
                            QString newDeviceId = m_data[dataIndex][DeviceConfigDeviceId].toString();

                            if(newDeviceId.toStdString() == deviceId) {
                                deviceFirmwareUpdate(deviceId, m_deviceFirmwareUpdatingMap[deviceId]->firmwarePath, dataIndex);
                                break;
                            }
                        }
                    }
                    m_channelFirmwareUpdatingMap.erase(deviceId);
                }
                emit isFirmwareUpdatingChanged();
            }

        });
    }
}

void DeviceConfigViewModel::configBackup(std::string deviceId, std::string path, std::string password, int row)
{
    auto request = std::make_shared<Wisenet::Device::DeviceConfigBackupRequest>();
    request->deviceID = deviceId;
    request->backupFilePath = path;
    request->password = password;

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::DeviceConfigBackup,
                this, request,
                [=](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::ResponseBase>(reply->responseDataPtr);

        if (response->isSuccess()) {
            setCellValue(row, DeviceConfigStatus, QString("Success"));
        }
        else {
            setCellValue(row, DeviceConfigStatus, QString::fromStdString(response->errorString()));
        }
    });
}

void DeviceConfigViewModel::startConfigBackup(QString folderPath, QString password)
{
    auto deviceList = QCoreServiceManager::Instance().DB()->GetDevices();
    SPDLOG_DEBUG("DeviceConfigViewModel::startConfigBackup ");

    for (int dataIndex = 0; dataIndex < m_data.size(); ++dataIndex) {
        if(m_data[dataIndex][DeviceConfigCheck].toBool() == false)
            continue;
        QString deviceId = m_data[dataIndex][DeviceConfigDeviceId].toString();

        auto iter = deviceList.find(deviceId.toStdString());

        if(iter != deviceList.end()){
            std::string ext = ".bin";
            if(iter->second.deviceType == Wisenet::Device::DeviceType::SunapiRecorder)
                ext = ".dat";

            std::string tempModelName = iter->second.modelName;
            tempModelName.erase(std::remove(tempModelName.begin(), tempModelName.end(), '/'), tempModelName.end());

            std::string tempHost = iter->second.connectionInfo.host;
            tempHost.erase(std::remove(tempHost.begin(), tempHost.end(), ':'), tempHost.end());


            std::string filePath = tempModelName + "_" + tempHost + ext;

            std::string path = folderPath.toLocal8Bit().toStdString() + "/" + filePath;

            std::string pass = "";
            if(iter->second.deviceCapabilities.configBackupEncrypt)
                pass = password.toLocal8Bit().toStdString();

            configBackup(iter->second.deviceID, path, pass, dataIndex);
        }
    }
}

void DeviceConfigViewModel::startConfigRestore(bool excludeNetworkSettings, QString password)
{
    SPDLOG_DEBUG("DeviceConfigViewModel::startConfigRestore ");
    auto deviceList = QCoreServiceManager::Instance().DB()->GetDevices();

    for (int dataIndex = 0; dataIndex < m_data.size(); ++dataIndex) {
        if(m_data[dataIndex][DeviceConfigCheck].toBool() == false)
            continue;
        QString deviceId = m_data[dataIndex][DeviceConfigDeviceId].toString();
        QString path = m_data[dataIndex][DeviceConfigPath].toString();

        auto iter = deviceList.find(deviceId.toStdString());

        std::string pass = "";
        if(iter->second.deviceCapabilities.configRestoreDecrypt)
            pass = password.toLocal8Bit().toStdString();

        if(iter != deviceList.end()){
            configRestore(iter->second.deviceID, path.toLocal8Bit().toStdString(), pass, dataIndex, excludeNetworkSettings);
        }
    }
}

void DeviceConfigViewModel::configRestore(std::string deviceId, std::string path, std::string password, int row, bool excludeNetworkSettings)
{
    auto request = std::make_shared<Wisenet::Device::DeviceConfigRestoreRequest>();
    request->deviceID = deviceId;
    request->restoreFilePath = path;
    request->excludeNetworkSettings = excludeNetworkSettings;
    request->password = password;

    SPDLOG_DEBUG("DeviceConfigViewModel::configRestore path:{} exclude:{}", path, excludeNetworkSettings);

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::DeviceConfigRestore,
                this, request,
                [=](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::ResponseBase>(reply->responseDataPtr);

        if (response->isSuccess()) {
            setCellValue(row, DeviceConfigStatus, QString("Success"));
        }
        else {
            setCellValue(row, DeviceConfigStatus, QString::fromStdString(response->errorString()));
        }
    });
}

void DeviceConfigViewModel::startChangePasswordSetting(QString currentPassword, QString newPassword)
{
    SPDLOG_DEBUG("DeviceConfigViewModel::startChangePasswordSetting current:{} new:{}",currentPassword.toStdString() , newPassword.toStdString());
    auto deviceList = QCoreServiceManager::Instance().DB()->GetDevices();
    for (int dataIndex = 0; dataIndex < m_data.size(); ++dataIndex) {
        if(m_data[dataIndex][DeviceConfigCheck].toBool() == false)
            continue;
        QString deviceId = m_data[dataIndex][DeviceConfigDeviceId].toString();

        auto iter = deviceList.find(deviceId.toStdString());

        if(iter != deviceList.end()){
            auto device = iter->second;

            auto request = std::make_shared<Wisenet::Device::DeviceChangeUserPasswordRequest>();

            if(device.connectionInfo.password != currentPassword.toStdString()){
                setCellValue(dataIndex, DeviceConfigStatus, QString("Please check password."));
                continue;
            }

            request->deviceID = device.deviceID;
            request->password = newPassword.toStdString();
            request->userName = device.connectionInfo.user;

            QCoreServiceManager::Instance().RequestToCoreService(
                        &Wisenet::Core::ICoreService::DeviceChangeUserPassword,
                        this, request,
                        [this, dataIndex, device, newPassword](const QCoreServiceReplyPtr& reply)
            {
                auto response = std::static_pointer_cast<Wisenet::ResponseBase>(reply->responseDataPtr);

                if (response->isSuccess()) {
                    setCellValue(dataIndex, DeviceConfigStatus, QString("Success"));
                }
                else {
                    setCellValue(dataIndex, DeviceConfigStatus, QString::fromStdString(response->errorString()));
                }

                if (response->isSuccess()){
                    auto localDevice = device;
                    DisconnectDevice(localDevice);
                    SaveCredential(localDevice, dataIndex, newPassword);
                }
            });
        }
    }
}


void DeviceConfigViewModel::DisconnectDevice(Wisenet::Device::Device &device)
{
    auto disconnectRequest = std::make_shared<Wisenet::Device::DeviceDisconnectRequest>();
    disconnectRequest->deviceID = device.deviceID;

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::DeviceDisconnect,
                this, disconnectRequest,
                [=](const QCoreServiceReplyPtr& reply)
    {
        auto response = reply->responseDataPtr;
        if(response->isSuccess()){
            qDebug() << "DeviceMaintenanceViewModel::DisconnectDevice success";

        }
        else
            qDebug() << "DeviceMaintenanceViewModel::DisconnectDevice fail";
    });
}

void DeviceConfigViewModel::SaveCredential(Wisenet::Device::Device &device, int row, QString password)
{
    device.connectionInfo.password = password.toStdString();
    auto saveDevicesCredentialRequest = std::make_shared<Wisenet::Core::SaveDevicesCredentialRequest>();
    saveDevicesCredentialRequest->devices.push_back(device);

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::SaveDevicesCredential,
                this, saveDevicesCredentialRequest,
                [this, device, row](const QCoreServiceReplyPtr& reply)
    {
        Wisenet::Device::Device localDevice = device;

        auto response = reply->responseDataPtr;
        if(response->isSuccess()){
            ConnectDevice(localDevice, row);
        }
        else{
            setCellValue(row, DeviceConfigStatus, QString::fromStdString(response->errorString()));
        }
    });
}

void DeviceConfigViewModel::ConnectDevice(Wisenet::Device::Device &device, int row)
{
    auto connectRequest = std::make_shared<Wisenet::Device::DeviceConnectRequest>();
    connectRequest->deviceID = device.deviceID;
    connectRequest->connectionInfo = device.connectionInfo;

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::DeviceConnect,
                this, connectRequest,
                [this, row](const QCoreServiceReplyPtr& reply)
    {
        auto response = reply->responseDataPtr;
        if(response->isSuccess()){
            setCellValue(row, DeviceConfigStatus, QString("Success"));
        }
        else{
            setCellValue(row, DeviceConfigStatus, QString::fromStdString(response->errorString()));
        }
    });
}

void DeviceConfigViewModel::checkAllDevicePasswordExpiry()
{
    for (int dataIndex = 0; dataIndex < m_data.size(); ++dataIndex)
    {
        // setup 페이지의 테이블에서 녹화장비(ex.NVR)의 deviceId 가져옴
        QString deviceId = m_data[dataIndex][DeviceConfigDeviceId].toString();

        auto request = std::make_shared<Wisenet::Device::DeviceRequestBase>();
        request->deviceID = deviceId.toStdString();

        QCoreServiceManager::Instance().RequestToCoreService(
                    &Wisenet::Core::ICoreService::DeviceGetPasswordExpiry,
                    this, request,
                    [this, request, dataIndex](const QCoreServiceReplyPtr& reply)
        {
            // 패스워드가 만료됐다면 noti에 알림 추가
            // noti 알림을 클릭하면 설정-비밀번호 변경 메뉴로 이동
            auto response = std::static_pointer_cast<Wisenet::Device::DeviceGetPasswordExpiryResponse>(reply->responseDataPtr);

            SPDLOG_DEBUG("DeviceConfigViewModel::checkAllDevicePasswordExpiry() - DeviceGetPasswordExpiry, isSuccess:{} isPasswordExpired:{} expiryDurationSettingMonths:{}", response->isSuccess(), response->isPasswordExpired, response->expiryDurationSettingMonths);

            if(response->isSuccess())
            {
                // 비밀번호 만료되었다면
                qDebug() << "DeviceConfigViewModel::checkAllDevicePasswordExpiry() Success - this->m_viewType:" << this->m_viewType;
                if(response->isPasswordExpired)
                {
                    // 비밀번호 변경 테이블의 상태 값 갱신
                    setCellValue(dataIndex, DeviceConfigStatus, translate("Device Password Expiration"));
                }
            }
        });
    }
}


void DeviceConfigViewModel::setSameFile(bool bSameChecked, int rowNum, QString path)
{
    int row = m_index[rowNum];
    QString model = m_data[row][DeviceConfigModel].toString();
    QString chModel = m_data[row][DeviceConfigChannelModel].toString();
    QString chName = m_data[row][DeviceConfigChannelName].toString();

    if(bSameChecked){
        if(chModel == "-" && chName == "-") {
            for(int i = 0; i < m_data.size(); ++i){
                if( m_data[i][DeviceConfigModel].toString() == model
                        && m_data[i][DeviceConfigChannelModel].toString() == chModel
                        && m_data[i][DeviceConfigChannelName].toString() == chName){
                    setCellValue(i,DeviceConfigPath,  path);
                    setCellValue(i,DeviceConfigCheck,  true);
                    setCellValue(i,DeviceConfigStatus,  "0");
                    if(m_viewType == FirmwareUpdateViewType)
                        setCellValue(i,DeviceConfigServerUpdate, false, true);
                }
            }
        }
        else {
            for(int i = 0; i < m_data.size(); ++i){
                if( m_data[i][DeviceConfigChannelModel].toString() == chModel
                        && m_data[i][DeviceConfigChannelName].toString() == chName){
                    setCellValue(i,DeviceConfigPath,  path);
                    setCellValue(i,DeviceConfigCheck,  true);
                    setCellValue(i,DeviceConfigStatus,  "0");
                    if(m_viewType == FirmwareUpdateViewType)
                        setCellValue(i,DeviceConfigServerUpdate, false, true);
                }
            }
        }
    }
    else{
        setCellValue(row,DeviceConfigPath,  path);
        setCellValue(row,DeviceConfigCheck,  true);
        setCellValue(row,DeviceConfigStatus,  "0");
        if(m_viewType == FirmwareUpdateViewType)
            setCellValue(row,DeviceConfigServerUpdate, false, true);
    }
}

void DeviceConfigViewModel::deletePath(int rowNum)
{
    int row = m_index[rowNum];
    setCellValue(row,DeviceConfigPath,  QString("-"));
    setCellValue(row,DeviceConfigCheck,  false);
    if(m_viewType == FirmwareUpdateViewType)
        setCellValue(row,DeviceConfigServerUpdate, false, true);
}

void DeviceConfigViewModel::setFirmwareDownloading(bool flag)
{
    if(m_firmwareDownloading != flag) {
        m_firmwareDownloading = flag;
        emit isFirmwareUpdatingChanged();
    }
}

QString DeviceConfigViewModel::translate(QVariant display){
    std::string result = display.toString().toStdString();
    return QCoreApplication::translate("WisenetLinguist",result.c_str());

}

double DeviceConfigViewModel::convertType(QVariant display){
    auto result = display.toString().toDouble();
    return result;

}

bool DeviceConfigViewModel::isConfigBackupRequirePassword()
{
    SPDLOG_DEBUG("DeviceConfigViewModel::isConfigBackupRequirePassword() start");
    auto deviceList = QCoreServiceManager::Instance().DB()->GetDevices();

    for (int dataIndex = 0; dataIndex < m_data.size();dataIndex++) {
        if(m_data[dataIndex][DeviceConfigCheck].toBool() == false)
            continue;
        QString deviceId = m_data[dataIndex][DeviceConfigDeviceId].toString();

        auto iter = deviceList.find(deviceId.toStdString());

        if(iter != deviceList.end() && iter->second.deviceCapabilities.configBackupEncrypt){
            SPDLOG_DEBUG("DeviceConfigViewModel::isConfigBackupRequirePassword() true {}", iter->second.connectionInfo.ip);
            return true;
        }
    }
    SPDLOG_DEBUG("DeviceConfigViewModel::isConfigBackupRequirePassword() false");
    return false;
}

bool DeviceConfigViewModel::isConfigRestoreRequirePassword()
{
    auto deviceList = QCoreServiceManager::Instance().DB()->GetDevices();

    for (int dataIndex = 0; dataIndex < m_data.size(); ++dataIndex) {
        if(m_data[dataIndex][DeviceConfigCheck].toBool() == false)
            continue;
        QString deviceId = m_data[dataIndex][DeviceConfigDeviceId].toString();

        auto iter = deviceList.find(deviceId.toStdString());

        if(iter != deviceList.end() && iter->second.deviceCapabilities.configRestoreDecrypt){
            SPDLOG_DEBUG("DeviceConfigViewModel::isConfigRestoreRequirePassword() true {}", iter->second.connectionInfo.ip);
            return true;
        }
    }
    SPDLOG_DEBUG("DeviceConfigViewModel::isConfigRestoreRequirePassword() false");
    return false;
}

bool DeviceConfigViewModel::isSetFilePath()
{
    auto deviceList = QCoreServiceManager::Instance().DB()->GetDevices();

    for (int dataIndex = 0; dataIndex < m_data.size(); ++dataIndex) {
        if(m_data[dataIndex][DeviceConfigCheck].toBool() == false)
            continue;

        if(m_viewType == DeviceConfigViewType::ConfigRestoreViewType && m_data[dataIndex][DeviceConfigPath].toString() != "-" && m_data[dataIndex][DeviceConfigPath].toString().size() > 0)
            continue;

        if(m_viewType == DeviceConfigViewType::FirmwareUpdateViewType && m_data[dataIndex][DeviceConfigPath].toString() != "-" && m_data[dataIndex][DeviceConfigPath].toString().size() > 0)
            continue;

        return false;

    }
    SPDLOG_DEBUG("DeviceConfigViewModel::isSetFilePath() true");
    return true;
}
