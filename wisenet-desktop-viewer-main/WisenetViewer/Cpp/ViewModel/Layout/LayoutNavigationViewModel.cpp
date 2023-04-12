#include "LayoutNavigationViewModel.h"

LayoutNavigationViewModel::LayoutNavigationViewModel(QObject* parent) :
    QObject(parent)
{
    connect(&QCoreServiceManager::Instance(), &QCoreServiceManager::coreServiceEventTriggered,
            this, &LayoutNavigationViewModel::coreServiceEventTriggered, Qt::QueuedConnection);
}

LayoutNavigationViewModel::~LayoutNavigationViewModel()
{
    qDebug() << "LayoutNavigationViewModel::~LayoutTabBehaviors()";
}

void LayoutNavigationViewModel::makeNewTab()
{
    LayoutTabModel* item1 = new LayoutTabModel();
    item1->setText("Layout");
    item1->setUuid(QUuid::createUuid().toString(QUuid::WithoutBraces));
    m_layoutTabModel.append(item1);
}

void LayoutNavigationViewModel::addTab(int type, QString name, QString uuid)
{    
    qDebug() << "LayoutNavigationViewModel::addTab() " << type << name << uuid;

    LayoutTabModel* item1 = new LayoutTabModel();
    item1->setItemType(type);
    item1->setText(name);
    item1->setUuid(uuid);
    m_layoutTabModel.append(item1);

    auto db = QCoreServiceManager::Instance().DB();

    if(db != nullptr)
        db->countUpLayoutOpen(uuid);
}

void LayoutNavigationViewModel::saveTab(QString name)
{
    qDebug() << "LayoutNavigationViewModel::saveTab() " << name;
}

void LayoutNavigationViewModel::closeTab(QString layoutId)
{
    qDebug() << "LayoutNavigationViewModel::closeTab() " << layoutId;
    m_layoutTabModel.closeTab(layoutId);

    if(m_layoutTabModel.count() <= 0)
        makeNewTab();
}

void LayoutNavigationViewModel::closeAll()
{
    m_layoutTabModel.closeAll();
}

QList<QString> LayoutNavigationViewModel::closeAllButThis(QString layoutId)
{
    qDebug() << "LayoutNavigationViewModel::closeAllButThis() " << layoutId;
    return m_layoutTabModel.closeAllButThis(layoutId);
}

QVariantList LayoutNavigationViewModel::getLayoutListOfSequence(QString sequenceId)
{
    Wisenet::Core::SequenceLayout sequence;
    QCoreServiceManager::Instance().DB()->FindSequenceLayout(sequenceId, sequence);

    QVariantList layoutInfoList;
    for(int i=0; i<sequence.sequenceLayoutItems.size(); i++)
    {
        QVariantMap map;
        map.insert("layoutId",QString::fromStdString(sequence.sequenceLayoutItems[i].layoutID));
        map.insert("delayMs",sequence.sequenceLayoutItems[i].delayMs);
        map.insert("order",sequence.sequenceLayoutItems[i].order);
        layoutInfoList.append(map);
    }

    return layoutInfoList;
}

void LayoutNavigationViewModel::loadFromCoreService(QString layoutId)
{
    Wisenet::Core::Layout layout;
    QCoreServiceManager::Instance().DB()->FindLayout(layoutId, layout);

    m_MediaLayoutViewModel.loadFromCoreService(layout);
}

QList<QString> LayoutNavigationViewModel::getSavedLayoutList()
{
    QList<QString> idList = m_layoutTabModel.getLayoutIdList();

    for(int i=idList.count()-1; i >= 0; i--)
    {
        std::string stdLayoutId = idList[i].toStdString();

        // QCoreService에 있으면 제외시킴.
        Wisenet::Core::Layout layout;
        if(!QCoreServiceManager::Instance().DB()->FindLayout(stdLayoutId, layout))
        {
            idList.removeAt(i);
        }
    }

    return idList;
}

void LayoutNavigationViewModel::coreServiceEventTriggered(QCoreServiceEventPtr event)
{
    auto serviceEvent = event->eventDataPtr;
    //SPDLOG_INFO("LayoutNavigationViewModel get CoreServiceEvent!! EventType={}", serviceEvent->EventType());

    switch(serviceEvent->EventTypeId()){
    case Wisenet::Core::SaveLayoutEventType:
        Event_SaveLayout(event);
        break;
    case Wisenet::Core::RemoveLayoutsEventType:
        Event_RemoveLayout(event);
        break;
    case Wisenet::Core::RemoveSequenceLayoutsEventType:
        Event_RemoveSequenceLayout(event);
        break;
    }
}

void LayoutNavigationViewModel::Event_SaveLayout(QCoreServiceEventPtr event)
{
    auto saveLayoutEvent = std::static_pointer_cast<Wisenet::Core::SaveLayoutEvent>(event->eventDataPtr);

    Wisenet::Core::Layout layout = saveLayoutEvent->layout;

    QString uuid = QString::fromStdString(layout.layoutID);
    QString name = QString::fromUtf8(layout.name.c_str());

    emit layoutNameChanged(uuid, name);
}

void LayoutNavigationViewModel::Event_RemoveLayout(QCoreServiceEventPtr event)
{
    auto removeLayoutsEvent = std::static_pointer_cast<Wisenet::Core::RemoveLayoutsEvent>(event->eventDataPtr);

    for (auto& uuid : removeLayoutsEvent->layoutIDs) {

        QString deletedLayoutId = QString::fromStdString(uuid);

        closeTab(deletedLayoutId);
    }
}

void LayoutNavigationViewModel::Event_RemoveSequenceLayout(QCoreServiceEventPtr event)
{
    auto removeSequenceLayoutsEvent = std::static_pointer_cast<Wisenet::Core::RemoveSequenceLayoutsEvent>(event->eventDataPtr);

    for (auto& uuid : removeSequenceLayoutsEvent->sequenceLayoutIDs) {

        QString deletedSequenceLayoutId = QString::fromStdString(uuid);

        closeTab(deletedSequenceLayoutId);
    }
}
