/*********************************************************************************
 * Copyright(c) 2023 Hanwha Vision Co., Ltd.  All rights reserved.
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
#include "TextItemViewModel.h"
#include "LogSettings.h"
#include "QLocaleManager.h"

TextItemViewModel::TextItemViewModel(QObject *parent)
    : QAbstractListModel(parent)
{
    ;
}

TextItemViewModel::~TextItemViewModel()
{

}

int TextItemViewModel::rowCount(const QModelIndex &) const
{
    return m_textData.size();
}

int TextItemViewModel::columnCount(const QModelIndex &) const
{
    return 0;
}

QHash<int, QByteArray> TextItemViewModel::roleNames() const
{
    return {
        { Qt::DisplayRole, "displayRole" },
        { HoveredRole, "hoveredRole" },
        { TextDataIdRole, "textDataIdRole" },
        { DeviceNameIdRole, "deviceNameIdRole" },
        { PosNameIdRole, "posNameIdRole" },
        { DateTimeIdRole, "dateTimeIdRole" },
        { ChannelIdRole, "channelIdRole" },
        { ChannelIdsRole, "channelIdsRole" },
        { BookmarkIdRole, "bookmarkIdRole"},
        { IsDeletedChannelRole, "isDeletedChannelRole"}
    };
}

QVariant TextItemViewModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    switch (role)
    {
    case DateTimeIdRole:
        return getDataPlayTimeIdRole(index);
//    case HoveredRole:
//        if(m_hoveredRow == index.row())
//            return true;
//        return false;
    case DeviceNameIdRole:
        return getDataDeviceNameIdRole(index);
    case PosNameIdRole:
        return getDataPosNameIdRole(index);
    case ChannelIdRole:
        return getDataChannelIdRole(index);
    case ChannelIdsRole:
        return getDataChannelIdsRole(index);
    case TextDataIdRole:
        return getDataTextIdRole(index);
    case BookmarkIdRole:
    case IsDeletedChannelRole:
    default:
        return QVariant();
    }

    return QVariant();
}

QVariant TextItemViewModel::getDataPlayTimeIdRole(const QModelIndex &index) const
{
    QString playTime = QString::fromStdString(QLocaleManager::Instance()->getDateTimeFromSec(std::stoi(m_textData[index.row()].playTime)).toStdString());

    const QVariant& cellValue = playTime;

    if (cellValue.isNull())
        return QVariant();
    else
        return cellValue;
}

QVariant TextItemViewModel::getDataDeviceNameIdRole(const QModelIndex &index) const
{
    // 2022.12.28. coverity
    auto db = QCoreServiceManager::Instance().DB();
    if (db == nullptr) {
        return QVariant();
    }

    // 2023.01.03. coverity
    bool found = false;
    Wisenet::Device::Device device;    
    if (db != nullptr) {
        found = db->FindDevice(m_textData[index.row()].uuid, device);
    }

    if (!found) {
        return QVariant();
    }

    const QVariant& cellValue = QString::fromUtf8(device.deviceName.c_str());

    if (cellValue.isNull())
        return QVariant();
    else
        return cellValue;
}

QVariant TextItemViewModel::getDataPosNameIdRole(const QModelIndex &index) const
{
    auto channelIDList = m_textData[index.row()].channelIDList;

    QString posName = "";

    for (auto& data : m_confData)
    {
        if (std::string::npos != channelIDList.find(data.deviceId))
        {
            if (data.enable)
            {
                posName = QString::fromUtf8(data.deviceName.c_str());
                break;
            }
        }
    }

    const QVariant& cellValue = posName;

    if (cellValue.isNull())
        return QVariant();
    else
        return cellValue;
}

QVariant TextItemViewModel::getDataChannelIdsRole(const QModelIndex &index) const
{
    std::vector<std::string> chList;
    std::istringstream ss(m_textData[index.row()].channelIDList);
    std::string subs;
    while (getline(ss, subs,','))
    {
        chList.push_back(subs);
    }

    int chListSize = chList.size();

    const QVariant& cellValue = QString::number(chListSize);

    if (cellValue.isNull())
        return QVariant();
    else
        return cellValue;
}

QVariant TextItemViewModel::getDataChannelIdRole(const QModelIndex &index) const
{
    // 2022.12.28. coverity
    auto db = QCoreServiceManager::Instance().DB();
    if (db == nullptr) {
        return QVariant();
    }

    // 2023.01.03. coverity
    std::string encodingType = "";
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

    QString convertValue = "";

    if (encodingType == "US-ASCII")
        convertValue = QString::fromLocal8Bit(m_textData[index.row()].textData.c_str());
    else if (encodingType == "UTF-8" || encodingType == "UTF-16")
        convertValue = QString::fromLocal8Bit(m_textData[index.row()].textData.c_str());
    else if (encodingType == "EUC-KR" || encodingType == "ISO-2022-KR")
        convertValue = QString::fromUtf8(m_textData[index.row()].textData.c_str());
    else if (encodingType == "EUC-JP" || encodingType == "ISO-2022-JP" || encodingType == "SHIFT-JIS")
        convertValue = QString::fromUtf8(m_textData[index.row()].textData.c_str());
    else if (encodingType == "EUC-CN" || encodingType == "ISO-2022-CN" || encodingType == "BIG5")
        convertValue = QString::fromUtf8(m_textData[index.row()].textData.c_str());
    else
        convertValue = QString::fromUtf8(m_textData[index.row()].textData.c_str());

    const QVariant& cellValue = convertValue;


    if (cellValue.isNull())
        return QVariant();
    else
        return cellValue;
}

QVariant TextItemViewModel::getDataTextIdRole(const QModelIndex &index) const
{
    // 2022.12.28. coverity
    auto db = QCoreServiceManager::Instance().DB();
    if (db == nullptr) {
        return QVariant();
    }

    // 2023.01.03. coverity
    std::string encodingType = "";
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

    QString convertValue = "";

    if (encodingType == "US-ASCII")
        convertValue = QString::fromLocal8Bit(m_textData[index.row()].textData.c_str());
    else if (encodingType == "UTF-8" || encodingType == "UTF-16")
        convertValue = QString::fromLocal8Bit(m_textData[index.row()].textData.c_str());
    else if (encodingType == "EUC-KR" || encodingType == "ISO-2022-KR")
        convertValue = QString::fromUtf8(m_textData[index.row()].textData.c_str());
    else if (encodingType == "EUC-JP" || encodingType == "ISO-2022-JP" || encodingType == "SHIFT-JIS")
        convertValue = QString::fromUtf8(m_textData[index.row()].textData.c_str());
    else if (encodingType == "EUC-CN" || encodingType == "ISO-2022-CN" || encodingType == "BIG5")
        convertValue = QString::fromUtf8(m_textData[index.row()].textData.c_str());
    else
        convertValue = QString::fromUtf8(m_textData[index.row()].textData.c_str());

    const QVariant& cellValue = convertValue;

    if (cellValue.isNull())
        return QVariant();
    else
        return cellValue;
}

void TextItemViewModel::setPosConfData(std::vector<Wisenet::Device::TextPosConfResult> confData)
{
    m_confData.clear();

    m_confData = confData;
}

void TextItemViewModel::setTextItems(std::vector<Wisenet::Device::TextSearchDataResult> textItems)
{
    beginResetModel();

    m_textData.clear();

    m_textData = textItems;

    endResetModel();

    SPDLOG_INFO("TextItemViewModel::setTextItems size = {}", textItems.size());
}

QString TextItemViewModel::getDeviceID(QString deviceName)
{
    // 2022.12.28. coverity
    auto db = QCoreServiceManager::Instance().DB();
    if (db == nullptr) {
        return QString("");
    }

    QString result = "";

    // 2023.01.03. coverity
    std::string encodingType = "";
    std::map<Wisenet::uuid_string, Wisenet::Device::Device> devices;
    if (db != nullptr) {
        devices = db->GetDevices();
    }

    for (auto& device : devices)
    {
        if (device.second.deviceName == deviceName.toStdString())
        {
            result = QString::fromStdString(device.second.deviceID);

            break;
        }
    }

    return result;
}

qint64 TextItemViewModel::getServiceTime(QString deviceTime)
{
    qint64 result = 0;

    for (auto& info : m_textData)
    {
        auto timestamp = QString::fromStdString(QLocaleManager::Instance()->getDateTimeFromSec(std::stoi(info.playTime, nullptr, 10)).toStdString());

        if (timestamp == deviceTime)
        {
            result = (qint64)std::stoi(info.playTime);
            break;
        }
    }

    return result;
}

QString TextItemViewModel::convertTimestampToDatetime(qint64 timestamp)
{
    auto playTime = QString::number(timestamp).toStdString();
    return QString::fromStdString(QLocaleManager::Instance()->getDateTimeFromSec(std::stoi(playTime)).toStdString());
}
