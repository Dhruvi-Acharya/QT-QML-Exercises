#include "HealthMonitoringDeviceModel.h"

HealthMonitoringDeviceModel::HealthMonitoringDeviceModel(QObject *parent)
    : QObject(parent)
{
    m_cameraListProxyModel.setSourceModel(&m_cameraListModel);
}

HealthMonitoringDeviceModel::~HealthMonitoringDeviceModel()
{
    m_cameraListModel.clear();
}
