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
#include <QRunnable>
#include <atomic>
#include <condition_variable>
#include "MediaThreadPool.h"
#include "LogSettings.h"

class MediaRunnableTask
{
public:
    MediaRunnableTask() = default;
    ~MediaRunnableTask() = default;
    void Notify()
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_isReady = true;
        }
        m_condition.notify_one();
    }
    void Wait(const int timeoutMsec)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        std::chrono::milliseconds msec(timeoutMsec);
        if (!m_condition.wait_for(lock, msec, [this]() { return m_isReady; })) {
            /* timed out */
        }
        m_isReady = false;
    }

private:
    std::condition_variable m_condition;
    std::mutex m_mutex;
    bool m_isReady = false;
};
typedef std::shared_ptr<MediaRunnableTask> MediaRunnableTaskPtr;

class MediaRunnable : public QRunnable
{
public:
    MediaRunnable()
        : m_isRunning(false)
        , m_abortRequested(false)
    {
        setAutoDelete(false);
    }
    virtual ~MediaRunnable()
    {
        if (m_abortRequested.load() == false)
            stop();
    }

    void run() override
    {
        m_isRunning.store(true);
        while(false == m_abortRequested.load())
        {
            loopTask();
        }

        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_isRunning = false;
        }
        m_condition.notify_one();
    }

    void start()
    {
        if(m_abortRequested.load() == true)
            m_abortRequested.store(false);

        if (!MediaThreadPool::Instance()->tryStart(this)) {
            SPDLOG_WARN("MediaRunnable::start(), no more active thread, active={}",
                        MediaThreadPool::Instance()->activeThreadCount());
            MediaThreadPool::Instance()->start(this);
        }
        SPDLOG_DEBUG("MediaRunnable::start(), active={} m_abortRequested={}",
                     MediaThreadPool::Instance()->activeThreadCount(),
                     m_abortRequested.load());
    }

    void stop()
    {
        m_abortRequested.store(true);
        std::unique_lock<std::mutex> lock(m_mutex);

        m_condition.wait(lock, [this]{return m_isRunning == false;});
        SPDLOG_DEBUG("MediaRunnable::stop() end, active={} m_abortRequested={}",
                     MediaThreadPool::Instance()->activeThreadCount(), m_abortRequested.load());
    }

    MediaRunnable(MediaRunnable const&) = delete;
    MediaRunnable& operator =(MediaRunnable const&) = delete;

    virtual void loopTask() = 0;

protected:
    void endTask()
    {
        m_abortRequested.store(true);
    }

private:
    std::atomic<bool> m_isRunning;
    std::atomic<bool> m_abortRequested;
    std::condition_variable m_condition;
    std::mutex m_mutex;

};
