#pragma once

#include <QObject>
#include <QDebug>
#include "QCoreServiceManager.h"
#include "BestShotViewModel.h"
#include "ObjectSearchFilterViewModel.h"
#include "BestshotFilterTooltipModel.h"
#include "GridListViewModel.h"

class ObjectSearchViewModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY (BestShotViewModel* bestShotViewModel READ bestShotViewModel NOTIFY bestShotViewModelChangedEvent)
    Q_PROPERTY (BestShotViewModel* flagViewModel READ flagViewModel NOTIFY flagViewModelChangedEvent)
    Q_PROPERTY (ObjectSearchFilterViewModel* objectSearchFilterViewModel READ objectSearchFilterViewModel NOTIFY objectSearchFilterViewModelChangedEvent)
    Q_PROPERTY (BestshotFilterTooltipModel* bestshotFilterTooltipModel READ bestshotFilterTooltipModel NOTIFY bestshotFilterTooltipModelChangedEvent)
    Q_PROPERTY (GridListViewModel* gridListViewModel READ gridListViewModel NOTIFY gridListViewModelChangedEvent)

    Q_PROPERTY (int receivedMetaCount READ receivedMetaCount WRITE setReceivedMetaCount NOTIFY receivedMetaCountChangedEvent)
    Q_PROPERTY (int totalMetaCount READ totalMetaCount WRITE setTotalMetaCount NOTIFY totalMetaCountChangedEvent)

public:
    explicit ObjectSearchViewModel(QObject *parent = nullptr);
    static ObjectSearchViewModel* getInstance(){
        static ObjectSearchViewModel instance;
        return &instance;
    }

    BestShotViewModel* bestShotViewModel()
    {
        return &m_bestShotViewModel;
    }

    BestShotViewModel* flagViewModel()
    {
        return &m_flagViewModel;
    }

    ObjectSearchFilterViewModel* objectSearchFilterViewModel()
    {
        return &m_objectSearchFilterViewModel;
    }

    BestshotFilterTooltipModel* bestshotFilterTooltipModel()
    {
        return &m_bestshotFilterTooltipModel;
    }

    GridListViewModel* gridListViewModel()
    {
        return &m_gridListViewModel;
    }

    int receivedMetaCount()
    {
        return m_receivedMetaCount;
    }

    int totalMetaCount()
    {
        return m_totalMetaCount;
    }

    void setReceivedMetaCount(int receivedMetaCount)
    {
        m_receivedMetaCount = receivedMetaCount;
        emit receivedMetaCountChangedEvent(receivedMetaCount);
    }

    void setTotalMetaCount(int totalMetaCount)
    {
        m_totalMetaCount = totalMetaCount;
        emit totalMetaCountChangedEvent(totalMetaCount);
    }

    Wisenet::Device::MetaFilter makeMetaFilter();

public slots:
    void searchRequest(const QDateTime &from, const QDateTime &to, QStringList channels, /*Wisenet::Device::AiClassType*/int classType, const bool &allDevice);
    void clear();
    void cancel();
    void applyFilter();
    void updateBestshotTooltipChecked();

signals:
    void bestShotViewModelChangedEvent(BestShotViewModel* arg);
    void flagViewModelChangedEvent(BestShotViewModel* arg);
    void objectSearchFilterViewModelChangedEvent(ObjectSearchFilterViewModel* arg);
    void bestshotFilterTooltipModelChangedEvent(BestshotFilterTooltipModel* arg);
    void gridListViewModelChangedEvent(GridListViewModel* arg);
    void refreshWidth();
    void openLoading();
    void closeLoading();
    void popupNoResult();

    void receivedMetaCountChangedEvent(int receivedMetaCount);
    void totalMetaCountChangedEvent(int receivedMetaCount);

private:
    std::vector<Wisenet::Device::MetaAttribute> m_result;
    BestShotViewModel m_bestShotViewModel;
    BestShotViewModel m_flagViewModel;
    ObjectSearchFilterViewModel m_objectSearchFilterViewModel;
    BestshotFilterTooltipModel m_bestshotFilterTooltipModel;
    GridListViewModel m_gridListViewModel;

    int m_receivedMetaCount = 0;
    int m_totalMetaCount = 0;

    std::atomic<bool> m_isCanceled{false};
    std::atomic<int> m_requestCount{0};
    std::set<Wisenet::uuid_string> m_requestedDeviceIdSet;
};
