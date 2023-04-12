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
#pragma once
#include <boost/asio.hpp>
#include <QString>

static uint ipv4StringToUint(const QString& qstr)
{
    boost::system::error_code ecc;
    std::string str = qstr.toStdString();
    boost::asio::ip::address_v4 addr = boost::asio::ip::make_address_v4(str, ecc);
    if (!ecc) {
        return addr.to_uint();
    }
    return 0;
}
