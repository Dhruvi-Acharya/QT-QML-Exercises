#include "EventStatusDispatcher.h"

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

EventStatusDispatcher::EventStatusDispatcher(const std::string& logPrefix)
    :IEventStatusDispatcher(logPrefix)
{
}

EventStatusDispatcher::~EventStatusDispatcher()
{
}

void EventStatusDispatcher::ParseNewEvent(const std::string& eventString)
{
    std::vector<std::string> lines;
    boost::split(lines, eventString, boost::is_any_of("\r\n"), boost::token_compress_on);
    long long timeStamp = 0;

    for(auto& line: lines) //BOOST_FOREACH(auto line, lines)
    {
        if (boost::contains(line, "="))
        {
            std::vector<std::string> tokens;

            if (boost::contains(line, "Timestamp"))
            {
                // Parse timestamp
                boost::split(tokens, line, boost::is_any_of("="), boost::token_compress_on);
                timeStamp = Common::utcMsecsFromRfc3339(tokens[1]);
            }
            else if (boost::contains(line, "ChangedConfigURI"))
            {
                std::string queryToken = "ChangedConfigURI";
                std::string cgi = line.substr(queryToken.length() + 1, line.length() - 1);

                changeConfigUriCallback(cgi);
            }
            else
            {
                boost::split(tokens, line, boost::is_any_of("="), boost::token_compress_on);

                if (tokens.size() == 2)
                {
                    bool received = tokens[1] == "False" ? false : true;

                    if(tokens[0] == "SystemEvent.ConfigRestore" && received){
                        SPDLOG_DEBUG("tokens[0] == SystemEvent.ConfigRestore");
                       changeConfigUriCallback("security.cgi?msubmenu=users");
                    }
                    else
                        ParseAndSendAlarm(timeStamp, tokens[0], received);


                    // 초기 상태에서도 True는 다 처리함.
                    //if (m_eventMap.find(tokens[0]) == m_eventMap.end())
                    //{
                    //    m_eventMap.insert(make_pair(tokens[0], received));
                    //
                    //    //초기 상태가 True인 경우는 전달. 일단 주석.
                    //    //if(received){
                    //    //    ParseAndSendAlarm(timeStamp, tokens[0], received);
                    //    //}
                    //
                    //    // AlarmOutput은 Status를 올려야 하기 때문에 항상 전달한다.
                    //    if(0 == tokens[0].compare(0,11,"AlarmOutput")){
                    //        ParseAndSendAlarm(timeStamp, tokens[0], received);
                    //    }
                    //}
                    //else
                    //{
                    //    if (m_eventMap[tokens[0]] != received)
                    //    {
                    //        m_eventMap[tokens[0]] = received;
                    //        // Send event
                    //        ParseAndSendAlarm(timeStamp, tokens[0], received);
                    //    }
                    //}
                }
            }
        }
    }
}

void EventStatusDispatcher::ParseAndSendAlarm(const long long& timeStamp, const std::string& category, bool data)
{
    std::vector<std::string> tokens;
    boost::split(tokens, category, boost::is_any_of("."), boost::token_compress_on);

    if (tokens.size() <= 1)
        return;

    std::string channelType = tokens[0];
    std::string eventType;
    bool isDevice = false;
    bool isChannel = false;

    std::string channelID("");
    std::string profileID("");
    std::string systemID("");
    std::map<std::string, std::string> sourceParameters;
    if (channelType == "SystemEvent")
    {
        isDevice = true;
        eventType = tokens[0] + "." + tokens[1];

        // channel number
        if(tokens.size() >= 3)
        {
            sourceParameters["SystemID"] = tokens[2];
        }

        SendDeviceLatestStatusEventBase(eventType, timeStamp, data);
    }
    else if (channelType == "AlarmInput") // AlarmInput.1, AlarmOutput.1
    {
        isDevice = true;
        eventType = tokens[0];
        sourceParameters["SystemID"] = tokens[1];
    }
    else if(channelType == "AlarmOutput")
    {
        eventType = tokens[0];
        systemID = tokens[1];
    }
    else if (channelType == "POS") // POS.0.KeywordMatch
    {
        eventType = tokens[0];
        channelID = tokens[1];
    }
    else if (channelType == "Channel")
    {
        isChannel = true;
        channelID = tokens[1];

        if (tokens.size() == 3) // Normal event
        {
            eventType = tokens[2];
            if((0 == eventType.compare("SDFormat"))
                        || (0 == eventType.compare("SDFail"))
                        || (0 == eventType.compare("SDFull"))
                        || (0 == eventType.compare("SDInsert"))
                        || (0 == eventType.compare("SDRemove"))
                        || (0 == eventType.compare("LowFps"))){

                eventType = "SystemEvent." + tokens[2];
                isDevice = true;
                isChannel = false;
                channelID = "";
                sourceParameters["ChannelID"] = fromSunapiChannel(tokens[1]);

                SendDeviceLatestStatusEventBase(eventType, timeStamp, data);
            }
            else if(0 == eventType.compare("Videoloss")){
                if(data){
                    eventType = "Videoloss.Start";
                }else{
                    eventType = "Videoloss.End";
                    //연결 후 처음 발생되는 이벤트를 제외하기 위하여 처리
                    if(m_eventMap.find(category) == m_eventMap.end()){
                        m_eventMap.insert(make_pair(category, data));
                        return;
                    }
                }
            }
            else if(0 == eventType.compare("Tracking")){
                if(data){
                    eventType = "Tracking.Start";
                }else{
                    eventType = "Tracking.End";
                    //연결 후 처음 발생되는 이벤트를 제외하기 위하여 처리
                    if(m_eventMap.find(category) == m_eventMap.end()){
                        m_eventMap.insert(make_pair(category, data));
                        return;
                    }
                }
            }
        }
        else if (tokens.size() == 4) // Channel.0.VideoAnalytics.~~~~~ , Channel.0.AudioAnalytics.~~~~~
        {
            isChannel = true;
            if(0 == tokens[3].compare("Disappering")){
                eventType = tokens[2] + "." + "Disappearing"; //예외 처리.
            }else{
                eventType = tokens[2] + "." + tokens[3];
            }
        }
        else if (tokens.size() == 5) // Profile event
        {
            isChannel = true;
            if (tokens[2] == "Profile")
            {
                profileID = tokens[3];
                eventType = tokens[4];

            } else if(tokens[2] == "MotionDetection") // Channel.<int>.MotionDetection.RegionID.<int>
            {
                eventType = tokens[2];
                sourceParameters[tokens[3]] = tokens[4];
            } else {
                //일단 예외 처리.
                eventType = tokens[2] + "." + tokens[3] + "." + tokens[4];
            }

        }else if (tokens.size() == 6) // Channel.<int>.VideoAnalytics.Loitering.DefinedAreaID.<int>
        {
            isChannel = true;
            if(0 == tokens[3].compare("Disappering")){
                eventType = tokens[2] + "." + "Disappearing"; //예외 처리.
            }else{
                eventType = tokens[2] + "." + tokens[3];
            }
            sourceParameters[tokens[4]] = tokens[5];
        }else if (tokens.size() == 7)
        {
            isChannel = true;
            if((0 == tokens[2].compare("ObjectDetection"))
                    && (0 == tokens[3].compare("Detail"))){

                eventType = tokens[2] + "." + tokens[4];
                sourceParameters[tokens[5]] = tokens[6];

            }else {
                //일단 예외 처리
                eventType = tokens[2] + "." + tokens[3] + "." + tokens[4] + "." + tokens[5] + "." + tokens[6];
            }
        }else{
            //일단 예외 처리
            size_t size = tokens.size();
            for(size_t i = 2; i < size; i++){

                eventType += tokens[i];
                if(size != (i + 1)){
                    eventType += ".";
                }
            }
        }
    }

    if(0 == eventType.size()){
        SPDLOG_WARN("Unknown Event Type = {}", category);
        return;
    }

    auto eventBase = CreateDeviceEventBase(eventType, timeStamp, isDevice, isChannel, channelID, profileID, systemID, sourceParameters, data);

    if(eventBase){
        eventCallback(eventBase);
    }

}

DeviceEventBaseSharedPtr EventStatusDispatcher::CreateDeviceEventBase(const std::string &eventType, const long long &timeStamp,
                                                                      bool isDevice, bool isChannel,
                                                                      const std::string &channelID, const std::string &profileID, const std::string &systemID,
                                                                      const std::map<std::string, std::string>& parameters,
                                                                      const bool &data)
{
    boost::ignore_unused(profileID);

    // AlarmOutput은 Status로 처리하기로 함.
    if(0 == eventType.compare("AlarmOutput")) {
        AlarmOutputStatusEventSharedPtr event = std::make_shared<AlarmOutputStatusEvent>();

        event->deviceID = m_repos->device().deviceID;
        event->serviceUtcTimeMsec = Common::currentUtcMsecs();
        event->deviceUtcTimeMsec = timeStamp;
        event->deviceOutputID = systemID;
        event->status = data;

        changeAlarmOutputStatusCallback(systemID, data);

        SLOG_DEBUG("AlarmOutputStatus = {}", event->ToString());
        return event;
    }

    //None인 경우 알 수 없는 타입이므로 nullptr
    if(!IsSupportedEvent(eventType)){
        return nullptr;
    }

    DeviceAlarmEventSharedPtr alarm = std::make_shared<DeviceAlarmEvent>();

    alarm->serviceUtcTimeMsec = Common::currentUtcMsecs();
    alarm->deviceUtcTimeMsec = timeStamp;
    alarm->type = eventType;

    alarm->isDevice = isDevice;
    alarm->isChannel = isChannel;

    alarm->deviceID = m_repos->device().deviceID;
    if(!channelID.empty()){
        alarm->channelID = fromSunapiChannel(channelID);
    }

    alarm->data = data;

    if(parameters.size() > 0){
        alarm->sourceParameters = parameters;
    }

    SLOG_DEBUG("DeviceAlarm : {}", alarm->ToString());
    return alarm;
}

void EventStatusDispatcher::SendDeviceLatestStatusEventBase(const std::string &eventType, const long long &timeStamp, const bool &data)
{
    if(0 == eventType.compare("AlarmOutput")) {
        return;
    }

    //None인 경우 알 수 없는 타입이므로 nullptr
    if(!IsSupportedEvent(eventType)){
        return;
    }

    DeviceLatestStatusEventSharedPtr event = std::make_shared<DeviceLatestStatusEvent>();

    event->deviceUtcTimeMsec = timeStamp;
    event->eventKey = eventType;
    event->status = data;
    event->deviceID = m_repos->device().deviceID;

    deviceLatestEventCallback(event);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief SchemaBasedEventStatusDispatcher
///
/// /////////////////////////////////////////////////////////////////////////////////////////////////////////

SchemaBasedEventStatusDispatcher::SchemaBasedEventStatusDispatcher(const std::string& logPrefix)
    :IEventStatusDispatcher(logPrefix)
{

}
SchemaBasedEventStatusDispatcher::~SchemaBasedEventStatusDispatcher()
{

}

void SchemaBasedEventStatusDispatcher::ParseNewEvent(const std::string& eventString)
{
   rapidjson::Document document;

   size_t pos = eventString.find_first_of("{");
   if(pos == std::string::npos){
        SLOG_WARN("Not found json start character Event String: {}", eventString);
       return;
   }

   auto jsonString = eventString.substr(pos);

   SLOG_DEBUG("Json String: {}", jsonString);

   if (document.Parse(jsonString.c_str()).HasParseError()) {
       SLOG_ERROR("json parse error.. ErrorOffset={}, ErrorMessage={}", document.GetErrorOffset(), GetParseError_En(document.GetParseError()));
       return;
   }

   if(!document.HasMember("EventStatus")){
       SLOG_WARN("Not found EventStatus element");
       return;
   }

   const rapidjson::Value& eventStatus = document["EventStatus"];

   for(rapidjson::Value::ConstValueIterator event = eventStatus.Begin(); event != eventStatus.End(); ++event){

       if(!event->HasMember("EventName") || !event->HasMember("Time")){
           SLOG_DEBUG("EvnetName or Time isn't in a member.");
           continue;
       }
       if(!(*event)["EventName"].IsString() || !(*event)["Time"].IsString()){
           SLOG_DEBUG("EvnetName or Time isn't a string.");
           continue;
       }

       SLOG_DEBUG("EventName = {}", (*event)["EventName"].GetString());
       SLOG_DEBUG("Time = {}", (*event)["Time"].GetString());

       std::string eventName = (*event)["EventName"].GetString();
       std::string time = (*event)["Time"].GetString();

       // AlarmOutput과 ConfigChange는 따로 처리가 되어야 하므로 IsSupportedEvent 이전에 처리.
       if(0 == eventName.compare("AlarmOutput")){
           // AlarmOut은 Status로 처리되므로 별도로 처리함.
           TriggerAlarmOutput(time, event);
           continue;
       }else if(0 == eventName.compare("ConfigChange")){
           TriggerConfigChange(event);
           continue;
       }

       if(!IsSupportedEvent(eventName)){
           SLOG_DEBUG("Not Supported Event = {}", (*event)["EventName"].GetString());
           continue;
       }

       if(0 == eventName.compare(0,12,"SystemEvent.")){
           TriggerSystemEvent(eventName, time, event);
       }else if((0 == eventName.compare("SDFormat"))
                || (0 == eventName.compare("SDFail"))
                || (0 == eventName.compare("SDFull"))
                || (0 == eventName.compare("SDInsert"))
                || (0 == eventName.compare("SDRemove"))
                || (0 == eventName.compare("LowFps"))){

           std::string systemEventName = "SystemEvent." + eventName;
           TriggerSystemEvent(systemEventName, time, event);
       }
       else if(0 == eventName.compare("AlarmInput")){
           TriggerAlarmIn(eventName, time, event);
       }else{
           TriggerChannel(eventName, time, event);
       }
   }

}

void SchemaBasedEventStatusDispatcher::TriggerConfigChange(rapidjson::Value::ConstValueIterator event)
{
    const rapidjson::Value& source = (*event)["Source"];
    if(!source.IsObject()){
        SLOG_WARN("Source isn't object");
        return;
    }
    if(!source["ChangedConfigURI"].IsString()){
        SLOG_WARN("Channel isn't int.");
        return;
    }
    std::string url = source["ChangedConfigURI"].GetString();

    SLOG_DEBUG("ChangedConfigURI = {}", url);

    changeConfigUriCallback(url);
}

void SchemaBasedEventStatusDispatcher::TriggerAlarmOutput(const std::string& time, rapidjson::Value::ConstValueIterator event)
{

    AlarmOutputStatusEventSharedPtr alarmOutputStatus = std::make_shared<AlarmOutputStatusEvent>();

    alarmOutputStatus->serviceUtcTimeMsec = Common::currentUtcMsecs();
    alarmOutputStatus->deviceUtcTimeMsec = Common::utcMsecsFromRfc3339(time);
    alarmOutputStatus->deviceID = m_repos->device().deviceID;

    const rapidjson::Value& source = (*event)["Source"];
    if(!source.IsObject()){
        SLOG_WARN("Source isn't object");
        return;
    }

    if(!source["Channel"].IsInt()){
        SLOG_WARN("Channel isn't int.");
        return;
    }

    alarmOutputStatus->deviceOutputID = ToStringValue(source["Channel"]);


    const rapidjson::Value& data = (*event)["Data"];

    if(!data.IsObject()){
        SLOG_WARN("Data isn't object.");
        return;
    }

    if(!data["State"].IsBool()){
        SLOG_WARN("State isn't bool.");
        return;
    }

    alarmOutputStatus->status = data["State"].GetBool();

    changeAlarmOutputStatusCallback(alarmOutputStatus->deviceOutputID, alarmOutputStatus->status);
    SLOG_DEBUG("AlarmOutputStatus = {}", alarmOutputStatus->ToString());

    eventCallback(alarmOutputStatus);

}

void SchemaBasedEventStatusDispatcher::TriggerSystemEvent(const std::string &eventName, std::string &time, rapidjson::Value::ConstValueIterator event)
{
    DeviceAlarmEventSharedPtr alarm = std::make_shared<DeviceAlarmEvent>();
    alarm->serviceUtcTimeMsec = Common::currentUtcMsecs();
    alarm->deviceUtcTimeMsec = Common::utcMsecsFromRfc3339(time);
    alarm->deviceID = m_repos->device().deviceID;

    alarm->type = eventName;
    alarm->isDevice = true;

    if(!event->HasMember("Source")){
        SLOG_WARN("Source isn't in a member");
        return;
    }

    const rapidjson::Value& source = (*event)["Source"];
    if(!source.IsObject()){
        SLOG_WARN("Source isn't object");
        return;
    }

    if(!source.HasMember("Channel")){
        SLOG_WARN("Channel isn't in a member");
        return;
    }

    if(!source["Channel"].IsInt()){
        SLOG_WARN("Channel isn't int.");
        return;
    }

    alarm->sourceParameters["SystemID"] = fromSunapiChannel(ToStringValue(source["Channel"]));

    if(!event->HasMember("Data")){
        SLOG_WARN("Data isn't in a member");
        return;
    }
    const rapidjson::Value& data = (*event)["Data"];
    if(!data.IsObject()){
        SLOG_WARN("Data isn't object.");
        return;
    }
    if(!data.HasMember("State")){
        SLOG_WARN("State isn't int.");
        return;
    }
    if(!data["State"].IsBool()){
        SLOG_WARN("State isn't bool.");
        return;
    }
    alarm->data = data["State"].GetBool();

    SLOG_DEBUG("DeviceAlarm : {}", alarm->ToString());

    eventCallback(alarm);
}

void SchemaBasedEventStatusDispatcher::TriggerAlarmIn(const std::string& eventName, std::string& time, rapidjson::Value::ConstValueIterator event)
{
    DeviceAlarmEventSharedPtr alarm = std::make_shared<DeviceAlarmEvent>();
    alarm->serviceUtcTimeMsec = Common::currentUtcMsecs();
    alarm->deviceUtcTimeMsec = Common::utcMsecsFromRfc3339(time);
    alarm->deviceID = m_repos->device().deviceID;
    alarm->type = eventName;
    alarm->isDevice = true;

    if(!event->HasMember("Source")){
        SLOG_WARN("Source isn't in a member");
        return;
    }

    const rapidjson::Value& source = (*event)["Source"];
    if(!source.IsObject()){
        SLOG_WARN("Source isn't object");
        return;
    }

    if(!source.HasMember("Channel")){
        SLOG_WARN("Channel isn't in a member");
        return;
    }

    if(!source["Channel"].IsInt()){
        SLOG_WARN("Channel isn't int");
        return;
    }

    alarm->sourceParameters["SystemID"] = fromSunapiChannel(ToStringValue(source["Channel"]));

    if(!event->HasMember("Data")){
        SLOG_WARN("Data isn't in a member");
        return;
    }

    const rapidjson::Value& data = (*event)["Data"];
    if(!data.IsObject()){
        SLOG_WARN("Data isn't object.");
        return;
    }

    if(!data.HasMember("State")){
        SLOG_WARN("State isn't in a member");
        return;
    }

    if(!data["State"].IsBool()){
        SLOG_WARN("State isn't bool.");
        return;
    }

    alarm->data = data["State"].GetBool();

    SLOG_DEBUG("DeviceAlarm : {}", alarm->ToString());
    eventCallback(alarm);
}

void SchemaBasedEventStatusDispatcher::TriggerChannel(const std::string& eventName, std::string& time, rapidjson::Value::ConstValueIterator event)
{
    DeviceAlarmEventSharedPtr alarm = std::make_shared<DeviceAlarmEvent>();
    alarm->serviceUtcTimeMsec = Common::currentUtcMsecs();
    alarm->deviceUtcTimeMsec = Common::utcMsecsFromRfc3339(time);
    alarm->deviceID = m_repos->device().deviceID;

    alarm->type = eventName;
    alarm->isChannel = true;

    if(!event->HasMember("Source")){
        SLOG_WARN("Source isn't in a member");
        return;
    }

    const rapidjson::Value& source = (*event)["Source"];
    if(!source.IsObject()){
        SLOG_WARN("Source isn't object");
        return;
    }

    for(rapidjson::Value::ConstMemberIterator member = source.MemberBegin(); member != source.MemberEnd(); ++member){

        std::string name = ToStringValue((*member).name);

        if(0 == name.compare("Channel")){
            alarm->channelID = fromSunapiChannel(ToStringValue((*member).value));
        }else{
            alarm->sourceParameters.emplace(ToStringValue((*member).name),ToStringValue((*member).value));
        }
    }

    if(!event->HasMember("Data")){
        SLOG_WARN("Data isn't in a member");
        return;
    }

    const rapidjson::Value& data = (*event)["Data"];

    if(!data.IsObject()){
        SLOG_WARN("Data isn't object");
        return;
    }

    for(rapidjson::Value::ConstMemberIterator member = data.MemberBegin(); member != data.MemberEnd(); ++member){
        std::string name = ToStringValue((*member).name);

        if(0 == name.compare("State")){
            alarm->data = (*member).value.GetBool();
        }else{
            alarm->data = true;
            alarm->dataParameters.emplace(ToStringValue((*member).name),ToStringValue((*member).value));
        }
    }

    if(0 == alarm->type.compare("Videoloss")){

        if(alarm->data){
            alarm->type = "Videoloss.Start";
        }else{
            alarm->type = "Videoloss.End";
        }

    }else if(0 == alarm->type.compare("Tracking")){

        if(alarm->data){
            alarm->type = "Tracking.Start";
        }else{
            alarm->type = "Tracking.End";
        }
    }

    SLOG_DEBUG("DeviceAlarm : {}", alarm->ToString());

    eventCallback(alarm);
}

template <typename Encoding, typename Allocator>
std::string SchemaBasedEventStatusDispatcher::ToStringValue(const rapidjson::GenericValue<Encoding, Allocator>&  value)
{
    std::stringstream ss;

    if(value.IsNull()){
        return "";
    }else if(value.IsFalse()){
        ss << "False";
    }else if(value.IsTrue()){
        ss << "True";
    }else if(value.IsObject()){
        ss << "Unknown-Object";
    }else if(value.IsArray()){
        ss << "Unknown-Array";
    }else if(value.IsInt()){
        ss << value.GetInt();
    }else if(value.IsUint()){
        ss << value.GetUint();
    }else if(value.IsInt64()){
        ss << value.GetInt64();
    }else if(value.IsUint64()){
        ss << value.GetUint64();
    }else if(value.IsDouble()){
        ss << value.GetDouble();
    }else if(value.IsString()){
        ss << value.GetString();
    }else{
        ss << "Unknown-Type";
    }

    return ss.str();
}

}
}
