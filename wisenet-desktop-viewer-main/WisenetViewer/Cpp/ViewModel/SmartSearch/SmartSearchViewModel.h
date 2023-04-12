#pragma once
#include <QAbstractTableModel>
#include <QObject>
#include <QDebug>
#include <QMap>
#include "QCoreServiceManager.h"
class SmartSearchViewModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_PROPERTY(int totalCount READ totalCount NOTIFY totalCountChanged)

public:
    explicit SmartSearchViewModel(QObject *parent = nullptr);
    ~SmartSearchViewModel();

    static SmartSearchViewModel* getInstance(){
        static SmartSearchViewModel instance;
        return &instance;
    }

    int rowCount(const QModelIndex & = QModelIndex()) const override;
    int columnCount(const QModelIndex & = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    int totalCount() const;

    Q_INVOKABLE void selectChannel(const QString channelKey);   // channelKey : deviceId_channelId_overlappedId
    Q_INVOKABLE QVariantList getSelectedChannelResult();  // 현재 선택 채널의 결과 리스트를 QVariantList 타입으로 변환하여 리턴하는 함수
    Q_INVOKABLE void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;
    Q_INVOKABLE void filter(const QStringList & types);
    Q_INVOKABLE void setHoveredRow(int rowNum, bool hovered);
    Q_INVOKABLE void clear();

    enum Role{
        HoveredRole= Qt::UserRole,
        EventImageRole,
        DeviceTimeRole,
    };

public slots:
    void clearRequest();
    void setSearchParam(qint64 fromDate, qint64 toDate, int overlappedID, QString channelId);
    void addLine(int eventType, QList<float> coordinates);
    void addArea(int areaType, QList<float> coordinates, QList<int> eventTypes, QList<int> aiTypes);
    void searchRequest();    

signals:
    void totalCountChanged();
    void resultUpdated(QSet<QString> typeSet);
    void smartSearchFinished();
    void smartSearchNoData(bool isSuccess);

private:
    void UpdateSortData(int column, Qt::SortOrder order);
    QVariant getDataDisplayRole(const QModelIndex &index) const;
    QVariant getDataFromSearchResults(const QModelIndex &index, Role roleName) const;
    void sortFilter();
    void reloadTableData();

    qint64 m_startTime = 0;
    qint64 m_endTime = 0;
    int m_overlappedID = 0;
    QString m_deviceChannelId = ""; // deviceId_channelId
    std::vector<Wisenet::Device::SmartSearchArea> m_areas;
    std::vector<Wisenet::Device::SmartSearchLine> m_lines;


    QVector<QVector<QVariant>> m_data;
    QVector<int> m_index;
    QVector<int> m_sorted;
    QSet<QString> m_typeSet;    // 선택 채널 결과에 포함된 type의  set
    QStringList m_filterTypes;  // 현재 설정 된 filter type
    int m_hoveredRow;

    QString m_channelKey = "";  // channelKey : deviceId_channelId_overlappedId
    QMap<QString, std::vector<Wisenet::Device::SmartSearchResult>> m_smartSearchResult;
};
