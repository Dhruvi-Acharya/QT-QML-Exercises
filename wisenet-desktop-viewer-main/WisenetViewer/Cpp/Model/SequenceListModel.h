#pragma once

#include <QAbstractListModel>
#include <QList>
#include <QDebug>

struct SequenceItem
{
    int layoutIndex = 0;
    int interval = 5;
};

class SequenceListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int dataCount READ dataCount NOTIFY dataCountChanged)

public:
    enum RoleNames
    {
        ButtonDisplayRole = Qt::UserRole,
        SelectedLayoutRole,
        IntervalRole,
        RowRole,
    };

    int dataCount()
    {
        return m_data.count();
    }

    explicit SequenceListModel(QObject *parent = nullptr);
    ~SequenceListModel();

    // Basic functionality:
    Q_INVOKABLE int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Q_INVOKABLE  bool setData(const QModelIndex &idx, const QVariant &value, int role = Qt::EditRole) override;

    // Data edit functionality:
    void insert(int idx, SequenceItem* item);
    void append(SequenceItem* item);
    void remove(int idx);
    void clear();
    QList<SequenceItem*> getData();
    void swapData(int firstIdx, int secondIdx);

signals:
    void dataCountChanged(int dataCount);

protected:
    virtual QHash<int, QByteArray> roleNames() const override;

private:
    QHash<int, QByteArray> m_roleNames;
    QList<SequenceItem*> m_data;
};
