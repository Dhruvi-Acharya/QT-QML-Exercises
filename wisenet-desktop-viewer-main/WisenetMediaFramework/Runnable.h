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
#include <atomic>
#include <thread>

class Runnable
{
public:
    Runnable()
        : m_isRunning(false)
        , m_abortRequested(false)
    {
    }
    virtual ~Runnable()
    {
        stop();
    }

    void start()
    {
        m_thread = std::thread(&Runnable::run, this);
    }
    void stop()
    {
        m_abortRequested.store(true);
        if (m_thread.joinable())
            m_thread.join();
    }

    Runnable(Runnable const&) = delete;
    Runnable& operator =(Runnable const&) = delete;

protected:
    virtual void run()
    {
        m_isRunning.store(true);
        while(false == m_abortRequested.load())
        {
            loopTask();
        }
        m_isRunning.store(false);
    }

    virtual void loopTask() = 0;
    std::atomic<bool> m_isRunning;
    std::atomic<bool> m_abortRequested;

private:
    std::thread m_thread;
};
