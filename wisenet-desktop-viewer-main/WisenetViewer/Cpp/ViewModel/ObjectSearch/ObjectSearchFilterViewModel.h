#pragma once

#include <QObject>
#include <QDebug>
#include "BaseFilterModel.h"
#include "QCoreServiceManager.h"

class ObjectSearchFilterViewModel : public BaseFilterModel
{
    Q_OBJECT
    // 필터링 Flickable의 visible
    Q_PROPERTY(bool isObjectOptionFlickableVisible READ isObjectOptionFlickableVisible WRITE setIsObjectOptionFlickableVisible NOTIFY isObjectOptionFlickableVisibleChanged)

public:
    explicit ObjectSearchFilterViewModel(QObject *parent = nullptr);
    ~ObjectSearchFilterViewModel();

    void setVisibleObjectOptionButtonsByResult(std::vector<Wisenet::Device::MetaAttribute>* result);

    // 필터링 Flickable의 visible
    bool isObjectOptionFlickableVisible()
    {
        return m_isObjectOptionFlickableVisible;
    }
    // 필터링 Flickable의 visible
    void setIsObjectOptionFlickableVisible(bool isObjectOptionFlickableVisible)
    {
        m_isObjectOptionFlickableVisible = isObjectOptionFlickableVisible;

        emit isObjectOptionFlickableVisibleChanged(isObjectOptionFlickableVisible);
    }

public slots:
    void search(const QDateTime &from, const QDateTime &to, QStringList channels, /*Wisenet::Device::AiClassType*/int classType, const bool &allDevice);
    void setMetaAttribute();

    void updateFilterChecked(Wisenet::Device::MetaFilter& metaFilter);

signals:
    // Search Request to ObjectSearchViewModel
    void searchRequest(const QDateTime &from, const QDateTime &to, QStringList channels, /*Wisenet::Device::AiClassType*/int classType, const bool &allDevice);
    void filterChanged();
    // 필터링 Flickable의 visible
    void isObjectOptionFlickableVisibleChanged(bool isObjectOptionFlickableVisible);

private:
    // search result from ObjectSearchViewModel
    std::vector<Wisenet::Device::MetaAttribute>* m_result = nullptr;
    // 필터링 Flickable의 visible
    bool m_isObjectOptionFlickableVisible = false;
};
