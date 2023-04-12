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

void Test__CoreService::test_Login()
{
    // logout first
    if (!UserLogout(m_user.userID))
        QFAIL("UserLogout() failed");

    std::string id, password, falseUser, falsePassword;

    try {
        const auto data = toml::parse(m_confPath);
        const auto& user = toml::find(data, "user");
        id = toml::find<std::string>(user, "userName");
        password = toml::find<std::string>(user, "password");
        falseUser = toml::find<std::string>(user, "falseUserName");
        falsePassword = toml::find<std::string>(user, "falsePassword");

        SPDLOG_INFO("CoreService user id={}, pw={}, falseId={}, falsePw={}",
                    id, password, falseUser, falsePassword);
    }
    catch (std::exception &e) {
        std::string errMsg = "LoadLoginQtestData() exception::" + std::string(e.what());
        QFAIL(errMsg.c_str());
    }

    QVERIFY(id.length() > 0);
    QVERIFY(password.length() > 0);
    QVERIFY(falseUser.length() > 0);
    QVERIFY2(falsePassword.length() > 0, "falsePassword is empty!!");

    // false test
    QVERIFY2(UserLogin(falseUser, falsePassword) == false, "False login test failed");

    // true test
    QVERIFY2(UserLogin(id, password) == true, "login test failed");

}
