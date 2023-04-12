#pragma once

#include <memory>
#include <functional>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio/strand.hpp>

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;
namespace ssl = asio::ssl;

class HttpStream
{
public :
    HttpStream(asio::strand<boost::asio::io_context::executor_type>& strand)
        : m_strand(strand)
        , m_isSsl(false)
    {
        m_stream = std::make_shared<beast::tcp_stream>(m_strand);
        m_sslStream = nullptr;
    }

    ~HttpStream()
    {}

    void reset_connection(const bool isSsl)
    {
        m_stream = nullptr;
        m_sslStream = nullptr;

        m_isSsl = isSsl;
        if (m_isSsl) {
            beast::error_code ec;
            ssl::context ctx(ssl::context::tls);
            ctx.set_verify_mode(ssl::verify_none);
            ctx.set_default_verify_paths(ec);
            m_sslStream = std::make_shared<beast::ssl_stream<beast::tcp_stream>>(m_strand, ctx);
        }
        else {
            m_stream = std::make_shared<beast::tcp_stream>(m_strand);
        }
    }

    bool is_open()
    {
        if (m_stream)
            return m_stream->socket().is_open();
        else
            return beast::get_lowest_layer(*m_sslStream).socket().is_open();
    }

    bool is_ssl()
    {
        return (m_isSsl && m_sslStream);
    }

    template <typename SettableSocketOption>
    void set_option(const SettableSocketOption& option)
    {
        if (m_stream)
            m_stream->socket().set_option(option);
        else
            beast::get_lowest_layer(*m_sslStream).socket().set_option(option);
    }

    void expires_after(int timeoutSec)
    {
        if (m_stream) {
            m_stream->expires_after(std::chrono::seconds(timeoutSec));
        }
        else {
            beast::get_lowest_layer(*m_sslStream).expires_after(std::chrono::seconds(timeoutSec));
        }
    }
    void expires_never()
    {
        if (m_stream) {
            m_stream->expires_never();
        }
        else {
            beast::get_lowest_layer(*m_sslStream).expires_never();
        }
    }

    beast::tcp_stream& tcp_stream()
    {
        if (m_stream) {
            return *m_stream;
        }
        else {
            return beast::get_lowest_layer(*m_sslStream);
        }
    }

    beast::ssl_stream<beast::tcp_stream>& ssl_stream()
    {
        return *m_sslStream;
    }

    void close()
    {        
        if (m_stream) {
            // Gracefully close the socket
            beast::error_code ec;
            m_stream->socket().shutdown(asio::ip::tcp::socket::shutdown_both, ec);
            m_stream->close();
        }
        else {

            beast::error_code ec;

            //TODO :: ssl shutdown hang on event session.
            //SPDLOG_DEBUG("START SSL CLOSE!!");
            //m_sslStream->shutdown(ec);
            //SPDLOG_DEBUG("END SSL CLOSE!!");
            beast::get_lowest_layer(*m_sslStream).socket().shutdown(asio::ip::tcp::socket::shutdown_both, ec);
            beast::get_lowest_layer(*m_sslStream).close();
        }
    }

private:
    bool m_isSsl = false;
    asio::strand<boost::asio::io_context::executor_type>&  m_strand;
    std::shared_ptr<beast::tcp_stream> m_stream;
    std::shared_ptr<beast::ssl_stream<beast::tcp_stream>> m_sslStream;
};



