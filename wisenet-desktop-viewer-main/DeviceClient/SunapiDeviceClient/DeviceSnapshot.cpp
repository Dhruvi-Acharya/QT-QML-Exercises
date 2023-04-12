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

class VideoSnapshotView : public BaseCommand, public JpegParser
{
public:
    explicit VideoSnapshotView(const std::string& logPrefix, DeviceSnapshotRequestSharedPtr request)
        :BaseCommand(this,logPrefix,std::make_shared<DeviceSnapshotResponse>(),false,false),
         m_request(request)
    {

    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;
        builder.CreateCgi("video", "snapshot", "view")
                .AddParameter("Channel", toSunapiChannel(m_request->channelID));

        return builder.GetUrl();
    };

    ResponseBaseSharedPtr ProcessPost() override {
        m_responseBase->errorCode = Wisenet::ErrorCode::NoError;
        return m_responseBase;
    }

private:
    bool setContent(const std::string& content) override {

        if(0 == content.size()) {
            SPDLOG_ERROR("JPEG size is zero.");
            return false;
        }

        auto response = std::static_pointer_cast<DeviceSnapshotResponse>(m_responseBase);

        response->snapshotData.reserve(content.size());
        std::copy(content.begin(), content.end(),
                  std::back_inserter(response->snapshotData));

        return true;
    }
    DeviceSnapshotRequestSharedPtr m_request;
};

void SunapiDeviceClientImpl::DeviceSnapshot(const DeviceSnapshotRequestSharedPtr &request,
                                        const ResponseBaseHandler &responseHandler)
{
    SLOG_DEBUG("DeviceSnapshot():: channel={}", request->channelID);

    asyncRequest(m_httpCommandSession,
                 std::make_shared<VideoSnapshotView>(m_logPrefix,request),
                 responseHandler,
                 nullptr,AsyncRequestType::HTTPGET,false);

}

}
}
