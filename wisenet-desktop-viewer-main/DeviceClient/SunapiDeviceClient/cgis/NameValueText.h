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
#include <string>
#include <unordered_map>
#include <vector>
#include <boost/optional.hpp>

namespace Wisenet
{
namespace Device
{

class NameValueText
{
public:
    explicit NameValueText(unsigned int keySize = 0);

    bool Read(const std::string& content);
    boost::optional<std::string> getStringValue(const std::string& key);
    boost::optional<bool> getBoolValue(const std::string& key);
    boost::optional<int> getIntValue(const std::string& key);

    std::string getString(const std::string& key);
    int getInt(const std::string& key);
    bool getBool(const std::string& key, const bool def = false);
    void Clear();
public:
    std::unordered_map<std::string, std::string>  m_value;
    // must use m_listValue to read in order from contents
    std::vector<std::pair<std::string, std::string>> m_listValue;
};

}
}
