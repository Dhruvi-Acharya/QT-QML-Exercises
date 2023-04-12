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

#include <cstddef>

namespace Wisenet
{
namespace Rtsp
{

/* http://stackoverflow.com/questions/12018535/get-the-width-height-of-the-video-from-h-264-nalu */

class BitStream
{
public:
	BitStream();
	~BitStream();
	
	void Reset(unsigned char * streamPtr, const size_t streamSize);

	unsigned int ReadBit();
	unsigned int ReadBits(int n);
	unsigned int ReadExponentialGolombCode();
	unsigned int ReadSE();

private:
	unsigned char *	m_buffer;
	size_t			m_bufferSize;
	size_t			m_nCurrentBit;

};

}
}
