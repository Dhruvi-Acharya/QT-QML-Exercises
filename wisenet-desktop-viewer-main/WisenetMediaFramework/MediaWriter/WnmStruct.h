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
#include <QObject>
#include "Media/MediaSourceFrame.h"
#include <QCoreApplication>

static const int FILE_OFFSET_INDEX_LENGTH = 324;
static const int MAX_PASSWORD_LEN = 33;

struct WnmInitalizeParameter
{
    // Info
    QString deviceName = "";
    QString channelName = "";
    QString deviceModel = "";
    QString channelModel = "";

    // Timezone
    qint32 timezoneBias = 0;
    qint32 daylightBias = 0;
    qint64 dstStartTime = 0;
    qint64 dstEndTime = 0;

    // Version
    qint32 majorVersion = 0;
    qint32 minorVersion = 0;

    // Encryption
    bool usePassword = false;
    QString password = "";

    // Digital Signing
    bool useDigitalSignature = true;
    QString pfxFilePath = QCoreApplication::applicationDirPath() + "/Cert/WisenetViewerCert.pfx";
    QString pfxPassword = "D5A3DYebct46sABDCna03ZM4EDN9rT";
};

enum class WnmElementType
{

    None = 0,
    Infos,
    Frames,
    Metadatas,
    IFrameIndex,
    Timelines,
    Encryption,
    DigitalSigning,
    Tags,
};

struct WnmOffsetEntry
{
    qint64 startOffset = 0;
    qint64 nextElementOffset = 0;
};

struct IFrameIndex
{
    qint64 pts = 0;
    qint64 pos = 0;
};

struct Timeline
{
    qint64 startTime = 0;
    qint64 endTime = 0;
};

struct WnmFramePacket
{
    // common
    Wisenet::Media::MediaType frameType = Wisenet::Media::MediaType::UNKNOWN;
    qint64 pts = 0;
    qint64 dts = 0;
    int pesLength = 0;
    int originalDataLength = 0;

    // video
    Wisenet::Media::VideoFrameType videoFrameType = Wisenet::Media::VideoFrameType::UNKNOWN;
    Wisenet::Media::VideoCodecType videoCodecType = Wisenet::Media::VideoCodecType::UNKNOWN;
    int frameWidth;
    int frameHeight;
    bool immediate = false;

    // auduio
    Wisenet::Media::AudioCodecType audioCodecType;
    char channels;
    int sampleRate;
    int bitPerSample;
    int bitRate;

    char * dataPtr = nullptr;
};
