#include "devicetreeitemtype.h"

DeviceTreeItemType::DeviceTreeItemType(QObject *parent) : QObject(parent),m_type(0)
{

}
DeviceTreeItemType::DeviceTreeItemType(DeviceTreeItemType &other)
{
    m_name = other.m_name;
    m_ip = other.m_ip;
    m_type = other.m_type;
}

DeviceTreeItemType::~DeviceTreeItemType()
{

}

QString DeviceTreeItemType::name()
{
    return m_name;
}

void DeviceTreeItemType::setName(const QString &name)
{
    m_name = name;
    emit nameChanged();
}

QString DeviceTreeItemType::ip()
{
    return m_ip;
}

void DeviceTreeItemType::setIp(const QString &ip)
{
    m_ip = ip;
    emit ipChanged();
}

int DeviceTreeItemType::type()
{
    return m_type;
}

void DeviceTreeItemType::setType(int type)
{
    m_type = type;
    emit typeChanged();
}
