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
#include <QAudioOutput>
#include <QScopedPointer>
#include <QPointer>
#include <QBasicMutex>
#include "FFmpegStruct.h"
#include "ThreadSafeQueue.h"
#include "MediaRunnable.h"

typedef Wisenet::Common::ThreadSafeQueue<AudioOutputFrameSharedPtr> AudioOutFrameQueue;
typedef std::shared_ptr<AudioOutFrameQueue>      AudioOutputFrameQueueSharedPtr;

class AudioOutputManager : public QObject, public MediaRunnable
{
    Q_OBJECT
    Q_PROPERTY(bool enable READ enable WRITE setEnable NOTIFY enableChanged)
    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged)

public:
    static AudioOutputManager* Instance();
    static void Initialize();

    /* do not use */
    AudioOutputManager();
    ~AudioOutputManager();

    /* special purpose */
    void onNewAudioFrame(const AudioOutputFrameSharedPtr& audioFrame);

    bool enable() const;
    int volume() const;


protected:
    void loopTask() override;

public slots:
    void setEnable(const bool onOff);
    void setVolume(const int volume); /* 0 ~ 100 */

signals:
    void volumeChanged();
    void enableChanged();

private:
    void checkAudioDeviceInfo();
    void release();
    void updateVolume();

    Q_DISABLE_COPY_MOVE(AudioOutputManager)
    AudioOutputFrameQueueSharedPtr m_audioOutFramesQueue;

    QScopedPointer<QAudioOutput> m_audioOutput;
    QPointer<QIODevice> m_audioDevice;
    QAudioFormat m_audioFormat;
    QString m_sourceId;

    bool m_enable = false;
    int m_volume = 100;

    bool m_lastEnable = false;
    int m_lastVolume = 100;
};

