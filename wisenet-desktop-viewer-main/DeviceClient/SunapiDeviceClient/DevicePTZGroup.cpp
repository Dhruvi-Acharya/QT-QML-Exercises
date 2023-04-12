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
#include "SunapiDeviceClientImpl.h"

namespace Wisenet
{
namespace Device
{

class PTZConfigGroupView : public BaseCommand, public JsonParser
{
public:
    explicit PTZConfigGroupView(const std::string& logPrefix, ChannelRequestBaseSharedPtr request)
        :BaseCommand(this,logPrefix,std::make_shared<DeviceGetGroupResponse>(),false,false),
         m_request(request)
    {
        SPDLOG_DEBUG("DevicePTZGroup::PTZConfigGroupView()");
    }
    std::string RequestUri() override {
        SPDLOG_DEBUG("DevicePTZGroup::PTZConfigGroupView::RequestUri()");
        SunapiSyntaxBuilder builder;

        builder.CreateCgi("ptzconfig", "group", "view")
               .AddParameter("Channel", toSunapiChannel(m_request->channelID));

        return builder.GetUrl();
    }

    ResponseBaseSharedPtr ProcessPost() override {
        SPDLOG_DEBUG("DevicePTZGroup::PTZConfigGroupView::ProcessPost()");
        auto response = std::static_pointer_cast<DeviceGetGroupResponse>(m_responseBase);
        response->groupPresets = parseResult;

        response->errorCode = Wisenet::ErrorCode::NoError;
        return response;
    }

   std::map<int, std::vector<int>> parseResult; // preset list of group 1 ~ 6
private:
    bool parseJson(rapidjson::Document& jsonDoc) override
    {
        SPDLOG_DEBUG("DevicePTZGroup::PTZConfigGroupView::parseJson()");
        rapidjson::Value::MemberIterator itr = jsonDoc.FindMember("PTZGroups");
        if (itr != jsonDoc.MemberEnd())
        {
            auto & results = itr->value;
            for(auto & result : results.GetArray())
            {
                auto groupsResult = result.FindMember("Groups");

                if(groupsResult != result.MemberEnd())
                {
                    auto & resultGroupList = groupsResult->value;
                    for(auto &resultGroup : resultGroupList.GetArray())
                    {
                        int groupIdx = 0;
                        std::vector<int> presetList;
                        auto groupNumberResult = resultGroup.FindMember("Group");
                        if(groupNumberResult != resultGroup.MemberEnd())
                        {
                            groupIdx = groupNumberResult->value.GetInt();
                        }

                        auto presetSeqResult = resultGroup.FindMember("PresetSequences");
                        if(presetSeqResult != resultGroup.MemberEnd())
                        {
                            auto & resultGroupPresetSeq = presetSeqResult->value;

                            for(auto &resultGroupPreset : resultGroupPresetSeq.GetArray())
                            {
                                auto presetNumberResult = resultGroupPreset.FindMember("Preset");
                                if(presetNumberResult != resultGroupPreset.MemberEnd())
                                {
                                    presetList.push_back(presetNumberResult->value.GetInt());
                                    SPDLOG_DEBUG("PTZConfigGroupView::parseJson() groupIdx={} preset={}",groupIdx,presetNumberResult->value.GetInt());
                                }
                            }
                        }
                        parseResult[groupIdx] = presetList; // 그룹 번호 마다의 preset list를 얻어서 삽입
                    }
                }
            }
        }

        return true;
    }

    ChannelRequestBaseSharedPtr m_request;
};

void SunapiDeviceClientImpl::DeviceGetGroup(ChannelRequestBaseSharedPtr const& request,
                                         ResponseBaseHandler const& responseHandler)
{
    SPDLOG_DEBUG("DevicePTZGroup::DeviceGetGroup() start");
    asyncRequest(m_httpPtzSession,
                 std::make_shared<PTZConfigGroupView>(m_logPrefix,request),
                 responseHandler,
                 nullptr,AsyncRequestType::HTTPGET,false);
    SPDLOG_DEBUG("DevicePTZGroup::DeviceGetGroup() end");
}

// move to group
class PTZControlMoveGroup : public BaseCommand
{
public:
    explicit PTZControlMoveGroup(const std::string& logPrefix, DeviceMoveGroupRequestSharedPtr request)
        :BaseCommand(nullptr,logPrefix,std::make_shared<Wisenet::ResponseBase>(),false,false),
         m_request(request)
    {
    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;

        builder.CreateCgi("ptzcontrol", "group", "control")
                .AddParameter("Channel", toSunapiChannel(m_request->channelID))
                .AddParameter("Group", m_request->groupNumber)
                .AddParameter("Mode", "Start"); // 1~:group#

        return builder.GetUrl();
    };

    ResponseBaseSharedPtr ProcessPost() override {
        m_responseBase->errorCode = Wisenet::ErrorCode::NoError;
        return m_responseBase;
    }

private:
    DeviceMoveGroupRequestSharedPtr m_request;
};

// move to group
void SunapiDeviceClientImpl::DeviceMoveGroup(DeviceMoveGroupRequestSharedPtr const& request, ResponseBaseHandler const& responseHandler)
{
    asyncRequest(m_httpPtzSession,
                 std::make_shared<PTZControlMoveGroup>(m_logPrefix,request),
                 responseHandler,
                 nullptr,AsyncRequestType::HTTPGET,false);
}

// stop this position
class PTZControlStopGroup : public BaseCommand
{
public:
    explicit PTZControlStopGroup(const std::string& logPrefix, DeviceStopGroupRequestSharedPtr request)
        :BaseCommand(nullptr,logPrefix,std::make_shared<Wisenet::ResponseBase>(),false,false),
          m_request(request)
    {
    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;

        builder.CreateCgi("ptzcontrol", "group", "control")
                .AddParameter("Channel", toSunapiChannel(m_request->channelID))
                .AddParameter("Group", m_request->groupNumber)
                .AddParameter("Mode", "Stop"); // 1~:group#

        return builder.GetUrl();
    };

    ResponseBaseSharedPtr ProcessPost() override {
        m_responseBase->errorCode = Wisenet::ErrorCode::NoError;
        return m_responseBase;
    }

private:
    DeviceStopGroupRequestSharedPtr m_request;
};

// stop this position
void SunapiDeviceClientImpl::DeviceStopGroup(DeviceStopGroupRequestSharedPtr const& request, ResponseBaseHandler const& responseHandler)
{
    asyncRequest(m_httpPtzSession,
                 std::make_shared<PTZControlStopGroup>(m_logPrefix,request),
                 responseHandler,
                 nullptr,AsyncRequestType::HTTPGET,false);
}

}
}
