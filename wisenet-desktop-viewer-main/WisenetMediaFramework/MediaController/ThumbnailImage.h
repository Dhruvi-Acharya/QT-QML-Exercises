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

#ifndef THUMBNAILIMAGE_H
#define THUMBNAILIMAGE_H

#include <QQuickPaintedItem>
#include <QQuickItem>
#include <QPainter>
#include <QImage>
#include <QDebug>

class ThumbnailImage : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QImage image READ image WRITE setImage NOTIFY imageChanged)
    Q_PROPERTY(bool stretch READ stretch WRITE setStretch NOTIFY stretchChanged)

public:
    ThumbnailImage(QQuickItem *parent = nullptr);
    QImage image() const;
    bool stretch() const;
    void setImage(const QImage &image);
    void setStretch(const bool stretch);
    void paint(QPainter *painter);

signals:
    void imageChanged();
    void stretchChanged();

private:
    QImage current_image;
    bool m_stretch = false;
};

#endif // THUMBNAILIMAGE_H
