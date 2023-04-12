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
#include <QObject>
#include <QScopedPointer>
#include <QPointer>
#include <QBasicMutex>
#include "FFmpegStruct.h"
#include "ThreadSafeQueue.h"
#include "MediaRunnable.h"
#include "QCoreServiceManager.h"

#define PUGIXML_HEADER_ONLY
#include <pugixml.hpp>

typedef Wisenet::Common::ThreadSafeQueue<MetaFrameSharedPtr> MetaFrameQueue;
typedef std::shared_ptr<MetaFrameQueue>      MetaFrameQueueSharedPtr;

class PosMetaManager : public QObject, public MediaRunnable
{
    Q_OBJECT
    Q_PROPERTY(QString posMeta READ posMeta WRITE setPosMeta NOTIFY posDataChanged)

public:
    static PosMetaManager* Instance();
    static void Initialize();

    /* do not use */
    PosMetaManager();
    ~PosMetaManager();

    /* special purpose */
    void onNewMetaFrame(const MetaFrameSharedPtr& metaFrame);

    QString posMeta() const;

    void setPosConfData(std::vector<Wisenet::Device::TextPosConfResult> confData);

protected:
    void loopTask() override;

public slots:
    void setPosMeta(const QString posMeta);

signals:
    void posDataChanged(QString pos);

private:
    QString parseXmlData(std::string rawData);

private:
    std::vector<Wisenet::Device::TextPosConfResult> m_confData;
    pugi::xml_document  m_xmlDoc;
    pugi::xml_node m_xmlNode;
    QString m_posData;
    Q_DISABLE_COPY_MOVE(PosMetaManager)
    MetaFrameQueueSharedPtr m_metaFramesQueue;
};

