#pragma once

#include <QObject>
#include <QDebug>
#include <QImage>
#include "QCoreServiceManager.h"
#include "QMetaAttribute.h"

class BestShotItemModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QImage image READ image WRITE setImage NOTIFY imageChanged)
    Q_PROPERTY(int imageWidth READ imageWidth WRITE setImageWidth NOTIFY imageWidthChanged)
    Q_PROPERTY(int imageHeight READ imageHeight WRITE setImageHeight NOTIFY imageHeightChanged)
    Q_PROPERTY(int x READ x WRITE setX NOTIFY xChanged)
    Q_PROPERTY(int y READ y WRITE setY NOTIFY yChanged)
    Q_PROPERTY(int width READ width WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(int height READ height WRITE setHeight NOTIFY heightChanged)
    Q_PROPERTY(int rowNumber READ rowNumber WRITE setRowNumber NOTIFY rowNumberChanged)
    Q_PROPERTY(int sequenceOfRow READ sequenceOfRow WRITE setSequenceOfRow NOTIFY sequenceOfRowChanged)

    Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged)
    Q_PROPERTY(bool flagChecked READ flagChecked WRITE setFlagChecked NOTIFY flagCheckedChanged)

    Q_PROPERTY(int attributeType READ attributeType NOTIFY attributeTypeChanged)
    Q_PROPERTY(QString cameraName READ cameraName)
    Q_PROPERTY(QString displayTime READ displayTime)

    Q_PROPERTY(long long deviceTime READ deviceTime)
    Q_PROPERTY(QString deviceId READ deviceId)
    Q_PROPERTY(QString channelId READ channelId)

    Q_PROPERTY(QString personGender READ personGender)
    Q_PROPERTY(QString personClothingTopColor READ personClothingTopColor)
    Q_PROPERTY(QString personClothingBottomColor READ personClothingBottomColor)
    Q_PROPERTY(QString personBag READ personBag)

    Q_PROPERTY(QString faceGender READ faceGender)
    Q_PROPERTY(QString faceAge READ faceAge)
    Q_PROPERTY(QString faceGlasses READ faceGlasses)
    Q_PROPERTY(QString faceMask READ faceMask)

    Q_PROPERTY(QString vehicleType READ vehicleType)
    Q_PROPERTY(QString vehicleColor READ vehicleColor)

    Q_PROPERTY(QString ocrNumber READ ocrNumber)

    Q_PROPERTY(QMetaAttribute* metaAttr READ metaAttr)
public:
    explicit BestShotItemModel(QObject *parent = nullptr);
    ~BestShotItemModel();

    QImage image() { return m_image; }
    int imageWidth();
    int imageHeight();
    int x();
    int y();
    int width();
    int height();
    int rowNumber();
    int sequenceOfRow();
    bool visible();
    bool flagChecked();
    int attributeType();
    QString cameraName();
    QString displayTime();

    long long deviceTime();
    QString deviceId();
    QString channelId();

    QString personGender();
    QString personClothingTopColor();
    QString personClothingBottomColor();
    QString personBag();

    QString faceGender();
    QString faceAge();
    QString faceGlasses();
    QString faceMask();

    QString vehicleType();
    QString vehicleColor();

    QString ocrNumber();

    QMetaAttribute* metaAttr();
    Wisenet::Device::MetaAttribute metaAttribute();

    void setImage(const QImage& image);
    void setImageWidth(int imageWidth);
    void setImageHeight(int imageHeight);
    void setX(int x);
    void setY(int y);
    void setWidth(int width);
    void setHeight(int height);
    void setRowNumber(int rowNumber);
    void setSequenceOfRow(int sequenceOfRow);
    void setVisible(bool visible);
    void setFlagChecked(bool flagChecked);
    void setOrigin(BestShotItemModel* origin);
    void setMetaAttribute(Wisenet::Device::MetaAttribute metaAttribute);

    void applyFilter(Wisenet::Device::MetaFilter& metaFilter);

signals:
    void imageChanged();
    void imageWidthChanged(int imageWidth);
    void imageHeightChanged(int imageWidth);
    void xChanged(int x);
    void yChanged(int y);
    void widthChanged(int width);
    void heightChanged(int height);
    void rowNumberChanged(int rowNumber);
    void sequenceOfRowChanged(int sequenceOfRow);
    void visibleChanged(bool visible);
    void flagCheckedChanged(bool flagChecked);

    void itemChanged(BestShotItemModel* item);
    void attributeTypeChanged(int title);
private:
    QImage m_image;
    int m_width = 0;
    int m_height = 0;
    int m_x = 0;
    int m_y = 0;
    int m_imageWidth = 0;
    int m_imageHeight = 0;
    int m_rowNumber = 0;
    int m_sequenceOfRow = 0;

    bool m_visible = false;
    bool m_flagChecked = false;

    BestShotItemModel* m_origin = nullptr;
    QMetaAttribute m_qMetaAttribute;
    Wisenet::Device::MetaAttribute m_metaAttribute;
};
