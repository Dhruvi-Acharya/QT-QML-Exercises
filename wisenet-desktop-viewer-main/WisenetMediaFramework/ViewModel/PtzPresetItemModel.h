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
#include <QString>
#include <QUrl>
#include <QRectF>

class PtzPresetItemModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int presetNo READ presetNo)
    Q_PROPERTY(QString presetName READ presetName)

public:
    explicit PtzPresetItemModel(QObject *parent = nullptr);
    ~PtzPresetItemModel();

    int presetNo() const;
    void setPresetNo(const int no);

    QString presetName() const;
    void setPresetName(const QString& name);

private:
    int m_presetNo = 0;
    QString m_presetName;
};
