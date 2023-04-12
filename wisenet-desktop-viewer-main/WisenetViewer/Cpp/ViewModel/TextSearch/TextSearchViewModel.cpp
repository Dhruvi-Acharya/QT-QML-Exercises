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
#include "TextSearchViewModel.h"
#include "LogSettings.h"
#include "QLocaleManager.h"
#include "PosMetaManager.h"

TextSearchViewModel::TextSearchViewModel(QObject *parent)
    : QAbstractTableModel(parent)
    , m_hoveredRow(-1)
    , m_totalCount(0)
    , m_sortIndex(1)
    , m_sortOrder(Qt::DescendingOrder)
{
    SPDLOG_DEBUG("TextSearchViewModel::TextSearchViewModel");
}

TextSearchViewModel::~TextSearchViewModel()
{

}

int TextSearchViewModel::rowCount(const QModelIndex &) const
{
    return m_textData.size();
}

int TextSearchViewModel::columnCount(const QModelIndex &) const
{
    return 7;
}

QVariant TextSearchViewModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    switch (role)
    {
    case DateTimeIdRole:
        return getDataPlayTimeIdRole(index);
    case HoveredRole:
        if(m_hoveredRow == index.row())
            return true;
        return false;
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
//    case BookmarkIdRole:
//    case IsDeletedChannelRole:
//    default:
//        return QVariant();
    }

    return QVariant();
}

QVariant TextSearchViewModel::getDataDeviceNameIdRole(const QModelIndex &index) const
{
    auto db = QCoreServiceManager::Instance().DB();
    if (db == nullptr) {
        return QVariant();
    }

    Wisenet::Device::Device device;

    if (!db->FindDevice(m_textData[index.row()].uuid, device))
    {
        return QVariant();
    }

    const QVariant& cellValue = QString::fromUtf8(device.deviceName.c_str());

    if (cellValue.isNull())
        return QVariant();
    else
        return cellValue;
}

QVariant TextSearchViewModel::getDataPosNameIdRole(const QModelIndex &index) const
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

QVariant TextSearchViewModel::getDataChannelIdsRole(const QModelIndex &index) const
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

QVariant TextSearchViewModel::getDataChannelIdRole(const QModelIndex &index) const
{
    const QVariant& cellValue = QString::fromUtf8(m_textData[index.row()].channelIDList.c_str());

    if (cellValue.isNull())
        return QVariant();
    else
        return cellValue;
}

QVariant TextSearchViewModel::getDataPlayTimeIdRole(const QModelIndex &index) const
{
    QString playTime = QString::fromStdString(QLocaleManager::Instance()->getDateTimeFromSec(std::stoi(m_textData[index.row()].playTime)).toStdString());

    const QVariant& cellValue = playTime;

    if (cellValue.isNull())
        return QVariant();
    else
        return cellValue;
}

QVariant TextSearchViewModel::getDataTextIdRole(const QModelIndex &index) const
{
    std::string encodingType = "";

    for (auto& device : m_devices)
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

void TextSearchViewModel::setHoveredRow(int rowNum, bool hovered)
{
    QVector<int> role;
    role << Role::HoveredRole;

    int last = m_hoveredRow;
    if (hovered) {
        m_hoveredRow = rowNum;
        if(last > m_hoveredRow)
            emit dataChanged(QModelIndex(index(m_hoveredRow, 0)), QModelIndex(index(last, 5)), role);
        else
            emit dataChanged(QModelIndex(index(last, 0)), QModelIndex(index(m_hoveredRow, 5)), role);
    }
    else {
        if (rowNum == m_hoveredRow) {
            m_hoveredRow = -1;
            emit dataChanged(QModelIndex(index(rowNum, 0)), QModelIndex(index(rowNum, 5)), role);
        }
    }
}

void TextSearchViewModel::sort(int column, Qt::SortOrder order)
{
    m_sortIndex = column;
    m_sortOrder = order;
}

QString TextSearchViewModel::getDeviceID(QString deviceName)
{
    auto db = QCoreServiceManager::Instance().DB();
    if (db == nullptr)
    {
        return QString("");
    }

    QString result = "";

    // 2022.12.29. coverity
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

qint64 TextSearchViewModel::getServiceTime(QString deviceTime)
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

void TextSearchViewModel::setTextData(std::vector<Wisenet::Device::TextSearchDataResult> textData)
{
    beginResetModel();

    m_textData.clear();

    m_textData = textData;

    endResetModel();
}

QHash<int, QByteArray> TextSearchViewModel::roleNames() const
{
    return {
        { Qt::DisplayRole, "displayRole" },
        { HoveredRole, "hoveredRole" },
        { DateTimeIdRole, "dateTimeIdRole" },
        { DeviceNameIdRole, "deviceNameIdRole" },
        { PosNameIdRole, "posNameIdRole" },
        { ChannelIdRole, "channelIdRole" },
        { ChannelIdsRole, "channelIdsRole" },
        { TextDataIdRole, "textDataIdRole" },
        { BookmarkIdRole, "bookmarkIdRole"},
        { IsDeletedChannelRole, "isDeletedChannelRole"}
    };
}

void TextSearchViewModel::initialize()
{
    auto db = QCoreServiceManager::Instance().DB();
    if (db == nullptr)
    {
        return;
    }

    // 사이즈가 있는 경우, 이미 등록 장비 정보를 획득한 것으로 취급
    if (m_devices.size())
    {
        return;
    }

    // Text Search - PosConf
    m_devices = db->GetDevices();

    for (auto& device : m_devices)
    {
        searchPosConf(device.first);
    }
}

bool TextSearchViewModel::search(bool allDevice, QStringList channels, const QString& keyword, const QDateTime &from, const QDateTime &to, const bool &isWholeWord, const bool &isCaseSensitive)
{
    if (!m_devices.size())
    {
        m_totalCount = 0;
        emit totalCountChanged();
        return false;
    }

    emit searchStarted();

    // Text Search - Metadata - Search Token / Search Status / Search Result
    if (allDevice)
    {
        SPDLOG_INFO("=====> all Device Select");

        for (auto& device : m_devices)
        {
            for (auto& confdata : m_confData)
            {
                if (confdata.enable)
                {
                    //SPDLOG_INFO("=====> {}, {}, {}", device.first, std::to_string(std::stoi(confdata.deviceId) + 1), confdata.enable);
                    search(device.first, std::to_string(std::stoi(confdata.deviceId) + 1), keyword, from, to, isWholeWord, isCaseSensitive);
                }
            }
        }
    }
    else
    {
        for (auto& channel : channels)
        {
            QStringList ids = channel.split('_');

            //SPDLOG_INFO("=====> device={}, ch={}", ids[0].toStdString(), ids[1].toStdString());
            search(ids[0].toStdString(), ids[1].toStdString(), keyword, from, to, isWholeWord, isCaseSensitive);
        }
    }

    return true;
}

void TextSearchViewModel::searchPosConf(const std::string deviceID)
{
    auto deviceTextPosConfRequest = std::make_shared<Wisenet::Device::DeviceTextPosConfRequest>();

    deviceTextPosConfRequest->deviceID = deviceID.c_str();

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::DeviceTextPosConf,
                this, deviceTextPosConfRequest,
                [=](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::Device::DeviceTextPosConfResponse>(reply->responseDataPtr);

        // 2022.12.29. coverity
        if (response == nullptr) {
            //SPDLOG_INFO("TextSearchViewModel::searchPosConf, isFailed={}", response->isFailed());
            return;
        }

        if (response->isFailed())
        {
            SPDLOG_INFO("TextSearchViewModel::searchPosConf, isFailed={}", response->isFailed());
        }
        else
        {
            SPDLOG_INFO("TextSearchViewModel::searchPosConf, isSuccess={}", response->isSuccess());

            // 업데이트 - POS Config 결과 (POS 이름 구하기 위해서 사용)
            m_confData.clear();

            m_confData = response->confData;

            PosMetaManager::Instance()->setPosConfData(response->confData);
        }
    });
}

void TextSearchViewModel::search(const std::string deviceID, const std::string chID, const QString& keyword, const QDateTime &from, const QDateTime &to, const bool &isWholeWord, const bool &isCaseSensitive)
{
#if 0
    SPDLOG_DEBUG("TextSearchViewModel::search => Request => deviceID = {}", deviceID.c_str());
    SPDLOG_DEBUG("TextSearchViewModel::search => Request => chID = {}", chID.c_str());
    SPDLOG_DEBUG("TextSearchViewModel::search => Request => Keyword = {}", keyword.toStdString().c_str());
    SPDLOG_DEBUG("TextSearchViewModel::search => Request => FromDate = {}", from.toString("yyyy-MM-ddTHH:mm:ssZ").toStdString().c_str());
    SPDLOG_DEBUG("TextSearchViewModel::search => Request => ToDate = {}", to.toString("yyyy-MM-ddTHH:mm:ssZ").toStdString().c_str());
    SPDLOG_DEBUG("TextSearchViewModel::search => Request => IsWholeWord = {}", isWholeWord);
    SPDLOG_DEBUG("TextSearchViewModel::search => Request => IsCaseSensitive = {}", isCaseSensitive);
#endif

    auto deviceTextSearchRequest = std::make_shared<Wisenet::Device::DeviceTextSearchRequest>();

    deviceTextSearchRequest->deviceID = deviceID.c_str();
    deviceTextSearchRequest->mode = "Start";
    deviceTextSearchRequest->metaDataType = "POS";
    deviceTextSearchRequest->deviceIDList = chID.c_str();
    deviceTextSearchRequest->overlappedID = "-1";
    deviceTextSearchRequest->keyword = keyword.toStdString();
    deviceTextSearchRequest->isWholeword = isWholeWord;
    deviceTextSearchRequest->isCaseSensitive = isCaseSensitive;
    deviceTextSearchRequest->fromDate = from.toUTC().toMSecsSinceEpoch();
    deviceTextSearchRequest->toDate = to.toUTC().toMSecsSinceEpoch();

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::DeviceTextSearch,
                this, deviceTextSearchRequest,
                [=](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::Device::DeviceTextSearchResultResponse>(reply->responseDataPtr);
        if (response != nullptr || response->isFailed())
        {
            SPDLOG_INFO("TextSearchViewModel::search, isFailed={}", response->isFailed());

            // 업데이트 - 검색 갯수
            m_totalCount = 0;
            emit totalCountChanged();
        }
        else
        {
            SPDLOG_INFO("TextSearchViewModel::search, isSuccess={}", response->isSuccess());
#if 0
            SPDLOG_DEBUG("TextSearchViewModel::search => Response => searchToken={}", response->searchToken);
            SPDLOG_DEBUG("TextSearchViewModel::search => Response => searchTokenExpirytime={}", response->searchTokenExpirytime);
            SPDLOG_DEBUG("TextSearchViewModel::search => Response => totalCount={}", response->totalCount);
            SPDLOG_DEBUG("TextSearchViewModel::search => Response => totalResultsFound={}", response->totalResultsFound);
            SPDLOG_DEBUG("TextSearchViewModel::search => Response => intervalFrom={}", response->intervalFrom);
            SPDLOG_DEBUG("TextSearchViewModel::search => Response => intervalTo={}", response->intervalTo);

            for (auto& data : response->textData)
            {
                SPDLOG_DEBUG("TextSearchViewModel::search => Response => deviceID={}, textData={}, channelIDList={}, date={}, keywordsMatched={}, playTime={}, uuid={}",
                          data.deviceID,
                          data.textData,
                          data.channelIDList,
                          data.date,
                          data.keywordsMatched,
                          data.playTime,
                          data.uuid);
            }
#endif
            // 업데이트 - 검색 갯수
            if (response->textData.size() > 0)
                m_totalCount = response->totalResultsFound;
            else
                m_totalCount = 0;

            emit totalCountChanged();

            // 업데이트 - 검색 결과
            m_textItemViewModel.setTextItems(response->textData);

            setTextData(response->textData);
            emit searchFinished();
        }
    });
}
