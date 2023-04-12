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

class UrlSnapshotView : public BaseCommand, public JpegParser
{
public:
    explicit UrlSnapshotView(const std::string& logPrefix, std::string url,
                             DeviceUrlSnapShotResponseSharedPtr finalResponse)
        :BaseCommand(this,logPrefix,std::make_shared<DeviceUrlSnapShotResponse>(),false,false),
         m_request(url), m_finalResponse(finalResponse)
    {
    }

    std::string RequestUri() override {

        return m_request;
    };

    ResponseBaseSharedPtr ProcessPost() override {
        m_responseBase->errorCode = Wisenet::ErrorCode::NoError;
        return m_responseBase;
    }

private:
    bool setContent(const std::string& content) override {

        UrlSnapShot snapShot;
        snapShot.isSuccess = true;
        snapShot.url = m_request;

        if(0 == content.size()) {
            SPDLOG_ERROR("JPEG size is zero.");
            m_finalResponse->snapShots.push_back(snapShot);
            return false;
        }

        snapShot.snapshotData.reserve(content.size());
        std::copy(content.begin(), content.end(),
                  std::back_inserter(snapShot.snapshotData));
        m_finalResponse->snapShots.push_back(snapShot);

        return true;
    }

    std::string m_request;
public :
    DeviceUrlSnapShotResponseSharedPtr m_finalResponse;

};


void SunapiDeviceClientImpl::DeviceUrlSnapShot(const DeviceUrlSnapShotRequestSharedPtr &request,
                                        const ResponseBaseHandler &responseHandler)
{
    SLOG_DEBUG("DeviceUrlSnapShot():: urlList.size()={}", request->urlList.size());

    auto finalResponse = std::make_shared<DeviceUrlSnapShotResponse>();

    urlSnapShotResult(request, finalResponse, responseHandler);

}

void SunapiDeviceClientImpl::urlSnapShotResult(DeviceUrlSnapShotRequestSharedPtr const& request,
                       DeviceUrlSnapShotResponseSharedPtr finalResponse,
                       ResponseBaseHandler const& responseHandler)
{
    auto urlSnapShot = std::make_shared<UrlSnapshotView>(
                m_logPrefix, request->urlList.at(finalResponse->snapShots.size()), finalResponse);

    asyncRequest(m_httpSearchMetaDataSession,
                 urlSnapShot,
                 [this, request, finalResponse, responseHandler](const ResponseBaseSharedPtr& response)

    {
        if(response->isSuccess()){
            SLOG_DEBUG("urlSnapShotResult() response request->urlList.size()={} finalResponse->snapShots.size()={} maxCount={}",
                       request->urlList.size(), finalResponse->snapShots.size());

            if(request->urlList.size() ==finalResponse->snapShots.size()){
                SLOG_DEBUG("urlSnapShotResult() final");
                finalResponse->errorCode = Wisenet::ErrorCode::NoError;
                responseHandler(finalResponse);
            }
            else
               urlSnapShotResult(request, finalResponse, responseHandler);

        }
    }, nullptr, AsyncRequestType::HTTPGET,false);

}

}
}
