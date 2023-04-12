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
#include "VideoScaler.h"
#include "LogSettings.h"


#undef SPDLOG_LOGGER_CALL
#define SPDLOG_LOGGER_CALL(logger, level, ...) (logger)->log(spdlog::source_loc{__FILE__, __LINE__, "[FFMPEG-VSCALE] "}, level, __VA_ARGS__)


static bool needToScale(const AVFrame *srcFrame, const VideoOutParam &outParam, int& scaleOpt, int& dstWidth, int& dstHeight)
{
    if (outParam.keepOriginalSize)
        return false;
    if (outParam.outHeight <= 0 || outParam.outWidth <= 0) {
        SPDLOG_DEBUG("needToScale(), invalid width/height param, width={}, height={}",
                     outParam.outWidth, outParam.outHeight);
        return false;
    }

    const int srcWidth = srcFrame->width;
    const int srcHeight = srcFrame->height;

    if(srcWidth >= srcHeight) {
        dstWidth = outParam.maximumOutSize == 0 ? outParam.outWidth : std::min(srcWidth, outParam.maximumOutSize);
        dstWidth = Align16(dstWidth);
        dstHeight = dstWidth * srcHeight / srcWidth;
        dstHeight = dstHeight%2 == 0 ? dstHeight : dstHeight+1;
    }
    else {
        dstHeight = outParam.maximumOutSize == 0 ? outParam.outHeight : std::min(srcHeight, outParam.maximumOutSize);
        dstHeight = Align16(dstHeight);
        dstWidth = dstHeight * srcWidth / srcHeight;
        dstWidth = dstWidth%2 == 0 ? dstWidth : dstWidth+1;
    }

    //SPDLOG_DEBUG("Check needToScale(), {}x{}, {}x{}", srcWidth, srcHeight, dstWidth, dstHeight);

    // 스케일 계산 값이 원본 대비 80% 이상인 경우에는 원본을 그대로 올려주도록 수정
    if (srcWidth*0.8 <= dstWidth) {
        return false;
    }

    // 22년 6월 2일 이전 주석
    // width 기준 원본영상 대비 20%이하면 FAST_BILINEAR, 아니면 BILINEAR
    //scaleOpt = srcWidth*0.20 <= dstWidth ? SWS_BILINEAR : SWS_FAST_BILINEAR;

    // 22년 6월 2일 주석
    /* SQA PC 에서 3840 * 2160 해상도 4CH 플레이시 화질 열화 발생 (Destination 모니터 해상도 4K 모니터를 1920* 1080으로 조정하여 테스트)
     * SWS_FAST_BILINEAR // CPU : 29~31%, GPU : 28~29%
     * SWS_BILINEAR      // CPU : 33~34%, GPU : 27~29%
     * 성능 측정하여 문제가 기존과 차이가 크면 SWS_FAST_BILINEAR 사용 검토가 필요할수도 있다.
    */
    scaleOpt = SWS_BILINEAR;

    //SPDLOG_DEBUG("scaleOpt={},{},{},{}", scaleOpt, srcWidth, dstWidth, srcWidth*0.25);
    return true;
}


VideoScaler::VideoScaler()
{

}

VideoScaler::~VideoScaler()
{
    release();
}


inline AVPixelFormat get_av_supported_pixel_format(AVPixelFormat sourceFormat,
                                                   VideoOutPixelFormat outPixel)
{
    // output pixel format이 지정된 경우에는 outpixel format 우선 리턴
    if (outPixel == VideoOutPixelFormat::YUV420P)
        return AVPixelFormat::AV_PIX_FMT_YUV420P;
    if (outPixel == VideoOutPixelFormat::YUVJ420P)
        return AVPixelFormat::AV_PIX_FMT_YUVJ420P;
    if (outPixel == VideoOutPixelFormat::NV12)
        return AVPixelFormat::AV_PIX_FMT_NV12;
    if (outPixel == VideoOutPixelFormat::BGRA)
        return AVPixelFormat::AV_PIX_FMT_BGRA;

    // output pixel format이 지정이 되지 않은 경우,
    // source 가 yuv420p, yuvj420p, nv12, bgra를 제외하고는
    // yuv420p로 사용
    if (sourceFormat == AVPixelFormat::AV_PIX_FMT_YUV420P ||
            sourceFormat == AVPixelFormat::AV_PIX_FMT_YUVJ420P ||
            sourceFormat == AVPixelFormat::AV_PIX_FMT_NV12 ||
            sourceFormat == AVPixelFormat::AV_PIX_FMT_BGRA)
        return sourceFormat;

    return AVPixelFormat::AV_PIX_FMT_YUV420P;
}

/**
 * @brief VideoScaler::getScaledData
 * scale 및 pixel format 변환이 필요한 경우 output data를 가공하고, 그렇지 않으면 원본데이터를 리턴한다.
 * @param srcFrame
 * @param outParam
 * outParam.keepOriginalSize == true 인 경우에는 원본데이터를 리턴한다.
 * @return 디코딩된 비디오 (필요시 스케일링&픽셀포맷 변환)
 */
VideoOutputFrameSharedPtr VideoScaler::getScaledData(AVFrame *srcFrame,
                                                     const VideoOutParam &outParam)
{
    AVPixelFormat sourceFormat = (AVPixelFormat)(srcFrame->format);
    AVPixelFormat outFormat = get_av_supported_pixel_format(sourceFormat, outParam.pixelFormat);

    int  dstWidth = 0;
    int  dstHeight = 0;
    int  scaleOpt = SWS_BILINEAR;
    bool needScale = needToScale(srcFrame, outParam, scaleOpt, dstWidth, dstHeight);
    bool needFormat = sourceFormat != outFormat;

    int outWidth = srcFrame->width;
    int outHeight = srcFrame->height;
    if (needScale) {
        outWidth = dstWidth;
        outHeight = dstHeight;
    }
#if 0
    SPDLOG_INFO("getScaledData():: needScale={}, needFormat={}, outWidth={}, outHeight={}",
                needScale, needFormat, outWidth, outHeight);
#endif
    if (needScale || needFormat) {
        // check swsContext Status
        if (m_swsContext == nullptr ||
                m_swsContextInfo.equal(srcFrame->width, srcFrame->height, sourceFormat,
                                       outWidth, outHeight, outFormat) != true) {
            SPDLOG_DEBUG("Reset Scale context:: srcFormat={}, outFormat={}, src={}x{}, outSize={}x{}, outParam={}x{}, scaleOpt={}",
                         av_make_pix_fmt_stdstring(srcFrame->format),
                         av_make_pix_fmt_stdstring(outFormat),
                         srcFrame->width, srcFrame->height,
                         outWidth, outHeight,
                         outParam.outWidth, outParam.outHeight, scaleOpt);
            release();

            m_swsContext = sws_getContext(srcFrame->width, srcFrame->height, sourceFormat,
                                          outWidth, outHeight, outFormat,
                                          scaleOpt, NULL, NULL, NULL);
            if (m_swsContext == NULL) {
                SPDLOG_WARN("sws_getContext() failed, source={}x{}, sourceFormat={}, out={}x{}, outFormat={}",
                            srcFrame->width, srcFrame->height, sourceFormat,
                            outWidth, outHeight, outFormat);
                return nullptr;
            }
            m_swsContextInfo.reset(srcFrame->width, srcFrame->height, sourceFormat,
                                   outWidth, outHeight, outFormat);

        }
    }

    auto outVideoPtr = std::make_shared<VideoOutputFrame>(outWidth, outHeight, outFormat);
    /*
    SPDLOG_DEBUG("out video info={}, fmt={}",
                 outVideoPtr->dataSize(), av_make_pix_fmt_stdstring((int)outVideoPtr->format()));
*/

    uint8_t *src_data[4] = {NULL};
    uint8_t *dst_data[4] = {NULL};
    int src_linesize[4] = {0};
    int dst_linesize[4] = {0};

    src_data[0] = srcFrame->data[0];
    src_data[1] = srcFrame->data[1];
    src_data[2] = srcFrame->data[2];
    src_data[3] = srcFrame->data[3];
    src_linesize[0] = srcFrame->linesize[0];
    src_linesize[1] = srcFrame->linesize[1];
    src_linesize[2] = srcFrame->linesize[2];
    src_linesize[3] = srcFrame->linesize[3];

    dst_data[0] = outVideoPtr->data(0);
    dst_data[1] = outVideoPtr->data(1);
    dst_data[2] = outVideoPtr->data(2);
    dst_data[3] = outVideoPtr->data(3);
    dst_linesize[0] = outVideoPtr->lineSize(0);
    dst_linesize[1] = outVideoPtr->lineSize(1);
    dst_linesize[2] = outVideoPtr->lineSize(2);
    dst_linesize[3] = outVideoPtr->lineSize(3);

    if (needScale || needFormat) {
        sws_scale(m_swsContext, (const uint8_t * const*)src_data, src_linesize, 0, srcFrame->height,
                  dst_data, dst_linesize);
#if 0
        SPDLOG_DEBUG("Decode And Scale, resolution={}x{}=>{}x{}, fmt={}=>{}, timeStamp={}",
                     srcFrame->width, srcFrame->height, outWidth, outHeight,
                     av_make_pix_fmt_stdstring((int)sourceFormat),
                     av_make_pix_fmt_stdstring((int)outFormat),
                     srcFrame->best_effort_timestamp);
#endif
    }
    else {
        av_image_copy(dst_data, dst_linesize,
                      (const uint8_t **)src_data, src_linesize,
                      outFormat, outWidth, outHeight);
#if 0
        SPDLOG_DEBUG("Decode And NoScale, resolution={}x{}, fmt={}, line0={}, line1={}, line2={}, line=3={}",
                     outWidth, outHeight, av_make_pix_fmt_stdstring((int)outFormat),
                     dst_linesize[0], dst_linesize[1], dst_linesize[2], dst_linesize[3]);
#endif
    }

    return outVideoPtr;
}

VideoOutputFrameSharedPtr VideoScaler::getData(AVFrame *srcFrame, const VideoOutParam &outParam)
{
    AVPixelFormat sourceFormat = (AVPixelFormat)(srcFrame->format);
    AVPixelFormat outFormat = get_av_supported_pixel_format(sourceFormat, outParam.pixelFormat);

    int outWidth = srcFrame->width;
    int outHeight = srcFrame->height;

    auto outVideoPtr = std::make_shared<VideoOutputFrame>(outWidth, outHeight, outFormat);

    uint8_t *src_data[4] = {NULL};
    uint8_t *dst_data[4] = {NULL};
    int src_linesize[4] = {0};
    int dst_linesize[4] = {0};

    src_data[0] = srcFrame->data[0];
    src_data[1] = srcFrame->data[1];
    src_data[2] = srcFrame->data[2];
    src_data[3] = srcFrame->data[3];
    src_linesize[0] = srcFrame->linesize[0];
    src_linesize[1] = srcFrame->linesize[1];
    src_linesize[2] = srcFrame->linesize[2];
    src_linesize[3] = srcFrame->linesize[3];

    dst_data[0] = outVideoPtr->data(0);
    dst_data[1] = outVideoPtr->data(1);
    dst_data[2] = outVideoPtr->data(2);
    dst_data[3] = outVideoPtr->data(3);
    dst_linesize[0] = outVideoPtr->lineSize(0);
    dst_linesize[1] = outVideoPtr->lineSize(1);
    dst_linesize[2] = outVideoPtr->lineSize(2);
    dst_linesize[3] = outVideoPtr->lineSize(3);

    av_image_copy(dst_data, dst_linesize, (const uint8_t **)src_data, src_linesize, outFormat, outWidth, outHeight);
#if 0
    SPDLOG_DEBUG("Decode And NoScale, resolution={}x{}, fmt={}, line0={}, line1={}, line2={}, line=3={}",
                 outWidth, outHeight, av_make_pix_fmt_stdstring((int)outFormat),
                 dst_linesize[0], dst_linesize[1], dst_linesize[2], dst_linesize[3]);
#endif

    return outVideoPtr;
}

void VideoScaler::release()
{
    if (m_swsContext != nullptr) {
        sws_freeContext(m_swsContext);
        m_swsContext = nullptr;
    }
    m_swsContextInfo.reset();
}
