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
#include "AlarmOutActionManager.h"
#include "LogSettings.h"

namespace Wisenet
{
namespace Core
{

AlarmOutActionManager::AlarmOutActionManager()
    :m_alarmOutputControlHandler(nullptr)
{

}

void AlarmOutActionManager::SetAlarmOutControlHandler(const AlarmOutputControlHandler &handler)
{
    m_alarmOutputControlHandler = handler;
}

void AlarmOutActionManager::PushAction(const uuid_string& eventRuleID, const EventRule::AlarmOutputAction &action)
{
    if(-1 == action.intervalSeconds){
        setAlarmOutputControl(action.itemIDs, true);
    }else if(0 == action.intervalSeconds){
        setAlarmOutputControl(action.itemIDs, false);
    }else{

        SPDLOG_DEBUG("AlarmOutActionManager:: eventRuleID({})",eventRuleID);
        EventRule::AlarmOutputAction alarmOutputAction = action;
        alarmOutputAction.intervalSeconds++; // Timer가 계속 돌고 있는 상태라 보정을 위해 1초를 더한다.
        auto itor = m_alarmOutputIntervals.find(eventRuleID);

        if(itor == m_alarmOutputIntervals.end()){
            m_alarmOutputIntervals.emplace(eventRuleID, alarmOutputAction);
            setAlarmOutputControl(action.itemIDs,true);
        }else{
            m_alarmOutputIntervals[eventRuleID] = alarmOutputAction;
        }

    }
}

void AlarmOutActionManager::ReduceActionInterval()
{
    for(auto itor = m_alarmOutputIntervals.begin(); itor != m_alarmOutputIntervals.end(); ){

        auto action = itor->second;
        action.intervalSeconds--;

        SPDLOG_DEBUG("AlarmOutActionManager:: eventRuleID({}), intervalSeconds({})",itor->first, action.intervalSeconds);

        if(0 < action.intervalSeconds){
            m_alarmOutputIntervals[itor->first] = action;
            ++itor;
        }else{
            setAlarmOutputControl(action.itemIDs,false);
            itor = m_alarmOutputIntervals.erase(itor);
        }
    }

}

void AlarmOutActionManager::setAlarmOutputControl(const std::set<EventRule::ItemID> &itemIDs, bool on)
{
    for(auto& itemID : itemIDs){
        if(m_alarmOutputControlHandler){
            m_alarmOutputControlHandler(itemID.deviceID, itemID.alarmID, on);
        }
    }
}

}
}
