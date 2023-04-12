/*********************************************************************************
 * Copyright(c) 2023 Hanwha Vision Co., Ltd.  All rights reserved.
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


void SunapiDeviceClientImpl::DeviceTextPosConf(DeviceTextPosConfRequestSharedPtr const& request,
                                               ResponseBaseHandler const& responseHandler )
{
    boost::asio::post(m_strand, WeakCallback(shared_from_this(), [this, request, responseHandler]() {
        SLOG_DEBUG("DeviceTextPosConf(), deviceID={}", request->deviceID);

        auto textPosConfView = std::make_shared<RecordingTextPosConfView>(m_logPrefix,
                                                                          std::make_shared<DeviceTextPosConfResponse>());

        asyncRequest(m_httpSearchMetaDataSession,
                     textPosConfView,
                     [this, request, textPosConfView, responseHandler](const ResponseBaseSharedPtr& result)
        {
            auto response = std::make_shared<DeviceTextPosConfResponse>();
            if (result->isSuccess())
            {
                SLOG_DEBUG("DeviceTextPosConf - success");
                response->confData = textPosConfView->m_confData;
                responseHandler(response);
            }
            else
            {
                responseHandler(response);
            }

        },  nullptr, AsyncRequestType::HTTPGET, false);
    }));
}

void SunapiDeviceClientImpl::DeviceTextSearch(DeviceTextSearchRequestSharedPtr const& request,
                                              ResponseBaseHandler const& responseHandler)
{
    m_runningAiSearch = true;
    boost::asio::post(m_strand, WeakCallback(shared_from_this(), [this, request, responseHandler]() {
        SLOG_DEBUG("DeviceTextSearch(), deviceID={}, fromDate={}, toDate={}", request->deviceID, request->fromDate, request->toDate);

        if (request->fromDate == 0 || request->toDate == 0) {
            SLOG_DEBUG("DeviceTextSearch(), deviceID={}, fromDate={}, toDate={}", request->deviceID, request->fromDate, request->toDate);
            auto response = std::make_shared<ResponseBase>();
            response->errorCode = Wisenet::ErrorCode::InvalidRequest;
            m_runningAiSearch = false;
            responseHandler(response);
            return;
        }

        auto textSearchTokenControl = std::make_shared<RecordingTextSearchControl>(m_logPrefix, request);

        asyncRequest(m_httpSearchMetaDataSession,
                     textSearchTokenControl,
                     [this, request, textSearchTokenControl, responseHandler](const ResponseBaseSharedPtr& result)
        {
            auto response = std::make_shared<DeviceTextSearchResponse>();
            if (result->isSuccess())
            {
                auto deviceID = request->deviceID;
                SLOG_DEBUG("DeviceTextSearch - GetToken(): isSuccess=success, deviceID={}, searchToken={} ", deviceID, textSearchTokenControl->m_searchToken);

                // 2022.08.29. 획득한 검색 토큰으로 검색 상태를 요청
                auto finalResponse = std::make_shared<DeviceTextSearchResponse>();
                textSearchViewStatus(deviceID, textSearchTokenControl->m_searchToken, finalResponse, responseHandler);
            }
            else
            {
                m_runningAiSearch = false;
                responseHandler(response);
            }
        },  nullptr, AsyncRequestType::HTTPGET, false);
    }));
}

void SunapiDeviceClientImpl::textSearchViewStatus(std::string deviceID,
                                                  std::string searchToken,
                                                  DeviceTextSearchResponseSharedPtr finalResponse,
                                                  ResponseBaseHandler const& responseHandler)
{
    if (!m_runningAiSearch)
        return;

    auto textSearchView = std::make_shared<RecordingTextSearchView>(m_logPrefix, finalResponse, deviceID, searchToken, "");

    asyncRequest(m_httpSearchMetaDataSession,
                 textSearchView,
                 [this, textSearchView, deviceID, searchToken, finalResponse, responseHandler](const ResponseBaseSharedPtr& result)

    {
        auto response = std::make_shared<DeviceTextSearchStatusResponse>();
        if (result->isSuccess())
        {
            SLOG_DEBUG("DeviceTextSearch - Status(): deviceID={}, searchToken={}, totalCount={}, status={} ", textSearchView->m_deviceID, textSearchView->m_searchToken, textSearchView->m_totalCount, textSearchView->m_status);

            if (textSearchView->m_status != "Completed")
            {
                StartTextSearchStatusTimer(textSearchView->m_deviceID, searchToken, finalResponse, responseHandler);
            }
            else
            {
                // 2022.08.29. 검색 결과 요청
                textSearchViewResult(textSearchView->m_deviceID, searchToken, response->totalCount, finalResponse, responseHandler);
            }
        }
        else
        {
            m_runningAiSearch = false;
            responseHandler(response);
        }
    }, nullptr, AsyncRequestType::HTTPGET, false);
}

void SunapiDeviceClientImpl::textSearchViewResult(std::string deviceID,
                                                  std::string searchToken, std::string requestCount,
                                                  DeviceTextSearchResponseSharedPtr finalResponse,
                                                  ResponseBaseHandler const& responseHandler)
{
    if (!m_runningAiSearch)
        return;

    auto textSearch = std::make_shared<RecordingTextSearchResultView>(m_logPrefix, finalResponse, deviceID, searchToken, requestCount);

    asyncRequest(m_httpSearchMetaDataSession,
                 textSearch,
                 [this, textSearch, searchToken, requestCount, finalResponse, responseHandler](const ResponseBaseSharedPtr& result)

    {
        auto response = std::make_shared<DeviceTextSearchResultResponse>();
        if (result->isSuccess())
        {
            m_runningAiSearch = false;

            if (textSearch->m_totalCount > 0 && textSearch->m_totalResultsFound > 0)
            {
                response->searchToken = textSearch->m_searchToken;
                response->intervalFrom = textSearch->m_intervalFrom;
                response->intervalTo = textSearch->m_intervalTo;
                response->searchTokenExpirytime = textSearch->m_searchTokenExpirytime;
                response->totalCount = textSearch->m_totalCount;
                response->totalResultsFound = textSearch->m_totalResultsFound;
                response->textData = textSearch->m_textData;
            }

            finalResponse->errorCode = Wisenet::ErrorCode::NoError;
            responseHandler(response);

            // 검색은 최대 4개의 session 지원하고,
            // 세션을 renew 하지 않으면 최대 60초간 유지가 된다.
            // 새로 검색하기전에 token을 cancel해서 사용하도록 한다.
            // cacel 을 하지 않으면 3 ~ 4회 검색 요청 이후 ”Cannot Access Resource, 610” 에러 발생함.
            textSearchCancelInternal(response->searchToken);
        }
        else
        {
            m_runningAiSearch = false;
            responseHandler(response);
        }
    }, nullptr, AsyncRequestType::HTTPGET, false);
}

void SunapiDeviceClientImpl::textSearchCancelInternal(std::string searchToken)
{
    SLOG_DEBUG("textSearchCancelInternal() seachToken={} ", searchToken);

    auto textSearch = std::make_shared<RecordingTextSearchControlCancel>(
                m_logPrefix, searchToken);
    asyncRequest(m_httpSearchMetaDataSession,
                 textSearch,
                 [this, textSearch, searchToken](const ResponseBaseSharedPtr& response)

    {
        SLOG_DEBUG("textSearchCancelInternal() response seachToken={} success={}",
                   searchToken, response->isSuccess());
    }, nullptr, AsyncRequestType::HTTPGET,false, nullptr, 10);
}

void SunapiDeviceClientImpl::StartTextSearchStatusTimer(std::string deviceID,
                                                        std::string searchToken,
                                                        DeviceTextSearchResponseSharedPtr finalResponse,
                                                        ResponseBaseHandler const& responseHandler, long long durationMsec)
{
    m_aiSearchStatusTimer.cancel();
    m_aiSearchStatusTimer.expires_after(std::chrono::milliseconds(durationMsec));
    m_aiSearchStatusTimer.async_wait( WeakCallback(shared_from_this(),
                                                   [this, deviceID, searchToken, finalResponse, responseHandler](const boost::system::error_code& ec) {
        if (ec) {
            return;
        }

        boost::asio::post(m_strand, WeakCallback(shared_from_this(), [this, deviceID, searchToken, finalResponse, responseHandler]() {
            SLOG_DEBUG("StartTextSearchStatusTimer");
            textSearchViewStatus(deviceID, searchToken, finalResponse, responseHandler);
        }));
    }));
}

}
}
