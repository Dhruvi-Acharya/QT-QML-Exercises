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

#ifndef OSCAR_RTP_METADATA_SOURCE_HPP
#define OSCAR_RTP_METADATA_SOURCE_HPP

#pragma once

#include "RtpSource.h"


namespace Wisenet
{
namespace Rtsp
{


class RtpMetaDataSource : public RtpSource
{
public:
	RtpMetaDataSource();
	virtual ~RtpMetaDataSource();
    virtual void Setup(MediaStreamInfo &streamInfo);

private:
	bool ProcessSpecialHeader(unsigned char *dataPtr, size_t rtpDataSize, size_t &retSkipBytes);
	void CallbackData();
};


}
}

#endif
