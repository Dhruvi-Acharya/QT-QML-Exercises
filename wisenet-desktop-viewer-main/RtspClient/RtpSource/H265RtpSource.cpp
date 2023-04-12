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
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include "LogSettings.h"
#include "H265RtpSource.hpp"


/*
IETF RFC 7798, RTP Payload Format for High Efficiency Video Coding (HEVC)
<http://www.ietf.org/rfc/rfc7798.txt>
*/

namespace Wisenet
{
namespace Rtsp
{

bool is_h265_sps(const unsigned char nal_unit)
{
    unsigned char nal_type = (nal_unit & 0x7E) >> 1;
    return (nal_type == 33);
}


bool is_h265_pps(const unsigned char nal_unit)
{
    unsigned char nal_type = (nal_unit & 0x7E) >> 1;
    return (nal_type == 35);
}

unsigned char const h265_start_code[4] = { 0x00, 0x00, 0x00, 0x01 };

H265RtpSource::H265RtpSource()
    : m_previousNALUnitDON(0)
    , m_currentNALUnitAbsDon((uint64_t)(~0))
    , m_curPacketNALUnitType(0)
    , m_expectDONField(false)
{

}


void H265RtpSource::Setup(MediaStreamInfo &streamInfo)
{
    RtpVideoSource::Setup(streamInfo);

    m_videoCodec = Media::VideoCodecType::H265;
    m_formatParams = streamInfo.FormatParams;

    for(auto &param : m_formatParams)
    {
        if (param.first == "prop-depack-buf-nalus")
        {
            unsigned int value = 0;
            boost::trim(param.second);
            boost::conversion::try_lexical_convert<unsigned int>(param.second, value);
            if (value > 0)
                m_expectDONField = true;
        }
        else if (param.first == "sprop-max-don-diff")
        {
            unsigned int value = 0;
            boost::trim(param.second);
            boost::conversion::try_lexical_convert<unsigned int>(param.second, value);
            if (value > 0)
                m_expectDONField = true;
        }
        else if (param.first == "sprop-vps")
        {
            m_vps = SecurityUtil::Base64Decoding(param.second);
        }
        else if (param.first == "sprop-sps")
        {
            m_sps = SecurityUtil::Base64Decoding(param.second);
        }
        else if (param.first == "sprop-pps")
        {
            m_pps = SecurityUtil::Base64Decoding(param.second);
        }
    }

#if 1 /* 해당 정보를 최초에 무조건 한번 올려주도록 한다. 우리 장비는 필요없지만...*/
    if (m_vps.size() > 0)
    {
        m_mediaSourceDataPtr->AddData(h265_start_code, 4);
        m_mediaSourceDataPtr->AddData((const unsigned char*)m_vps.c_str(), m_vps.size());
    }
    if (m_sps.size() > 0)
    {
        m_mediaSourceDataPtr->AddData(h265_start_code, 4);
        m_mediaSourceDataPtr->AddData((const unsigned char*)m_sps.c_str(), m_sps.size());
    }
    if (m_pps.size() > 0)
    {
        m_mediaSourceDataPtr->AddData(h265_start_code, 4);
        m_mediaSourceDataPtr->AddData((const unsigned char*)m_pps.c_str(), m_pps.size());
    }
#endif
}

H265RtpSource::~H265RtpSource()
{

}

bool H265RtpSource::ProcessSpecialHeader(unsigned char *dataPtr, size_t rtpDataSize, size_t &retSkipBytes)
{
    if (rtpDataSize < 2)
    {
        SPDLOG_DEBUG("[H265][{}] Invalid rtp data size={}", m_url, rtpDataSize);
        return false;
    }

    unsigned char *ptr = dataPtr;
    size_t copyBytes = rtpDataSize;

    // Check the 'nal_unit_type' for special 'aggregation' or 'fragmentation' packets:
    m_curPacketNALUnitType = (ptr[0] & 0x7E) >> 1;
    unsigned int skipBytes = 0;
    uint16_t DONL = 0;

    //PLOG_DEBUG << "ProcessSpecialHeader m_curPacketNALUnitType=" << (unsigned int)m_curPacketNALUnitType;

    // Aggregation Packet (AP)
    if (m_curPacketNALUnitType == 48)
    {
        if (rtpDataSize < 4)
        {
            SPDLOG_DEBUG("[H265][{}] Invalid rtp data size={}", m_url, rtpDataSize);
            return false;
        }

        // We skip over the 2-byte Payload Header, and the DONL header (if any).
        if (m_expectDONField)
        {
            DONL = (ptr[2] << 8) | ptr[3];
            skipBytes = 4;
        }
        else
        {
            skipBytes = 2;
        }
    }
    // Fragmentation Unit (FU)
    else if (m_curPacketNALUnitType == 49)
    {
        if (rtpDataSize < 3)
        {
            SPDLOG_DEBUG("[H265][{}] Invalid rtp data size={}", m_url, rtpDataSize);
            return false;
        }

        // This NALU begins with the 2-byte Payload Header, the 1-byte FU header, and (optionally)
        // the 2-byte DONL header.
        // If the start bit is set, we reconstruct the original NAL header at the end of these
        // 3 (or 5) bytes, and skip over the first 1 (or 3) bytes.

        // For these NALUs, the first two bytes are the FU indicator and the FU header.
        // If the start bit is set, we reconstruct the original NAL header into byte 1:
        unsigned char startBit = ptr[2] & 0x80; // from the FU header
        unsigned char endBit = ptr[2] & 0x40; // from the FU header
        if (startBit)
        {
            m_currentPacketBeginFrame = true;

            unsigned char nal_unit_type = ptr[2] & 0x3F; // the last 6 bits of the FU header
            unsigned char newNALHeader[2];
            newNALHeader[0] = (ptr[0] & 0x81) | (nal_unit_type << 1);
            newNALHeader[1] = ptr[1];

            if (rtpDataSize < 5)
            {
                SPDLOG_DEBUG("[H265][{}] Invalid rtp data size={}", m_url, rtpDataSize);
                return false;
            }

            if (m_expectDONField)
            {
                DONL = (ptr[3] << 8) | ptr[4];
                ptr[3] = newNALHeader[0];
                ptr[4] = newNALHeader[1];
                skipBytes = 3;
            }
            else {
                ptr[1] = newNALHeader[0];
                ptr[2] = newNALHeader[1];
                skipBytes = 1;
            }
        }
        else
        {
            // The start bit is not set, so we skip over all headers:
            m_currentPacketBeginFrame = false;
            if (m_expectDONField)
            {
                if (rtpDataSize < 5)
                {
                    SPDLOG_DEBUG("[H265][{}] Invalid rtp data size={}", m_url, rtpDataSize);
                    return false;
                }

                DONL = (ptr[3] << 8) | ptr[4];
                skipBytes = 5;
            }
            else
            {
                skipBytes = 3;
            }
        }
        m_currentPacketCompleteFrame = (endBit != 0);
    }

    // This packet contains one complete NAL unit:
    else
    {
        m_currentPacketBeginFrame = m_currentPacketCompleteFrame = true;
        skipBytes = 0;
    }

    computeAbsDonFromDON(DONL);
    retSkipBytes = skipBytes;
    return true;

}


bool is_access_unit_boundary_nal_h265(const unsigned char nal_unit)
{
    unsigned char nal_type = nal_unit;// &0x1F;
                                      // Check if this packet marks access unit boundary by checking the packet type.
    if (nal_type == 32 ||  // VPS
        nal_type == 33 ||  // Sequence parameter set
        nal_type == 34   // Picture parameter set
        ) {  // Reserved types
        return true;
    }
    return false;
}


bool H265RtpSource::AddData(unsigned char *dataPtr, size_t dataSize, size_t &remainBytes)
{
    //OLOG_DEBUG << "==H265 VIDEO FRAME:: RTP size=" << dataSize << " // BEGIN="<< m_currentPacketBeginFrame << "// END=" << m_currentPacketCompleteFrame;

    remainBytes = 0;
    unsigned char *ptr = dataPtr;
    size_t resultNALUSize = 0;
    size_t writeBytes = dataSize;
    size_t packetBytes = dataSize;
    size_t offset = 0;

    // Aggregation Packet (AP)
    if (m_curPacketNALUnitType == 48)
    {
        // We're other than the first NAL unit inside this Aggregation Packet.
        // Update our 'decoding order number':
        if (!m_currentPacketBeginFrame)
        {
            uint16_t DONL = 0;
            if (m_expectDONField)
            {
                uint8_t DOND = ptr[0];
                DONL = m_previousNALUnitDON + (uint16_t)(DOND + 1);
                offset++;
            }
            computeAbsDonFromDON(DONL);
        }

        // The next 2 bytes are the NAL unit size:
        resultNALUSize = (ptr[0] << 8) | ptr[1];
        offset += 2;

        if (offset > 0)
        {
            ptr += offset;
            dataSize -= offset;
            writeBytes -= offset;
        }

        if (resultNALUSize > 0 && resultNALUSize < dataSize)
        {
            writeBytes = resultNALUSize;
        }

        if (dataSize > writeBytes)
        {
            remainBytes = packetBytes - (writeBytes + offset);
        }
    }

    if (m_currentPacketBeginFrame)
    {

        // TODO:: SPS가 아닌 경우에도 I/P 확인 가능해야 한다.
        if (is_h265_sps(ptr[0]))
        {
            GetResolution(ptr, dataSize);
            m_currentVideoFrameType = Media::VideoFrameType::I_FRAME;
        }

        else if (m_mediaSourceDataPtr->FrameSize() == 0)
        {
            m_currentVideoFrameType = Media::VideoFrameType::P_FRAME;
        }

        m_mediaSourceDataPtr->AddData(h265_start_code, 4);
    }

    m_mediaSourceDataPtr->AddData(ptr, dataSize);
    if (m_currentPacketCompleteFrame)
    {
        if (m_currentPacketBeginFrame)
        {
            if (is_access_unit_boundary_nal_h265(m_curPacketNALUnitType))
            {
                return false;
            }
        }

        if (m_markerBit && remainBytes == 0)
        {
#if 0
            char start_code[4] = { 0x00, 0x00, 0x00, 0x01 };
            std::ofstream outfile("D:\\vtest\\h265.dmp", std::ofstream::binary | std::ofstream::app);
            outfile.write(start_code, 4);
            outfile.write((char*)m_mediaSourceDataPtr->Ptr(), m_mediaSourceDataPtr->FrameSize());

#endif
            m_currentPacketBeginFrame = true;
            return true;
        }

    }

    return false;
}



void H265RtpSource::computeAbsDonFromDON(uint16_t DON)
{
    if (!m_expectDONField)
    {
        // Without DON fields in the input stream, we just increment our "AbsDon" count each time:
        ++m_currentNALUnitAbsDon;
    }
    else
    {
        if (m_currentNALUnitAbsDon == (uint64_t)(~0))
        {
            // This is the very first NAL unit, so "AbsDon" is just "DON":
            m_currentNALUnitAbsDon = (uint64_t)DON;
        }
        else
        {
            // Use the previous NAL unit's DON and the current DON to compute "AbsDon":
            //     AbsDon[n] = AbsDon[n-1] + (DON[n] - DON[n-1]) mod 2^16
            short signedDiff16 = (short)(DON - m_previousNALUnitDON);
            int64_t signedDiff64 = (int64_t)signedDiff16;
            m_currentNALUnitAbsDon += signedDiff64;
        }

        m_previousNALUnitDON = DON; // for next time
    }
}



bool H265RtpSource::GetResolution(unsigned char *data, const size_t dataSize)
{
    if (!is_h265_sps(data[0]))
        return false;

    //unsigned char rbsp_byte[1280] = { 0 };
    unsigned char *rbsp_byte = new unsigned char[dataSize + 1];
    rbsp_byte[0] = data[0];
    rbsp_byte[1] = data[1];
    int num_bytes_in_rbsp = 2;

    for (size_t i = 2; i < dataSize; i++)
    {
        if (i + 2 < dataSize &&
            data[i] == 0x00 && data[i + 1] == 0x00 && data[i + 2] == 0x03)
        {
            rbsp_byte[num_bytes_in_rbsp++] = data[i];
            rbsp_byte[num_bytes_in_rbsp++] = data[i + 1];
            i += 2;
        }
        else
        {
            rbsp_byte[num_bytes_in_rbsp++] = data[i];
        }
    }

    m_bitStream.Reset(rbsp_byte, num_bytes_in_rbsp);

    // skip nal header
    m_bitStream.ReadBits(16);

    unsigned vps_id = m_bitStream.ReadBits(4);
    unsigned max_sub_layers_minus1 = m_bitStream.ReadBits(3);
    m_bitStream.ReadBits(1);

    // profile_tier_level()/////////
    m_bitStream.ReadBits(88);
    unsigned general_level_idc = m_bitStream.ReadBits(8);
    unsigned i;
    int sub_layer_profile_present_flag[7], sub_layer_level_present_flag[7];
    for (i = 0; i < max_sub_layers_minus1; ++i)
    {
        sub_layer_profile_present_flag[i] = m_bitStream.ReadBits(1);
        sub_layer_level_present_flag[i] = m_bitStream.ReadBits(1);
    }
    if (max_sub_layers_minus1 > 0)
    {
        m_bitStream.ReadBits(2 * (8 - max_sub_layers_minus1));
    }
    for (i = 0; i < max_sub_layers_minus1; ++i)
    {
        if (sub_layer_profile_present_flag[i])
        {
            m_bitStream.ReadBits(88);
        }
        if (sub_layer_level_present_flag[i])
        {
            m_bitStream.ReadBits(8);
        }
    }
    ////////////////////////////////
    /*
    chroma_format_idc	separate_colour_plane_flag	Chroma format	SubWidthC	SubHeightC
    0					0							Monochrome		1			1
    1					0							4:2:0			2			2
    2					0							4:2:2			2			1
    3					0							4:4:4			1			1
    3					1							4:4:4			1			1
    */
    unsigned subWidthC = 1;
    unsigned subHeightC = 1;


    unsigned sps_set_id = m_bitStream.ReadExponentialGolombCode(); // sps_seq_parameter_set_id
    unsigned  chroma_format_idc = m_bitStream.ReadExponentialGolombCode();
    if (chroma_format_idc == 3)
    {
        m_bitStream.ReadBit();
    }
    else if (chroma_format_idc == 2)
    {
        subWidthC = 2;
        subHeightC = 1;
    }
    else if (chroma_format_idc == 1)
    {
        subWidthC = 2;
        subHeightC = 2;
    }

    long pic_width_in_luma_samples = m_bitStream.ReadExponentialGolombCode();
    long pic_height_in_luma_samples = m_bitStream.ReadExponentialGolombCode();


    int conformance_window_flag = m_bitStream.ReadBit();
    unsigned conf_win_left_offset = 0;
    unsigned conf_win_right_offset = 0;
    unsigned conf_win_top_offset = 0;
    unsigned conf_win_bottom_offset = 0;
    if (conformance_window_flag) {
        conf_win_left_offset = m_bitStream.ReadExponentialGolombCode();
        conf_win_right_offset = m_bitStream.ReadExponentialGolombCode();
        conf_win_top_offset = m_bitStream.ReadExponentialGolombCode();
        conf_win_bottom_offset = m_bitStream.ReadExponentialGolombCode();
    }

    m_currentVideoWidth = pic_width_in_luma_samples - (subWidthC * conf_win_right_offset + subWidthC * conf_win_left_offset);
    m_currentVideoHeight = pic_height_in_luma_samples - (subHeightC * conf_win_bottom_offset + subHeightC * conf_win_top_offset);

    delete[] rbsp_byte;

    return true;
}

}
}
