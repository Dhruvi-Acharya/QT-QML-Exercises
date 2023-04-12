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

#include "RtpMetaImageSource.hpp"

/*
ONVIF Media Service Specification
<http://www.onvif.org/specs/srv/media/ONVIF-Media-Service-Spec-v220.pdf>
*/

namespace Wisenet
{
namespace Rtsp
{

RtpMetaImageSource::RtpMetaImageSource()
{
}


RtpMetaImageSource::~RtpMetaImageSource()
{
}


void RtpMetaImageSource::CallbackData()
{
    if (m_mediaCallbacks)
    {
        Media::MetadataSourceFrameSharedPtr metaFrame = std::make_shared<Media::MetadataSourceFrame>(m_mediaSourceDataPtr->Ptr(), m_mediaSourceDataPtr->FrameSize());

        metaFrame->sourceType = Media::MediaSourceType::RTP;
        metaFrame->metaDataCodecType = Media::MetaDataCodecType::JPEG;

        metaFrame->frameTime.ptsTimestampMsec = m_currentTimeVal;
        metaFrame->frameSequence = m_currentFrameSequence;
        metaFrame->commandSeq = m_RtpExtensionSeq;

        metaFrame->refID = m_miObjectID;

        /* 필요 없는지 확인
        metaFrame->videoFrameType = m_currentVideoFrameType;
        metaFrame->videoCodecType = m_videoCodec;
        metaFrame->videoWidth = m_currentVideoWidth;
        metaFrame->videoHeight = m_currentVideoHeight;
        metaFrame->frameRate = m_frameRate;
        */

        m_mediaCallbacks(metaFrame);
	}
}


}
}
