#ifndef EVENTSCHEDULEVIEWMODEL_H
#define EVENTSCHEDULEVIEWMODEL_H

#include <QAbstractTableModel>
#include <set>
#include "QCoreServiceManager.h"
#include <QObject>

struct ScheduleListData
{
    QString name = "";
    QString table = QString(8*24, '1');
};

class EventScheduleViewModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_PROPERTY(int highlightRow READ highlightRow NOTIFY highlightRowChanged)
    Q_PROPERTY(int selectedRowCount READ selectedRowCount NOTIFY selectedRowCountChanged)

    Q_PROPERTY(QString scheduleGuid READ scheduleGuid WRITE setScheduleGuid NOTIFY scheduleGuidChanged)
    Q_PROPERTY(QString scheduleName READ scheduleName WRITE setScheduleName NOTIFY scheduleNameChanged)
    Q_PROPERTY(QString scheduleTable READ scheduleTable WRITE setScheduleTable NOTIFY scheduleTableChanged)

public:
    enum EventScheduleTitle{
        EventScheduleID = 0,
        EventScheduleCheck,
        EventScheduleName,
        EventScheduleTable
    };

    enum Role{
        HighlightRole = Qt::UserRole,
        HoveredRole
    };

    Q_INVOKABLE void reset();
    Q_INVOKABLE void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

    Q_INVOKABLE void setHighlightRow(int rowNum);
    Q_INVOKABLE void setHoveredRow(int rowNum, bool hovered);
    Q_INVOKABLE bool setCheckState(int rowNum, bool checked);
    Q_INVOKABLE bool setAllCheckState(bool checked);

    Q_INVOKABLE void deleteSchedule();

    Q_INVOKABLE void readSchedule(QString scheduleId);
    Q_INVOKABLE void saveSchedule();

    Q_INVOKABLE void addSchedule(QString scheduleName);
    Q_INVOKABLE void editSchedule(int rowNum, QString scheduleTableInfo);

    Q_INVOKABLE QString getScheduleId(int rowNum);
    Q_INVOKABLE QString getScheduleName(int rowNum);
    Q_INVOKABLE QString getScheduleTable(int rowNum);

    // setter
    Q_INVOKABLE void setScheduleGuid(QString scheduleGuid);
    Q_INVOKABLE void setScheduleName(QString scheduleName);
    Q_INVOKABLE void setScheduleTable(QString scheduleTable);

    // getter
    QString scheduleGuid() {return m_scheduleGuid;}
    QString scheduleName() {return m_scheduleName;}
    QString scheduleTable() {return m_scheduleTable;}


    void append(const QString& scheduleGuid, const QString& scheduleName);
    void remove(int index);

    EventScheduleViewModel(QObject* parent = nullptr);

    int highlightRow() const;
    int selectedRowCount();
    int rowCount(const QModelIndex & = QModelIndex()) const override;
    int columnCount(const QModelIndex & = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void coreServiceEventTriggered(QCoreServiceEventPtr event);

signals:
    void highlightRowChanged();
    void selectedRowCountChanged();
    void resultMessage(bool success, QString msg);

    void scheduleGuidChanged();
    void scheduleNameChanged();
    void scheduleTableChanged();

    void deleted();
    void exist(int index);

private:
    void UpdateSortData(int column, Qt::SortOrder order);
    QVariant getDataDisplayRole(const QModelIndex &index) const;

    QVector<QVector<QVariant>> m_data;
    QVector<int> m_index;
    QVector<int> m_sorted;
    int m_alwaysIndex = 0;

    QStringList m_columnNames;

    int m_hoveredRow;
    int m_highlightRow;

    Qt::SortOrder m_sortOrder = Qt::AscendingOrder;
    QString m_scheduleGuid;
    QString m_scheduleName;
    QString m_scheduleTable;
};

#endif // EVENTSCHEDULEVIEWMODEL_H
