#include "UserGroupModel.h"

UserGroupModel::UserGroupModel(QObject *parent) : QObject(parent)
{
    connect(&QCoreServiceManager::Instance(), &QCoreServiceManager::coreServiceEventTriggered,
            this, &UserGroupModel::coreServiceEventTriggered, Qt::QueuedConnection);
}

void UserGroupModel::coreServiceEventTriggered(QCoreServiceEventPtr event)
{
    auto serviceEvent = event->eventDataPtr;

    if(serviceEvent->EventTypeId() == Wisenet::Core::FullInfoEventType)
    {
        loginCompleted();
    }
}

QString UserGroupModel::userId()
{
    Wisenet::Core::User user;
    QCoreServiceManager::Instance().DB()->GetLoginUser(user);

    return QString::fromUtf8(user.loginID.c_str());
}

bool UserGroupModel::isAdmin()
{
    Wisenet::Core::User user;
    QCoreServiceManager::Instance().DB()->GetLoginUser(user);

    if(user.userType == Wisenet::Core::User::UserType::Owner || user.userType == Wisenet::Core::User::UserType::Admin)
        return true;

    return false;
}

bool UserGroupModel::playback()
{
    Wisenet::Core::UserGroup usergroup;
    QCoreServiceManager::Instance().DB()->GetLoginUserGroup(usergroup);

    return usergroup.userPermission.playback;
}

bool UserGroupModel::exportVideo()
{
    Wisenet::Core::UserGroup usergroup;
    QCoreServiceManager::Instance().DB()->GetLoginUserGroup(usergroup);

    return usergroup.userPermission.exportVideo;
}

bool UserGroupModel::ptzControl()
{
    Wisenet::Core::UserGroup usergroup;
    QCoreServiceManager::Instance().DB()->GetLoginUserGroup(usergroup);

    return usergroup.userPermission.ptzControl;
}

bool UserGroupModel::audio()
{
    Wisenet::Core::UserGroup usergroup;
    QCoreServiceManager::Instance().DB()->GetLoginUserGroup(usergroup);

    return usergroup.userPermission.audio;
}

bool UserGroupModel::mic()
{
    Wisenet::Core::UserGroup usergroup;
    QCoreServiceManager::Instance().DB()->GetLoginUserGroup(usergroup);

    return usergroup.userPermission.mic;
}

void UserGroupModel::loginCompleted()
{
    emit userIdChanged();
    emit isAdminChanged();
    emit playbackChanged();
    emit exportVideoChanged();
    emit ptzControlChanged();
    emit audioChanged();
    emit micChanged();
}

