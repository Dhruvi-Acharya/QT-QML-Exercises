#include <QtTest>
#include <QDir>
#include <atomic>
#include <thread>
#include <boost/core/ignore_unused.hpp>
#include <toml.hpp>

#include "ThreadPool.h"
#include "LogSettings.h"
#include "Media/MediaSourceFrame.h"
#include "RtspClient.h"
#include <QAudioEncoderSettings>
#include <QAudioDeviceInfo>
#include <QAudioInput>
#include <QtCore/QIODevice>
#include <algorithm> // for std::min
#include <QThread>
#include <QtConcurrent>
#include "FFmpegStruct.h"
#include <chrono>

using namespace Wisenet::Rtsp;
using namespace Wisenet::Media;
class TalkIoDevice;

struct EncoderFrameBase
{
    explicit EncoderFrameBase(const unsigned char *dataPtr = nullptr,
                    const size_t dataSize = 0)
    {
        if (dataSize > 0 && dataPtr != nullptr) {
            m_dataPtr = new unsigned char[dataSize];
            m_dataSize = dataSize;
            std::copy(dataPtr, dataPtr+dataSize, m_dataPtr);
        }
    }

    virtual ~EncoderFrameBase()
    {
        if (m_dataPtr != nullptr)
            delete[] m_dataPtr;

        m_dataPtr = nullptr;
        m_dataSize = 0;
    }

    size_t getDataSize() const
    {
        return m_dataSize;
    }

    unsigned char * getDataPtr() const
    {
        return m_dataPtr;
    }

protected:
    size_t          m_dataSize = 0;
    unsigned char * m_dataPtr = nullptr;
};

typedef std::shared_ptr<EncoderFrameBase>    EncoderFrameBaseSharedPtr;

class Test__RtspClient : public QObject
{
    Q_OBJECT

public:
    Test__RtspClient();
    ~Test__RtspClient();

private:
    ThreadPool m_threadPool;
    RtspClientPtr m_rtspClients[16];

    std::string m_url;
    std::string m_multiChannelUrl;
    std::string m_user;
    std::string m_password;
    RtspPlayControlType m_playType;
    TransportMethod m_transportType;
    bool    m_supportQuickPlay = false;
    int     m_multiChannel = 1;
    std::string m_audioSamplePath;
    QAudioInput* m_audioInput;
public:

    int encoderOpen(EncoderFrameBaseSharedPtr &audioSourceData);
    void encoderRelease();
    bool encode(EncoderFrameBaseSharedPtr &audioSourceData, EncoderFrameBaseSharedPtr &audioEncodeData);

    RtspClientPtr m_talkClient;

    QBuffer m_wrBuff;
    QBuffer* m_rdBuff;

    AVCodecContext* m_codecContext = nullptr;
    AVFrame*        m_frame = nullptr;
    AVPacket*       m_packet = nullptr;

    EncoderFrameBaseSharedPtr encoderSource;

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_LiveStreaming();
    void test_Talk();
    void test_TalkFile();

    void handleStateChanged(QAudio::State newState);
};

Test__RtspClient::Test__RtspClient()
    : m_threadPool(8)
{

}

Test__RtspClient::~Test__RtspClient()
{

}

bool bFirst = true;
std::chrono::steady_clock::time_point begin;

void Test__RtspClient::initTestCase()
{
    Wisenet::Common::InitializeLogSettings("logs");


    static constexpr auto SAMPLE_UNIT_CONF = R"x(
    url = "rtsp://192.168.122.217:558/LiveChannel/0/media.smp"
    user = "admin"
    password = "9900pp[["
    play_type = "RTSP_PLAY_LIVE"
    transport_type = "RTP_UDP_UNICAST"
    quick_play = false
    )x";


    auto a = QDir::currentPath().toStdString();
    const auto confPath = QDir::currentPath().toStdString() + "/test_conf.txt";
    std::ifstream iconfs(confPath);
    if (!iconfs.is_open()) {
        std::ofstream oconfs(confPath);
        if (oconfs.is_open()) {
            oconfs << SAMPLE_UNIT_CONF;
        }
        oconfs.close();
    }
    iconfs.close();
    try {
        const auto data = toml::parse(confPath);
        m_url = toml::find_or(data, "url", "rtsp://192.168.122.217:558/LiveChannel/");
        m_user = toml::find_or(data, "user", "admin");
        m_password = toml::find_or(data, "password", "9900pp[[");
        std::string playType = toml::find_or(data, "play_type", "RTSP_PLAY_LIVE");
        std::string transportType = toml::find_or(data, "transport_type", "RTP_RTSP");
        m_supportQuickPlay = toml::find_or(data, "quick_play", false);
        m_multiChannel = toml::find_or(data, "multi_channel", 1);
        m_multiChannelUrl = toml::find_or(data, "multi_channel_url", "rtsp://192.168.122.217:558/LiveChannel/");

        SPDLOG_INFO("load info url={}, user={}, pw={}, ptype={}, ttype={} m_supportQuickPlay={} m_multiChannel={} m_multiChannelUrl={}",
                    m_url, m_user, m_password, playType, transportType, m_supportQuickPlay, m_multiChannel, m_multiChannelUrl);
        if (playType == "RTSP_PLAY_LIVE")
            m_playType = RtspPlayControlType::RTSP_PLAY_LIVE;
        else if (playType == "RTSP_PLAY_PB_SUNAPI_CAMERA")
            m_playType = RtspPlayControlType::RTSP_PLAY_PB_SUNAPI_CAMERA;
        else if (playType == "RTSP_PLAY_PB_SUNAPI_CAMERA_LC")
            m_playType = RtspPlayControlType::RTSP_PLAY_PB_SUNAPI_CAMERA_LC;
        else if (playType == "RTSP_PLAY_PB_SUNAPI_NVR")
            m_playType = RtspPlayControlType::RTSP_PLAY_PB_SUNAPI_NVR;
        else if (playType == "RTSP_PLAY_AUDIO_BACKCHANNEL")
            m_playType = RtspPlayControlType::RTSP_PLAY_AUDIO_BACKCHANNEL;
        else if (playType == "RTSP_PLAY_AUDIO_BACKCHANNEL")
            m_playType = RtspPlayControlType::RTSP_PLAY_AUDIO_BACKCHANNEL;

        if (transportType == "RTP_UDP_UNICAST")
            m_transportType = TransportMethod::RTP_UDP_UNICAST;
        else if (transportType == "RTP_UDP_MULTICAST")
            m_transportType = TransportMethod::RTP_UDP_MULTICAST;
        else if (transportType == "RTP_RTSP")
            m_transportType = TransportMethod::RTP_RTSP;
        else if (transportType == "RTP_RTSP_HTTP")
            m_transportType = TransportMethod::RTP_RTSP_HTTP;
        else if (transportType == "RTP_RTSP_HTTPS")
            m_transportType = TransportMethod::RTP_RTSP_HTTPS;

        m_audioSamplePath = QDir::currentPath().toStdString() + "/" + toml::find_or(data, "audio_sample_path", "");
        SPDLOG_INFO("m_audioSamplePath={}", m_audioSamplePath);
    }
    catch (std::exception& e) {
        // do nothing
        boost::ignore_unused(e);
    }

    begin = std::chrono::steady_clock::now();

    std::string url =  m_url;
    for(int i = 0; i < m_multiChannel ; i++){
        m_rtspClients[i] = std::make_shared<RtspClient>(m_threadPool.ioContext());

        if(m_multiChannel > 1)
            url = m_multiChannelUrl + std::to_string(i) + "/media.smp";
        m_rtspClients[i]->Initialize(m_playType,
                                     url,
                                     m_transportType,
                                     m_user,
                                     m_password,
                                     kDefaultRtspConnectionTimeout);
    }

    if(m_playType == RtspPlayControlType::RTSP_PLAY_AUDIO_BACKCHANNEL){

        // Talk client initalize
        m_talkClient = std::make_shared<RtspClient>(m_threadPool.ioContext());

        m_talkClient->Initialize(RtspPlayControlType::RTSP_PLAY_AUDIO_BACKCHANNEL,
                                 m_url,
                                 TransportMethod::RTP_RTSP,
                                 m_user,
                                 m_password,
                                 kDefaultRtspConnectionTimeout);
    }
    m_threadPool.Start();
}

void Test__RtspClient::cleanupTestCase()
{
    m_threadPool.Stop();
}

void Test__RtspClient::test_LiveStreaming()
{
    // Setup Test200
    SPDLOG_DEBUG("SETUP START");

    RtspSetupParam setupParam;
    setupParam.VideoOn = true;
    setupParam.AudioOn = true;
    setupParam.OnvifMetaDataOn = true;
    setupParam.MetaImageOn = false;

    int64_t audioLastSec = 0;
    SPDLOG_INFO("Time difference bFirst={} supportQuick={} " ,bFirst,  m_supportQuickPlay);
    for(int i = 0; i < m_multiChannel ; i++){
        m_rtspClients[i]->SetIntermediateCallback( // event callback
                                               [=](const RtspEventSharedPtr& e)
        {
            SPDLOG_DEBUG("RTSP EVENT OCCURRED :: {}", e->EventType);
        },

        // media callback
        [&audioLastSec](const MediaSourceFrameBaseSharedPtr& mediaSourceFrame)
        {
            // Video
            if(mediaSourceFrame->getMediaType() == MediaType::VIDEO)
            {
                const VideoSourceFrameSharedPtr& videoFrame = std::static_pointer_cast<VideoSourceFrame>(mediaSourceFrame);

                if (videoFrame->videoFrameType == VideoFrameType::I_FRAME) {
                    SPDLOG_DEBUG("[VIDEO]  size={}, width={}, height={}, type={}, ptsMsec={}, timestampMsec={}",
                                 videoFrame->getDataSize(), videoFrame->videoWidth, videoFrame->videoHeight,
                                 videoFrame->videoFrameType, videoFrame->frameTime.ptsTimestampMsec, videoFrame->frameTime.rtpTimestampMsec);

                    if(bFirst){
                        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
                        SPDLOG_INFO("Time difference = {} " , std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() );
                        SPDLOG_INFO("Time difference  size={}, width={}, height={}, type={}, ptsMsec={}, timestampMsec={}",
                                    videoFrame->getDataSize(), videoFrame->videoWidth, videoFrame->videoHeight,
                                    videoFrame->videoFrameType, videoFrame->frameTime.ptsTimestampMsec, videoFrame->frameTime.rtpTimestampMsec);
                        bFirst = false;

                    }
                }
                else {
                    SPDLOG_TRACE("[VIDEO]  size={}, width={}, height={}, type={}, ptsMsec={}, timestampMsec={}",
                                 videoFrame->getDataSize(), videoFrame->videoWidth, videoFrame->videoHeight,
                                 videoFrame->videoFrameType, videoFrame->frameTime.ptsTimestampMsec, videoFrame->frameTime.rtpTimestampMsec);
                }
            }

            // Audio
            if(mediaSourceFrame->getMediaType() == MediaType::AUDIO)
            {
                AudioSourceFrameSharedPtr audioFrame = std::static_pointer_cast<AudioSourceFrame>(mediaSourceFrame);

                if(audioLastSec != audioFrame->frameTime.ptsTimestampMsec / 1000)
                {
                    SPDLOG_DEBUG("[AUDIO] size={}, codec={}, BitPerSample={}, Bitrate={}, ptsMsec={}, timestampMsec={}",
                                 audioFrame->getDataSize(), audioFrame->audioCodecType, audioFrame->audioBitPerSample, audioFrame->audioBitrate,
                                 audioFrame->audioSampleRate, audioFrame->frameTime.ptsTimestampMsec, audioFrame->frameTime.rtpTimestampMsec);
                }
                else
                {
                    SPDLOG_TRACE("[AUDIO] size={}, codec={}, BitPerSample={}, Bitrate={}, ptsMsec={}, timestampMsec={}",
                                 audioFrame->getDataSize(), audioFrame->audioCodecType, audioFrame->audioBitPerSample, audioFrame->audioBitrate,
                                 audioFrame->audioSampleRate, audioFrame->frameTime.ptsTimestampMsec, audioFrame->frameTime.rtpTimestampMsec);
                }

                audioLastSec = audioFrame->frameTime.ptsTimestampMsec / 1000;
            }

            // MetaData
            if(mediaSourceFrame->getMediaType() == MediaType::META_DATA)
            {
                MetadataSourceFrameSharedPtr metaFrame = std::static_pointer_cast<MetadataSourceFrame>(mediaSourceFrame);
                if(metaFrame->metaDataCodecType == MetaDataCodecType::XML)
                {
                    SPDLOG_DEBUG("[META_XML] size={}, ptsMsec={}, timestampMsec={}",
                                 metaFrame->getDataSize(), metaFrame->frameTime.ptsTimestampMsec, metaFrame->frameTime.rtpTimestampMsec);
                }
                else if(metaFrame->metaDataCodecType == MetaDataCodecType::JPEG)
                {
                    SPDLOG_DEBUG("[META_JPEG] size={}, ptsMsec={}, timestampMsec={}",
                                 metaFrame->getDataSize(), metaFrame->frameTime.ptsTimestampMsec, metaFrame->frameTime.rtpTimestampMsec);
                }
            }

            return true;
        }
        );
    }

    if(m_supportQuickPlay)
    {
        std::atomic_bool quickPlayCompleted(false);
        RtspPlaybackParam playParam;
        for(int i = 0; i < m_multiChannel ; i++){
            m_rtspClients[i]->QuickPlay(
                        setupParam,
                        playParam,
                        // response callback
                        [&quickPlayCompleted](const RtspResponseSharedPtr& response)
            {
                if (!response->IsSuccess)
                {
                    SPDLOG_DEBUG("QuickPlay() failed");
                }
                else
                {
                    SPDLOG_DEBUG("QuickPlay() succeeded");
                }

                quickPlayCompleted = true;
            }
            );
        }

        // wait 10 seconds until setup completed
        QTRY_VERIFY_WITH_TIMEOUT(quickPlayCompleted == true, 10000);
    }
    else
    {
        std::atomic_bool setupCompleted(false);
        for(int i = 0; i < m_multiChannel ; i++){
            m_rtspClients[i]->Setup(
                        setupParam,

                        // response callback
                        [&setupCompleted](const RtspResponseSharedPtr& response)
            {
                if (!response->IsSuccess)
                {
                    SPDLOG_DEBUG("Setup() failed");
                }
                else
                {
                    SPDLOG_DEBUG("Setup() succeeded");
                }

                setupCompleted = true;
            }
            );
        }

        // wait 10 seconds until setup completed
        QTRY_VERIFY_WITH_TIMEOUT(setupCompleted == true, 10000);


        // Play Test
        SPDLOG_DEBUG("PLAY START");

        boost::optional<RtspPlaybackParam> playParam;

        std::atomic_bool playCompleted(false);

        for(int i = 0; i < m_multiChannel ; i++){
            m_rtspClients[i]->Play(playParam,
                                   [&playCompleted](const RtspResponseSharedPtr& response)
            {
                if (!response->IsSuccess)
                {
                    SPDLOG_DEBUG("Play() failed");
                }
                else
                {
                    SPDLOG_DEBUG("Play() succeeded");
                }

                playCompleted = true;
            }
            );
        }

        // wait 10 seconds until play completed
        QTRY_VERIFY_WITH_TIMEOUT(playCompleted == true, 10000);
    }


    // Receive video for 10 sec.
    uint waitSec = 10;
    while(waitSec > 0)
    {
        waitSec--;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // Teardown Test
    SPDLOG_DEBUG("TEARDOWN START");
    std::atomic_bool teardownCompleted(false);

    for(int i = 0; i < m_multiChannel ; i++){
        m_rtspClients[i]->Teardown([&teardownCompleted]
                               (const RtspResponseSharedPtr& response)
        {
            if (!response->IsSuccess)
            {
                SPDLOG_DEBUG("Teardown() failed");
            }
            else
            {
                SPDLOG_DEBUG("Teardown() succeeded");
            }

            teardownCompleted = true;
        }
        );
    }

    // wait 10 seconds until teardown completed
    QTRY_VERIFY_WITH_TIMEOUT(teardownCompleted == true, 10000);



    // Close Rtsp Client
    for(int i = 0; i < m_multiChannel ; i++){
        m_rtspClients[i]->Close();
    }


    /*
    // keep run test code for debug.
    while(true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    */
}

void Test__RtspClient::handleStateChanged(QAudio::State newState)
{
    SPDLOG_DEBUG("Test__RtspClient::handleStateChanged newState={}", (int)newState);
    switch (newState) {
    case QAudio::StoppedState:
        if (m_audioInput->error() != QAudio::NoError) {
            // Error handling
        } else {
            // Finished recording
        }
        break;

    case QAudio::ActiveState:{


        break;
    }
    default:
        // ... other cases as appropriate
        break;
    }
}
std::mutex g_mutex;


bool Test__RtspClient::encode(EncoderFrameBaseSharedPtr &audioSourceData, EncoderFrameBaseSharedPtr &audioEncodeData)
{
#if ENCODE_TRACE
    // SPDLOG_DEBUG("encode start, source={}\n{:X}", audioSourceData->dataSize(),
    //              spdlog::to_hex(audioSourceData->data(), audioSourceData->data()+320));
#endif
    if (!m_codecContext) {
        if (encoderOpen(audioSourceData) < 0) {
            SPDLOG_ERROR("open() failed");
            return false;
        }
    }
    int ret = av_frame_make_writable(m_frame);
    if (ret < 0) {
        SPDLOG_WARN("av_frame_make_writable() failed, ret={}, err={}", ret, av_make_error_stdstring(ret));
        return false;
    }
    //uint16_t *samples = (uint16_t*)m_frame->data[0];
    //std::memcpy(samples, audioSourceData->data(), audioSourceData->dataSize());
    ret = avcodec_fill_audio_frame(m_frame, 1, AV_SAMPLE_FMT_S16,
                                   audioSourceData->getDataPtr(), audioSourceData->getDataSize(), 1);
    if (ret < 0) {
        SPDLOG_WARN("avcodec_fill_audio_frame() failed, ret={}, err={}", ret, av_make_error_stdstring(ret));
        return false;
    }

    ret = avcodec_send_frame(m_codecContext, m_frame);
    if (ret < 0) {
        SPDLOG_WARN("avcodec_send_frame() failed, ret={}, err={}", ret, av_make_error_stdstring(ret));
        return false;
    }
    av_packet_unref(m_packet);
    while (ret >= 0) {
        ret = avcodec_receive_packet(m_codecContext, m_packet);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
#if ENCODE_TRACE
            SPDLOG_DEBUG("avcodec_receive_packet() RET={}, EAGAIN={}", ret, AVERROR(EAGAIN));
#endif
            break;
        } else if (ret < 0) {
            SPDLOG_WARN("avcodec_receive_packet() failed, ret={}, err={}", ret,
                        av_make_error_stdstring(ret));
            break;
        }

        if (m_packet->size > 0) {
            audioEncodeData = std::make_shared<EncoderFrameBase>(m_packet->data, m_packet->size);
            av_packet_unref(m_packet);
        }
    }
    if (audioEncodeData->getDataSize() > 0) {
#if ENCODE_TRACE
        SPDLOG_DEBUG("encode audio success, ret={}, dataSize={}\n{:X}", ret, audioEncodeData->dataSize(),
                     spdlog::to_hex(audioEncodeData->data(), audioEncodeData->data() + audioEncodeData->dataSize()));
#endif
        return true;
    }
    SPDLOG_WARN("encode audio failed, encoded packet size is invalid, size={}", m_packet->size);
    return false;
}

int Test__RtspClient::encoderOpen(EncoderFrameBaseSharedPtr &audioSourceData)
{
    if (m_codecContext != nullptr) {
        SPDLOG_DEBUG("PcmuEncoder::open() release opened encoder context first.");
        encoderRelease();
    }

    // find pcmu encoder
    const AVCodec *codec = avcodec_find_encoder(AV_CODEC_ID_PCM_MULAW);
    if (!codec) {
        SPDLOG_ERROR("Can not find audio encoder, AV_CODEC_ID_PCM_MULAW");
        return -1;
    }

    m_codecContext = avcodec_alloc_context3(codec);
    if (!m_codecContext) {
        SPDLOG_ERROR("Failed ato alloc audio encoder codec context.");
        encoderRelease();
        return -1;
    }

    // put sample parameter
    m_codecContext->bit_rate = 64000;
    m_codecContext->sample_rate = 8000;
    m_codecContext->channels = 1;
    m_codecContext->sample_fmt = AV_SAMPLE_FMT_S16;
    m_codecContext->channel_layout = AV_CH_LAYOUT_MONO;

    int ret = avcodec_open2(m_codecContext, codec, NULL);
    if (ret < 0) {
        SPDLOG_WARN("Failed to open audio codec encoder, ret={}, err={}", ret, av_make_error_stdstring(ret));
        encoderRelease();
        return ret;
    }

    SPDLOG_DEBUG("Success to open pcmu encoder, codec={}, sample_fmt={}, bit_rate={}, sampleRate={}, channels={}, channelLayout={}",
                 avcodec_get_name(AV_CODEC_ID_PCM_MULAW),
                 av_get_sample_fmt_name(m_codecContext->sample_fmt),
                 m_codecContext->bit_rate, m_codecContext->sample_rate,
                 m_codecContext->channels, m_codecContext->channel_layout);

    // initialize packet, set data to NULL
    m_packet = av_packet_alloc();

    // frame containing input raw audio
    m_frame = av_frame_alloc();
    if (!m_frame) {
        SPDLOG_WARN("Could not allocate frame");
        encoderRelease();
        return -1;
    }
    m_frame->channels = 1;
    m_frame->channel_layout = AV_CH_LAYOUT_MONO;
    m_frame->format = AV_SAMPLE_FMT_S16;
    m_frame->sample_rate = 8000;
    m_frame->nb_samples = audioSourceData->getDataSize() / 2;

    ret = av_frame_get_buffer(m_frame, 1);
    if (ret < 0) {
        SPDLOG_ERROR("av_frame_get_buffer() failed, ret={}, err={}", ret, av_make_error_stdstring(ret));
        encoderRelease();
        return -1;
    }
    SPDLOG_DEBUG("av_frame_get_buffer() success, nb_samples={}", m_frame->nb_samples);


    return 0;
}


static void sendMicData(Test__RtspClient* client, RtspClientPtr m_talkClient, QBuffer* rdBuff, std::atomic_bool& sendAudioCompleted)
{
    int count = 0;

    SPDLOG_DEBUG("sendMicData start");
    EncoderFrameBaseSharedPtr outputFrame;
    while(1){
        if(sendAudioCompleted==true) break;

        g_mutex.lock();
        if(rdBuff->size() > 1600){

            EncoderFrameBaseSharedPtr inputFrame = std::make_shared<EncoderFrameBase>((const unsigned char*)rdBuff->buffer().constData(), 1600);
            rdBuff->buffer().remove(0, 1600);
            const auto res = rdBuff->seek(0);
            g_mutex.unlock();

            client->encode(inputFrame, outputFrame);

            SPDLOG_DEBUG("readData #1 bufferSize= {}", (int)outputFrame->getDataSize());

            m_talkClient->SendBackChannelData(outputFrame->getDataPtr(), 800);

            count++;
        }
        else
            g_mutex.unlock();

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

    }
    SPDLOG_DEBUG("sendMicData end");
}

void Test__RtspClient::encoderRelease()
{
    if (m_codecContext != nullptr) {
        avcodec_free_context(&m_codecContext);
        m_codecContext = nullptr;
    }
    if(m_frame) {
        av_frame_free(&m_frame);
        m_frame = nullptr;
    }
    if (m_packet) {
        av_packet_free(&m_packet);
        m_packet = nullptr;
    }
}


void Test__RtspClient::test_Talk()
{
    // Setup backchannel Test
    SPDLOG_DEBUG("test_Talk START");

    if(m_playType != RtspPlayControlType::RTSP_PLAY_AUDIO_BACKCHANNEL){
        SPDLOG_INFO("test_Talk m_playType != RtspPlayControlType::RTSP_PLAY_AUDIO_BACKCHANNEL");
        return;
    }

    std::atomic_bool setupCompleted(false);

    m_talkClient->SetIntermediateCallback(
                // event callback
                [=](const RtspEventSharedPtr& e)
    {
        SPDLOG_DEBUG("RTSP EVENT OCCURRED :: {}", e->EventType);
    },
    nullptr);

    m_talkClient->SetupBackChannel(
                // response callback
                [&setupCompleted](const RtspResponseSharedPtr& response)
    {
        if (!response->IsSuccess)
        {
            SPDLOG_DEBUG("SetupBackChannel() failed");
        }
        else
        {
            SPDLOG_DEBUG("SetupBackChannel() succeeded");
        }

        setupCompleted = true;
    }
    );

    // wait 10 seconds until setup completed
    QTRY_VERIFY_WITH_TIMEOUT(setupCompleted == true, 10000);


    std::atomic_bool playCompleted(false);
    boost::optional<RtspPlaybackParam> playParam;

    m_talkClient->Play(playParam,
                       [&playCompleted](const RtspResponseSharedPtr& response)
    {
        if (!response->IsSuccess)
        {
            SPDLOG_DEBUG("Play() failed");
        }
        else
        {
            SPDLOG_DEBUG("Play() succeeded");
        }

        playCompleted = true;
    }
    );

    QTRY_VERIFY_WITH_TIMEOUT(playCompleted == true, 10000);

    // wait 10 seconds until play completed

    std::atomic_bool playingCompleted(false);


    m_wrBuff.open(QBuffer::WriteOnly);
    m_rdBuff = new QBuffer();
    m_rdBuff->open(QBuffer::ReadOnly);

    QObject::connect(&m_wrBuff, &QIODevice::bytesWritten, this, [&](qint64)
    {
        SPDLOG_DEBUG("write buffer {}", this->m_wrBuff.buffer().size());
        // write new data
        g_mutex.lock();
        m_rdBuff->buffer().append(this->m_wrBuff.buffer());
        g_mutex.unlock();
        // remove all data that was already written
        this->m_wrBuff.buffer().clear();
        this->m_wrBuff.seek(0);
    });


    const auto decideAudioFormat = [](const QAudioDeviceInfo& devInfo)
    {
        QAudioFormat format;

        format.setSampleRate(8000);
        format.setChannelCount(1);
        format.setSampleSize(16);
        format.setCodec("audio/pcm");
        format.setByteOrder(QAudioFormat::LittleEndian);
        format.setSampleType(QAudioFormat::SignedInt);// SignedInt);

        if (devInfo.isFormatSupported(format))
        {
            return format;
        }
        else
        {
            std::cerr << "Raw audio format not supported by backend, cannot play audio.\n";
            throw 0;
        }
    };

    QFutureWatcher<void> watcher;
    QFuture<void> result = QtConcurrent::run(sendMicData, this, m_talkClient, m_rdBuff, std::ref(playingCompleted));
    watcher.setFuture(result);


    QAudioInput audioInput(decideAudioFormat(QAudioDeviceInfo::defaultInputDevice()));
    audioInput.start(&m_wrBuff);

    QTRY_VERIFY_WITH_TIMEOUT(playingCompleted == true, 100000000);
    audioInput.stop();


    // Teardown Test
    SPDLOG_DEBUG("TEARDOWN START");
    std::atomic_bool teardownCompleted(false);

    m_talkClient->Teardown([&teardownCompleted]
                           (const RtspResponseSharedPtr& response)
    {
        if (!response->IsSuccess)
        {
            SPDLOG_DEBUG("Teardown() failed");
        }
        else
        {
            SPDLOG_DEBUG("Teardown() succeeded");
        }

        teardownCompleted = true;
    }
    );

    // wait 10 seconds until teardown completed
    QTRY_VERIFY_WITH_TIMEOUT(teardownCompleted == true, 10000);


    // Close Rtsp Client
    m_talkClient->Close();


    /*
    // keep run test code for debug.
    while(true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    */
}

void Test__RtspClient::test_TalkFile()
{
    // Setup backchannel Test

    SPDLOG_DEBUG("test_TalkFile File START");
    if(m_playType != RtspPlayControlType::RTSP_PLAY_AUDIO_BACKCHANNEL){
        SPDLOG_INFO("test_TalkFile m_playType != RtspPlayControlType::RTSP_PLAY_AUDIO_BACKCHANNEL");
        return;
    }

    std::atomic_bool setupCompleted(false);

    m_talkClient->SetIntermediateCallback(
                // event callback
                [=](const RtspEventSharedPtr& e)
    {
        SPDLOG_DEBUG("RTSP EVENT OCCURRED :: {}", e->EventType);
    },
    nullptr);

    m_talkClient->SetupBackChannel(
                // response callback
                [&setupCompleted](const RtspResponseSharedPtr& response)
    {
        if (!response->IsSuccess)
        {
            SPDLOG_DEBUG("SetupBackChannel() File failed");
        }
        else
        {
            SPDLOG_DEBUG("SetupBackChannel() File succeeded");
        }

        setupCompleted = true;
    }
    );

    // wait 10 seconds until setup completed
    QTRY_VERIFY_WITH_TIMEOUT(setupCompleted == true, 10000);


    std::atomic_bool playCompleted(false);
    boost::optional<RtspPlaybackParam> playParam;

    m_talkClient->Play(playParam,
                       [&playCompleted](const RtspResponseSharedPtr& response)
    {
        if (!response->IsSuccess)
        {
            SPDLOG_DEBUG("Play() File failed");
        }
        else
        {
            SPDLOG_DEBUG("Play() File succeeded");
        }

        playCompleted = true;
    }
    );

    QTRY_VERIFY_WITH_TIMEOUT(playCompleted == true, 10000);

    // wait 10 seconds until play completed

    std::atomic_bool sendAudioCompleted(false);
    // Send audio sample data
    std::ifstream fin(m_audioSamplePath, std::ios::in | std::ios::binary);
    int count = 0;
    if (fin.is_open())
    {
        SPDLOG_DEBUG("test_TalkFile file open");
        unsigned char * data = new unsigned char[2000];
        while (1)
        {
            fin.read((char*)data, 800);
            count++;
            if (count == 2000){
                sendAudioCompleted = true;
                break;
            }
            if (!fin){
                break;
            }
            m_talkClient->SendBackChannelData(data, 800);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        delete[] data;
    }
    else{
        SPDLOG_DEBUG("test_TalkFile file open fail");
    }

    QTRY_VERIFY_WITH_TIMEOUT(sendAudioCompleted == true, 10000);

    // Teardown Test
    SPDLOG_DEBUG("TEARDOWN START");
    std::atomic_bool teardownCompleted(false);

    m_talkClient->Teardown([&teardownCompleted]
                           (const RtspResponseSharedPtr& response)
    {
        if (!response->IsSuccess)
        {
            SPDLOG_DEBUG("Teardown() failed");
        }
        else
        {
            SPDLOG_DEBUG("Teardown() succeeded");
        }

        teardownCompleted = true;
    }
    );

    // wait 10 seconds until teardown completed
    QTRY_VERIFY_WITH_TIMEOUT(teardownCompleted == true, 10000);


    // Close Rtsp Client
    m_talkClient->Close();


    /*
    // keep run test code for debug.
    while(true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    */
}


QTEST_MAIN(Test__RtspClient)

#include "tst_test__rtspclient.moc"
