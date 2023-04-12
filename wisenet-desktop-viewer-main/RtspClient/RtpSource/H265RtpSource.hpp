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

#ifndef OSCAR_H265_RTP_SOURCE_HPP
#define OSCAR_H265_RTP_SOURCE_HPP

#pragma once

#include <string>
#include <map>
#include "RtpVideoSource.hpp"
#include "BitStream.hpp"

namespace Wisenet
{
namespace Rtsp
{


class H265RtpSource : public RtpVideoSource
{
public:
	H265RtpSource();
	~H265RtpSource();

	void Setup(MediaStreamInfo &streamInfo);

private:
	bool ProcessSpecialHeader(unsigned char *dataPtr, size_t rtpDataSize, size_t &retSkipBytes);
	bool AddData(unsigned char *dataPtr, size_t dataSize, size_t &remainBytes);

	void computeAbsDonFromDON(uint16_t DON);

	bool GetResolution(unsigned char *data, const size_t dataSize);
private:
	std::map<std::string, std::string> m_formatParams;
	unsigned char m_curPacketNALUnitType;
	bool m_expectDONField;
	std::string m_vps;
	std::string m_sps;
	std::string m_pps;

	uint16_t m_previousNALUnitDON;
	uint64_t m_currentNALUnitAbsDon;

	BitStream	m_bitStream;

};


}
}

#endif
