#include "ThumbnailImage.h"

ThumbnailImage::ThumbnailImage(QQuickItem *parent) : QQuickPaintedItem(parent)
{
}

void ThumbnailImage::paint(QPainter *painter)
{
    QRectF bounding_rect = boundingRect();
    bool needScale = m_stretch;

    if(needScale) {
        if(std::abs(bounding_rect.width() - current_image.width()) < 2
                && std::abs(bounding_rect.height() - current_image.height()) < 2) {
            needScale = false;  // 오차가 2 pixel 이내이면 scale 하지않음.
        }
    }

    if(!needScale) {
        painter->drawImage(QPointF(0,0), current_image);
    }
    else {
        QImage stretched = this->current_image.scaled(QSize(bounding_rect.width(), bounding_rect.height()));
        painter->drawImage(QPointF(0,0), stretched);
        //qDebug() << "ThumbnailImage::paint() stretched";
    }
}

QImage ThumbnailImage::image() const
{
    return this->current_image;
}

void ThumbnailImage::setImage(const QImage &image)
{
    this->current_image = image;
    update();
}

bool ThumbnailImage::stretch() const
{
    return m_stretch;
}

void ThumbnailImage::setStretch(const bool stretch)
{
    if(m_stretch != stretch) {
        m_stretch = stretch;
        emit stretchChanged();
    }
}
