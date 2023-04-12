#include "ResourceMonitor.h"
#include <QDebug>
#include "LogSettings.h"

ResourceMonitor::ResourceMonitor(QObject *parent)
    : QObject(parent)
    , m_ioContext(std::make_shared<ThreadPool>(1))
    , m_usageCheckTimer(m_ioContext->ioContext())
    , m_handler(nullptr) // 2023.01.11. coverity (ubuntu)
{
#ifdef Q_OS_WINDOWS
    PdhOpenQuery(NULL, NULL, &cpuQuery);
    PdhAddCounter(cpuQuery, L"\\Processor(_TOTAL)\\% Processor Time", NULL, &cpuTotal);
    PdhCollectQueryData(cpuQuery);

    SYSTEM_INFO sysInfo;
    FILETIME ftime, fsys, fuser;

    GetSystemInfo(&sysInfo);
    numProcessors = sysInfo.dwNumberOfProcessors;

    GetSystemTimeAsFileTime(&ftime);
    memcpy(&lastCPU, &ftime, sizeof(FILETIME));

    m_self = GetCurrentProcess();
    GetProcessTimes(m_self, &ftime, &ftime, &fsys, &fuser);
    memcpy(&lastSysCPU, &fsys, sizeof(FILETIME));
    memcpy(&lastUserCPU, &fuser, sizeof(FILETIME));

#endif
    m_ioContext->Start();

    m_handler = [&](boost::system::error_code ec) {
        if (ec) {
            return;
        }

        getData(m_cpuUsage, m_usedMem, m_totalMem, m_currentProcessCpuUsage);

        if(m_cpuUsage < 0)
        {
            restartResourceCheckTimer();
            return;
        }

#ifdef Q_OS_WINDOWS
        emit normalPeriod(m_cpuUsage, m_usedMem, m_totalMem, m_currentProcessCpuUsage);
#endif

        m_sendCount++;
        if(m_sendCount >= 5)
        {
            m_sendCount = 0;
            emit longtermPeriod(m_cpuUsage, m_usedMem, m_totalMem);
        }

        restartResourceCheckTimer();
    };

    restartResourceCheckTimer();
}

ResourceMonitor::~ResourceMonitor()
{
    m_usageCheckTimer.cancel();
    m_ioContext->Stop();
}

void ResourceMonitor::restartResourceCheckTimer()
{
    m_usageCheckTimer.expires_after(std::chrono::milliseconds(200));
    m_usageCheckTimer.async_wait(m_handler);
}

void ResourceMonitor::getData(double& cpuUsage, long long& usedMem, long long& totalMem, double& currentProcessCpuUsage)
{
    cpuUsage = getCpuUsage();
    getMemoryUsage(usedMem, totalMem);
    currentProcessCpuUsage = getCurrentProcessCpuUsage();
}

double ResourceMonitor::getCpuUsage()
{
#ifdef Q_OS_WINDOWS
    return getWindowsCpuUsage();
#endif

#ifdef Q_OS_MACOS
    return getMacCpuUsage();
#endif

#ifdef Q_OS_LINUX
    return getLinuxCpuUsage();
#endif

    return 0.0;
}

double ResourceMonitor::getCurrentProcessCpuUsage()
{
#ifdef Q_OS_WINDOWS
    FILETIME ftime = { 0 };
    FILETIME fsys = { 0 };
    FILETIME fuser = { 0 };
    ULARGE_INTEGER now, sys, user = { 0 };
    double percent = 0;

    GetSystemTimeAsFileTime(&ftime);
    memcpy(&now, &ftime, sizeof(FILETIME));

    GetProcessTimes(m_self, &ftime, &ftime, &fsys, &fuser);
    memcpy(&sys, &fsys, sizeof(FILETIME));
    memcpy(&user, &fuser, sizeof(FILETIME));

    percent = (sys.QuadPart - lastSysCPU.QuadPart) + (user.QuadPart - lastUserCPU.QuadPart);
    percent /= (now.QuadPart - lastCPU.QuadPart);
    percent/= numProcessors;

    lastCPU = now;
    lastUserCPU = user;
    lastSysCPU = sys;

    lastUsage = static_cast<int>(percent * 100);

    return percent * 100;

#endif

    return 0.0;
}

void ResourceMonitor::getMemoryUsage(long long& usedMem, long long& totalMem)
{
#ifdef Q_OS_WINDOWS
    getWindowsMemoryUsage(usedMem, totalMem);
#endif

#ifdef Q_OS_MACOS
    getMacMemoryUsage(usedMem, totalMem);
#endif

#ifdef Q_OS_LINUX
    getLinuxMemoryUsage(usedMem, totalMem);
#endif
}


double ResourceMonitor::getWindowsCpuUsage()
{
#ifdef Q_OS_WINDOWS
    PDH_FMT_COUNTERVALUE counterVal = { 0 };

    PdhCollectQueryData(cpuQuery);
    PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_DOUBLE, NULL, &counterVal);
    return counterVal.doubleValue;
#endif

    return -1.0;
}

void ResourceMonitor::getWindowsMemoryUsage(long long &usedMem, long long &totalMem)
{
#ifdef Q_OS_WINDOWS
    //메모리값
    MEMORYSTATUSEX MemoryStatus = {0};
    MemoryStatus.dwLength = sizeof (MemoryStatus);
    ::GlobalMemoryStatusEx(&MemoryStatus);

    usedMem = (long long)((MemoryStatus.ullTotalPhys - MemoryStatus.ullAvailPhys)/(1024*1024));
    totalMem = (long long)((MemoryStatus.ullTotalPhys)/(1024*1024));
#endif
}

// Returns 1.0f for "CPU fully pinned", 0.0f for "CPU idle", or somewhere in between
// You'll need to call this at regular intervals, since it measures the load between
// the previous call and the current one.
double ResourceMonitor::getMacCpuUsage()
{
#ifdef Q_OS_MACOS
    host_cpu_load_info_data_t cpuinfo;
    mach_msg_type_number_t count = HOST_CPU_LOAD_INFO_COUNT;
    if (host_statistics(mach_host_self(), HOST_CPU_LOAD_INFO, (host_info_t)&cpuinfo, &count) == KERN_SUCCESS) {
        unsigned long long totalTicks = 0;
        for(int i=0; i<CPU_STATE_MAX; i++)
            totalTicks += cpuinfo.cpu_ticks[i];

        return calculateMacCpuUsage(cpuinfo.cpu_ticks[CPU_STATE_IDLE], totalTicks);
    }
    else {
        return -1.0;
    }
#endif

    return -1.0;
}

void ResourceMonitor::getMacMemoryUsage(long long &usedMem, long long &totalMem)
{
#ifdef Q_OS_MACOS
    vm_size_t page_size;
    mach_port_t mach_port;
    mach_msg_type_number_t count;
    vm_statistics64_data_t vm_stats;

    mach_port = mach_host_self();
    count = sizeof(vm_stats) / sizeof(natural_t);
    if (KERN_SUCCESS == host_page_size(mach_port, &page_size) &&
            KERN_SUCCESS == host_statistics64(mach_port, HOST_VM_INFO, (host_info64_t)&vm_stats, &count))
    {
        totalMem = ((int64_t)vm_stats.active_count +
                    (int64_t)vm_stats.inactive_count +
                    (int64_t)vm_stats.wire_count +
                    (int64_t)vm_stats.free_count) * (int64_t)page_size;

        usedMem = ((int64_t)vm_stats.active_count + (int64_t)vm_stats.wire_count) * (int64_t)page_size;

        /*
        SPDLOG_INFO("mac memory active: {}, inactive: {}, wire: {}, free: {}, pageins: {}, pageouts: {}, purges: {}, vm_page_size:{}",
                    vm_stats.active_count, vm_stats.inactive_count,
                    vm_stats.wire_count, vm_stats.free_count,
                    vm_stats.pageins, vm_stats.pageouts,
                    vm_stats.purges, vm_page_size);*/

        totalMem /= (1024*1024);
        usedMem /= (1024*1024);

    }
#endif
}

double ResourceMonitor::calculateMacCpuUsage(unsigned long long idleTicks, unsigned long long totalTicks)
{
#ifdef Q_OS_MACOS
    unsigned long long totalTicksSinceLastTime = totalTicks-_previousTotalTicks;
    unsigned long long idleTicksSinceLastTime  = idleTicks-_previousIdleTicks;

    if(totalTicksSinceLastTime <= 0 || idleTicksSinceLastTime <= 0)
        return -1.0f;

    float ret = 1.0f-((totalTicksSinceLastTime > 0) ? ((float)idleTicksSinceLastTime)/totalTicksSinceLastTime : 0);

    _previousTotalTicks = totalTicks;
    _previousIdleTicks  = idleTicks;
    return (double)ret*100;
#endif

    return -1.0;
}

double ResourceMonitor::getLinuxCpuUsage()
{
#ifdef Q_OS_LINUX
    double percent;
    FILE* file;
    unsigned long long totalUser, totalUserLow, totalSys, totalIdle, total;

    file = fopen("/proc/stat", "r");
    fscanf(file, "cpu %llu %llu %llu %llu", &totalUser, &totalUserLow,
           &totalSys, &totalIdle);
    fclose(file);

    if (totalUser < lastTotalUser || totalUserLow < lastTotalUserLow ||
            totalSys < lastTotalSys || totalIdle < lastTotalIdle){
        //Overflow detection. Just skip this value.
        percent = -1.0;
    }
    else{
        total = (totalUser - lastTotalUser) + (totalUserLow - lastTotalUserLow) +
                (totalSys - lastTotalSys);
        percent = total;
        total += (totalIdle - lastTotalIdle);
        percent /= total;
        percent *= 100;
    }

    lastTotalUser = totalUser;
    lastTotalUserLow = totalUserLow;
    lastTotalSys = totalSys;
    lastTotalIdle = totalIdle;

    return percent;
#endif
    return 0.0;
}

void ResourceMonitor::getLinuxMemoryUsage(long long &usedMem, long long &totalMem)
{
#ifdef Q_OS_LINUX
  /*  struct sysinfo memInfo;

    sysinfo (&memInfo);

    long long totalPhysMem = memInfo.totalram;
    //Multiply in next statement to avoid int overflow on right hand side...
    totalPhysMem *= memInfo.mem_unit;

    long long physMemUsed = memInfo.totalram - memInfo.freeram;
    //Multiply in next statement to avoid int overflow on right hand side...
    physMemUsed *= memInfo.mem_unit;

    SPDLOG_INFO("getLinuxMemoryUsage() total:{}, free:{}, memunit:{}, shared:{}, buffer:{}, totalswap:{}, freeswap:{}",
                memInfo.totalram/(1024*1024), memInfo.freeram/(1024*1024),
                memInfo.mem_unit, memInfo.sharedram/(1024*1024),
                memInfo.bufferram/(1024*1024), memInfo.totalswap/(1024*1024),
                memInfo.freeswap/(1024*1024));

    totalMem = totalPhysMem/(1024*1024);
    usedMem = physMemUsed/(1024*1024);
*/

    const std::string meminfo_file("/proc/meminfo");
    std::ifstream ifs(meminfo_file);
    if (ifs)
    {
        int total=0;
        int avaliable = 0;
        std::string line;
        while (std::getline(ifs, line))
        {
            std::istringstream iss(line);
            std::string name;
            std::string value;
            if (iss >> name >> value)
            {
                if(QString::fromStdString(name) == "MemTotal:")
                {
                    total = QString::fromStdString(value).toInt();
                }

                if(QString::fromStdString(name) == "MemAvailable:")
                {
                    avaliable = QString::fromStdString(value).toInt();
                }
            }
        }

        if(total != 0 && avaliable != 0)
        {
            totalMem = total / 1024;
            usedMem = (total - avaliable) / 1024;
        }
    }
#endif
}
