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

#include "BitStream.hpp"


namespace Wisenet
{
namespace Rtsp
{

BitStream::BitStream()
	: m_buffer(0)
	, m_bufferSize(0)
	, m_nCurrentBit(0)
{
}


BitStream::~BitStream()
{
}

void BitStream::Reset(unsigned char * streamPtr, const size_t streamSize)
{
	m_buffer = streamPtr;
	m_bufferSize = streamSize;
	m_nCurrentBit = 0;
}


unsigned int BitStream::ReadBit()
{
	size_t nIndex = m_nCurrentBit / 8;
	size_t nOffset = m_nCurrentBit % 8 + 1;

	m_nCurrentBit++;
	return (m_buffer[nIndex] >> (8 - nOffset)) & 0x01;
}

unsigned int BitStream::ReadBits(int n)
{
	int r = 0;
	int i;
	for (i = 0; i < n; i++)
	{
		r |= (ReadBit() << (n - i - 1));
	}
	return r;
}

unsigned int BitStream::ReadExponentialGolombCode()
{
	unsigned int r = 0;
	unsigned int i = 0;

	while ((ReadBit() == 0) && (i < 32))
	{
		i++;
	}

	r = ReadBits(i);
	r += (1 << i) - 1;
	return r;
}


unsigned int BitStream::ReadSE()
{
	int r = ReadExponentialGolombCode();
	if (r & 0x01)
	{
		r = (r + 1) / 2;
	}
	else
	{
		r = -(r / 2);
	}
	return r;
}

}
}
