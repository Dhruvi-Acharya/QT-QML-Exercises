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
#include "SunapiBroadcastProtocol.h"
#include "CoreServiceLogSettings.h"

namespace Wisenet
{
namespace Core
{

QList<QNetworkAddressEntry> GetDiscoverySources()
{
    spdlog::stopwatch sw;

    QList<QNetworkAddressEntry> validAddrs;

    SPDLOG_DEBUG("Start GetDiscoverySources()");
    //spdlog::enable_backtrace(32);

    auto nics = QNetworkInterface::allInterfaces();
    for (const auto& nic : nics) {
        if (!nic.isValid())
            continue;

        if (nic.type() != QNetworkInterface::InterfaceType::Wifi &&
            nic.type() != QNetworkInterface::InterfaceType::Ethernet)
            continue;

        if (!nic.flags().testFlag(QNetworkInterface::InterfaceFlag::CanBroadcast))
            continue;

        if (!nic.flags().testFlag(QNetworkInterface::InterfaceFlag::IsUp))
            continue;

        spdlog::trace("nic:: {}", nic.hardwareAddress().toUtf8().toStdString());
        auto addresses = nic.addressEntries();
        for (const auto& addr : addresses) {
            if (addr.ip().protocol() == QAbstractSocket::IPv4Protocol) {
                spdlog::trace("--addr:: {}", addr.ip().toString().toStdString());
                validAddrs.push_back(addr);
                break;
            }
        }
    }
    //spdlog::dump_backtrace();
    //spdlog::disable_backtrace();

    SPDLOG_TRACE("End GetDiscoverySources(), Elapsed={:.4} seconds", sw);
    return validAddrs;
}

QList<QNetworkAddressEntry> GetDiscoveryIPv6Sources()
{
    spdlog::stopwatch sw;

    QList<QNetworkAddressEntry> validAddrs;

    SPDLOG_DEBUG("Start GetDiscoverySources()");
    //spdlog::enable_backtrace(32);

    auto nics = QNetworkInterface::allInterfaces();
    for (const auto& nic : nics) {
        if (!nic.isValid())
            continue;

        if (nic.type() != QNetworkInterface::InterfaceType::Wifi &&
            nic.type() != QNetworkInterface::InterfaceType::Ethernet)
            continue;

        if (!nic.flags().testFlag(QNetworkInterface::InterfaceFlag::CanBroadcast))
            continue;

        if (!nic.flags().testFlag(QNetworkInterface::InterfaceFlag::IsUp))
            continue;

        spdlog::trace("nic:: {}", nic.hardwareAddress().toUtf8().toStdString());
        auto addresses = nic.addressEntries();
        for (const auto& addr : addresses) {
            if (addr.ip().protocol() == QAbstractSocket::IPv6Protocol) {
                spdlog::trace("--addr:: {}", addr.ip().toString().toStdString());
                SPDLOG_WARN("IPv6 address from {}.", addr.ip().toString().toStdString());
                validAddrs.push_back(addr);
                //break;
            }
        }
    }
    //spdlog::dump_backtrace();
    //spdlog::disable_backtrace();

    SPDLOG_TRACE("End GetDiscoveryIPv6Sources(), Elapsed={:.4} seconds", sw);
    return validAddrs;
}
}
}
