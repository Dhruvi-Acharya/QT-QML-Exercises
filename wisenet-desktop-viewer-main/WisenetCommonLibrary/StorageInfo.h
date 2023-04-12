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
#include <QStorageInfo>
#include "LogSettings.h"

#ifdef Q_OS_WINDOWS
#include <fileapi.h>
#endif

inline qint64 GetMbytesAvailable(QString filePath)
{
    qint64 bytes = -1;

#ifdef Q_OS_WINDOWS
    // Call the Win32 API to retrieve disk space information.
    ULARGE_INTEGER freeBytesAvailable, totalBytes, freeBytes;
    if(GetDiskFreeSpaceEx(filePath.toStdWString().c_str(), &freeBytesAvailable, &totalBytes, &freeBytes))
        bytes = freeBytesAvailable.QuadPart;
#else
    QStorageInfo storage = QStorageInfo(filePath);
    bytes = storage.bytesAvailable();
#endif
    if(bytes == -1) {
        SPDLOG_INFO("GetMbytesAvailable() check failed. filePath:{}", filePath.toLocal8Bit().toStdString());
        return -1;
    }
    else {
        return bytes / (1024 * 1024);
    }
}
