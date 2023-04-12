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

#include "ExportProgressItem.h"

ExportProgressItem::ExportProgressItem(const QString name, const int progress, const QString filePath, void* exportMediaItemPtr)
    : m_name(name)
    , m_progress(progress)
    , m_filePath(filePath)
    , m_exportMediaItemPtr(exportMediaItemPtr)
{

}

ExportProgressItem::~ExportProgressItem()
{

}

void ExportProgressItem::onProgressChanged(int progress)
{
    m_progress = progress;
    emit progressChanged(this);
}

void ExportProgressItem::onEndExport(void* item, bool isFailed)
{
    m_failed = isFailed;
    emit failedChanged(this);
}




