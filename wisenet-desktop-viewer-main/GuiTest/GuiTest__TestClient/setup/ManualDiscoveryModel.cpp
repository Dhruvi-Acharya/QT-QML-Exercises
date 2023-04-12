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
#include "ManualDiscoveryModel.h"
#include "LogSettings.h"
#include <QQmlEngine>

const QString ManualDiscoveryModel::m_roleIpAddress = QString("ip");
const QString ManualDiscoveryModel::m_rolePort = QString("port");
const QString ManualDiscoveryModel::m_roleStatus = QString("status");

ManualDiscoveryModel::ManualDiscoveryModel(QObject *parent) : QObject(parent)
{
    SPDLOG_DEBUG("ManualDiscoveryModel()");
}

ManualDiscoveryModel::~ManualDiscoveryModel()
{
    SPDLOG_DEBUG("qml: ~ManualDiscoveryModel()");
    qDebug() << "qml: ~ManualDiscoveryModel()";
}

void ManualDiscoveryModel::registerQml()
{
    qmlRegisterType<ManualDiscoveryModel>("Wisenet.ManualDiscovery", 0, 1, "ManualDiscoveryModel");
}

void ManualDiscoveryModel::setGenericTableModel(bpp::TableModel* model)
{
    SPDLOG_DEBUG("ManualDiscoveryModel::setGenericTableModel()");
    m_genericTableModel = model;
}

void ManualDiscoveryModel::startDiscovery(QString startIP, QString endIP, QString port)
{
    SPDLOG_DEBUG("Start to discovery{} - {} - {}", startIP.toStdString(), endIP.toStdString(), port.toStdString());

    m_genericTableModel->beginReset(true);
    setDeviceCount(0);
    QVariantList values;

    /*auto start =boost::asio::ip::address::from_string(startIP.toStdString());
    auto end =boost::asio::ip::address::from_string(endIP.toStdString());
    */

    std::stringstream sstream(startIP.toStdString());
    int a,b,c,d; //to store the 4 ints
    char ch; //to temporarily store the '.'
    sstream >> a >> ch >> b >> ch >> c >> ch >> d;
    int start = d;
    sstream.clear();

    std::stringstream estream(endIP.toStdString());
    SPDLOG_DEBUG("{}", endIP.toStdString());
    int e,f,g,h; //to store the 4 ints
    char ch2; //to temporarily store the '.'

    estream >> e >> ch2 >> f >> ch2 >> g >> ch2 >> h;
    int end = h;

    SPDLOG_DEBUG("{} {}", std::to_string(start), std::to_string(end));

    std::stringstream stream;
    SPDLOG_DEBUG("666");
    for (auto i = start; i <= end; i++) {
        Wisenet::Core::DiscoveryDeviceInfo device;
        QVariantMap field;
        stream.str("");
        stream << a << ch << b << ch << c << ch << i;
        device.ipAddress = stream.str();
        device.httpPort = port.toInt();
        field[roleIpAddress()] = QString::fromStdString(device.ipAddress);
        field[rolePort()] = port;
        field[roleStatus()] = static_cast<int>(RegisterDeviceStatus::Ready);
        values.push_back(field);
        m_discoveredDevices[device.ipAddress] = device;
        SPDLOG_DEBUG("ip = {}", device.ipAddress);
    }

    if (values.count() > 0) {
        SPDLOG_DEBUG("values.count() = {} m_discoveredDevices.size(){}", values.count(), m_discoveredDevices.size());
        m_genericTableModel->addFromList(values, false);
        setDeviceCount(m_discoveredDevices.size());
    }
    m_genericTableModel->endReset();
}

void ManualDiscoveryModel::startRegister(QString id, QString password)
{
    SPDLOG_DEBUG("Start to register selected devices");

    auto selectedRows = m_genericTableModel->getHighlightRows();

    for (auto &selectedRow : selectedRows) {

        m_genericTableModel->setCellValue(selectedRow, 2, static_cast<int>(RegisterDeviceStatus::Registering));

        SPDLOG_DEBUG("selected row={}", selectedRow);

        QString rowString = m_genericTableModel->getRowString(selectedRow);

        QJsonDocument doc = QJsonDocument::fromJson(rowString.toUtf8());
        QJsonObject obj = doc.object();
        QString ip = obj["IP Address"].toString();

        auto itr = m_discoveredDevices.find(ip.toStdString());
        if (itr != m_discoveredDevices.end())
        {
            auto device = itr->second;
            auto request = std::make_shared<Wisenet::Core::AddDeviceRequest>();
            request->deviceID = QCoreServiceManager::Instance().CreateNewUUidString();
            request->connectionInfo.host = device.ipAddress;
            request->connectionInfo.port = device.httpPort;
            request->connectionInfo.user = id.toStdString();
            request->connectionInfo.password = password.toStdString();

            QCoreServiceManager::Instance().RequestToCoreService(
                        &Wisenet::Core::ICoreService::AddDevice,
                        this, request,
                        [this, selectedRow](const QCoreServiceReplyPtr& reply)
            {
                auto response = std::static_pointer_cast<Wisenet::ResponseBase>(reply->responseDataPtr);

                if (response->isFailed()) {
                    m_genericTableModel->setCellValue(selectedRow, 2, static_cast<int>(RegisterDeviceStatus::ConnectionFailed));
                }
                else {
                    m_genericTableModel->setCellValue(selectedRow, 2, static_cast<int>(RegisterDeviceStatus::Registered));
                }
            });
        }
    }
}
void ManualDiscoveryModel::cancelRegister()
{
    SPDLOG_DEBUG("Cancel to register");

    int selectedRow = m_genericTableModel->getHighlightRow();
    if (selectedRow >= 0) {
        auto selectedData = m_genericTableModel->data(QModelIndex(m_genericTableModel->index(selectedRow, 2)), Qt::DisplayRole);

        if (selectedData == RegisterDeviceStatus::Registering) {
            m_genericTableModel->setCellValue(selectedRow, 2, static_cast<int>(RegisterDeviceStatus::Ready));
        }
    }
}











