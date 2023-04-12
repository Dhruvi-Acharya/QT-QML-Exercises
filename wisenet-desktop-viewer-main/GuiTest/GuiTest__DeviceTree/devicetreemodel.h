#ifndef DEVICETREEMODEL_H
#define DEVICETREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

class DeviceTreeItem;

class DeviceTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    enum Roles  {
        DeviceTreeRole = Qt::UserRole + 1,
    };

    explicit DeviceTreeModel(QObject *parent = nullptr);
    ~DeviceTreeModel();

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int,QByteArray> roleNames() const override;

    Q_INVOKABLE QModelIndex rootIndex();

   //// Header:
   //QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
   //
   //bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) override;

   //// Basic functionality:
   //QModelIndex index(int row, int column,
   //                  const QModelIndex &parent = QModelIndex()) const override;
   //QModelIndex parent(const QModelIndex &index) const override;
   //
   //int rowCount(const QModelIndex &parent = QModelIndex()) const override;
   //int columnCount(const QModelIndex &parent = QModelIndex()) const override;
   //
   //// Fetch data dynamically:
   //bool hasChildren(const QModelIndex &parent = QModelIndex()) const override;
   //
   //bool canFetchMore(const QModelIndex &parent) const override;
   //void fetchMore(const QModelIndex &parent) override;
   //
   //QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
   //
   //// Editable:
   //bool setData(const QModelIndex &index, const QVariant &value,
   //             int role = Qt::EditRole) override;
   //
   //Qt::ItemFlags flags(const QModelIndex& index) const override;
   //
   //// Add data:
   //bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
   //bool insertColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;
   //
   //// Remove data:
   //bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
   //bool removeColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;

private:
    QVariant newItemType(const QString &name, const QString &ip, int type);

    void DefaultSetup();

    DeviceTreeItem *getItem(const QModelIndex &index) const;

    DeviceTreeItem *rootItem;
    QVector<QVariant> makeColumnData(QString baseName, QString baseIp, int index);
};

#endif // DEVICETREEMODEL_H
