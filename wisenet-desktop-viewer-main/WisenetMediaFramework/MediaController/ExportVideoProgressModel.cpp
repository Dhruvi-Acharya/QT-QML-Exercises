#include "ExportVideoProgressModel.h"
#include <QDesktopServices>

ExportVideoProgressModel::ExportVideoProgressModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_roleNames[NameRole] = "name";
    m_roleNames[ProgressRole] = "progress";
    m_roleNames[FailedRole] = "isFailed";
    m_roleNames[CanceledRole] = "isCanceled";

    connect(FileWriteManager::getInstance(), &FileWriteManager::exportMediaItemAdded,
            this, &ExportVideoProgressModel::onExportMediaItemAdded, Qt::QueuedConnection);

    connect(FileWriteManager::getInstance(), &FileWriteManager::endExportAll,
            this, &ExportVideoProgressModel::onEndExportAll, Qt::QueuedConnection);
}

ExportVideoProgressModel::~ExportVideoProgressModel()
{
    clear();
}

QHash<int, QByteArray> ExportVideoProgressModel::roleNames() const
{
    return m_roleNames;
}

int ExportVideoProgressModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_exportItemList.size();
}

QVariant ExportVideoProgressModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if(row < 0 || row >= m_exportItemList.size())
        return QVariant();

    ExportProgressItem* item = m_exportItemList[row];
    if(item == nullptr)
        return QVariant();

    switch(role)
    {
    case NameRole:
        return item->name();
    case ProgressRole:
        return item->progress();
    case FailedRole:
        return item->isFailed();
    case CanceledRole:
        return item->isCanceled();
    }

    return QVariant();
}

void ExportVideoProgressModel::insert(int index, ExportProgressItem* item)
{
    if(index < 0 || index > m_exportItemList.size())
    {
        return;
    }

    beginInsertRows(QModelIndex(), index, index);
    m_exportItemList.insert(index, item);
    endInsertRows();
}

void ExportVideoProgressModel::append(ExportProgressItem* item)
{
    insert(m_exportItemList.size(), item);
}

void ExportVideoProgressModel::remove(int index)
{
    if(index < 0 || index > m_exportItemList.size() - 1)
    {
        return;
    }

    beginRemoveRows(QModelIndex(), index, index);
    m_exportItemList[index]->deleteLater();
    m_exportItemList.removeAt(index);
    endRemoveRows();
}

void ExportVideoProgressModel::clear()
{
    if(m_exportItemList.size() == 0)
        return;

    beginRemoveRows(QModelIndex(), 0, m_exportItemList.size()-1);

    for(auto& item : m_exportItemList)
    {
        item->deleteLater();
    }
    m_exportItemList.clear();

    endRemoveRows();
}

void ExportVideoProgressModel::clearFinishedItem()
{
    QMutexLocker lock(&m_mutex);

    for(int i=0 ; i<m_exportItemList.size() ; )
    {
        if(m_exportItemList[i]->progress() == 100 || m_exportItemList[i]->isFailed() || m_exportItemList[i]->isCanceled())
        {
            remove(i);
        }
        else
        {
            i++;
        }
    }
}

void ExportVideoProgressModel::stopExportItem(int index)
{
    QMutexLocker lock(&m_mutex);

    if(m_exportItemList.size() <= index)
        return;

    FileWriteManager::getInstance()->stopVideoExport(m_exportItemList[index]->exportMediaItemPtr(), true);    

    if(m_exportItemList[index]->progress() == 0) {
        // export 시작하지 않은 Item은 목록에서 삭제
        remove(index);
    }
    else {
        // export 도중 취소 된 Item은 상태만 업데이트
        m_exportItemList[index]->setIsCanceled(true);
        QVector<int> rols {CanceledRole};
        emit dataChanged(QAbstractListModel::index(index,0,QModelIndex()), QAbstractListModel::index(index,0,QModelIndex()), rols);
    }
}

void ExportVideoProgressModel::stopAllExportItem()
{
    QMutexLocker lock(&m_mutex);

    while(m_exportItemList.size() > 0)
    {
        FileWriteManager::getInstance()->stopVideoExport(m_exportItemList[0]->exportMediaItemPtr(), false);
        remove(0);
    }
}

void ExportVideoProgressModel::onExportMediaItemAdded(ExportMediaItem* item)
{
    QMutexLocker lock(&m_mutex);

    ExportMediaRequest request = item->getExportMediaRequest();
    Wisenet::Device::Device::Channel channel;
#ifndef MEDIA_FILE_ONLY
    QCoreServiceManager::Instance().DB()->FindChannel(request.deviceId.toStdString(), request.channeId.toStdString(), channel);
#endif

    QString name = QString::fromUtf8(channel.name.c_str());
    if(request.trackIndex != -1) {
        // multi track 백업이면 이름에 OverlappedId index를 붙임
        name = name + " (" + QCoreApplication::translate("WisenetLinguist","Overlapped") + " " + QString::number(request.trackIndex) + ")";
    }

    ExportProgressItem* progressItem = new ExportProgressItem(name, item->progress(), item->filePath(), item);
    connect(item, &ExportMediaItem::progressChanged, progressItem, &ExportProgressItem::onProgressChanged, Qt::QueuedConnection);
    connect(item, &ExportMediaItem::endExport, progressItem, &ExportProgressItem::onEndExport, Qt::QueuedConnection);
    connect(progressItem, &ExportProgressItem::progressChanged, this, &ExportVideoProgressModel::onProgressChanged, Qt::QueuedConnection);
    connect(progressItem, &ExportProgressItem::failedChanged, this, &ExportVideoProgressModel::onFailedChanged, Qt::QueuedConnection);

    append(progressItem);

    m_exportFinished = false;
    emit exportFinishedChanged();
}

void ExportVideoProgressModel::onProgressChanged(ExportProgressItem* item)
{
    for(int i=0 ; i<m_exportItemList.size() ; i++)
    {
        if(item == m_exportItemList[i])
        {
            QVector<int> rols {ProgressRole};
            emit dataChanged(index(i,0,QModelIndex()), index(i,0,QModelIndex()), rols);
            break;
        }
    }
}

void ExportVideoProgressModel::onFailedChanged(ExportProgressItem* item)
{
    for(int i=0 ; i<m_exportItemList.size() ; i++)
    {
        if(item == m_exportItemList[i])
        {
            QVector<int> rols {FailedRole};
            emit dataChanged(index(i,0,QModelIndex()), index(i,0,QModelIndex()), rols);
            break;
        }
    }
}

void ExportVideoProgressModel::onEndExportAll()
{
    m_exportFinished = true;
    emit exportFinishedChanged();
}

void ExportVideoProgressModel::openFilePath(int index)
{
    QMutexLocker lock(&m_mutex);

    if(m_exportItemList.size() <= index)
        return;

    QUrl url = QUrl::fromLocalFile(m_exportItemList[index]->filePath());
    QDesktopServices::openUrl(url);
}
