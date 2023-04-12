#pragma once

#include <string>
#include <map>
#include <set>

#include "DeviceClient/DeviceEvent.h"
#include "DeviceObjectComposer.h"

namespace Wisenet
{
namespace Device
{


class PosDataDispatcher
{
public:
    PosDataDispatcher(const std::string& logPrefix)
        :m_logPrefix(logPrefix)
    {

    }
    ~PosDataDispatcher()
    {
        m_eventHandler = nullptr;
        m_repos = nullptr;
    }

    void SetEventHandler(DeviceEventBaseHandler const& eventHandler)
    {
        m_eventHandler = eventHandler;
    }

    void ResetObjectComposer(DeviceObjectComposerPtr& oc)
    {
        m_repos = oc;
    }

    void ParseNewEvent(const std::string& eventString);

protected:
    void eventCallback(const DeviceEventBaseSharedPtr& event)
    {
        if (m_eventHandler)
            m_eventHandler(event);
    }

private:
    DeviceEventBaseSharedPtr SendPosEvent(const std::string& posID, const int64_t receivedDateTimeUtc,  const std::string& receipt);

    DeviceObjectComposerPtr m_repos = nullptr;
    DeviceEventBaseHandler  m_eventHandler = nullptr;
    std::string m_logPrefix;
};


}
}
