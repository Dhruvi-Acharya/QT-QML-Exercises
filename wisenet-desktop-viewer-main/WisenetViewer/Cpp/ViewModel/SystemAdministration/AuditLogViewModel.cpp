#include "AuditLogViewModel.h"
#include <QtConcurrent>
#include "LogSettings.h"
#include "QCoreServiceManager.h"
#include "WisenetViewerDefine.h"

AuditLogViewModel::AuditLogViewModel(QObject *parent)
    :QAbstractTableModel(parent)
    ,m_hoveredRow(-1)
    ,m_totalCount(0)
    ,m_sortIndex(0)
    ,m_sortOrder(Qt::DescendingOrder)
{
    connect(&m_thread, &AuditLogThread::processCompleted, this, &AuditLogViewModel::processComplated);
}

int AuditLogViewModel::rowCount(const QModelIndex &) const
{
    return m_index.size();
}

int AuditLogViewModel::columnCount(const QModelIndex &) const
{
    return 5;
}

QVariant AuditLogViewModel::data(const QModelIndex &index, int role) const
{
    switch(role) {
    case Qt::DisplayRole:
        return getDataDisplayRole(index);
    case HoveredRole:
        if(m_hoveredRow == index.row())
            return true;
        return false;
    default:
        break;
    }

    return QVariant();
}

QHash<int, QByteArray> AuditLogViewModel::roleNames() const
{
    return { {Qt::DisplayRole, "displayRole"}, {HoveredRole, "hoveredRole"}};
}

void AuditLogViewModel::search(const QDateTime &from, const QDateTime &to)
{

    emit searchStarted();

    clearData();

    auto auditLogRequest = std::make_shared<Wisenet::Core::GetAuditLogRequest>();

    auditLogRequest->fromTime = from.toUTC().toMSecsSinceEpoch();
    auditLogRequest->toTime = to.toUTC().toMSecsSinceEpoch();

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::GetAuditLog,
                this, auditLogRequest,
                [=](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::Core::GetAuditLogResponse>(reply->responseDataPtr);
        if(response->isFailed()){
            SPDLOG_INFO("GetAuditLog, isFailed={}", response->isFailed());
            QSet<QString> userList;
            QVector<QStringList> cameraList;
            QSet<QString> logList;
            m_totalCount = 0;
            emit totalCountChanged();
            emit searchFinished(userList, cameraList, logList);
            return;
        }

        if(!response->auditLogs || response->auditLogs->empty()){
            SPDLOG_INFO("GetAuditLog, auditlog is empty");
            QSet<QString> userList;
            QVector<QStringList> cameraList;
            QSet<QString> logList;
            m_totalCount = 0;
            emit totalCountChanged();
            emit searchFinished(userList, cameraList, logList);
            return;
        }

        SPDLOG_DEBUG("Audit Log - start process");
        m_thread.process(response->deletedDevices, response->auditLogs);

    });
}

void AuditLogViewModel::sort(int column, Qt::SortOrder order)
{
    m_sortIndex = column;
    m_sortOrder = order;

    sortFilter();
}

void AuditLogViewModel::setHoveredRow(int rowNum, bool hovered)
{
    QVector<int> role;
    role << Role::HoveredRole;

    int last = m_hoveredRow;
    if(hovered){
        m_hoveredRow = rowNum;
        if(last > m_hoveredRow)
            emit dataChanged(QModelIndex(index(m_hoveredRow, 0)), QModelIndex(index(last, 5)), role);
        else
            emit dataChanged(QModelIndex(index(last, 0)), QModelIndex(index(m_hoveredRow, 5)), role);
    }
    else{
        if(rowNum == m_hoveredRow){
            m_hoveredRow = -1;
            emit dataChanged(QModelIndex(index(rowNum, 0)), QModelIndex(index(rowNum, 5)), role);
        }
    }
}

bool AuditLogViewModel::exportCSV(QString path, QString fileName)
{
    QString filePath = path + QDir::separator() + fileName;
    QFile csvFile(filePath);

    if (!csvFile.open(QIODevice::WriteOnly))
        return false;

    QTextStream textStream(&csvFile);
    textStream.setCodec("UTF-8");
    textStream.setGenerateByteOrderMark(true);
    QStringList stringList;

    stringList << "Date&Time"<<"User" << "Device Name" <<"Log Type"<<"Decsription";
    textStream << stringList.join( ',' )+"\n";

    for(int row = 0; row < rowCount(); row++ )
    {
        stringList.clear();

        for(int column = 0; column < columnCount(); column++ )
            stringList << data(index(row, column), Qt::DisplayRole).toString().replace(',',QString(" "));

        textStream << stringList.join( ',' )+"\n";
    }

    csvFile.close();
    return true;
}

void AuditLogViewModel::filter(const QStringList &users, const QStringList &channels, const QStringList &logTypes, const QString &description)
{
    qDebug() << "AuditLogViewModel::filter - Users: " << users;
    qDebug() << "AuditLogViewModel::filter - channels: " << channels;
    qDebug() << "AuditLogViewModel::filter - logTypes: " << logTypes;
    qDebug() << "AuditLogViewModel::filter - Description: " << description;
    m_filterUsers = users;
    m_filterChannels = channels;
    m_filterLogTypes = logTypes;
    m_filterDescription = description;

    sortFilter();
}

void AuditLogViewModel::processComplated(std::shared_ptr<std::vector<Wisenet::Core::AuditLog>> auditLogs,
                      QSharedPointer<QVector<QVector<QVariant>>> data,
                      QSharedPointer<QVector<int>> dateTimeDescIndex,
                      QSharedPointer<QVector<int>> userDescIndex,
                      QSharedPointer<QVector<int>> cameraDescIndex,
                      QSharedPointer<QVector<int>> eventDescIndex,
                      QSet<QString> userList,
                      QVector<QStringList> cameraList,
                      QSet<QString> logTypeList)
{
    SPDLOG_DEBUG("Audit Log processComplated");

    beginResetModel();

    m_auditLogs = auditLogs;
    m_data = data;

    m_index.resize(dateTimeDescIndex->size());
    std::copy(dateTimeDescIndex->begin(),dateTimeDescIndex->end(),m_index.begin());

    m_dateTimeDescIndex = dateTimeDescIndex;
    m_userDescIndex = userDescIndex;
    m_cameraDescIndex = cameraDescIndex;
    m_eventDescIndex = eventDescIndex;

    endResetModel();

    if(!m_auditLogs){
        m_totalCount = 0;
    }else {
        m_totalCount = (int)m_auditLogs->size();
    }
    emit totalCountChanged();

    emit searchFinished(userList, cameraList, logTypeList);
}

QVariant AuditLogViewModel::getDataDisplayRole(const QModelIndex &index) const
{
    int row = m_index[index.row()];

    const QVariant& cellValue = (*m_data)[row][index.column()];

    if(cellValue.isNull())
        return QVariant(QVariant::String);

    return cellValue.toString();
}

void AuditLogViewModel::clearData()
{
    beginResetModel();

    m_sortIndex = 0;
    m_sortOrder = Qt::DescendingOrder;
    m_filterUsers.clear();
    m_filterChannels.clear();
    m_filterLogTypes.clear();
    m_filterDescription = "";

    m_auditLogs.reset();
    m_data.reset();
    m_index.clear();

    m_userDescIndex.reset();
    m_dateTimeDescIndex.reset();
    m_cameraDescIndex.reset();
    m_eventDescIndex.reset();

    endResetModel();
}

void AuditLogViewModel::sortFilter()
{
    qDebug() << "start sortFilter";

    if(!m_dateTimeDescIndex){
        qDebug() << "m_dateTimeDescIndex is null";
        return;
    }

    if(!m_userDescIndex){
        qDebug() << "m_userDescIndex is null";
        return;
    }

    if(!m_cameraDescIndex){
        qDebug() << "m_cameraDescIndex is null";
        return;
    }

    if(!m_eventDescIndex){
        qDebug() << "m_eventDescIndex is null";
        return;
    }

    if(!m_auditLogs){
        qDebug() << "m_auditLogs is null";
        return;
    }

    if(!m_data){
        qDebug() << "m_data is null";
        return;
    }

    beginResetModel();

    QVector<int> index;

    if(0 == m_sortIndex){
        qDebug() << "Start sort 0";
        index.resize(m_dateTimeDescIndex->size());
        if(Qt::SortOrder::DescendingOrder == m_sortOrder){
            std::copy(m_dateTimeDescIndex->begin(),m_dateTimeDescIndex->end(),index.begin());
        }else{
            std::reverse_copy(m_dateTimeDescIndex->begin(),m_dateTimeDescIndex->end(),index.begin());
        }

    }else if(1 == m_sortIndex){
        qDebug() << "Start sort 1";
        index.resize(m_userDescIndex->size());
        if(Qt::SortOrder::DescendingOrder == m_sortOrder){
            std::copy(m_userDescIndex->begin(),m_userDescIndex->end(),index.begin());
        }else{
            std::reverse_copy(m_userDescIndex->begin(),m_userDescIndex->end(),index.begin());
        }
    }else if(2 == m_sortIndex){
        qDebug() << "Start sort 2";
        index.resize(m_cameraDescIndex->size());
        if(Qt::SortOrder::DescendingOrder == m_sortOrder){
            std::copy(m_cameraDescIndex->begin(),m_cameraDescIndex->end(),index.begin());
        }else{
            std::reverse_copy(m_cameraDescIndex->begin(),m_cameraDescIndex->end(),index.begin());
        }

    }else if(3 == m_sortIndex){
        qDebug() << "Start sort 3";
        index.resize(m_eventDescIndex->size());
        if(Qt::SortOrder::DescendingOrder == m_sortOrder){
            std::copy(m_eventDescIndex->begin(),m_eventDescIndex->end(),index.begin());
        }else{
            std::reverse_copy(m_eventDescIndex->begin(),m_eventDescIndex->end(),index.begin());
        }
    }

    if((m_filterUsers.size() > 0 || m_filterChannels.size() > 0 || m_filterLogTypes.size() > 0 || m_filterDescription.size() > 0)
            && index.size() > 0){

        qDebug() << "Start filterChannel";
        QVector<int> result = QtConcurrent::blockingFiltered(index,[this](int n){

            return containsUserFilter(n) && containsChannelFilter(n) && containsEventFilter(n) && containsDescriptionFilter(n);
        });

        if(result.size() > 0){
            qDebug() << "copy filter channel result size: " << result.size();
            m_index.reserve(result.size());
            m_index = result;
        }else {
            m_index.clear();
        }

    }else {

        m_index.clear();
        if(index.size() > 0){
            qDebug() << "copy m_index size : " << index.size();
            m_index.reserve(index.size());
            m_index = index;
        }
    }

    qDebug() << "complated sortFilter m_index size : " << m_index.size();

    endResetModel();

}

bool AuditLogViewModel::containsUserFilter(int index)
{
    if(m_filterUsers.empty()){
        return true;
    }

    foreach(const QString &user , m_filterUsers){
        qDebug() << "m_auditLogs size : " << m_auditLogs->size() << ", index = " << index;
        if((*m_data)[index][1] == user){
            return true;
        }
    }

    return false;

}
bool AuditLogViewModel::containsChannelFilter(int index)
{
    if(m_filterChannels.empty()){
        return true;
    }

    foreach(const QString &channel , m_filterChannels){

        if(0 == channel.compare("Service")){

            if(Wisenet::Core::AuditLog::ItemType::Service == (*m_auditLogs)[index].itemType){
                return true;
            }

        }else{
            QStringList id = channel.split('_');

            if(1 == id.size()){

                if(Wisenet::Core::AuditLog::ItemType::Device == (*m_auditLogs)[index].itemType){
                    if(QString::fromUtf8((*m_auditLogs)[index].itemID.c_str()) == id[0]){
                        return true;
                    }
                }

            }else{

                if((QString::fromUtf8((*m_auditLogs)[index].parentID.c_str()) == id[0])
                        &&(QString::fromUtf8((*m_auditLogs)[index].itemID.c_str()) == id[1])){
                    return true;
                }
            }
        }

    }

    return false;
}

bool AuditLogViewModel::containsEventFilter(int index)
{
    if(m_filterLogTypes.empty()){
        return true;
    }

    foreach(const QString &logType , m_filterLogTypes){
        if((QString::fromUtf8((*m_auditLogs)[index].actionType.c_str()) == logType)){
            return true;
        }
    }

    return false;
}

bool AuditLogViewModel::containsDescriptionFilter(int index)
{
    if(0 == m_filterDescription.size()){
        return true;
    }

    if((*m_data)[index][4].toString().contains(m_filterDescription, Qt::CaseInsensitive)){
        return true;
    }

    return false;
}
