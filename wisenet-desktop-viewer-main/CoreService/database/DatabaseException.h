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

#include <iostream>
#include <exception>

#include <QtSql/QSqlError>
//#include <ErrorCode.h>

namespace Wisenet
{
namespace Core
{
class ManagementDatabaseException : public std::exception {

public:
    ManagementDatabaseException(const std::string& what)
        :m_what(what)
    {

    }
    ManagementDatabaseException(const QSqlError& error)
    {
        m_what = error.text().toStdString();
        m_what += "(";
        m_what += error.nativeErrorCode().toStdString();
        m_what += ").";
    }
    ManagementDatabaseException(const std::string& query, const QSqlError& error)
    {
        m_what = "Query:" + query;
        m_what += ", Error:" + error.text().toStdString();
        m_what += "(" + error.nativeErrorCode().toStdString() + ").";
    }
    const char * what() const throw()
    {
        return m_what.c_str();
    }

private:
    std::string m_what;
};

class LogDatabaseException : public std::exception {

public:
    LogDatabaseException(const std::string& what)
        :m_what(what)
    {

    }
    LogDatabaseException(const QSqlError& error)
    {
        m_what = error.text().toStdString();
        m_what += "(";
        m_what += error.nativeErrorCode().toStdString();
        m_what += ").";
    }
    LogDatabaseException(const std::string& query, const QSqlError& error)
    {
        m_what = "Query:" + query;
        m_what += ", Error:" + error.text().toStdString();
        m_what += "(" + error.nativeErrorCode().toStdString() + ").";
    }
    const char * what() const throw()
    {
        return m_what.c_str();
    }

private:
    std::string m_what;
};

class DashboardDatabaseException : public std::exception {

public:
    DashboardDatabaseException(const std::string& what)
        :m_what(what)
    {

    }
    DashboardDatabaseException(const QSqlError& error)
    {
        m_what = error.text().toStdString();
        m_what += "(";
        m_what += error.nativeErrorCode().toStdString();
        m_what += ").";
    }
    DashboardDatabaseException(const std::string& query, const QSqlError& error)
    {
        m_what = "Query:" + query;
        m_what += ", Error:" + error.text().toStdString();
        m_what += "(" + error.nativeErrorCode().toStdString() + ").";
    }
    const char * what() const throw()
    {
        return m_what.c_str();
    }

private:
    std::string m_what;
};

}
}
