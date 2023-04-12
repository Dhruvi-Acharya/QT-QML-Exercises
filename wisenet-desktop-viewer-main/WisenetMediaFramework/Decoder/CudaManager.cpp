/*********************************************************************************
 * Copyright(c) 2021 by Hanwha Techwin Co., Ltd.
 *
 * This software is copyrighted by, and is the sole property of Hanwha Techwin.
 * All rigths, title, ownership, or other interests in the software remain the
 * property of Hanwha Techwin. This software may only be used in accordance with
 * the corresponding license agreement. Any unauthorized use, duplication,
 * transmission, distribution, or disclosure of this software is expressly
 * forbidden.
 *
 * This Copyright notice may not be removed or modified without prior written
 * consent of Hanwha Techwin.
 *
 * Hanwha Techwin reserves the right to modify this software without notice.
 *
 * Hanwha Techwin Co., Ltd.
 * KOREA
 * https://www.hanwha-security.com/
 *********************************************************************************/
#include "CudaManager.h"
#include "LogSettings.h"
#include <QtGlobal>

#undef SPDLOG_LOGGER_CALL
#define SPDLOG_LOGGER_CALL(logger, level, ...) (logger)->log(spdlog::source_loc{__FILE__, __LINE__, "[CUDA] "}, level, __VA_ARGS__)


static void getOutputFormatNames(unsigned short nOutputFormatMask, char *OutputFormats)
{
    if (nOutputFormatMask == 0) {
        strcpy(OutputFormats, "N/A");
        return;
    }

    if (nOutputFormatMask & (1U << cudaVideoSurfaceFormat_NV12)) {
        strcat(OutputFormats, "NV12 ");
    }

    if (nOutputFormatMask & (1U << cudaVideoSurfaceFormat_P016)) {
        strcat(OutputFormats, "P016 ");
    }

    if (nOutputFormatMask & (1U << cudaVideoSurfaceFormat_YUV444)) {
        strcat(OutputFormats, "YUV444 ");
    }

    if (nOutputFormatMask & (1U << cudaVideoSurfaceFormat_YUV444_16Bit)) {
        strcat(OutputFormats, "YUV444P16 ");
    }
    return;
}

CudaDeviceSession::CudaDeviceSession(CudaFunctions *cuda, CuvidFunctions *cuvid)
    : m_cuda(cuda)
    , m_cuvid(cuvid)
{

}

CudaDeviceSession::~CudaDeviceSession()
{
#ifndef Q_OS_WIN
    return;
#endif

    SPDLOG_DEBUG("Destroy CudaDeviceSession, id={}, name={}", cuDevice, deviceName);
    if (cuContext != nullptr) {
        m_cuda->cuCtxPushCurrent(cuContext);
        m_cuda->cuCtxDestroy(cuContext);
        cuContext = nullptr;
        m_cuda->cuCtxPopCurrent(NULL);
    }
}


CudaManager::CudaManager(QObject *parent)
{
    SPDLOG_DEBUG("Create a new NvVideoDecoderManager instance.");
}

CudaManager::~CudaManager()
{
    Stop();
    SPDLOG_DEBUG("Destroy a NvVideoDecoderManager instance.");
}

bool CudaManager::Start()
{
#ifndef Q_OS_WIN
    return false;
#endif
    if (cuda_load_functions(&m_cuda, NULL) < 0) {
        SPDLOG_INFO("cuda_load_functions() failed");
        Stop();
        return false;
    }
    if (cuvid_load_functions(&m_cuvid, NULL) < 0) {
        SPDLOG_INFO("cuvid_load_functions() failed");
        Stop();
        return false;
    }

    const char *aszCodecName[] = {"JPEG", "MPEG1", "MPEG2", "MPEG4", "H264", "HEVC", "HEVC", "HEVC", "HEVC", "HEVC", "HEVC", "VC1", "VP8", "VP9", "VP9", "VP9", "AV1", "AV1", "AV1", "AV1"};
    const char *aszChromaFormat[] = { "4:0:0", "4:2:0", "4:2:2", "4:4:4" };
    char strOutputFormats[64];
    cudaVideoCodec aeCodec[] = { cudaVideoCodec_JPEG, cudaVideoCodec_MPEG1, cudaVideoCodec_MPEG2, cudaVideoCodec_MPEG4, cudaVideoCodec_H264, cudaVideoCodec_HEVC,
        cudaVideoCodec_HEVC, cudaVideoCodec_HEVC, cudaVideoCodec_HEVC, cudaVideoCodec_HEVC, cudaVideoCodec_HEVC, cudaVideoCodec_VC1, cudaVideoCodec_VP8,
        cudaVideoCodec_VP9, cudaVideoCodec_VP9, cudaVideoCodec_VP9, cudaVideoCodec_AV1, cudaVideoCodec_AV1, cudaVideoCodec_AV1, cudaVideoCodec_AV1 };
    int anBitDepthMinus8[] = {0, 0, 0, 0, 0, 0, 2, 4, 0, 2, 4, 0, 0, 0, 2, 4, 0, 2, 0, 2};

    cudaVideoChromaFormat aeChromaFormat[] = { cudaVideoChromaFormat_420, cudaVideoChromaFormat_420, cudaVideoChromaFormat_420, cudaVideoChromaFormat_420,
        cudaVideoChromaFormat_420, cudaVideoChromaFormat_420, cudaVideoChromaFormat_420, cudaVideoChromaFormat_420, cudaVideoChromaFormat_444, cudaVideoChromaFormat_444,
        cudaVideoChromaFormat_444, cudaVideoChromaFormat_420, cudaVideoChromaFormat_420, cudaVideoChromaFormat_420, cudaVideoChromaFormat_420, cudaVideoChromaFormat_420,
        cudaVideoChromaFormat_420, cudaVideoChromaFormat_420, cudaVideoChromaFormat_Monochrome, cudaVideoChromaFormat_Monochrome };

    SPDLOG_DEBUG("Start NvVideoDecoderManager");
    CUresult result = m_cuda->cuInit(0);
    if (result != CUDA_SUCCESS) {
        SPDLOG_INFO("Failed to initialize the CUDA driver API. ret={}", result);
        return false;
    }
    int nGpu = 0;
    result = m_cuda->cuDeviceGetCount(&nGpu);
    if (result != CUDA_SUCCESS) {
        SPDLOG_INFO("Failed to cuDeviceGetCount(), ret={}", result);
        return false;
    }

    SPDLOG_DEBUG("NVIDIA GPU count = {}", nGpu);
    for (int iGpu = 0 ; iGpu < nGpu ; iGpu++) {
        CUdevice cuDevice = 0;
        result = m_cuda->cuDeviceGet(&cuDevice, iGpu);
        if (result != CUDA_SUCCESS)
            continue;

        CUcontext cuContext = NULL;
        result = m_cuda->cuCtxCreate(&cuContext, 0, cuDevice);
        if (result != CUDA_SUCCESS) {
            SPDLOG_INFO("cuCtxCreate failed, device={}, ret={}", cuDevice, result);
            continue;
        }

        char deviceName[128] = {};
        int major = 0, minor = 0;
        m_cuda->cuDeviceGetName(deviceName, sizeof(deviceName), cuDevice);
        m_cuda->cuDeviceComputeCapability(&major, &minor, cuDevice);
        SPDLOG_DEBUG("== [{}] DEVICE NAME={}, version={}.{}",
                     iGpu, deviceName, major, minor);

        char strOutputFormats[64];
        if (spdlog::should_log(spdlog::level::debug)) {
            for (int i = 0; i < sizeof(aeCodec) / sizeof(aeCodec[0]); i++) {
                CUVIDDECODECAPS decodeCaps = {};
                decodeCaps.eCodecType = aeCodec[i];
                decodeCaps.eChromaFormat = aeChromaFormat[i];
                decodeCaps.nBitDepthMinus8 = anBitDepthMinus8[i];

                m_cuvid->cuvidGetDecoderCaps(&decodeCaps);

                strOutputFormats[0] = '\0';
                getOutputFormatNames(decodeCaps.nOutputFormatMask, strOutputFormats);

                // setw() width = maximum_width_of_string + 2 spaces
                std::ostringstream msg;
                msg << "Codec  " << std::left << std::setw(7) << aszCodecName[i] <<
                       "Bit  " << std::setw(4) << decodeCaps.nBitDepthMinus8 + 8 <<
                       "Chroma  " << std::setw(7) << aszChromaFormat[decodeCaps.eChromaFormat] <<
                       "Supported  " << std::setw(3) << (int)decodeCaps.bIsSupported <<
                       "MaxW  " << std::setw(7) << decodeCaps.nMaxWidth <<
                       "MaxH  " << std::setw(7) << decodeCaps.nMaxHeight <<
                       "MaxMB  " << std::setw(10) << decodeCaps.nMaxMBCount <<
                       "MinW  " << std::setw(5) << decodeCaps.nMinWidth <<
                       "MinH  " << std::setw(5) << decodeCaps.nMinHeight <<
                       "Surface  " << std::setw(11) << strOutputFormats;
                SPDLOG_DEBUG(" ({}) {}", i, msg.str());
            }
        }

        auto deviceSession = std::make_shared<CudaDeviceSession>(m_cuda, m_cuvid);
        deviceSession->cuDevice = cuDevice;
        deviceSession->cuContext = cuContext;
        deviceSession->majorVersion = major;
        deviceSession->minorVersion = minor;
        deviceSession->deviceName = deviceName;
        m_cudaSessions.push_back(deviceSession);
    }
    return true;
}

void CudaManager::Stop()
{
#ifndef Q_OS_WIN
    return;
#endif

    SPDLOG_DEBUG("Stop NvVideoDecoderManager");
    m_cudaSessions.clear();

    if (m_cuvid) {
        cuvid_free_functions(&m_cuvid);
        m_cuvid = NULL;
    }
    if (m_cuda) {
        cuda_free_functions(&m_cuda);
        m_cuda = NULL;
    }

}

bool CudaManager::IsSupport()
{
#ifndef Q_OS_WIN
    return false;
#endif
    return (m_cuda && m_cuvid);
}

CudaDeviceSessionPtr CudaManager::GetCudaSession()
{
    if (m_cudaSessions.empty())
        return nullptr;
    // TODO
    return m_cudaSessions[0];
}
