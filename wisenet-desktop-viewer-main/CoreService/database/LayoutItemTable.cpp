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
#include "LayoutItemTable.h"

#include <QString>
#include <QVariant>

#include <boost/core/ignore_unused.hpp>
#include "CoreServiceLogSettings.h"


namespace Wisenet
{
namespace Core
{

LayoutItemTable::LayoutItemTable(QSqlDatabase& database)
    :m_database(database)
{

}

void LayoutItemTable::CreateTable()
{
    if(m_database.tables().contains(QLatin1String("T_LAYOUT_ITEM"))) {
        SPDLOG_INFO("[T_LAYOUT_ITEM] Already exists.");
        return;
    }

    QSqlQuery query(m_database);

    QString queryString = "";
    queryString += "CREATE TABLE [T_LAYOUT_ITEM](";
    queryString += "[LAYOUT_ITEM_ID] VARCHAR(40) NOT NULL UNIQUE PRIMARY KEY";
    queryString += ",[LAYOUT_ID]VARCHAR(40) NOT NULL";
    queryString += ",[ITEM_TYPE] INTEGER NOT NULL DEFAULT 0";
    queryString += ",[ITEM_ID] TEXT ";
    queryString += ",[PARENT_ID]VARCHAR(40) ";
    queryString += ",[COLUMN] INTEGER NOT NULL ";
    queryString += ",[COLUMN_COUNT] INTEGER NOT NULL ";
    queryString += ",[ROW] INTEGER NOT NULL ";
    queryString += ",[ROW_COUNT] INTEGER NOT NULL ";
    queryString += ",[ROTATION] INTEGER NOT NULL";
    queryString += ",[DISPLAY_INFO] BOOLEAN DEFAULT 0";
    queryString += ",[ZOOM_TARGET_NX] REAL";
    queryString += ",[ZOOM_TARGET_NY] REAL";
    queryString += ",[ZOOM_TARGET_NWIDTH] REAL";
    queryString += ",[ZOOM_TARGET_NHEIGHT] REAL";
    queryString += ",[ZOOM_TARGET_ID] VARCHAR(40)";
    queryString += ",[BRIGHTNESS] REAL";
    queryString += ",[CONTRAST] REAL";
    queryString += ",[IMAGE_BACKGROUND] INTEGER NOT NULL DEFAULT 1";
    queryString += ",[IMAGE_OPACITY] REAL";
    queryString += ",[FILL_MODE] INTEGER NOT NULL DEFAULT 0";
    queryString += ",[STREAM_PROFILE] INTEGER NOT NULL DEFAULT 0";
    queryString += ",[FISHEYE_DEWARP_ENABLE] BOOLEAN DEFAULT 0";
    queryString += ",[FISHEYE_VIEW_MODE] INTEGER NOT NULL DEFAULT 0";
    queryString += ",[FISHEYE_VIEW_PARAM] TEXT";
    queryString += ",[FISHEYE_FILE_ENABLE] BOOLEAN DEFAULT 0";
    queryString += ",[FISHEYE_FILE_LENSTYPE] VARCHAR(40) ";
    queryString += ",[FISHEYE_FILE_LOCATION] INTEGER NOT NULL DEFAULT 0";
    queryString += ")";

    if(!query.exec(queryString)) {

        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

}

void LayoutItemTable::Get(const uuid_string &layoutID, std::vector<LayoutItem> &layoutItems)
{
    QSqlQuery query(m_database);

    query.prepare("SELECT * FROM [T_LAYOUT_ITEM] WHERE LAYOUT_ID=?");
    query.addBindValue(QString(layoutID.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    while(query.next()) {
        LayoutItem layoutItem;
        layoutItem.itemID = query.value(0).toString().toStdString();
        layoutItem.itemType = static_cast<LayoutItem::LayoutItemType>(query.value(2).toInt());
        if((LayoutItem::LayoutItemType::Camera == layoutItem.itemType)
                || (LayoutItem::LayoutItemType::ZoomTarget == layoutItem.itemType)) {
            layoutItem.channelID = query.value(3).toString().toStdString();
            layoutItem.deviceID = query.value(4).toString().toStdString();
        }else if(LayoutItem::LayoutItemType::Webpage == layoutItem.itemType) {
            layoutItem.webpageID = query.value(3).toString().toStdString();
        }else if(LayoutItem::LayoutItemType::LocalResource == layoutItem.itemType ||
                 LayoutItem::LayoutItemType::Image == layoutItem.itemType) {
            layoutItem.resourcePath = query.value(3).toString().toStdString();
        }

        layoutItem.column = query.value(5).toInt();
        layoutItem.columnCount = query.value(6).toInt();
        layoutItem.row = query.value(7).toInt();
        layoutItem.rowCount = query.value(8).toInt();
        layoutItem.rotation = query.value(9).toInt();
        layoutItem.displayInfo = query.value(10).toBool();
        layoutItem.zoomTargetParam.normX = query.value(11).toFloat();
        layoutItem.zoomTargetParam.normY = query.value(12).toFloat();
        layoutItem.zoomTargetParam.normWidth = query.value(13).toFloat();
        layoutItem.zoomTargetParam.normHeight = query.value(14).toFloat();
        layoutItem.zoomTargetParam.zoomTargetID = query.value(15).toString().toStdString();
        layoutItem.bncParam.brightness = query.value(16).toFloat();
        layoutItem.bncParam.contrast = query.value(17).toFloat();
        layoutItem.imageParam.isBackground = query.value(18).toBool();
        layoutItem.imageParam.opacity = query.value(19).toFloat();
        layoutItem.fillMode = static_cast<LayoutItem::FillMode>(query.value(20).toInt());
        layoutItem.profile = static_cast<LayoutItem::StreamProfile>(query.value(21).toInt());

        /* fisheye settings */
        layoutItem.fisheyeDewarpParam.dewarpEnable = query.value(22).toBool();
        layoutItem.fisheyeDewarpParam.viewMode = static_cast<FisheyeViewMode>(query.value(23).toInt());
        std::string viewParam = query.value(24).toString().toStdString();
        JsonReader reader(viewParam.c_str());
        reader & layoutItem.fisheyeDewarpParam.viewParam;
        layoutItem.fisheyeDewarpParam.fileDewarpEnable = query.value(25).toBool();
        layoutItem.fisheyeDewarpParam.fileLensType = query.value(26).toString().toStdString();
        layoutItem.fisheyeDewarpParam.fileLensLocation = static_cast<FisheyeLensLocation>(query.value(27).toInt());

        layoutItems.emplace_back(layoutItem);
    }
}

void LayoutItemTable::Add(const uuid_string& layoutID, const LayoutItem &layoutItem)
{
    QSqlQuery query(m_database);

    QString queryString = "";
    queryString += "INSERT INTO [T_LAYOUT_ITEM]";
    queryString += "(LAYOUT_ITEM_ID,LAYOUT_ID,ITEM_TYPE,ITEM_ID,"
                   "PARENT_ID,COLUMN,COLUMN_COUNT,ROW,ROW_COUNT,ROTATION,DISPLAY_INFO,"
                   "ZOOM_TARGET_NX,ZOOM_TARGET_NY,ZOOM_TARGET_NWIDTH,ZOOM_TARGET_NHEIGHT,ZOOM_TARGET_ID,"
                   "BRIGHTNESS,CONTRAST,IMAGE_BACKGROUND,IMAGE_OPACITY,FILL_MODE,STREAM_PROFILE,"
                   "FISHEYE_DEWARP_ENABLE,FISHEYE_VIEW_MODE,FISHEYE_VIEW_PARAM,FISHEYE_FILE_ENABLE,"
                   "FISHEYE_FILE_LENSTYPE,FISHEYE_FILE_LOCATION) ";
    queryString += "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
    query.prepare(queryString);

    query.addBindValue(QString(layoutItem.itemID.c_str()));
    query.addBindValue(QString(layoutID.c_str()));
    query.addBindValue(static_cast<int>(layoutItem.itemType));

    if((LayoutItem::LayoutItemType::Camera == layoutItem.itemType)
            || (LayoutItem::LayoutItemType::ZoomTarget == layoutItem.itemType)) {
        query.addBindValue(QString(layoutItem.channelID.c_str()));
        query.addBindValue(QString(layoutItem.deviceID.c_str()));
    }else if(LayoutItem::LayoutItemType::Webpage == layoutItem.itemType) {
        query.addBindValue(QString(layoutItem.webpageID.c_str()));
        query.addBindValue(QString(""));
    }else if(LayoutItem::LayoutItemType::LocalResource == layoutItem.itemType ||
             LayoutItem::LayoutItemType::Image == layoutItem.itemType) {
        query.addBindValue(QString(layoutItem.resourcePath.c_str()));
        query.addBindValue(QString(""));
    }

    query.addBindValue(layoutItem.column);
    query.addBindValue(layoutItem.columnCount);
    query.addBindValue(layoutItem.row);
    query.addBindValue(layoutItem.rowCount);
    query.addBindValue(layoutItem.rotation);
    query.addBindValue(layoutItem.displayInfo);
    query.addBindValue(layoutItem.zoomTargetParam.normX);
    query.addBindValue(layoutItem.zoomTargetParam.normY);
    query.addBindValue(layoutItem.zoomTargetParam.normWidth);
    query.addBindValue(layoutItem.zoomTargetParam.normHeight);
    query.addBindValue(QString(layoutItem.zoomTargetParam.zoomTargetID.c_str()));
    query.addBindValue(layoutItem.bncParam.brightness);
    query.addBindValue(layoutItem.bncParam.contrast);
    query.addBindValue(layoutItem.imageParam.isBackground);
    query.addBindValue(layoutItem.imageParam.opacity);
    query.addBindValue(static_cast<int>(layoutItem.fillMode));
    query.addBindValue(static_cast<int>(layoutItem.profile));
    query.addBindValue(layoutItem.fisheyeDewarpParam.dewarpEnable);
    query.addBindValue(static_cast<int>(layoutItem.fisheyeDewarpParam.viewMode));

    JsonWriter writer;
    LayoutItem::FisheyeViewParam viewParam = layoutItem.fisheyeDewarpParam.viewParam;
    writer & viewParam;
    query.addBindValue(QString(writer.GetString()));

    query.addBindValue(layoutItem.fisheyeDewarpParam.fileDewarpEnable);
    query.addBindValue(QString(layoutItem.fisheyeDewarpParam.fileLensType.c_str()));
    query.addBindValue(static_cast<int>(layoutItem.fisheyeDewarpParam.fileLensLocation));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void LayoutItemTable::Remove(const uuid_string &layoutID)
{
    QSqlQuery query(m_database);

    query.prepare("DELETE FROM [T_LAYOUT_ITEM] WHERE LAYOUT_ID=?");
    query.addBindValue(QString(layoutID.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void LayoutItemTable::RemoveByParentID(const uuid_string &parentID, std::set<uuid_string> &changedLayoutIDs)
{
    QSqlQuery query(m_database);

    //삭제항목 확인
    query.prepare("SELECT * FROM [T_LAYOUT_ITEM] WHERE PARENT_ID=?");
    query.addBindValue(QString(parentID.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    while(query.next()) {
        changedLayoutIDs.emplace(query.value(1).toString().toStdString());
    }

    if(changedLayoutIDs.empty()){
        return;
    }

    query.prepare("DELETE FROM [T_LAYOUT_ITEM] WHERE PARENT_ID=?");
    query.addBindValue(QString(parentID.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void LayoutItemTable::RemoveByChannelID(const std::string &deviceId, const std::string channelId, std::set<uuid_string> &changedLayoutIDs)
{
    QSqlQuery query(m_database);

    //삭제항목 확인
    query.prepare("SELECT * FROM [T_LAYOUT_ITEM] WHERE PARENT_ID=? AND ITEM_ID=?");
    query.addBindValue(QString(deviceId.c_str()));
    query.addBindValue(QString(channelId.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    while(query.next()) {
        changedLayoutIDs.emplace(query.value(1).toString().toStdString());
    }

    if(changedLayoutIDs.empty()){
        return;
    }

    query.prepare("DELETE FROM [T_LAYOUT_ITEM] WHERE PARENT_ID=? AND ITEM_ID=?");
    query.addBindValue(QString(deviceId.c_str()));
    query.addBindValue(QString(channelId.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void LayoutItemTable::RemoveByItemID(const std::string itemId, std::set<uuid_string> &changedLayoutIDs)
{
    QSqlQuery query(m_database);

    //삭제항목 확인
    query.prepare("SELECT * FROM [T_LAYOUT_ITEM] WHERE ITEM_ID=?");
    query.addBindValue(QString(itemId.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    while(query.next()) {
        changedLayoutIDs.emplace(query.value(1).toString().toStdString());
    }

    if(changedLayoutIDs.empty()){
        return;
    }

    query.prepare("DELETE FROM [T_LAYOUT_ITEM] WHERE ITEM_ID=?");
    query.addBindValue(QString(itemId.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

}
}
