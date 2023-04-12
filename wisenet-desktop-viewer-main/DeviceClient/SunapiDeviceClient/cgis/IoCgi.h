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
#include <string>
#include <map>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include "SunapiDeviceClientUtil.h"
#include "cgis/BaseCommand.h"
#include "cgis/SunapiSyntaxBuilder.h"

namespace Wisenet
{
namespace Device
{

//************************* IoAlarmOutputView ******************************//

struct IoAlarmOutputViewResult
{
    struct AlarmOutputStatus
    {
        std::string type;
        std::string idleState;
    };

    std::map<int, AlarmOutputStatus> alarmOutputs;
};


class IoAlarmOutputView : public BaseCommand, public IniParser
{
public:
    explicit IoAlarmOutputView(const std::string& logPrefix,
                               ResponseBaseSharedPtr response,
                               bool ignoreHttpErrorResponse = false,
                               bool ignoreParsingErrorResponse = false)
        :BaseCommand(this,logPrefix,response,
                     ignoreHttpErrorResponse,
                     ignoreParsingErrorResponse)
    {

    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;
        builder.CreateCgi("io","alarmoutput", "view");
        return builder.GetUrl();
    }

    ResponseBaseSharedPtr ProcessPost() override {
        return nullptr;
    }
    IoAlarmOutputViewResult parseResult;

private:
    bool parseINI(NameValueText& iniDoc) override
    {
        // Parse full response
        for (auto &item : iniDoc.m_listValue) {
            std::vector<std::string> tokens;
            int index = 0;
            boost::split(tokens, item.first, boost::is_any_of("."), boost::token_compress_on);
            if (tokens.size() != 3)
                continue;
            if (tokens[0] != "AlarmOutput")
                continue;
            if (!try_stoi(tokens[1], &index))
                continue;

            std::string& key = tokens[2];
            if (key == "Type") {
                parseResult.alarmOutputs[index].type = item.second;
            }
            else if (key == "IdleState") {
                parseResult.alarmOutputs[index].idleState = item.second;
            }
        }
        return true;
    }
};

}
}
