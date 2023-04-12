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
#include "AudioOutputManager.h"
#include <QPointer>
#include <QBasicMutex>
#include <QCoreApplication>


#undef SPDLOG_LOGGER_CALL
#define SPDLOG_LOGGER_CALL(logger, level, ...) (logger)->log(spdlog::source_loc{__FILE__, __LINE__, "[AudioManager] "}, level, __VA_ARGS__)


AudioOutputManager *AudioOutputManager::Instance()
{
    static QPointer<AudioOutputManager> gInstance;
    static QBasicMutex mutex;

    const QMutexLocker lock(&mutex);
    if (gInstance.isNull() && !QCoreApplication::closingDown()) {
        gInstance = new AudioOutputManager();
    }
    return gInstance;
}

void AudioOutputManager::Initialize()
{
    // just create singletone instance
    AudioOutputManager::Instance();
}


AudioOutputManager::AudioOutputManager()
    : m_audioOutFramesQueue(std::make_shared<AudioOutFrameQueue>())
{
    SPDLOG_DEBUG("AudioOutputManager() THREAD START");
    start();
}

AudioOutputManager::~AudioOutputManager()
{
    stop();
    release();
    SPDLOG_DEBUG("~AudioOutputManager() THREAD STOP");
}

void AudioOutputManager::release()
{
    if (m_audioOutput) {
        m_audioOutput->stop();
        m_audioOutput.reset();
        m_audioDevice = nullptr;
    }

}


void AudioOutputManager::onNewAudioFrame(const AudioOutputFrameSharedPtr &audioFrame)
{
    //SPDLOG_DEBUG("NEW AUDIO FRAME::size={}", audioFrame->dataSize());
    m_audioOutFramesQueue->push(audioFrame);
}

int AudioOutputManager::volume() const
{
    return m_volume;
}

void AudioOutputManager::setVolume(const int volume)
{
    if (m_volume != volume) {
        //SPDLOG_DEBUG("Set audio volume::{}", volume);
        m_volume = volume;
        if (m_volume > 100)
            m_volume = 100;
        else if (m_volume < 0)
            m_volume = 0;
        emit volumeChanged();
    }
}


bool AudioOutputManager::enable() const
{
    return m_enable;
}

void AudioOutputManager::setEnable(const bool onOff)
{
    if (m_enable != onOff) {
        //SPDLOG_DEBUG("Set audio enabled::{}", onOff);
        m_enable = onOff;
        emit enableChanged();
    }
}


void AudioOutputManager::updateVolume()
{
    m_lastEnable = m_enable;
    m_lastVolume = m_volume;

    if (m_audioOutput) {
        qreal linearVolume = 0;
        if (m_lastEnable) {
            linearVolume = QAudio::convertVolume(qreal(m_lastVolume) / qreal(100.0),
                                                 QAudio::LogarithmicVolumeScale,
                                                 QAudio::LinearVolumeScale);
        }
        m_audioOutput->setVolume(linearVolume);
    }
}

void AudioOutputManager::loopTask()
{
    AudioOutputFrameSharedPtr audioFrame = nullptr;
    if (!m_audioOutFramesQueue->pop_front(audioFrame)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        return;
    }

    auto audioFormat = audioFrame->audioFormat();
    if (!m_audioOutput || audioFormat != m_audioFormat || m_sourceId != audioFrame->sourceID()) {
        SPDLOG_DEBUG("reset audio output");
        release();

        m_audioFormat = audioFormat;
        m_sourceId = audioFrame->sourceID();
        m_audioOutput.reset(new QAudioOutput(m_audioFormat));
        m_audioOutput->setBufferSize((int)audioFrame->dataSize()*16);
        updateVolume();

        m_audioDevice = m_audioOutput->start();

    }
    if (m_audioDevice) {
        m_audioDevice->write((const char*)audioFrame->data(), audioFrame->dataSize());
    }

    if (m_volume != m_lastVolume || m_enable != m_lastEnable) {
        updateVolume();
    }


    std::this_thread::sleep_for(std::chrono::milliseconds(2));
}


void AudioOutputManager::checkAudioDeviceInfo()
{
#if 0 // TODO :: 위치 이동 (WINDOWS에서 아래 루틴의 체크 시간이 오래 걸림
    QList<QAudioDeviceInfo> devices = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    QAudioDeviceInfo defaultDevice(QAudioDeviceInfo::defaultOutputDevice());

    for (int i = 0 ; i < devices.size() ; i++) {
        auto &device = devices.at(i);
        SPDLOG_DEBUG("{} AUDIO OUT DEVICE NAME : {}", i, device.deviceName().toStdString());
        SPDLOG_DEBUG("{} PREFERED FORMAT : channel={}, sampleRate={}, sampleSize={}, sampleType={}, codec={}",
                     i,
                     device.preferredFormat().channelCount(),
                     device.preferredFormat().sampleRate(),
                     device.preferredFormat().sampleSize(),
                     device.preferredFormat().sampleType(),
                     device.preferredFormat().codec().toStdString());
        SPDLOG_DEBUG("{} Supported channels==", i);
        auto supportedChannels = device.supportedChannelCounts();
        for (int j = 0 ; j < supportedChannels.size(); j++) {
            SPDLOG_DEBUG("  {}", supportedChannels.at(j));
        }
        SPDLOG_DEBUG("{} Supported sampleRates==", i);
        auto supportedSampleRates = device.supportedSampleRates();
        for (int j = 0 ; j < supportedSampleRates.size() ; j++) {
            SPDLOG_DEBUG("  {}", supportedSampleRates.at(j));
        }
        SPDLOG_DEBUG("{} Supported sampleSizes==", i);
        auto supportedSampleSizes = device.supportedSampleSizes();
        for (int j = 0 ; j < supportedSampleSizes.size() ; j++) {
            SPDLOG_DEBUG("  {}", supportedSampleSizes.at(j));
        }
        SPDLOG_DEBUG("{} Supported sampleTypes==", i);
        auto supportedSampleTypes = device.supportedSampleTypes();
        for (int j = 0 ; j < supportedSampleTypes.size() ; j++) {
            SPDLOG_DEBUG("  {}", supportedSampleTypes.at(j));
        }
    }

    SPDLOG_DEBUG("DEFAULT AUDIO OUT DEVICE NAME: {}", defaultDevice.deviceName().toStdString());
#endif
}
