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
#include <QString>
#include <QStandardPaths>
#include <QCoreApplication>

inline QString GetUserAppDataLocation()
{
    return QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
}

inline QString GetAppDataLocation()
{
#ifdef Q_OS_WINDOWS
    QString publicPath = getenv("public");
    QString appName = QCoreApplication::applicationName();
    if (appName.isEmpty()) {
        appName = "WisenetViewer";
#ifdef MEDIA_FILE_ONLY
        appName = "Wisenet Player";
#endif
    }

    if(publicPath != "")
        return publicPath.replace("\\", "/") + "/Documents/" + appName;
    else
        return QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
#else
    return QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
#endif
}

inline QString GetDefaultMediaLocation()
{
#ifdef Q_OS_WINDOWS
    QString publicPath = getenv("public");
    if(publicPath != "")
        return publicPath.replace("\\", "/") + "/Videos";
    else
        return QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#else
    return QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#endif
}
