#pragma once

#include "BaseFilterModel.h"
#include "QCoreServiceManager.h"
#include "../../Model/QMetaAttribute.h"

class BestshotFilterTooltipModel : public BaseFilterModel
{
    Q_OBJECT
    Q_PROPERTY(int objectType READ objectType WRITE setObjectType NOTIFY objectTypeChanged)
public:
    BestshotFilterTooltipModel();
    ~BestshotFilterTooltipModel();

    int objectType();
    void setObjectType(int objectType);

public slots:
    void setFilterVisible(QMetaAttribute* metaAttr);
    void setFilterChecked(Wisenet::Device::MetaFilter& metaFilter);
    void applyFilter();

signals:
    void objectTypeChanged(int objectType);
    void filterChanged(Wisenet::Device::MetaFilter& metaFilter);

private:
    int m_objectType = 0;
};
