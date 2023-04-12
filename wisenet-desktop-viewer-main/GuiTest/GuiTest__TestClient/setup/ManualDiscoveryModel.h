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
#pragma once

#include <QObject>
#include <QPointer>
#include <QDebug>
#include <bpptablemodel.h>
#include <unordered_map>
#include <string>
#include "QCoreServiceManager.h"

class ManualDiscoveryModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString roleIpAddress READ roleIpAddress CONSTANT)
    Q_PROPERTY(QString rolePort READ rolePort CONSTANT)
    Q_PROPERTY(QString roleStatus READ roleStatus CONSTANT)
    Q_PROPERTY(int deviceCount READ getDeviceCount WRITE setDeviceCount NOTIFY deviceCountChanged)
public:
    explicit ManualDiscoveryModel(QObject *parent = nullptr);
    ~ManualDiscoveryModel();

    static void registerQml();
    enum RegisterDeviceStatus {Ready = 0, Registering, Registered, ConnectionFailed, AuthFailed};
    Q_ENUM(RegisterDeviceStatus)
    Q_INVOKABLE void startDiscovery(QString startIP, QString endIP, QString port);
    Q_INVOKABLE void startRegister(QString id, QString password);
    Q_INVOKABLE void cancelRegister();

    Q_INVOKABLE void setGenericTableModel(bpp::TableModel* model);

public:
    QString roleIpAddress() const { return m_roleIpAddress; }
    QString rolePort() const { return m_rolePort; }
    QString roleStatus() const { return m_roleStatus; }
    int getDeviceCount() const { return m_deviceCount; }
    void setDeviceCount(const int count) {
        if (m_deviceCount != count) {
            m_deviceCount = count;
            emit deviceCountChanged(count);
        }
    }

    static const QString m_roleIpAddress;
    static const QString m_rolePort;
    static const QString m_roleStatus;
signals:
    void deviceCountChanged(int count);
private:
    QPointer<bpp::TableModel> m_genericTableModel;
    std::unordered_map<std::string, Wisenet::Core::DiscoveryDeviceInfo> m_discoveredDevices;
    int m_deviceCount;
};
