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
#include "MediaThreadPool.h"
#include <QPointer>
#include <QBasicMutex>
#include <QCoreApplication>
#include "LogSettings.h"

QThreadPool *MediaThreadPool::Instance()
{
    static QPointer<QThreadPool> gInstance;
    static QBasicMutex mutex;

    const QMutexLocker lock(&mutex);
    if (gInstance.isNull() && !QCoreApplication::closingDown())
        gInstance = new QThreadPool();
    return gInstance;
}

void MediaThreadPool::Initialize()
{
    Instance()->setMaxThreadCount(10240);   // 최대 증가가능한 쓰레드 갯수는 최대한 크게
    Instance()->setExpiryTimeout(30000);    // 30초 지나면 쉬고 있는 쓰레드 삭제
    SPDLOG_DEBUG("MediaThreadPool::Initialize(), maxThreadCount={}, expirytimeout={}, activeThreadCount={}",
                 Instance()->maxThreadCount(),
                 Instance()->expiryTimeout(),
                 Instance()->activeThreadCount());
}

MediaThreadPool::MediaThreadPool()
{

}

MediaThreadPool::~MediaThreadPool()
{

}
