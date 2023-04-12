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
#include "AudioInputManager.h"
#include <QPointer>
#include <QBasicMutex>
#include <QCoreApplication>
#include "QCoreServiceManager.h"
#include "CoreService/CoreServiceStructure.h"


#undef SPDLOG_LOGGER_CALL
#define SPDLOG_LOGGER_CALL(logger, level, ...) (logger)->log(spdlog::source_loc{__FILE__, __LINE__, "[AudioInputManager] "}, level, __VA_ARGS__)


AudioInputManager *AudioInputManager::Instance()
{
    static QPointer<AudioInputManager> gInstance;
    static QBasicMutex mutex;

    const QMutexLocker lock(&mutex);
    if (gInstance.isNull() && !QCoreApplication::closingDown()) {
        gInstance = new AudioInputManager();
    }
    return gInstance;
}

void AudioInputManager::Initialize()
{
    // just create singletone instance
    AudioInputManager::Instance();
}


AudioInputManager::AudioInputManager()
    : m_audioInputFramesQueue(std::make_shared<AudioInputFrameQueue>())
{
    SPDLOG_DEBUG("AudioInputManager() THREAD START");
    connect(this, &AudioInputManager::enableChanged, this, &AudioInputManager::audioInputStatusChange);
}

AudioInputManager::~AudioInputManager()
{
    stop();
    release();
    SPDLOG_DEBUG("~AudioInputManager() THREAD STOP");
}

void AudioInputManager::release()
{
    if (m_audioInput) {
        m_audioInput->stop();
        m_audioInput.reset();
        m_audioDevice = nullptr;
    }
    if(m_rdBuff){
        m_rdBuff.reset();
    }

}

bool AudioInputManager::setStatus(std::string deviceId, std::string channelId, bool on)
{
    std::string guid = deviceId + "_" + channelId;

    SPDLOG_DEBUG("AudioInputManager::setStatus guid={} on={}", guid, on);
    if(on) {
        if(m_talkList.find(guid) == m_talkList.end()){
            SPDLOG_DEBUG("AudioInputManager::setStatus1 guid={} on={}", guid, on);
            AudioChannel channel;
            channel.deviceId = deviceId;
            channel.channelId = channelId;
            m_talkList.emplace(guid, channel);
        }
        else
            SPDLOG_ERROR("AudioInputManager::setStatus2 guid={} on={}", guid, on);
    }
    else {
        if(m_talkList.find(guid) != m_talkList.end())
            m_talkList.erase(guid);
        else
            SPDLOG_ERROR("AudioInputManager::setStatus3 guid={} on={}", guid, on);
    }
    bool ret = setEnable(m_talkList.size()>0);
    if(!ret)
        m_talkList.erase(guid);


    return ret;
}

bool AudioInputManager::getStatus(std::string deviceId, std::string channelId)
{
    std::string guid = deviceId + "_" + channelId;

    if(m_talkList.find(guid) != m_talkList.end()){
        SPDLOG_DEBUG("AudioInputManager::getStatus guid={} true", guid);
        return true;
    }
    else{
        SPDLOG_DEBUG("AudioInputManager::getStatus guid={} false", guid);
        return false;
    }
}

bool AudioInputManager::enable() const
{
    return m_enable;
}

bool AudioInputManager::setEnable(const bool onOff)
{
    if (m_enable != onOff) {
        SPDLOG_DEBUG("Set audio enabled::{}", onOff);
        m_enable = onOff;

        if(m_enable){

            QAudioDeviceInfo devInfo = QAudioDeviceInfo::defaultInputDevice();
            QAudioFormat format;

            format.setSampleRate(8000);
            format.setChannelCount(1);
            format.setSampleSize(16);
            format.setCodec("audio/pcm");
            format.setByteOrder(QAudioFormat::LittleEndian);
            format.setSampleType(QAudioFormat::SignedInt);// SignedInt);


            if (devInfo.isFormatSupported(format) && devInfo.defaultInputDevice().deviceName().toStdString() != std::string("auto_null.monitor")){
                SPDLOG_DEBUG("AudioInputManager::setEnable inputDevice={}",
                             devInfo.defaultInputDevice().deviceName().toStdString());
                m_audioInput.reset(new QAudioInput(format));
            }
            else{
                SPDLOG_DEBUG("AudioInputManager::setEnable mic unavailable");
                m_enable = false;
                return false;
            }
        }

        emit enableChanged();
    }
    return true;
}

void AudioInputManager::audioInputStatusChange()
{
    SPDLOG_DEBUG("AudioInputManager audioInputStatusChange m_enable={}", m_enable);
    if(m_enable){
        m_wrBuff.open(QBuffer::WriteOnly);
        m_rdBuff.reset(new QBuffer());
        m_rdBuff->open(QBuffer::ReadOnly);

        QObject::connect(&m_wrBuff, &QIODevice::bytesWritten, this, [&](qint64)
        {
            if(this->m_wrBuff.buffer().size() > 0){
              //  SPDLOG_DEBUG("AudioInputManager::capture");
                m_inputMutex.lock();
                if(!m_rdBuff.isNull())
                    m_rdBuff->buffer().append(this->m_wrBuff.buffer());
                m_inputMutex.unlock();
                // remove all data that was already written
                this->m_wrBuff.buffer().clear();
                this->m_wrBuff.seek(0);
            }
        });
        m_audioInput->start(&m_wrBuff);
        SPDLOG_INFO("AudioInputManager::m_audioInput.start");
        start();
    }
    else{
        if(m_wrBuff.isOpen())
            m_wrBuff.close();
        stop();
        if(!m_audioInput.isNull()){
            m_audioInput->stop();
            m_audioInput.reset();
        }
        m_inputMutex.lock();
        if(!m_rdBuff.isNull() && m_rdBuff->isOpen()){
            m_rdBuff->close();
            m_rdBuff.reset();
        }
        m_inputMutex.unlock();
    }
}

void AudioInputManager::loopTask()
{
    m_inputMutex.lock();
    if(m_rdBuff->size() > 1600){
        AudioInputFrameSharedPtr inputFrame =
                std::make_shared<AudioInputFrame>((const unsigned char*)m_rdBuff->buffer().constData(), 1600);
        m_rdBuff->buffer().remove(0, 1600);
        const auto res = m_rdBuff->seek(0);
        m_inputMutex.unlock();

        // 2023.01.11. coverity (ubuntu)
        Wisenet::Media::AudioSourceFrameSharedPtr outputFrame(nullptr);
         if(!m_audioEncoder)
            m_audioEncoder = std::make_shared<PcmuEncoder>();
        bool success = m_audioEncoder->encode(inputFrame, outputFrame);
        sendAudioData(outputFrame);
        Q_UNUSED(success);
        Q_UNUSED(res);
    }
    else
        m_inputMutex.unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
}

void AudioInputManager::sendAudioData(Wisenet::Media::AudioSourceFrameSharedPtr outputFrame)
{
#ifndef MEDIA_FILE_ONLY
    for(auto &iter : m_talkList){
        Wisenet::Device::DeviceSendTalkDataRequestSharedPtr reqParam =
                std::make_shared<Wisenet::Device::DeviceSendTalkDataRequest>();
        reqParam->deviceID = iter.second.deviceId;
        reqParam->channelID = iter.second.channelId;
        reqParam->audioFrame = outputFrame;

        QCoreServiceManager::Instance().RequestToCoreService(
                    &Wisenet::Core::ICoreService::DeviceSendTalkData,
                    this, reqParam, nullptr);
    }
#endif
}

