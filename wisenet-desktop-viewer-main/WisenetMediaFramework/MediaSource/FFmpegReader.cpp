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
#include "FFmpegReader.h"
#include "LogSettings.h"
#include <chrono>


#undef SPDLOG_LOGGER_CALL
#define SPDLOG_LOGGER_CALL(logger, level, ...) (logger)->log(spdlog::source_loc{__FILE__, __LINE__, "[FFMPEG-R] "}, level, __VA_ARGS__)


#define AV_TIME_MSEC 1000
static constexpr AVRational AV_TIME_MSEC_Q{1, AV_TIME_MSEC};

FFmpegReader::FFmpegReader(QObject* parent)
    : QObject(parent)
    , m_mediaSourceFramesQueue(std::make_shared<MediaSourceFrameQueue>())
{

    //SPDLOG_DEBUG("FFmpegReader::FFmpegReader()");
    SPDLOG_DEBUG("FFmpegReader() THREAD START");
}

FFmpegReader::~FFmpegReader()
{
    stop();
    _release();
    SPDLOG_DEBUG("~FFmpegReader() THREAD END");
}

SourceFrameQueueSharedPtr FFmpegReader::getMediaSourceQueue()
{
    return m_mediaSourceFramesQueue;
}

qint64 FFmpegReader::duration() const
{
    return m_durationMsec;
}

std::string FFmpegReader::localResource() const
{
    return m_localResource;
}

bool isSupportedVideoCodec(AVCodecID codecID)
{
    if (codecID == AV_CODEC_ID_MJPEG ||
        codecID == AV_CODEC_ID_MPEG4 ||
        codecID == AV_CODEC_ID_VP8 ||
        codecID == AV_CODEC_ID_VP9 ||
        codecID == AV_CODEC_ID_AV1 ||
        codecID == AV_CODEC_ID_H264 ||
        codecID == AV_CODEC_ID_HEVC) {
        return true;
    }

    return false;
}

void FFmpegReader::_open()
{
    int err = 0;

    FFmpegCodecParamSharedPtr vcodecParamPtr;
    FFmpegCodecParamSharedPtr acodecParamPtr;

    SPDLOG_DEBUG("_open(), file={}", m_localResource);

    if (m_fmtContext) {
        SPDLOG_WARN("_open(), close opend file first.");
        _release();
    }

    // 패킷 구조체 초기화
    m_packet = av_packet_alloc();
    m_packetFiltered = av_packet_alloc();

    // 파일 열기
    err = avformat_open_input(&m_fmtContext, m_localResource.c_str(), NULL, NULL);
    if (err < 0) {
        SPDLOG_ERROR("Can not open media file, path={}", m_localResource);
        _release();
        emit openResult(MediaControlBase::Error::ResourceError);
        return;
    }

    err = avformat_find_stream_info(m_fmtContext, NULL);
    if (err < 0) {
        SPDLOG_ERROR("Can not find media stream infomation, path={}", m_localResource);
        _release();
        emit openResult(MediaControlBase::Error::FormatError);
        return;
    }

    // 비디오 스트림 찾기
    err = av_find_best_stream(m_fmtContext, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (err < 0) {
        SPDLOG_WARN("Can not find video stream, path={}", m_localResource);
    }
    else {
        SPDLOG_DEBUG("set video stream index={}", err);
        m_videoStream = m_fmtContext->streams[err];
        m_videoStreamIndex = err;
    }

    // 오디오 스트림 찾기
    err = av_find_best_stream(m_fmtContext, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    if (err < 0) {
        SPDLOG_DEBUG("Can not find audio stream, path={}", m_localResource);
    }
    else {
        SPDLOG_DEBUG("set audio stream index={}", err);
        m_audioStream = m_fmtContext->streams[err];
        m_audioStreamIndex = err;
    }

    // 오디오/비디오 둘다 못찾으면 에러처리한다.
    if (m_audioStream == nullptr && m_videoStream == nullptr) {
        SPDLOG_INFO("Can not find audio/video streams from file, path={}", m_localResource);
        _release();
        emit openResult(MediaControlBase::Error::FormatError);
        return;
    }

    if (spdlog::should_log(spdlog::level::debug)) {
        av_dump_format(m_fmtContext, 0, m_localResource.c_str(), 0);
    }

    if (m_videoStream) {
        vcodecParamPtr = std::make_shared<FFmpegCodecParam>(Wisenet::Media::MediaType::VCODEC_PARAM);
        vcodecParamPtr->allocParam(m_videoStream->codecpar);
    }
    if (m_audioStream) {
        acodecParamPtr = std::make_shared<FFmpegCodecParam>(Wisenet::Media::MediaType::ACODEC_PARAM);
        acodecParamPtr->allocParam(m_audioStream->codecpar);
    }


    if (m_videoStream) {
        AVCodecID eVideoCodec = m_videoStream->codecpar->codec_id;
        if (!isSupportedVideoCodec(eVideoCodec)) {
            SPDLOG_INFO("Not support video codec, codecID={},{}", eVideoCodec, avcodec_get_name(eVideoCodec));
            _release();
            emit openResult(MediaControlBase::Error::FormatError);
            return;
        }


        /* from FFmpegDemuxer.h of NVDEC */
        bool bMp4H264 = eVideoCodec == AV_CODEC_ID_H264 && (
                    !strcmp(m_fmtContext->iformat->long_name, "QuickTime / MOV")
                    || !strcmp(m_fmtContext->iformat->long_name, "FLV (Flash Video)")
                    || !strcmp(m_fmtContext->iformat->long_name, "Matroska / WebM")
                    );
        bool bMp4HEVC = eVideoCodec == AV_CODEC_ID_HEVC && (
                    !strcmp(m_fmtContext->iformat->long_name, "QuickTime / MOV")
                    || !strcmp(m_fmtContext->iformat->long_name, "FLV (Flash Video)")
                    || !strcmp(m_fmtContext->iformat->long_name, "Matroska / WebM")
                    );

        m_isMp4MPEG4FirstTime = eVideoCodec == AV_CODEC_ID_MPEG4 && (
                    !strcmp(m_fmtContext->iformat->long_name, "QuickTime / MOV")
                    || !strcmp(m_fmtContext->iformat->long_name, "FLV (Flash Video)")
                    || !strcmp(m_fmtContext->iformat->long_name, "Matroska / WebM")
                    );

        // Initialize bitstream filter and its required resources
        if (bMp4H264 || bMp4HEVC) {
            SPDLOG_DEBUG("av_bsf_init() needed!, extraDataSize={}", m_videoStream->codecpar->extradata_size);
        }

        m_width = m_videoStream->codecpar->width;
        m_height = m_videoStream->codecpar->height;
        m_avgFps = av_q2d(m_videoStream->avg_frame_rate);
        m_durationMsec = m_fmtContext->duration/AV_TIME_MSEC;
        SPDLOG_INFO("Media file open success..{}, {}x{}, fps={}, codec={}, streams={}, duration={}msec/{}usec",
                    m_localResource, m_width, m_height, m_avgFps, avcodec_get_name(eVideoCodec),
                    m_fmtContext->nb_streams, m_durationMsec, m_fmtContext->duration);
    }
    err = 0;

    SPDLOG_DEBUG("_open() END, file={}, err={}", m_localResource, err);
    emit openResult(MediaControlBase::Error::NoError);


    if (vcodecParamPtr)
        m_mediaSourceFramesQueue->push(vcodecParamPtr);
    if (acodecParamPtr) {
        m_mediaSourceFramesQueue->push(acodecParamPtr);
    }
}

void FFmpegReader::_release()
{
    if (m_packet) {
        av_packet_free(&m_packet);
        m_packet = nullptr;
    }
    if (m_packetFiltered) {
        av_packet_free(&m_packetFiltered);
        m_packetFiltered = nullptr;
    }

   if (m_fmtContext) {
       avformat_close_input(&m_fmtContext);
       m_fmtContext = nullptr;
   }

   _resetPlay();
   m_audioStream = nullptr;
   m_videoStream = nullptr;
   m_videoStreamIndex = -1;
   m_audioStreamIndex = -1;
   m_avgFps = 0.f;
   m_width = 0;
   m_height = 0;
   m_isFirstVideo = true;
   m_firstVideoTimeStamp = 0;
}


void FFmpegReader::open(QString& localResource)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isOpenCommand = true;
    m_localResource = localResource.toUtf8().toStdString(); // 영문 OS에서 한글 파일, 폴더 안열리는 문제 수정
}

void FFmpegReader::close()
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isCloseCommand = true;
}

void FFmpegReader::setSpeed(const float speed, const qint64 lastPlaybackTimeMsec, const unsigned char commandSeq)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isSetSpeedCommand = true;
    m_newSpeed = speed;
    m_newSeekTimeMsec = lastPlaybackTimeMsec;
    m_newCommandSeq = commandSeq;
}

void FFmpegReader::seek(const qint64 playbackTimeMsec, const unsigned char commandSeq)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isSeekCommand = true;
    m_newSeekTimeMsec = playbackTimeMsec;
    m_newCommandSeq = commandSeq;
}

void FFmpegReader::play(const qint64 lastPlaybackTimeMsec, const float lastSpeed, const unsigned char commandSeq)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isPlayCommand = true;
    m_newSeekTimeMsec = lastPlaybackTimeMsec;
    m_newSpeed = lastSpeed;
    m_newCommandSeq = commandSeq;
}

void FFmpegReader::pause(const unsigned char commandSeq)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isPauseCommand = true;
    m_newCommandSeq = commandSeq;
}

void FFmpegReader::step(const bool isForward, const qint64 lastPlaybackTimeMsec, const unsigned char commandSeq)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isStepCommand = true;
    // backward step 인 경우에는 playtime을 약간 조정해준다.
    // forward step인 경우에는 파라미터값을 사용하지 않느다.
    m_newSeekTimeMsec = isForward ? lastPlaybackTimeMsec : lastPlaybackTimeMsec-5;
    m_newStepForwardStatus = isForward;
    m_newCommandSeq = commandSeq;
}

void FFmpegReader::sleep(const bool isOn)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isSleepCommand = true;
    m_newSleepStatus = isOn;
}

/**
 * @brief FFmpegReader::_checkCommands
 * 외부에서 들어오는 명령처리
 */
void FFmpegReader::_checkCommands()
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    if (m_isOpenCommand) {
        m_isOpenCommand = false;
        _open();

    }
    if (m_isCloseCommand) {
        m_isCloseCommand = false;
        _release();
    }

    bool seekFlag = false;
    bool backwardSeekFlag = false;

    if (m_newCommandSeq != m_lastCommandSeq) {
        m_lastCommandSeq = m_newCommandSeq;
        seekFlag = true;
    }

    // playback control
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
        backwardSeekFlag = !m_newStepForwardStatus;

        // forward step 인 경우에는 cSeq를 증가하지 않는다.
        // 곧바로 다음 한장을 읽기 위해서 flag를 푼다.
        if (m_newCommandSeq == m_lastCommandSeq && m_newStepForwardStatus) {
            m_isPaused = false;
        }
    }

    if (seekFlag) {
        _resetPlay();
        _seek(m_newSeekTimeMsec, backwardSeekFlag);
    }

    if (m_isSleepCommand) {
        m_isSleepCommand = false;
        if (m_isSleep != m_newSleepStatus) {
            m_isSleep = m_newSleepStatus;
            m_lastSpeed = 1.0f;
            if (m_isSleep) {
                // sleep시 마지막 비디오 시간 저장
                m_lastSleepVideoTimeStamp = m_lastVideoTimeStamp;
                _resetPlay();
            }
            else {
                // wake시 마지막 비디오 시간에서 재생(1x)
                _seek(m_lastSleepVideoTimeStamp);
            }
        }
    }
}

/**
 * @brief FFmpegReader::loopTask
 * FFmpegReader 쓰레드 메인
 */
void FFmpegReader::loopTask()
{
    //SPDLOG_DEBUG("FFmpegReader::loopTask()");

    // 명령 확인 및 처리
    _checkCommands();

    // do nothing when sleep
    if (m_isSleep) {
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        //SPDLOG_DEBUG("sleep mode On!!");
        return;
    }

    // read continue
    if (m_fmtContext == nullptr || (m_videoStream == nullptr && m_audioStream == nullptr)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        return;
    }

    // 큐가 계속 쌓이면 대기한다. (audio + video frame count)
    // Pause 상태인 경우 대기한다. (pause상태에서 seek시에는 한장은 찾아야 하기때문에, flag는 나중에 set한다.
    if (m_isPaused || m_mediaSourceFramesQueue->size() > 100) {
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        return;
    }

    int ret = av_read_frame(m_fmtContext, m_packet);
    if (ret < 0) {
        av_packet_unref(m_packet);
        av_packet_unref(m_packetFiltered);
        if (ret == AVERROR_EOF || avio_feof(m_fmtContext->pb)) {
            SPDLOG_INFO("av_read_frame() end_of_file, ret={}, loop={}", ret, m_loopPlay);
            // EOF까지 가면 처음부터 다시 재생
            if (m_loopPlay) {
                if (!_seek(0)) {
                    SPDLOG_INFO("seek to start failed for loop");
                    return;
                }
            }
            else {
//                emit readFail(ReadResult::ReadEnd);
                return;
            }
        }

        SPDLOG_WARN("av_read_frame() failed, ret={}, msg={}", ret, av_make_error_stdstring(ret));
//        emit readFail(ReadResult::ReadFail);
        return;
    }

    bool isVideoFrame = false;
    if (m_packet->stream_index == m_videoStreamIndex && m_videoStream) {
        _pushVideo(m_packet);

        isVideoFrame = true;
        m_videoFrameSequence++;
    }
    else if (m_packet->stream_index == m_audioStreamIndex && m_audioStream) {
        if (isAudioPlay(m_lastSpeed)) {
            _pushAudio(m_packet);
            m_audioFrameSequence++;
        }
    }
    av_packet_unref(m_packet);
    av_packet_unref(m_packetFiltered);

    if (isVideoFrame) {
        // full frame play가 아닌 경우에는 다음 프레임을 seek한다.
        if (!isFullFramePlay(m_lastSpeed)) {
            bool isBackward = (m_lastSpeed < 0);
            int nextOffset = isBackward ? -5 : 5;
            auto nextTimeStamp = m_lastVideoTimeStamp + nextOffset;

            if (nextTimeStamp < m_firstVideoTimeStamp)
                nextTimeStamp = m_durationMsec;
            _seek(nextTimeStamp, isBackward);
        }

        // pause 상태인 경우에는 seek시 비디오 한장만 올려준다.
        if (m_lastSpeed == 0.0) {
            m_isPaused = true;
        }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
}


void FFmpegReader::_pushVideo(AVPacket *packet)
{
    Wisenet::Media::VideoSourceFrameSharedPtr video = nullptr;

    if (m_isMp4MPEG4FirstTime) {
        m_isMp4MPEG4FirstTime = false;
        int extraDataSize = m_videoStream->codecpar->extradata_size;
        if (extraDataSize > 0) {
            // extradata contains start codes 00 00 01. Subtract its size
            size_t dataWithHeaderSize = extraDataSize + packet->size - 3 * sizeof(uint8_t);
            auto pDataWithHeader = (uint8_t *)av_malloc(dataWithHeaderSize);

            memcpy(pDataWithHeader, m_videoStream->codecpar->extradata, extraDataSize);
            memcpy(pDataWithHeader+extraDataSize, packet->data+3, packet->size - 3 * sizeof(uint8_t));
            video = std::make_shared<Wisenet::Media::VideoSourceFrame>(pDataWithHeader, dataWithHeaderSize);
            av_free(pDataWithHeader);
        }
    }

    if (!video)
        video = std::make_shared<Wisenet::Media::VideoSourceFrame>(packet->data, packet->size);

    video->videoCodecType = get_wisenet_vcodec_type(m_videoStream->codecpar->codec_id);
    video->frameSequence = m_videoFrameSequence;
    video->frameRate = m_avgFps;
    video->videoWidth = m_width;
    video->videoHeight = m_height;
    video->videoFrameType = (packet->flags & AV_PKT_FLAG_KEY) ?
                Wisenet::Media::VideoFrameType::I_FRAME :
                Wisenet::Media::VideoFrameType::P_FRAME ;

    // convert timestamp to ms
    if (packet->pts == AV_NOPTS_VALUE && packet->dts == AV_NOPTS_VALUE) {
        SPDLOG_DEBUG("pts, dts have no values");
    }
    int64_t pkt_timeStamp = packet->pts == AV_NOPTS_VALUE ? packet->dts : packet->pts;
    video->frameTime.ptsTimestampMsec = av_rescale_q(pkt_timeStamp, m_videoStream->time_base, AV_TIME_MSEC_Q);
    video->dtsTimestampMsec = av_rescale_q(packet->dts, m_videoStream->time_base, AV_TIME_MSEC_Q);
    video->commandSeq = m_lastCommandSeq;
    m_mediaSourceFramesQueue->push(video);
    m_lastVideoTimeStamp = video->frameTime.ptsTimestampMsec;
    if (m_isFirstVideo) {
        m_isFirstVideo = false;
        m_firstVideoTimeStamp = m_lastVideoTimeStamp;
    }

    //SPDLOG_DEBUG("DECODE TIMESTAMP :: {}s,{}ms,{}dts,{}pts",  m_lastVideoTimeStamp/1000, video->frameTime.ptsTimestampMsec, packet->dts, packet->pts);
}

void FFmpegReader::_pushAudio(AVPacket* packet)
{
    auto audio = std::make_shared<Wisenet::Media::AudioSourceFrame>(packet->data, packet->size);
    audio->audioCodecType = get_wisenet_acodec_type(m_audioStream->codecpar->codec_id);
    audio->frameSequence = m_audioFrameSequence;
    audio->commandSeq = m_lastCommandSeq;
    int64_t pkt_timeStamp = packet->pts == AV_NOPTS_VALUE ? packet->dts : packet->pts;
    audio->frameTime.ptsTimestampMsec = av_rescale_q(pkt_timeStamp, m_audioStream->time_base, AV_TIME_MSEC_Q);
    m_mediaSourceFramesQueue->push(audio);
}

bool FFmpegReader::isFullFramePlay(const float speed)
{
    // 역방향인경우
    if (speed < 0.0f)
        return false;

    // 2x을 초과한 경우
    if (speed > 2.0f)
        return false;

    return true;
}

bool FFmpegReader::isAudioPlay(const float speed)
{
    // 오디오 재생은 1x만 지원
    return (speed == 1.0f);
}

bool FFmpegReader::needToResetPlay(const float lastSpeed, const float newSpeed)
{
    // 재생방향이 바뀐 경우
    if (lastSpeed * newSpeed < 0.0f)
        return true;
    // Full frame, Key frame play 방식이 변경된 경우
    if (isFullFramePlay(lastSpeed) != isFullFramePlay(newSpeed))
        return true;

    return false;
}

void FFmpegReader::_resetPlay()
{
    SPDLOG_DEBUG("_resetPlay(), clear media buffer path={}", m_localResource);
    m_mediaSourceFramesQueue->clear();
    m_videoFrameSequence = 0;
    m_audioFrameSequence = 0;
    m_lastVideoTimeStamp = 0;
    m_isPaused = false;
}

bool FFmpegReader::_seek(const qint64 timeStampMsec, const bool isBackward /* = false */)
{
    if(isFullFramePlay(m_lastSpeed))
        SPDLOG_DEBUG("_seek(), timestampMsec={}, isBackward={}", timeStampMsec, isBackward);
    qint64 timeBaseSeekTime = timeStampMsec*1000;
    int flag = isBackward ? AVSEEK_FLAG_BACKWARD : 0;
    int r = av_seek_frame(m_fmtContext, -1, timeBaseSeekTime, flag);
    if (r < 0) {
        SPDLOG_INFO("av_seek_frame() failed, path={}, r={}", m_localResource, r);
        return false;
    }
    return true;
}
