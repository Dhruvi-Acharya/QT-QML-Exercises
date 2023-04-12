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
#include "EasyConnectionLogger.h"

#include "sipproxy.h"

namespace Wisenet
{
namespace Device
{

void Log(char* msg)
{
    EasyConnectionLogger::getInstance().printLog(msg);
}

EasyConnectionLogger::EasyConnectionLogger()
{

}

void EasyConnectionLogger::printLog(char* msg)
{
    m_logFunc(msg);
}

void EasyConnectionLogger::SetLog(unsigned int logLevel, std::function<void(char*)> logger)
{
        m_logFunc = logger;
        SIPPROXY_SetLogLevel("", logLevel, &Log);
}

}
}
