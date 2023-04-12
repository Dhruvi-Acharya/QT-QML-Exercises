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
#pragma once

#include <map>
#include <CoreService/CoreServiceStructure.h>
#include "DeviceClient/DeviceEvent.h"

namespace Wisenet
{
namespace Core
{

typedef std::function <void(const uuid_string&, const uuid_string&, bool)>  AlarmOutputControlHandler;
class AlarmOutActionManager : public std::enable_shared_from_this<AlarmOutActionManager>
{
public:
    AlarmOutActionManager();

    void SetAlarmOutControlHandler(AlarmOutputControlHandler const& handler);

    void PushAction(const uuid_string& eventRuleID, const EventRule::AlarmOutputAction& action);
    void ReduceActionInterval();

private:
    void setAlarmOutputControl(const std::set<EventRule::ItemID>& itemIDs, bool on);

    std::map<std::string, EventRule::AlarmOutputAction> m_alarmOutputIntervals;

    AlarmOutputControlHandler m_alarmOutputControlHandler;
};

}
}
