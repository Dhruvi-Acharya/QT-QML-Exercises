#include "BookmarkListModel.h"

BookmarkListModel::BookmarkListModel(QObject *parent)
    : QAbstractListModel(parent)
{
    Q_UNUSED(parent);
    qDebug() << "BookmarkListModel::BookmarkListModel()";
}

BookmarkListModel::~BookmarkListModel()
{
    qDebug() << "BookmarkListModel::~BookmarkListModel()";
}

int BookmarkListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_data.size();
}

QHash<int, QByteArray> BookmarkListModel::roleNames() const
{
    static QHash<int, QByteArray> roles;
    roles[BookmarkIdRole] = "bookmarkId";
    roles[BookmarkNameRole] = "bookmarkName";
    roles[SourceIdRole] = "sourceId";
    roles[CameraNameRole] = "cameraName";
    roles[DescriptionRole] = "description";
    roles[DisplayTimeRole] = "displayTime";
    roles[UtcStartTimeRole] = "utcStartTime";
    roles[UtcEndTimeRole] = "utcEndTime";
    roles[ImageRole] = "image";
    roles[ImageWidthRole] = "imageWidth";
    roles[ImageHeightRole] = "imageHeight";
    return roles;
}

QVariant BookmarkListModel::data(const QModelIndex &index, int role) const
{
    QVariant value;

    switch (role)
    {
    case BookmarkIdRole:
        value = m_data[index.row()]->bookmarkId();
        break;
    case BookmarkNameRole:
        value = m_data[index.row()]->bookmarkName();
        break;
    case SourceIdRole:
        value = m_data[index.row()]->sourceId();
        break;
    case CameraNameRole:
        value = m_data[index.row()]->cameraName();
        break;
    case DescriptionRole:
        value = m_data[index.row()]->description();
        break;
    case DisplayTimeRole:
        value = m_data[index.row()]->displayTime();
        break;
    case UtcStartTimeRole:
        value = m_data[index.row()]->utcStartTime();
        break;
    case UtcEndTimeRole:
        value = m_data[index.row()]->utcEndTime();
        break;
    case ImageRole:
        value.setValue(m_data[index.row()]->image());
        break;
    case ImageWidthRole:
        value.setValue(m_data[index.row()]->imageWidth());
        break;
    case ImageHeightRole:
        value.setValue(m_data[index.row()]->imageHeight());
        break;
    default:
        break;
    }

    return value;
}

void BookmarkListModel::applySaveChannels(QString id, QString name)
{
    for(int i=0; i<m_data.size(); i++)
    {
        if(m_data[i]->sourceId() == id)
        {
            m_data[i]->setCameraName(name);

            QVector<int> role;
            role << CameraNameRole;

            QModelIndex idx = index(i, 0, QModelIndex());
            emit dataChanged(idx, idx, role);
        }
    }
}

void BookmarkListModel::setSelectedBookmark(QString bookmarkId)
{
    emit selectedBookmarkChanged(bookmarkId);
}

void BookmarkListModel::insertFirst(BookmarkItemModel* model)
{
    beginInsertRows(QModelIndex(), 0, 0);
    this->m_data.insert(0, model);
    this->m_dataMap.insert(model->bookmarkId(), model);
    endInsertRows();

    emit countChanged();
}

void BookmarkListModel::append(BookmarkItemModel* model)
{
    int i = m_data.size();

    beginInsertRows(QModelIndex(), i, i);
    this->m_data.append(model);
    this->m_dataMap.insert(model->bookmarkId(), model);
    endInsertRows();

    emit countChanged();
}

void BookmarkListModel::removeAt(int position)
{
    beginRemoveRows(QModelIndex(), position, position);

    BookmarkItemModel* model = this->m_data.takeAt(position);
    this->m_dataMap.remove(model->bookmarkId());
    delete model;

    endRemoveRows();

    emit countChanged();
}

void BookmarkListModel::removeAll()
{
    beginResetModel();

    this->m_dataMap.clear();

    qDeleteAll(m_data);
    this->m_data.clear();

    endResetModel();

    emit countChanged();
}

bool BookmarkListModel::contains(QString bookmarkId)
{
    return this->m_dataMap.contains(bookmarkId);
}

BookmarkItemModel* BookmarkListModel::getItem(QString bookmarkId)
{
    return this->m_dataMap[bookmarkId];
}

void BookmarkListModel::update(QString bookmarkId, QString bookmarkName, QString sourceId,
                               QString cameraName, QString description, QString displayTime,
                               long long utcStartTime, long long utcEndTime, QImage& image)
{

    if(this->m_dataMap.contains(bookmarkId))
    {
        QVector<int> role;

        BookmarkItemModel* model = this->m_dataMap[bookmarkId];

        int row = m_data.indexOf(model, 0);


        if(model->bookmarkName() != bookmarkName)
        {
            model->setBookmarkName(bookmarkName);
            role << BookmarkNameRole;
        }

        if(model->sourceId() != sourceId)
        {
            model->setSourceId(sourceId);
            role << SourceIdRole;
        }

        if(model->cameraName() != cameraName)
        {
            model->setCameraName(cameraName);
            role << CameraNameRole;
        }

        if(model->description() != description)
        {
            model->setDescription(description);
            role << DescriptionRole;
        }

        if(model->displayTime() != displayTime)
        {
            model->setDisplayTime(displayTime);
            role << DisplayTimeRole;
        }

        if(model->utcStartTime() != utcStartTime)
        {
            model->setUtcStartTime(utcStartTime);
            role << UtcStartTimeRole;
        }

        if(model->utcEndTime() != utcEndTime)
        {
            model->setUtcEndTime(utcEndTime);
            role << UtcEndTimeRole;
        }

        emit dataChanged(QModelIndex(index(row, 0)), QModelIndex(index(row, 0)), role);
    }
}

void BookmarkListModel::remove(QString bookmarkId)
{
    if(!this->m_dataMap.contains(bookmarkId))
        return;

    BookmarkItemModel* model = this->m_dataMap.take(bookmarkId);
    int position = this->m_data.indexOf(model, 0);

    beginRemoveRows(QModelIndex(), position, position);
    delete this->m_data.takeAt(position);
    endRemoveRows();

    emit countChanged();
}
