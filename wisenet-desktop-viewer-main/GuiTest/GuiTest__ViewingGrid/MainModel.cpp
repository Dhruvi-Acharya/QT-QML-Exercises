/*********************************************************************************
 * Copyright(c) 2021 by Hanwha Techwin Co., Ltd.
 *
 * This software is copyrighted by, and is the sole property of Hanwha Techwin.
 * All rigths, title, ownership, or other interests in the software remain the
 * property of Hanwha Techwin. This software may only be used in accordance with
 * the corresponding license agreement. Any unauthorized use, duplication,
 * transmission, distribution, or disclosure of this software is expressly
 * forbidden.
 *
 * This Copyright notice may not be removed or modified without prior written
 * consent of Hanwha Techwin.
 *
 * Hanwha Techwin reserves the right to modify this software without notice.
 *
 * Hanwha Techwin Co., Ltd.
 * KOREA
 * https://www.hanwha-security.com/
 *********************************************************************************/
#include "MainModel.h"
#include "LogSettings.h"
#include "DatabaseManager.h"
#include "LogManager.h"
#include <QQmlEngine>

#define SERVICE_RESET
MainModel::MainModel(QObject *parent)
    : QObject(parent)
{
    connect(&QCoreServiceManager::Instance(), &QCoreServiceManager::coreServiceEventTriggered,
            this, &MainModel::onCoreServiceEventTriggered, Qt::QueuedConnection);

    m_mediaLayout = new MediaLayoutViewModel();
    m_mediaLayout->setName("New Layout1");
    m_mediaLayout->setLayoutId("testLayoutModel!!");
}

void MainModel::serviceStart()
{
    ::qRegisterMetaType<DeviceInfo>("DeviceInfo");

#ifdef SERVICE_RESET
    Wisenet::Core::DatabaseManager::Delete();
    Wisenet::Core::LogManager::Delete();
#endif
    QCoreServiceManager::Instance().Start();
    SPDLOG_DEBUG("service Start!!");

#ifdef SERVICE_RESET
    auto request = std::make_shared<Wisenet::Core::SetupServiceRequest>();
    request->userName = "admin";
    request->newPassword = "5tkatjd!";
    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::SetupService,
                this, request,
                [this](const QCoreServiceReplyPtr& reply)
    {
#endif
        QCoreServiceManager::Instance().Login(
                    "admin", "5tkatjd!", this,
                    [this](const QCoreServiceReplyPtr& reply)
        {
            SPDLOG_DEBUG("MAIN MODEL LOG IN COMPLETED");
        });
#ifdef SERVICE_RESET
    });
#endif
}

MediaLayoutViewModel *MainModel::layout() const
{
    return m_mediaLayout;
}


void MainModel::registerDevice(QString ip, QString pw)
{
    auto request = std::make_shared<Wisenet::Core::AddDeviceRequest>();
    request->deviceID = QCoreServiceManager::Instance().CreateNewUUidString();
    request->connectionInfo.host = ip.toStdString();
    request->connectionInfo.port = 80;
    request->connectionInfo.user = "admin";
    request->connectionInfo.password = pw.toUtf8().toStdString();

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::AddDevice,
                this, request,
                [this, request](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::ResponseBase>(reply->responseDataPtr);
        if (response->isFailed()) {
            SPDLOG_DEBUG("REGISTER FAILED, {}", request->connectionInfo.host);
        }
        else {
            SPDLOG_DEBUG("REGISTER SUCCESSED, {}", request->connectionInfo.host);
        }
    });
}

void MainModel::addWebpage(QString name, QString url, bool useAuth, QString id, QString pw)
{
    auto request = std::make_shared<Wisenet::Core::SaveWebpageRequest>();
    request->webpage.webpageID = QCoreServiceManager::Instance().CreateNewUUidString();
    request->webpage.name = name.toStdString();
    request->webpage.url = url.toStdString();
    if (useAuth) {
        request->webpage.useAuth = true;
        request->webpage.userId = id.toStdString();
        request->webpage.userPw = pw.toStdString();
    }

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::SaveWebpage,
                this, request,
                [this, request](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::ResponseBase>(reply->responseDataPtr);
        if (response->isFailed()) {
            SPDLOG_DEBUG("ADD WEBPAGE FAILED");
        }
        else {
            SPDLOG_DEBUG("ADD WEBPAGE SUCCESSED");
        }
    });
}

void MainModel::onCoreServiceEventTriggered(QCoreServiceEventPtr event)
{
    auto serviceEvent = event->eventDataPtr;
    SPDLOG_DEBUG("CoreServiceEvent!! EventType={}", serviceEvent->EventType());
    if (serviceEvent->EventTypeId() == Wisenet::Core::FullInfoEventType) {
        auto fullInfoEvent = std::static_pointer_cast<Wisenet::Core::FullInfoEvent>(event->eventDataPtr);
        SPDLOG_DEBUG("CoreServiceEvent!! devices={}", fullInfoEvent->devices.size());

        for (auto& deviceInfo : fullInfoEvent->devices) {
            auto device = deviceInfo.second;
            auto deviceStatus = fullInfoEvent->devicesStatus[deviceInfo.first];
            setDevice(device, deviceStatus);
        }

        for (auto& layoutInfo : fullInfoEvent->layouts) {
            auto layout = layoutInfo.second;
            if (layout.layoutID == "testLayoutModel!!") {
                setLayout(layout);
            }
        }
    }

    else if (serviceEvent->EventTypeId() ==  Wisenet::Device::DeviceStatusEventType) {
        auto statusEvent = std::static_pointer_cast<Wisenet::Device::DeviceStatusEvent>(event->eventDataPtr);
        updateDevice(statusEvent->deviceStatus);
    }

    else if (serviceEvent->EventTypeId() == Wisenet::Core::AddDeviceEventType) {
        auto coreEvent = std::static_pointer_cast<Wisenet::Core::AddDeviceEvent>(event->eventDataPtr);

        auto device = coreEvent->device;
        auto deviceStatus = coreEvent->deviceStatus;
        setDevice(device, deviceStatus);
    }
    else if (serviceEvent->EventTypeId() == Wisenet::Core::SaveWebpageEventType) {
        auto coreEvent = std::static_pointer_cast<Wisenet::Core::SaveWebpageEvent>(event->eventDataPtr);
        setWebpage(coreEvent->webpage);
    }
}


void MainModel::setDevice(Wisenet::Device::Device &device, Wisenet::Device::DeviceStatus &deviceStatus)
{
    DeviceInfo deviceInfo;
    //deviceInfo.setName(QString::fromUtf8(device.modelName.c_str()));
    deviceInfo.setName(QString::fromStdString(device.modelName));
    deviceInfo.setIp(QString::fromStdString(device.connectionInfo.host));
    deviceInfo.setDeviceId(QString::fromStdString(device.deviceID));

    SPDLOG_DEBUG("SET DEVICE={}, channel={}", device.modelName, device.channels.size());
    for (auto& channel : deviceStatus.channelsStatus) {
        deviceInfo.setChannelId(QString::fromStdString(channel.first));
        QString status = "normal";
        if (!channel.second.isGood()) {
            status = "abnormal";
        }
        deviceInfo.setStatus(status);
        emit newDeviceRegistered(deviceInfo);
    }
}

void MainModel::updateDevice(Wisenet::Device::DeviceStatus &deviceStatus)
{
    DeviceInfo deviceInfo;
    deviceInfo.setDeviceId(QString::fromStdString(deviceStatus.deviceID));
    for (auto& channel : deviceStatus.channelsStatus) {
        deviceInfo.setChannelId(QString::fromStdString(channel.first));
        QString status = "normal";
        if (!channel.second.isGood()) {
            status = "abnormal";
        }
        deviceInfo.setStatus(status);
        emit deviceUpdated(deviceInfo);
    }
}

void MainModel::setLayout(Wisenet::Core::Layout& layout)
{
    SPDLOG_DEBUG("setLayout(), layoutID={}, items={}", layout.layoutID, layout.layoutItems.size());
    if (m_mediaLayout) {
        delete m_mediaLayout;
    }
    m_mediaLayout = new MediaLayoutViewModel();
    m_mediaLayout->loadFromCoreService(layout);
    emit layoutChanged();
}


void MainModel::setWebpage(Wisenet::Core::Webpage &webpage)
{
    SPDLOG_DEBUG("setWebpage(), webpageID={}, name={}, url={}, useAuth={}, id={}, pw={}",
                 webpage.webpageID, webpage.name, webpage.url,
                 webpage.useAuth, webpage.userId, webpage.userPw);

    WebpageInfo webpageInfo;
    webpageInfo.setWebpageId(QString::fromStdString(webpage.webpageID));
    webpageInfo.setName(QString::fromStdString(webpage.name));
    webpageInfo.setUrl(QString::fromStdString(webpage.url));
    emit newWebpageAdded(webpageInfo);
}
