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

#include <QObject>
#include <set>
#include "QCoreServiceManager.h"

class DeviceListViewModel : public QObject
{
    Q_OBJECT
    Q_ENUMS(DeviceIpType)

    Q_PROPERTY(int deviceCount READ deviceCount NOTIFY deviceCountChanged)
   // Q_PROPERTY(int channelCount READ channelCount NOTIFY channelCountChanged)

    Q_PROPERTY(QString deviceName READ deviceName NOTIFY deviceNameChanged)
    Q_PROPERTY(QString deviceGuid READ deviceGuid NOTIFY deviceGuidChanged)
    Q_PROPERTY(QString model READ model NOTIFY modelChanged)
    Q_PROPERTY(QString version READ version NOTIFY versionChanged)
    Q_PROPERTY(QString macAddress READ macAddress NOTIFY macAddressChanged)
    Q_PROPERTY(QString deviceCertResult READ deviceCertResult NOTIFY deviceCertResultChanged)
    Q_PROPERTY(QString connectedType READ connectedType NOTIFY connectedTypeChanged)
    Q_PROPERTY(QString host READ host WRITE setHost NOTIFY hostChanged)
    Q_PROPERTY(QString id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(QString url READ url NOTIFY urlChanged)
    Q_PROPERTY(QString url2 READ url2 NOTIFY url2Changed)
    Q_PROPERTY(QString supportWebSocket READ supportWebSocket NOTIFY supportWebSocketChanged)
    Q_PROPERTY(DeviceListViewModel::DeviceIpType ipType READ ipType WRITE setIpType NOTIFY ipTypeChanged)
    Q_PROPERTY(int port READ port WRITE setPort NOTIFY portChanged)
    Q_PROPERTY(int streamingProtocol READ streamingProtocol WRITE setStreamingProtocol NOTIFY streamingProtocolChanged)
    Q_PROPERTY(bool connection READ connection WRITE setConnection NOTIFY connectionChanged)
    Q_PROPERTY(bool dtls READ dtls WRITE setDtls NOTIFY dtlsChanged)

   // Q_PROPERTY(QVector<QString> channelGuidList READ channelGuidList NOTIFY channelGuidListChanged)
    Q_PROPERTY(QVector<QString> deviceGuidList READ deviceGuidList NOTIFY deviceGuidListChanged)

public:
    Q_INVOKABLE void reset();
    Q_INVOKABLE void deleteDevices();
    Q_INVOKABLE void selectDevices(QVector<QString> deviceIdList);
    Q_INVOKABLE void changeConnectInfo(bool connection, int ipType, int streamingType, int port, QString ip, QString deviceName, bool dtls);
    Q_INVOKABLE void changeCredential(QString id, QString password);
    Q_INVOKABLE QString translate(QVariant text);
    Q_INVOKABLE void resetDeviceName();
    Q_INVOKABLE void openBrowser();

    enum DeviceIpType{
        DeviceStaticIp = 0,
        DeviceStaticIpSSL,
        DeviceStaticIpv6,
        DeviceStaticIpv6SSL,
        DeviceUrl,
        DeviceUrlSSL,
        DeviceDdnsP2p,
        DeviceS1Ddns,
        DeviceS1Sip,
    };


    DeviceListViewModel(QObject* parent = nullptr);

    QString deviceName(){ return m_deviceName; }
    QString deviceGuid(){ return m_deviceGuid; }
    QString model(){ return m_model; }
    QString version(){ return m_version; }
    QString macAddress(){ return m_macAddress; }
    QString deviceCertResult();
    QString connectedType() { return m_connectedType;}
    QString webViewer(){ return m_model; }
    QString host(){ return m_host; }
    QString id(){ return m_id; }
    QString password(){ return m_password; }
    QString url(){ return m_url; }
    QString url2(){ return m_url2; }
    bool connection() { return m_connection; }
    bool supportWebSocket() { return m_supportWebSocket; }
    bool dtls() { return m_dtls; }

    // QVector<QString> channelGuidList() { return m_channelGuidList;}
    QVector<QString> deviceGuidList() { return m_deviceGuidList;}

    int deviceCount(){ return m_deviceGuidList.size();}
   // int channelCount(){ return m_channelGuidList.size();}
    DeviceListViewModel::DeviceIpType ipType() { return  m_ipType;}
    int port() { return  m_port;}
    int streamingProtocol() { return m_streamingProtocol; }

    void setDeviceName(QString name);
    void setDeviceGuid(QString name);
    void setModel(QString model);
    void setVersion(QString version);
    void setMacAddress(QString mac);
    void setDeviceCertResult(QString result);
    void setConnectedType(QString connectedType);
    void setUrl(QString url);
    void setUrl2(QString url);
    void setSecondUrl(QString url);
    void setSupportWebSocket(bool webSocket);

    void setHost(QString host);
    void setId(QString id);
    void setPassword(QString password);
    void setIpType(DeviceListViewModel::DeviceIpType type);
    void setStreamingProtocol(int type);
    void setPort(int port);
    void setConnection(bool connect);
    void setDtls(bool param);

    void coreServiceEventTriggered(QCoreServiceEventPtr event);
signals:
    void resultMessage(bool success, QString msg);
    void removedDevice();
    void deviceCountChanged();
   // void channelCountChanged();

    void deviceGuidChanged();
    void deviceNameChanged();
    void modelChanged();
    void versionChanged();
    void macAddressChanged();
    void ipTypeChanged();
    void hostChanged();
    void portChanged();
    void streamingProtocolChanged();
    void connectionChanged();
    void idChanged();
    void passwordChanged();
    void urlChanged();
    void url2Changed();
    void deviceCertResultChanged();
    void connectedTypeChanged();
    void supportWebSocketChanged();
    void dtlsChanged();

    void deviceGuidListChanged();

private:
    QString makeUrl(Wisenet::Device::ConnectedType connectedType, bool supportWebSocket, std::string ip, int port, bool useSSL, Wisenet::Device::DeviceType dType);
    void disconnectDevice();
    void saveDevice(bool use, int ipTypeIndex, int streamingTypeIndex, int newPort, QString newIp, QString deviceName, bool dtls);
    void connectDevice(std::vector<Wisenet::Device::Device> devices);
    void saveDeviceCredential(QString id, QString pass);

    QString m_deviceGuid;
    QString m_deviceName;
    QString m_model;
    QString m_version;
    QString m_macAddress;
    QString m_url;
    QString m_url2;
    QString m_deviceCertResult;
    QString m_connectedType;

    QString m_host;
    QString m_id;
    QString m_password;

    DeviceListViewModel::DeviceIpType m_ipType;
    int m_port;
    int m_streamingProtocol;
    bool m_connection;
    bool m_supportWebSocket;
    bool m_dtls;
    QVector<QString>    m_deviceGuidList;
};

Q_DECLARE_METATYPE(DeviceListViewModel::DeviceIpType)

