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
#include "DeviceTable.h"

#include <QString>
#include <QVariant>

#include <boost/core/ignore_unused.hpp>
#include <boost/algorithm/string.hpp>

#include "CoreServiceLogSettings.h"
#include "LogSettings.h"

namespace Wisenet
{
namespace Core
{

DeviceTable::DeviceTable(QSqlDatabase& database,QString& key)
    :m_database(database)
{
    m_crypto.Init(key);
}

void DeviceTable::CreateTable()
{
    if(m_database.tables().contains(QLatin1String("T_DEVICE"))) {
        SPDLOG_INFO("[T_DEVICE] Already exists.");
        return;
    }

    QSqlQuery query(m_database);

    QString queryString = "";

    queryString += "CREATE TABLE [T_DEVICE]( ";
    queryString += "[DEVICE_ID] VARCHAR(40) NOT NULL UNIQUE PRIMARY KEY";
    queryString += ",[USE] BOOLEAN NULL DEFAULT 1";
    queryString += ",[DEVICE_PROTOCOL_TYPE] INTEGER";
    queryString += ",[MEDIA_PROTOCOL_TYPE] INTEGER";
    queryString += ",[CONNECTION_TYPE] INTEGER";
    queryString += ",[CONNECTED_TYPE] INTEGER ";
    queryString += ",[IS_SSL] BOOLEAN NULL DEFAULT 0";
    queryString += ",[HOST] VARCHAR(128)";
    queryString += ",[PORT] INTEGER";
    queryString += ",[PRODUCT_P2P_ID] VARCHAR(64)";
    queryString += ",[DEVICE_TYPE] INTEGER";
    queryString += ",[USERNAME] VARCHAR(128)";
    queryString += ",[PASSWORD] VARCHAR(128)";
    queryString += ",[MODEL] VARCHAR(128)";
    queryString += ",[MAC] VARCHAR(128)";
    queryString += ",[NAME] VARCHAR(128)";
    queryString += ",[USAGE_DTLS_MODE] BOOLEAN NULL DEFAULT 1";
    queryString += ",[REGISTRATION_MSEC] INTEGER ";
    queryString += ")";

    if(!query.exec(queryString)) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void DeviceTable::GetAll(std::map<uuid_string, Device::Device> &devices)
{
    QSqlQuery query(m_database);

    if(!query.exec("SELECT * FROM [T_DEVICE]")) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    while(query.next()) {
        Device::Device device;
        device.deviceID = query.value(0).toString().toStdString();
        device.connectionInfo.use = query.value(1).toBool();
        device.connectionInfo.deviceProtocolType = static_cast<Device::DeviceProtocolType>(query.value(2).toInt());
        device.connectionInfo.mediaProtocolType = static_cast<Device::MediaProtocolType>(query.value(3).toInt());
        device.connectionInfo.connectionType = static_cast<Device::ConnectionType>(query.value(4).toInt());
        device.connectionInfo.connectedType = static_cast<Device::ConnectedType>(query.value(5).toInt());
        device.connectionInfo.isSSL = query.value(6).toBool();
        device.connectionInfo.host = query.value(7).toString().toStdString();
        device.connectionInfo.port = static_cast<unsigned short>(query.value(8).toUInt());
        device.connectionInfo.sslPort = static_cast<unsigned short>(query.value(8).toUInt());
        device.connectionInfo.productP2PID = query.value(9).toString().toStdString();
        device.deviceType = static_cast<Device::DeviceType>(query.value(10).toInt());
        device.connectionInfo.user = query.value(11).toString().toStdString();
        device.connectionInfo.password = m_crypto.Decrypt(query.value(12).toByteArray()).toStdString();

        device.modelName = query.value(13).toString().toStdString();
        device.macAddress = query.value(14).toString().toStdString();

        device.name = query.value(15).toString().toStdString();

        device.connectionInfo.usageDtlsMode = query.value(16).toBool();
        device.connectionInfo.mac = boost::erase_all_copy(device.macAddress, ":");

        device.connectionInfo.registrationMsec = query.value(17).toInt();

        SPDLOG_DEBUG("DeviceTable::GetAll ip:{}, use:{}, usageDtlsMode:{}", device.connectionInfo.host, device.connectionInfo.use, device.connectionInfo.usageDtlsMode);


        devices.emplace(device.deviceID, device);
    }
}

void DeviceTable::Save(const Device::Device &device)
{
    QSqlQuery query(m_database);

    QString queryString = "";
    queryString += "INSERT OR REPLACE INTO [T_DEVICE] ";
    queryString += "(DEVICE_ID, USE, DEVICE_PROTOCOL_TYPE, MEDIA_PROTOCOL_TYPE, CONNECTION_TYPE, CONNECTED_TYPE, IS_SSL, HOST, PORT, PRODUCT_P2P_ID, DEVICE_TYPE, USERNAME, PASSWORD, MODEL, MAC, NAME, USAGE_DTLS_MODE, REGISTRATION_MSEC) ";
    queryString += "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";

    query.prepare(queryString);

    // (1) DEVICE_ID
    query.addBindValue(QString(device.deviceID.c_str()));
    // (2) USE
    query.addBindValue(device.connectionInfo.use);
    // (3) DEVICE_PROTOCOL_TYPE
    query.addBindValue(static_cast<int>(device.connectionInfo.deviceProtocolType));
    // (4) MEDIA_PROTOCOL_TYPE
    query.addBindValue(static_cast<int>(device.connectionInfo.mediaProtocolType));
    // (5) CONNECTION_TYPE
    query.addBindValue(static_cast<int>(device.connectionInfo.connectionType));
    // (6) CONNECTED_TYPE
    query.addBindValue(static_cast<int>(device.connectionInfo.connectedType));
    // (7) IS_SSL
    query.addBindValue(device.connectionInfo.isSSL);
    // (8) HOST
    query.addBindValue(QString(device.connectionInfo.host.c_str()));
    // (9) PORT
    query.addBindValue(device.connectionInfo.isSSL? device.connectionInfo.sslPort: device.connectionInfo.port);
    // (10) PRODUCT_P2P_ID
    query.addBindValue(QString(device.connectionInfo.productP2PID.c_str()));
    // (11) DEVICE_TYPE
    query.addBindValue(static_cast<int>(device.deviceType));
    // (12) USERNAME
    query.addBindValue(QString(device.connectionInfo.user.c_str()));
    // (13) PASSWORD
    query.addBindValue(m_crypto.Encrypt(QString(device.connectionInfo.password.c_str())));
    // (14) MODEL
    query.addBindValue(QString(device.modelName.c_str()));
    // (15) MAC
    query.addBindValue(QString(device.macAddress.c_str()));
    // (16) NAME
    query.addBindValue(QString(device.name.c_str()));
    // (17) USAGE_DTLS_MODE
    query.addBindValue(device.connectionInfo.usageDtlsMode);
    // (18) REGISTRATION_MSEC
    query.addBindValue(device.connectionInfo.registrationMsec);

    SPDLOG_DEBUG("DeviceTable::Save ip:{}, use:{}, usageDtlsMode:{}", device.connectionInfo.host, device.connectionInfo.use, device.connectionInfo.usageDtlsMode);

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void DeviceTable::Remove(const uuid_string &deviceID)
{
    QSqlQuery query(m_database);

    query.prepare("DELETE FROM [T_DEVICE] WHERE DEVICE_ID=?");
    query.addBindValue(QString(deviceID.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

}
}
