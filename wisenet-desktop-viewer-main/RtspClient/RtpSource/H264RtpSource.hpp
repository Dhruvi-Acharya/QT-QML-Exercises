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

#ifndef OSCAR_H264_RTP_SOURCE_HPP
#define OSCAR_H264_RTP_SOURCE_HPP

#include <string>
#include <map>
#include "RtpVideoSource.hpp"
#include "BitStream.hpp"

namespace Wisenet
{
namespace Rtsp
{

class H264RtpSource : public RtpVideoSource
{
public:
    H264RtpSource();
    ~H264RtpSource();

    void Setup(MediaStreamInfo &streamInfo);

private:
    bool ProcessSpecialHeader(unsigned char *dataPtr, size_t rtpDataSize, size_t &retSkipBytes);
    bool AddData(unsigned char *dataPtr, size_t dataSize, size_t &remainBytes);
    bool GetResolution(unsigned char *data, const size_t dataSize);

private:
    std::map<std::string, std::string> m_formatParams;
    unsigned char m_curPacketNALUnitType;

    bool m_getSpsPacket;
    MediaFramePtr m_spropParameterSetDataPtr;
    BitStream m_bitStream;
};

}
}
#endif
