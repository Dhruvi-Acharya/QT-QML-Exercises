#include "ResourceUsageViewModel.h"
#include "ResourceMonitor.h"

ResourceUsageViewModel::ResourceUsageViewModel(QObject* parent) :
    QObject(parent)
{
    connect(ResourceMonitor::Instance(), &ResourceMonitor::longtermPeriod, this, &ResourceUsageViewModel::updateUsage);
}

ResourceUsageViewModel::~ResourceUsageViewModel()
{
    qDebug() << "ResourceUsageViewModel::~ResourceUsageViewModel()";
}

void ResourceUsageViewModel::updateUsage(double cpuUsage, long long usedMem, long long totalMem)
{
    //qDebug() << "ResourceDataEventType CPU " << cpuUsage << "%, Used:" << usedMem << "MB, Total:" << totalMem << "MB";
    setCpuUsage(cpuUsage);
    setMemoryUsage(usedMem, totalMem);
}
