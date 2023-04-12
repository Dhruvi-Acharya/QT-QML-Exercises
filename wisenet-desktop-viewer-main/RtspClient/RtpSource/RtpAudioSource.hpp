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

#ifndef OSCAR_RTP_AUDIO_SOURCE_HPP
#define OSCAR_RTP_AUDIO_SOURCE_HPP

#pragma once

#include "RtpSource.h"


namespace Wisenet
{
namespace Rtsp
{


class RtpAudioSource : public RtpSource
{
public:
	RtpAudioSource();
	virtual ~RtpAudioSource();

	virtual void Setup(MediaStreamInfo &streamInfo);

protected:
    Media::AudioCodecType	m_audioCodec;
    int m_channels = 1;
    int m_sampleRate = 0;		//	Samples per seconds (Hz: 16000, 32000, 44100...)
    int m_bitPerSample = 0;		//	Bit per Sample (bits)
    unsigned int m_bitRate = 0;			//  Bitrate (bits/s)

private:
	void CallbackData();
};


}
}

#endif
