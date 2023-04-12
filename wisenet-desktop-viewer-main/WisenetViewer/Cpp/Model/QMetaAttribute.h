#pragma once
#include <QObject>
#include "QCoreServiceManager.h"

class QMetaAttribute : public QObject
{
    Q_OBJECT
public:
    explicit QMetaAttribute(QObject *parent = nullptr) : QObject(parent){}
    Wisenet::Device::MetaAttribute* metaAttribute()
    {
        return m_metaAttribute;
    }

    void setMetaAttribute( Wisenet::Device::MetaAttribute* metaAttribute)
    {
        m_metaAttribute = metaAttribute;
    }
private:
    Wisenet::Device::MetaAttribute* m_metaAttribute = nullptr;
};
