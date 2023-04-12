#pragma once

#include <QObject>
#include <QDebug>
#include "QCoreServiceManager.h"

class ResourceUsageViewModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double cpuUsage READ cpuUsage WRITE setCpuUsage NOTIFY cpuUsageChanged)
    Q_PROPERTY(QString cpuUsageString READ cpuUsageString NOTIFY cpuUsageStringChanged)
    Q_PROPERTY(double memoryUsage READ memoryUsage NOTIFY memoryUsageChanged)
    Q_PROPERTY(QString memoryUsageString READ memoryUsageString NOTIFY memoryUsageStringChanged)
    Q_PROPERTY(QString usedMemory READ usedMemory NOTIFY usedMemoryChanged)
    Q_PROPERTY(QString totalMemory READ totalMemory NOTIFY totalMemoryChanged)

public:
    static ResourceUsageViewModel* getInstance()
    {
        static ResourceUsageViewModel instance;
        return &instance;
    }

    explicit ResourceUsageViewModel(QObject *parent = nullptr);
    ~ResourceUsageViewModel();

    double cpuUsage(){ return m_cpuUsage; }
    QString cpuUsageString(){ return QString::number(m_cpuUsage, 'f', 1); }
    double memoryUsage(){ return m_usedMemory/m_totalMemory*100; }
    QString memoryUsageString(){ return QString::number(m_usedMemory/m_totalMemory*100, 'f', 1); }
    QString usedMemory(){ return QString::number(m_usedMemory/1024, 'f', 1); }
    QString totalMemory(){ return QString::number(m_totalMemory/1024, 'f', 1); }

    void setCpuUsage(double cpuUsage)
    {
        m_cpuUsage = cpuUsage;
        cpuUsageChanged(m_cpuUsage);
        cpuUsageStringChanged();
    }

    void setMemoryUsage(double usedMemory, double totalMemory)
    {
        m_usedMemory = usedMemory;
        m_totalMemory = totalMemory;

        memoryUsageChanged();
        memoryUsageStringChanged();
        usedMemoryChanged();
        totalMemoryChanged();
    }

public slots:
    void updateUsage(double cpuUsage, long long usedMem, long long totalMem);

signals:
    void cpuUsageChanged(double cpuUsage);
    void cpuUsageStringChanged();
    void memoryUsageChanged();
    void memoryUsageStringChanged();
    void usedMemoryChanged();
    void totalMemoryChanged();

private:

    double m_cpuUsage = 0.0;
    double m_usedMemory = 0.0;
    double m_totalMemory = 0.0;
};
