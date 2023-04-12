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

#include <QAbstractTableModel>
#include <set>
#include "QCoreServiceManager.h"

class AutoDiscoveryViewModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_PROPERTY(bool scanningStatus READ scanningStatus WRITE setScanningStatus NOTIFY scanningStatusChanged)
    Q_PROPERTY(bool ipv6Mode READ ipv6Mode WRITE setIpv6Mode NOTIFY ipv6ModeChanged)
    Q_PROPERTY(int deviceCount READ deviceCount WRITE setDeviceCount NOTIFY deviceCountChanged)
    Q_PROPERTY(int highlightRow READ highlightRow NOTIFY highlightRowChanged)
    Q_PROPERTY(int highlightRowCount READ highlightRowCount NOTIFY highlightRowCountChanged)
    Q_PROPERTY(int selectedRowCount READ selectedRowCount NOTIFY selectedRowCountChanged)
    Q_PROPERTY(int needPasswordCount READ needPasswordCount WRITE setNeedPasswordCount NOTIFY needPasswordCountChanged)
    Q_PROPERTY(int conflictIpCount READ conflictIpCount WRITE setConflictIpCount NOTIFY conflictIpCountChanged)
public:
    Q_INVOKABLE void reset();
    Q_INVOKABLE void startDiscoveryAndRegister(QString type, QString id, QString password);
    Q_INVOKABLE void startDiscovery(bool clear = true);
    Q_INVOKABLE void cancelDiscovery();
    Q_INVOKABLE void startRegister(QString id, QString password);
    Q_INVOKABLE void cancelRegister();
    Q_INVOKABLE void startInitPasswordSetting(QString password);
    Q_INVOKABLE void startIpSetting( bool dhcp, QString id, QString password, QString port, QString startIp, QString subnet, QString gateway);
    Q_INVOKABLE void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

    Q_INVOKABLE void setHighlightRow(int rowNum, int modifiers);
    Q_INVOKABLE void setHoveredRow(int rowNum, bool hovered);
    Q_INVOKABLE bool setCheckState(int rowNum, bool checked);
    Q_INVOKABLE bool setAllCheckState(bool checked);

    Q_INVOKABLE QVector<int> getHighlightRows() const;
    Q_INVOKABLE bool setCellValue(int rowNum, int columnNum, const QVariant& data);
    Q_INVOKABLE bool checkInitDeviceSelect();
    Q_INVOKABLE QString translate(QVariant text);

    Q_INVOKABLE QString getFirstItemIp();

    AutoDiscoveryViewModel();
    ~AutoDiscoveryViewModel();

    static AutoDiscoveryViewModel* getInstance(){
        static AutoDiscoveryViewModel instance;
        return &instance;
    }



    enum RegisterDeviceStatus {
        Ready = 0,
        Registering,
        Registered,
        ConnectionFailed,
        AuthFailed,
        AccountBlocked,
        InitDevicePassword,
        InitDevicePasswordChanging,
        InitDevicePasswordChanged,
        InitDevicePasswordChangeFailed,
        IpChanging,
        IpChanged,
        IpChangeFailed,
        IpConflict,
    };
    Q_ENUM(RegisterDeviceStatus);

    enum AutoDiscoveryTitle {
        AutoDiscoveryIsHttps = 0,
        AutoDiscoveryCheck ,
        AutoDiscoveryModel ,

        AutoDiscoveryIp,
        AutoDiscoveryMac,
        AutoDiscoveryPort,
        AutoDiscoveryStatus
    };
    Q_ENUM(AutoDiscoveryTitle);

    int deviceCount() const { return m_deviceCount; }
    void setDeviceCount(const int count) {
        if (m_deviceCount != count) {
            m_deviceCount = count;
            emit deviceCountChanged(count);
        }
    }

    bool scanningStatus();
    void setScanningStatus(bool scanning);

    bool ipv6Mode();
    void setIpv6Mode(bool isIpv6);

    int highlightRow() const;
    int highlightRowCount(){ return (int)m_highlightRows.size(); }
    int selectedRowCount();
    int needPasswordCount();
    int conflictIpCount();
    void setNeedPasswordCount(int count);
    void setConflictIpCount(int count);
signals:
    void scanningStatusChanged(bool status);
    void ipv6ModeChanged(bool isIpv6);
    void deviceCountChanged(int count);
    void highlightRowChanged();
    void highlightRowCountChanged();
    void selectedRowCountChanged();
    void initializeDeviceFound();
    void networkDuplicatedDeviceFound();
    void deviceDiscoveryRegister();
    void deviceIpChangeNeed(QString mac, QString newIp);

    void connectFailDeviceFound();
    int needPasswordCountChanged(int count);
    int conflictIpCountChanged(int count);

    void deviceRegistering();
    void deviceRegisterCompleted(bool success);
    void monitoringPageRegistering(bool connecting);
public:
    int rowCount(const QModelIndex & = QModelIndex()) const override;
    int columnCount(const QModelIndex & = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    enum Role{
        HighlightRole = Qt::UserRole,
        HoveredRole
    };

    int getCurrentRow(int row){
        for(int i=0; i< m_index.size();i++){
            if(m_index[i] == row)
                return i;
        }
        return 0;
    }
    int m_totalRegisterCount{0};
    int  m_currentTryRegisterCount{0};
    bool m_registerComplete{true};
private:
    void clearData(bool clear);
    void updateNonce();
    void updateSortData(int column, Qt::SortOrder order);

    QVariant getDataDisplayRole(const QModelIndex &index) const;
    bool checkDuplicateDevice(bool onlyInitPassword, bool ipSetting);

    void InitializeNotify();     // 비동기 명령 호출 전 m_canContinue, m_isSuccess false로 초기화 함.
    void Notify(bool isSuccess); // 비동기 응답 처리가 완료된 경우 응답값의 정상 여부와 함께 호출.
    bool Wait(long timeoutSec = 1800); // true: timeout 시간 안에 Notify(true)가 호출된 경우, false: timeout이 지났거나, Nofity(false)가 호출된 경우.
private:
    std::unordered_map<std::string, Wisenet::Core::DiscoveryDeviceInfo> m_discoveredDevices;

    QVector<QVector<QVariant>> m_data;
    QVector<int> m_index;
    QVector<int> m_sorted;
    QVector<int> m_roles;
    int m_deviceCount = 0;
    std::atomic_bool m_isScanning;
    std::atomic_bool m_isIpv6Mode;

    int m_hoveredRow = -1;
    int m_highlightRow = -1;
    int m_lastHighlightRow = -1;
    int m_conflictIpCount = 0;
    int m_needPasswordCount = 0;
    std::set<int> m_highlightRows;

    QStringList m_columnNames;

    std::condition_variable m_condition;
    std::mutex m_mutex;
    bool m_canContinue = false; //Wait or Go 여부
    bool m_isSuccess = false;   //비동기 응답 성공 여부
};

