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

void SunapiDeviceClientImpl::StartSmartSearchStatusTimer(std::string searchToken,
                                                         DeviceSmartSearchResponseSharedPtr finalResponse,
                                                         ResponseBaseHandler const& responseHandler, long long durationMsec)
{
    m_aiSearchStatusTimer.cancel();
    m_aiSearchStatusTimer.expires_after(std::chrono::milliseconds(durationMsec));
    m_aiSearchStatusTimer.async_wait( WeakCallback(shared_from_this(),
                                                   [this, searchToken, finalResponse, responseHandler](const boost::system::error_code& ec) {
        if (ec) {
            m_runningAiSearch = false;
            return;
        }

        boost::asio::post(m_strand, WeakCallback(shared_from_this(), [this, searchToken, finalResponse, responseHandler]() {
            SLOG_DEBUG("StartSmartSearchStatusTimer");
            smartSearchViewStatus(searchToken, finalResponse, responseHandler);
        }));
    }));
}

void SunapiDeviceClientImpl::DeviceSmartSearchCancel( DeviceRequestBaseSharedPtr const& request,
                                                      ResponseBaseHandler const& responseHandler )
{
    asio::post(m_strand, WeakCallback(shared_from_this(), [this, request, responseHandler]()
    {
        auto response = std::make_shared<ResponseBase>();
        response->errorCode = Wisenet::ErrorCode::NoError;
        m_runningAiSearch = false;
        responseHandler(response);
    }));
}

void SunapiDeviceClientImpl::DeviceSmartSearch( DeviceSmartSearchRequestSharedPtr const& request,
                                                ResponseBaseHandler const& responseHandler )
{
    m_runningAiSearch = true;
    asio::post(m_strand, WeakCallback(shared_from_this(), [this, request, responseHandler]()
    {
        SLOG_DEBUG("DeviceSmartSearch(), fromDate={} toDate={}  areaSize={}",
                   request->fromDate, request->toDate,  (int)request->areas.size());

        if(request->fromDate == 0 || request->toDate == 0){
            SLOG_DEBUG("DeviceSmartSearch() from={} to={}",
                       request->fromDate, request->toDate);
            auto response = std::make_shared<ResponseBase>();
            response->errorCode = Wisenet::ErrorCode::InvalidRequest;
            m_runningAiSearch = false;
            responseHandler(response);
            return;
        }

        auto searchControl = std::make_shared<RecordingSmartSearchControl>(
                    m_logPrefix,
                    request);


        asyncRequest(m_httpSearchMetaDataSession,
                     searchControl,
                     [this, request, searchControl, responseHandler](const ResponseBaseSharedPtr& response)

        {
            auto finalResponse = std::make_shared<DeviceSmartSearchResponse>();
            if(response->isSuccess()){
                SLOG_DEBUG("DeviceSmartSearch() isSuccess token={} ",
                           searchControl->m_searchToken);

                smartSearchViewStatus(searchControl->m_searchToken, finalResponse, responseHandler);


            }
            else{
                m_runningAiSearch = false;
                responseHandler(finalResponse);
            }

        },  nullptr, AsyncRequestType::HTTPGET,false);

    }));
}

void SunapiDeviceClientImpl::smartSearchViewStatus(std::string searchToken,
                                                   DeviceSmartSearchResponseSharedPtr finalResponse,
                                                   ResponseBaseHandler const& responseHandler)
{
    SLOG_DEBUG("smartSearchViewStatus() seachToken={}",
               searchToken);
    if(!m_runningAiSearch) return;

    auto smartSearch = std::make_shared<RecordingSmartSearchView>(
                m_logPrefix, finalResponse, searchToken, false);

    asyncRequest(m_httpSearchMetaDataSession,
                 smartSearch,
                 [this, smartSearch, searchToken, finalResponse, responseHandler](const ResponseBaseSharedPtr& response)

    {
        if(response->isSuccess()){
            SLOG_DEBUG("smartSearchViewStatus() isSuccess isResult={} m_isComplete={}",
                       smartSearch->m_isResult, smartSearch->m_isComplete);
            if(smartSearch->m_isResult == false && smartSearch->m_isComplete == false){
                StartSmartSearchStatusTimer(searchToken, finalResponse, responseHandler);
            }
            else{
                smartSearchViewResult(searchToken, finalResponse, responseHandler);
            }
        }
        else{
            m_runningAiSearch = false;
            responseHandler(finalResponse);
        }
    }, nullptr, AsyncRequestType::HTTPGET,false);
}

void SunapiDeviceClientImpl::smartSearchViewResult(std::string searchToken,
                                                   DeviceSmartSearchResponseSharedPtr finalResponse,
                                                   ResponseBaseHandler const& responseHandler)
{
    SLOG_DEBUG("smartSearchViewResult() seachToken={} ",
               searchToken);
    if(!m_runningAiSearch) return;

    auto smartSearch = std::make_shared<RecordingSmartSearchView>(
                m_logPrefix, finalResponse, searchToken, true);
    asyncRequest(m_httpSearchMetaDataSession,
                 smartSearch,
                 [this, smartSearch, searchToken, finalResponse, responseHandler](const ResponseBaseSharedPtr& response)

    {
        if(response->isSuccess()){
            SLOG_DEBUG("smartSearchViewResult() response seachToken={} ",
                       searchToken);

            finalResponse->errorCode = Wisenet::ErrorCode::NoError;

            SLOG_DEBUG("smartSearchViewResult() final");
            m_runningAiSearch = false;
            responseHandler(finalResponse);
            smartSearchCancelInternal(finalResponse->searchToken);
        }
        else{
            m_runningAiSearch = false;
            responseHandler(finalResponse);
        }
    }, nullptr, AsyncRequestType::HTTPBIGGET,false, nullptr, 60);
}

void SunapiDeviceClientImpl::smartSearchCancelInternal(std::string searchToken)
{
    SLOG_DEBUG("smartSearchCancelInternal() seachToken={} ",searchToken);

    auto smartSearch = std::make_shared<RecordingSmartSearchControlCancel>(
                m_logPrefix, searchToken);
    asyncRequest(m_httpSearchMetaDataSession,
                 smartSearch,
                 [this, smartSearch, searchToken](const ResponseBaseSharedPtr& response)

    {
        SLOG_DEBUG("smartSearchCancelInternal() response seachToken={} success={}",
                   searchToken, response->isSuccess());
    }, nullptr, AsyncRequestType::HTTPGET,false, nullptr, 10);
}

}
}
