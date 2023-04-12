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

#include "iPolisDDNS.h"
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include "LogSettings.h"
#include "WeakCallback.h"


IpolisDDNS::IpolisDDNS(asio::io_context& ioContext)
    : m_ioContext(ioContext)
    , m_strand(ioContext.get_executor())
{
    SPDLOG_INFO("Create a new IpolisDDNS instance");
     //m_recvPackets.resize(4096);
}

IpolisDDNS::~IpolisDDNS()
{
    SPDLOG_INFO("Destroy a IpolisDDNS instance");
}

void IpolisDDNS::ParseResponse(std::istream& stream, ResponseMap& resMap)
{
    std::string line;

    while(std::getline(stream, line)){
        SPDLOG_INFO("IpolisDDNS::GetDDNSAddress() dataTemp= {}", line);

        if(line.substr(0,4) == "BMSP"){
            std::vector<std::string> tokens;
            boost::split(tokens, line, boost::is_any_of("\t "), boost::token_compress_on);
            if (tokens.size() > 2){
                resMap.insert(ResponseMap::value_type("status", tokens[1]));
            }
        }
        else{
            const auto equals_idx = line.find_first_of(':');
            if (std::string::npos != equals_idx)
            {
                std::string key = line.substr(0, equals_idx);
                std::string value = line.substr(equals_idx + 1);
                boost::trim(key);
                boost::trim(value);
                boost::to_lower(key);
                //PLOG_TRACE << key << ":" << value;
                resMap.insert(ResponseMap::value_type(key, value));
            }
        }
    }
}

std::string getResponseMap(std::string key, ResponseMap& resMap){
    auto res = resMap.find(key);
    if (res != resMap.end())
    {
        boost::trim(res->second);
        return res->second;
    }
    return "";
}
bool IpolisDDNS::GetDDNSAddressByIp(const std::string& serverIp, const unsigned short serverPort, const std::string& ddnsId, std::string& ip, unsigned short & port, std::string& productP2PID, unsigned int& status)
{
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(serverIp), serverPort);
    boost::asio::ip::tcp::socket socket(m_strand);

    status = 0;

    try{

        boost::system::error_code connect_error, write_error;
        socket.connect(endpoint, connect_error);

        if (connect_error)
        {
            SPDLOG_INFO("IpolisDDNS::GetAddress connect_error.value({}) connect_error.message({}) ", connect_error.value(), connect_error.message());
            socket.close();
            return false;
        }
        
        std::ostringstream stream;
        stream << "DIRGETSVRINFO BMSP/0.3\r\nID: " << ddnsId << "\r\n\r\n";

        SPDLOG_INFO("IpolisDDNS::GetAddress Connect IP:{}, PORT:{}, Request:{}", serverIp, serverPort, stream.str());

        //auto buffer = std::make_shared<std::string>(stream.str());
        socket.write_some(boost::asio::buffer(stream.str()), write_error);

        if (write_error)
        {
            SPDLOG_INFO("IpolisDDNS::GetAddress SendMessage write_error.value({}) write_error.message({}) ", write_error.value(), write_error.message());
        } else {
            SPDLOG_INFO("IpolisDDNS::GetAddress SendMessage");
        }
        

        boost::asio::streambuf receiveBuffer;
        boost::asio::read_until(socket, receiveBuffer, "\r\n\r\n");
        std::istream is(&receiveBuffer);
        ResponseMap resMap;
        ParseResponse(is, resMap);

        std::string strIp = getResponseMap("ip", resMap);
        std::string strPort = getResponseMap("web_port", resMap);
        std::string strStatus = getResponseMap("status", resMap);
        std::string strProductP2PID = getResponseMap("description", resMap);

        SPDLOG_INFO("IpolisDDNS::GetDDNSAddress() Response : ip={} port={} status={} productP2PID={}", strIp, strPort, strStatus, strProductP2PID);

        socket.close();
        if(strStatus == "200" && strPort != "" && strPort != "0" && strIp != ""){
            ip = strIp;
            port = boost::lexical_cast<unsigned short>(strPort);
            productP2PID = strProductP2PID;
            status = 200;
            return true;
        }else{
            status = boost::lexical_cast<unsigned int>(strStatus);
        }
    }
    catch (std::exception& e)
    {
        SPDLOG_ERROR("IpolisDDNS::GetDDNSAddress() exception = {}", e.what());
        status = 0;
        socket.close();
    }
    return false;
}

bool IpolisDDNS::GetDDNSAddress(const std::string& ddnsId, std::string& ip, unsigned short & port, std::string& productP2PID, unsigned int& status)
{
#if 0
    ip = "55.101.67.112";
    port = 80;
    productP2PID = "HBIRD_00096CEDFE11";
    SPDLOG_INFO("Ipolis Sample Data = ip={}, port={}, productID={}",ip, port, productP2PID);
    return true;
#endif

    SPDLOG_INFO("IpolisDDNS::GetAddress {} ", ddnsId);
    std::string ddnsServerIp;
    if(!GetUrlDDNSAddress("ddns.hanwha-security.com", ddnsServerIp)){
        status = 0;
        return false;
    }

    if(GetDDNSAddressByIp(ddnsServerIp, SERVER_NEW_PORT, ddnsId, ip, port, productP2PID, status))
        return true;
    else
        return GetDDNSAddressByIp(ddnsServerIp, SERVER_OLD_PORT, ddnsId, ip, port, productP2PID, status);

}
bool SocketWasClosed(const boost::system::error_code& error)
{
    return ((boost::asio::error::eof == error)
            || (boost::asio::error::connection_reset == error)
            || (boost::asio::error::connection_aborted == error));
}

bool OperationAborted(const boost::system::error_code& error)
{
    return (boost::asio::error::operation_aborted == error);
}



bool IpolisDDNS::GetUrlDDNSAddress(const std::string& url, std::string & serverIp)
{
    SPDLOG_INFO("IpolisDDNS::GetUrlAddress url:{} ", url);

    try{
        asio::ip::tcp::resolver::query query(url,"");
        boost::asio::ip::tcp::resolver resolver(m_ioContext);
        asio::ip::tcp::resolver::iterator destination = resolver.resolve(query);
        asio::ip::tcp::resolver::iterator end;
        asio::ip::tcp::endpoint endpoint;

        while (destination != end){
            endpoint = *destination++;
            serverIp = endpoint.address().to_string();
            SPDLOG_INFO("IpolisDDNS::GetUrlAddress ip:{} ", serverIp);
            return true;
        }
    } catch(...){

    }
    return false;

}

bool IpolisDDNS::GetUrlAddress(const std::string& url, std::string & serverIp, bool isSSL)
{
    SPDLOG_INFO("IpolisDDNS::GetUrlAddress url:{} ", url);

    try{

        std::vector<std::string> tokens;
        //boost::split(tokens, url, boost::is_any_of("://"));
        std::string spliter = isSSL?"https":"http";
        std::string destUrl = url;

        auto current =url.find("://");

        if(current != std::string::npos){
            spliter = url.substr(0, current);
            destUrl = url.substr(current + 3, url.size() - current -3);
            SPDLOG_INFO("IpolisDDNS::GetUrlAddress2 url:{} dest:{} spliter:{} tokenSize:{}", url, destUrl, spliter, tokens.size());

        }

        SPDLOG_INFO("IpolisDDNS::GetUrlAddress3 url:{} dest:{} spliter:{} tokenSize:{}", url, destUrl, spliter, tokens.size());


        asio::ip::tcp::resolver::query query(destUrl, spliter);
        boost::asio::ip::tcp::resolver resolver(m_ioContext);
        asio::ip::tcp::resolver::iterator destination = resolver.resolve(query);
        asio::ip::tcp::resolver::iterator end;
        asio::ip::tcp::endpoint endpoint;

        while (destination != end){
            endpoint = *destination++;
            serverIp = endpoint.address().to_string();
            SPDLOG_INFO("IpolisDDNS::GetUrlAddress ip:{} ", serverIp);
            return true;
        }
    } catch(...){

    }
    return false;

}
