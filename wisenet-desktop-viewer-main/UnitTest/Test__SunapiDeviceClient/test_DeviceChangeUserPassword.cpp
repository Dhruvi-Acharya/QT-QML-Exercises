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

#include "tst_test__sunapideviceclient.h"

#include <thread>
#include <mutex>

#include <boost/core/ignore_unused.hpp>
#include <condition_variable>
#include <chrono>

void Test__SunapiDeviceClient::test_ChangeUserPassword()
{

    SPDLOG_DEBUG("[1] DeviceConnect");

    std::string originalPassword = m_password;
    QVERIFY2(connectInternal(), "[1] --> Can not connect to the device..");

    // Change User Password (원래 패스워드를 반대로 변경하여 설정을 변경한다.)
    SPDLOG_DEBUG("[2] Change reversed user password");
    auto changeUserPasswordRequest = std::make_shared<DeviceChangeUserPasswordRequest>();
    changeUserPasswordRequest->password = originalPassword;
    std::reverse(changeUserPasswordRequest->password.begin(), changeUserPasswordRequest->password.end());

    InitializeNotify();
    m_deviceClient->DeviceChangeUserPassword(changeUserPasswordRequest,[this, changeUserPasswordRequest](const ResponseBaseSharedPtr& response)
    {
        if (response->isFailed()) {
            SPDLOG_WARN("[2] --> ChangeUsePassword() failed, error_string={}", response->errorString());
            QFAIL("[2] --> ChangeUsePassword() failed");
            Notify(false);
        } else {
            Notify(true);
        }
    });

    QVERIFY2(Wait(),"[2] --> Failed to change user password.");

    disconnectInternal();

    std::this_thread::sleep_for(std::chrono::seconds(5));

    //////////////////////////////////////////////////////////////////////////////
    /// The changed password is returned to the original password.

    //접속을 위해 변경된 패스워드로 접속하도록 설정한다.
    SPDLOG_DEBUG("[3] DeviceConnect with changed password");

    m_password = changeUserPasswordRequest->password;
    QVERIFY2(connectInternal(), "[3] --> Can not connect to the device..");

    // Change User Password (원래 패스워드로 다시 설정한다.)
    SPDLOG_DEBUG("[4] Change original User Password");

    changeUserPasswordRequest->password = originalPassword;
    InitializeNotify();
    m_deviceClient->DeviceChangeUserPassword(changeUserPasswordRequest,[this, changeUserPasswordRequest](const ResponseBaseSharedPtr& response)
    {
        if (response->isFailed()) {
            SPDLOG_WARN("[4] --> ChangeUsePassword() failed, error_string={}", response->errorString());
            QFAIL("[4] --> ChangeUsePassword() failed");
            Notify(false);
        }else {
            Notify(true);
        }
    });

    QVERIFY2(Wait(), "[4] ---> Failed to change user password.");

    disconnectInternal();
    QTRY_VERIFY_WITH_TIMEOUT(m_deviceClient == nullptr, 1000);
}
