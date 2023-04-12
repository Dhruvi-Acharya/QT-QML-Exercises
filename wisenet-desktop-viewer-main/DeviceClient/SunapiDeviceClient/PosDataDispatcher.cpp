#include "PosDataDispatcher.h"

#include <boost/foreach.hpp>
#include <vector>
#include <time.h>

#include "TimeUtil.h"
#include "SunapiDeviceClientLogSettings.h"
#include "SunapiDeviceClientUtil.h"
#include "LogSettings.h"

namespace Wisenet
{
namespace Device
{

void PosDataDispatcher::ParseNewEvent(const std::string& eventString)
{
    std::vector<std::string> lines;
    boost::split(lines, eventString, boost::is_any_of("\r\n"), boost::token_compress_on);

    // ToDo : Data Parse -> 이벤트 전달
    long long receivedDate = 0;
    std::string deviceId = "";
    std::string receipt = "";

    for(auto& line: lines) //BOOST_FOREACH(auto line, lines)
    {
        if (boost::contains(line, "="))
        {
            std::vector<std::string> tokens;

            if (boost::contains(line, "ReceivedDate"))
            {
                // Parse Received Date
                //ReceivedDate=2022-10-28T05:58:10Z
                boost::split(tokens, line, boost::is_any_of("="), boost::token_compress_on);
                receivedDate = Common::utcMsecsFromRfc3339(tokens[1]);
            }
            else if (boost::contains(line, "DeviceID"))
            {
                // DeviceID=1 (start from 0)
                boost::split(tokens, line, boost::is_any_of("="), boost::token_compress_on);
                deviceId = tokens[1];
            }
            else if (boost::contains(line, "Receipt"))
            {
                // Receipt=string...
                boost::split(tokens, line, boost::is_any_of("="), boost::token_compress_on);
                receipt = tokens[1];
            }
        }
    }

    if(receivedDate == 0 || deviceId == "" || receipt == "") {
        return;
    }
    else {
        SendPosEvent(deviceId, receivedDate, receipt);
    }
}

DeviceEventBaseSharedPtr PosDataDispatcher::SendPosEvent(const std::string& posID, const int64_t receivedDateTimeUtc,  const std::string& receipt)
{
    SPDLOG_DEBUG("PosDataDispatcher::SendPosEvent() {} {} {}", receivedDateTimeUtc, posID, receipt);

    /*
    // ToDo : DeviceEventBase를 상속받은 PosEvent 구조체 정의 후 이벤트를 생성하여 eventCallback으로 보냄
    PosEventSharedPtr posEvent = std::make_shared<PosEvent>();
    eventCallback(posEvent);
    */

    return nullptr;
}

}
}
