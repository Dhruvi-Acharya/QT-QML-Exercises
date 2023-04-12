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

#include "LogSettings.h"


#define SLOG_LOGGER_CALL(logger, level, ...) (logger)->log(spdlog::source_loc{__FILE__, __LINE__, m_logPrefix.c_str()}, level, __VA_ARGS__)

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_TRACE
#define SLOG_LOGGER_TRACE(logger, ...) SLOG_LOGGER_CALL(logger, spdlog::level::trace, __VA_ARGS__)
#define SLOG_TRACE(...) SLOG_LOGGER_TRACE(spdlog::default_logger_raw(), __VA_ARGS__)
#else
#define SLOG_LOGGER_TRACE(logger, ...) (void)0
#define SLOG_TRACE(...) (void)0
#endif

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_DEBUG
#define SLOG_LOGGER_DEBUG(logger, ...) SLOG_LOGGER_CALL(logger, spdlog::level::debug, __VA_ARGS__)
#define SLOG_DEBUG(...) SLOG_LOGGER_DEBUG(spdlog::default_logger_raw(), __VA_ARGS__)
#else
#define SLOG_LOGGER_DEBUG(logger, ...) (void)0
#define SLOG_DEBUG(...) (void)0
#endif

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_INFO
#define SLOG_LOGGER_INFO(logger, ...) SLOG_LOGGER_CALL(logger, spdlog::level::info, __VA_ARGS__)
#define SLOG_INFO(...) SLOG_LOGGER_INFO(spdlog::default_logger_raw(), __VA_ARGS__)
#else
#define SLOG_LOGGER_INFO(logger, ...) (void)0
#define SLOG_INFO(...) (void)0
#endif

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_WARN
#define SLOG_LOGGER_WARN(logger, ...) SLOG_LOGGER_CALL(logger, spdlog::level::warn, __VA_ARGS__)
#define SLOG_WARN(...) SLOG_LOGGER_WARN(spdlog::default_logger_raw(), __VA_ARGS__)
#else
#define SLOG_LOGGER_WARN(logger, ...) (void)0
#define SLOG_WARN(...) (void)0
#endif

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_ERROR
#define SLOG_LOGGER_ERROR(logger, ...) SLOG_LOGGER_CALL(logger, spdlog::level::err, __VA_ARGS__)
#define SLOG_ERROR(...) SLOG_LOGGER_ERROR(spdlog::default_logger_raw(), __VA_ARGS__)
#else
#define SLOG_LOGGER_ERROR(logger, ...) (void)0
#define SLOG_ERROR(...) (void)0
#endif

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_CRITICAL
#define SLOG_LOGGER_CRITICAL(logger, ...) SLOG_LOGGER_CALL(logger, spdlog::level::critical, __VA_ARGS__)
#define SLOG_CRITICAL(...) SLOG_LOGGER_CRITICAL(spdlog::default_logger_raw(), __VA_ARGS__)
#else
#define SLOG_LOGGER_CRITICAL(logger, ...) (void)0
#define SLOG_CRITICAL(...) (void)0
#endif
