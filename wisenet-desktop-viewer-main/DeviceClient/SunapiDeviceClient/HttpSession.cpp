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

#include "HttpSession.h"
#include "LogSettings.h"
#include "WeakCallback.h"
#include "TextEncodeUtil.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include "DeviceCertificate.h"
#include "VersionManager.h"

const std::string kMonitorDiffuri = u8"/stw-cgi/eventstatus.cgi?msubmenu=eventstatus&action=monitordiff";
const std::string kPosMonitorDiffuri = u8"/stw-cgi/recording.cgi?msubmenu=posdata&action=monitordiff";

const std::string kAgentBeast = BOOST_BEAST_VERSION_STRING;
const std::string kAgentName = "Wisenet Sunapi Agent("+kAgentBeast+")";

const std::string kPostMultipartBoundary = "+=================hanwha_wisent_viewer==+";
const std::string kPostMultipartLastLine = "\r\n--" + kPostMultipartBoundary + "--\r\n";

// LOGGER MACRO REDEFINE
#undef SPDLOG_LOGGER_CALL
#define SPDLOG_LOGGER_CALL(logger, level, ...) (logger)->log(spdlog::source_loc{__FILE__, __LINE__, m_logPrefix.c_str()}, level, __VA_ARGS__)

/////// http beast read/write wrapper /////////////////////////////////////////////

template<BOOST_BEAST_ASYNC_TPARAM2 WriteHandler>
static void http_async_write(
        HttpStream& stream,
        http::request<http::string_body>& request,
        WriteHandler&& handler)
{
    if(stream.is_ssl()) {
        http::async_write(stream.ssl_stream(), request, handler);
    }
    else {
        http::async_write(stream.tcp_stream(), request, handler);
    }
}

template<BOOST_BEAST_ASYNC_TPARAM2 WriteHandler>
static void http_async_writeBuffer_header(
        HttpStream& stream,
        http::request_serializer<http::buffer_body>& sr,
        WriteHandler&& handler)
{
    if (stream.is_ssl()) {
        http::async_write_header(stream.ssl_stream(), sr, handler);
    }
    else {
        http::async_write_header(stream.tcp_stream(), sr, handler);
    }
}
template<BOOST_BEAST_ASYNC_TPARAM2 WriteHandler>
static void http_async_writeBuffer(
        HttpStream& stream,
        http::request_serializer<http::buffer_body>& sr,
        WriteHandler&& handler)
{
    if (stream.is_ssl()) {
        http::async_write(stream.ssl_stream(), sr, handler);
    }
    else {
        http::async_write(stream.tcp_stream(), sr, handler);
    }
}


template<BOOST_BEAST_ASYNC_TPARAM2 ReadHandler>
static void http_async_read(
        HttpStream& stream,
        beast::flat_buffer& buffer,
        http::response<http::string_body>& response,
        ReadHandler&& handler)
{
    if(stream.is_ssl()) {
        http::async_read(stream.ssl_stream(), buffer, response, handler);
    }
    else {
        http::async_read(stream.tcp_stream(), buffer, response, handler);
    }
}

template<BOOST_BEAST_ASYNC_TPARAM2 ReadHandler>
static void http_async_read_header(
        HttpStream& stream,
        beast::flat_buffer& buffer,
        http::response_parser<http::empty_body>& parser,
        ReadHandler&& handler)
{
    if(stream.is_ssl()) {
        http::async_read_header(stream.ssl_stream(), buffer, parser, handler);
    }
    else {
        http::async_read_header(stream.tcp_stream(), buffer, parser, handler);
    }
}

template<BOOST_BEAST_ASYNC_TPARAM2 ReadHandler>
static void http_async_read(
        HttpStream& stream,
        beast::flat_buffer& buffer,
        http::response_parser<http::string_body>& parser,
        ReadHandler&& handler)
{
    if(stream.is_ssl()) {
        http::async_read(stream.ssl_stream(), buffer, parser, handler);
    }
    else {
        http::async_read(stream.tcp_stream(), buffer, parser, handler);
    }
}

template<BOOST_BEAST_ASYNC_TPARAM2 ReadHandler>
static void http_async_read_some(
        HttpStream& stream,
        beast::flat_buffer& buffer,
        http::response_parser<http::string_body>& parser,
        ReadHandler&& handler)
{
    if(stream.is_ssl()) {
        http::async_read_some(stream.ssl_stream(), buffer, parser, handler);
    }
    else {
        http::async_read_some(stream.tcp_stream(), buffer, parser, handler);
    }
}

static bool isPostMethod(RequestMethod taskMethod)
{
    if(     taskMethod == RequestMethod::updateFirmware ||
            taskMethod == RequestMethod::encodedUpload ||
            taskMethod == RequestMethod::encodedData ||
            taskMethod == RequestMethod::updateConfigBackupPassword ||
            taskMethod == RequestMethod::updateConfigRestorePassword) {
        return true;
    }
    return false;
}

static std::string fileNameFromPath(const std::string &filePath)
{
    // folder separator
#ifdef _WIN32
    static const char folder_sep = '\\';
#else
    static const char folder_sep = '/';
#endif
    std::string path = filePath;
    std::replace(path.begin(), path.end(), '/', folder_sep);
    auto pos = path.find_last_of(folder_sep);
    if (pos == std::string::npos)
        return path;
    return path.substr(pos + 1, path.length() - pos);
}


static bool HttpSocketWasClosed(const beast::error_code& ec)
{

    return ( (http::error::end_of_stream == ec)
            || (boost::asio::error::eof == ec)
            || (boost::asio::error::connection_reset == ec)
            || (boost::asio::error::connection_aborted == ec)
            || (boost::asio::error::connection_refused == ec));
}


/////// HttpSession /////////////////////////////////////////////
HttpSession::HttpSession(asio::io_context& ioc)
    : m_ioContext(ioc)
    , m_strand(asio::make_strand(ioc.get_executor()))
    , m_resolver(m_strand)
    , m_connectionTimeoutSec(0) // 2023.01.11. coverity (ubuntu)
    , m_queue(*this)    
    , m_logPrefix("[HTTP] ")    
    , m_httpIO(std::make_shared<HttpIoStruct>(m_strand))    
{    
    SPDLOG_DEBUG("HttpSession::HttpSession()");
}

HttpSession::~HttpSession()
{
    SPDLOG_DEBUG("HttpSession::~HttpSession()");
}

void HttpSession::SetConnection(const std::string& host, const unsigned short port,
                                const std::string& user, const std::string& password,
                                const std::string& mac,
                                const bool& isSSL /* =false */,
                                const int connectionTimeoutSec /* = 10 */)
{
    SPDLOG_DEBUG("Set HTTP connection info :: host={}, port={}, mac={}, isSSL={}", host, port, mac, isSSL);

    asio::dispatch(m_strand,
                   WeakCallback(shared_from_this(),
                                [this, host, port, user, password,
                                isSSL, connectionTimeoutSec, mac]()
    {
        closeInternal(true);
        m_logPrefix = "[HTTP](" + host + ") ";
        m_host = host;
        m_port = std::to_string(port);
        m_user = user;
        m_password = password;
        m_isSSL = isSSL;
        m_connectionTimeoutSec = connectionTimeoutSec;
        m_mac = mac;
    }));
}

void HttpSession::SetCheckDeviceCert(bool check)
{
    m_isCheckDeviceCert = check;
}

void HttpSession::Close()
{
    SPDLOG_INFO("Close HTTP session start :: host={}, port={}, isSSL={}", m_host, m_port, m_isSSL);
    // close() 호출시에는 shared_ptr이 삭제되더라도 정상적으로 close()가 완료되도록
    // weak_ptr을 사용하지 않는다.
    auto self = shared_from_this();
    asio::dispatch(m_strand, [this, self]()
    {
        closeInternal(true);
    });
    SPDLOG_INFO("Close HTTP session end :: host={}, port={}, isSSL={}", m_host, m_port, m_isSSL);
}

void HttpSession::closeInternal(const bool cancelAll)
{
    SPDLOG_DEBUG("Close internally HTTP session :: host={}, port={}, isSSL={}, reset={}",
                 m_host, m_port, m_isSSL, cancelAll);

    if (cancelAll) m_queue.CancelAll();

    m_httpIO->m_httpStream.close();
    if (cancelAll)
        m_digestAuthenticator.ResetAuthorization();
    else
        m_digestAuthenticator.ResetRetryCount();
    
    m_httpIO->m_buffer.clear();
}


void HttpSession::HttpGetRequest(const std::string& uri, bool needJsonResponse,
                                 const HttpReponseHandler& responseHandler,
                                 const int timeoutSec, bool bigGet)
{
    auto taskPtr = std::make_shared<HttpRequestTask>();
    taskPtr->method = bigGet? RequestMethod::bigGet : RequestMethod::get;
    taskPtr->uri = Wisenet::Common::UrlEncode(uri);
    taskPtr->handler = responseHandler;
    taskPtr->timeoutSec = timeoutSec;
    taskPtr->lastStatus = http::status::ok;
    taskPtr->needJsonResponse = needJsonResponse;

    asio::post(m_strand, WeakCallback(shared_from_this(), [this, taskPtr]()
    {
        m_queue.Request(taskPtr);
    }));
}

void HttpSession::HttpGetDownload(const std::string &uri,
                                  const std::string &downloadFilePath,
                                  const HttpReponseHandler &responseHandler,
                                  const int timeoutSec)
{
    auto taskPtr = std::make_shared<HttpRequestTask>();
    taskPtr->method = RequestMethod::download;
    taskPtr->uri = Wisenet::Common::UrlEncode(uri);
    taskPtr->handler = responseHandler;
    taskPtr->timeoutSec = timeoutSec;
    taskPtr->lastStatus = http::status::ok;
    taskPtr->needJsonResponse = false;
    taskPtr->filePath = downloadFilePath;

    asio::post(m_strand, WeakCallback(shared_from_this(), [this, taskPtr]()
    {
        m_queue.Request(taskPtr);
    }));
}

void HttpSession::HttpPostDownload(const std::string &uri,
                                   const std::string &rawData,
                                  const std::string &downloadFilePath,
                                  const HttpReponseHandler &responseHandler,
                                  const int timeoutSec)
{
    auto taskPtr = std::make_shared<HttpRequestTask>();
    taskPtr->method = RequestMethod::updateConfigBackupPassword;
    taskPtr->uri = Wisenet::Common::UrlEncode(uri);
    taskPtr->handler = responseHandler;
    taskPtr->timeoutSec = timeoutSec;
    taskPtr->lastStatus = http::status::ok;
    taskPtr->needJsonResponse = false;
    taskPtr->filePath = downloadFilePath;
    taskPtr->postData = rawData;

    asio::post(m_strand, WeakCallback(shared_from_this(), [this, taskPtr]()
    {
        m_queue.Request(taskPtr);
    }));
}

void HttpSession::HttpPostEncodedUpload(const std::string &uri,
                                        const std::string &uploadFilePath,
                                        const HttpReponseHandler &responseHandler,
                                        const int timeoutSec)
{
    auto taskPtr = std::make_shared<HttpRequestTask>();
    taskPtr->method = RequestMethod::encodedUpload;
    taskPtr->uri = Wisenet::Common::UrlEncode(uri);
    taskPtr->handler = responseHandler;
    taskPtr->timeoutSec = timeoutSec;
    taskPtr->lastStatus = http::status::ok;
    taskPtr->needJsonResponse = false;
    taskPtr->filePath = uploadFilePath;

    asio::post(m_strand, WeakCallback(shared_from_this(), [this, taskPtr]()
    {
        m_queue.Request(taskPtr);
    }));
}

void HttpSession::HttpPostEncodedUploadPassword(const std::string &uri,
                                        const std::string &rawData,
                                        const std::string &uploadFilePath,
                                        const HttpReponseHandler &responseHandler,
                                        const int timeoutSec)
{
    auto taskPtr = std::make_shared<HttpRequestTask>();
    taskPtr->method = RequestMethod::updateConfigRestorePassword;
    taskPtr->uri = Wisenet::Common::UrlEncode(uri);
    taskPtr->handler = responseHandler;
    taskPtr->timeoutSec = timeoutSec;
    taskPtr->lastStatus = http::status::ok;
    taskPtr->needJsonResponse = false;
    taskPtr->postData = rawData;
    taskPtr->filePath = uploadFilePath;

    asio::post(m_strand, WeakCallback(shared_from_this(), [this, taskPtr]()
    {
        m_queue.Request(taskPtr);
    }));
}


void HttpSession::HttpPostUpdateFirmware(const std::string &uri,
                                         const std::string &uploadFilePath,
                                         const HttpReponseHandler &responseHandler,
                                         const int timeoutSec)
{
    auto taskPtr = std::make_shared<HttpRequestTask>();
    taskPtr->method = RequestMethod::updateFirmware;
    taskPtr->uri = Wisenet::Common::UrlEncode(uri);
    taskPtr->handler = responseHandler;
    taskPtr->timeoutSec = timeoutSec;
    taskPtr->lastStatus = http::status::ok;
    taskPtr->needJsonResponse = false;
    taskPtr->filePath = uploadFilePath;

    asio::post(m_strand, WeakCallback(shared_from_this(), [this, taskPtr]()
    {
        m_queue.Request(taskPtr);
    }));
}

void HttpSession::HttpPostEncodedData(const std::string &uri,
                                      const std::string &rawData,
                                      const HttpReponseHandler &responseHandler,
                                      const int timeoutSec)
{
    auto taskPtr = std::make_shared<HttpRequestTask>();
    taskPtr->method = RequestMethod::encodedData;
    taskPtr->uri = Wisenet::Common::UrlEncode(uri);
    taskPtr->handler = responseHandler;
    taskPtr->timeoutSec = timeoutSec;
    taskPtr->lastStatus = http::status::ok;
    taskPtr->needJsonResponse = false;
    taskPtr->postData = rawData;

    asio::post(m_strand, WeakCallback(shared_from_this(), [this, taskPtr]()
    {
        m_queue.Request(taskPtr);
    }));
}

bool HttpSession::openUploadFileStream(const HttpRequestTaskSharedPtr &taskPtr)
{
    taskPtr->uploadFileStream.open(taskPtr->filePath, std::ios::in | std::ios::binary);
    if (taskPtr->uploadFileStream.fail()) {
        SPDLOG_WARN("Can not open upload filestream, path={}", taskPtr->filePath);
        return false;
    }
    auto &is = taskPtr->uploadFileStream;
    is.seekg(0, is.end);
    taskPtr->uploadFileSize = is.tellg();
    is.seekg(0, is.beg);
    return true;
}

void HttpSession::startRequest(const HttpRequestTaskSharedPtr& taskPtr)
{
    // file upload task, open file first
    if (taskPtr->method == RequestMethod::encodedUpload ||
            taskPtr->method == RequestMethod::updateConfigRestorePassword ||
            taskPtr->method == RequestMethod::updateFirmware) {

        if (!openUploadFileStream(taskPtr)) {
            closeInternal();
            beast::error_code ec(http::error::bad_method);
            fail(ec, taskPtr);
            return;
        }
    }

    // already open
    if (m_httpIO->m_httpStream.is_open()) {
        SPDLOG_DEBUG("startRequest()::asyncWrite() :: uri={}, is_open={}",
                     taskPtr->uri, m_httpIO->m_httpStream.is_open());
        asyncWrite(taskPtr, true);
    }
    // try to connect new endpoint
    else {
        SPDLOG_DEBUG("startRequest()::asyncResolve() :: uri={}, is_open={}",
                     taskPtr->uri, m_httpIO->m_httpStream.is_open());
        asyncResolve(taskPtr);
    }
}

void HttpSession::HttpMonitorRequest(const bool needJsonResponse, const bool supportTimeStamp,
                                     const HttpReponseHandler& eventHandler, const int timeoutSec)
{
    auto taskPtr = std::make_shared<HttpRequestTask>();
    taskPtr->method = RequestMethod::monitor;
    taskPtr->uri = kMonitorDiffuri;
    taskPtr->handler = eventHandler;
    taskPtr->timeoutSec = timeoutSec;
    taskPtr->lastStatus = http::status::ok;
    taskPtr->needJsonResponse = needJsonResponse;

    // timestamp support
    if (supportTimeStamp)
        taskPtr->uri += u8"&IncludeTimestamp=True";
    // schema based support
    if (needJsonResponse)
        taskPtr->uri += u8"&SchemaBased=True";

    asio::post(m_strand, WeakCallback(shared_from_this(), [this, taskPtr]()
    {
        // try to connect new endpoint
        closeInternal();
        asyncResolve(taskPtr);

    }));
}

void HttpSession::HttpPosMonitorRequest(const HttpReponseHandler& eventHandler, const int timeoutSec)
{
    auto taskPtr = std::make_shared<HttpRequestTask>();
    taskPtr->method = RequestMethod::monitor;
    taskPtr->uri = kPosMonitorDiffuri;
    taskPtr->handler = eventHandler;
    taskPtr->timeoutSec = timeoutSec;
    taskPtr->lastStatus = http::status::ok;
    taskPtr->needJsonResponse = false;

    asio::post(m_strand, WeakCallback(shared_from_this(), [this, taskPtr]()
    {
        // try to connect new endpoint
        closeInternal();
        asyncResolve(taskPtr);
    }));
}


void HttpSession::fail(beast::error_code ec,
                       const HttpRequestTaskSharedPtr& taskPtr,
                       unsigned int httpStatus)
{
    bool socketClosed = HttpSocketWasClosed(ec);

    SPDLOG_DEBUG("Failed a HTTP request :: uri={}, ec={}, status={}, socketClosed={}/{}",
                 taskPtr->uri, ec.message(), httpStatus,
                 socketClosed, (ec == http::error::end_of_stream));

    if (socketClosed) {
        closeInternal();
        SPDLOG_DEBUG("HTTP error::end_of_stream, check retry count={}", taskPtr->retryCnt);
        if (taskPtr->retryCnt == 0) {
            taskPtr->retryCnt++;
            startRequest(taskPtr);
            return;
        }
    }

    if (isPostMethod(taskPtr->method)) {
        closeInternal();
    }

    m_queue.Done();

    auto response = std::make_shared<HttpResponse>();

    response->uri = taskPtr->uri;
    response->failed = (ec.failed()) || (httpStatus != 200);
    response->error = ec;
    response->httpStatus = httpStatus;
    response->isContinue = false;
    if ((http::int_to_status(httpStatus) == http::status::moved_permanently)
        || (http::int_to_status(httpStatus) == http::status::found)) {
        response->movedLocation = m_httpIO->m_response[http::field::location].to_string();
    }

    taskPtr->handler(response);
}

void HttpSession::success(const HttpRequestTaskSharedPtr& taskPtr,
                          const std::string& contentType,
                          const std::string& responseBody,
                          const bool isContinue)
{
    SPDLOG_DEBUG("HTTP request returned OK :: contentType={}, uri={}",
                 contentType, taskPtr->uri);

    auto response = std::make_shared<HttpResponse>();

    response->uri = taskPtr->uri;
    response->failed = false;
    response->error = beast::error_code();
    response->httpStatus = 200;
    response->contentType = contentType;
    response->isContinue = isContinue;

    // save data into a file
    if (taskPtr->method == RequestMethod::download || taskPtr->method == RequestMethod::updateConfigBackupPassword) {
        if (contentType.find("octet-stream") != std::string::npos) {
            std::ofstream outFile(taskPtr->filePath, std::ios::out | std::ios::binary);
            if (outFile.fail())
            {
                SPDLOG_WARN("Can not open file to write, path={}", taskPtr->filePath);
                response->failed = true;
                response->error = beast::error_code(http::error::bad_method);
            }
            else
            {
                outFile.write(responseBody.data(), responseBody.size());
                outFile.close();
            }
        }
        else{
            SPDLOG_INFO("HttpSession::success RequestMethod::download={}", response->httpBody);
            response->httpBody = responseBody;
        }
    }
    else {
        response->httpBody = responseBody;
        SPDLOG_TRACE("HTTP response={}{}", SPDLOG_EOL, response->httpBody);
    }

    if (!isContinue && isPostMethod(taskPtr->method)) {
        closeInternal();
    }

    m_queue.Done();
    taskPtr->handler(response);
}

void HttpSession::asyncResolve(const HttpRequestTaskSharedPtr& taskPtr)
{
    SPDLOG_TRACE("Start asyncResolve()");
    m_resolver.async_resolve(m_host, m_port,
                             WeakCallback(shared_from_this(), [this, taskPtr]
                                          (beast::error_code ec, tcp::resolver::results_type results)
    {
        if (ec) {
            SPDLOG_DEBUG("Failed async_resolve() :: uri={}, ec={}",
                         taskPtr->uri, ec.message());
            fail(ec, taskPtr);
            return;
        }

        m_resolverResults = results;
        asyncConnect(taskPtr);
    }));
}

void HttpSession::asyncConnect(const HttpRequestTaskSharedPtr& taskPtr)
{
    //DeviceCertificate::GetInstance()->CheckRootCA();
    m_httpIO->m_httpStream.reset_connection(m_isSSL);

    // Set a timeout on the operation
    m_httpIO->m_httpStream.expires_after(m_connectionTimeoutSec);

    // get tcp_stream
    auto& tcp_stream = m_httpIO->m_httpStream.tcp_stream();

    // async connect to the http server
    auto hio = m_httpIO;
    tcp_stream.async_connect(m_resolverResults,
                             WeakCallback(shared_from_this(), [this, hio, taskPtr]
                                          (beast::error_code ec, tcp::resolver::results_type::endpoint_type)
    {
        if (ec) {
            SPDLOG_DEBUG("Failed async_connect() :: uri={}, ec={}", taskPtr->uri, ec.message());
            fail(ec, taskPtr);
            return;
        }

        // no ssl mode (default)
        if (!m_httpIO->m_httpStream.is_ssl()) {
            asyncWrite(taskPtr, false);
            return;
        }

        // ssl handshake
        SPDLOG_DEBUG("asyncConnect() :: Handshaking for SSL");
        m_httpIO->m_httpStream.set_option(tcp::no_delay(true));
        auto& ssl_stream = m_httpIO->m_httpStream.ssl_stream();
        auto hio = m_httpIO;
        ssl_stream.async_handshake(ssl::stream_base::client,
                                   WeakCallback(shared_from_this(), [this, hio, taskPtr]
                                                (beast::error_code ec2)
        {
            if (ec2) {
                SPDLOG_DEBUG("Failed async_handshake() :: uri={}, ec={}",
                             taskPtr->uri, ec2.message());
                fail(ec2, taskPtr);
                return;
            }

            if(m_isCheckDeviceCert){
                auto& ssl_stream = m_httpIO->m_httpStream.ssl_stream();
                X509* cert = SSL_get_peer_certificate(ssl_stream.native_handle());

                if (cert != NULL) {
                    m_deviceCertResult = DeviceCertificate::getInstance().
                            CheckDeviceCertificate(cert, m_deviceCertMac);
                    char subjectName[256];
                    X509_NAME_oneline(X509_get_subject_name(cert), subjectName, 256);
                    SPDLOG_DEBUG("HTTPS CERT X509 NAME::{}", subjectName);
                }
                m_isCheckDeviceCert = false;
            }

            //기기인증서 체크하고, 기기인증서 맥주소 저장.
            asyncWrite(taskPtr, false);
        }));

    }));
}

void HttpSession::asyncWrite(const HttpRequestTaskSharedPtr& taskPtr, bool retryConnect)
{
    m_httpIO->m_request = {};
    m_httpIO->m_request.version(11);
    if (isPostMethod(taskPtr->method))
        m_httpIO->m_request.method(http::verb::post);
    else
        m_httpIO->m_request.method(http::verb::get);
    m_httpIO->m_request.keep_alive(true);

    m_httpIO->m_request.set(http::field::host, m_host);
    m_httpIO->m_request.set(http::field::user_agent, kAgentName);
    m_httpIO->m_request.set(http::field::connection, "keep-alive");

    if(VersionManager::Instance()->s1Support() && !m_mac.empty()) {
        SPDLOG_DEBUG("[HttpSession] asyncWrite : s1Support set mac address in the request = {}", m_mac);
        m_httpIO->m_request.set("NVR-Identify-ID ", m_mac);
    }

    if(taskPtr->needJsonResponse) {
        m_httpIO->m_request.set(http::field::accept, "application/json");
    }

    m_httpIO->m_request.target(taskPtr->uri);

    bool hasAuth = m_digestAuthenticator.hasAuthorization();

    // do not contain post data until auth
    if (!hasAuth && m_httpIO->m_request.method() == http::verb::post)
         m_httpIO->m_request.content_length(0);

    if (hasAuth) {
        m_digestAuthenticator.UpdateAuthorization(m_httpIO->m_request.method_string().to_string(),
                                                  m_httpIO->m_request.target().to_string());
        m_httpIO->m_request.set(http::field::authorization, m_digestAuthenticator.authorization());

        // contain post data with auth
        if (m_httpIO->m_request.method() == http::verb::post) {
            if (taskPtr->method == RequestMethod::encodedUpload ||
                taskPtr->method == RequestMethod::encodedData ||
                  taskPtr->method == RequestMethod::updateConfigBackupPassword ||
                   taskPtr->method == RequestMethod::updateConfigRestorePassword) {

                std::string b64Data;
                if (taskPtr->method == RequestMethod::encodedUpload  || taskPtr->method == RequestMethod::updateConfigRestorePassword) {
                    std::string buff(taskPtr->uploadFileSize, '\0');
                     auto &is = taskPtr->uploadFileStream;
                    is.read(&buff[0], taskPtr->uploadFileSize);
                    b64Data = Wisenet::Common::Base64Encode(buff);
                }
                else {
                    b64Data = Wisenet::Common::Base64Encode(taskPtr->postData);
                }


                if(taskPtr->method == RequestMethod::updateConfigRestorePassword){
                    std::string tempData = b64Data;
                    b64Data = "<DecryptionKey>" +
                            Wisenet::Common::Base64Encode(taskPtr->postData) +
                            "<DecryptionKey>" +
                            "<ConfigFile>" +
                            tempData +
                            "</ConfigFile>";
                }

                std::string urlEncodedBinary = Wisenet::Common::UrlEncode(b64Data);
                m_httpIO->m_request.content_length(urlEncodedBinary.size());
                m_httpIO->m_request.set(http::field::content_type, "application/x-www-form-urlencoded; charset=utf-8");
                m_httpIO->m_request.body() = urlEncodedBinary;

                SPDLOG_DEBUG("Read encodedUpload ContentLength={}", urlEncodedBinary.size());

            }
            else if (taskPtr->method == RequestMethod::updateFirmware) {
                asyncFileUplaodHeader(taskPtr);
                return;
            }
        }
    }

    // set write timeout
    m_httpIO->m_httpStream.expires_after(taskPtr->timeoutSec);
    auto hio = m_httpIO;
    // Send the HTTP request to the remote host
    http_async_write(m_httpIO->m_httpStream,
                     m_httpIO->m_request,
                     WeakCallback(shared_from_this(), [this, hio, taskPtr, retryConnect]
                                  (beast::error_code ec, std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);
        if (ec) {
            bool isClosed = !m_httpIO->m_httpStream.is_open() || HttpSocketWasClosed(ec);

            SPDLOG_DEBUG("http_async_write() failed :: uri={}, ec={}, open={}, wasClosed={}, ovClosed={}",
                         taskPtr->uri, ec.message(), m_httpIO->m_httpStream.is_open(),
                         HttpSocketWasClosed(ec), isClosed);
            if (isClosed && retryConnect) {
                asyncConnect(taskPtr);
                return;
            }
            fail(ec, taskPtr);
            return;
        }

        // asyncReadHeader can be used for general HTTP-GET, but split logic for easy read and performance..
        if (taskPtr->method == RequestMethod::monitor || taskPtr->method == RequestMethod::bigGet)
            asyncReadHeader(taskPtr);
        else
            asyncRead(taskPtr);
    }));
}


void HttpSession::asyncFileUplaodHeader(const HttpRequestTaskSharedPtr &taskPtr)
{
    auto fileSize = taskPtr->uploadFileSize;
    auto fileName = fileNameFromPath(taskPtr->filePath);

    SPDLOG_DEBUG("asyncFileUplaod() Start, path={}, size={}, name={}",
                 taskPtr->filePath, fileSize, fileName);

    m_httpIO->m_bufferedRequest = {};
    auto sr = std::make_shared<http::request_serializer<http::buffer_body>>(m_httpIO->m_bufferedRequest);

    m_httpIO->m_bufferedRequest.version(11);
    m_httpIO->m_bufferedRequest.method(http::verb::post);
    m_httpIO->m_bufferedRequest.keep_alive(true);
    m_httpIO->m_bufferedRequest.set(http::field::host, m_host);
    m_httpIO->m_bufferedRequest.set(http::field::user_agent, kAgentName);
    m_httpIO->m_bufferedRequest.set(http::field::connection, "keep-alive");
    m_httpIO->m_bufferedRequest.target(taskPtr->uri);
    m_httpIO->m_bufferedRequest.set(http::field::authorization, m_digestAuthenticator.authorization());
    m_httpIO->m_bufferedRequest.set(http::field::content_type, "multipart/form-data; boundary=" + kPostMultipartBoundary);

    if(VersionManager::Instance()->s1Support()) {
        SPDLOG_DEBUG("[HttpSession] asyncFileUplaodHeader : s1Support set mac address in the request = {}", m_mac);
        m_httpIO->m_request.set("NVR-Identify-ID ", m_mac);
    }

    // content-length for the overall transmission.
    // Even if it were not to do so, HTTP clients are
    // encouraged to supply content-length for overall file input so that a
    // busy server could detect.
    auto contentHeader = getMultipartContentHeader(taskPtr);
    m_httpIO->m_bufferedRequest.content_length(fileSize + contentHeader.length() + kPostMultipartLastLine.length());
    m_httpIO->m_httpStream.expires_after(taskPtr->timeoutSec);
    auto hio = m_httpIO;
    http_async_writeBuffer_header(m_httpIO->m_httpStream,
                                  *sr,
                                  WeakCallback(shared_from_this(), [this, hio, taskPtr, sr]
                                               (beast::error_code ec, std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);
        if (ec) {
            SPDLOG_ERROR("http_async_write_header() failed :: uri={}, ec={}",
                         taskPtr->uri, ec.message());
            fail(ec, taskPtr);
            return;
        }
        SPDLOG_DEBUG("http_async_write_header() success, byteTransferred={}", bytes_transferred);
        std::string contentHeader = getMultipartContentHeader(taskPtr);


        m_httpIO->m_bufferedData.clear();
        m_httpIO->m_bufferedData.reserve(contentHeader.length());
        std::copy(contentHeader.begin(), contentHeader.end(), std::back_inserter(m_httpIO->m_bufferedData));

        m_httpIO->m_bufferedRequest.body().data = (void*)&(m_httpIO->m_bufferedData[0]);
        m_httpIO->m_bufferedRequest.body().size = m_httpIO->m_bufferedData.size();
        m_httpIO->m_bufferedRequest.body().more = true;
        m_httpIO->m_httpStream.expires_after(taskPtr->timeoutSec);
        auto hio = m_httpIO;
        http_async_writeBuffer(m_httpIO->m_httpStream,
                                *sr,
                                WeakCallback(shared_from_this(), [this, hio, taskPtr, sr]
                                             (beast::error_code ec, std::size_t bytes_transferred)
        {
            boost::ignore_unused(bytes_transferred);
            // This error is returned by body_buffer during
            // serialization when it is done sending the data
            // provided and needs another buffer.
            if(ec == http::error::need_buffer) {
                ec = {};
            }

            if (ec) {
                SPDLOG_ERROR("http_async_write()-content header failed :: uri={}, ec={}",
                             taskPtr->uri, ec.message());
                fail(ec, taskPtr);
                return;
            }
            SPDLOG_DEBUG("http_async_write()-content header success, byteTransferred={}, done={}", bytes_transferred, sr->is_done());
            asyncFileUplaodContent(taskPtr, sr);
        }));
    }));


}

void HttpSession::asyncFileUplaodContent(const HttpRequestTaskSharedPtr &taskPtr,
                                         const std::shared_ptr<http::request_serializer<http::buffer_body>>& sr)
{
    auto &is = taskPtr->uploadFileStream;
    const size_t buffSize = 512000; // 500K
  //  SPDLOG_DEBUG("Continue of asyncFileUplaodContent() :: uri={} -- {}", taskPtr->uri, taskPtr->timeoutSec);

    m_httpIO->m_bufferedData.resize(buffSize, 0);
    is.read(&(m_httpIO->m_bufferedData[0]), buffSize);
    if (!is && !is.eof()) {
        SPDLOG_WARN("asyncFileUplaodContent() read stream failed, path={}, gc_count={}, eof={}",
                    taskPtr->filePath, is.gcount(), is.eof());
        beast::error_code ec(http::error::bad_method);
        fail(ec, taskPtr);
        return;
    }

    auto eof = is.eof();
    auto readBytes = is.gcount();
    size_t totalReadBytes = eof ? (size_t)taskPtr->uploadFileSize : (size_t)is.tellg();

    double percent = totalReadBytes / (double)taskPtr->uploadFileSize * 100;
    SPDLOG_DEBUG("asyncFileUplaodContent() sending fileUpload sendBytes={}, {}/{}",
                 readBytes, totalReadBytes, taskPtr->uploadFileSize);

    if (eof) {
        m_httpIO->m_bufferedData.resize(readBytes);
        std::copy(kPostMultipartLastLine.begin(), kPostMultipartLastLine.end(),
                  std::back_inserter(m_httpIO->m_bufferedData));

        readBytes += kPostMultipartLastLine.length();

        SPDLOG_DEBUG("asyncFileUplaodContent() appending lastLine={}", readBytes);
    }

    m_httpIO->m_bufferedRequest.body().data = (void*)&(m_httpIO->m_bufferedData[0]);
    m_httpIO->m_bufferedRequest.body().size = readBytes;
    m_httpIO->m_bufferedRequest.body().more = eof ? false : true;

    m_httpIO->m_httpStream.expires_after(taskPtr->timeoutSec);
    auto hio = m_httpIO;
    http_async_writeBuffer(m_httpIO->m_httpStream,
                     *sr,
                     WeakCallback(shared_from_this(), [this, taskPtr, sr, eof, hio, percent]
                                  (beast::error_code ec, std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);
        if(ec == http::error::need_buffer) {
            ec = {};
        }

        if (ec) {
            SPDLOG_ERROR("asyncFileUplaodContent() failed :: uri={}, ec={} timeout={}",
                         taskPtr->uri, ec.message(),  taskPtr->timeoutSec);
            fail(ec, taskPtr);
            return;
        }

        SPDLOG_DEBUG("asyncFileUplaodContent() success, byteTransferred={}, done={}",
                     bytes_transferred, sr->is_done());
        std::stringstream msg;
        msg << "Status=Uploading\r\n" << "Progress=" << (int)percent << "\r\n";

        if (eof) {
            SPDLOG_DEBUG("asyncFileUplaodContent() eof, start to wait response from server, path={}",
                         taskPtr->filePath);

            // callback progress status
            success(taskPtr, "text/plain", msg.str(), true);

            // read from server response
            asyncReadHeader(taskPtr);
            return;
        }

        // callback progress status
        success(taskPtr, "text/plain", msg.str(), true);

        // read next data and sending
        asyncFileUplaodContent(taskPtr, sr);
    }));
}

std::string HttpSession::getMultipartContentHeader(const HttpRequestTaskSharedPtr &taskPtr)
{
    auto fileName = fileNameFromPath(taskPtr->filePath);

    std::stringstream contentHeader;
    contentHeader << "--" << kPostMultipartBoundary << "\r\n";
    contentHeader << "Content-Disposition: form-data; name=\"UploadedFile\"; filename=\"" << fileName << "\"" << "\r\n";
    contentHeader << "Content-Type: application/octet-stream\r\n\r\n";
    return contentHeader.str();
}

void HttpSession::asyncRead(const HttpRequestTaskSharedPtr& taskPtr)
{
    m_httpIO->m_response = {}; // clear response

    auto hio = m_httpIO;
    // Receive the HTTP response
    http_async_read(m_httpIO->m_httpStream, m_httpIO->m_buffer, m_httpIO->m_response,
                    WeakCallback(shared_from_this(), [this, hio, taskPtr]
                                 (beast::error_code ec, std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);
        if (ec) {
            SPDLOG_DEBUG("Failed async_read() :: uri={}, ec={}", taskPtr->uri, ec.message());
            fail(ec, taskPtr);
            return;
        }

        if (m_httpIO->m_response.result() != http::status::ok) {
            if (m_httpIO->m_response.result() == http::status::unauthorized &&
                    taskPtr->lastStatus == http::status::ok) {
                // retry once again with authenticate
                taskPtr->lastStatus = m_httpIO->m_response.result();
                auto wwwAuthenticate = m_httpIO->m_response[http::field::www_authenticate].to_string();

                // just for debugging, do not enable this for release
#if 0
                SPDLOG_DEBUG("asyncRead():: try to Generate Authorization, auth={}, m_user={}, m_password={}",
                             wwwAuthenticate, m_user, m_password);
#endif
                if (m_digestAuthenticator.GenerateAuthorization(wwwAuthenticate, m_user, m_password)) {
                    SPDLOG_DEBUG("asyncRead():: Generate Authorization");
                    if (m_httpIO->m_response.keep_alive())
                        asyncWrite(taskPtr, false);
                    else
                        asyncConnect(taskPtr);
                    return;
                }
            }

            fail(ec, taskPtr, m_httpIO->m_response.result_int());
            return;
        }
        success(taskPtr, m_httpIO->m_response[http::field::content_type].to_string(), m_httpIO->m_response.body());
    }));
}




void HttpSession::asyncReadHeader(const HttpRequestTaskSharedPtr& taskPtr)
{
    SPDLOG_TRACE("Start of asyncReadHeader() :: uri={} timeout={}", taskPtr->uri, taskPtr->timeoutSec);

    m_httpIO->m_httpStream.expires_after(taskPtr->timeoutSec);

    // construct a new parser for each message
    m_httpIO->m_headParser.emplace();
    m_httpIO->m_headParser->body_limit(boost::none);
    auto hio = m_httpIO;
    http_async_read_header(m_httpIO->m_httpStream, m_httpIO->m_buffer, m_httpIO->m_headParser.get(),
                           WeakCallback(shared_from_this(), [this, hio, taskPtr]
                                        (beast::error_code ec, std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);
        if (ec) {
            SPDLOG_DEBUG("Failed asyncReadHeader() :: uri={}, ec={}", taskPtr->uri, ec.message());
            fail(ec, taskPtr);
            return;
        }

        std::string contentType = m_httpIO->m_headParser.get().get()[http::field::content_type].to_string();
        SPDLOG_TRACE("Read asyncReadHeader :: ContentType={}, byteTransferred={}", contentType, bytes_transferred);

        if (boost::starts_with(contentType, "multipart/")) {
            // find boundary
            const auto bstart = contentType.find("boundary=");
            if (bstart == std::string::npos) {
                SPDLOG_WARN("Can't find boundary ; uri={}, ContentType={}", taskPtr->uri, contentType);
                beast::error_code ec2(http::error::bad_transfer_encoding);
                fail(ec2, taskPtr);
                return;
            }

            auto boundaryStr = contentType.substr(bstart + 9);
            boost::trim(boundaryStr);
            m_multipartBoundary = "--"+boundaryStr;

            SPDLOG_DEBUG("Multipart boundary={}", m_multipartBoundary);

            m_httpIO->m_responseParser.emplace(std::move(m_httpIO->m_headParser.get()));
            m_lastEventContentType = "text/plain";
            asyncReadMultipart(taskPtr);
        }
        else {
            asyncReadAll(taskPtr);
        }
    }));
}


void HttpSession::asyncReadAll(const HttpRequestTaskSharedPtr& taskPtr)
{
    SPDLOG_TRACE("Start of asyncReadAll() :: uri={} timeout={}", taskPtr->uri, taskPtr->timeoutSec);

    m_httpIO->m_responseParser.emplace(std::move(m_httpIO->m_headParser.get()));
    m_httpIO->m_httpStream.expires_after(taskPtr->timeoutSec);

    // Receive the HTTP response all
    auto hio = m_httpIO;
    http_async_read(m_httpIO->m_httpStream, m_httpIO->m_buffer, m_httpIO->m_responseParser.get(),
                    WeakCallback(shared_from_this(), [this, hio, taskPtr]
                                 (beast::error_code ec, std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        const auto& res = m_httpIO->m_responseParser.get().get();
        if (ec) {
            SPDLOG_DEBUG("Failed asyncReadAll() :: uri={}, ec={}", taskPtr->uri, ec.message());
            fail(ec, taskPtr);
            return;
        }

        SPDLOG_TRACE("asyncReadAll() result :: uri={}, done={}, byte_transfered={} :: RES DATA={}{}",
                     taskPtr->uri, m_httpIO->m_responseParser.get().is_done(), bytes_transferred, SPDLOG_EOL, res.body());

        if (res.result() != http::status::ok) {
            if (res.result() == http::status::unauthorized && taskPtr->lastStatus == http::status::ok) {

                SPDLOG_TRACE("Try to login in async_read_some() :: uri={}", taskPtr->uri);

                // retry once again with authenticate
                taskPtr->lastStatus = res.result();
                auto wwwAuthenticate = res[http::field::www_authenticate].to_string();

                if (m_digestAuthenticator.GenerateAuthorization(wwwAuthenticate, m_user, m_password)) {
                    if (res.keep_alive())
                        asyncWrite(taskPtr, false);
                    else
                        asyncConnect(taskPtr);
                    return;
                }
            }

            fail(ec, taskPtr, res.result_int());
            return;
        }
        success(taskPtr, res[http::field::content_type].to_string(), res.body());
    }));
}

void HttpSession::asyncReadMultipart(const HttpRequestTaskSharedPtr& taskPtr)
{
    SPDLOG_TRACE("Continue of asyncReadMultipart() :: uri={} -- {}", taskPtr->uri, taskPtr->timeoutSec);

    if (taskPtr->method == RequestMethod::monitor) {
        // do not expire timeout if monitor action.
        m_httpIO->m_httpStream.expires_never();
    }
    else {
        m_httpIO->m_httpStream.expires_after(taskPtr->timeoutSec);
    }


    m_httpIO->m_responseParser.get().get().body() = {};
    // Receive the HTTP response some..
    auto hio = m_httpIO;
    http_async_read_some(m_httpIO->m_httpStream, m_httpIO->m_buffer, m_httpIO->m_responseParser.get(),
                         WeakCallback(shared_from_this(), [this, hio, taskPtr]
                                      (beast::error_code ec, std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);
        if (ec) {
            SPDLOG_DEBUG("Failed asyncReadMultipart() :: ec={}", ec.message());
            fail(ec, taskPtr);
            return;
        }

        const auto& res = m_httpIO->m_responseParser.get().get();
        m_eventBuffer.append(res.body());
        std::string result;
        //SPDLOG_DEBUG("eventBuffer={}", m_eventBuffer);
        if (parseMultipartData(result)) {
            SPDLOG_TRACE("asyncReadMultipart() result :: done={}, byte_transfered={}, RES={}{}",
                         m_httpIO->m_responseParser.get().is_done(),
                         bytes_transferred, SPDLOG_EOL, res.body());
            success(taskPtr, m_lastEventContentType, result, true);
        }
        else {
            SPDLOG_TRACE("asyncReadMultipart() -continue- :: done={}, byte_transfered={}, RES={}{}",
                         m_httpIO->m_responseParser.get().is_done(),
                         bytes_transferred, SPDLOG_EOL, res.body());
        }

        if (m_httpIO->m_responseParser.get().is_done()) {
            // completed http response
            SPDLOG_DEBUG("asyncReadMultipart() -END-");
            success(taskPtr, "text/plain", "Status=EndResponse\r\n");
            return;
        }

        // Go to read next body
        asyncReadMultipart(taskPtr);
    }));
}


bool HttpSession::parseMultipartData(std::string &result)
{
    size_t next = 0;
    std::string line;

    auto length = m_eventBuffer.length();
    if (length < 4)
        return false;

    auto ptr = m_eventBuffer.c_str();
    if (ptr[length-4] != '\r' &&
            ptr[length-3] != '\n' &&
            ptr[length-2] != '\r' &&
            ptr[length-1] != '\n') {
        SPDLOG_TRACE("Wait more event data..");
        return false;
    }

    while (1) {
        next = m_eventBuffer.find("\r\n", 0);

        if (next == std::string::npos) {
            break;
        }
        else if (next == 0) {
            m_eventBuffer.erase(0, next+2);
            continue;
        }

        line = m_eventBuffer.substr(0, next);
        if (boost::starts_with(line, "Content-Type:")) {
            m_lastEventContentType = line.substr(13);
            boost::trim(m_lastEventContentType);
            m_eventBuffer.erase(0, next+2);
            continue;
        }
        else if (boost::starts_with(line, m_multipartBoundary)) {
            m_eventBuffer.erase(0, next+2);
            continue;
        }
        result.append(m_eventBuffer, 0, next+2);
        m_eventBuffer.erase(0, next+2);
    }

    if (result.empty())
        return false;

    return true;
}



/////// nested RequestQueue Class /////////////////////////////////////////////
void HttpSession::RequestQueue::Request(const HttpRequestTaskSharedPtr &request)
{
    bool doStart = (m_requestTasks.size() == 0);
    m_requestTasks.push(request);
    SPDLOG_DEBUG("HTTP Request Start:: queue count = {}", m_requestTasks.size());
    if (doStart) {
        m_self.startRequest(request);
    }
}

void HttpSession::RequestQueue::Done()
{
    if (m_requestTasks.size() > 0) {
        m_requestTasks.pop();
        SPDLOG_DEBUG("HTTP Request Done:: queue count = {}", m_requestTasks.size());
        if (m_requestTasks.size() > 0) {
            auto& request = m_requestTasks.front();
            m_self.startRequest(request);
        }
    }
}

void HttpSession::RequestQueue::CancelAll()
{
    if (m_requestTasks.size() > 0) {
        SPDLOG_DEBUG("Clear all request queue, count={}", m_requestTasks.size());
        std::queue<HttpRequestTaskSharedPtr> empty;
        std::swap(m_requestTasks, empty);
    }
}
