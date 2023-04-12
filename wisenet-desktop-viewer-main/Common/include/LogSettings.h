/*********************************************************************************
 * Copyright(c) 2021 by Hanwha Techwin Co., Ltd.
 *
 * This software is copyrighted by, and is the sole property of Hanwha Techwin.
 * All rigths, title, ownership, or other interests in the software remain the
 * property of Hanwha Techwin. This software may only be used in accordance with
 * the corresponding license agreement. Any unauthorized use, duplication,
 * transmission, distribution, or disclosure of this software is expressly
 * forbidden.
 *
 * This Copyright notice may not be removed or modified without prior written
 * consent of Hanwha Techwin.
 *
 * Hanwha Techwin reserves the right to modify this software without notice.
 *
 * Hanwha Techwin Co., Ltd.
 * KOREA
 * https://www.hanwha-security.com/
 *********************************************************************************/
#pragma once

#ifdef NDEBUG
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#else
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#endif

#include <memory>
#include <string>
#include <vector>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/msvc_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
//#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/stopwatch.h>
#include <toml.hpp>
#include <fstream>
#include <iostream>

namespace Wisenet
{
namespace Common
{

//#
//# toml v1.0.0-rc.2 compatible
//# https://toml.io/en/v1.0.0-rc.2
//#

static constexpr auto SAMPLE_CONF = R"x(
# pattern example : %T.%e | %5t | %6l | %!%v
# ==> 18:49:03.421 |  52712 |    info | Stop QCoreServiceManager instance
# level : trace < debug < info < warning < error << critical, remark! trace level works on debug build.
# size : log file max size (MB)
# files : log file count
# fileline : true or false to print source file information for debugging

# Console() output enable, enable outputdebugstring for windows
console_enable = false

# normal log file configuration
[normal]
pattern = "%D %T.%e | %5t | %6l | %!%v"
level = "info"
size = 50
files = 2
fileline = false

# statistics log file configuration
[stat]
pattern = "%D %T.%e | %5t | %6l | %!%v"
level = "info"
size = 50
files = 2
fileline = false

# QT log file enable
[qt]
logfile = false

)x";

inline void InitializeLogSettings(const std::string& logDir, const bool forceDisableFileLog = false)
{
    const auto confPath = logDir + "/log_settings.conf";
    const auto applogPath = logDir + "/app.log";
    const auto statlogPath = logDir + "/stat.log";
    const auto qtlogPath = logDir + "/qt.log";

    spdlog::details::os::create_dir(logDir);
    std::ifstream iconfs(confPath);
    if (!iconfs.is_open()) {
        std::ofstream oconfs(confPath);
        if (oconfs.is_open()) {
#ifdef DEVELOP_VERSION
            std::string debug_conf = SAMPLE_CONF;
            std::string pattern = "level = \"info\"";
            std::string replace = "level = \"debug\"";
            auto pos = debug_conf.find(pattern);
            if (pos != std::string::npos) {
                debug_conf.replace(debug_conf.begin() + pos,
                                   debug_conf.begin() + pos + pattern.size(), replace);
            }

            pattern = "console_enable = false";
            replace = "console_enable = true";
            pos = debug_conf.find(pattern);
            if (pos != std::string::npos) {
                debug_conf.replace(debug_conf.begin() + pos,
                                   debug_conf.begin() + pos + pattern.size(), replace);
            }
            oconfs << debug_conf;
#else
            oconfs << SAMPLE_CONF;
#endif

        }
        oconfs.close();
    }
    iconfs.close();

    const std::string fileLine = " (%s:%#)";
    const std::string defaultPattern = "%D %T.%e | %5t | %7l | %!%v";

    std::string appPattern    = defaultPattern;
    int appMaxSize = 50;
    int appMaxCount = 2;
    bool appFileLine = false;
    spdlog::level::level_enum appLevel = spdlog::level::info;

    std::string statPattern   = defaultPattern;
    int statMaxSize = 50;
    int statMaxCount = 2;
    spdlog::level::level_enum statLevel = spdlog::level::info;
    bool statFileLine = false;

    bool qtLogWrite = false;
    bool consoleEnable = false;
    bool fileEnable = true;

    try {
        const auto data = toml::parse(confPath);

#ifdef NDEBUG
        consoleEnable = toml::find_or(data, "console_enable", false);
#else
        consoleEnable = toml::find_or(data, "console_enable", true);
#endif

        const auto& normal = toml::find(data, "normal");
        const auto& stat = toml::find(data, "stat");

        appPattern = toml::find_or(normal, "pattern", defaultPattern);
        const auto appLevelStr = toml::find_or(normal, "level", "debug");
//        appDays = toml::find_or(normal, "days", 14);
        appMaxSize = toml::find_or(normal, "size", 50);
        appMaxCount = toml::find_or(normal, "files", 2);
        appFileLine = toml::find_or(normal, "fileline", false);
        if (appLevelStr == "trace")
            appLevel = spdlog::level::trace;
        else if (appLevelStr == "debug")
            appLevel = spdlog::level::debug;
        else if (appLevelStr == "info")
            appLevel = spdlog::level::info;
        else if (appLevelStr == "warning")
            appLevel = spdlog::level::warn;
        else if (appLevelStr == "error")
            appLevel = spdlog::level::err;
        else if (appLevelStr == "critical")
            appLevel = spdlog::level::critical;

        statPattern = toml::find_or(stat, "pattern", defaultPattern);
        const auto statLevelStr = toml::find_or(stat, "level", "debug");
//        statDays = toml::find_or(stat, "days", 14);
        statMaxSize = toml::find_or(normal, "size", 50);
        statMaxCount = toml::find_or(normal, "files", 2);
        statFileLine = toml::find_or(stat, "fileline", false);
        if (statLevelStr == "trace")
            statLevel = spdlog::level::trace;
        else if (statLevelStr == "debug")
            statLevel = spdlog::level::debug;
        else if (statLevelStr == "info")
            statLevel = spdlog::level::info;
        else if (statLevelStr == "warning")
            statLevel = spdlog::level::warn;
        else if (statLevelStr == "error")
            statLevel = spdlog::level::err;
        else if (statLevelStr == "critical")
            statLevel = spdlog::level::critical;

        const auto& qt = toml::find(data, "qt");
        qtLogWrite = toml::find_or(qt, "logfile", false);
    }
    catch (std::exception &e) {
        (void)e;
        // do nothing
    }

    if (forceDisableFileLog) {
        fileEnable = false;
        consoleEnable = true;
        appFileLine = true;
    }

    if (appFileLine)        appPattern += fileLine;
    if (statFileLine)       statPattern += fileLine;

    // normal logger
    std::vector<spdlog::sink_ptr> normalSinks;
    //auto normalFileSink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(applogPath, 0, 0, false, appDays);
    if (fileEnable) {
        auto normalFileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(applogPath, (size_t)(appMaxSize*1048576), appMaxCount);
        normalFileSink->set_pattern(appPattern);
        normalSinks.push_back(normalFileSink);
    }

    if (consoleEnable) {
#if defined (_WIN32)
        auto normalMsvcSink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
        normalMsvcSink->set_pattern(appPattern);
        normalSinks.push_back(normalMsvcSink);
#else
        auto normalStdoutSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        normalStdoutSink->set_pattern(appPattern);
        normalSinks.push_back(normalStdoutSink);
#endif
    }

    auto normalLogger = std::make_shared<spdlog::logger>("normal", normalSinks.begin(), normalSinks.end());
    normalLogger->set_level(appLevel);

    // statistics logger
    std::vector<spdlog::sink_ptr> statSinks;
    if (fileEnable) {
        //auto statFileSink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(statlogPath, 0, 0, false, statDays);
        auto statFileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(statlogPath, statMaxSize*1048576, statMaxCount);
        statFileSink->set_pattern(statPattern);
        statSinks.push_back(statFileSink);
    }

    if (consoleEnable) {
#if defined (_WIN32)
        auto normalMsvcSink2 = std::make_shared<spdlog::sinks::msvc_sink_mt>();
        normalMsvcSink2->set_pattern(statPattern);
        statSinks.push_back(normalMsvcSink2);
#else
        auto statStdoutSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        statStdoutSink->set_pattern(statPattern);
        statSinks.push_back(statStdoutSink);
#endif
    }

    auto statLogger = std::make_shared<spdlog::logger>("stat", statSinks.begin(), statSinks.end());
    statLogger->set_level(statLevel);

    // qtdebug logger
    if (qtLogWrite && fileEnable) {
        auto qtFileLogger = spdlog::rotating_logger_mt("qtlog", qtlogPath, appMaxSize*1048576, appMaxCount);
        qtFileLogger->set_pattern("%T.%e | %5t | %7l | %v");
        qtFileLogger->set_level(spdlog::level::debug);
    }

    spdlog::register_logger(statLogger);
    spdlog::set_default_logger(normalLogger);

    spdlog::flush_on(spdlog::level::trace);

#if 0 // 잘못된 로그를 찾고 싶을 때
    spdlog::set_error_handler([](const std::string& msg) {
        std::cerr << "my err handler: " << msg << std::endl;
    });
#endif
}

inline spdlog::logger *statLoggerRaw()
{
    auto statLogger = spdlog::get("stat");
    if (statLogger)
        return statLogger.get();
    return spdlog::default_logger_raw();
}


}
}

#undef SPDLOG_LOGGER_CALL
#define SPDLOG_LOGGER_CALL(logger, level, ...) (logger)->log(spdlog::source_loc{__FILE__, __LINE__, ""}, level, __VA_ARGS__)


#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_TRACE
#define STATLOG_TRACE(...) SPDLOG_LOGGER_TRACE(Wisenet::Common::statLoggerRaw(), __VA_ARGS__)
#else
#define STATLOG_TRACE(...) (void)0
#endif

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_DEBUG
#define STATLOG_DEBUG(...) SPDLOG_LOGGER_DEBUG(Wisenet::Common::statLoggerRaw(), __VA_ARGS__)
#else
#define STATLOG_DEBUG(...) (void)0
#endif

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_INFO
#define STATLOG_INFO(...) SPDLOG_LOGGER_INFO(Wisenet::Common::statLoggerRaw(), __VA_ARGS__)
#else
#define STATLOG_INFO(...) (void)0
#endif

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_WARN
#define STATLOG_WARN(...) SPDLOG_LOGGER_WARN(Wisenet::Common::statLoggerRaw(), __VA_ARGS__)
#else
#define STATLOG_WARN(...) (void)0
#endif

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_ERROR
#define STATLOG_ERROR(...) SPDLOG_LOGGER_ERROR(Wisenet::Common::statLoggerRaw(), __VA_ARGS__)
#else
#define STATLOG_ERROR(...) (void)0
#endif

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_CRITICAL
#define STATLOG_CRITICAL(...) SPDLOG_LOGGER_CRITICAL(Wisenet::Common::statLoggerRaw(), __VA_ARGS__)
#else
#define STATLOG_CRITICAL(...) (void)0
#endif

