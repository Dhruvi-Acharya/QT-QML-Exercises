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

#include <fstream>
#include <boost/algorithm/string.hpp>

#include "LogSettings.h"
#include "H264RtpSource.hpp"

/*
IETF RFC 3984, RTP Payload Format for H.264 Video
<http://www.ietf.org/rfc/rfc3984>
*/

namespace Wisenet
{
namespace Rtsp
{

unsigned char const h264_start_code[4] = { 0x00, 0x00, 0x00, 0x01 };

bool is_access_unit_boundary_nal(const unsigned char nal_unit)
{
    unsigned char nal_type = nal_unit;// &0x1F;
    // Check if this packet marks access unit boundary by checking the packet type.
    if (nal_type == 6 ||  // Supplemental enhancement information
        nal_type == 7 ||  // Sequence parameter set
        nal_type == 8 ||  // Picture parameter set
        nal_type == 9 ||  // Access unit delimiter
        (nal_type >= 14 && nal_type <= 18)) {  // Reserved types
        return true;
    }
    return false;
}

bool is_h264_key_frame(const unsigned char* packet)
{
    int fragment_type = packet[0] & 0x1F;
    int nal_type = packet[1] & 0x1F;
    int start_bit = packet[1] & 0x80;
    int end_bit = packet[1] & 0x40;

    if (((fragment_type == 28 || fragment_type == 29) &&
        nal_type == 5 && start_bit == 128) ||
        (fragment_type == 5))
    {
        return true;
    }

    return false;
}

bool is_h264_sps(const unsigned char nal_unit)
{
    unsigned char nal_type = nal_unit & 0x1F;
    return (nal_type == 7);
}

bool is_h264_pps(const unsigned char nal_unit)
{
    unsigned char nal_type = nal_unit & 0x1F;
    return (nal_type == 8);
}



H264RtpSource::H264RtpSource()
    : m_curPacketNALUnitType(0)
    , m_getSpsPacket(false)
{

}

void H264RtpSource::Setup(MediaStreamInfo &streamInfo)
{
    RtpVideoSource::Setup(streamInfo);

    m_videoCodec = Media::VideoCodecType::H264;
    m_formatParams = streamInfo.FormatParams;

    auto itr = m_formatParams.find("sprop-parameter-sets");
    if (itr != m_formatParams.end())
    {
        std::vector<std::string> tokens;
        boost::split(tokens, itr->second, boost::is_any_of(","));

        if (tokens.size() > 0)
        {
            m_spropParameterSetDataPtr = std::make_shared<MediaFrame>(256);
            for(auto token : tokens)
            {
                std::string b64 = SecurityUtil::Base64Decoding(token);

                unsigned char *ptr = (unsigned char*)b64.c_str();
                size_t ptrSize = b64.size();
                if (is_h264_sps(ptr[0]))
                {
                    GetResolution(ptr, ptrSize);
                }

                m_spropParameterSetDataPtr->AddData(h264_start_code, 4);
                m_spropParameterSetDataPtr->AddData(ptr, ptrSize);
            }
        }
    }
}

H264RtpSource::~H264RtpSource()
{

}


bool H264RtpSource::ProcessSpecialHeader(unsigned char *dataPtr, size_t rtpDataSize, size_t &retSkipBytes)
{
    if (rtpDataSize < 1)
    {
        SPDLOG_DEBUG("[H264][{}] Invalid rtp data size={}", m_url, rtpDataSize);
        return false;
    }

    /*
    NAL Unit  Packet    Packet Type Name               Section
    Type      Type
    -------------------------------------------------------------
    0        reserved                                     -
    1-23     NAL unit  Single NAL unit packet             5.6
    24       STAP-A    Single-time aggregation packet     5.7.1
    25       STAP-B    Single-time aggregation packet     5.7.1
    26       MTAP16    Multi-time aggregation packet      5.7.2
    27       MTAP24    Multi-time aggregation packet      5.7.2
    28       FU-A      Fragmentation unit                 5.8
    29       FU-B      Fragmentation unit                 5.8
    30-31    reserved

    Summary of allowed NAL unit types for each packetization
    mode (yes = allowed, no = disallowed, ig = ignore)

    Payload Packet    Single NAL    Non-Interleaved    Interleaved
    Type    Type      Unit Mode           Mode             Mode
    -------------------------------------------------------------
    0      reserved      ig               ig               ig
    1-23   NAL unit     yes              yes               no
    24     STAP-A        no              yes               no
    25     STAP-B        no               no              yes
    26     MTAP16        no               no              yes
    27     MTAP24        no               no              yes
    28     FU-A          no              yes              yes
    29     FU-B          no               no              yes
    30-31  reserved      ig               ig               ig-
    */
    unsigned char *ptr = dataPtr;
    size_t copyBytes = rtpDataSize;

    // Check the 'nal_unit_type' for special 'aggregation' or 'fragmentation' packets:
    m_curPacketNALUnitType = (ptr[0] & 0x1F);
    unsigned int skipBytes = 0;

    //SPDLOG_DEBUG("[H264][{}] nal unit type={}, size={}", m_url, (int)m_curPacketNALUnitType, rtpDataSize);

    // STAP-A
    if (m_curPacketNALUnitType == 24)
    {
        // discard the type byte
        skipBytes = 1;
    }
    // STAP-B, MTAP16, or MTAP24
    else if (m_curPacketNALUnitType >= 25 && m_curPacketNALUnitType <= 27)
    {
        // discard the type byte, and the initial DON
        skipBytes = 3;
    }
    // FU-A or FU-B
    else if (m_curPacketNALUnitType >= 28 && m_curPacketNALUnitType <= 29)
    {
        if (rtpDataSize < 2)
        {
            SPDLOG_DEBUG("[H264][{}] Invalid rtp data size={}", m_url, rtpDataSize);
            return false;
        }

        // For these NALUs, the first two bytes are the FU indicator and the FU header.
        // If the start bit is set, we reconstruct the original NAL header into byte 1:
        unsigned char startBit = ptr[1] & 0x80;
        unsigned char endBit = ptr[1] & 0x40;
        if (startBit)
        {
            m_currentPacketBeginFrame = true;
            ptr[1] = (ptr[0] & 0xE0) | (ptr[1] & 0x1F);
            skipBytes = 1;
        }
        else
        {
            m_currentPacketBeginFrame = false;
            skipBytes = 2;
        }
        m_currentPacketCompleteFrame = (endBit != 0);
    }
    // This packet contains one complete NAL unit:
    else
    {

        // single nal type이고, sps/pps 데이터인경우에는 merge 처리한다.
        if (m_curPacketNALUnitType == 7 || m_curPacketNALUnitType == 8)
        {
            m_currentPacketBeginFrame = true;
            m_currentPacketCompleteFrame = false;
        }
        // isvcl
        //else if (m_curPacketNALUnitType > 0 && m_curPacketNALUnitType <= 5)
        //{
        //	const unsigned char byteAfterNalUnitHeader = ptr[1];
        //	bool thisNalunitEndsAccessUnit = (byteAfterNalUnitHeader & 0x80) != 0;
        //	if (thisNalunitEndsAccessUnit)
        //	{
        //		m_currentPacketBeginFrame = m_currentPacketCompleteFrame = true;
        //		//PLOG_DEBUG << "****This NAL unit ends the current access unit::" << (byteAfterNalUnitHeader & 0x80);
        //	}
        //	else
        //	{
        //		m_currentPacketBeginFrame = true;
        //		m_currentPacketCompleteFrame = false;
        //		//PLOG_DEBUG << "====The current NAL unit doesn't end one::"  << (byteAfterNalUnitHeader & 0x80);
        //	}
        //}
        else
        {
            m_currentPacketBeginFrame = m_currentPacketCompleteFrame = true;
        }
        skipBytes = 0;
    }
    retSkipBytes = skipBytes;
    return true;
}

bool H264RtpSource::AddData(unsigned char *dataPtr, size_t dataSize, size_t &remainBytes)
{
    remainBytes = 0;
    unsigned char *ptr = dataPtr;
    size_t resultNALUSize = 0;
    size_t offset = 0;
    size_t packetBytes = dataSize;
    size_t writeBytes = dataSize;

    if (dataSize == 0)
    {
        return false;
    }

    // STAP-A or STAP-B
    if (m_curPacketNALUnitType == 24 || m_curPacketNALUnitType == 25)
    {
        // The first two bytes are NALU size:
        resultNALUSize = (ptr[0] << 8) | ptr[1];
        offset = 2;
    }
    // MTAP16
    else if (m_curPacketNALUnitType == 26)
    {
        // The first two bytes are NALU size.  The next three are the DOND and TS offset:
        resultNALUSize = (ptr[0] << 8) | ptr[1];
        offset = 5;
    }
    // MTAP24
    else if (m_curPacketNALUnitType == 27)
    {
        // The first two bytes are NALU size.  The next four are the DOND and TS offset:
        resultNALUSize = (ptr[0] << 8) | ptr[1];
        offset = 6;
    }

    if (offset > 0)
    {
        ptr += offset;
        dataSize -= offset;
        writeBytes -= offset;
    }

    if (resultNALUSize > 0 && resultNALUSize < writeBytes)
    {
        SPDLOG_DEBUG("[H264][{}] resultNALUSize={}, writeBytes={}", m_url, resultNALUSize, writeBytes);
        writeBytes = resultNALUSize;
    }

    if (dataSize > writeBytes)
    {
        remainBytes = packetBytes - (writeBytes + offset);
    }

    if (m_currentPacketBeginFrame)
    {
        if (is_h264_sps(ptr[0]))
        {
            m_getSpsPacket = true;
            GetResolution(ptr, dataSize);

            m_currentVideoFrameType = Media::VideoFrameType::I_FRAME;
        }
        else
        {
            if (is_h264_key_frame(ptr))
            {
                if (!m_getSpsPacket && m_spropParameterSetDataPtr)
                {
                    m_mediaSourceDataPtr->AddData(m_spropParameterSetDataPtr->Ptr(),
                        m_spropParameterSetDataPtr->FrameSize());
                }
                m_currentVideoFrameType = Media::VideoFrameType::I_FRAME;
            }
            else
            {
                m_currentVideoFrameType = Media::VideoFrameType::P_FRAME;
            }
        }

        m_mediaSourceDataPtr->AddData(h264_start_code, 4);
    }

    if (m_mediaSourceDataPtr->FrameSize() == 0)
    {
        m_mediaSourceDataPtr->AddData(h264_start_code, 4);
    }

    m_mediaSourceDataPtr->AddData(ptr, dataSize);
    if (m_currentPacketCompleteFrame)
    {
        if (m_currentPacketBeginFrame)
        {
            //return false;
            // if the device does not use marker bit.
            // single nal unit이고 sps/pps 정보 등이면 false return
            // 그외 frame은 모두 완료된 프레임으로 가정함
            // "Decoders MAY use this bit as an early indication of the last packet of an access unit, but MUST NOT rely on this property."
            // https://tools.ietf.org/html/rfc3984#page-34
            if (is_access_unit_boundary_nal(m_curPacketNALUnitType))
            {
                return false;
            }

            return true;
            //return false;
        }

        if (m_markerBit)
        {
#if 0
            static int count = 0;
            std::string fileName = "D:\\vtest\\h264.dmp." + boost::lexical_cast<std::string>(count);
            std::ofstream outfile(fileName, std::ofstream::binary | std::ofstream::app);
            outfile.write((char*)m_mediaSourceDataPtr->Ptr(), m_mediaSourceDataPtr->FrameSize());
            count++;
#endif
            return true;
        }

    }
    return false;

}


bool H264RtpSource::GetResolution(unsigned char *data, const size_t dataSize)
{
    if (!is_h264_sps(data[0]))
        return false;

    m_bitStream.Reset(data, dataSize);

    int frame_crop_left_offset = 0;
    int frame_crop_right_offset = 0;
    int frame_crop_top_offset = 0;
    int frame_crop_bottom_offset = 0;

    m_bitStream.ReadBits(8);
    int profile_idc = m_bitStream.ReadBits(8);
    m_bitStream.ReadBits(8);

    int level_idc = m_bitStream.ReadBits(8);
    int seq_parameter_set_id = m_bitStream.ReadExponentialGolombCode();


    if (profile_idc == 100 || profile_idc == 110 ||
        profile_idc == 122 || profile_idc == 244 ||
        profile_idc == 44 || profile_idc == 83 ||
        profile_idc == 86 || profile_idc == 118)
    {
        int chroma_format_idc = m_bitStream.ReadExponentialGolombCode();

        if (chroma_format_idc == 3)
        {
            int residual_colour_transform_flag = m_bitStream.ReadBit();
        }
        int bit_depth_luma_minus8 = m_bitStream.ReadExponentialGolombCode();
        int bit_depth_chroma_minus8 = m_bitStream.ReadExponentialGolombCode();
        int qpprime_y_zero_transform_bypass_flag = m_bitStream.ReadBit();
        int seq_scaling_matrix_present_flag = m_bitStream.ReadBit();

        if (seq_scaling_matrix_present_flag)
        {
            int i = 0;
            for (i = 0; i < 8; i++)
            {
                int seq_scaling_list_present_flag = m_bitStream.ReadBit();
                if (seq_scaling_list_present_flag)
                {
                    int sizeOfScalingList = (i < 6) ? 16 : 64;
                    int lastScale = 8;
                    int nextScale = 8;
                    int j = 0;
                    for (j = 0; j < sizeOfScalingList; j++)
                    {
                        if (nextScale != 0)
                        {
                            int delta_scale = m_bitStream.ReadSE();
                            nextScale = (lastScale + delta_scale + 256) % 256;
                        }
                        lastScale = (nextScale == 0) ? lastScale : nextScale;
                    }
                }
            }
        }
    }

    int log2_max_frame_num_minus4 = m_bitStream.ReadExponentialGolombCode();
    int pic_order_cnt_type = m_bitStream.ReadExponentialGolombCode();
    if (pic_order_cnt_type == 0)
    {
        int log2_max_pic_order_cnt_lsb_minus4 = m_bitStream.ReadExponentialGolombCode();
    }
    else if (pic_order_cnt_type == 1)
    {
        int delta_pic_order_always_zero_flag = m_bitStream.ReadBit();
        int offset_for_non_ref_pic = m_bitStream.ReadSE();
        int offset_for_top_to_bottom_field = m_bitStream.ReadSE();
        int num_ref_frames_in_pic_order_cnt_cycle = m_bitStream.ReadExponentialGolombCode();
        int i;
        for (i = 0; i < num_ref_frames_in_pic_order_cnt_cycle; i++)
        {
            m_bitStream.ReadSE();
            //sps->offset_for_ref_frame[ i ] = ReadSE();
        }
    }
    int max_num_ref_frames = m_bitStream.ReadExponentialGolombCode();
    int gaps_in_frame_num_value_allowed_flag = m_bitStream.ReadBit();
    int pic_width_in_mbs_minus1 = m_bitStream.ReadExponentialGolombCode();
    int pic_height_in_map_units_minus1 = m_bitStream.ReadExponentialGolombCode();
    int frame_mbs_only_flag = m_bitStream.ReadBit();
    if (!frame_mbs_only_flag)
    {
        int mb_adaptive_frame_field_flag = m_bitStream.ReadBit();
    }
    int direct_8x8_inference_flag = m_bitStream.ReadBit();
    int frame_cropping_flag = m_bitStream.ReadBit();
    if (frame_cropping_flag)
    {
        frame_crop_left_offset = m_bitStream.ReadExponentialGolombCode();
        frame_crop_right_offset = m_bitStream.ReadExponentialGolombCode();
        frame_crop_top_offset = m_bitStream.ReadExponentialGolombCode();
        frame_crop_bottom_offset = m_bitStream.ReadExponentialGolombCode();
    }
    int vui_parameters_present_flag = m_bitStream.ReadBit();

    m_currentVideoWidth = ((pic_width_in_mbs_minus1 + 1) * 16) - frame_crop_right_offset * 2 - frame_crop_left_offset * 2;
    m_currentVideoHeight = ((2 - frame_mbs_only_flag)* (pic_height_in_map_units_minus1 + 1) * 16) - (frame_crop_bottom_offset * 2) - (frame_crop_top_offset * 2);
    return true;
}

}
}
