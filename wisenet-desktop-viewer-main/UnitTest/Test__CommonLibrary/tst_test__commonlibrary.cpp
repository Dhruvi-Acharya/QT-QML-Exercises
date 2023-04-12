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
#include "Test__CommonLibrary.h"

Test__CommonLibrary::Test__CommonLibrary()
{

}

Test__CommonLibrary::~Test__CommonLibrary()
{

}

void Test__CommonLibrary::initTestCase()
{
    Common::InitializeLogSettings("logs");
}

void Test__CommonLibrary::cleanupTestCase()
{

}

QTEST_MAIN(Test__CommonLibrary)
//QTEST_APPLESS_MAIN(CommonLibrary)
//#include "tst_commonlibrary.moc"
