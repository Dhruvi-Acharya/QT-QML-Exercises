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
#include <QAudioInput>
#include <QBuffer>
#include <QScopedPointer>
#include <QPointer>
#include <QBasicMutex>
#include "FFmpegStruct.h"
#include "ThreadSafeQueue.h"
#include "MediaRunnable.h"
#include <set>
#include "PcmuEncoder.h"

typedef Wisenet::Common::ThreadSafeQueue<AudioInputFrameSharedPtr> AudioInputFrameQueue;
typedef std::shared_ptr<AudioInputFrameQueue>      AudioInputFrameQueueSharedPtr;

struct AudioChannel
{
    std::string deviceId;
    std::string channelId;
};

class AudioInputManager : public QObject, public MediaRunnable
{
    Q_OBJECT
    Q_PROPERTY(bool enable READ enable WRITE setEnable NOTIFY enableChanged)

public:
    static AudioInputManager* Instance();
    static void Initialize();

    /* do not use */
    AudioInputManager();
    ~AudioInputManager();

    bool enable() const;
    bool setStatus(std::string deviceId, std::string channelId, bool on);
    bool getStatus(std::string deviceId, std::string channelId);

protected:
    void loopTask() override;

public slots:
    bool setEnable(const bool onOff);
    void audioInputStatusChange();
signals:
    void enableChanged();
    void micUnavailable();

private:
    void release();
    void updateVolume();
    void sendAudioData(Wisenet::Media::AudioSourceFrameSharedPtr outputFrame);
    Q_DISABLE_COPY_MOVE(AudioInputManager)
    AudioInputFrameQueueSharedPtr m_audioInputFramesQueue;

    QScopedPointer<QAudioInput> m_audioInput;
    QPointer<QIODevice> m_audioDevice;
    QString m_sourceId;

    bool m_enable = false;

    bool m_lastEnable = false;
    QBuffer m_wrBuff;
    QScopedPointer<QBuffer> m_rdBuff;

    std::map<std::string, AudioChannel> m_talkList;
    std::mutex m_inputMutex;
    std::shared_ptr<PcmuEncoder> m_audioEncoder;

};

