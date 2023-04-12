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

#include "LogSettings.h"
#include "RtpAudioSource.hpp"


namespace Wisenet
{
namespace Rtsp
{

RtpAudioSource::RtpAudioSource()
{
    m_checkExtensionHeader = false;
    m_audioCodec = Media::AudioCodecType::UNKNOWN;
}

RtpAudioSource::~RtpAudioSource()
{
}

void RtpAudioSource::Setup(MediaStreamInfo &streamInfo)
{
	RtpSource::Setup(streamInfo);

	if (m_clockRate == 0)
	{
        SPDLOG_WARN("[RTP-A][{}] Clockrate is 0, assume 8000, codecName={}", m_url, m_codecName);
		m_clockRate = 8000;
	}
	m_sampleRate = m_clockRate;
    m_channels = streamInfo.NumChannels;

	if (streamInfo.CodecName == "PCMU")
	{
        m_audioCodec = Media::AudioCodecType::G711U;
		m_bitRate = 64000;
		m_bitPerSample = 8;
	}
	else if (streamInfo.CodecName == "G726-16")
	{
        m_audioCodec = Media::AudioCodecType::G726;
		m_bitRate = 16000;
		m_bitPerSample = m_bitRate / m_sampleRate;
	}
	else if (streamInfo.CodecName == "G726-24")
	{
        m_audioCodec = Media::AudioCodecType::G726;
		m_bitRate = 24000;
		m_bitPerSample = m_bitRate / m_sampleRate;
	}
	else if (streamInfo.CodecName == "G726-32")
	{
        m_audioCodec = Media::AudioCodecType::G726;
		m_bitRate = 32000;
		m_bitPerSample = m_bitRate / m_sampleRate;
	}
	else if (streamInfo.CodecName == "G726-40")
	{
        m_audioCodec = Media::AudioCodecType::G726;
		m_bitRate = 40000;
		m_bitPerSample = m_bitRate / m_sampleRate;
    }
	else if (streamInfo.CodecName == "MPEG4-GENERIC")
	{
        m_audioCodec = Media::AudioCodecType::AAC;
	}
		
}

void RtpAudioSource::CallbackData()
{
    if (m_mediaCallbacks)
	{
        Media::AudioSourceFrameSharedPtr audioFrame = std::make_shared<Media::AudioSourceFrame>(m_mediaSourceDataPtr->Ptr(), m_mediaSourceDataPtr->FrameSize());
        audioFrame->sourceType = Media::MediaSourceType::RTP;

        audioFrame->frameTime.ptsTimestampMsec = m_currentTimeVal;
        audioFrame->frameSequence = m_currentFrameSequence;

        audioFrame->audioChannels = m_channels;
        audioFrame->audioCodecType = m_audioCodec;
        audioFrame->audioSampleRate = m_sampleRate;
        audioFrame->audioBitPerSample = m_bitPerSample;
        audioFrame->audioBitrate = m_bitRate;

        m_mediaCallbacks(audioFrame);
	}
}


}
}
