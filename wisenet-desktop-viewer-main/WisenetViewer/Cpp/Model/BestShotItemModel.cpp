#include "BestShotItemModel.h"
#include <QLocaleManager.h>

BestShotItemModel::BestShotItemModel(QObject *parent)
    : QObject{parent}
{
    m_image = QImage(1,1, QImage::Format_RGB888);
    m_image.fill(qRgba(35, 35, 35, 255));
}

BestShotItemModel::~BestShotItemModel()
{
    m_origin = nullptr;
}

int BestShotItemModel::imageWidth()
{
    return m_imageWidth;
}

int BestShotItemModel::imageHeight()
{
    return m_imageHeight;
}

int BestShotItemModel::x()
{
    //qDebug() << "x" << m_x;
    return m_x;
}

int BestShotItemModel::y()
{
    //qDebug() << "y" << m_y;
    return m_y;
}

int BestShotItemModel::width()
{
    //qDebug() << "width" << m_width;
    return m_width;
}

int BestShotItemModel::height()
{
    //qDebug() << "height" << m_height;
    return m_height;
}

int BestShotItemModel::rowNumber()
{
    return m_rowNumber;
}

int BestShotItemModel::sequenceOfRow()
{
    return m_sequenceOfRow;
}

bool BestShotItemModel::visible()
{
    return m_visible;
}

bool BestShotItemModel::flagChecked()
{
    return m_flagChecked;
}

QMetaAttribute* BestShotItemModel::metaAttr()
{
    return &m_qMetaAttribute;
}

Wisenet::Device::MetaAttribute BestShotItemModel::metaAttribute()
{
    return m_metaAttribute;
}

void BestShotItemModel::setImage(const QImage& image)
{
    m_image = image;
    imageChanged();
    emit itemChanged(this);
}

void BestShotItemModel::setImageWidth(int imageWidth)
{
    m_imageWidth = imageWidth;
    imageWidthChanged(imageWidth);
}

void BestShotItemModel::setImageHeight(int imageHeight)
{
    m_imageHeight = imageHeight;
    imageHeightChanged(imageHeight);
}

void BestShotItemModel::setX(int x)
{
    m_x = x;
    xChanged(x);
}

void BestShotItemModel::setY(int y)
{
    m_y = y;
    yChanged(y);
}

void BestShotItemModel::setWidth(int width)
{
    m_width = width;
    widthChanged(width);

}

void BestShotItemModel::setHeight(int height)
{
    m_height = height;
    heightChanged(height);
}

void BestShotItemModel::setRowNumber(int rowNumber)
{
    m_rowNumber = rowNumber;
    rowNumberChanged(rowNumber);
}

void BestShotItemModel::setSequenceOfRow(int sequenceOfRow)
{
    m_sequenceOfRow = sequenceOfRow;
    sequenceOfRowChanged(sequenceOfRow);
}

void BestShotItemModel::setVisible(bool visible)
{
    m_visible = visible;
    visibleChanged(visible);
}

void BestShotItemModel::setFlagChecked(bool flagChecked)
{
    qDebug() << "setFlagChecked" << flagChecked;
    m_flagChecked = flagChecked;

    if(m_origin != nullptr){
        m_origin->setFlagChecked(flagChecked);
    }

    emit itemChanged(this);
    emit flagCheckedChanged(flagChecked);
}

void BestShotItemModel::setOrigin(BestShotItemModel* origin)
{
    m_origin = origin;
}

void BestShotItemModel::setMetaAttribute(Wisenet::Device::MetaAttribute metaAttribute)
{
    m_metaAttribute = metaAttribute;
    m_qMetaAttribute.setMetaAttribute(&m_metaAttribute);

    /*
    qDebug() << "BestShotItemModel::setMetaAttribute" << (int)m_metaAttribute.attributeType;

    for(int i=0; i<m_metaAttribute.personGender.size(); i++){
        qDebug() << "BestShotItemModel::setMetaAttribute personGender" << QString::fromStdString(m_metaAttribute.personGender.at(i));
    }

    for(int i=0; i<m_metaAttribute.personClothingTopsColor.size(); i++){
        qDebug() << "BestShotItemModel::setMetaAttribute personClothingTopsColor" << QString::fromStdString(m_metaAttribute.personClothingTopsColor.at(i));
    }

    for(int i=0; i<m_metaAttribute.personClothingBottomsColor.size(); i++){
        qDebug() << "BestShotItemModel::setMetaAttribute personClothingBottomsColor" << QString::fromStdString(m_metaAttribute.personClothingBottomsColor.at(i));
    }

    for(int i=0; i<m_metaAttribute.personBelongingBag.size(); i++){
        qDebug() << "BestShotItemModel::setMetaAttribute personBelongingBag" << QString::fromStdString(m_metaAttribute.personBelongingBag.at(i));
    }

    for(int i=0; i<m_metaAttribute.faceGender.size(); i++){
        qDebug() << "BestShotItemModel::setMetaAttribute faceGender" << QString::fromStdString(m_metaAttribute.faceGender.at(i));
    }

    for(int i=0; i<m_metaAttribute.faceAgeType.size(); i++){
        qDebug() << "BestShotItemModel::setMetaAttribute faceAgeType" << QString::fromStdString(m_metaAttribute.faceAgeType.at(i));
    }

    for(int i=0; i<m_metaAttribute.faceOpticals.size(); i++){
        qDebug() << "BestShotItemModel::setMetaAttribute faceOpticals" << QString::fromStdString(m_metaAttribute.faceOpticals.at(i));
    }

    for(int i=0; i<m_metaAttribute.faceMask.size(); i++){
        qDebug() << "BestShotItemModel::setMetaAttribute faceMask" << QString::fromStdString(m_metaAttribute.faceMask.at(i));
    }

    for(int i=0; i<m_metaAttribute.vehicleType.size(); i++){
        qDebug() << "BestShotItemModel::setMetaAttribute vehicleType" << QString::fromStdString(m_metaAttribute.vehicleType.at(i));
    }

    for(int i=0; i<m_metaAttribute.vehicleColor.size(); i++){
        qDebug() << "BestShotItemModel::setMetaAttribute vehicleColor" << QString::fromStdString(m_metaAttribute.vehicleColor.at(i));
    }*/
}

void BestShotItemModel::applyFilter(Wisenet::Device::MetaFilter& metaFilter)
{
    m_visible = true;
    bool hasAttr = false;

    if(m_metaAttribute.attributeType == Wisenet::Device::AiClassType::person)
    {
        if(m_metaAttribute.personGender.size() > 0 && metaFilter.personGender.size() > 0)
        {
            for(int i=0; i<m_metaAttribute.personGender.size(); i++)
            {
                if(metaFilter.personGender.find(m_metaAttribute.personGender.at(i)) != metaFilter.personGender.end())
                {
                    hasAttr = true;
                    break;
                }
            }

            if(!hasAttr)
            {
                qDebug() << "BestShotItemModel::applyFilter - personGender no more object";
                m_visible = false;
                return;
            }
        }

        if(m_metaAttribute.personClothingTopsColor.size() > 0 && metaFilter.personClothingTopsColor.size() > 0)
        {
            hasAttr = false;
            for(int i=0; i<m_metaAttribute.personClothingTopsColor.size(); i++)
            {
                if(metaFilter.personClothingTopsColor.find(m_metaAttribute.personClothingTopsColor.at(i)) != metaFilter.personClothingTopsColor.end())
                {
                    hasAttr = true;
                    break;
                }
            }

            if(!hasAttr)
            {
                m_visible = false;
                return;
            }
        }

        if(m_metaAttribute.personClothingBottomsColor.size() > 0 && metaFilter.personClothingBottomsColor.size() > 0)
        {
            hasAttr = false;
            for(int i=0; i<m_metaAttribute.personClothingBottomsColor.size(); i++)
            {
                if(metaFilter.personClothingBottomsColor.find(m_metaAttribute.personClothingBottomsColor.at(i)) != metaFilter.personClothingBottomsColor.end())
                {
                    hasAttr = true;
                    break;
                }
            }

            if(!hasAttr)
            {
                m_visible = false;
                return;
            }
        }

        if(m_metaAttribute.personBelongingBag.size() > 0 && metaFilter.personBelongingBag.size() > 0)
        {
            hasAttr = false;
            for(int i=0; i<m_metaAttribute.personBelongingBag.size(); i++)
            {
                if(metaFilter.personBelongingBag.find(m_metaAttribute.personBelongingBag.at(i)) != metaFilter.personBelongingBag.end())
                {
                    hasAttr = true;
                    break;
                }
            }

            if(!hasAttr)
            {
                m_visible = false;
                return;
            }
        }
    }
    else if(m_metaAttribute.attributeType == Wisenet::Device::AiClassType::vehicle)
    {
        if(m_metaAttribute.vehicleType.size() > 0 && metaFilter.vehicleType.size() > 0)
        {
            hasAttr = false;
            for(int i=0; i<m_metaAttribute.vehicleType.size(); i++)
            {
                if(metaFilter.vehicleType.find(m_metaAttribute.vehicleType.at(i)) != metaFilter.vehicleType.end())
                {
                    hasAttr = true;
                    break;
                }
            }

            if(!hasAttr)
            {
                m_visible = false;
                return;
            }
        }

        if(m_metaAttribute.vehicleColor.size() > 0 && metaFilter.vehicleColor.size() > 0)
        {
            hasAttr = false;
            for(int i=0; i<m_metaAttribute.vehicleColor.size(); i++)
            {
                if(metaFilter.vehicleColor.find(m_metaAttribute.vehicleColor.at(i)) != metaFilter.vehicleColor.end())
                {
                    hasAttr = true;
                    break;
                }
            }

            if(!hasAttr)
            {
                m_visible = false;
                return;
            }
        }
    }
    else if(m_metaAttribute.attributeType == Wisenet::Device::AiClassType::face)
    {
        if(m_metaAttribute.faceGender.size() > 0 && metaFilter.faceGender.size() > 0)
        {
            hasAttr = false;
            for(int i=0; i<m_metaAttribute.faceGender.size(); i++)
            {
                if(metaFilter.faceGender.find(m_metaAttribute.faceGender.at(i)) != metaFilter.faceGender.end())
                {
                    hasAttr = true;
                    break;
                }
            }

            if(!hasAttr)
            {
                m_visible = false;
                return;
            }
        }

        if(m_metaAttribute.faceAgeType.size() > 0 && metaFilter.faceAgeType.size() > 0)
        {
            hasAttr = false;
            for(int i=0; i<m_metaAttribute.faceAgeType.size(); i++)
            {
                if(metaFilter.faceAgeType.find(m_metaAttribute.faceAgeType.at(i)) != metaFilter.faceAgeType.end())
                {
                    hasAttr = true;
                    break;
                }
            }

            if(!hasAttr)
            {
                m_visible = false;
                return;
            }
        }

        if(m_metaAttribute.faceOpticals.size() > 0 && metaFilter.faceOpticals.size() > 0)
        {
            hasAttr = false;
            for(int i=0; i<m_metaAttribute.faceOpticals.size(); i++)
            {
                if(metaFilter.faceOpticals.find(m_metaAttribute.faceOpticals.at(i)) != metaFilter.faceOpticals.end())
                {
                    hasAttr = true;
                    break;
                }
            }

            if(!hasAttr)
            {
                m_visible = false;
                return;
            }
        }

        if(m_metaAttribute.faceMask.size() > 0 && metaFilter.faceMask.size() > 0)
        {
            hasAttr = false;
            for(int i=0; i<m_metaAttribute.faceMask.size(); i++)
            {
                if(metaFilter.faceMask.find(m_metaAttribute.faceMask.at(i)) != metaFilter.faceMask.end())
                {
                    hasAttr = true;
                    break;
                }
            }

            if(!hasAttr)
            {
                m_visible = false;
                return;
            }
        }
    }
    else if(m_metaAttribute.attributeType == Wisenet::Device::AiClassType::ocr)
    {
        QString source = QString::fromUtf8(m_metaAttribute.ocrText.c_str());
        QString filterText = QString::fromUtf8(metaFilter.ocrText.c_str());

        if(!filterText.contains('*'))
            filterText = "*" + filterText + "*";

        //qDebug() << "OCR filterText"<< filterText;

        QRegExp exp(filterText);
        exp.setPatternSyntax(QRegExp::Wildcard);

        if(!exp.exactMatch(source))
        {
            m_visible = false;
            return;
        }
    }
}

int BestShotItemModel::attributeType()
{
    return (int)m_metaAttribute.attributeType;
}

QString BestShotItemModel::cameraName()
{
    QString deviceId = QString::fromStdString(m_metaAttribute.aiBaseAttribute.deviceID);
    QString cameraId = QString::number(m_metaAttribute.aiBaseAttribute.channelId);

    Wisenet::Device::Device::Channel channel;
    if(QCoreServiceManager::Instance().DB()->FindChannel(deviceId, cameraId, channel))
    {
        return QString::fromUtf8(channel.name.c_str());
    }

    return "";
}

QString BestShotItemModel::displayTime()
{
    return QLocaleManager::Instance()->getDateTimeFromMsec(m_metaAttribute.aiBaseAttribute.dateTime);
}

long long BestShotItemModel::deviceTime()
{
    return m_metaAttribute.aiBaseAttribute.dateTime;
}

QString BestShotItemModel::deviceId()
{
    QString deviceId = QString::fromStdString(m_metaAttribute.aiBaseAttribute.deviceID);
    return deviceId;
}

QString BestShotItemModel::channelId()
{
    QString channelId = QString::number(m_metaAttribute.aiBaseAttribute.channelId);
    return channelId;
}

QString BestShotItemModel::personGender()
{
    if(m_metaAttribute.personGender.size() == 1)
        return QCoreApplication::translate("WisenetLinguist", m_metaAttribute.personGender.at(0).c_str());
    else
        return "-";
}

QString BestShotItemModel::personClothingTopColor()
{
    if(m_metaAttribute.personClothingTopsColor.size() == 1)
    {
        return QCoreApplication::translate("WisenetLinguist", m_metaAttribute.personClothingTopsColor.at(0).c_str());
    }
    else if(m_metaAttribute.personClothingTopsColor.size() == 2)
    {
        return QCoreApplication::translate("WisenetLinguist", m_metaAttribute.personClothingTopsColor.at(0).c_str()) + ", " + QCoreApplication::translate("WisenetLinguist", m_metaAttribute.personClothingTopsColor.at(1).c_str());
    }
    else
        return "-";
}

QString BestShotItemModel::personClothingBottomColor()
{
    if(m_metaAttribute.personClothingBottomsColor.size() == 1)
    {
        return QCoreApplication::translate("WisenetLinguist", m_metaAttribute.personClothingBottomsColor.at(0).c_str());
    }
    else if(m_metaAttribute.personClothingBottomsColor.size() == 2)
    {
        return QCoreApplication::translate("WisenetLinguist", m_metaAttribute.personClothingBottomsColor.at(0).c_str()) + ", " + QCoreApplication::translate("WisenetLinguist", m_metaAttribute.personClothingBottomsColor.at(1).c_str());
    }
    else
        return "-";
}

QString BestShotItemModel::personBag()
{
    if(m_metaAttribute.personBelongingBag.size() == 1)
    {
        QString str = QString::fromStdString(m_metaAttribute.personBelongingBag.at(0));

        QString linguist = "";
        if(str == "Wear")
            linguist = "Bag";
        else
            linguist = "No bag";

        return QCoreApplication::translate("WisenetLinguist", linguist.toStdString().c_str());
    }
    else
        return "-";
}

QString BestShotItemModel::faceGender()
{
    if(m_metaAttribute.faceGender.size() == 1)
        return QCoreApplication::translate("WisenetLinguist", m_metaAttribute.faceGender.at(0).c_str());
    else
        return "-";
}

QString BestShotItemModel::faceAge()
{
    if(m_metaAttribute.faceAgeType.size() == 1)
        return QCoreApplication::translate("WisenetLinguist", m_metaAttribute.faceAgeType.at(0).c_str());
    else
        return "-";
}

QString BestShotItemModel::faceGlasses()
{
    if(m_metaAttribute.faceOpticals.size() == 1)
    {
        QString str = QString::fromStdString(m_metaAttribute.faceOpticals.at(0));

        QString linguist = "";
        if(str == "Wear")
            linguist = "Glasses";
        else
            linguist = "No glasses";

        return QCoreApplication::translate("WisenetLinguist", linguist.toStdString().c_str());
    }
    else
        return "-";
}

QString BestShotItemModel::faceMask()
{
    if(m_metaAttribute.faceMask.size() == 1)
    {
        QString str = QString::fromStdString(m_metaAttribute.faceMask.at(0));

        QString linguist = "";
        if(str == "Wear")
            linguist = "Face mask";
        else
            linguist = "No face mask";

        return QCoreApplication::translate("WisenetLinguist", linguist.toStdString().c_str());
    }
    else
        return "-";
}

QString BestShotItemModel::vehicleType()
{
    if(m_metaAttribute.vehicleType.size() == 1)
        return QCoreApplication::translate("WisenetLinguist", m_metaAttribute.vehicleType.at(0).c_str());
    else
        return "-";
}

QString BestShotItemModel::vehicleColor()
{
    if(m_metaAttribute.vehicleColor.size() == 1)
    {
        return QCoreApplication::translate("WisenetLinguist", m_metaAttribute.vehicleColor.at(0).c_str());
    }
    else if(m_metaAttribute.vehicleColor.size() == 2)
    {
        return QCoreApplication::translate("WisenetLinguist", m_metaAttribute.vehicleColor.at(0).c_str()) + ", " + QCoreApplication::translate("WisenetLinguist", m_metaAttribute.vehicleColor.at(1).c_str());
    }
    else
        return "-";
}

QString BestShotItemModel::ocrNumber()
{
    qDebug() << "BestShotItemModel::ocrNumber()" << QString::fromStdString(m_metaAttribute.ocrText);
    return QString::fromStdString(m_metaAttribute.ocrText);
}
