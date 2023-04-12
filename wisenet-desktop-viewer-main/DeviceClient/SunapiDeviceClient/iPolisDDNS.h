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
#include <boost/asio.hpp>
#include <boost/unordered_map.hpp>

namespace asio = boost::asio;

typedef boost::unordered_map<std::string, std::string> ResponseMap;

struct IpolisDDNSResponse
{
    std::string ip;
    int httpPort;
};

class IpolisDDNS: public std::enable_shared_from_this<IpolisDDNS>
{
    enum { SERVER_URL_LENGTH = 256, SERVER_OLD_PORT = 7016, SERVER_NEW_PORT = 7026 };
public:
    IpolisDDNS(asio::io_context& ioContext);
    ~IpolisDDNS();
     bool GetDDNSAddress(const std::string& ddnsId, std::string& ip, unsigned short & port, std::string& productP2PID, unsigned int& status);
     bool GetUrlDDNSAddress(const std::string& url, std::string& ip);
     bool GetUrlAddress(const std::string& url, std::string& ip, bool isSSL);

private:
     bool GetDDNSAddressByIp(const std::string& serverIp, const unsigned short serverPort, const std::string& ddnsId, std::string& ip, unsigned short & port, std::string& productP2PID, unsigned int& status);

     void ParseResponse(std::istream& stream, ResponseMap& resMap);

     void ErrorComplete(const boost::system::error_code &error);


    asio::io_context& m_ioContext;
    asio::strand<boost::asio::io_context::executor_type> m_strand;
    boost::asio::streambuf m_responseStream;
};
