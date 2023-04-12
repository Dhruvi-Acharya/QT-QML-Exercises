/*********************************************************************************
 * Copyright(c) 2021 by Hanwha Techwin Co., Ltd.
 *
 * This software is copyrighted by, and is the sole property of Hanwha Techwin.
 * All rigths, title, ownership, or other interests in the software remain the
 * property of Hanwha Techwin. This software may only be used in accordance with
 * the corresponding license agreement. Any unauthorized use, duplication,
 * transmission, distribution, or disclosure of this software is expressly
 * forbidden.
 *
 * This Copyright notice may not be removed or modified without prior written
 * consent of Hanwha Techwin.
 *
 * Hanwha Techwin reserves the right to modify this software without notice.
 *
 * Hanwha Techwin Co., Ltd.
 * KOREA
 * https://www.hanwha-security.com/
 *********************************************************************************/
#include "AutoDiscoveryModel.h"
#include "LogSettings.h"
#include <QQmlEngine>


const QString AutoDiscoveryModel::m_roleType = QString("type");
const QString AutoDiscoveryModel::m_roleModelName = QString("model");
const QString AutoDiscoveryModel::m_roleIpAddress = QString("ip");
const QString AutoDiscoveryModel::m_roleMacAddress = QString("mac");
const QString AutoDiscoveryModel::m_rolePort = QString("port");
const QString AutoDiscoveryModel::m_roleStatus = QString("status");
using namespace std::placeholders;

AutoDiscoveryModel::AutoDiscoveryModel(QObject *parent) : QObject(parent)
  , m_isScanning(false)
{
    SPDLOG_DEBUG("AutoDiscoveryModel()");
}

AutoDiscoveryModel::~AutoDiscoveryModel()
{
    SPDLOG_DEBUG("~AutoDiscoveryModel()");
}

void AutoDiscoveryModel::registerQml()
{
    qmlRegisterType<AutoDiscoveryModel>("Wisenet.Qmls", 0, 1, "AutoDiscoveryModel");
}

void AutoDiscoveryModel::setGenericTableModel(bpp::TableModel* model)
{
    SPDLOG_DEBUG("AutoDiscoveryModel::setGenericTableModel()");
    m_genericTableModel = model;
}

void AutoDiscoveryModel::videoOnTest()
{
    SPDLOG_INFO("Video streamming start!!");
}

void AutoDiscoveryModel::videoOffTest()
{
     SPDLOG_INFO("Video streamming stop!!");
}

bool AutoDiscoveryModel::getScanningStatus()
{
    return m_isScanning;
}

void AutoDiscoveryModel::setScanningStatus(bool scanning)
{
    if (m_isScanning != scanning) {
        m_isScanning = scanning;
        emit scanningStatusChanged(scanning);
    }
}


void AutoDiscoveryModel::startDiscovery()
{
    SPDLOG_DEBUG("Start to discovery");

    // clear data
    m_discoveredDevices.clear();
    m_genericTableModel->beginReset(true);
    m_genericTableModel->endReset();
    setScanningStatus(true);
    setDeviceCount(0);

    auto request = std::make_shared<Wisenet::Core::DiscoverDevicesRequest>();
    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::DiscoverDevices,
                this, request,
                [this](const QCoreServiceReplyPtr& reply)
    {
        if (!m_isScanning) {
            /* already canceled, ignore all responses.. */
            return;
        }

        QVariantList values;
        auto response = std::static_pointer_cast<Wisenet::Core::DiscoverDevicesResponse>(reply->responseDataPtr);
        for (auto& item : response->discoveredDevices) {
            auto itr = m_discoveredDevices.find(item.macAddress);
            if (itr == m_discoveredDevices.end()) {
                // just for test for (int i = 0 ; i < 20 ; i++ ){
                QVariantMap field;
                field[roleType()] = static_cast<int>(item.deviceType);
                field[roleModelName()] = QString::fromStdString(item.model);
                field[roleIpAddress()] = QString::fromStdString(item.ipAddress);
                field[roleMacAddress()] = QString::fromStdString(item.macAddress);
                field[rolePort()] = static_cast<int>(item.httpPort);
                field[roleStatus()] = static_cast<int>(RegisterDeviceStatus::Ready);
                values.push_back(field);
                //}
                m_discoveredDevices[item.macAddress] = item;
            }
        }

//        SPDLOG_DEBUG("DISCOVERY RESPONSE, response={}, map count={}, list count={}, CONTINUE={}",
//                     response->discoveredDevices.size(),
//                     m_discoveredDevices.size(),
//                     values.count(),
//                     response->isContinue);

        if (values.count() > 0) {
            m_genericTableModel->addFromList(values, false);
            setDeviceCount(m_discoveredDevices.size());
        }

        if (!response->isContinue) {
            setScanningStatus(false);
        }

    });
}

void AutoDiscoveryModel::cancelDiscovery()
{
    SPDLOG_DEBUG("Cancel to discovery");
    m_isScanning = false;
}

void AutoDiscoveryModel::startRegister()
{
    SPDLOG_DEBUG("Start to register selected devices");

    // get mac address from table to find in map
    int selectedRow = m_genericTableModel->getHighlightRow();
    if (selectedRow >= 0) {
        m_genericTableModel->setCellValue(selectedRow, 5, static_cast<int>(RegisterDeviceStatus::Registering));
        SPDLOG_DEBUG("selected row={}", selectedRow);

        /* just for testing */
        auto request = std::make_shared<Wisenet::Core::AddDeviceRequest>();
        request->deviceID =  QCoreServiceManager::Instance().CreateNewUUidString();
        request->connectionInfo.host = "192.168.15.88";
        request->connectionInfo.port = 80;
        request->connectionInfo.user = u8"admin";
        request->connectionInfo.password = u8"5tkatjd!";

        QCoreServiceManager::Instance().RequestToCoreService(
                    &Wisenet::Core::ICoreService::AddDevice,
                    this, request,
                    [this, selectedRow](const QCoreServiceReplyPtr& reply)
        {
            auto response = std::static_pointer_cast<Wisenet::Core::AddDeviceResponse>(reply->responseDataPtr);
            if (response->isFailed()) {
                m_genericTableModel->setCellValue(selectedRow, 5, static_cast<int>(RegisterDeviceStatus::ConnectionFailed));
            }
            else {
                m_genericTableModel->setCellValue(selectedRow, 5, static_cast<int>(RegisterDeviceStatus::Registered));
            }
        });

    }


}


void AutoDiscoveryModel::cancelRegister()
{
    SPDLOG_DEBUG("Cancel to register");

    // get mac address from table to find in map
    int selectedRow = m_genericTableModel->getHighlightRow();
    if (selectedRow >= 0) {
        auto selectedData = m_genericTableModel->data(QModelIndex(m_genericTableModel->index(selectedRow, 5)), Qt::DisplayRole);

        if (selectedData == RegisterDeviceStatus::Registering)
            m_genericTableModel->setCellValue(selectedRow, 5, static_cast<int>(RegisterDeviceStatus::Ready));
    }
}

