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
#include "EmailManager.h"
#include "UserSession.h"
#include "EventRuleManager.h"
#include "AlarmOutActionManager.h"

#include "DeviceClient/DeviceRequestResponse.h"

namespace Wisenet
{
namespace Core
{
void CoreService::SetAlertAlarmAction()
{
    m_eventRuleManager->SetAlertAlarmActionHandler(WeakCallback(shared_from_this(), [this](const long long& serviceUtcTimeMsec, const long long& deviceUtcTimeMsec, const std::string& eventRuleName, const EventRule::AlertAlarmAction& action){

        asio::post(m_strand,
                   WeakCallback(shared_from_this(),
                                [this, serviceUtcTimeMsec, deviceUtcTimeMsec, eventRuleName, action]()
        {
            boost::ignore_unused(action);

            auto priorityAlarmEvent = std::make_shared<PriorityAlarmEvent>();
            priorityAlarmEvent->serviceUtcTimeMsec = serviceUtcTimeMsec;
            priorityAlarmEvent->deviceUtcTimeMsec = deviceUtcTimeMsec;

            priorityAlarmEvent->eventRuleName = eventRuleName;
            priorityAlarmEvent->intervalSeconds = action.intervalSeconds;

            for(auto& item : action.itemIDs){

                priorityAlarmEvent->deviceID = item.deviceID;
                priorityAlarmEvent->channelID = item.channelID;
                break;
            }

            SPDLOG_TRACE("{}", priorityAlarmEvent->ToString());
            m_userSession->ProcessEvent(priorityAlarmEvent);

        }));
    }));

}

void CoreService::SetEmailChangeAction()
{
    m_eventRuleManager->SetEmailActionHandler(WeakCallback(shared_from_this(), [this](const EventLogSharedPtr& info, const EventRule::EmailAction& action){

        asio::post(m_strand,
                   WeakCallback(shared_from_this(),
                                [this, info, action]()
        {
            //SPDLOG_DEBUG("=====> calling cb handler for emailChange... (1)" );
            auto emailChangeEvent = std::make_shared<EmailEvent>();
            emailChangeEvent->itemIDs = action.itemIDs;

            auto deviceInfo = info;

            EmailManager::Instance()->SetEventDeviceInfo(deviceInfo);
            EmailManager::Instance()->SetEmailList(action.itemIDs);

            //SPDLOG_DEBUG("=====> calling cb handler for emailChange... (3)" );
            m_userSession->ProcessEvent(emailChangeEvent);
        }));
    }));
}

void CoreService::SetLayoutChangeAction()
{
    m_eventRuleManager->SetLayoutChangeActionHandler(WeakCallback(shared_from_this(), [this](const EventRule::LayoutChangeAction& action){

        asio::post(m_strand,
                   WeakCallback(shared_from_this(),
                                [this, action]()
        {
            auto layoutChangeEvent = std::make_shared<OpenLayoutEvent>();
            layoutChangeEvent->layoutID = action.layoutID;
            SPDLOG_DEBUG("{}:{} [KHJ] calling cb handler for layoutChange...to[{}]", __FUNCTION__, __LINE__, action.layoutID );
            m_userSession->ProcessEvent(layoutChangeEvent);
        }));
    }));
}

void CoreService::SetAlarmOutAction()
{
    m_alarmOutputActionManager->SetAlarmOutControlHandler(WeakCallback(shared_from_this(), [this](const uuid_string& deviceID, const uuid_string& alarmOutputID, bool on){

        asio::post(m_strand,
                   WeakCallback(shared_from_this(),
                                [this, deviceID, alarmOutputID, on]()
        {
            Device::DeviceSetAlarmOutputRequestSharedPtr request = std::make_shared<Device::DeviceSetAlarmOutputRequest>();

            request->deviceID = deviceID;
            request->deviceOutputID = alarmOutputID;
            request->on = on;

            SPDLOG_INFO("Event Action AlarmOutput Control: deviceID({}),alarmOutputID({}),on({})",deviceID,alarmOutputID,on);

            DeviceSetAlarmOutput(request, nullptr);


        }));

    }));

    m_eventRuleManager->SetAlarmOutputActionHandler(WeakCallback(shared_from_this(), [this](const uuid_string& eventRuleID, const EventRule::AlarmOutputAction& action){

        asio::post(m_strand,
                   WeakCallback(shared_from_this(),
                                [this, eventRuleID, action]()
        {

            SPDLOG_INFO("Event Action AlarmOutput : eventRuleID({})",eventRuleID);
            m_alarmOutputActionManager->PushAction(eventRuleID, action);

        }));

    }));
}

void CoreService::SetWriteToLogAction()
{
    m_eventRuleManager->SetWriteToLogActionHandler(WeakCallback(shared_from_this(), [this](const EventLogSharedPtr& eventLog){


        asio::post(m_strand,
                   WeakCallback(shared_from_this(),
                                [this, eventLog]()
        {
            if(eventLog->isChannel){

                m_logManager->AddEventLog(eventLog);

                auto showEventPanelEvent = std::make_shared<ShowEventPanelEvent>();
                showEventPanelEvent->serviceUtcTimeMsec = eventLog->serviceUtcTimeMsec;
                showEventPanelEvent->intervalSeconds = 0;
                showEventPanelEvent->eventLog = *eventLog;
                m_userSession->ProcessEvent(showEventPanelEvent);

            }else{
                m_logManager->AddSystemLog(eventLog);

                auto showSystemAlarmPanelEvent = std::make_shared<ShowSystemAlarmPanelEvent>();
                showSystemAlarmPanelEvent->serviceUtcTimeMsec = eventLog->serviceUtcTimeMsec;
                showSystemAlarmPanelEvent->intervalSeconds = 0;
                showSystemAlarmPanelEvent->eventLog = *eventLog;
                m_userSession->ProcessEvent(showSystemAlarmPanelEvent);
            }

        }));

    }));
}

}
}
