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
#include "RtpMetaDataSource.hpp"

/*
ONVIF Media Service Specification
<http://www.onvif.org/specs/srv/media/ONVIF-Media-Service-Spec-v220.pdf>
*/

namespace Wisenet
{
namespace Rtsp
{

RtpMetaDataSource::RtpMetaDataSource()
{
    m_checkExtensionHeader = false;    
}

RtpMetaDataSource::~RtpMetaDataSource()
{
}

void RtpMetaDataSource::Setup(MediaStreamInfo &streamInfo)
{
	RtpSource::Setup(streamInfo);
	if (m_clockRate == 0)
	{
        SPDLOG_INFO("[RTP-M][{}] Clockrate is 0, assume 90000, codecName={}", m_url, m_codecName);
		m_clockRate = 90000;
	}
}

bool RtpMetaDataSource::ProcessSpecialHeader(
	unsigned char *dataPtr, size_t rtpDataSize,
	size_t &retSkipBytes)
{
	m_currentPacketCompleteFrame = m_markerBit;
	return true;
}

void RtpMetaDataSource::CallbackData()
{
    if (m_mediaCallbacks)
    {
        Media::MetadataSourceFrameSharedPtr metaFrame = std::make_shared<Media::MetadataSourceFrame>(m_mediaSourceDataPtr->Ptr(), m_mediaSourceDataPtr->FrameSize());

        metaFrame->sourceType = Media::MediaSourceType::RTP;
        metaFrame->metaDataCodecType = Media::MetaDataCodecType::XML;

        metaFrame->frameTime.ptsTimestampMsec = m_currentTimeVal;
        metaFrame->frameSequence = m_currentFrameSequence;

        m_mediaCallbacks(metaFrame);        
	}
}

}
}
