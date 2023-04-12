#pragma once
#include <QObject>
#include <mutex>
#include "FFmpegStruct.h"
//#include <QTimer>
#include <QQueue>

class DecoderManager : public QObject
{
    Q_OBJECT
public:
    explicit DecoderManager(QObject *parent = nullptr);
    ~DecoderManager();

    static DecoderManager* getInstance(){
        static DecoderManager instance;
        return &instance;
    }

    VideoDecodeMethod GetAvaliableCodecMethod(int pixelSize, int decoderNotAvailable, bool isSequence);
    void SetUseHwDecoding(bool useHwDecoding);
    void SetQsvCount(int channelCount);
    void SetCudaCount(int channelCount);
    void SetToolboxCount(int channelCount);

    void DecoderClosed(VideoDecodeMethod method, int pixelSize, bool isSequence);

    bool isIframeMode();

public slots:
    void checkCpuUsage(double cpuUsage, long long usedMem, long long totalMem, double currentProcess);
    Q_INVOKABLE bool supportsHardwareDecoding();
    void switchFullFrameMode();

private:
    void checkHardwareDecoder();
    void sendSystemLog(double cpuUsage);
    void setIframeModeAsFalse();

signals:
    void cudaChannelCountChanged(int count);
    void qsvChannelCountChanged(int count);
    void toolboxChannelCountChanged(int count);

private:
    bool m_useHwDecoding = false;
    bool m_qsvSupport = false;
    bool m_cudaSupport = false;
    bool m_videoToolboxSupport = false;

    //std::atomic<int> m_qsvResolutionTotal{0};
    //std::atomic<int> m_cudaResolutionTotal{0};
    //std::atomic<int> m_toolboxResolutionTotal{0};

    int m_qsvResolutionTotal = 0;
    int m_cudaResolutionTotal = 0;
    int m_toolboxResolutionTotal = 0;

    int m_qsvChannelCount = 0;
    int m_cudaChannelCount = 0;
    int m_toolboxChannelCount = 0;

    const int fullHdPixel = 1920 * 1080;
    int maxQsvResolutionTotal = fullHdPixel * 16; // 33,177,600 pixel
    int maxCudaResolutionTotal = fullHdPixel * 16; // 33,177,600 pixel
    int maxToolboxResolutionTotal = fullHdPixel * 8; // 24,883,200 pixel

    std::mutex m_mutex;

    int m_totalPixel = 0;

    //QTimer* m_checkCpuUsageTimer = nullptr;
    QQueue<double> m_cpuUsageQueue;
    double m_averageCpuUsage = 0.0;

    QQueue<double> m_processCpuUsageQueue;
    double m_averageProcessCpuUsageAtIframeModeEntered = 0.0;

    bool m_iframeMode = false;
    int m_pixelAtOverload = 0;

    QQueue<double> m_iframeModeCpuUsageQueue;
};
