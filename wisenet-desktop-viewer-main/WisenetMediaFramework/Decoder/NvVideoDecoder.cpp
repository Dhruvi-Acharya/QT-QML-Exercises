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
#include "NvVideoDecoder.h"
#include "LogSettings.h"
#include <chrono>


#undef SPDLOG_LOGGER_CALL
#define SPDLOG_LOGGER_CALL(logger, level, ...) (logger)->log(spdlog::source_loc{__FILE__, __LINE__, "[NVDEC] "}, level, __VA_ARGS__)

inline cudaVideoCodec FFmpeg2NvCodecId(AVCodecID id) {
    switch (id) {
    case AV_CODEC_ID_MPEG1VIDEO : return cudaVideoCodec_MPEG1;
    case AV_CODEC_ID_MPEG2VIDEO : return cudaVideoCodec_MPEG2;
    case AV_CODEC_ID_MPEG4      : return cudaVideoCodec_MPEG4;
    case AV_CODEC_ID_WMV3       :
    case AV_CODEC_ID_VC1        : return cudaVideoCodec_VC1;
    case AV_CODEC_ID_H264       : return cudaVideoCodec_H264;
    case AV_CODEC_ID_HEVC       : return cudaVideoCodec_HEVC;
    case AV_CODEC_ID_VP8        : return cudaVideoCodec_VP8;
    case AV_CODEC_ID_VP9        : return cudaVideoCodec_VP9;
    case AV_CODEC_ID_MJPEG      : return cudaVideoCodec_JPEG;
    case AV_CODEC_ID_AV1        : return cudaVideoCodec_AV1;
    default                     : return cudaVideoCodec_NumCodecs;
    }
}
static const char * GetVideoCodecString(cudaVideoCodec eCodec) {
    static struct {
        cudaVideoCodec eCodec;
        const char *name;
    } aCodecName [] = {
        { cudaVideoCodec_MPEG1,     "MPEG-1"       },
        { cudaVideoCodec_MPEG2,     "MPEG-2"       },
        { cudaVideoCodec_MPEG4,     "MPEG-4 (ASP)" },
        { cudaVideoCodec_VC1,       "VC-1/WMV"     },
        { cudaVideoCodec_H264,      "AVC/H.264"    },
        { cudaVideoCodec_JPEG,      "M-JPEG"       },
        { cudaVideoCodec_H264_SVC,  "H.264/SVC"    },
        { cudaVideoCodec_H264_MVC,  "H.264/MVC"    },
        { cudaVideoCodec_HEVC,      "H.265/HEVC"   },
        { cudaVideoCodec_VP8,       "VP8"          },
        { cudaVideoCodec_VP9,       "VP9"          },
        { cudaVideoCodec_AV1,       "AV1"          },
        { cudaVideoCodec_NumCodecs, "Invalid"      },
        { cudaVideoCodec_YUV420,    "YUV  4:2:0"   },
        { cudaVideoCodec_YV12,      "YV12 4:2:0"   },
        { cudaVideoCodec_NV12,      "NV12 4:2:0"   },
        { cudaVideoCodec_YUYV,      "YUYV 4:2:2"   },
        { cudaVideoCodec_UYVY,      "UYVY 4:2:2"   },
    };

    if (eCodec >= 0 && eCodec <= cudaVideoCodec_NumCodecs) {
        return aCodecName[eCodec].name;
    }
    for (int i = cudaVideoCodec_NumCodecs + 1; i < sizeof(aCodecName) / sizeof(aCodecName[0]); i++) {
        if (eCodec == aCodecName[i].eCodec) {
            return aCodecName[eCodec].name;
        }
    }
    return "Unknown";
}
static const char * GetVideoChromaFormatString(cudaVideoChromaFormat eChromaFormat) {
    static struct {
        cudaVideoChromaFormat eChromaFormat;
        const char *name;
    } aChromaFormatName[] = {
        { cudaVideoChromaFormat_Monochrome, "YUV 400 (Monochrome)" },
        { cudaVideoChromaFormat_420,        "YUV 420"              },
        { cudaVideoChromaFormat_422,        "YUV 422"              },
        { cudaVideoChromaFormat_444,        "YUV 444"              },
    };

    if (eChromaFormat >= 0 && eChromaFormat < sizeof(aChromaFormatName) / sizeof(aChromaFormatName[0])) {
        return aChromaFormatName[eChromaFormat].name;
    }
    return "Unknown";
}

static float GetChromaHeightFactor(cudaVideoSurfaceFormat eSurfaceFormat)
{
    float factor = 0.5;
    switch (eSurfaceFormat)
    {
    case cudaVideoSurfaceFormat_NV12:
    case cudaVideoSurfaceFormat_P016:
        factor = 0.5;
        break;
    case cudaVideoSurfaceFormat_YUV444:
    case cudaVideoSurfaceFormat_YUV444_16Bit:
        factor = 1.0;
        break;
    }

    return factor;
}

static int GetChromaPlaneCount(cudaVideoSurfaceFormat eSurfaceFormat)
{
    int numPlane = 1;
    switch (eSurfaceFormat)
    {
    case cudaVideoSurfaceFormat_NV12:
    case cudaVideoSurfaceFormat_P016:
        numPlane = 1;
        break;
    case cudaVideoSurfaceFormat_YUV444:
    case cudaVideoSurfaceFormat_YUV444_16Bit:
        numPlane = 2;
        break;
    }

    return numPlane;
}


///////////////////////////////////////////////////////////////////////////////////
NvVideoDecoder::NvVideoDecoder(QObject *parent, const bool lowDelay)
    : BaseVideoDecoder(parent, lowDelay)
{
}

NvVideoDecoder::~NvVideoDecoder()
{
    release();
}

void NvVideoDecoder::release()
{
    if (m_cudaSession) {
        if (m_hParser) {
            cuvid()->cuvidDestroyVideoParser(m_hParser);
            m_hParser = NULL;
        }
        cuda()->cuCtxPushCurrent(m_cudaSession->cuContext);
        if (m_hDecoder) {
            cuvid()->cuvidDestroyDecoder(m_hDecoder);
            m_hDecoder = NULL;
        }
        {
            std::lock_guard<std::mutex> lock(m_mtxVPFrame);
            for (uint8_t *pFrame : m_vpFrame) {
                delete[] pFrame;
            }
            m_vpFrame.clear();
        }
        cuda()->cuCtxPopCurrent(NULL);
        cuvid()->cuvidCtxLockDestroy(m_ctxLock);
    }
    m_cudaSession = nullptr;


}


void NvVideoDecoder::setVideoOutParam(const VideoOutParam& outParam)
{
    m_outParam = outParam;
    return;
//    if (m_cudaSession) {
//        bool needReconfig = (m_outParam.outWidth != outParam.outWidth) ||
//                (m_outParam.outHeight != outParam.outHeight) ||
//                (m_outParam.pixelFormat != outParam.pixelFormat);

//        if (needReconfig) {
//            m_outParam = outParam;
//            SPDLOG_DEBUG("setOutParam:: out={}x{}, pixel={}",
//                         m_outParam.outWidth, m_outParam.outHeight,
//                         m_outParam.pixelFormat);
//        }
//    }
}


DecodeResult NvVideoDecoder::openWithFFmpegCodecParam(const FFmpegCodecParamSharedPtr &codecParam)
{
    /* do nothing */
    return DecodeResult::Success;
}

int NvVideoDecoder::openInternal(const Wisenet::Media::VideoSourceFrameSharedPtr& sourceFrame)
{
    int ret = -1;
    SPDLOG_DEBUG("openInternal()");

    if (m_cudaSession) {
        release();
    }

    CUVIDPARSERPARAMS videoParserParameters = {};
    m_cudaSession = CudaManager::Instance().GetCudaSession();
    if (!m_cudaSession) {
        SPDLOG_WARN("GetCudaSession() failed");
        goto __EmitReturn;
    }

    CUresult result;
    result = cuvid()->cuvidCtxLockCreate(&m_ctxLock, m_cudaSession->cuContext);
    if (result != CUDA_SUCCESS) {
        SPDLOG_ERROR("cuvidCtxLockCreate() failed, ret={}", result);
        goto __EmitReturn;
    }

    AVCodecID ffCodecID = get_vcodec_id(sourceFrame->videoCodecType);
    if (ffCodecID == AV_CODEC_ID_NONE) {
        SPDLOG_INFO("Not support codec::{}", sourceFrame->videoCodecType);
        goto __EmitReturn;
    }

    cudaVideoCodec nvCodecId = FFmpeg2NvCodecId(ffCodecID);
    videoParserParameters.CodecType = nvCodecId;
    videoParserParameters.ulMaxNumDecodeSurfaces = 1;
    videoParserParameters.ulClockRate = 1000;
    videoParserParameters.ulMaxDisplayDelay = m_lowDelay ? 0 : 1;
    videoParserParameters.pUserData = this;

    // Callback function to be registered for getting a callback
    videoParserParameters.pfnSequenceCallback = HandleVideoSequenceProc;
    videoParserParameters.pfnDecodePicture = HandlePictureDecodeProc;
    videoParserParameters.pfnDisplayPicture = HandlePictureDisplayProc;
    videoParserParameters.pfnGetOperatingPoint = HandleOperatingPointProc;

    result = cuvid()->cuvidCreateVideoParser(&m_hParser, &videoParserParameters);
    if (result != CUDA_SUCCESS) {
        SPDLOG_INFO("cuvidCreateVideoParser() failed, ret={}", result);
        goto __EmitReturn;
    }

    m_eCodec = nvCodecId;

    if (m_outParam->outWidth == 0 || m_outParam->outHeight == 0) {
        m_outParam->outWidth = sourceFrame->videoWidth;
        m_outParam->outHeight = sourceFrame->videoHeight;
    }
    if (m_outParam->pixelFormat == VideoOutPixelFormat::Unknown)
        m_outParam->pixelFormat = VideoOutPixelFormat::NV12;
    ret = 0;
    SPDLOG_DEBUG("Success to open cuda video decoder, codec={}, width={}, height={}, low_delay={}",
                 GetVideoCodecString(m_eCodec),
                 sourceFrame->videoWidth, sourceFrame->videoHeight, m_lowDelay);

__EmitReturn:
    if (ret < 0)
        release();

    return ret;
}

DecodeResult NvVideoDecoder::decodeStart(const Wisenet::Media::VideoSourceFrameSharedPtr& videoSourceFrame,
                                         VideoOutputFrameSharedPtr& outFrame)
{
    auto start = std::chrono::steady_clock::now();

    int ret = -1;
    bool gotPicture = false;
    CUresult result;

    if (!m_cudaSession || !m_hParser) {
        if (videoSourceFrame->videoFrameType != Wisenet::Media::VideoFrameType::I_FRAME) {
            SPDLOG_DEBUG("Not ready to init with key frame.. skip it.");
            goto __EmitReturn;
        }

        ret = openInternal(videoSourceFrame);
        if (ret < 0) {
            goto __EmitReturn;
        }
    }

    CUVIDSOURCEDATAPACKET packet = { 0 };
    packet.payload = videoSourceFrame->getDataPtr();
    packet.payload_size = videoSourceFrame->getDataSize();
    packet.flags |= CUVID_PKT_TIMESTAMP;
    packet.timestamp = videoSourceFrame->frameTime.ptsTimestampMsec;

    result = cuvid()->cuvidParseVideoData(m_hParser, &packet);
    if (result != CUDA_SUCCESS) {
        SPDLOG_INFO("cuvidParseVideoData() failed");
        goto __EmitReturn;
    }

__EmitReturn:
    auto end = std::chrono::steady_clock::now();
    int elapsedMsec = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    if (gotPicture)
        ret = 0;

    DecodeResult dresult = (ret==0) ? DecodeResult::Success : DecodeResult::DecodeFail;

    m_decodeCount++;
    m_decodeTimeMsec += elapsedMsec;
    constexpr int checkFrameCount = 300;
    if (m_decodeCount >= checkFrameCount) {
        double frameAvg = (double)m_decodeTimeMsec / m_decodeCount;
        STATLOG_DEBUG("Decoding {} frames avg elapsed Millseconds={}",
                      checkFrameCount, frameAvg);
        m_decodeCount = 0;
        m_decodeTimeMsec = 0;
    }

    return dresult;
}


//////////////////////////////////////////////////////////////////////////////////////////

// Callback function to be registered for getting a callback when decoding of sequence starts
int CUDAAPI NvVideoDecoder::HandleVideoSequenceProc(void *pUserData, CUVIDEOFORMAT *pVideoFormat)
{
    return ((NvVideoDecoder *)pUserData)->HandleVideoSequence(pVideoFormat);
}

// Callback function to be registered for getting a callback when a decoded frame is ready to be decoded
int CUDAAPI NvVideoDecoder::HandlePictureDecodeProc(void *pUserData, CUVIDPICPARAMS *pPicParams)
{
    return ((NvVideoDecoder *)pUserData)->HandlePictureDecode(pPicParams);
}

//  Callback function to be registered for getting a callback when a decoded frame is available for display
int CUDAAPI NvVideoDecoder::HandlePictureDisplayProc(void *pUserData, CUVIDPARSERDISPINFO *pDispInfo)
{
    return ((NvVideoDecoder *)pUserData)->HandlePictureDisplay(pDispInfo);
}

// Callback function to be registered for getting a callback to get operating point when AV1 SVC sequence header start.
int CUDAAPI NvVideoDecoder::HandleOperatingPointProc(void *pUserData, CUVIDOPERATINGPOINTINFO *pOPInfo)
{
    return ((NvVideoDecoder *)pUserData)->GetOperatingPoint(pOPInfo);
}


/* Return value from HandleVideoSequence() are interpreted as   :
*  0: fail, 1: succeeded, > 1: override dpb size of parser (set by CUVIDPARSERPARAMS::ulMaxNumDecodeSurfaces while creating parser)
*/
int NvVideoDecoder::HandleVideoSequence(CUVIDEOFORMAT *pVideoFormat)
{
    if (spdlog::should_log(spdlog::level::debug)) {
        std::ostringstream videoInfo;
        videoInfo << "HandleVideoSequence() :: Video Input Information" << std::endl
                  << "\tCodec        : " << GetVideoCodecString(pVideoFormat->codec) << std::endl
                  << "\tFrame rate   : " << pVideoFormat->frame_rate.numerator << "/" << pVideoFormat->frame_rate.denominator
                  << " = " << 1.0 * pVideoFormat->frame_rate.numerator / pVideoFormat->frame_rate.denominator << " fps" << std::endl
                  << "\tSequence     : " << (pVideoFormat->progressive_sequence ? "Progressive" : "Interlaced") << std::endl
                  << "\tCoded size   : [" << pVideoFormat->coded_width << ", " << pVideoFormat->coded_height << "]" << std::endl
                  << "\tDisplay area : [" << pVideoFormat->display_area.left << ", " << pVideoFormat->display_area.top << ", "
                << pVideoFormat->display_area.right << ", " << pVideoFormat->display_area.bottom << "]" << std::endl
                << "\tChroma       : " << GetVideoChromaFormatString(pVideoFormat->chroma_format) << std::endl
                << "\tBit depth    : " << pVideoFormat->bit_depth_luma_minus8 + 8
                   ;
        videoInfo << std::endl;
        SPDLOG_DEBUG("{}", videoInfo.str());
    }

    int nDecodeSurface = pVideoFormat->min_num_decode_surfaces;
    CUVIDDECODECAPS decodecaps;
    std::memset(&decodecaps, 0, sizeof(decodecaps));
    decodecaps.eCodecType = pVideoFormat->codec;
    decodecaps.eChromaFormat = pVideoFormat->chroma_format;
    decodecaps.nBitDepthMinus8 = pVideoFormat->bit_depth_luma_minus8;

    cuda()->cuCtxPushCurrent(context());
    cuvid()->cuvidGetDecoderCaps(&decodecaps);
    cuda()->cuCtxPopCurrent(NULL);

    if(!decodecaps.bIsSupported){
        SPDLOG_INFO("Codec not supported on this GPU");
        return 0;
    }

    if ((pVideoFormat->coded_width > decodecaps.nMaxWidth) ||
            (pVideoFormat->coded_height > decodecaps.nMaxHeight)){
        SPDLOG_INFO("Resolution not supported on this GPU, resolution={}x{}, Supported={}x{}",
                    pVideoFormat->coded_width, pVideoFormat->coded_height,
                    decodecaps.nMaxWidth, decodecaps.nMaxHeight);
        return 0;
    }

    if ((pVideoFormat->coded_width>>4)*(pVideoFormat->coded_height>>4) > decodecaps.nMaxMBCount){
        SPDLOG_INFO("MBCount not supported on this GPU, MBCount={}, Supported={}",
                    (pVideoFormat->coded_width >> 4)*(pVideoFormat->coded_height >> 4),
                    decodecaps.nMaxMBCount);
        return 0;
    }

    if (m_nWidth && m_nLumaHeight && m_nChromaHeight) {
        // cuvidCreateDecoder() has been called before, and now there's possible config change
        return ReconfigureDecoder(pVideoFormat);
    }

    // eCodec has been set in the constructor (for parser). Here it's set again for potential correction
    m_eCodec = pVideoFormat->codec;
    m_eChromaFormat = pVideoFormat->chroma_format;
    m_nBitDepthMinus8 = pVideoFormat->bit_depth_luma_minus8;
    m_nBPP = m_nBitDepthMinus8 > 0 ? 2 : 1;

    // Set the output surface format same as chroma format
    if (m_eChromaFormat == cudaVideoChromaFormat_420 || cudaVideoChromaFormat_Monochrome)
        m_eOutputFormat = pVideoFormat->bit_depth_luma_minus8 ? cudaVideoSurfaceFormat_P016 : cudaVideoSurfaceFormat_NV12;
    else if (m_eChromaFormat == cudaVideoChromaFormat_444)
        m_eOutputFormat = pVideoFormat->bit_depth_luma_minus8 ? cudaVideoSurfaceFormat_YUV444_16Bit : cudaVideoSurfaceFormat_YUV444;
    else if (m_eChromaFormat == cudaVideoChromaFormat_422)
        m_eOutputFormat = cudaVideoSurfaceFormat_NV12;  // no 4:2:2 output format supported yet so make 420 default

    // Check if output format supported. If not, check falback options
    if (!(decodecaps.nOutputFormatMask & (1 << m_eOutputFormat)))
    {
        if (decodecaps.nOutputFormatMask & (1 << cudaVideoSurfaceFormat_NV12))
            m_eOutputFormat = cudaVideoSurfaceFormat_NV12;
        else if (decodecaps.nOutputFormatMask & (1 << cudaVideoSurfaceFormat_P016))
            m_eOutputFormat = cudaVideoSurfaceFormat_P016;
        else if (decodecaps.nOutputFormatMask & (1 << cudaVideoSurfaceFormat_YUV444))
            m_eOutputFormat = cudaVideoSurfaceFormat_YUV444;
        else if (decodecaps.nOutputFormatMask & (1 << cudaVideoSurfaceFormat_YUV444_16Bit))
            m_eOutputFormat = cudaVideoSurfaceFormat_YUV444_16Bit;
        else {
            SPDLOG_INFO("No supported output format found");
            return 0;
        }
    }
    m_videoFormat = *pVideoFormat;

    CUVIDDECODECREATEINFO videoDecodeCreateInfo = { 0 };
    videoDecodeCreateInfo.CodecType = pVideoFormat->codec;
    videoDecodeCreateInfo.ChromaFormat = pVideoFormat->chroma_format;
    videoDecodeCreateInfo.OutputFormat = m_eOutputFormat;
    videoDecodeCreateInfo.bitDepthMinus8 = pVideoFormat->bit_depth_luma_minus8;
    if (pVideoFormat->progressive_sequence)
        videoDecodeCreateInfo.DeinterlaceMode = cudaVideoDeinterlaceMode_Weave;
    else
        videoDecodeCreateInfo.DeinterlaceMode = cudaVideoDeinterlaceMode_Adaptive;
    videoDecodeCreateInfo.ulNumOutputSurfaces = 2;
    // With PreferCUVID, JPEG is still decoded by CUDA while video is decoded by NVDEC hardware
    videoDecodeCreateInfo.ulCreationFlags = cudaVideoCreate_PreferCUVID;
    videoDecodeCreateInfo.ulNumDecodeSurfaces = nDecodeSurface;
    videoDecodeCreateInfo.vidLock = m_ctxLock;
    videoDecodeCreateInfo.ulWidth = pVideoFormat->coded_width;
    videoDecodeCreateInfo.ulHeight = pVideoFormat->coded_height;

    // AV1 has max width/height of sequence in sequence header
    if (pVideoFormat->codec == cudaVideoCodec_AV1 && pVideoFormat->seqhdr_data_length > 0)
    {
        // dont overwrite if it is already set from cmdline or reconfig.txt
        if (!(m_nMaxWidth > pVideoFormat->coded_width || m_nMaxHeight > pVideoFormat->coded_height))
        {
            CUVIDEOFORMATEX *vidFormatEx = (CUVIDEOFORMATEX *)pVideoFormat;
            m_nMaxWidth = vidFormatEx->av1.max_width;
            m_nMaxHeight = vidFormatEx->av1.max_height;
        }
    }
    if (m_nMaxWidth < (int)pVideoFormat->coded_width)
        m_nMaxWidth = pVideoFormat->coded_width;
    if (m_nMaxHeight < (int)pVideoFormat->coded_height)
        m_nMaxHeight = pVideoFormat->coded_height;
    videoDecodeCreateInfo.ulMaxWidth = m_nMaxWidth;
    videoDecodeCreateInfo.ulMaxHeight = m_nMaxHeight;

    bool bResized = false; // TODO::
    if (!bResized) {
        m_nWidth = pVideoFormat->display_area.right - pVideoFormat->display_area.left;
        m_nLumaHeight = pVideoFormat->display_area.bottom - pVideoFormat->display_area.top;
        videoDecodeCreateInfo.ulTargetWidth = pVideoFormat->coded_width;
        videoDecodeCreateInfo.ulTargetHeight = pVideoFormat->coded_height;
    }
    else {
        videoDecodeCreateInfo.display_area.left = pVideoFormat->display_area.left;
        videoDecodeCreateInfo.display_area.top = pVideoFormat->display_area.top;
        videoDecodeCreateInfo.display_area.right = pVideoFormat->display_area.right;
        videoDecodeCreateInfo.display_area.bottom = pVideoFormat->display_area.bottom;
        m_nWidth = m_outParam->outWidth;
        m_nLumaHeight = m_outParam->outHeight;
        // <hh3.kim> 2018-11-12 : 해상도 640x360 일때 하단부 회식픽셀 표시되는 문제 수정
        // TODO::
        //	if (m_dci.ulWidth == 640 && m_dci.ulHeight == 368)
        //	{
        //		m_dci.display_area.left = 0;
        //		m_dci.display_area.right = nOutWidth;
        //		m_dci.display_area.top = 0;
        //		m_dci.display_area.bottom = nOutHeight;
        //	}
    }

    m_nChromaHeight = (int)(ceil(m_nLumaHeight * GetChromaHeightFactor(m_eOutputFormat)));
    m_nNumChromaPlanes = GetChromaPlaneCount(m_eOutputFormat);
    m_nSurfaceHeight = videoDecodeCreateInfo.ulTargetHeight;
    m_nSurfaceWidth = videoDecodeCreateInfo.ulTargetWidth;
    m_displayRect.b = videoDecodeCreateInfo.display_area.bottom;
    m_displayRect.t = videoDecodeCreateInfo.display_area.top;
    m_displayRect.l = videoDecodeCreateInfo.display_area.left;
    m_displayRect.r = videoDecodeCreateInfo.display_area.right;

    if (spdlog::should_log(spdlog::level::debug)) {
        std::ostringstream videoInfo;
        videoInfo << "Video Decoding Params:" << std::endl
                   << "\tNum Surfaces : " << videoDecodeCreateInfo.ulNumDecodeSurfaces << std::endl
                   << "\tCrop         : [" << videoDecodeCreateInfo.display_area.left << ", " << videoDecodeCreateInfo.display_area.top << ", "
        << videoDecodeCreateInfo.display_area.right << ", " << videoDecodeCreateInfo.display_area.bottom << "]" << std::endl
        << "\tResize       : " << videoDecodeCreateInfo.ulTargetWidth << "x" << videoDecodeCreateInfo.ulTargetHeight << std::endl
        << "\tDeinterlace  : " << std::vector<const char *>{"Weave", "Bob", "Adaptive"}[videoDecodeCreateInfo.DeinterlaceMode]
           ;
        videoInfo << std::endl;
        SPDLOG_DEBUG("{}", videoInfo.str());
    }

    cuda()->cuCtxPushCurrent(context());
    CUresult result = cuvid()->cuvidCreateDecoder(&m_hDecoder, &videoDecodeCreateInfo);
    cuda()->cuCtxPopCurrent(NULL);
    if (result != CUDA_SUCCESS) {
        SPDLOG_INFO("cuvidCreateDecoder() failed()");
        return 0;
    }
    return nDecodeSurface;
}

int NvVideoDecoder::ReconfigureDecoder(CUVIDEOFORMAT *pVideoFormat)
{
    if (pVideoFormat->bit_depth_luma_minus8 != m_videoFormat.bit_depth_luma_minus8 ||
            pVideoFormat->bit_depth_chroma_minus8 != m_videoFormat.bit_depth_chroma_minus8){
        SPDLOG_INFO("Reconfigure Not supported for bit depth change");
        return 0;
    }

    if (pVideoFormat->chroma_format != m_videoFormat.chroma_format) {
        SPDLOG_INFO("Reconfigure Not supported for chroma format change");
        return 0;
    }

    bool bDecodeResChange = !(pVideoFormat->coded_width == m_videoFormat.coded_width && pVideoFormat->coded_height == m_videoFormat.coded_height);
    bool bDisplayRectChange = !(pVideoFormat->display_area.bottom == m_videoFormat.display_area.bottom && pVideoFormat->display_area.top == m_videoFormat.display_area.top \
                                && pVideoFormat->display_area.left == m_videoFormat.display_area.left && pVideoFormat->display_area.right == m_videoFormat.display_area.right);

    int nDecodeSurface = pVideoFormat->min_num_decode_surfaces;

    if ((pVideoFormat->coded_width > m_nMaxWidth) || (pVideoFormat->coded_height > m_nMaxHeight)) {
        // For VP9, let driver  handle the change if new width/height > maxwidth/maxheight
        if ((m_eCodec != cudaVideoCodec_VP9) || m_bReconfigExternal)
        {
            SPDLOG_INFO("Reconfigure Not supported when width/height > maxwidth/maxheight");
            return 0;
        }
        return 1;
    }

    if (!bDecodeResChange && !m_bReconfigExternal) {
        // if the coded_width/coded_height hasn't changed but display resolution has changed, then need to update width/height for
        // correct output without cropping. Example : 1920x1080 vs 1920x1088
        if (bDisplayRectChange)
        {
            m_nWidth = pVideoFormat->display_area.right - pVideoFormat->display_area.left;
            m_nLumaHeight = pVideoFormat->display_area.bottom - pVideoFormat->display_area.top;
            m_nChromaHeight = (int)ceil(m_nLumaHeight * GetChromaHeightFactor(m_eOutputFormat));
            m_nNumChromaPlanes = GetChromaPlaneCount(m_eOutputFormat);
        }

        // no need for reconfigureDecoder(). Just return
        return 1;
    }

    CUVIDRECONFIGUREDECODERINFO reconfigParams = { 0 };

    reconfigParams.ulWidth = m_videoFormat.coded_width = pVideoFormat->coded_width;
    reconfigParams.ulHeight = m_videoFormat.coded_height = pVideoFormat->coded_height;

    // Dont change display rect and get scaled output from decoder. This will help display app to present apps smoothly
    reconfigParams.display_area.bottom = m_displayRect.b;
    reconfigParams.display_area.top = m_displayRect.t;
    reconfigParams.display_area.left = m_displayRect.l;
    reconfigParams.display_area.right = m_displayRect.r;
    reconfigParams.ulTargetWidth = m_nSurfaceWidth;
    reconfigParams.ulTargetHeight = m_nSurfaceHeight;

    // If external reconfigure is called along with resolution change even if post processing params is not changed,
    // do full reconfigure params update
    if (m_bReconfigExternal || bDecodeResChange) {
        // update display rect and target resolution if requested explicitely
        if (m_bReconfigExternal) {
            m_bReconfigExternal = false;
            reconfigParams.display_area.left = pVideoFormat->display_area.left;
            reconfigParams.display_area.top = pVideoFormat->display_area.top;
            reconfigParams.display_area.right = pVideoFormat->display_area.right;
            reconfigParams.display_area.bottom = pVideoFormat->display_area.bottom;
            m_nWidth = m_outParam->outWidth;
            m_nLumaHeight = m_outParam->outHeight;
            reconfigParams.ulTargetWidth = m_nWidth;
            reconfigParams.ulTargetHeight = m_nLumaHeight;
        }
        else if (bDecodeResChange) {
            m_nWidth = pVideoFormat->display_area.right - pVideoFormat->display_area.left;
            m_nLumaHeight = pVideoFormat->display_area.bottom - pVideoFormat->display_area.top;
            reconfigParams.ulTargetWidth = pVideoFormat->coded_width;
            reconfigParams.ulTargetHeight = pVideoFormat->coded_height;
        }

        m_nChromaHeight = (int)ceil(m_nLumaHeight * GetChromaHeightFactor(m_eOutputFormat));
        m_nNumChromaPlanes = GetChromaPlaneCount(m_eOutputFormat);
        m_nSurfaceHeight = reconfigParams.ulTargetHeight;
        m_nSurfaceWidth = reconfigParams.ulTargetWidth;
        m_displayRect.b = reconfigParams.display_area.bottom;
        m_displayRect.t = reconfigParams.display_area.top;
        m_displayRect.l = reconfigParams.display_area.left;
        m_displayRect.r = reconfigParams.display_area.right;
    }

    reconfigParams.ulNumDecodeSurfaces = nDecodeSurface;

    cuda()->cuCtxPushCurrent(context());
    cuvid()->cuvidReconfigureDecoder(m_hDecoder, &reconfigParams);
    cuda()->cuCtxPopCurrent(NULL);

    return nDecodeSurface;
}


/* Return value from HandlePictureDecode() are interpreted as:
*  0: fail, >=1: succeeded
*/
int NvVideoDecoder::HandlePictureDecode(CUVIDPICPARAMS *pPicParams)
{
    if (!m_hDecoder) {
        SPDLOG_WARN("Decoder not initialized.");
        return 0;
    }
    m_nPicNumInDecodeOrder[pPicParams->CurrPicIdx] = m_nDecodePicCnt++;

    cuda()->cuCtxPushCurrent(context());
    auto result = cuvid()->cuvidDecodePicture(m_hDecoder, pPicParams);
    cuda()->cuCtxPopCurrent(NULL);
    if (result != CUDA_SUCCESS)
        return 0;

    return 1;
}

/* Return value from HandlePictureDisplay() are interpreted as:
*  0: fail, >=1: succeeded
*/
int NvVideoDecoder::HandlePictureDisplay(CUVIDPARSERDISPINFO *pDispInfo)
{
    SPDLOG_DEBUG("HandlePictureDisplay() START");
    CUVIDPROCPARAMS videoProcessingParameters = {};
    videoProcessingParameters.progressive_frame = pDispInfo->progressive_frame;
    videoProcessingParameters.second_field = pDispInfo->repeat_first_field + 1;
    videoProcessingParameters.top_field_first = pDispInfo->top_field_first;
    videoProcessingParameters.unpaired_field = pDispInfo->repeat_first_field < 0;
    videoProcessingParameters.output_stream = m_cuvidStream;

    CUdeviceptr dpSrcFrame = 0;
    unsigned int nSrcPitch = 0;
    cuda()->cuCtxPushCurrent(context());
    cuvid()->cuvidMapVideoFrame(m_hDecoder, pDispInfo->picture_index, &dpSrcFrame,
                                &nSrcPitch, &videoProcessingParameters);

    CUVIDGETDECODESTATUS DecodeStatus;
    memset(&DecodeStatus, 0, sizeof(DecodeStatus));
    CUresult result = cuvid()->cuvidGetDecodeStatus(m_hDecoder, pDispInfo->picture_index, &DecodeStatus);
    if (result == CUDA_SUCCESS &&
            (DecodeStatus.decodeStatus == cuvidDecodeStatus_Error ||
             DecodeStatus.decodeStatus == cuvidDecodeStatus_Error_Concealed))
    {
        SPDLOG_DEBUG("Decode Error occurred for picture {}", m_nPicNumInDecodeOrder[pDispInfo->picture_index]);
    }

    uint8_t *pDecodedFrame = nullptr;
    {
        std::lock_guard<std::mutex> lock(m_mtxVPFrame);
        if ((unsigned)++m_nDecodedFrame > m_vpFrame.size())
        {
            // Not enough frames in stock
            m_nFrameAlloc++;
            uint8_t *pFrame = NULL;
            pFrame = new uint8_t[GetFrameSize()];
            m_vpFrame.push_back(pFrame);
        }
        pDecodedFrame = m_vpFrame[m_nDecodedFrame - 1];
    }

    // Copy luma plane
    CUDA_MEMCPY2D m = { 0 };
    m.srcMemoryType = CU_MEMORYTYPE_DEVICE;
    m.srcDevice = dpSrcFrame;
    m.srcPitch = nSrcPitch;
    m.dstMemoryType = CU_MEMORYTYPE_HOST;
    m.dstDevice = (CUdeviceptr)(m.dstHost = pDecodedFrame);
    m.dstPitch = GetWidth() * m_nBPP;
    m.WidthInBytes = GetWidth() * m_nBPP;
    m.Height = m_nLumaHeight;
    cuda()->cuMemcpy2DAsync(&m, m_cuvidStream);

    // Copy chroma plane
    // NVDEC output has luma height aligned by 2. Adjust chroma offset by aligning height
    m.srcDevice = (CUdeviceptr)((uint8_t *)dpSrcFrame + m.srcPitch * ((m_nSurfaceHeight + 1) & ~1));
    m.dstDevice = (CUdeviceptr)(m.dstHost = pDecodedFrame + m.dstPitch * m_nLumaHeight);
    m.Height = m_nChromaHeight;
    cuda()->cuMemcpy2DAsync(&m, m_cuvidStream);

    if (m_nNumChromaPlanes == 2)
    {
        m.srcDevice = (CUdeviceptr)((uint8_t *)dpSrcFrame + m.srcPitch * ((m_nSurfaceHeight + 1) & ~1) * 2);
        m.dstDevice = (CUdeviceptr)(m.dstHost = pDecodedFrame + m.dstPitch * m_nLumaHeight * 2);
        m.Height = m_nChromaHeight;
        cuda()->cuMemcpy2DAsync(&m, m_cuvidStream);
    }
    cuda()->cuStreamSynchronize(m_cuvidStream);
    cuda()->cuCtxPopCurrent(NULL);

    if ((int)m_vTimestamp.size() < m_nDecodedFrame) {
        m_vTimestamp.resize(m_vpFrame.size());
    }
    m_vTimestamp[m_nDecodedFrame - 1] = pDispInfo->timestamp;

    cuvid()->cuvidUnmapVideoFrame(m_hDecoder, dpSrcFrame);
    return 1;
}

/* Called when the parser encounters sequence header for AV1 SVC content
*  return value interpretation:
*      < 0 : fail, >=0: succeeded (bit 0-9: currOperatingPoint, bit 10-10: bDispAllLayer, bit 11-30: reserved, must be set 0)
*/
int NvVideoDecoder::GetOperatingPoint(CUVIDOPERATINGPOINTINFO *pOPInfo)
{
    if (pOPInfo->codec == cudaVideoCodec_AV1) {
        if (pOPInfo->av1.operating_points_cnt > 1) {

            // clip has SVC enabled
            if (m_nOperatingPoint >= pOPInfo->av1.operating_points_cnt)
                m_nOperatingPoint = 0;

            SPDLOG_DEBUG("AV1 SVC clip: operating point count {0:d} Selected operating point: {1:d}, IDC {2:x} bOutputAllLayers {3:d}",
                         pOPInfo->av1.operating_points_cnt,
                         m_nOperatingPoint, pOPInfo->av1.operating_points_idc[m_nOperatingPoint],
                         m_bDispAllLayers);
            return (m_nOperatingPoint | (m_bDispAllLayers << 10));
        }
    }
    return -1;
}


CudaFunctions *NvVideoDecoder::cuda()
{
    if (!m_cudaSession) {
        return NULL;
    }
    return m_cudaSession->m_cuda;
}

CuvidFunctions *NvVideoDecoder::cuvid()
{
    if (!m_cudaSession) {
        return NULL;
    }
    return m_cudaSession->m_cuvid;
}

CUcontext NvVideoDecoder::context()
{
    if (!m_cudaSession) {
        return NULL;
    }
    return m_cudaSession->cuContext;
}

/**
*  @brief  This function is used to get the output frame width.
*  NV12/P016 output format width is 2 byte aligned because of U and V interleave
*/
int NvVideoDecoder::GetWidth()
{
    if (m_eOutputFormat == cudaVideoSurfaceFormat_NV12 || m_eOutputFormat == cudaVideoSurfaceFormat_P016)
        return ((m_nWidth + 1) & ~1 );

    return m_nWidth;
}

int NvVideoDecoder::GetFrameSize()
{
    return GetWidth() * (m_nLumaHeight + (m_nChromaHeight * m_nNumChromaPlanes)) * m_nBPP;
}
