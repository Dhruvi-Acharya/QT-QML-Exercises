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
#include "WnmReader.h"
#include "LogSettings.h"
#include <chrono>


#undef SPDLOG_LOGGER_CALL
#define SPDLOG_LOGGER_CALL(logger, level, ...) (logger)->log(spdlog::source_loc{__FILE__, __LINE__, "[WNM-R] "}, level, __VA_ARGS__)


WnmReader::WnmReader(QObject* parent)
    : QObject(parent)
    , m_sourceFramesQueue(std::make_shared<MediaSourceFrameQueue>())
    , m_isOpenCommand(false)
    , m_isCloseCommand(false)
    , m_isSetSpeedCommand(false)
    , m_isSeekCommand(false)
    , m_isPlayCommand(false)
    , m_isPauseCommand(false)
    , m_isStepCommand(false)
    , m_isSleepCommand(false)
    , m_isCheckPasswordCommand(false)
{

    SPDLOG_DEBUG("WnmReader() THREAD START");
}

WnmReader::~WnmReader()
{
    stop();
    release();
    SPDLOG_DEBUG("~WnmReader() THREAD END");
}

SourceFrameQueueSharedPtr WnmReader::getSourceFrameQueue()
{
    return m_sourceFramesQueue;
}

void WnmReader::open(QString& localResource)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isOpenCommand = true;
    m_localResource = localResource;
}

void WnmReader::close()
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isCloseCommand = true;
}

void WnmReader::setSpeed(const float speed, const qint64 lastPlaybackTimeMsec, const unsigned char commandSeq)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isSetSpeedCommand = true;
    m_newSpeed = speed;
    m_newSeekTimeMsec = lastPlaybackTimeMsec;
    m_newCommandSeq = commandSeq;
}

void WnmReader::seek(const qint64 playbackTimeMsec, const unsigned char commandSeq)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isSeekCommand = true;
    m_newCommandSeq = commandSeq;
    m_newSeekTimeMsec = playbackTimeMsec;
}

void WnmReader::play(const qint64 lastPlaybackTimeMsec, const float lastSpeed, const unsigned char commandSeq)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isPlayCommand = true;
    m_newCommandSeq = commandSeq;
    m_newSeekTimeMsec = lastPlaybackTimeMsec;
    m_newSpeed = lastSpeed;
}

void WnmReader::pause(const unsigned char commandSeq)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isPauseCommand = true;
    m_newCommandSeq = commandSeq;
}

void WnmReader::step(const bool isForward, const qint64 lastPlaybackTimeMsec, const unsigned char commandSeq)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isStepCommand = true;
    // backward step 인 경우에는 playtime을 약간 조정해준다.
    // forward step인 경우에는 파라미터값을 사용하지 않느다.
    m_newSeekTimeMsec = isForward ? lastPlaybackTimeMsec : lastPlaybackTimeMsec-5;
    m_newStepForwardStatus = isForward;
    m_newCommandSeq = commandSeq;
}

void WnmReader::sleep(const bool isOn)
{
    SPDLOG_DEBUG("sleep(), isOn={}", isOn);
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isSleepCommand = true;
    m_newSleepStatus = isOn;
}

void WnmReader::checkPassword(QString& password)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isCheckPasswordCommand = true;
    m_tempPassword = password;
}

QMap<qint64, qint64> WnmReader::getTimeline()
{
    QMap<qint64, qint64> timeline;

    for(auto& block : m_timelineList)
    {
        timeline.insert(block.startTime, block.endTime);
    }

    return timeline;
}

QString WnmReader::getChannelName()
{
    return m_wnmParameter.channelName;
}

void WnmReader::_open()
{
    release();

    // 확장자 체크
    int extensionIndex = m_localResource.lastIndexOf(".");
    if(extensionIndex != -1 && extensionIndex != m_localResource.length() - 1)
    {
        QString extension = m_localResource.right(m_localResource.length() - extensionIndex - 1);
        if(extension.toLower() != "wnm")
        {
            SPDLOG_ERROR("WnmReader::_open() File extension error. fileName={}", m_localResource.toLocal8Bit().toStdString());
            emit openResult(MediaControlBase::Error::FormatError);
            return;
        }
    }

    // File open
    m_file.setFileName(m_localResource);
    if(m_file.open(QIODevice::ReadWrite) == false)
    {
        SPDLOG_ERROR("WnmReader::_open() File open error. fileName={}", m_localResource.toLocal8Bit().toStdString());
        emit openResult(MediaControlBase::Error::ResourceError);
        return;
    }
    m_dataStream.setDevice(&m_file);

    // Load FileOffsetIndex
    qint32 offsetEntryCount = 0;
    m_dataStream >> offsetEntryCount;
    for(int i=0 ; i<offsetEntryCount ; i++)
    {
        WnmElementType elementType = WnmElementType::None;
        m_dataStream >> elementType;
        m_dataStream >> m_fileOffsetIndex[elementType].startOffset;
        m_dataStream >> m_fileOffsetIndex[elementType].nextElementOffset;
    }

    loadInfos();            // Load Infos
    loadIFrameIndex();      // Load I-Frame Index
    loadTimelines();        // Load Timelines
    loadEncryption();       // Load Encryption
    loadDigitalSigning();   // Load Digital Signing

    // Seek to first frame
    if(m_fileOffsetIndex.contains(WnmElementType::Frames))
    {
        m_file.seek(m_fileOffsetIndex[WnmElementType::Frames].startOffset);
        m_nextFrameOffset = m_fileOffsetIndex[WnmElementType::Frames].startOffset;
    }

    if(m_wnmParameter.usePassword)
        emit openResult(MediaControlBase::Error::PasswordRequired); // 패스워드 입력이 필요하면, open 실패(패스워드 요청) signal 전송
    else
        emit openResult(MediaControlBase::Error::NoError);  // 패스워드 설정이 없으면 open 성공 signal 전송
}

void WnmReader::release()
{
    if(m_file.isOpen())
        m_file.close();

    if(m_dataStream.device() != nullptr)
        m_dataStream.setDevice(nullptr);

    if (m_packet.dataPtr != nullptr)
    {
        delete[] m_packet.dataPtr;
        m_packet.dataPtr = nullptr;
    }

    if(m_encryptionDataPtr != nullptr)
    {
        delete[] m_encryptionDataPtr;
        m_encryptionDataPtr = nullptr;
        m_encryptionDataSize = 0;
    }

    if(m_signingDataPtr != nullptr)
    {
        delete[] m_signingDataPtr;
        m_signingDataPtr = nullptr;
        m_signingDataSize = 0;
    }

    if(m_certificateDataPtr != nullptr)
    {
        delete[] m_certificateDataPtr;
        m_certificateDataPtr = nullptr;
        m_certificateDataSize = 0;
    }

    _resetPlay();
    m_fileOffsetIndex.clear();
    m_nextFrameOffset = 0;
    m_avgFps = 0.f;
    m_width = 0;
    m_height = 0;
    m_lastIFrameTimestamp = 0;
    m_govMsec = 0;
}

void WnmReader::_checkPassword()
{
    m_wnmParameter.password = "";

    // 암호화 되지 않은 파일이면 open 성공 signal emit 후 리턴
    if(!m_wnmParameter.usePassword || m_encryptionDataPtr == nullptr || m_encryptionDataSize == 0)
    {
        emit openResult(MediaControlBase::Error::NoError);
        return;
    }

    char chPassword[MAX_PASSWORD_LEN];
    std::memset(chPassword, 0x00, MAX_PASSWORD_LEN);
    std::strncpy(chPassword, m_tempPassword.toStdString().c_str(), m_tempPassword.toStdString().length());

    m_crypto.Init((unsigned char *)chPassword, (int)strlen(chPassword));

    int nSize = m_encryptionDataSize;
    char * plainText = m_crypto.Decrypt(m_encryptionDataPtr, &nSize);
    int plainTextSize = strlen(plainText);

    if (plainTextSize < 1) {
        return;
    }

    if(std::strncmp("wisenet viewer media export file", plainText, nSize) == 0)
    {
        // 패스워드로 복호화 한 데이터가 원본 데이터와 동일하면,
        m_wnmParameter.password = m_tempPassword;   // 패스워드 저장
        emit openResult(MediaControlBase::Error::NoError);  // open 성공 emit
    }
    else
    {
        // 패스워드 틀렸을 때 입력 팝업을 다시 띄우고싶으면 주석 해제.
        //emit openResult(MediaControlBase::Error::PasswordRequired); // open 실패(패스워드 요청) signal 재전송
    }
}

void WnmReader::_resetPlay()
{
    m_sourceFramesQueue->clear();
    m_videoFrameSequence = 0;
    m_audioFrameSequence = 0;
    m_lastVideoPts = 0;
    m_isPaused = false;
}

void WnmReader::_seek(const qint64 timeStampMsec)
{
    QListIterator<IFrameIndex> iter(m_iFrameIndexList);
    IFrameIndex iframeIndex;

    bool isForward = m_lastSpeed == 0.0f ? m_newStepForwardStatus : m_lastSpeed >= 0;

    if(isForward)
    {
        iter.toFront();
        while(iter.hasNext())
        {
            iframeIndex = iter.next();
            if(iframeIndex.pts >= timeStampMsec)
            {
                if(iframeIndex.pos >= m_fileOffsetIndex[WnmElementType::Frames].startOffset &&
                        iframeIndex.pos < m_fileOffsetIndex[WnmElementType::Frames].nextElementOffset)
                {
                    m_nextFrameOffset = iframeIndex.pos;
                    return;
                }
            }
        }
        m_nextFrameOffset = m_fileOffsetIndex[WnmElementType::Frames].nextElementOffset;
    }
    else
    {
        iter.toBack();
        while(iter.hasPrevious())
        {
            iframeIndex = iter.previous();
            if(iframeIndex.pts <= timeStampMsec)
            {
                m_nextFrameOffset = iframeIndex.pos;
                return;
            }
        }
        m_nextFrameOffset = m_fileOffsetIndex[WnmElementType::Frames].startOffset - 1;
    }
}

void WnmReader::loopTask()
{
    // 명령 확인 및 처리
    checkCommands();

    // do nothing when sleep
    if (m_isSleep) {
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        return;
    }

    // read continue
    if (!m_file.isOpen() || m_dataStream.device() == nullptr)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        return;
    }

    // TODO :: 큐가 계속 쌓일 때?
    if (m_isPaused || m_sourceFramesQueue->size() > 100)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        return;
    }

    // Read & Push one frame
    if(!readNextFrame())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        return;
    }

    if(m_packet.frameType == Wisenet::Media::MediaType::VIDEO)
    {
        pushVideo();

        // pause 상태인 경우에는 seek시 비디오 한장만 올려준다.
        if (m_lastSpeed == 0.0)
            m_isPaused = true;
    }
    else if(m_packet.frameType == Wisenet::Media::MediaType::AUDIO)
    {
        // Audio는 1배속일 때만 올려준다.
        if(isAudioPlay(m_lastSpeed))
            pushAudio();
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
}


void WnmReader::checkCommands()
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    // open, close, password
    if (m_isOpenCommand) {
        m_isOpenCommand = false;
        _open();
    }
    if (m_isCloseCommand) {
        m_isCloseCommand = false;
        release();
    }
    if (m_isCheckPasswordCommand) {
        m_isCheckPasswordCommand = false;
        _checkPassword();
    }

    // media control
    if (m_isSetSpeedCommand) {
        m_isSetSpeedCommand = false;
        m_lastSpeed = m_newSpeed;
    }
    if (m_isSeekCommand) {
        m_isSeekCommand = false;
    }
    if (m_isPlayCommand) {
        m_isPlayCommand = false;
        m_lastSpeed = m_newSpeed;
        m_isPaused = false;
    }
    if (m_isPauseCommand) {
        m_isPauseCommand = false;
        m_lastSpeed = 0.0f;
    }
    if (m_isStepCommand) {
        m_isStepCommand = false;
        // 곧바로 다음 한장을 읽기 위해서 flag를 푼다.
        m_isPaused = false;
    }

    if (m_newCommandSeq != m_lastCommandSeq) {
        m_lastCommandSeq = m_newCommandSeq;
        _resetPlay();
        _seek(m_newSeekTimeMsec);
    }

    if (m_isSleepCommand) {
        m_isSleepCommand = false;
        if (m_isSleep != m_newSleepStatus) {
            m_isSleep = m_newSleepStatus;
            m_lastSpeed = 1.0f;
            if (m_isSleep) {
                // sleep시 마지막 비디오 시간 저장
                m_lastSleepVideoPts = m_lastVideoPts;
                _resetPlay();
            }
            else {
                // wake시 마지막 비디오 시간에서 재생(1x)
                if (m_lastSleepVideoPts != 0) {
                    _seek(m_lastSleepVideoPts);
                }
            }
        }
    }
}

bool WnmReader::readNextFrame()
{
    if (!m_file.isOpen() || m_dataStream.device() == nullptr || !m_fileOffsetIndex.contains(WnmElementType::Frames))
        return false;   // 파일이 정상 open 되지 않았거나, Frames의 offset정보가 없는 경우

    if(m_nextFrameOffset < m_fileOffsetIndex[WnmElementType::Frames].startOffset ||
                m_nextFrameOffset >= m_fileOffsetIndex[WnmElementType::Frames].nextElementOffset)
        return false;   // 다음 읽을 frame 위치가 잘못되었거나, 마지막 frame까지 모두 읽은 경우

    if(m_wnmParameter.usePassword == true && m_wnmParameter.password == "")
        return false;   // 패스워드 암호화 된 파일인데, 사용자의 패스워드 입력이 되지 않은 경우

    if(m_file.pos() != m_nextFrameOffset)
        m_file.seek(m_nextFrameOffset);

    // Read base frame header
    char startCode = 0x00;
    m_dataStream.readRawData(&startCode, 1);   // start code
    if(startCode != 0x00)
    {
        // Frame 데이터 손상. Frames 영역 마지막 지점으로 이동하여 더이상 Frame read 하지 않도록 함.
        m_file.seek(m_fileOffsetIndex[WnmElementType::Frames].nextElementOffset);
        m_nextFrameOffset = m_fileOffsetIndex[WnmElementType::Frames].nextElementOffset;
        return false;
    }

    char frameType = 0x00;
    m_dataStream.readRawData(&frameType, 1);   // frame type
    m_packet.frameType = frameType == 0x01 ? Wisenet::Media::MediaType::VIDEO :
                          frameType == 0x02 ? Wisenet::Media::MediaType::AUDIO : Wisenet::Media::MediaType::UNKNOWN;

    m_dataStream >> m_packet.pts;  // pts
    m_dataStream >> m_packet.pesLength;    // pes length
    m_dataStream >> m_packet.originalDataLength;   // original data length

    char reserved[6] = {0};
    m_dataStream.readRawData(reserved, 6); // reserved


    if(m_packet.frameType == Wisenet::Media::MediaType::VIDEO)
    {
        // Read video header
        char videoFrameType = 0x00;
        m_dataStream.readRawData(&videoFrameType, 1);  // video frame type
        m_packet.videoFrameType = videoFrameType == 0x01 ? Wisenet::Media::VideoFrameType::I_FRAME :
                                   videoFrameType == 0x02 ? Wisenet::Media::VideoFrameType::P_FRAME : Wisenet::Media::VideoFrameType::UNKNOWN;

        char videoCodecType = 0x00;
        m_dataStream.readRawData(&videoCodecType, 1);  // video codec type
        m_packet.videoCodecType = videoCodecType == 0x01 ? Wisenet::Media::VideoCodecType::H265 :
                                   videoCodecType == 0x02 ? Wisenet::Media::VideoCodecType::H264 :
                                   videoCodecType == 0x03 ? Wisenet::Media::VideoCodecType::MJPEG :
                                   videoCodecType == 0x04 ? Wisenet::Media::VideoCodecType::MPEG4 :
                                   videoCodecType == 0x05 ? Wisenet::Media::VideoCodecType::VP8 :
                                   videoCodecType == 0x06 ? Wisenet::Media::VideoCodecType::VP9 :Wisenet::Media::VideoCodecType::UNKNOWN;

        m_dataStream >> m_packet.frameWidth;   // frame width
        m_dataStream >> m_packet.frameHeight;  // frame height

        if(m_packet.videoFrameType == Wisenet::Media::VideoFrameType::I_FRAME) {
            qint64 govMsec = std::abs(m_lastIFrameTimestamp - m_packet.pts);
            if(govMsec < 5100) {
                m_govMsec = govMsec;    // I-frame 사이의 간격 저장
            }
            else {
                // 5초 초과 간격의 I-Frame은 연속이 아닌 것으로..
            }
            m_lastIFrameTimestamp = m_packet.pts;
        }

        if(isFullFramePlay(m_lastSpeed)) {
            m_packet.immediate = std::abs(m_lastVideoPts - m_packet.pts) > 3100 ? true : false;
        }
        else {
            m_packet.immediate = std::abs(m_lastVideoPts - m_packet.pts) > 3100 + m_govMsec ? true : false;
        }
        m_lastVideoPts = m_packet.pts;
    }
    else if(m_packet.frameType == Wisenet::Media::MediaType::AUDIO)
    {
        // Read audio header
        char audioCodecType = 0x00;
        m_dataStream.readRawData(&audioCodecType, 1);  // audio codec type
        m_packet.audioCodecType = audioCodecType == 0x01 ? Wisenet::Media::AudioCodecType::G711U :
                                  audioCodecType == 0x02 ? Wisenet::Media::AudioCodecType::G711A :
                                  audioCodecType == 0x03 ? Wisenet::Media::AudioCodecType::G723 :
                                  audioCodecType == 0x04 ? Wisenet::Media::AudioCodecType::G726 :
                                  audioCodecType == 0x05 ? Wisenet::Media::AudioCodecType::AAC : Wisenet::Media::AudioCodecType::UNKNOWN;

        char channels = 0x00;
        m_dataStream.readRawData(&channels, 1);    // channels
        m_packet.channels = channels == 0x01 ? 1 :
                            channels == 0x02 ? 2 : 0;

        m_dataStream >> m_packet.sampleRate;    // samplerate
        m_dataStream >> m_packet.bitPerSample;  // bit per sample
        m_dataStream >> m_packet.bitRate;       // bitrate
    }

    // Read frame data
    if(m_packet.originalDataLength > 0)
    {
        // delete old data buffer
        if (m_packet.dataPtr != nullptr)
        {
            delete[] m_packet.dataPtr;
            m_packet.dataPtr = nullptr;
        }

        // alloc new data buffer & read data
        m_packet.dataPtr = new char[m_packet.originalDataLength];

        if(m_wnmParameter.usePassword == true)
        {
            // 암호화 된 데이터 read 및 복호화
            char * ciper = new char[m_packet.pesLength];
            int len = m_packet.pesLength;
            m_dataStream.readRawData(ciper, len);
            char * plain = m_crypto.Decrypt(ciper, &len);

            // 복호화 한 데이터를 패킷 data buffer에 복사
            std::memcpy(m_packet.dataPtr, plain, m_packet.originalDataLength);

            if(plain)
                delete[] plain;

            if(ciper)
                delete[] ciper;
        }
        else
        {
            // 암호화 되어있지 않으면 패킷 data buffer에 바로 Read
            m_dataStream.readRawData(m_packet.dataPtr, m_packet.originalDataLength);
        }
    }

    if(m_packet.frameType == Wisenet::Media::MediaType::VIDEO && !isFullFramePlay(m_lastSpeed))
    {
        bool isBackward = (m_lastSpeed < 0);
        int nextOffset = isBackward ? -5 : 5;
        _seek(m_packet.pts + nextOffset);
    }
    else
    {
        m_nextFrameOffset = m_file.pos();
    }

    return true;
}

void WnmReader::pushVideo()
{
    // new wisenet VideoSourceFrame
    Wisenet::Media::VideoSourceFrameSharedPtr video = std::make_shared<Wisenet::Media::VideoSourceFrame>((unsigned char*)m_packet.dataPtr, m_packet.originalDataLength);

    video->videoCodecType = m_packet.videoCodecType;
    video->frameSequence = m_videoFrameSequence++;
    //video->frameRate = 0;
    video->videoWidth = m_packet.frameWidth;
    video->videoHeight = m_packet.frameHeight;
    video->videoFrameType = m_packet.videoFrameType;
    video->frameTime.ptsTimestampMsec = m_packet.pts;
    video->dtsTimestampMsec = m_packet.pts;
    video->commandSeq = m_lastCommandSeq;
    video->immediate = m_packet.immediate;

    // push sourceFramesQueue
    m_sourceFramesQueue->push(video);
}

void WnmReader::pushAudio()
{
    // new wisenet AudioSourceFrame
    Wisenet::Media::AudioSourceFrameSharedPtr audio = std::make_shared<Wisenet::Media::AudioSourceFrame>((unsigned char*)m_packet.dataPtr, m_packet.originalDataLength);
    audio->audioCodecType = m_packet.audioCodecType;
    audio->frameSequence = m_audioFrameSequence++;
    audio->frameTime.ptsTimestampMsec = m_packet.pts;
    audio->audioChannels = m_packet.channels;
    audio->audioSampleRate = m_packet.sampleRate;
    audio->audioBitPerSample = m_packet.bitPerSample;
    audio->audioBitrate = m_packet.bitRate;
    audio->commandSeq = m_lastCommandSeq;

    // push sourceFramesQueue
    m_sourceFramesQueue->push(audio);
}

void WnmReader::loadInfos()
{
    if (!m_file.isOpen() || m_dataStream.device() == nullptr || !m_fileOffsetIndex.contains(WnmElementType::Infos))
        return;   // 파일이 정상 open 되지 않았거나, Infos offset정보가 없는 경우

    // seek to Infos offset
    m_file.seek(m_fileOffsetIndex[WnmElementType::Infos].startOffset);

    char buffer256[256]={0};

    // read device & channel Info
    std::memset(buffer256, 0x00, 256);
    m_dataStream.readRawData(buffer256, 256);
    m_wnmParameter.deviceName = QString::fromUtf8(buffer256);

    std::memset(buffer256, 0x00, 256);
    m_dataStream.readRawData(buffer256, 256);
    m_wnmParameter.channelName = QString::fromUtf8(buffer256);

    std::memset(buffer256, 0x00, 256);
    m_dataStream.readRawData(buffer256, 20);
    m_wnmParameter.deviceModel = QString::fromUtf8(buffer256);

    std::memset(buffer256, 0x00, 256);
    m_dataStream.readRawData(buffer256, 20);
    m_wnmParameter.channelModel = QString::fromUtf8(buffer256);

    // read Timezone Info
    m_dataStream >> m_wnmParameter.timezoneBias;
    m_dataStream >> m_wnmParameter.daylightBias;
    m_dataStream >> m_wnmParameter.dstStartTime;
    m_dataStream >> m_wnmParameter.dstEndTime;

    // read Version Info
    m_dataStream >> m_wnmParameter.majorVersion;
    m_dataStream >> m_wnmParameter.minorVersion;
}

void WnmReader::loadIFrameIndex()
{
    m_iFrameIndexList.clear();

    if (!m_file.isOpen() || m_dataStream.device() == nullptr || !m_fileOffsetIndex.contains(WnmElementType::IFrameIndex))
        return;   // 파일이 정상 open 되지 않았거나, IFrameIndex offset정보가 없는 경우

    // seek to IFrameIndex offset
    m_file.seek(m_fileOffsetIndex[WnmElementType::IFrameIndex].startOffset);

    // Read IFrameIndex list
    while(m_file.pos() < m_fileOffsetIndex[WnmElementType::IFrameIndex].nextElementOffset)
    {
        IFrameIndex index;
        m_dataStream >> index.pts;
        m_dataStream >> index.pos;
        m_iFrameIndexList.push_back(index);
    }
}

void WnmReader::loadTimelines()
{
    m_timelineList.clear();

    if (!m_file.isOpen() || m_dataStream.device() == nullptr || !m_fileOffsetIndex.contains(WnmElementType::Timelines))
        return;   // 파일이 정상 open 되지 않았거나, Timelines offset정보가 없는 경우

    // seek to Timelines offset
    m_file.seek(m_fileOffsetIndex[WnmElementType::Timelines].startOffset);

    // Read Timeline list
    while(m_file.pos() < m_fileOffsetIndex[WnmElementType::Timelines].nextElementOffset)
    {
        Timeline timeline;
        m_dataStream >> timeline.startTime;
        m_dataStream >> timeline.endTime;
        m_timelineList.push_back(timeline);
    }
}

void WnmReader::loadEncryption()
{
    m_wnmParameter.usePassword = false;

    if(m_encryptionDataPtr != nullptr)
    {
        delete[] m_encryptionDataPtr;
        m_encryptionDataPtr = nullptr;
        m_encryptionDataSize = 0;
    }

    if (!m_file.isOpen() || m_dataStream.device() == nullptr || !m_fileOffsetIndex.contains(WnmElementType::Encryption))
        return; // 파일이 정상 open 되지 않았거나, Encryption offset정보가 없는 경우

    // seek to Encryption offset
    m_file.seek(m_fileOffsetIndex[WnmElementType::Encryption].startOffset);

    // Read Encryption data
    m_encryptionDataSize = 0;
    m_dataStream >> m_encryptionDataSize;
    if(m_encryptionDataSize > 0)
    {
        m_wnmParameter.usePassword = true;
        m_encryptionDataPtr = new char[m_encryptionDataSize];
        m_dataStream.readRawData(m_encryptionDataPtr, m_encryptionDataSize);
    }
}

void WnmReader::loadDigitalSigning()
{
    m_wnmParameter.useDigitalSignature = false;

    if(m_signingDataPtr != nullptr)
    {
        delete[] m_signingDataPtr;
        m_signingDataPtr = nullptr;
        m_signingDataSize = 0;
    }

    if(m_certificateDataPtr != nullptr)
    {
        delete[] m_certificateDataPtr;
        m_certificateDataPtr = nullptr;
        m_certificateDataSize = 0;
    }

    if (!m_file.isOpen() || m_dataStream.device() == nullptr || !m_fileOffsetIndex.contains(WnmElementType::DigitalSigning))
        return; // 파일이 정상 open 되지 않았거나, DigitalSigning offset정보가 없는 경우

    // seek to DigitalSigning offset
    m_file.seek(m_fileOffsetIndex[WnmElementType::DigitalSigning].startOffset);

    // Read signing data
    m_signingDataSize = 0;
    m_dataStream >> m_signingDataSize;
    if(m_signingDataSize > 0)
    {
        m_signingDataPtr = new char[m_signingDataSize];
        m_dataStream.readRawData(m_signingDataPtr, m_signingDataSize);
    }

    // Read certificate data
    m_certificateDataSize = 0;
    m_dataStream >> m_certificateDataSize;
    if(m_certificateDataSize > 0)
    {
        m_certificateDataPtr = new char[m_certificateDataSize];
        m_dataStream.readRawData(m_certificateDataPtr, m_certificateDataSize);
    }
}

bool WnmReader::isFullFramePlay(const float speed)
{
    // 역방향인경우
    if (speed < 0.0f)
        return false;

    // 2x을 초과한 경우
    if (speed > 2.0f)
        return false;

    return true;
}

bool WnmReader::isAudioPlay(const float speed)
{
    // 오디오 재생은 1x만 지원
    return (speed == 1.0f);
}

bool WnmReader::needToResetPlay(const float lastSpeed, const float newSpeed)
{
    // 재생방향이 바뀐 경우
    if (lastSpeed * newSpeed < 0.0f)
        return true;
    // Full frame, Key frame play 방식이 변경된 경우
    if (isFullFramePlay(lastSpeed) != isFullFramePlay(newSpeed))
        return true;

    return false;
}

