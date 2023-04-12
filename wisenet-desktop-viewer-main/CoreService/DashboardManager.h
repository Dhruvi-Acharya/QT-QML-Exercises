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
#pragma once

#include <memory>
#include <boost/asio.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/io_context.hpp>

#include "database/DashboardDatabase.h"

namespace Wisenet
{
namespace Core
{

class DashboardManager
{
public:
    DashboardManager();

    bool Open();
    void Close();

    static bool Delete();

    // Dashboard data
    void SaveDashboardData(const DashboardData& dashboardData, const int deleteTime);
    void GetDashboardData(std::map<int, DashboardData>& dashboardDataMap);
    int GetDashboardLatestTime();

private:
    bool Load();
    void ClearAll();

    std::shared_ptr<DashboardDatabase> m_dashboardDatabase;
};

}
}