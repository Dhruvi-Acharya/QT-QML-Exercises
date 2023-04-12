#include "DecoderManager.h"
#include "FFmpegStruct.h"
#include <QDebug>
#include "LogSettings.h"
#include "WeakCallback.h"
#include "ResourceMonitor.h"
#include "CoreService/CoreServiceStructure.h"
#include "QCoreServiceManager.h"
#include <QDateTime>

DecoderManager::DecoderManager(QObject *parent)
    : QObject(parent)
{
    qDebug() << "DecoderManager()";
    checkHardwareDecoder();

    //https://stackoverflow.com/questions/36006956/qt-qtimer-doesnt-work-what-details-have-i-missed
    //m_checkCpuUsageTimer->setTimerType(Qt::PreciseTimer);
    // start 함수는 QApplication이 만들어진 이후에 호출되어야 동작 main.cpp에 있음
    //m_checkCpuUsageTimer = new QTimer(this);
    //m_checkCpuUsageTimer->setInterval(200);
    //connect(m_checkCpuUsageTimer, &QTimer::timeout, this, &DecoderManager::checkCpuUsage);

    connect(ResourceMonitor::Instance(), &ResourceMonitor::normalPeriod, this, &DecoderManager::checkCpuUsage);
}

DecoderManager::~DecoderManager()
{
    qDebug() << "~DecoderManager()";

    /*
    if(m_checkCpuUsageTimer)
    {
        m_checkCpuUsageTimer->stop();
        delete m_checkCpuUsageTimer;
        m_checkCpuUsageTimer = nullptr;
    }*/
}

void DecoderManager::checkHardwareDecoder()
{
    /*
    // Intel Quick Sync (Windows)
    std::string qsvName = "h264_qsv";
    const AVCodec* qsv = avcodec_find_decoder_by_name(qsvName.c_str());
    if(qsv)
    {
        SPDLOG_INFO("DecoderManager::supportsHardwareDecoding() QSV Support");
        m_qsvSupport = true;
    }

    // NVIDIA CUDA (Windows)
    std::string cudaName = "h264_cuvid";
    const AVCodec* cuda = avcodec_find_decoder_by_name(cudaName.c_str());
    if(cuda)
    {
        SPDLOG_INFO("DecoderManager::supportsHardwareDecoding() CUDA Support");
        m_cudaSupport = true;
    }*/

    // Intel Quick Sync (Windows)
    AVBufferRef* qsvDeviceContext = nullptr;
    int supportQsv = av_hwdevice_ctx_create(&qsvDeviceContext, AVHWDeviceType::AV_HWDEVICE_TYPE_QSV, NULL, NULL, 0);

    if (supportQsv == 0) {
        SPDLOG_INFO("DecoderManager::supportsHardwareDecoding() QSV Support");
        m_qsvSupport = true;
    }

    if (qsvDeviceContext) {
        av_buffer_unref(&qsvDeviceContext);
        qsvDeviceContext = nullptr;
    }

    // NVIDIA CUDA (Windows)
    AVBufferRef* cudaDeviceContext = nullptr;
    int supportCuda = av_hwdevice_ctx_create(&cudaDeviceContext, AVHWDeviceType::AV_HWDEVICE_TYPE_CUDA, NULL, NULL, 0);

    if (supportCuda == 0) {
        SPDLOG_INFO("DecoderManager::supportsHardwareDecoding() CUDA Support");
        m_cudaSupport = true;
    }

    if (cudaDeviceContext) {
        av_buffer_unref(&cudaDeviceContext);
        cudaDeviceContext = nullptr;
    }

    // Video Toolbox (Mac OS)
    AVBufferRef* toolboxContext = nullptr;
    int supportToolBox = av_hwdevice_ctx_create(&toolboxContext, AVHWDeviceType::AV_HWDEVICE_TYPE_VIDEOTOOLBOX, NULL, NULL, 0);
    if(supportToolBox == 0)
    {
        SPDLOG_INFO("DecoderManager::supportsHardwareDecoding() TOOLBOX Support");
        m_videoToolboxSupport = true;
    }

    if (toolboxContext) {
        av_buffer_unref(&toolboxContext);
        toolboxContext = nullptr;
    }
}

bool DecoderManager::isIframeMode()
{
    return m_iframeMode;
}

void DecoderManager::checkCpuUsage(double cpuUsage, long long usedMem, long long totalMem, double currentProcess)
{
    Q_UNUSED(usedMem);
    Q_UNUSED(totalMem);

    // System CPU usage
    m_cpuUsageQueue.enqueue(cpuUsage);
    if(m_cpuUsageQueue.size() > 20)
        m_cpuUsageQueue.dequeue();

    // Process (Wisenet viewer) CPU Usage
    m_processCpuUsageQueue.enqueue(currentProcess);
    if(m_processCpuUsageQueue.size() > 20)
        m_processCpuUsageQueue.dequeue();

    // Calculate System average CPU usage during 4 seconds.
    double cpuUsageSum = 0.0;
    for(int i=0; i<m_cpuUsageQueue.size(); i++)
        cpuUsageSum += m_cpuUsageQueue[i];
    m_averageCpuUsage = cpuUsageSum/m_cpuUsageQueue.size();

    //qDebug() << "[CPU Check] usage" <<  cpuUsage << ", average" << m_averageCpuUsage << ", currentProocess" << currentProcess;
    if(m_averageCpuUsage > 70.0 && !m_iframeMode && m_cpuUsageQueue.size() == 20)
    {
        // Calculate process cpu usage
        double processCpuUsageSum = 0.0;
        for(int i=0; i<m_processCpuUsageQueue.size(); i++)
            processCpuUsageSum += m_processCpuUsageQueue[i];
        m_averageProcessCpuUsageAtIframeModeEntered = processCpuUsageSum/m_processCpuUsageQueue.size();
        m_iframeModeCpuUsageQueue.clear();

        m_pixelAtOverload = m_totalPixel;

        m_iframeMode = true;
        sendSystemLog(m_averageCpuUsage);

        SPDLOG_INFO("[CPU Check] overload!!!! usage {}, pixel {}, iframemode {}, process avg usage {}", m_averageCpuUsage, m_pixelAtOverload, m_iframeMode, m_averageProcessCpuUsageAtIframeModeEntered);
        qDebug() << "[CPU Check] overload!!!! usage" << m_averageCpuUsage << "pixel" << m_pixelAtOverload << "iframemode" << m_iframeMode << "process avg usage" << m_averageProcessCpuUsageAtIframeModeEntered;
    }

    // Full frame mode return condition - 30seconds
    if(m_iframeMode)
    {
        m_iframeModeCpuUsageQueue.enqueue(cpuUsage);

        if(m_iframeModeCpuUsageQueue.size() > 150)
        {
            m_iframeModeCpuUsageQueue.dequeue();

            double sum = 0.0;
            for(int i=0; i<m_iframeModeCpuUsageQueue.size(); i++)
                sum += m_iframeModeCpuUsageQueue[i];
            double averageCpu = sum/m_iframeModeCpuUsageQueue.size();

            if(averageCpu - currentProcess + m_averageProcessCpuUsageAtIframeModeEntered < 70.0)
            {
                SPDLOG_INFO("[CPU Check] change to full frame mode 30seconds cpu avg {}, process avg usage {}", averageCpu, m_averageProcessCpuUsageAtIframeModeEntered);
                qDebug() << "[CPU Check] change to full frame mode 30seconds cpu avg" << averageCpu << ", process avg usage" << m_averageProcessCpuUsageAtIframeModeEntered;
                m_averageProcessCpuUsageAtIframeModeEntered = 0;

                setIframeModeAsFalse();
            }
        }
    }
}

void DecoderManager::sendSystemLog(double cpuUsage)
{
#ifndef MEDIA_FILE_ONLY
    Wisenet::Core::EventLog eventLog;
    eventLog.isService = true;
    eventLog.serviceUtcTimeMsec = QDateTime::currentMSecsSinceEpoch();

    if(m_iframeMode)
    {
        eventLog.type = "SystemEvent.IFrameMode";
        eventLog.parameters.data.emplace("CPU", QString::number(cpuUsage, 'f', 1).toStdString());
    }
    else
    {
        eventLog.type = "SystemEvent.FullFrameMode";
    }

    auto addLogRequest = std::make_shared<Wisenet::Core::AddLogRequest>();
    addLogRequest->eventLogs.push_back(eventLog);

    QCoreServiceManager::Instance().RequestToCoreService(&Wisenet::Core::ICoreService::AddLog,
                                                         this,
                                                         addLogRequest,
                                                         nullptr);
#endif
}

bool DecoderManager::supportsHardwareDecoding()
{
    if(m_qsvSupport)
        SPDLOG_INFO("DecoderManager::supportsHardwareDecoding() QSV Support");
    else
        SPDLOG_INFO("DecoderManager::supportsHardwareDecoding() QSV Not support");

    if(m_cudaSupport)
        SPDLOG_INFO("DecoderManager::supportsHardwareDecoding() CUDA Support");
    else
        SPDLOG_INFO("DecoderManager::supportsHardwareDecoding() CUDA Not support");

    if(m_videoToolboxSupport)
        SPDLOG_INFO("DecoderManager::supportsHardwareDecoding() TOOLBOX Support");
    else
        SPDLOG_INFO("DecoderManager::supportsHardwareDecoding() TOOLBOX not support");

    return m_qsvSupport || m_cudaSupport;
}

void DecoderManager::switchFullFrameMode()
{
    SPDLOG_INFO("[CPU Check] switchFullFrameMode, set iframe mode as false");
    qDebug() << "[CPU Check] switchFullFrameMode, set iframe mode as false";
    setIframeModeAsFalse();
}

void DecoderManager::setIframeModeAsFalse()
{
    bool isInIframeMode = m_iframeMode;

    m_iframeMode = false;

    if(isInIframeMode)
        sendSystemLog(0.0);
}

VideoDecodeMethod DecoderManager::GetAvaliableCodecMethod(int pixelSize, int decoderNotAvailable, bool isSequence)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if(!isSequence)
    {
        m_totalPixel += pixelSize;

        if(m_totalPixel < m_pixelAtOverload || m_pixelAtOverload <= 0)
        {
            SPDLOG_INFO("[CPU Check] GetAvaliableCodecMethod() iframe mode has been ended - totalpixel {}, pixelAtOverload {}", m_totalPixel, m_pixelAtOverload);
            //qDebug() << "[CPU Check] GetAvaliableCodecMethod() iframe mode has been ended - totalpixel" << m_totalPixel << "pixelAtOverload" << m_pixelAtOverload;
            m_pixelAtOverload = 0;

            setIframeModeAsFalse();
        }
    }

    if(!m_useHwDecoding)
        return VideoDecodeMethod::SW;

    if(decoderNotAvailable == (int)VideoDecodeMethod::CUVID)
    {
        int before = maxCudaResolutionTotal;
        maxCudaResolutionTotal = m_cudaResolutionTotal - fullHdPixel * 2;

        int maxCount = maxCudaResolutionTotal / fullHdPixel;
        if(maxCount < 0)
            maxCount = 0;
        //cudaChannelCountChanged(maxCount);

        SPDLOG_DEBUG("DecoderManager::GetAvaliableCodecMethod() decrease CUVID total : {} to {}, maxCount: {}", before, maxCudaResolutionTotal, maxCount);
        qDebug() << "DecoderManager::GetAvaliableCodecMethod() decrease CUVID total :" << before << "to" << maxCudaResolutionTotal << ", maxCount:" << maxCount;
    }
    else if(decoderNotAvailable == (int)VideoDecodeMethod::QSV)
    {
        int before = maxQsvResolutionTotal;
        maxQsvResolutionTotal = m_qsvResolutionTotal - fullHdPixel * 2;

        int maxCount = maxQsvResolutionTotal / fullHdPixel;
        if(maxCount < 0)
            maxCount = 0;
        //qsvChannelCountChanged(maxCount);

        SPDLOG_DEBUG("DecoderManager::GetAvaliableCodecMethod() decrease QSV total : {} to {}, maxCount: {}", before, maxQsvResolutionTotal, maxCount);
        qDebug() << "DecoderManager::GetAvaliableCodecMethod() decrease QSV total :" << before << "to" << maxQsvResolutionTotal << ", maxCount:" << maxCount;
    }
    else if(decoderNotAvailable == (int)VideoDecodeMethod::VIDEOTOOLBOX)
    {
        int before = maxToolboxResolutionTotal;
        maxToolboxResolutionTotal = m_toolboxResolutionTotal - fullHdPixel * 2;

        int maxCount = maxToolboxResolutionTotal / fullHdPixel;
        //toolboxChannelCountChanged(maxCount);
        if(maxCount < 0)
            maxCount = 0;

        SPDLOG_DEBUG("DecoderManager::GetAvaliableCodecMethod() decrease VIDEOTOOLBOX total : {} to {}, maxCount: {}", before, maxToolboxResolutionTotal, maxCount);
        qDebug() << "DecoderManager::GetAvaliableCodecMethod() decrease VIDEOTOOLBOX total :" << before << "to" << maxToolboxResolutionTotal << ", maxCount:" << maxCount;
    }

    if(decoderNotAvailable != (int)VideoDecodeMethod::CUVID && m_cudaSupport && m_cudaResolutionTotal + pixelSize <= maxCudaResolutionTotal)
    {
        m_cudaResolutionTotal += pixelSize;

        SPDLOG_DEBUG("DecoderManager::DecoderOpened() method {}, pixelSize {}, QSV {}, CUVID {}, VIDEOTOOLBOX {}", VideoDecodeMethod::CUVID, pixelSize, m_qsvResolutionTotal, m_cudaResolutionTotal, m_toolboxResolutionTotal);
        //qDebug() << "DecoderManager::DecoderOpened() method" << (int)VideoDecodeMethod::CUVID << ", pixelSize" << pixelSize << ", QSV" << m_qsvResolutionTotal << ", CUVID" << m_cudaResolutionTotal << ", VIDEOTOOLBOX" << m_toolboxResolutionTotal;
        return VideoDecodeMethod::CUVID;
    }

    if(decoderNotAvailable != (int)VideoDecodeMethod::QSV && m_qsvSupport && (m_qsvResolutionTotal + pixelSize <= maxQsvResolutionTotal) && (pixelSize >= 1280*720) )
    {
        m_qsvResolutionTotal += pixelSize;

        SPDLOG_DEBUG("DecoderManager::DecoderOpened() method {}, pixelSize {}, QSV {}, CUVID {}, VIDEOTOOLBOX {}", VideoDecodeMethod::QSV, pixelSize, m_qsvResolutionTotal, m_cudaResolutionTotal, m_toolboxResolutionTotal);
        //qDebug() << "DecoderManager::DecoderOpened() method" << (int)VideoDecodeMethod::QSV << ", pixelSize" << pixelSize << ", QSV" << m_qsvResolutionTotal << ", CUVID" << m_cudaResolutionTotal << ", VIDEOTOOLBOX" << m_toolboxResolutionTotal;
        return VideoDecodeMethod::QSV;
    }

    if(decoderNotAvailable != (int)VideoDecodeMethod::VIDEOTOOLBOX && m_videoToolboxSupport && m_toolboxResolutionTotal + pixelSize <= maxToolboxResolutionTotal)
    {
        m_toolboxResolutionTotal += pixelSize;

        SPDLOG_DEBUG("DecoderManager::DecoderOpened() method {}, pixelSize {}, QSV {}, CUVID {}, VIDEOTOOLBOX {}", VideoDecodeMethod::VIDEOTOOLBOX, pixelSize, m_qsvResolutionTotal, m_cudaResolutionTotal, m_toolboxResolutionTotal);
        //qDebug() << "DecoderManager::DecoderOpened() method" << (int)VideoDecodeMethod::VIDEOTOOLBOX << ", pixelSize" << pixelSize << ", QSV" << m_qsvResolutionTotal << ", CUVID" << m_cudaResolutionTotal << ", VIDEOTOOLBOX" << m_toolboxResolutionTotal;
        return VideoDecodeMethod::VIDEOTOOLBOX;
    }

    return VideoDecodeMethod::SW;
}

void DecoderManager::SetUseHwDecoding(bool useHwDecoding)
{
    m_useHwDecoding = useHwDecoding;
}

void DecoderManager::SetQsvCount(int channelCount)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_qsvChannelCount = channelCount;
    maxQsvResolutionTotal = fullHdPixel * m_qsvChannelCount;
}

void DecoderManager::SetCudaCount(int channelCount)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_cudaChannelCount = channelCount;
    maxCudaResolutionTotal = fullHdPixel * m_cudaChannelCount;
}

void DecoderManager::SetToolboxCount(int channelCount)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_toolboxChannelCount = channelCount;
    maxToolboxResolutionTotal = fullHdPixel * m_toolboxChannelCount;
}

void DecoderManager::DecoderClosed(VideoDecodeMethod method, int pixelSize, bool isSequence)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if(!isSequence)
        m_totalPixel -= pixelSize;

    if(m_totalPixel < m_pixelAtOverload || m_pixelAtOverload <= 0)
    {
        SPDLOG_INFO("[CPU Check] DecoderClosed() iframe mode has been ended - totalpixel {}, pixelAtOverload {}", m_totalPixel, m_pixelAtOverload);
        //qDebug() << "[CPU Check] DecoderClosed() iframe mode has been ended - totalpixel" << m_totalPixel << "pixelAtOverload" << m_pixelAtOverload;
        m_pixelAtOverload = 0;

        setIframeModeAsFalse();
    }

    if(method == VideoDecodeMethod::QSV)
    {
        m_qsvResolutionTotal -= pixelSize;
    }

    if(method == VideoDecodeMethod::CUVID)
    {
        m_cudaResolutionTotal -= pixelSize;
    }

    if(method == VideoDecodeMethod::VIDEOTOOLBOX)
    {
        m_toolboxResolutionTotal -= pixelSize;
    }

    SPDLOG_DEBUG("DecoderManager::DecoderClosed() method {}, pixelSize {}, QSV {}, CUVID {}, VIDEOTOOLBOX {}, isSequence {}", method, pixelSize, m_qsvResolutionTotal, m_cudaResolutionTotal, m_toolboxResolutionTotal, isSequence);
    //qDebug() << "DecoderManager::DecoderClosed() method" << (int)method << ", pixelSize" << pixelSize << ", QSV" << m_qsvResolutionTotal << ", CUVID" << m_cudaResolutionTotal << ", VIDEOTOOLBOX" << m_toolboxResolutionTotal;
}

