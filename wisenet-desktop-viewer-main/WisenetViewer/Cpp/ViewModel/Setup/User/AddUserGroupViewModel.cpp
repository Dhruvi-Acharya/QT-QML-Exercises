#include "AddUserGroupViewModel.h"

#include "LogSettings.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/algorithm/string.hpp>
#include <set>


AddUserGroupViewModel::AddUserGroupViewModel(QObject* parent) : m_groupId("")
{
    Q_UNUSED(parent);

    connect(&QCoreServiceManager::Instance(), &QCoreServiceManager::coreServiceEventTriggered,
            this, &AddUserGroupViewModel::coreServiceEventTriggered, Qt::QueuedConnection);
}

AddUserGroupViewModel::~AddUserGroupViewModel()
{
    qDebug() << "~AddUserGroupViewModel()";
}

void AddUserGroupViewModel::setGroupId(const QString &groupId)
{
    SPDLOG_DEBUG("AddUserGroupViewModel::ID:{}", groupId.toStdString());
    if(groupId != m_groupId){
        m_groupId = groupId;
        emit this->groupIdChanged();
    }
}

void AddUserGroupViewModel::setGroupName(const QString &groupName)
{
    SPDLOG_DEBUG("AddUserGroupViewModel::Name:{}", groupName.toStdString());
    if(groupName != m_groupName){
        m_groupName = groupName;
        emit this->groupNameChanged();
    }
}

QStringList AddUserGroupViewModel::groupNameList() {
    SPDLOG_DEBUG("AddUserGroupViewModel::groupNameList: size:{}", m_groupNameList.size());
    return m_groupNameList;
}

void AddUserGroupViewModel::setGroupNameList(const QStringList& groupNameList)
{
    if(groupNameList != m_groupNameList){
        SPDLOG_DEBUG("AddUserGroupViewModel::setGroupNameList: size:{}", groupNameList.size());
        m_groupNameList = groupNameList;
        emit this->groupNameListChanged();
    }
}

void AddUserGroupViewModel::setPlayback(bool playback)
{
    SPDLOG_DEBUG("AddUserGroupViewModel::playback:{}", playback);
    if(playback != m_playback){
        m_playback = playback;
        emit this->playbackChanged();
    }
}

void AddUserGroupViewModel::setExportVideo(bool exportVideo)
{
    SPDLOG_DEBUG("AddUserGroupViewModel::exportVideo:{}", exportVideo);
    if(exportVideo != m_exportVideo){
        m_exportVideo = exportVideo;
        emit this->exportVideoChanged();
    }
}

void AddUserGroupViewModel::setLocalRecording(bool localRecording)
{
    SPDLOG_DEBUG("AddUserGroupViewModel::localRecording:{}", localRecording);
    if(localRecording != m_localRecording){
        m_localRecording = localRecording;
        emit this->localRecordingChanged();
    }
}

void AddUserGroupViewModel::setPtzControl(bool ptzControl)
{
    SPDLOG_DEBUG("AddUserGroupViewModel::ptzControl:{}", ptzControl);
    if(ptzControl != m_ptzControl){
        m_ptzControl = ptzControl;
        emit this->ptzControlChanged();
    }
}

void AddUserGroupViewModel::setAudio(bool audio)
{
    SPDLOG_DEBUG("AddUserGroupViewModel::audio:{}", audio);
    if(audio != m_audio){
        m_audio = audio;
        emit this->audioChanged();
    }
}

void AddUserGroupViewModel::setMic(bool mic)
{
    SPDLOG_DEBUG("AddUserGroupViewModel::mic:{}", mic);
    if(mic != m_mic){
        m_mic = mic;
        emit this->micChanged();
    }
}

void AddUserGroupViewModel::setIsAdminGroup(bool isAdminGroup)
{
    SPDLOG_DEBUG("AddUserGroupViewModel::isAdminGroup:{}", isAdminGroup);
    if(isAdminGroup != m_isAdminGroup){
        m_isAdminGroup = isAdminGroup;
        emit this->isAdminGroupChanged();
    }
}

void AddUserGroupViewModel::setIsAllResource(bool isAllResource)
{
    SPDLOG_DEBUG("AddUserGroupViewModel::setIsAllResource:{}", isAllResource);
    if(isAllResource != m_isAllResource){
        m_isAllResource = isAllResource;
        emit this->isAllResourceChanged();
    }
}

void AddUserGroupViewModel::setChannelGuidList(QVector<QString> channelGuidList)
{
    if(channelGuidList != m_channelGuidList){
        m_channelGuidList = channelGuidList;
        emit this->channelGuidListChanged();
    }
}

void AddUserGroupViewModel::setLayoutGuidList(QVector<QString> layoutGuidList)
{
    if(layoutGuidList != m_layoutGuidList){
        m_layoutGuidList = layoutGuidList;
        emit this->layoutGuidListChanged();
    }
}
QString AddUserGroupViewModel::findUserGroupId(QString name)
{
    // 2022.12.27. coverity
    auto db = QCoreServiceManager::Instance().DB();
    if (db == nullptr) {
        return QString("");
    }

    SPDLOG_DEBUG("AddUserGroupViewModel::findUserGroupId:{}", name.toStdString());

    // 2023.01.04. coverity
    std::map<Wisenet::uuid_string, Wisenet::Core::UserGroup> groups;
    if (db != nullptr) {
        groups = db->GetUserGroups();
    }

    for(auto &group : groups){
        if(group.second.name == name.toStdString()){
            SPDLOG_DEBUG("AddUserGroupViewModel::findUserGroupId:{} found", group.first);
            return QString::fromStdString(group.first);
        }
    }

    SPDLOG_DEBUG("AddUserGroupViewModel::findUserGroupId:{} couldn't find");

    return "";
}

void AddUserGroupViewModel::readUserGroup(QString localGroupId, bool groupView)
{
    // 2022.12.27. coverity
    auto db = QCoreServiceManager::Instance().DB();
    if (db == nullptr) {
        return;
    }

    SPDLOG_DEBUG("c:{}", localGroupId.toStdString());

    // 2023.01.04. coverity
    std::map<Wisenet::uuid_string, Wisenet::Core::UserGroup> groups;
    if (db != nullptr) {
        groups = db->GetUserGroups();
    }
    auto group = groups.find(localGroupId.toStdString());

    QStringList localGroupNameList;
    for(auto &item : groups){
        SPDLOG_DEBUG("AddUserGroupViewModel::readUser combo name:{}", item.second.name);
        localGroupNameList.append(QString::fromStdString(item.second.name));
    }
    localGroupNameList.append("New user group");
    if(!groupView)
        setGroupNameList(localGroupNameList);

    if(group == groups.end() || localGroupId == "" || localGroupId.length() < 1) {
        setGroupId(QString::fromStdString(boost::uuids::to_string(boost::uuids::random_generator()())));
        setGroupName("");

        setPlayback(false);
        setExportVideo(false);
        setLocalRecording(false);
        setPtzControl(false);
        setAudio(false);
        setMic(false);
        setIsAdminGroup(false);
        setIsAllResource(true);
        setChannelGuidList({});
        setLayoutGuidList({});
        return;
    }

    SPDLOG_DEBUG("AddUserGroupViewModel::readGroup Name:{} mic:{}", group->second.name, group->second.userPermission.mic);
    setGroupId(QString::fromStdString(group->first));
    setGroupName(QString::fromStdString(group->second.name));
    setPlayback(group->second.userPermission.playback);
    setExportVideo(group->second.userPermission.exportVideo);
    setLocalRecording(group->second.userPermission.localRecording);
    setPtzControl(group->second.userPermission.ptzControl);
    setAudio(group->second.userPermission.audio);
    setMic(group->second.userPermission.mic);
    setIsAdminGroup(group->second.isAdminGroup);
    setIsAllResource(group->second.accessAllResources);
    QVector<QString> channelList;
    QVector<QString> layoutList;
    if(group->second.assignedResource){
        for(auto& device : group->second.assignedResource->devices){
            for(auto& channel : device.second.channels){
                SPDLOG_DEBUG("AddUserGroupViewModel::readGroup device:{} channel:{}", device.first, channel);
                channelList.push_back(QString::fromStdString(device.first + "_" + channel));
            }
        }
        for(int i= 0; i < (int)group->second.assignedResource->layouts.size(); i++){
            SPDLOG_DEBUG("AddUserGroupViewModel::readGroup layout:{}", group->second.assignedResource->layouts.at(i));
            layoutList.push_back(QString::fromStdString(group->second.assignedResource->layouts.at(i)));
        }
    }
    setChannelGuidList(channelList);
    setLayoutGuidList(layoutList);
}

void AddUserGroupViewModel::saveUserGroup(bool isAllDevice, std::vector<QString> channelList, std::vector<QString> layoutList)
{
    auto db = QCoreServiceManager::Instance().DB();
    if (db == nullptr) {
        return;
    }

    SPDLOG_DEBUG("AddUserGroupViewModel::saveUserGroup Name:{} {}", m_groupName.toStdString(), isAllDevice);

    // 2022.12.29. coverity
    if (db != nullptr) {
        // 2023.01.04. coverity
        std::map<Wisenet::uuid_string, Wisenet::Core::UserGroup> groups;
        if (db != nullptr) {
            groups = db->GetUserGroups();
        }
        for (auto& group : groups) {
            if (group.second.name == m_groupName.toStdString() && group.first != m_groupId.toStdString()) {
                emit resultMessage(QCoreApplication::translate("WisenetLinguist", "Duplicated name"));
                return;
            }
        }
    }

    auto request = std::make_shared<Wisenet::Core::SaveUserGroupRequest>();

    Wisenet::Core::UserGroup userGroup;
    userGroup.userGroupID = m_groupId.toStdString();
    userGroup.name = m_groupName.toStdString();
    userGroup.isAdminGroup = m_isAdminGroup;
    userGroup.userPermission.playback = m_playback;
    userGroup.userPermission.exportVideo = m_exportVideo;
    userGroup.userPermission.localRecording = m_localRecording;
    userGroup.userPermission.ptzControl = m_ptzControl;
    userGroup.userPermission.audio = m_audio;
    userGroup.userPermission.mic = m_mic;

    Wisenet::Core::UserGroup::Resource resource;
    if(isAllDevice)
        userGroup.accessAllResources = true;
    else {
        for(auto& channel : channelList){
            //SPDLOG_DEBUG("AddUserGroupViewModel::saveUserGroup channel:{}", channel.toStdString());
            auto tokens = channel.split('_');
            if(tokens.size()==2){
                auto devId = tokens.at(0).toStdString();
                auto chId = tokens.at(1).toStdString();
                auto deviceResource = resource.devices.find(devId);
                if(deviceResource != resource.devices.end()){
                    auto it = find(deviceResource->second.channels.begin(), deviceResource->second.channels.end(), chId);
                    if(it == deviceResource->second.channels.end()){
                        deviceResource->second.channels.emplace(chId);
                        //SPDLOG_DEBUG("AddUserGroupViewModel::saveUserGroup #2 devId:{} chId:{}", devId, chId);
                    }
                }
                else{
                    Wisenet::Core::UserGroup::Resource::Device device;
                    device.channels.emplace(chId);
                    resource.devices.emplace(devId, device);
                   // SPDLOG_DEBUG("AddUserGroupViewModel::saveUserGroup #1 devId:{} chId:{}", devId, chId);
                }
            }
        }
    }

    for(auto & layout : layoutList){
        SPDLOG_DEBUG("AddUserGroupViewModel::saveUserGroup #1 layout:{}", layout.toStdString());
        resource.layouts.push_back(layout.toStdString());
    }
    userGroup.assignedResource = resource;

    request->userGroup = userGroup;

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::SaveUserGroup,
                this, request,
                [=](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::ResponseBase>(reply->responseDataPtr);
        if(response->isSuccess())
            emit resultMessage("Success", m_groupId);
        else
            emit resultMessage(QString::fromStdString(response->errorString()), "");
        qDebug() << "AddUserGroupViewModel::saveUserGroup() response:" << QString::fromStdString(response->errorString());
    });
}

void AddUserGroupViewModel::coreServiceEventTriggered(QCoreServiceEventPtr event)
{
    auto serviceEvent = event->eventDataPtr;

    switch(serviceEvent->EventTypeId()){
    case Wisenet::Core::SaveUserGroupEventType:
    case Wisenet::Core::RemoveUserGroupsEventType:
    case Wisenet::Core::SaveLayoutEventType:
    case Wisenet::Core::RemoveLayoutsEventType:
        readUserGroup(m_groupId, true);
    }
}


