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
#include <memory>
#include <string>
#include <boost/core/ignore_unused.hpp>
#include "LogSettings.h"

#include "cgis/BaseCommand.h"
#include "cgis/SunapiSyntaxBuilder.h"

namespace Wisenet
{
namespace Device
{
struct EventStatusSchemaViewResult
{
    std::vector<std::string> eventNames;
};

class EventStatusSchemaView : public BaseCommand, public IniParser
{
public:
    explicit EventStatusSchemaView(const std::string& logPrefix, ResponseBaseSharedPtr response,
                                   bool ignoreHttpErrorResponse = false,
                                   bool ignoreParsingErrorResponse = false)
        :BaseCommand(this, logPrefix, response, ignoreHttpErrorResponse, ignoreParsingErrorResponse)
    {
    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;
        builder.CreateCgi("eventstatus","eventstatusschema", "view");
        return builder.GetUrl();
    };

    ResponseBaseSharedPtr ProcessPost() override {
        //응답처리를 하지 않으므로 nullptr로 리턴
        return nullptr;
    }

    EventStatusSchemaViewResult parseResult;

private:
    bool parseINI(NameValueText& iniDoc) override
    {
        // Parse full response
        for (auto &item : iniDoc.m_listValue) {

            std::vector<std::string> tokens;
            boost::split(tokens, item.first, boost::is_any_of("."), boost::token_compress_on);

            if (tokens.size() != 3){
                SLOG_DEBUG("Not the Event Name line = {} = ", item.first, item.second);
                continue;
            }

            if (tokens[2] != "Name"){
                SLOG_DEBUG("Not the Name token = {} = ", item.first, item.second);
                continue;
            }

            //SLOG_DEBUG("EVENT NAME = {}", item.second);

            if(IsDynamicEvent(item.second)){
                SLOG_INFO("DYNAMIC EVENT NAME = {}", item.second);
                parseResult.eventNames.push_back(item.second);
            }
        }

        return true;
    }

    bool IsDynamicEvent(const std::string& eventName)
    {
        if(("MotionDetection" != eventName)
                && ("VideoAnalytics" != eventName)
                && ("Tampering" != eventName)
                && ("Tracking" != eventName)
                && ("AlarmOutput" != eventName)
                && ("DefocusDetection" != eventName)
                && ("FogDetection" != eventName)
                && ("AudioDetection" != eventName)
                && ("AudioAnalytics" != eventName)
                && ("AlarmInput" != eventName)
                && ("NetworkAlarmInput" != eventName)
                && ("Videoloss" != eventName)
                && ("FaceDetection" != eventName)
                && ("SystemEvent" != eventName)){

            return true;
        }

        return false;
    }

};

}
}
