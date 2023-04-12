/*********************************************************************************
 * Copyright(c) 2021 by Hanwha Techwin Co., Ltd.
 *
 * This software is copyrighted by, and is the sole property of Hanwha Techwin.
 * All rigths, title, ownership, or other interests in the software remain the
 * property of Hanwha Techwin. This software may only be used in accordance with
 * the corresponding license agreement. Any unauthorized use, duplication,
 * transmission, distribution, or disclosure of this software is expressly
 * forbidden.
 *
 * This Copyright notice may not be removed or modified without prior written
 * consent of Hanwha Techwin.
 *
 * Hanwha Techwin reserves the right to modify this software without notice.
 *
 * Hanwha Techwin Co., Ltd.
 * KOREA
 * https://www.hanwha-security.com/
 *********************************************************************************/

#include "tst_test__coreservice.h"

// initTestCase()에서 전역으로 테스트 수행
void Test__CoreService::test_GetServiceStatus_RequiredAddAdmin()
{ 
    std::string serviceID;
    // 1. 초기 상태인지 확인 테스트
    {
        auto request = std::make_shared<Core::GetServiceInformationRequest>();

        InitializeNotify();
        m_service->GetServiceInformation(request, [this, &serviceID]
                                   (const ResponseBaseSharedPtr& response)
        {
            if(response->isFailed()) {
                std::ostringstream msg;
                msg << "GetServiceInformation is failed. error=" << response->errorString();
                QWARN(msg.str().c_str());
                Notify(false);
            } else {
                auto info = std::static_pointer_cast<Core::GetServiceInformationResponse>(response);
                SPDLOG_INFO("GetServiceInformation : serviceID={},version={},serviceType={},platform={},osversion={}",
                            info->serviceID, info->version, info->serviceType, info->osInfo.platform, info->osInfo.osVersion);
                serviceID = info->serviceID;
                Notify(true);
            }
        });

        QVERIFY2(Wait(), "GetServiceInformation() failed");

        QVERIFY2((0 == serviceID.compare("None")),"Not initialized.");
    }

    // 2. 소유자 패스워드 설정 테스트
    {
        auto request = std::make_shared<Core::SetupServiceRequest>();
        request->serviceID = serviceID;
        request->userName = m_adminName;
        request->newPassword = m_adminPassword;

        InitializeNotify();
        m_service->SetupService(request, [this]
                                   (const ResponseBaseSharedPtr& response)
        {
            if(response->isFailed()) {
                SPDLOG_INFO("SetupService is Failed. error={}({})", response->errorString(),response->errorCode);
                Notify(false);
            } else {
                SPDLOG_INFO("SetupService is succeeded.");
                Notify(true);
            }
        });

        QVERIFY2(Wait(), "SetupService() failed");

    }

    // 3. 패스워드 설정 후 serviceID 정상 유무 확인 테스트
    {
        auto request = std::make_shared<Core::GetServiceInformationRequest>();

        InitializeNotify();
        m_service->GetServiceInformation(request, [this, &serviceID]
                                   (const ResponseBaseSharedPtr& response)
        {
            if(response->isFailed()) {
                SPDLOG_INFO("GetServiceInformation is failed. error={}({})", response->errorString(),response->errorCode);
                Notify(false);
            } else {
                auto info = std::static_pointer_cast<Core::GetServiceInformationResponse>(response);
                SPDLOG_INFO("GetServiceInformation : serviceID={},version={},serviceType={},platform={},osversion={}",
                            info->serviceID, info->version, info->serviceType, info->osInfo.platform, info->osInfo.osVersion);
                serviceID = info->serviceID;
                Notify(true);
            }
        });

        QVERIFY2(Wait(), "GetServiceInformation() failed");

        QVERIFY2((0 != serviceID.compare("None")),"Out of Service.");
    }

    // 4. 패스워드 설정된 이후 다시 설정이 가능한지 테스트
    {
        auto request = std::make_shared<Core::SetupServiceRequest>();
        request->serviceID = serviceID;
        request->userName = m_adminName;
        request->newPassword = m_adminPassword;

        InitializeNotify();
        m_service->SetupService(request, [this]
                                   (const ResponseBaseSharedPtr& response)
        {
            // failed is OK.
            Notify(response->isFailed());
        });

        QVERIFY2(Wait(), "SetupService() failed");
    }

    SPDLOG_DEBUG("Test__CoreService::test_GetServiceStatus_RequiredAddAdmin() END");
}
