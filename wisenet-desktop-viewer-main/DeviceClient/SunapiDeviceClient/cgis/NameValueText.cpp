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
#include "NameValueText.h"
#include "SunapiDeviceClientUtil.h"
#include "LogSettings.h"

#include <sstream>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>

namespace Wisenet
{
namespace Device
{

NameValueText::NameValueText(unsigned int keySize)
    :m_value()
{
    if(keySize > 0)
        m_value.reserve(keySize);
}

bool NameValueText::Read(const std::string &content)
{
    std::string line("");
    std::istringstream data(content);

    SPDLOG_TRACE("Start of a content");
    while(std::getline(data,line))
    {
        // 주석부분은 제거.
       char compare = '"';
       if(0 == line.compare(0,1, &compare)) {
           SPDLOG_DEBUG("Comment line : {}" , line);
           continue;
       }

        const auto pos = line.find_first_of('=');
        if(std::string::npos != pos)
        {
           auto key = line.substr(0, pos);
           auto value = line.substr(pos + 1);

           boost::trim(key);
           boost::trim(value);

           SPDLOG_TRACE("KEY:{}=VALUE:{}",key, value);
           m_value.emplace(key, value);

           m_listValue.emplace_back(std::make_pair(key, value));
        }
    }
    SPDLOG_TRACE("End of a content");

    return true;
}

boost::optional<std::string> NameValueText::getStringValue(const std::string &key)
{
   auto itor = m_value.find(key);

   if(itor == m_value.end())
   {
       return boost::none;
   }

   return boost::optional<std::string>(itor->second);
}

std::string NameValueText::getString(const std::string& key)
{
    auto itor = m_value.find(key);

    if (itor == m_value.end())
    {
        return "";
    }

    return boost::optional<std::string>(itor->second).get();
}

int NameValueText::getInt(const std::string& key)
{
    auto val = getIntValue(key);

    if (val == boost::none)
        return 0;

    return val.get();
}

bool NameValueText::getBool(const std::string& key, const bool def)
{
    auto val = getBoolValue(key);

    if (val == boost::none)
        return def;

    return val.get();
}

void NameValueText::Clear()
{
    m_value.clear();
    m_listValue.clear();
}

boost::optional<bool> NameValueText::getBoolValue(const std::string &key)
{
    bool value = false;

    auto itor = m_value.find(key);

    if(itor == m_value.end())
    {
        return boost::none;
    }

    if(boost::iequals(itor->second,"True"))
    {
        value = true;
    }
    else
    {
        value = false;
    }


    return boost::optional<bool>(value);
}

boost::optional<int> NameValueText::getIntValue(const std::string &key)
{
    int value = 0;

    auto itor = m_value.find(key);

    if(itor == m_value.end())
    {
        return boost::none;
    }
    try_stoi(itor->second, &value);

    return boost::optional<int>(value);
}

}
}
