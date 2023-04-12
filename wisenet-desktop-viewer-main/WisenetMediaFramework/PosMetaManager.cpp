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
#include "PosMetaManager.h"
#include <QPointer>
#include <QBasicMutex>
#include <QCoreApplication>
#include "QCoreServiceManager.h"

#undef SPDLOG_LOGGER_CALL
#define SPDLOG_LOGGER_CALL(logger, level, ...) (logger)->log(spdlog::source_loc{__FILE__, __LINE__, "[PosMetaManager] "}, level, __VA_ARGS__)


PosMetaManager *PosMetaManager::Instance()
{
    static QPointer<PosMetaManager> gInstance;
    static QBasicMutex mutex;

    const QMutexLocker lock(&mutex);
    if (gInstance.isNull() && !QCoreApplication::closingDown()) {
        gInstance = new PosMetaManager();
    }
    return gInstance;
}

void PosMetaManager::Initialize()
{
    // just create singletone instance
    PosMetaManager::Instance();
}

PosMetaManager::PosMetaManager()
{
    m_metaFramesQueue = std::make_shared<MetaFrameQueue>();

    start();
}

PosMetaManager::~PosMetaManager()
{
    stop();
}

void PosMetaManager::onNewMetaFrame(const MetaFrameSharedPtr& metaFrame)
{
    m_metaFramesQueue->push(metaFrame);
}

QString PosMetaManager::posMeta() const
{
    return m_posData;
}

void PosMetaManager::setPosMeta(const QString posMeta)
{
    if (m_posData != posMeta) {
        m_posData = posMeta;
        //SPDLOG_INFO("=====> (result): {}", posMeta.toStdString().c_str());
        emit posDataChanged(posMeta);
    }
}

void PosMetaManager::loopTask()
{
    MetaFrameSharedPtr metaFrame = nullptr;
    if (!m_metaFramesQueue->pop_front(metaFrame)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        return;
    }

    auto rawData = metaFrame->metaData();
    setPosMeta(parseXmlData(rawData));

    std::this_thread::sleep_for(std::chrono::milliseconds(2));
}

QString PosMetaManager::parseXmlData(std::string rawData)
{
//    <?xml version="1.0" encoding="UTF-8"?><tt:MetadataStream xmlns:tt="http://www.onvif.org/ver10/schema">
//        <tt:POSData>
//            <tt:POSFrame ID="1" UtcTime="2022-10-11T05:26:59Z">
//                <datetime>1665466019</datetime><content>
//    Hanwha Vision !!!
//                </content></tt:POSFrame>
//        </tt:POSData>
//    </tt:MetadataStream>

    if (!m_xmlDoc.load_string(rawData.c_str())) {
        return "";
    }

    std::string parseData = "";

    pugi::xpath_query posFrame_query("//tt:POSData/tt:POSFrame");
    pugi::xpath_query posDateTime_query("datetime/text()");
    pugi::xpath_query posContent_query("content/text()");

    pugi::xpath_node_set xpathPosFrames= m_xmlDoc.select_nodes(posFrame_query);

    for (pugi::xpath_node xpathPosFrame : xpathPosFrames)
    {
        pugi::xml_node content = xpathPosFrame.node();
        pugi::xml_node posDateTime = content.select_node(posDateTime_query).node();
        pugi::xml_node posContent = content.select_node(posContent_query).node();

        parseData = posContent.value();
        //SPDLOG_INFO("=====> (parse): {}", QString::fromStdString(posContent.value()).trimmed().toStdString().c_str());
        break;
    }


    std::string encodingType = "";

#ifndef MEDIA_FILE_ONLY
    // 2023.01.03. coverity
    auto db = QCoreServiceManager::Instance().DB();
    if (db == nullptr) {
        return "";
    }

    // 2023.01.02. coverity
    std::map<Wisenet::uuid_string, Wisenet::Device::Device> devices;
    if (db != nullptr) {
        devices = db->GetDevices();
    }

    for (auto& device : devices)
    {
        for (auto& confdata : m_confData)
        {
            if (confdata.enable)
            {
                encodingType = confdata.encodingType;
                break;
            }
        }

        if (!encodingType.empty())
            break;
    }
#endif

    QString convertValue = "";

    if (encodingType == "US-ASCII")
        convertValue = QString::fromLocal8Bit(parseData.c_str());
    else if (encodingType == "UTF-8" || encodingType == "UTF-16")
        convertValue = QString::fromLocal8Bit(parseData.c_str());
    else if (encodingType == "EUC-KR" || encodingType == "ISO-2022-KR")
        convertValue = QString::fromUtf8(parseData.c_str());
    else if (encodingType == "EUC-JP" || encodingType == "ISO-2022-JP" || encodingType == "SHIFT-JIS")
        convertValue = QString::fromUtf8(parseData.c_str());
    else if (encodingType == "EUC-CN" || encodingType == "ISO-2022-CN" || encodingType == "BIG5")
        convertValue = QString::fromUtf8(parseData.c_str());
    else
        convertValue = QString::fromUtf8(parseData.c_str());

    return convertValue;
}

void PosMetaManager::setPosConfData(std::vector<Wisenet::Device::TextPosConfResult> confData)
{
    m_confData.clear();

    m_confData = confData;
}
