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
#pragma once
#include <qqml.h>
#include <QAbstractTableModel>
#include <set>
#include "QCoreServiceManager.h"

class ManualDiscoveryViewModel : public QAbstractTableModel
{
    Q_OBJECT
    QML_ELEMENT
    QML_ADDED_IN_MINOR_VERSION(1)
    Q_PROPERTY(int highlightRow READ highlightRow NOTIFY highlightRowChanged)

public:
    Q_INVOKABLE void reset();
    Q_INVOKABLE void startRegister(QString type, QString id, QString password,
                                   QString host, QString endIP, QString port, bool usageDtlsMode);
    Q_INVOKABLE void startP2pRegister(QString productId, QString id, QString password, bool usageDtlsMode);
    Q_INVOKABLE void startS1Register(QString productId, QString id, QString password, bool isSIP);
    Q_INVOKABLE void cancelRegister();
    Q_INVOKABLE void sort(int column, Qt::SortOrder order) override;

    Q_INVOKABLE void setHighlightRow(int rowNum, int modifiers);
    Q_INVOKABLE QString translate(QVariant text);

    enum Role{
        HighlightRole = Qt::UserRole,
        HoveredRole
    };
   // Q_ENUM(ManualDiscoveryRoles);

    enum ManualRegisterTitle {

        ManualRegisterIp = 0,
        ManualRegisterPort,
        ManualRegisterStatus
    };
    Q_ENUM(ManualRegisterTitle);

    int highlightRow() const;
signals:
    void deviceCountChanged(int count);
    void highlightRowChanged();
    void connectFailDevice(QString msg);
    void connectFailDeviceRelayMode();
    void connectFailDeviceRelayModeManual();


    void deviceRegistering();
    void deviceRegisterCompleted(bool success);
    void monitoringPageRegistering(bool connecting);
public:
    ManualDiscoveryViewModel();
    ~ManualDiscoveryViewModel();

    static ManualDiscoveryViewModel* getInstance(){
        static ManualDiscoveryViewModel instance;
        return &instance;
    }

    int rowCount(const QModelIndex & = QModelIndex()) const override;
    int columnCount(const QModelIndex & = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
private:
    void staticDevice(bool isSSL, std::string id, std::string password,
                      std::string startIP, std::string endIP, int port);
    void staticIPv6Device(bool isSSL, std::string id, std::string password,
                      std::string startIP, int port);
    void ddnsUrlDevice(bool isSSL, Wisenet::Device::ConnectionType type, std::string id, std::string password,
                    std::string ddnsId, int port, bool dtls);

    void registerDevice(QVector<Wisenet::Device::DeviceConnectInfo> discoveredDevices);
    void UpdateSortData(int column, Qt::SortOrder order);
    int getCurrentRow(int row);
    bool setCellValue(int rowNum, int columnNum, const QVariant &data);

    QVariant getDataDisplayRole(const QModelIndex &index) const;
    QVector<QVector<QVariant>> m_data;
    QVector<int> m_index;
    QVector<int> m_sorted;
    QVector<int> m_roles;
    QStringList m_columnNames;

    int m_highlightRow;
    int m_lastHighlightRow;
    std::set<int> m_highlightRows;

    int m_totalRegisterCount{0};
    int  m_currentTryRegisterCount{0};
    bool m_registerComplete{true};
};
