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
#include "CoreService.h"
#include "CoreServiceLogSettings.h"
#include "WeakCallback.h"
#include "ThreadPool.h"
#include "CoreServiceUtil.h"

#include "DatabaseManager.h"
#include "UserSession.h"
#include "EventRuleManager.h"

namespace Wisenet
{
namespace Core
{

void CoreService::processEvent(const EventBaseSharedPtr& event)
{
    asio::post(m_strand,
               WeakCallback(shared_from_this(),
                            [this, event]()
    {
        /* Check Event for db update */
        SPDLOG_DEBUG("CoreService::processEvent!! EventType={}", event->EventType());
        if (event->EventTypeId() == Device::DeviceEventType::DeviceStatusEventType)
        {
            Device::DeviceStatusEventSharedPtr deviceStatusEvent =
                    std::static_pointer_cast<Device::DeviceStatusEvent>(event);

            m_eventRuleManager->ProcessEvent(event);
            m_databaseManager->SaveDeviceStatus(deviceStatusEvent->deviceStatus);

            if (deviceStatusEvent->deviceProfileInfo)
            {
                m_databaseManager->SaveDeviceProfileInfo(deviceStatusEvent->deviceProfileInfo.value());

                //  PLM 215079, 215094, 215095, 215105, 215242, 215247 - Device status event 처리 시 이름이 공백으로 업데이트 되는 현상 수정
                if (deviceStatusEvent->device.has_value())
                {
                    m_databaseManager->FillDeviceInfoFromDB(deviceStatusEvent->device.value(), deviceStatusEvent->deviceProfileInfo.value());
                }

                // remove device profile info in user event
                deviceStatusEvent->deviceProfileInfo.reset();
            }
        }
        else if (event->EventTypeId() == Device::DeviceEventType::DeviceProfileInfoEventType)
        {
            Device::DeviceProfileInfoEventSharedPtr deviceProfileInfoEvent =
                    std::static_pointer_cast<Device::DeviceProfileInfoEvent>(event);
            m_databaseManager->SaveDeviceProfileInfo(deviceProfileInfoEvent->deviceProfileInfo);
        }
        else if (event->EventTypeId() == Wisenet::Device::ChannelStatusEventType)
        {
            Device::ChannelStatusEventSharedPtr channelStatusEvent = std::static_pointer_cast<Device::ChannelStatusEvent>(event);
            m_databaseManager->FillAndSaveChannelStatusFromDB(channelStatusEvent->deviceID,
                                                              channelStatusEvent->channelsStatus);
        }
        else if(Device::DeviceEventType::DeviceAlarmEventType == event->EventTypeId())
        {
            m_eventRuleManager->ProcessEvent(event);
            return;
        }
        else if(Device::DeviceEventType::S1DeviceConnectionTypeChangeEventType == event->EventTypeId())
        {
            Device::S1DeviceConnectionTypeChangeEventSharedPtr s1DeviceConnectionTypeChangeEvent =
                    std::static_pointer_cast<Device::S1DeviceConnectionTypeChangeEvent>(event);
            m_databaseManager->SaveDevice(s1DeviceConnectionTypeChangeEvent->device);
            return;
        }

        m_userSession->ProcessEvent(event);
    }));
}

}
}
