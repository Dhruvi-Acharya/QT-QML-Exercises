/*********************************************************************************
 * Copyright(c) 2023 Hanwha Vision Co., Ltd. All rights reserved.
 *
 * This software is copyrighted by, and is the sole property of Hanwha Vision.
 * All rigths, title, ownership, or other interests in the software remain the
 * property of Hanwha Vision. This software may only be used in accordance with
 * the corresponding license agreement. Any unauthorized use, duplication,
 * transmission, distribution, or disclosure of this software is expressly
 * forbidden.
 *
 * This Copyright notice may not be removed or modified without prior written
 * consent of Hanwha Vision.
 *
 * Hanwha Vision reserves the right to modify this software without notice.
 *
 * Hanwha Vision Co., Ltd.
 * KOREA
 * https://www.hanwhavision.com/
 *********************************************************************************/
#include "DeviceListModel.h"
#include "QCoreServiceManager.h"
#include "LogSettings.h"

DeviceListModel::DeviceListModel(QObject* parent)
{
    Q_UNUSED(parent);
}

void DeviceListModel::connectionChange(QVector<QString> deviceList, bool connection)
{
    SPDLOG_DEBUG("DeviceListModel::connectionChange devices={} connection={}", deviceList.size(), connection);

    m_devices.clear();

    for(auto& deviceId : deviceList)
        m_devices.emplace(deviceId.toStdString());

    auto updateDevicesRequest = std::make_shared<Wisenet::Core::UpdateDevicesRequest>();
    for(auto& deviceId : m_devices){
        Wisenet::Device::Device device;
        if(QCoreServiceManager::Instance().DB()->FindDevice(deviceId, device)){
            device.connectionInfo.use = connection;
            updateDevicesRequest->devices.push_back(device);

            auto disconnectRequest = std::make_shared<Wisenet::Device::DeviceDisconnectRequest>();
            disconnectRequest->deviceID = deviceId;
            QCoreServiceManager::Instance().RequestToCoreService(
                        &Wisenet::Core::ICoreService::DeviceDisconnect,
                        this, disconnectRequest,
                        [=](const QCoreServiceReplyPtr& reply)
            {
                auto response = reply->responseDataPtr;
                if(response->isSuccess()){
                    qDebug() << "DeviceListModel::DisconnectDevice success";
                }
                else
                    qDebug() << "DeviceListModel::DisconnectDevice fail";
            });
        }
    }
    if(updateDevicesRequest->devices.size() > 0){
        QCoreServiceManager::Instance().RequestToCoreService(
                    &Wisenet::Core::ICoreService::UpdateDevices,
                    this, updateDevicesRequest,
                    [=](const QCoreServiceReplyPtr& reply)
        {

            auto response = std::static_pointer_cast<Wisenet::Core::UpdateDevicesResponse>(reply->responseDataPtr);

            if(response->isSuccess()){
                SPDLOG_DEBUG("DeviceListModel::SaveDevice success");
                if(response->devices.size()>0)
                    connectDevice(response->devices);
            }
            else{
                qDebug() << "DeviceListModel::SaveDevice fail";
            }
        });
    }
}


void DeviceListModel::connectDevice(std::vector<Wisenet::Device::Device> devices)
{
    SPDLOG_DEBUG("DeviceListModel::ConnectDevice device:{}", devices.size());//, m_channelGuidList.size());
    for(auto& device : devices){
        auto connectRequest = std::make_shared<Wisenet::Device::DeviceConnectRequest>();
        if(!device.connectionInfo.use)
            continue;
        connectRequest->deviceID = device.deviceID;
        connectRequest->connectionInfo = device.connectionInfo;

        QCoreServiceManager::Instance().RequestToCoreService(
                    &Wisenet::Core::ICoreService::DeviceConnect,
                    this, connectRequest,
                    [](const QCoreServiceReplyPtr& reply)
        {
            auto response = reply->responseDataPtr;
            if(response->isSuccess()){
                qDebug() << "DeviceListModel::ConnectDevice success";
            }
            else
                qDebug() << "DeviceListModel::ConnectDevice fail";
        });
    }
}

