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
#pragma once

#include <vector>
#include "BaseDecoder.h"
#include "VideoScaler.h"
#include "CudaManager.h"


class NvVideoDecoder : public BaseVideoDecoder
{
    struct Rect {
        int l, t, r, b;
    };

    Q_OBJECT
public:
    explicit NvVideoDecoder(QObject *parent = nullptr, const bool lowDelay=false);
    ~NvVideoDecoder();


    void setVideoOutParam(const VideoOutParam& outParam) override;
    DecodeResult openWithFFmpegCodecParam(const FFmpegCodecParamSharedPtr& codecParam) override;
    DecodeResult decodeStart(const Wisenet::Media::VideoSourceFrameSharedPtr& videoSourceFrame,
                             VideoOutputFrameSharedPtr& outFrame) override;
private:
    void release();
    int openInternal(const Wisenet::Media::VideoSourceFrameSharedPtr& sourceFrame);
    // callback for nvdec
private:
    static int CUDAAPI HandleVideoSequenceProc(void *pUserData, CUVIDEOFORMAT *pVideoFormat);
    static int CUDAAPI HandlePictureDecodeProc(void *pUserData, CUVIDPICPARAMS *pPicParams);
    static int CUDAAPI HandlePictureDisplayProc(void *pUserData, CUVIDPARSERDISPINFO *pDispInfo);
    static int CUDAAPI HandleOperatingPointProc(void *pUserData, CUVIDOPERATINGPOINTINFO *pOPInfo);

    // This function gets called when a sequence is ready to be decoded.
    // The function also gets called when there is format change
    int HandleVideoSequence(CUVIDEOFORMAT *pVideoFormat);

    // This function gets called when a picture is ready to be decoded.
    // cuvidDecodePicture is called from this function to decode the picture
    int HandlePictureDecode(CUVIDPICPARAMS *pPicParams);

    // This function gets called after a picture is decoded and available for display.
    // Frames are fetched and stored in internal buffer
    int HandlePictureDisplay(CUVIDPARSERDISPINFO *pDispInfo);

    // This function gets called when AV1 sequence encounter more than one operating points
    int GetOperatingPoint(CUVIDOPERATINGPOINTINFO *pOPInfo);

    int ReconfigureDecoder(CUVIDEOFORMAT *pVideoFormat);

    CudaFunctions *cuda();
    CuvidFunctions *cuvid();
    CUcontext context();


    int GetWidth();
    int GetFrameSize();
private:
    CudaDeviceSessionPtr    m_cudaSession = nullptr;

    CUvideoctxlock          m_ctxLock;
    CUvideoparser           m_hParser = NULL;
    CUvideodecoder          m_hDecoder = NULL;
    size_t                  m_decodeCount = 0;
    size_t                  m_decodeTimeMsec = 0;

    // dimension of the output
    unsigned int m_nWidth = 0, m_nLumaHeight = 0, m_nChromaHeight = 0;
    unsigned int m_nNumChromaPlanes = 0;

    unsigned int m_nMaxWidth = 0, m_nMaxHeight = 0;
    int m_nSurfaceHeight = 0;
    int m_nSurfaceWidth = 0;
    cudaVideoCodec m_eCodec = cudaVideoCodec_NumCodecs;
    cudaVideoChromaFormat m_eChromaFormat = cudaVideoChromaFormat_420;
    cudaVideoSurfaceFormat m_eOutputFormat = cudaVideoSurfaceFormat_NV12;
    int m_nBitDepthMinus8 = 0;
    int m_nBPP = 1;
    CUVIDEOFORMAT m_videoFormat = {};
    Rect m_displayRect = {};
    int m_nDecodePicCnt = 0, m_nPicNumInDecodeOrder[32];
    bool m_bReconfigExternal = false;

    // set operating point for AV1 SVC clips
    unsigned int m_nOperatingPoint = 0;
    bool  m_bDispAllLayers = false;

    // stock of frames
    std::mutex m_mtxVPFrame;
    CUstream m_cuvidStream = 0;
    std::vector<uint8_t *> m_vpFrame;
    int m_nFrameAlloc = 0;
    int m_nDecodedFrame = 0, m_nDecodedFrameReturned = 0;
    std::vector<int64_t> m_vTimestamp;
};



