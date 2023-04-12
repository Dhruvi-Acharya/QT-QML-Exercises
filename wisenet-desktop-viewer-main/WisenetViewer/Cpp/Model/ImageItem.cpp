#include "ImageItem.h"

ImageItem::ImageItem(QQuickItem *parent) : QQuickPaintedItem(parent)
{
}

void ImageItem::paint(QPainter *painter)
{
    QRectF bounding_rect = boundingRect();
    QImage scaled;
    QPointF center;

    if(this->current_image.height() > this->current_image.width())
    {
        scaled = this->current_image.scaledToHeight(bounding_rect.height());

        if(scaled.width() > bounding_rect.width())
            scaled = scaled.scaledToWidth(bounding_rect.width());

        center = bounding_rect.center() - scaled.rect().center();
    }
    else
    {
        scaled = this->current_image.scaledToWidth(bounding_rect.width());

        if(scaled.height() > bounding_rect.height())
            scaled = scaled.scaledToHeight(bounding_rect.height());

        center = bounding_rect.center() - scaled.rect().center();
    }

    if(center.x() < 0)
        center.setX(0);
    if(center.y() < 0)
        center.setY(0);
    painter->drawImage(center, scaled);
}

QImage ImageItem::image() const
{
    return this->current_image;
}

void ImageItem::setImage(const QImage &image)
{
    this->current_image = image;
    update();
}
