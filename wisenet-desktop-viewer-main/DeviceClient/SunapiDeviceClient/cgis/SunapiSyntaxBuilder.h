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
#include <string>
#include <sstream>

namespace Wisenet
{
namespace Device
{

class SunapiSyntaxBuilder
{
public :
    SunapiSyntaxBuilder& CreateCgi(const std::string &cgi,
                                  const std::string &msubmenu,
                                  const std::string &action)
    {
        m_urlContents << "/stw-cgi/" << cgi << ".cgi?msubmenu=" << msubmenu << "&action=" << action;
        return *this;
    }

    template<typename T>
    SunapiSyntaxBuilder& AddParameter(const std::string &parameter, T value)
    {
        m_urlContents << "&" << parameter << "=" << value;
        return *this;
    }

    SunapiSyntaxBuilder& AddParameter(const std::string &parameter, bool value)
    {
        m_urlContents << "&" << parameter << "=" << (value ? "True" : "False");
        return *this;
    }

    std::string GetUrl() const
    {
        return m_urlContents.str();
    }

private:
    std::stringstream m_urlContents;
};

}
}
