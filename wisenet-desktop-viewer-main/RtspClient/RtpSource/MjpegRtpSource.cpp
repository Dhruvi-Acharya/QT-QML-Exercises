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

#include <fstream>
#include <boost/asio/detail/socket_ops.hpp>

#include "LogSettings.h"
#include "MjpegRtpSource.hpp"


/*
IETF RFC 2435, RFC2435 - RTP Payload Format for JPEG-compressed Video
<http://www.ietf.org/rfc/rfc2435.txt>
*/


namespace Wisenet
{
namespace Rtsp
{

enum {
    MARKER_SOF0 = 0xc0,		// start-of-frame, baseline scan
    MARKER_SOI = 0xd8,		// start of image
    MARKER_EOI = 0xd9,		// end of image
    MARKER_SOS = 0xda,		// start of scan
    MARKER_DRI = 0xdd,		// restart interval
    MARKER_DQT = 0xdb,		// define quantization tables
    MARKER_DHT = 0xc4,		// huffman tables
    MARKER_APP_FIRST = 0xe0,
    MARKER_APP_LAST = 0xef,
    MARKER_COMMENT = 0xfe,
};

static unsigned char const lum_dc_codelens[] = {
    0, 1, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
};

static unsigned char const lum_dc_symbols[] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
};

static unsigned char const lum_ac_codelens[] = {
    0, 2, 1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1, 0x7d,
};

static unsigned char const lum_ac_symbols[] = {
    0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12,
    0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07,
    0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08,
    0x23, 0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0,
    0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16,
    0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28,
    0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
    0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
    0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
    0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
    0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
    0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
    0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98,
    0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
    0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6,
    0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5,
    0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4,
    0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2,
    0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea,
    0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
    0xf9, 0xfa,
};

static unsigned char const chm_dc_codelens[] = {
    0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
};

static unsigned char const chm_dc_symbols[] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
};

static unsigned char const chm_ac_codelens[] = {
    0, 2, 1, 2, 4, 4, 3, 4, 7, 5, 4, 4, 0, 1, 2, 0x77,
};

static unsigned char const chm_ac_symbols[] = {
    0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21,
    0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71,
    0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91,
    0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33, 0x52, 0xf0,
    0x15, 0x62, 0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34,
    0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26,
    0x27, 0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38,
    0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
    0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
    0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
    0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
    0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96,
    0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5,
    0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4,
    0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3,
    0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2,
    0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda,
    0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9,
    0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
    0xf9, 0xfa,
};


static void createHuffmanHeader(unsigned char*& p,
    unsigned char const* codelens,
    int ncodes,
    unsigned char const* symbols,
    int nsymbols,
    int tableNo, int tableClass)
{
    *p++ = 0xff; *p++ = MARKER_DHT;
    *p++ = 0;               /* length msb */
    *p++ = 3 + ncodes + nsymbols; /* length lsb */
    *p++ = (tableClass << 4) | tableNo;
    std::memcpy(p, codelens, ncodes);
    p += ncodes;
    std::memcpy(p, symbols, nsymbols);
    p += nsymbols;
}

static unsigned computeJPEGHeaderSize(unsigned int qtlen, unsigned int dri) {
    unsigned qtlen_half = qtlen / 2; // in case qtlen is odd; shouldn't happen
    qtlen = qtlen_half * 2;

    unsigned numQtables = qtlen > 64 ? 2 : 1;
    return 485 + numQtables * 5 + qtlen + (dri > 0 ? 6 : 0);
}

static void createJPEGHeader(unsigned char* buf, unsigned type,
    unsigned w, unsigned h,
    unsigned char const* qtables, unsigned qtlen,
    unsigned dri,
    bool useSofMarker, unsigned char* sofMarker,
    bool useSosSegment, unsigned char* sosSegment)
{
    unsigned char *ptr = buf;
    unsigned numQtables = qtlen > 64 ? 2 : 1;

    // MARKER_SOI:
    *ptr++ = 0xFF; *ptr++ = MARKER_SOI;

    // MARKER_APP_FIRST:
    *ptr++ = 0xFF; *ptr++ = MARKER_APP_FIRST;
    *ptr++ = 0x00; *ptr++ = 0x10; // size of chunk
    *ptr++ = 'J'; *ptr++ = 'F'; *ptr++ = 'I'; *ptr++ = 'F'; *ptr++ = 0x00;
    *ptr++ = 0x01; *ptr++ = 0x01; // JFIF format version (1.1)
    *ptr++ = 0x00; // no units
    *ptr++ = 0x00; *ptr++ = 0x01; // Horizontal pixel aspect ratio
    *ptr++ = 0x00; *ptr++ = 0x01; // Vertical pixel aspect ratio
    *ptr++ = 0x00; *ptr++ = 0x00; // no thumbnail

    // MARKER_DRI:
    if (dri > 0) {
        *ptr++ = 0xFF; *ptr++ = MARKER_DRI;
        *ptr++ = 0x00; *ptr++ = 0x04; // size of chunk
        *ptr++ = (unsigned char)(dri >> 8); *ptr++ = (unsigned char)(dri); // restart interval
    }

    // MARKER_DQT (luma):
    unsigned tableSize = numQtables == 1 ? qtlen : qtlen / 2;
    *ptr++ = 0xFF; *ptr++ = MARKER_DQT;
    *ptr++ = 0x00; *ptr++ = tableSize + 3; // size of chunk
    *ptr++ = 0x00; // precision(0), table id(0)
    memcpy(ptr, qtables, tableSize);
    qtables += tableSize;
    ptr += tableSize;

    if (numQtables > 1) {
        unsigned tableSize = qtlen - qtlen / 2;
        // MARKER_DQT (chroma):
        *ptr++ = 0xFF; *ptr++ = MARKER_DQT;
        *ptr++ = 0x00; *ptr++ = tableSize + 3; // size of chunk
        *ptr++ = 0x01; // precision(0), table id(1)
        memcpy(ptr, qtables, tableSize);
        qtables += tableSize;
        ptr += tableSize;
    }

    // MARKER_SOF0:
    if (useSofMarker)
    {
        std::memcpy(ptr, sofMarker, 19);
        ptr += 19;
    }
    else
    {
        *ptr++ = 0xFF; *ptr++ = MARKER_SOF0;
        *ptr++ = 0x00; *ptr++ = 0x11; // size of chunk
        *ptr++ = 0x08; // sample precision
        *ptr++ = (unsigned char)(h >> 8);
        *ptr++ = (unsigned char)(h); // number of lines (must be a multiple of 8)
        *ptr++ = (unsigned char)(w >> 8);
        *ptr++ = (unsigned char)(w); // number of columns (must be a multiple of 8)
        *ptr++ = 0x03; // number of components
        *ptr++ = 0x01; // id of component
        *ptr++ = type ? 0x22 : 0x21; // sampling ratio (h,v)
        *ptr++ = 0x00; // quant table id
        *ptr++ = 0x02; // id of component
        *ptr++ = 0x11; // sampling ratio (h,v)
        *ptr++ = numQtables == 1 ? 0x00 : 0x01; // quant table id
        *ptr++ = 0x03; // id of component
        *ptr++ = 0x11; // sampling ratio (h,v)
        *ptr++ = numQtables == 1 ? 0x00 : 0x01; // quant table id
    }


    createHuffmanHeader(ptr, lum_dc_codelens, sizeof lum_dc_codelens,
        lum_dc_symbols, sizeof lum_dc_symbols, 0, 0);
    createHuffmanHeader(ptr, lum_ac_codelens, sizeof lum_ac_codelens,
        lum_ac_symbols, sizeof lum_ac_symbols, 0, 1);
    createHuffmanHeader(ptr, chm_dc_codelens, sizeof chm_dc_codelens,
        chm_dc_symbols, sizeof chm_dc_symbols, 1, 0);
    createHuffmanHeader(ptr, chm_ac_codelens, sizeof chm_ac_codelens,
        chm_ac_symbols, sizeof chm_ac_symbols, 1, 1);

    // MARKER_SOS:
    if (useSosSegment)
    {
        std::memcpy(ptr, sosSegment, 12);
    }
    else
    {
        *ptr++ = 0xFF;  *ptr++ = MARKER_SOS;
        *ptr++ = 0x00; *ptr++ = 0x0C; // size of chunk
        *ptr++ = 0x03; // number of components
        *ptr++ = 0x01; // id of component
        *ptr++ = 0x00; // huffman table id (DC, AC)
        *ptr++ = 0x02; // id of component
        *ptr++ = 0x11; // huffman table id (DC, AC)
        *ptr++ = 0x03; // id of component
        *ptr++ = 0x11; // huffman table id (DC, AC)
        *ptr++ = 0x00; // start of spectral
        *ptr++ = 0x3F; // end of spectral
        *ptr++ = 0x00; // successive approximation bit position (high, low)
    }

}

// The default 'luma' and 'chroma' quantizer tables, in zigzag order:
static unsigned char const defaultQuantizers[128] = {
    // luma table:
    16, 11, 12, 14, 12, 10, 16, 14,
    13, 14, 18, 17, 16, 19, 24, 40,
    26, 24, 22, 22, 24, 49, 35, 37,
    29, 40, 58, 51, 61, 60, 57, 51,
    56, 55, 64, 72, 92, 78, 64, 68,
    87, 69, 55, 56, 80, 109, 81, 87,
    95, 98, 103, 104, 103, 62, 77, 113,
    121, 112, 100, 120, 92, 101, 103, 99,
    // chroma table:
    17, 18, 18, 24, 21, 24, 47, 26,
    26, 47, 99, 66, 56, 66, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99
};

static void makeDefaultQtables(unsigned char* resultTables, unsigned int Q)
{
    int factor = Q;
    int q;

    if (Q < 1) factor = 1;
    else if (Q > 99) factor = 99;

    if (Q < 50) {
        q = 5000 / factor;
    }
    else {
        q = 200 - factor * 2;
    }

    for (int i = 0; i < 128; ++i) {
        int newVal = (defaultQuantizers[i] * q + 50) / 100;
        if (newVal < 1) newVal = 1;
        else if (newVal > 255) newVal = 255;
        resultTables[i] = newVal;
    }
}

MjpegRtpSource::MjpegRtpSource()
    : m_useSofMarker(false)
    , m_useSosSegment(false)
    , m_miObjectID(0)
{
    std::memset(m_sofMarker, 0x00, kSofMarkerHeaderSize);
    std::memset(m_sosSegment, 0x00, kSosSegmentHeaderSize);
    std::memset(m_jpegHeader, 0x00, kMaxJpegHeaderSize);
}


void MjpegRtpSource::Setup(MediaStreamInfo &streamInfo)
{
    RtpVideoSource::Setup(streamInfo);

    m_videoCodec = Media::VideoCodecType::MJPEG;
    m_currentVideoFrameType = Media::VideoFrameType::I_FRAME;
}

MjpegRtpSource::~MjpegRtpSource()
{

}

bool MjpegRtpSource::ProcessSpecialHeader(unsigned char *dataPtr, size_t rtpDataSize, size_t &retSkipBytes)
{

    if (rtpDataSize < 8)
    {
        SPDLOG_DEBUG("[MJPEG][{}] Invalid rtp data size={}", m_url, rtpDataSize);
        return false;
    }

    /*
    JPEG header

    Each packet contains a special JPEG header which immediately follows
    the RTP header.  The first 8 bytes of this header, called the "main
    JPEG header", are as follows:

    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    | Type-specific |              Fragment Offset                  |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |      Type     |       Q       |     Width     |     Height    |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    */


    unsigned char *ptr = dataPtr;
    size_t resultSpecialHeaderSize = 8;
    unsigned int dri = 0;
    unsigned char* qtables = NULL;
    unsigned qtlen = 0;

    unsigned int Offset = (unsigned int)((unsigned int)ptr[1] << 16 | (unsigned int)ptr[2] << 8 | (unsigned int)ptr[3]);
    unsigned int Type = (unsigned int)ptr[4];
    unsigned int type = Type & 1;
    unsigned int Q = (unsigned int)ptr[5];

    m_currentPacketBeginFrame = (Offset == 0);

    if (m_currentPacketBeginFrame)
    {
        m_currentVideoWidth = (int)ptr[6] * 8;
        m_currentVideoHeight = (int)ptr[7] * 8;
        if (m_currentVideoWidth == 0) m_currentVideoWidth = 256 * 8;
        if (m_currentVideoHeight == 0) m_currentVideoHeight = 256 * 8;
    }

    if (m_currentExtensionSize > 0)
    {
        unsigned char *extPtr = m_currentPacketPtr + m_currentExtensionOffset;
        uint32_t rtpExtensionHeader = boost::asio::detail::socket_ops::network_to_host_long(*(uint32_t*)(extPtr));
        uint16_t profile = (rtpExtensionHeader >> 16);

        //bug. 항상 0임. (hh3.kim. 2022-01-13)
        /*uint16_t firstExtLength = (rtpExtensionHeader >> 32);*/

        // JPEG EXTENSION HEADER
        // BestShot 스트림의 경우 mjpeg, extension header code를 ffdd로 시작
        if (profile == 0xffdd)
        {
            extPtr += 4;
            m_miObjectID = boost::asio::detail::socket_ops::network_to_host_long(*(uint32_t*)(extPtr));;
            /* TODO : 2048*2048 이상 해상도로 베스트샷 날아올 경우 추가 구현 필요해요.(그전엔 작게 crop해서 날아옴.
            if (firstExtLength >= 2)
            {
                extPtr += 4;
                profile = 0xffd8;
            }*/
        }
        // mjpeg 해상도가 2048x2048 이상인 경우, extension header에 ffd8로 jpeg 헤더 전달함.
        if (profile == 0xffd8)
        {
            m_useSofMarker = true;
            extPtr += 4;
            std::memcpy(m_sofMarker, extPtr, kSofMarkerHeaderSize);

            m_currentVideoHeight = (unsigned int)(m_sofMarker[5] << 8) + m_sofMarker[6];
            m_currentVideoWidth = (unsigned int)(m_sofMarker[7] << 8) + m_sofMarker[8];

            if (m_currentExtensionSize >= (kSofMarkerHeaderSize + kSosSegmentHeaderSize + 4))
            {
                extPtr += kSofMarkerHeaderSize;
                m_useSosSegment = true;
                std::memcpy(m_sosSegment, extPtr, kSosSegmentHeaderSize);
            }
        }

        // ONVIF/SUNAPI Playback extension header
        else if (profile == 0xabac || profile == 0xabad)
        {
            if (m_currentExtensionSize > kRtpPlaybackJpegExtHeaderSize)
            {
                extPtr += kRtpPlaybackJpegExtHeaderSize;
                rtpExtensionHeader = boost::asio::detail::socket_ops::network_to_host_long(*(uint32_t*)(extPtr));
                uint32_t subProfile = (rtpExtensionHeader >> 16);
                if (subProfile == 0xffd8)
                {
                    uint16_t jpeg_length = boost::asio::detail::socket_ops::network_to_host_short(*(uint16_t*)(extPtr+2))*4;

                    m_useSofMarker = true;
                    extPtr += 4;
                    std::memcpy(m_sofMarker, extPtr, kSofMarkerHeaderSize);

                    m_currentVideoHeight = (unsigned int)(m_sofMarker[5] << 8) + m_sofMarker[6];
                    m_currentVideoWidth = (unsigned int)(m_sofMarker[7] << 8) + m_sofMarker[8];

                    if (m_currentExtensionSize >= (kRtpPlaybackJpegExtHeaderSize + kSofMarkerHeaderSize + kSosSegmentHeaderSize + 4))
                    {
                        extPtr += kSofMarkerHeaderSize;
                        m_useSosSegment = true;
                        std::memcpy(m_sosSegment, extPtr, kSosSegmentHeaderSize);
                    }
                }
                //else if (subProfile == 0xffff)
                //{
                    //TODO:: FFFF인 경우는?
                //}
            }
        }
    }

    /*
    Restart Marker header

    This header MUST be present immediately after the main JPEG header
    when using types 64-127.  It provides the additional information
    required to properly decode a data stream containing restart markers.

    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |       Restart Interval        |F|L|       Restart Count       |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    */
    if (Type > 63)
    {
        unsigned int RestartInterval = (unsigned int)((unsigned int)ptr[resultSpecialHeaderSize] << 8 | (unsigned int)ptr[resultSpecialHeaderSize + 1]);
        dri = RestartInterval;
        resultSpecialHeaderSize += 4;
    }

    if (m_currentPacketBeginFrame)
    {
        if (Q > 127)
        {
            // Quantization Table header present
            /*
            0                   1                   2                   3
            0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
            +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            |      MBZ      |   Precision   |             Length            |
            +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            |                    Quantization Table Data                    |
            |                              ...                              |
            +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            */

            unsigned int MBZ = (unsigned int)ptr[resultSpecialHeaderSize];
            if (MBZ == 0)
            {
                unsigned int Length = (unsigned int)((unsigned int)ptr[resultSpecialHeaderSize + 2] << 8 | (unsigned int)ptr[resultSpecialHeaderSize + 3]);
                resultSpecialHeaderSize += 4;

                qtlen = Length;
                qtables = &ptr[resultSpecialHeaderSize];

                resultSpecialHeaderSize += Length;
            }
        }

        // If this is the first (or only) fragment of a JPEG frame, then we need
        // to synthesize a JPEG header, and prepend it to the incoming data.
        unsigned char newQtables[128] = { 0 };
        if (qtlen == 0)
        {
            // A quantization table was not present in the RTP JPEG header,
            // so use the default tables, scaled according to the "Q" factor:
            makeDefaultQtables(newQtables, Q);
            qtables = newQtables;
            qtlen = sizeof newQtables;
        }

        unsigned int hdrlen = computeJPEGHeaderSize(qtlen, dri);
        createJPEGHeader(m_jpegHeader, type,
            m_currentVideoWidth, m_currentVideoHeight, qtables, qtlen, dri,
            m_useSofMarker, m_sofMarker, m_useSosSegment, m_sosSegment);
        m_mediaSourceDataPtr->AddData(m_jpegHeader, hdrlen);
    }

    // The RTP "M" (marker) bit indicates the last fragment of a frame:
    m_currentPacketCompleteFrame = m_markerBit;

    retSkipBytes = resultSpecialHeaderSize;
    return true;


}

//static const char * EOI_MARKER = { 0xFF, MARKER_EOI };

bool MjpegRtpSource::AddData(unsigned char *dataPtr, size_t dataSize, size_t &remainBytes)
{
    remainBytes = 0;
    m_mediaSourceDataPtr->AddData(dataPtr, dataSize);


    if (m_currentPacketCompleteFrame)
    {
        // Normally, the enclosed frame size is just "dataSize".  If, however,
        // the frame does not end with the "EOI" marker, then add this now:
        if (!(dataPtr[dataSize - 2] == 0xFF && dataPtr[dataSize - 1] == MARKER_EOI))
        {

        }

#if 0
        std::string fileName = std::string("D:\\vtest\\MJPEG") +
            std::to_string(m_mediaSourceDataPtr->FrameSize()) +
            std::string(".jpg");

        std::ofstream outfile(fileName, std::ofstream::binary | std::ofstream::app);
        outfile.write((char*)m_mediaSourceDataPtr->Ptr(), m_mediaSourceDataPtr->FrameSize());
        OLOG_TRACE << "==MJPEG VIDEO FRAME:: size=" << m_mediaSourceDataPtr->FrameSize();
#endif

        return true;
    }
    return false;
}


}
}
