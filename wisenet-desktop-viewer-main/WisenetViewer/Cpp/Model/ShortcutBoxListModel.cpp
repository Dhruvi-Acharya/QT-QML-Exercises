#include "ShortcutBoxListModel.h"
#include <QDebug>

ShortcutBoxListModel::ShortcutBoxListModel(QObject *parent)
    :QAbstractListModel(parent)
{
    m_roleNames[InUseRole] = "inUse";
    m_roleNames[MenuEnumRole] = "menuEnum";

    connect(&QCoreServiceManager::Instance(), &QCoreServiceManager::coreServiceEventTriggered,
            this, &ShortcutBoxListModel::coreServiceEventTriggered, Qt::QueuedConnection);
    loadShortcutSetting();
}

ShortcutBoxListModel::~ShortcutBoxListModel()
{
    m_data.clear();
}

void ShortcutBoxListModel::loadShortcutSetting()
{
    SPDLOG_DEBUG("ShortcutBoxListModel::loadShortcutSetting");
    m_shortcutUseList.clear();
    m_data.clear();

    QCoreServiceManager::Instance().Settings()->loadShortcutUse(m_shortcutUseList);
    qDebug() << "loadShortcutSetting's size" << m_shortcutUseList.size();

    beginResetModel();

    if(m_shortcutUseList.size() == 0) {      //  ini에 shortcut정보가 없을 경우
        for (int i =0; i<m_shortcutMenuCount; i++) {
            if(m_defaultOffMenu.contains(i))
                m_shortcutUseList.append(false);
            else
                m_shortcutUseList.append(true);
        }
        for (int i : m_currentOrder) {
            if (m_defaultOffMenu.contains(i))
                m_data.append(qMakePair(i, false));
            else
                m_data.append(qMakePair(i, true));
        }
    }
    else { // ini에 shortcut 정보가 있을 경우
        for (int i : m_currentOrder) {
            m_data.append(qMakePair(i, m_shortcutUseList[i]));
        }
        //  버전 변경으로 인해 ini에 저장된 개수와 현재 버전의 숏컷 메뉴 개수가 다를 경우 m_shortcutUseList 수정
        if (m_shortcutMenuCount > m_shortcutUseList.size()) {
            qDebug() << "loadShortcutSetting m_shortcutMenuCount > m_shortcutUseList.size())";;
            for (int i=m_shortcutUseList.size(); i<m_shortcutMenuCount; i++)
                m_shortcutUseList.append(true);
        }
//        else if (m_shortcutMenuCount < m_shortcutUseList.size()) {
//            for (int i=m_shortcutUseList.size(); i<m_shortcutMenuCount; i--)
//                m_shortcutUseList.pop_back();
//        }
    }

    // licenseCheck - ini file에 masking enum에 해당하는 데이터를 true로 값을 바꿔도 보이지 않게 더블체크
    if(m_licenseActivated){
        m_data[m_currentOrder.indexOf(ShortcutMenu::Masking)].second = true;
        m_shortcutUseList[ShortcutMenu::Masking] = true;
    }
    else {
        SPDLOG_DEBUG("ShortcutBoxListModel::loadShortcutSetting license isn't activated.");
        m_data.removeAt(m_currentOrder.indexOf(ShortcutMenu::Masking));
        m_shortcutUseList[ShortcutMenu::Masking] = false;
    }

    endResetModel();
    emit settingChanged();

    for(int i=0; i<m_shortcutUseList.size(); i++)
        SPDLOG_DEBUG("ShortcutBoxListModel::loadShortcutSetting m_shortcutUseList i {} : use {}", i, m_shortcutUseList[i]);

    for(int i=0; i<m_data.size(); i++)
        SPDLOG_DEBUG("ShortcutBoxListModel::loadShortcutSetting m_data enum {} : use {}", m_data[i].first, m_data[i].second);
}

void ShortcutBoxListModel::saveShortcutSetting()
{
    SPDLOG_DEBUG("ShortcutBoxListModel::saveShortcutSetting");

    qDebug() << "saveShortcutSetting's size" << m_data.size();
    for(QPair<int, bool> menu : m_data){
        //qDebug() << "ShortcutBoxListModel::saveShortcutSetting" << menu.second;
        m_shortcutUseList[menu.first]= menu.second;
    }

    QCoreServiceManager::Instance().Settings()->saveShortcutUse(m_shortcutUseList);
    emit settingChanged();
}

int ShortcutBoxListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_data.count();
}

QVariant ShortcutBoxListModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if(row < 0 || row >= m_data.count())
        return QVariant();

    QPair<int, bool> value = m_data[index.row()];

    switch(role)
    {
    case InUseRole:
        return value.second;
    case MenuEnumRole:
        return value.first;
    }

    return QVariant();
}

bool ShortcutBoxListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    qDebug() << "ShortcutBoxListModel::setData" << index << value;
    int row = index.row();
    if(row < 0 || row >= m_data.count())
        return false;

    QVector<int> changedRole;
    switch(role)
    {
    case InUseRole:
        m_data[row].second = value.toBool();
        changedRole << RoleNames::InUseRole;
        emit dataChanged(index, index, changedRole);
        return true;
    case MenuEnumRole:
        m_data[row].first = value.toInt();
        changedRole << RoleNames::MenuEnumRole;
        emit dataChanged(index, index, changedRole);
        return true;
    default:
        break;
    }
    return false;
}

QList<bool> ShortcutBoxListModel::getData( )
{
    qDebug() << "ShortcutBoxListModel::getData() size " << m_shortcutUseList.size();
    return m_shortcutUseList;
}


void ShortcutBoxListModel::insert(int idx, QPair<int, bool> menu)
{
    if(idx < 0 || idx > m_data.count())
    {
        return;
    }

    beginInsertRows(QModelIndex(), idx, idx);
    m_data.insert(idx, menu);
    endInsertRows();
}

void ShortcutBoxListModel::append(int menuEnum, bool inUse)
{
    insert(m_data.count(), qMakePair(menuEnum, inUse));
}

void ShortcutBoxListModel::coreServiceEventTriggered(QCoreServiceEventPtr event)
{
    auto serviceEvent = event ->eventDataPtr;
    switch(serviceEvent->EventTypeId()){
    case Wisenet::Core::FullInfoEventType:
        updateLicenseActivation(event);
        break;
    case Wisenet::Core::ActivateLicenseEventType:
        updateLicenseActivation(event);
        break;
    default:
        break;
    }
}

void ShortcutBoxListModel::updateLicenseActivation(QCoreServiceEventPtr event)
{
    auto serviceEvent = event->eventDataPtr;
    if(serviceEvent->EventTypeId() == Wisenet::Core::FullInfoEventType) {
        auto fullInfoEvent =  std::static_pointer_cast<Wisenet::Core::FullInfoEvent>(event->eventDataPtr);
        m_licenseActivated = fullInfoEvent->licenseInfo.activated;
        qDebug() << "ShortcutBoxListModel::updateLicenseActivation Event Type is FullInfoEventType" << fullInfoEvent->licenseInfo.activated;
    }
    else {
        qDebug() << "ShortcutBoxListModel::updateLicenseActivation Event Type is ActivateLicenseEventType";
        auto activateEvent = std::static_pointer_cast<Wisenet::Core::ActivateLicenseEvent>(event->eventDataPtr);
        bool isActivated = activateEvent->licenseInfo.activated;

        if(m_licenseActivated == false && isActivated == true) {
            SPDLOG_INFO("ShortcutBoxListModel::updateLicenseActivation {} : Activate the masking view now {} ", isActivated);
            m_licenseActivated = isActivated;
        }
    }
    loadShortcutSetting();
}
