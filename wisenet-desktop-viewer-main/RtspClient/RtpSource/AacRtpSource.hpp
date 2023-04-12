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

#ifndef OSCAR_AAC_RTP_SOURCE_HPP
#define OSCAR_AAC_RTP_SOURCE_HPP

#pragma once
#include <string>
#include <map>
#include "RtpAudioSource.hpp"

namespace Wisenet
{
namespace Rtsp
{


class AacRtpSource : public RtpAudioSource
{
public:
	AacRtpSource();
	~AacRtpSource();
	void Setup(MediaStreamInfo &streamInfo);

private:
	bool ProcessSpecialHeader(unsigned char *dataPtr, size_t rtpDataSize, size_t &retSkipBytes);

private:
	std::map<std::string, std::string> m_formatParams;
	unsigned m_sizeLength;
	unsigned int m_indexLength;
	unsigned int m_indexDeltaLength;
	std::string m_mode;
};


}
}

#endif
