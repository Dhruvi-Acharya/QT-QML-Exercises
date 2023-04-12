#include "SequenceAddViewModel.h"

SequenceAddViewModel::SequenceAddViewModel(QObject *parent) : QObject(parent)
{
    qDebug() << "SequenceAddViewModel()";
    resetListModel();
}

SequenceAddViewModel::~SequenceAddViewModel()
{
    qDebug() << "~SequenceAddViewModel()";
}

void SequenceAddViewModel::reloadLayout()
{
    resetListModel();
    m_layoutList.clear();
    m_layoutIdList.clear();

    qDebug() << "SequenceAddViewModel::layoutList()";
    auto layouts = QCoreServiceManager::Instance().DB()->GetLayouts();


    for(auto& kvp : layouts)
    {
        m_layoutList.append(QString::fromStdString(kvp.second.name));
    }

    std::sort(m_layoutList.begin(), m_layoutList.end());


    for(auto& layout : m_layoutList)
    {
        for(auto& kvp : layouts)
        {
            if(layout == QString::fromStdString(kvp.second.name))
            {
                m_layoutIdList.append(QString::fromStdString(kvp.second.layoutID));
                break;
            }
        }
    }

    emit layoutListChanged(m_layoutList);
}

void SequenceAddViewModel::loadLayouts(QString sequenceName, QString sequenceId)
{
    qDebug() << "SequenceAddViewModel::loadLayouts()";

    // SequenceAddView 초기화
    reloadLayout();

    // 받아온 시퀀스 이름으로 설정
    setSequenceName(sequenceName);
    sequenceNameChanged(m_sequenceName);

    // 시퀀스ID에 해당하는 시퀀스 get from DB
    Wisenet::Core::SequenceLayout sequence;
    QCoreServiceManager::Instance().DB()->FindSequenceLayout(sequenceId, sequence);

    QVariantList indexList;

    // get한 시퀀스의 레이아웃 리스트들을 SequenceAddView의 layoutList에 append
    m_sequenceListModel.clear();
    for(auto seq : sequence.sequenceLayoutItems)
    {
        SequenceItem* item = new SequenceItem();
        int layoutIndex = 0;

        for(int li = 0; li<m_layoutIdList.size(); li++)
        {
            if(m_layoutIdList[li] == QString::fromStdString(seq.layoutID))
            {
                layoutIndex = li;
                break;
            }
        }

        item->interval = seq.delayMs;
        item->layoutIndex = layoutIndex;

        m_sequenceListModel.append(item);
        indexList.append(layoutIndex);
    }
    addSequenceItem();

    QList<SequenceItem*> data = m_sequenceListModel.getData();
    for(auto seqItem : data)
    {
        qDebug() <<" seqItem->layoutIndex"<<seqItem->layoutIndex <<"seqItem->interval=" << seqItem->interval;
    }
}

void SequenceAddViewModel:: Request_UpdateSequenceLayout(QString sequenceId)
{
    qDebug() << "SequenceAddViewModel::Request_UpdateSequenceLayout()";
    QString sequenceNameParam = m_sequenceName;
    Wisenet::Core::SequenceLayout sequence;
    Wisenet::Core::User user;
    QCoreServiceManager::Instance().DB()->GetLoginUser(user);

    sequence.name = m_sequenceName.toStdString();
    sequence.sequenceLayoutID = sequenceId.toStdString();
    sequence.userName = user.loginID;

    QList<SequenceItem*> data = m_sequenceListModel.getData();

    for(int i=0; i < data.size()-1; i++)
    {
        qDebug() << "layoutIndex=" << data[i]->layoutIndex << "interval=" << data[i]->interval;

        Wisenet::Core::SequenceLayoutItem seqItem;

        seqItem.itemID = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8().constData();
        seqItem.layoutID = m_layoutIdList.at(data[i]->layoutIndex).toStdString();
        seqItem.delayMs = data[i]->interval;
        seqItem.order = i;

        sequence.sequenceLayoutItems.push_back(seqItem);
    }

    auto request = std::make_shared<Wisenet::Core::SaveSequenceLayoutRequest>();
    request->sequenceLayout = sequence;

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::SaveSequenceLayout,
                this, request,
                [this, sequenceNameParam, sequenceId](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::ResponseBase>(reply->responseDataPtr);
        if (response->isFailed()) {
            qDebug() << "SequenceAddViewModel::Request_UpdateSequenceLayout - RequestToCoreService : add fail " << sequenceNameParam;
        }
        else {
            qDebug() << "SequenceAddViewModel::Request_UpdateSequenceLayout - RequestToCoreService : add success " << sequenceNameParam;
            emit sequenceInfoChanged(m_sequenceName, sequenceId); // 시퀀스 DB Update가 완료되었으므로 LayoutNavigationView에 UI 갱신 요청
        }
    });
}

void SequenceAddViewModel::Request_SaveSequenceLayout()
{
    qDebug() << "SequenceAddViewModel::Request_SaveSequenceLayout() " << m_sequenceName;

    QString sequenceNameParam = m_sequenceName;
    Wisenet::Core::SequenceLayout sequence;
    Wisenet::Core::User user;
    QCoreServiceManager::Instance().DB()->GetLoginUser(user);

    sequence.name = m_sequenceName.toStdString();
    sequence.sequenceLayoutID = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8().constData();
    sequence.userName = user.loginID;

    QList<SequenceItem*> data = m_sequenceListModel.getData();

    for(int i=0; i < data.size()-1; i++)
    {
        qDebug() << data[i]->layoutIndex << " " << data[i]->interval;

        Wisenet::Core::SequenceLayoutItem seqItem;

        seqItem.itemID = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8().constData();
        seqItem.layoutID = m_layoutIdList.at(data[i]->layoutIndex).toStdString();
        seqItem.delayMs = data[i]->interval;
        seqItem.order = i;

        sequence.sequenceLayoutItems.push_back(seqItem);
    }

    auto request = std::make_shared<Wisenet::Core::SaveSequenceLayoutRequest>();
    request->sequenceLayout = sequence;

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::SaveSequenceLayout,
                this, request,
                [this, sequenceNameParam](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::ResponseBase>(reply->responseDataPtr);
        if (response->isFailed()) {
            qDebug() << "SequenceAddViewModel::Request_SaveSequenceLayout - RequestToCoreService : add fail " << sequenceNameParam;
        }
        else {
            qDebug() << "SequenceAddViewModel::Request_SaveSequenceLayout - RequestToCoreService : add success " << sequenceNameParam;
        }
    });
}

int SequenceAddViewModel::getLayoutCount()
{
    auto layouts = QCoreServiceManager::Instance().DB()->GetLayouts();
    return layouts.size();
}

void SequenceAddViewModel::saveLayout()
{
    Request_SaveSequenceLayout();
}

void SequenceAddViewModel::updateLayout(QString sequenceId)
{
    Request_UpdateSequenceLayout(sequenceId);
}

void SequenceAddViewModel::addSequenceItem()
{
    SequenceItem* item = new SequenceItem();
    m_sequenceListModel.append(item);
}

void SequenceAddViewModel::removeSequenceItem(int row)
{
    m_sequenceListModel.remove(row);
}

void SequenceAddViewModel::swapSequenceItem(int firstIdx, int secondIdx)
{
    m_sequenceListModel.swapData(firstIdx, secondIdx);

}

void SequenceAddViewModel::resetListModel()
{
    m_sequenceListModel.clear();
    addSequenceItem();
}

/*
void SequenceAddViewModel::setData(const int row, const QVariant &value, int role)
{
    //m_sequenceListModel.setData(row, value, role);
}*/
