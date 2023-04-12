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

#include "LogSettings.h"
#include <QFile>
#include <QLoggingCategory>
#include <QScopedPointer>

// refer from https://msjo.kr/2020/04/23/1/
namespace Wisenet
{
namespace Common
{

void QLogMessageHandler(QtMsgType type,
                        const QMessageLogContext &context,
                        const QString &msg)
{
    Q_UNUSED(context);
    auto logger = spdlog::get("qtlog");
    if (!logger)
        return;

    std::string strMsg = msg.toStdString();
    spdlog::level::level_enum level;

    switch (type) {
    case QtInfoMsg:
        level = spdlog::level::info;
        break;
    case QtDebugMsg:
        level = spdlog::level::debug;
        break;
    case QtWarningMsg:
        level = spdlog::level::warn;
        break;
    case QtCriticalMsg:
        level = spdlog::level::critical;
        break;
    case QtFatalMsg:
        level = spdlog::level::critical;
        break;
    default:
        level = spdlog::level::critical;
        break;
    }

    logger->log(level, strMsg);
}

inline void QtInitializeLogSettings(const std::string& logDir, const bool forceDisableFileLog = false)
{
    InitializeLogSettings(logDir, forceDisableFileLog);
    if (spdlog::get("qtlog")) {
        qInstallMessageHandler(QLogMessageHandler);
    }
}


}
}
