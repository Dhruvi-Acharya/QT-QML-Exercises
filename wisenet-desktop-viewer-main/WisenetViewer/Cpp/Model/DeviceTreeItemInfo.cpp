#include "DeviceTreeItemInfo.h"

DeviceTreeItemInfo::DeviceTreeItemInfo(QObject *parent) : BaseTreeItemInfo(parent)
{
}

DeviceTreeItemInfo::DeviceTreeItemInfo(const DeviceTreeItemInfo &other) : BaseTreeItemInfo(other)
{
    m_modelName = other.modelName();
    m_deviceName = other.deviceName();
    m_deviceAddress = other.deviceAddress();
    m_channelNumber = other.channelNumber();
}

DeviceTreeItemInfo::~DeviceTreeItemInfo()
{
    //qDebug() << "DeviceTreeItemInfo::~DeviceTreeItemInfo()";
}

void DeviceTreeItemInfo::setModelName(QString modelName)
{
    m_modelName = modelName;
}

void DeviceTreeItemInfo::setDeviceName(QString deviceName)
{
    m_deviceName = deviceName;
}

void DeviceTreeItemInfo::setAddress(QString address)
{
    m_deviceAddress = address;
}

void DeviceTreeItemInfo::setChannelNumber(const int channelIndex)
{
    m_channelNumber = channelIndex;
}

void DeviceTreeItemInfo::setDeviceUuid(QString deviceUuid)
{
    m_deviceUuid = deviceUuid;
}

void DeviceTreeItemInfo::setSmartSearchCap(bool smartSearchCap)
{
    m_smartSearchCap = smartSearchCap;
}
