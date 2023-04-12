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

#include "Test__Util.h"
#include "TimeUtil.h"

void Test__Util::test_timeUtil()
{
    auto nowMsec = Common::currentUtcMsecs();
    SPDLOG_INFO("now msec={}, {}, {}, {}, {}",
                nowMsec,
                Common::utcMsecsToUtcIsoString(nowMsec, true),
                Common::utcMsecsToUtcIsoString(nowMsec),
                Common::utcMsecsToLocalIsoString(nowMsec, true),
                Common::utcMsecsToLocalIsoString(nowMsec));

    auto nowSec = Common::currentUtcSecs();
    auto nowSec2 = Common::msecsToSecs(nowMsec);

    SPDLOG_INFO("now sec={}, sec2={}, {}, {}",
                nowSec, nowSec2,
                Common::utcSecsToUtcIsoString(nowSec),
                Common::utcSecsToLocalIsoString(nowSec));

    QCOMPARE(nowSec, nowSec2);

    QTest::qSleep(50);
    auto nowMsec2 = Common::currentUtcMsecs();
    auto msecDiff = nowMsec2 - nowMsec;
    SPDLOG_INFO("now msec after 50ms={}, diff = {}, {}, {}, {}, {}",
                nowMsec2, msecDiff,
                Common::utcMsecsToUtcIsoString(nowMsec2, true),
                Common::utcMsecsToUtcIsoString(nowMsec2),
                Common::utcMsecsToLocalIsoString(nowMsec2, true),
                Common::utcMsecsToLocalIsoString(nowMsec2));


    QVERIFY(msecDiff >= 50 && msecDiff <= 60);

    //auto dateTime = QDateTime();
    //dateTime.setMSecsSinceEpoch(nowMsec);
    std::string isoUtc = "2021-02-03T23:59:59Z";
    std::string isoLocal = "2021-02-03T23:59:59+09:00";
    std::string YYYYMMDDhhmmss = "2021-02-03 23:59:59";
    std::string YYYYMMDDhhmmssZ = "2021-02-03 23:59:59Z";

    auto msec3 = Common::utcMsecsFromIsoString(isoUtc);
    auto msec4 = Common::utcMsecsFromIsoString(isoLocal);
    auto sec3 = Common::utcSecsFromIsoString(isoUtc);
    auto sec4 = Common::utcSecsFromIsoString(isoLocal);
    auto secFormat = Common::utcSecsFromIsoString(YYYYMMDDhhmmss);
    auto secFormatZ = Common::utcSecsFromIsoString(YYYYMMDDhhmmssZ);

    SPDLOG_INFO("from string={}, {}, {}, {}, {}, {}, YYYYMMDDhhmmss={}, Z={}",
                Common::utcMsecsToUtcIsoString(msec3),
                Common::utcMsecsToLocalIsoString(msec4),
                Common::utcMsecsToUtcIsoString(msec4),
                Common::utcSecsToUtcIsoString(sec3),
                Common::utcSecsToLocalIsoString(sec4),
                Common::utcSecsToUtcIsoString(sec4),
                Common::utcSecsToUtcIsoString(secFormat),
                Common::utcSecsToUtcIsoString(secFormatZ)
                );

    QVERIFY(isoUtc == Common::utcMsecsToUtcIsoString(msec3));
}

