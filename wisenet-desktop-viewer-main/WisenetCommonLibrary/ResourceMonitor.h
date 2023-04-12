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

#ifdef Q_OS_WINDOWS
#include <Pdh.h>
#include <PdhMsg.h>
#include <tchar.h>
#endif

#ifdef Q_OS_MACOS
#include <mach/mach_init.h>
#include <mach/mach_error.h>
#include <mach/mach_host.h>
#include <mach/vm_map.h>
#endif

#ifdef Q_OS_LINUX
#include "stdlib.h"
#include "stdio.h"
#include "sys/types.h"
#include "sys/sysinfo.h"
#endif

#include <boost/asio.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/io_context.hpp>
#include "ThreadPool.h"

class ResourceMonitor : public QObject
{
    Q_OBJECT

public:
    static ResourceMonitor* Instance()
    {
        static ResourceMonitor instance;
        return &instance;
    }

    explicit ResourceMonitor(QObject *parent = nullptr);
    ~ResourceMonitor();

    void    restartResourceCheckTimer();

    void    getData(double& cpuUsage, long long& usedMem, long long& totalMem, double& currentProcessCpuUsage);

    double  getCpuUsage();
    void    getMemoryUsage(long long& usedMem, long long& totalMem);

    double  getWindowsCpuUsage();
    void    getWindowsMemoryUsage(long long &usedMem, long long &totalMem);

    double  getMacCpuUsage();
    void    getMacMemoryUsage(long long &usedMem, long long &totalMem);

    double  getLinuxCpuUsage();
    void    getLinuxMemoryUsage(long long &usedMem, long long &totalMem);

    double  getCurrentProcessCpuUsage();
signals:
    void    normalPeriod(double cpuUsage, long long usedMem, long long totalMem, double currentProcess);
    void    longtermPeriod(double cpuUsage, long long usedMem, long long totalMem);

private:
    double  calculateMacCpuUsage(unsigned long long idleTicks, unsigned long long totalTicks);

#ifdef Q_OS_WINDOWS
    PDH_HQUERY cpuQuery = { 0 };
    PDH_HCOUNTER cpuTotal = {0 };

    int numProcessors = 0;
    HANDLE m_self;
    ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
    int lastUsage;
#endif

#ifdef Q_OS_MACOS
    unsigned long long _previousTotalTicks = 0;
    unsigned long long _previousIdleTicks = 0;
#endif

#ifdef Q_OS_LINUX
    unsigned long long lastTotalUser = 0;
    unsigned long long lastTotalUserLow = 0;
    unsigned long long lastTotalSys = 0;
    unsigned long long lastTotalIdle = 0;
#endif

    std::shared_ptr<ThreadPool> m_ioContext;
    boost::asio::steady_timer m_usageCheckTimer;
    std::function<void(boost::system::error_code ec)> m_handler;

    int             m_sendCount = 0;
    double          m_cpuUsage = 0.0;
    double          m_currentProcessCpuUsage = 0.0;
    long long       m_usedMem = 0;
    long long       m_totalMem = 0;
};
