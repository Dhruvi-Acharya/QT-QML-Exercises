#pragma once

#include <functional>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include "boost/asio.hpp"
#include "LogSettings.h"

class ThreadPool
{
public:
    explicit ThreadPool(std::size_t size)
        : m_ioContext()
        , m_work(boost::asio::require(m_ioContext.get_executor(),
                                      boost::asio::execution::outstanding_work.tracked))
        //, m_workGuard(boost::asio::make_work_guard(m_ioContext))
        , m_workersCount(size)
    {
    }

    ~ThreadPool()
    {
        Stop();
    }

    void Start()
    {
        if (!m_isStarted) {
            m_isStarted = true;
            m_workers.reserve(m_workersCount);
            for (std::size_t i = 0; i < m_workersCount; ++i) {
                m_workers.emplace_back([this]()
                {
                    //SPDLOG_INFO("IO CONTEXT RUN");
                    m_ioContext.run();
                });
            }
        }
    }

    void Stop()
    {
        if (m_isStarted) {
            m_isStarted = false;
            m_work = boost::asio::any_io_executor();
            SPDLOG_INFO("[ThreadPool] IO CONTEXT stop start");
            m_ioContext.stop();
            SPDLOG_INFO("[ThreadPool] IO CONTEXT stop end");
            for (auto& w : m_workers) {
                SPDLOG_INFO("[ThreadPool] m_workers join start");
                w.join();
                SPDLOG_INFO("[ThreadPool] m_workers join end");
            }
            m_workers.clear();
            SPDLOG_INFO("[ThreadPool] m_workers clear end");
        }
    }

    bool IsRunning()
    {
        return m_isStarted;
    }

    size_t threadCount()
    {
        return m_workersCount;
    }

    boost::asio::io_context& ioContext()
    {
        return m_ioContext;
    }

private:
    std::vector<std::thread> m_workers;
    boost::asio::io_context m_ioContext;
    boost::asio::any_io_executor m_work;
    bool    m_isStarted = false;
    size_t  m_workersCount = 0;

//    typedef boost::asio::io_context::executor_type ExecutorType;
//    boost::asio::executor_work_guard<ExecutorType> m_workGuard;
};
