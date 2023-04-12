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
#include "SecReader.h"
#include "LogSettings.h"
#include <chrono>


#undef SPDLOG_LOGGER_CALL
#define SPDLOG_LOGGER_CALL(logger, level, ...) (logger)->log(spdlog::source_loc{__FILE__, __LINE__, "[WNM-R] "}, level, __VA_ARGS__)


SecReader::SecReader(QObject* parent)
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
    SPDLOG_DEBUG("SecReader() THREAD START");
}

SecReader::~SecReader()
{
    stop();
    release();
    SPDLOG_DEBUG("~SecReader() THREAD END");
}

SourceFrameQueueSharedPtr SecReader::getSourceFrameQueue()
{
    return m_sourceFramesQueue;
}

void SecReader::open(QString& localResource)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isOpenCommand = true;
    m_localResource = localResource;
}

void SecReader::close()
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isCloseCommand = true;
}

void SecReader::setSpeed(const float speed, const qint64 lastPlaybackTimeMsec, const unsigned char commandSeq)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isSetSpeedCommand = true;
    m_newSpeed = speed;
    m_newSeekTimeMsec = lastPlaybackTimeMsec;
    m_newCommandSeq = commandSeq;
}

void SecReader::seek(const qint64 playbackTimeMsec, const unsigned char commandSeq)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isSeekCommand = true;
    m_newCommandSeq = commandSeq;
    m_newSeekTimeMsec = playbackTimeMsec;
}

void SecReader::play(const qint64 lastPlaybackTimeMsec, const float lastSpeed, const unsigned char commandSeq)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isPlayCommand = true;
    m_newCommandSeq = commandSeq;
    m_newSeekTimeMsec = lastPlaybackTimeMsec;
    m_newSpeed = lastSpeed;
}

void SecReader::pause(const unsigned char commandSeq)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isPauseCommand = true;
    m_newCommandSeq = commandSeq;
}

void SecReader::step(const bool isForward, const qint64 lastPlaybackTimeMsec, const unsigned char commandSeq)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isStepCommand = true;
    // backward step 인 경우에는 playtime을 약간 조정해준다.
    // forward step인 경우에는 파라미터값을 사용하지 않느다.
    m_newSeekTimeMsec = isForward ? lastPlaybackTimeMsec : lastPlaybackTimeMsec-5;
    m_newStepForwardStatus = isForward;
    m_newCommandSeq = commandSeq;
    SPDLOG_DEBUG("SecReader::Step() isForward:{} commandSeq:{} time:{}", m_newStepForwardStatus, m_newCommandSeq, lastPlaybackTimeMsec);
}

void SecReader::sleep(const bool isOn)
{
    SPDLOG_DEBUG("sleep(), isOn={}", isOn);
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isSleepCommand = true;
    m_newSleepStatus = isOn;
}

void SecReader::checkPassword(QString& password)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isCheckPasswordCommand = true;
    m_tempPassword = password;
}

QMap<qint64, qint64> SecReader::getTimeline()
{
    QMap<qint64, qint64> timeline;

    for(auto& block : m_timelineList)
    {
        timeline.insert(block.startTime, block.endTime);
    }

    return timeline;
}

QString SecReader::getChannelName()
{
    return m_wnmParameter.channelName;
}

void SecReader::_open()
{
    release();

    // 확장자 체크
    int extensionIndex = m_localResource.lastIndexOf(".");
    if(extensionIndex != -1 && extensionIndex != m_localResource.length() - 1)
    {
        QString extension = m_localResource.right(m_localResource.length() - extensionIndex - 1);
        if(extension.toLower() != "sec")
        {
            SPDLOG_ERROR("SecReader::_open() File extension error. fileName={}", m_localResource.toLocal8Bit().toStdString());
            emit openResult(MediaControlBase::Error::FormatError);
            return;
        }
    }

    // File open
    m_file.setFileName(m_localResource);
    if(m_file.open(QIODevice::ReadOnly) == false)
    {
        SPDLOG_ERROR("SecReader::_open() File open error. fileName={}", m_localResource.toLocal8Bit().toStdString());
        emit openResult(MediaControlBase::Error::ResourceError);
        return;
    }
    m_dataStream.setDevice(&m_file);

    loadIFrameIndex();      // Load I-Frame Index
    loadTimelines();        // Load Timelines
    loadInfos();            // Load Infos (장비명, 채널명, 암호화)

    if(m_useDeviceLocalTime)
    {
        // 파일에 타임존 정보가 없으면 장비의 localTime을 사용하기 위해 PC Timezone만큼 시간 보정
        const QDateTime now = QDateTime::currentDateTime();
        m_localTimeUtcOffset = now.timeZone().standardTimeOffset(now) * 1000;

        for(auto& iFrameIndex : m_iFrameIndexList)
        {
            iFrameIndex.pts -= m_localTimeUtcOffset;
        }

        for(auto& timeline : m_timelineList)
        {
            timeline.startTime -= m_localTimeUtcOffset;
            timeline.endTime -= m_localTimeUtcOffset;
        }
    }

    // Seek to first frame
    m_file.seek(0);
    m_nextFrameOffset = 0;

    if(m_wnmParameter.usePassword)
        emit openResult(MediaControlBase::Error::PasswordRequired); // 패스워드 입력이 필요하면, open 실패(패스워드 요청) signal 전송
    else
        emit openResult(MediaControlBase::Error::NoError);  // 패스워드 설정이 없으면 open 성공 signal 전송
}

void SecReader::release()
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

    _resetPlay();
    m_nextFrameOffset = -1;
    m_avgFps = 0.f;
    m_width = 0;
    m_height = 0;
    m_lastIFrameTimestamp = 0;
    m_govMsec = 0;
}

void SecReader::_checkPassword()
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

    if(std::strncmp("samsung backupviewer sec file", plainText, nSize) == 0)
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

void SecReader::_resetPlay()
{
    m_sourceFramesQueue->clear();
    m_videoFrameSequence = 0;
    m_audioFrameSequence = 0;
    m_lastVideoPts = 0;
    m_isPaused = false;
}

void SecReader::_seek(const qint64 timeStampMsec)
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
                if(iframeIndex.pos >= 0 && iframeIndex.pos < m_posMediaEnd)
                {
                    m_nextFrameOffset = iframeIndex.pos;
                    return;
                }
            }
        }
        m_nextFrameOffset = m_posMediaEnd;
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
        m_nextFrameOffset = -1;
    }
}

void SecReader::loopTask()
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


void SecReader::checkCommands()
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

bool SecReader::readNextFrame()
{
    if (!m_file.isOpen() || m_dataStream.device() == nullptr || m_posMediaEnd == 0)
        return false;   // 파일이 정상 open 되지 않았거나, Frames 데이터 끝의 offset 정보가 없는 경우

    if(m_nextFrameOffset < 0 || m_nextFrameOffset >= m_posMediaEnd)
        return false;   // 다음 읽을 frame 위치가 잘못되었거나, 마지막 frame까지 모두 읽은 경우

    if(m_wnmParameter.usePassword == true && m_wnmParameter.password == "")
        return false;   // 패스워드 암호화 된 파일인데, 사용자의 패스워드 입력이 되지 않은 경우

    if(m_file.pos() != m_nextFrameOffset)
        m_file.seek(m_nextFrameOffset);

    // Read frame header
    char buf[MAX_HEADER_LENGTH] = {0};
    std::memset(buf, 0x00, MAX_HEADER_LENGTH);
    int len = m_dataStream.readRawData(buf, m_nHeaderLength);
    if(len < m_nHeaderLength || buf[0] != 0x00)
    {
        // Frame 데이터 손상. Frames 영역 마지막 지점으로 이동하여 더이상 Frame read 하지 않도록 함.
        SPDLOG_DEBUG("SecReader::readNextFrame() header read failed");
        m_file.seek(m_posMediaEnd);
        m_nextFrameOffset = m_posMediaEnd;
        return false;
    }

    // frame type
    unsigned char frameType = buf[1];
    m_packet.frameType = frameType == SEC_VIDEO ? Wisenet::Media::MediaType::VIDEO :
                         frameType == SEC_AUDIO ? Wisenet::Media::MediaType::AUDIO : Wisenet::Media::MediaType::UNKNOWN;

    if(m_packet.frameType == Wisenet::Media::MediaType::VIDEO)
    {
        char videoFrameType = buf[4];   // video frame type
        m_packet.videoFrameType = videoFrameType == 0x01 ? Wisenet::Media::VideoFrameType::I_FRAME :
                                   videoFrameType == 0x02 ? Wisenet::Media::VideoFrameType::P_FRAME : Wisenet::Media::VideoFrameType::UNKNOWN;

        unsigned char videoCodecType = buf[3] & 0x0F; // video codec type
        m_packet.videoCodecType = videoCodecType == 10 ? Wisenet::Media::VideoCodecType::H265 :
                                  videoCodecType == 3 ? Wisenet::Media::VideoCodecType::H264 :
                                  videoCodecType == 2 ? Wisenet::Media::VideoCodecType::MJPEG :
                                  videoCodecType == 4 ? Wisenet::Media::VideoCodecType::MPEG4 :
                                  videoCodecType == 7 ? Wisenet::Media::VideoCodecType::MPEG4 :
                                  videoCodecType == 11 ? Wisenet::Media::VideoCodecType::VP8 : Wisenet::Media::VideoCodecType::UNKNOWN;
    }
    else if(m_packet.frameType == Wisenet::Media::MediaType::AUDIO)
    {
        // Read audio header
        unsigned char audioCodecType = buf[3] & 0x0F; // audio codec type
        m_packet.audioCodecType = audioCodecType == 1 ? Wisenet::Media::AudioCodecType::G711U :
                                  audioCodecType == 2 ? Wisenet::Media::AudioCodecType::G711U :
                                  audioCodecType == 10 ? Wisenet::Media::AudioCodecType::G711U :
                                  audioCodecType == 6 ? Wisenet::Media::AudioCodecType::G723 :
                                  audioCodecType == 7 ? Wisenet::Media::AudioCodecType::G723 :
                                  audioCodecType == 11 ? Wisenet::Media::AudioCodecType::G726 :
                                  audioCodecType == 12 ? Wisenet::Media::AudioCodecType::AAC : Wisenet::Media::AudioCodecType::UNKNOWN;

        char channels = buf[4]; // channels
        m_packet.channels = channels == 0x01 ? 1 :
                            channels == 0x02 ? 2 : 0;
    }

    // PTS
    m_packet.pts = (qint64)makeUlong(buf[6], buf[7], buf[8], buf[9], buf[10]) * 1000;

    // DTS
    m_packet.dts = (unsigned long)makeUlong(buf[11], buf[12], buf[13], buf[14], buf[15]);

    if(m_useDeviceLocalTime)
    {
        m_packet.pts -= m_localTimeUtcOffset;
        m_packet.dts -= m_localTimeUtcOffset;
    }

    if(m_nHeaderLength == USE_AES_ENCRYPT_VER_HEADER_LENGTH)
    {
        int width = 0; // Audio -> Sample Rate
        width += buf[25] << 24;
        width += buf[26] << 16;
        width += buf[27] << 8;
        width += buf[28] & 0xFF;

        int height = 0; // Audio -> Bit per second
        height += buf[29] << 24;
        height += buf[30] << 16;
        height += buf[31] << 8;
        height += buf[32] & 0xFF;

        if(frameType == SEC_VIDEO)
        {
            m_packet.frameWidth = width;    // frame width
            m_packet.frameHeight = height;  // frame height
        }
        else if(frameType == SEC_AUDIO)
        {
            if(m_packet.audioCodecType == Wisenet::Media::AudioCodecType::AAC)
            {
                switch(height)
                {
                case 0: m_packet.sampleRate = 96000; break;
                case 1: m_packet.sampleRate = 88200; break;
                case 2: m_packet.sampleRate = 64000; break;
                case 3: m_packet.sampleRate = 48000; break;
                case 4: m_packet.sampleRate = 44100; break;
                case 5: m_packet.sampleRate = 32000; break;
                case 6: m_packet.sampleRate = 24000; break;
                case 7: m_packet.sampleRate = 22050; break;
                case 8: m_packet.sampleRate = 16000; break;
                case 9: m_packet.sampleRate = 12000; break;
                case 10: m_packet.sampleRate = 11025; break;
                case 11: m_packet.sampleRate = 8000; break;
                case 12: m_packet.sampleRate = 7350; break;
                default: m_packet.sampleRate = 16000; break;
                }
            }
            else
            {
                m_packet.sampleRate = height * 1000;
            }

            m_packet.bitPerSample = width;
        }

        // PES Length
        int pesLength = (unsigned char)buf[21] << 24;
        pesLength += (unsigned char)buf[22] << 16;
        pesLength += (unsigned char)buf[23] << 8;
        pesLength += (unsigned char)buf[24] & 0xFF;
        m_packet.pesLength = pesLength - m_nHeaderLength;  // header size를 뺀다.

        // Encrypted PES Length
        int originalDataLength = (unsigned char)buf[49] << 24;
        originalDataLength += (unsigned char)buf[50] << 16;
        originalDataLength += (unsigned char)buf[51] << 8;
        originalDataLength += (unsigned char)buf[52] & 0xFF;
        m_packet.originalDataLength = originalDataLength;
    }
    else
    {
        if(m_nHeaderLength = MAKE_SEC_NEW_VER_HEADER_LENGTH)
        {
            int width = 0; // Audio -> Sample Rate
            width += buf[24] << 24;
            width += buf[25] << 16;
            width += buf[26] << 8;
            width += buf[27] & 0xFF;

            int height = 0; // Audio -> Bit per second
            height += buf[28] << 24;
            height += buf[29] << 16;
            height += buf[30] << 8;
            height += buf[31] & 0xFF;

            if(frameType == SEC_VIDEO)
            {
                m_packet.frameWidth = width;    // frame width
                m_packet.frameHeight = height;  // frame height
            }
            else if(frameType == SEC_AUDIO)
            {
                m_packet.sampleRate = height * 1000;
                m_packet.bitPerSample = width;
            }
        }
        else
        {
            unsigned char bFrameSize = buf[5];
            switch(bFrameSize){
            case 2:
                m_packet.frameWidth = 176;
                m_packet.frameHeight = 144;
                break;
            case 3:
                m_packet.frameWidth = 176;
                m_packet.frameHeight = 120;
                break;
            case 4:
                m_packet.frameWidth = 352;
                m_packet.frameHeight = 288;
                break;
            case 5:
                m_packet.frameWidth = 352;
                m_packet.frameHeight = 240;
                break;
            case 6:
                m_packet.frameWidth = 704;
                m_packet.frameHeight = 288;
                break;
            case 7:
                m_packet.frameWidth = 704;
                m_packet.frameHeight = 240;
                break;
            case 8:
                m_packet.frameWidth = 704;
                m_packet.frameHeight = 576;
                break;
            case 9:
                m_packet.frameWidth = 704;
                m_packet.frameHeight = 480;
                break;
            case 10:
                m_packet.frameWidth = 720;
                m_packet.frameHeight = 576;
                break;
            case 11:
                m_packet.frameWidth = 640;
                m_packet.frameHeight = 480;
                break;
            case 12:
                m_packet.frameWidth = 320;
                m_packet.frameHeight = 240;
                break;
            case 13:
                m_packet.frameWidth = 800;
                m_packet.frameHeight = 600;
                break;
            case 14:
                m_packet.frameWidth = 1280;
                m_packet.frameHeight = 720;
                break;
            case 15:
                m_packet.frameWidth = 1280;
                m_packet.frameHeight = 1024;
                break;
            case 16:
                m_packet.frameWidth = 1024;
                m_packet.frameHeight = 768;
                break;
            case 17:
                m_packet.frameWidth = 320;
                m_packet.frameHeight = 180;
            case 18:
                m_packet.frameWidth = 640;
                m_packet.frameHeight = 360;
                break;
            case 19:
                m_packet.frameWidth = 800;
                m_packet.frameHeight = 450;
                break;
            case 20:
                m_packet.frameWidth = 1920;
                m_packet.frameHeight = 1080;
                break;
            case 21:
                m_packet.frameWidth = 1280;
                m_packet.frameHeight = 960;
                break;
            case 22:
                m_packet.frameWidth = 1600;
                m_packet.frameHeight = 1200;
                break;
            case 23:
                m_packet.frameWidth = 2048;
                m_packet.frameHeight = 1536;
                break;
            default:
                m_packet.frameWidth = 352;
                m_packet.frameHeight = 240;
                break;
            }
        }

        // PES Length
        int pesLength = (unsigned char)buf[21] << 16;
        pesLength += (unsigned char)buf[22] << 8;
        pesLength += (unsigned char)buf[23] & 0xFF;
        m_packet.pesLength = pesLength - m_nHeaderLength; // header size를 뺀다.
        m_packet.originalDataLength = m_packet.pesLength;
    }

    if(m_packet.frameType == Wisenet::Media::MediaType::VIDEO)
    {
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

    if(m_packet.frameType == Wisenet::Media::MediaType::AUDIO)
    {
        if (m_packet.audioCodecType == Wisenet::Media::AudioCodecType::AAC)
        {
            m_packet.channels = 1;
            m_packet.bitRate = 48000;
            m_packet.bitPerSample = 48000 / m_packet.sampleRate;
        }
        else if (m_packet.audioCodecType == Wisenet::Media::AudioCodecType::G726)
        {
            m_packet.bitRate = m_packet.sampleRate * m_packet.bitPerSample;
        }
        else if (m_packet.audioCodecType == Wisenet::Media::AudioCodecType::G711U)
        {
            m_packet.channels = 1;
            m_packet.sampleRate = 8000;
            m_packet.bitRate = 64000;
            m_packet.bitPerSample = 8;
        }
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

void SecReader::pushVideo()
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
    video->dtsTimestampMsec = m_packet.dts;
    video->commandSeq = m_lastCommandSeq;
    video->immediate = m_packet.immediate;

    // push sourceFramesQueue
    m_sourceFramesQueue->push(video);
}

void SecReader::pushAudio()
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

void SecReader::loadInfos()
{
    if (!m_file.isOpen() || m_dataStream.device() == nullptr)
        return;   // 파일이 정상 open 되지 않은 경우

    if(m_posInfoStart == 0) {
        SPDLOG_DEBUG("SecReader::loadInfos() call loadTimelines() before loadInfos()");
        return;
    }

    if (m_file.pos() >= m_posFileEnd) {
        SPDLOG_DEBUG("SecReader::loadInfos() file end before device name");
        return;
    }

    m_nHeaderLength = MAKE_SEC_NEW_VER_HEADER_LENGTH;

    char buff[256] = {0};

    m_file.seek(m_posInfoStart);

    //device name
    std::memset(buff, 0x00, 256);
    m_dataStream.readRawData(buff, 256);
    m_wnmParameter.deviceName = QString::fromUtf8(buff);

    //channel name
    std::memset(buff, 0x00, 256);
    m_dataStream.readRawData(buff, 256);
    m_wnmParameter.channelName = QString::fromUtf8(buff);

    //model name
    std::memset(buff, 0x00, 256);
    m_dataStream.readRawData(buff, 20);
    m_wnmParameter.channelModel = QString::fromUtf8(buff);

    //recModel name
    std::memset(buff, 0x00, 256);
    m_dataStream.readRawData(buff, 20);
    m_wnmParameter.deviceModel = QString::fromUtf8(buff);

    // device name이 없을 경우의 예외처리.
    if (m_wnmParameter.deviceName.length() == 0)
    {
        if(m_wnmParameter.deviceModel.length() != 0)
            m_wnmParameter.deviceName =m_wnmParameter.deviceModel;
        else if(m_wnmParameter.channelModel.length() != 0)
            m_wnmParameter.deviceName = m_wnmParameter.channelModel;
    }

    if (m_file.pos() >= m_posFileEnd) {
        SPDLOG_DEBUG("SecReader::loadInfos() file end before encryption");
        return;
    }

    m_nHeaderLength = USE_AES_ENCRYPT_VER_HEADER_LENGTH;

    // 암호화 여부
    m_wnmParameter.usePassword = false;
    if(m_encryptionDataPtr != nullptr)
    {
        delete[] m_encryptionDataPtr;
        m_encryptionDataPtr = nullptr;
        m_encryptionDataSize = 0;
    }

    m_dataStream.readRawData(buff, 1);
    if (buff[0] == 0x01)
    {
        // 암호화 패스워드 확인을 위한 데이터 길이
        m_dataStream.readRawData(buff, 4);
        m_encryptionDataSize = makeLong(buff[0], buff[1], buff[2], buff[3]);

        if (m_encryptionDataSize > 0)
        {
            m_wnmParameter.usePassword = true;
            m_encryptionDataPtr = new char[m_encryptionDataSize];
            m_dataStream.readRawData(m_encryptionDataPtr, m_encryptionDataSize);
        }
    }
    else
    {
        m_dataStream.readRawData(buff, 4);
    }

    if (m_file.pos() >= m_posFileEnd) {
        SPDLOG_DEBUG("SecReader::loadInfos() file end after encryption");
        return;
    }

    // Dewarping Information 건너뜀
    m_file.seek(m_file.pos() + 4 + 4 + 4 + 4 + (20*9));
    if (m_file.pos() >= m_posFileEnd) {
        SPDLOG_DEBUG("SecReader::loadInfos() file end after Dewarping Information");
        return;
    }

    // MetaData 건너뜀
    m_file.seek(m_file.pos() + 1 + 256 + 4);
    if (m_file.pos() >= m_posFileEnd) {
        SPDLOG_DEBUG("SecReader::loadInfos() file end after MetaData");
        return;
    }

    // Privacy Masking 건너뜀
    m_dataStream.readRawData(buff, 4);
    int maskingCount = makeLong(buff[0], buff[1], buff[2], buff[3]);
    m_file.seek(m_file.pos() + (17*maskingCount));

    // v1.7
    if ((m_posFileEnd - 4) <= m_file.pos())
        return;

    // v1.8 (cameraMaxWidth, cameraMaxHeight) 건너뜀
    m_file.seek(m_file.pos() + 4 + 4);
    if ((m_posFileEnd - 4) <= m_file.pos()) // v1.8
        return;

    // v1.9 (Digital Signing) 건너뜀
    m_dataStream.readRawData(buff, 4);
    int signatureLength = makeLong(buff[0], buff[1], buff[2], buff[3]);
    if (signatureLength > 0)
        m_file.seek(m_file.pos() + signatureLength);

    m_dataStream.readRawData(buff, 4);
    int certificateLength = makeLong(buff[0], buff[1], buff[2], buff[3]);
    if (certificateLength > 0)
        m_file.seek(m_file.pos() + certificateLength);

    if ((m_posFileEnd - 4) <= m_file.pos())
        return;

    // Timezone
    SPDLOG_DEBUG("SecReader::loadInfos() timezone exist");
    m_useDeviceLocalTime = false;

    // GoogleMap
}

void SecReader::loadIFrameIndex()
{
    m_iFrameIndexList.clear();

    if (!m_file.isOpen() || m_dataStream.device() == nullptr)
        return;   // 파일이 정상 open 되지 않은 경우

    // seek to IFrameIndex offset
    qint64 fileSize = m_file.size();
    m_file.seek(fileSize - 5);

    // seek to IFrameIndex pos
    char chBuf[11]={0};
    std::memset(chBuf, 0x00, 5);
    m_dataStream.readRawData(chBuf, 5);

    qint64 posIframeLen = makeUlong(chBuf[0], chBuf[1], chBuf[2], chBuf[3], chBuf[4]);

    m_posMediaEnd = posIframeLen;
    m_file.seek(posIframeLen);

    // get index count
    std::memset(chBuf, 0x00, 5);
    m_dataStream.readRawData(chBuf, 5);

    long length = makeUlong(chBuf[0], chBuf[1], chBuf[2], chBuf[3], chBuf[4]);
    m_idxCount = length / 11;

    if (m_idxCount == 0)
        return;

    // load index data
    for (qint64 idx = 0; idx < m_idxCount; idx++)
    {
        std::memset(chBuf, 0x00, 11);
        m_dataStream.readRawData(chBuf, 11);

        IFrameIndex index;

        // pos
        index.pos = (chBuf[1] & 0x60) << 25;
        index.pos |= (chBuf[2] & 0x7F) << 23;
        index.pos |= (chBuf[3] & 0xFF) << 15;
        index.pos |= (chBuf[4] & 0x7F) << 8;
        index.pos |= (chBuf[5] & 0xFF);

        // pts
        index.pts = (qint64)makeUlong(chBuf[6], chBuf[7], chBuf[8], chBuf[9], chBuf[10]) * 1000;

        m_iFrameIndexList.push_back(index);
    }
}

void SecReader::loadTimelines()
{
    m_timelineList.clear();

    if (!m_file.isOpen() || m_dataStream.device() == nullptr)
        return;   // 파일이 정상 open 되지 않은 경우

    // seek to Timelines offset
    qint64 fileSize = m_file.size();
    m_file.seek(fileSize - 10);
    m_posFileEnd = m_file.pos();

    // seek to Timelines pos
    char chBuf[11]={0};
    std::memset(chBuf, 0x00, 5);
    m_dataStream.readRawData(chBuf, 5);

    qint64 posTimelineLength = makeUlong(chBuf[0], chBuf[1], chBuf[2], chBuf[3], chBuf[4]);
    m_file.seek(posTimelineLength);

    // Read Timeline list
    std::memset(chBuf, 0x00, 5);
    m_dataStream.readRawData(chBuf, 5);
    qint64 length = makeUlong(chBuf[0], chBuf[1], chBuf[2], chBuf[3], chBuf[4]);
    m_lTimeLineNum = length / 11;
    if (m_lTimeLineNum == 0)
        return;

    for (qint64 i = 0; i < m_lTimeLineNum; i++)
    {
        Timeline timeline;

        std::memset(chBuf, 0x00, 11);
        m_dataStream.readRawData(chBuf, 11);

        //Start Time
        timeline.startTime = (chBuf[1] & 0x60) << 25;
        timeline.startTime |= (chBuf[2] & 0x7F) << 23;
        timeline.startTime |= (chBuf[3] & 0xFF) << 15;
        timeline.startTime |= (chBuf[4] & 0x7F) << 8;
        timeline.startTime |= (chBuf[5] & 0xFF);
        timeline.startTime = timeline.startTime * 1000;

        //End Time
        timeline.endTime = (qint64)makeUlong(chBuf[6], chBuf[7], chBuf[8], chBuf[9], chBuf[10]) * 1000;

        // BackupViewer에서 에외처리로 존재함. 이유는 잘 모르겠음.
        if (timeline.startTime > timeline.endTime)
            timeline.endTime = timeline.startTime;

        // IFrame정보와 Valid 체크를 하여 넣는다. 실제 프레임이 있는 경우가 있어 배제함.
        ChangeVaildTimeline(timeline.startTime, timeline.endTime);

        m_timelineList.push_back(timeline);
    }

    m_posInfoStart = m_file.pos();
}

void SecReader::ChangeVaildTimeline(qint64 &lStart, qint64 &lEnd)
{
    int i = 0;

    //시작 시간 확인
    for(i = 0 ; i < m_iFrameIndexList.count(); i++ )
    {
        if(m_iFrameIndexList[i].pts == lStart)
            break;

        if(m_iFrameIndexList[i].pts > lStart)
        {
            lStart = m_iFrameIndexList[i].pts;
            break;
        }
    }

    if(lEnd < lStart)
        lEnd = lStart;
}

int SecReader::makeUlong(unsigned char p1, unsigned char p2, unsigned char p3, unsigned char p4, unsigned char p5)
{
    unsigned long ret = 0;

    ret = ((p1 & 0x7F) << 25) |
            (p2 << 17) |
            ((p3 & 0x7F) << 10) |
            (p4 << 2) |
            ((p5 & 0x60) >> 5);

    return ret;
}

int SecReader::makeLong(unsigned char p1, unsigned char p2, unsigned char p3, unsigned char p4)
{
    unsigned long ret = 0;

    ret += ((p1 << 24) & 0xFF000000);
    ret += ((p2 << 16) & 0x00FF0000);
    ret += ((p3 << 8) & 0x0000FF00);
    ret += ((p4 & 0xFF) & 0x000000FF);

    return ret;
}

bool SecReader::isFullFramePlay(const float speed)
{
    // 역방향인경우
    if (speed < 0.0f)
        return false;

    // 2x을 초과한 경우
    if (speed > 2.0f)
        return false;

    return true;
}

bool SecReader::isAudioPlay(const float speed)
{
    // 오디오 재생은 1x만 지원
    return (speed == 1.0f);
}

bool SecReader::needToResetPlay(const float lastSpeed, const float newSpeed)
{
    // 재생방향이 바뀐 경우
    if (lastSpeed * newSpeed < 0.0f)
        return true;
    // Full frame, Key frame play 방식이 변경된 경우
    if (isFullFramePlay(lastSpeed) != isFullFramePlay(newSpeed))
        return true;

    return false;
}

