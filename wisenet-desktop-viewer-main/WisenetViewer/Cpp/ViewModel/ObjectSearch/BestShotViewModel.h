#pragma once
#include <QObject>
#include "../../Model/BestShotListModel.h"
#include "QCoreServiceManager.h"

class BestShotViewModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(BestShotListModel* bestShotListModel READ bestShotListModel NOTIFY bestShotListModelChanged)
    Q_PROPERTY(int bestShotHeight READ bestShotHeight WRITE setBestShotHeight NOTIFY bestShotHeightChanged)
    Q_PROPERTY(int viewWidth READ viewWidth WRITE setViewWidth NOTIFY viewWidthChanged)
    Q_PROPERTY(int viewHeight READ viewHeight NOTIFY viewHeightChanged)
    Q_PROPERTY(int selectedIndex READ selectedIndex WRITE setSelectedIndex NOTIFY selectedIndexChanged)

    Q_PROPERTY(int bestShotCount READ bestShotCount NOTIFY bestShotCountChanged)
public:
    explicit BestShotViewModel(QObject *parent = nullptr);

    BestShotListModel* bestShotListModel();
    int bestShotHeight();
    void setBestShotHeight(int height);

    int viewWidth();
    int viewHeight();
    void setViewWidth(int viewWidth);

    int selectedIndex();
    void setSelectedIndex(int selectedIndex);

    void makeTestData();

    void setData(std::vector<Wisenet::Device::MetaAttribute>* result);
    void setPositions();

    int bestShotCount();

    bool filter(const Wisenet::Device::MetaAttribute& metaAttribute);
    void applyFilter(Wisenet::Device::MetaFilter& filter);

public slots:
    void loadMoreBestShots(bool isScrolled = true);
    void clearAll();
    void addFlagedItem(BestShotViewModel* arg);
    void exportImage(QString path);

signals:
    void bestShotListModelChanged(BestShotListModel* arg);
    void bestShotHeightChanged(int height);
    void viewWidthChanged(int viewWIdth);
    void viewHeightChanged(int viewHeight);
    void selectedIndexChanged(int selectedIndex);
    void selectedItemChanged(BestShotItemModel* itemModel);

    void filter0Changed(bool filter0);
    void filter1Changed(bool filter1);
    void filter2Changed(bool filter2);

    void bestShotCountChanged(int bestShotCount);

    void openLoading();
    void closeLoading();
private:
    BestShotListModel m_bestShotListModel;
    int m_bestShotHeight = 200;
    int m_minimumSpacing = 5;
    int m_viewWidth = 500;
    int m_selectedIndex = -1;

    std::atomic<bool> m_isReceivedMeta{false};
    std::atomic<int> m_requestCount{0};
    QList<Wisenet::Device::MetaAttribute> m_result;
    QMap<QString, BestShotItemModel*> m_urlMap;
    Wisenet::Device::MetaFilter m_filter;

    const int BESTSHOT_COUNT_FIRST_REQUEST = 60;
    const int BESTSHOT_COUNT_PER_REQUEST = 20;
};
