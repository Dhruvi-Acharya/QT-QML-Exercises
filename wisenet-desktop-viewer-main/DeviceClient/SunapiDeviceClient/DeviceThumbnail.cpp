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

class VideoThumbnailView : public BaseCommand, public JpegParser
{
public:
    explicit VideoThumbnailView(const std::string& logPrefix, DeviceThumbnailRequestSharedPtr request)
        :BaseCommand(this,logPrefix,std::make_shared<DeviceThumbnailResponse>(),false,false),
         m_request(request)
    {

    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;

        builder.CreateCgi("video", "thumbnail", "view");
        builder.AddParameter("Channel", toSunapiChannel(m_request->channelID));
        builder.AddParameter("FromDate", Common::utcMsecsToUtcIsoString(m_request->fromDate));
        builder.AddParameter("ToDate", Common::utcMsecsToUtcIsoString(m_request->toDate));
        builder.AddParameter("Width", m_request->width);
        builder.AddParameter("Height", m_request->height);
        builder.AddParameter("Column", m_request->column);
        builder.AddParameter("Row", m_request->row);
        builder.AddParameter("OverlappedID", m_request->overlappedID);

        return builder.GetUrl();
    }

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

        auto response = std::static_pointer_cast<DeviceThumbnailResponse>(m_responseBase);

        response->thumbnailData.reserve(content.size());
        std::copy(content.begin(), content.end(),
                  std::back_inserter(response->thumbnailData));

        return true;
    }

    DeviceThumbnailRequestSharedPtr m_request;
};

void SunapiDeviceClientImpl::DeviceThumbnail(const DeviceThumbnailRequestSharedPtr &request,
                                        const ResponseBaseHandler &responseHandler)
{
    SLOG_DEBUG("DeviceThumbnail():: channel={}", request->channelID);

    asyncRequest(m_httpCommandSession,
                 std::make_shared<VideoThumbnailView>(m_logPrefix,request),
                 responseHandler,
                 nullptr,AsyncRequestType::HTTPGET,false);
}

}
}
