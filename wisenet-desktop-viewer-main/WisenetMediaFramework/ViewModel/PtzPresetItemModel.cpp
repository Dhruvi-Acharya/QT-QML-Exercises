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
#include "PtzPresetItemModel.h"
#include <QDebug>
#include "LogSettings.h"

PtzPresetItemModel::PtzPresetItemModel(QObject *parent)
    : QObject(parent)
{

}

PtzPresetItemModel::~PtzPresetItemModel()
{
    SPDLOG_DEBUG("PtzPresetItemModel::~PtzPresetItemModel()");
}

int PtzPresetItemModel::presetNo() const
{
    return m_presetNo;
}

void PtzPresetItemModel::setPresetNo(const int no)
{
    if (m_presetNo != no) {
        m_presetNo = no;
    }
}

QString PtzPresetItemModel::presetName() const
{
    return m_presetName;
}

void PtzPresetItemModel::setPresetName(const QString &name)
{
    if (m_presetName != name) {
        m_presetName = name;
    }
}
