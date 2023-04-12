#pragma once
#include "BaseTreeItemInfo.h"

class DeviceTreeItemInfo : public BaseTreeItemInfo
{
    Q_OBJECT
    Q_PROPERTY(QString modelName READ modelName CONSTANT)
    Q_PROPERTY(QString deviceName READ deviceName CONSTANT)
    Q_PROPERTY(QString deviceAddress READ deviceAddress CONSTANT)
    Q_PROPERTY(int channelNumber READ channelNumber CONSTANT)
    Q_PROPERTY(QString deviceUuid READ deviceUuid CONSTANT)
    Q_PROPERTY(bool smartSearchCap READ smartSearchCap CONSTANT)

public:
    explicit DeviceTreeItemInfo(QObject *parent = 0);
    DeviceTreeItemInfo(const DeviceTreeItemInfo &other);
    virtual ~DeviceTreeItemInfo();

    // For device
    QString modelName() const {return m_modelName;}
    QString deviceName() const {return m_deviceName;}
    QString deviceAddress() const {return m_deviceAddress;}
    int channelNumber() const {return m_channelNumber;}
    QString getChannelId() const {return m_uuid + "_" + QString::number(m_channelNumber);}
    QString deviceUuid() const {return m_deviceUuid;}
    bool smartSearchCap() const {return m_smartSearchCap;}

    void setModelName(QString modelName);
    void setDeviceName(QString deviceName);
    void setAddress(QString address);
    void setChannelNumber(const int channelIndex);
    void setDeviceUuid(QString itemId);
    void setSmartSearchCap(bool smartSearchCap);

private:
    QString m_modelName="";
    QString m_deviceName="";
    QString m_deviceAddress="";
    int m_channelNumber = 0;
    QString m_deviceUuid = "";
    bool m_smartSearchCap = false;
};
