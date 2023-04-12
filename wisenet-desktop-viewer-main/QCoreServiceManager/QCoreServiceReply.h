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
#include <QtCore>
#include "CoreService.h"
//#include "LogSettings.h"



class QCoreServiceRequestor;
class QCoreServiceReply;
class QCoreServiceEvent;

typedef QSharedPointer<QCoreServiceRequestor> QCoreServiceRequestorPtr;
typedef QSharedPointer<QCoreServiceReply> QCoreServiceReplyPtr;
typedef QSharedPointer<QCoreServiceEvent> QCoreServiceEventPtr;


class QCoreServiceRequestor : public QObject
{
    Q_OBJECT
public:
    explicit QCoreServiceRequestor(QObject *parent = nullptr)
        : QObject(parent)
    {
        //SPDLOG_DEBUG("NEW QCoreServiceRequester");
    }
    ~QCoreServiceRequestor()
    {
        //SPDLOG_DEBUG("DESTROY QCoreServiceRequester");
    }
signals:
    void Completed(QCoreServiceReplyPtr);
};


class QCoreServiceReply : public QObject
{
    Q_OBJECT
public:
    explicit QCoreServiceReply(QObject *parent = nullptr)
        : QObject(parent)
    {
        //SPDLOG_DEBUG("NEW QCoreServiceReply");
    }
    ~QCoreServiceReply()
    {
        //SPDLOG_DEBUG("DESTROY QCoreServiceReply");
    }

    Wisenet::ResponseBaseSharedPtr  responseDataPtr;
    QCoreServiceRequestorPtr        requestorPtr;
};


class QCoreServiceEvent : public QObject
{
    Q_OBJECT
public:
    explicit QCoreServiceEvent(QObject *parent = nullptr)
        : QObject(parent)
    {
        //SPDLOG_DEBUG("NEW QCoreServiceEvent");
    }
    ~QCoreServiceEvent()
    {
        //SPDLOG_DEBUG("DESTROY QCoreServiceEvent");
    }

    Wisenet::EventBaseSharedPtr    eventDataPtr;
};



Q_DECLARE_METATYPE(QCoreServiceReplyPtr)
Q_DECLARE_METATYPE(QCoreServiceEventPtr)

typedef std::function <void(const QCoreServiceReplyPtr&)> QCoreServiceReplyHandler;
typedef std::function <void(const QCoreServiceEventPtr&)> QCoreServiceEventHandler;
