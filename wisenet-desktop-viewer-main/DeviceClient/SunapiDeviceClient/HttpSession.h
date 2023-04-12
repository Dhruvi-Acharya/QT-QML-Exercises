#pragma once

#include <memory>
#include <functional>
#include <queue>
#include <fstream>
#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio/strand.hpp>
#include <boost/optional.hpp>
#include "HttpStream.h"
#include "DigestAuthenticator.h"

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;
using tcp = asio::ip::tcp;
namespace ssl = asio::ssl;


struct HttpResponse
{
    std::string uri;
    bool failed = false;
    beast::error_code error;
    unsigned int httpStatus = 200;
    std::string contentType;
    std::string httpBody;
    std::string movedLocation;
    bool isContinue = false;
};

// BEAST I/O API의 REQUEST, RESPONSE 인자로 들어가는 변수들은
// shared_ptr로 보호한다.
// I/O 서비스 도중 HttpSession인스턴스가 삭제되는 경우 대비
struct HttpIoStruct
{
    explicit HttpIoStruct(asio::strand<boost::asio::io_context::executor_type>& strand)
        : m_httpStream(strand)
    {

    }
    HttpStream m_httpStream;

    http::request<http::string_body> m_request;
    http::request<http::buffer_body> m_bufferedRequest;
    std::vector<char>                m_bufferedData;

    // simple http-get
    beast::flat_buffer m_buffer;
    http::response<http::string_body> m_response;

    boost::optional<http::response_parser<http::empty_body>>    m_headParser;
    boost::optional<http::response_parser<http::string_body>>   m_responseParser;
};

typedef std::shared_ptr<HttpResponse> HttpResponseSharedPtr;
typedef std::function <void(const HttpResponseSharedPtr&)> HttpReponseHandler;
typedef std::shared_ptr<HttpIoStruct> HttpIoStructSharedPtr;

enum class RequestMethod
{
    get = 0,        // general HTTP Get
    monitor,        // event monitor_diff
    download,       // general HTTP Get + save binary data in a file
    updateFirmware, // firmware update POST and multipart response
    encodedUpload,  // url encoded POST (file upload)
    encodedData,    // url encoded POST (raw data)
    bigGet,
    updateConfigBackupPassword,    // url encoded POST + save binary data in a file (Config backup + password)
    updateConfigRestorePassword,
};

struct HttpRequestTask
{
    RequestMethod       method = RequestMethod::get;
    http::status        lastStatus = http::status::unknown;
    std::string         uri;
    int                 timeoutSec = 0;
    int                 retryCnt = 0;
    bool                needJsonResponse = false;
    HttpReponseHandler  handler = nullptr;
    std::string         filePath;
    std::ifstream       uploadFileStream;
    size_t              uploadFileSize = 0;
    std::string         postData;
};
typedef std::shared_ptr<HttpRequestTask> HttpRequestTaskSharedPtr;


class HttpSession : public std::enable_shared_from_this<HttpSession>
{
    class RequestQueue
    {
    public:
        explicit RequestQueue(HttpSession& self)
            : m_self(self), m_logPrefix(self.m_logPrefix)
        {

        }

        void Request(const HttpRequestTaskSharedPtr& request);
        void Done();
        void CancelAll();
    private:
        HttpSession& m_self;
        std::string &m_logPrefix;
        std::queue<HttpRequestTaskSharedPtr> m_requestTasks;
    };

public:
    explicit HttpSession(asio::io_context& ioc);
    ~HttpSession();

    void SetConnection(const std::string& host, const unsigned short port,
                       const std::string& user, const std::string& password,
                       const std::string& mac,
                       const bool& isSSL = false,
                       const int connectionTimeoutSec = 10);

    // HTTP GET (text/plain, text/xml, application/json)
    void HttpGetRequest(const std::string& uri, bool needJsonResponse, const HttpReponseHandler& responseHandler,
                        const int timeoutSec = 10, bool bigGet=false);

    // HTTP GET FOR MONITOR DIFF (Continous event streaming)
    void HttpMonitorRequest(const bool needJsonResponse, const bool supportTimeStamp,
                            const HttpReponseHandler& eventHandler, const int timeoutSec = 10);

    // HTTP GET FOR POS MONITOR DIFF (Continous pos data streaming)
    void HttpPosMonitorRequest(const HttpReponseHandler& eventHandler, const int timeoutSec = 10);

    // HTTP GET (binary download such as config import)
    void HttpGetDownload(const std::string& uri,
                         const std::string &downloadFilePath,
                         const HttpReponseHandler& responseHandler,
                         const int timeoutSec = 60);

    // HTTP POST (binary download such as config import)
    void HttpPostDownload(const std::string& uri,
                          const std::string &rawData,
                         const std::string &downloadFilePath,
                         const HttpReponseHandler& responseHandler,
                         const int timeoutSec = 60);


    // HTTP POST upload file (url-encoded upload such as config export)
    void HttpPostEncodedUpload(const std::string& uri,
                        const std::string &uploadFilePath,
                        const HttpReponseHandler& responseHandler,
                        const int timeoutSec = 60);

    // HTTP POST upload file (url-encoded upload such as config export and password)
    void HttpPostEncodedUploadPassword(const std::string& uri,
                        const std::string &rawData,
                        const std::string &uploadFilePath,
                        const HttpReponseHandler& responseHandler,
                        const int timeoutSec = 60);

    // HTTP POST upload firmware file
    void HttpPostUpdateFirmware(const std::string& uri,
                                const std::string &uploadFilePath,
                                const HttpReponseHandler& responseHandler,
                                const int timeoutSec = 60);

    // HTTP POST data (convert into application/x-www-form-urlencoded)
    void HttpPostEncodedData(const std::string& uri,
                        const std::string &rawData,
                        const HttpReponseHandler& responseHandler,
                        const int timeoutSec = 10);

    void Close();

    void SetCheckDeviceCert(bool check);

    std::string GetDeviceCertMac() { return m_deviceCertMac;}
    std::string GetDeviceCertResult() { return m_deviceCertResult; }

private:
    void startRequest(const HttpRequestTaskSharedPtr& taskPtr);

    void asyncResolve(const HttpRequestTaskSharedPtr& taskPtr);
    void asyncConnect(const HttpRequestTaskSharedPtr& taskPtr);
    void asyncWrite(const HttpRequestTaskSharedPtr& taskPtr, bool retryConnect);
    void asyncFileUplaodHeader(const HttpRequestTaskSharedPtr& taskPtr);
    void asyncFileUplaodContent(const HttpRequestTaskSharedPtr& taskPtr,
                                const std::shared_ptr<http::request_serializer<http::buffer_body>>& sr);
    std::string getMultipartContentHeader(const HttpRequestTaskSharedPtr& taskPtr);

    void asyncRead(const HttpRequestTaskSharedPtr& taskPtr);

    // for event streaming
    void asyncReadHeader(const HttpRequestTaskSharedPtr& taskPtr);
    void asyncReadAll(const HttpRequestTaskSharedPtr& taskPtr);
    void asyncReadMultipart(const HttpRequestTaskSharedPtr& taskPtr);

    void fail(beast::error_code ec,
              const HttpRequestTaskSharedPtr& taskPtr,
              unsigned int httpStatus = 200);
    void success(const HttpRequestTaskSharedPtr& taskPtr,
                 const std::string& contentType,
                 const std::string& responseBody,
                 const bool isContinue = false);

    void closeInternal(const bool cancelAll = false);
    bool parseMultipartData(std::string &result);

    bool openUploadFileStream(const HttpRequestTaskSharedPtr& taskPtr);
private:
    asio::io_context& m_ioContext;
    asio::strand<boost::asio::io_context::executor_type> m_strand;
    tcp::resolver m_resolver;
    tcp::resolver::results_type m_resolverResults;

    std::string m_host;
    std::string m_port;
    std::string m_user;
    std::string m_password;
    int         m_connectionTimeoutSec;
    bool        m_isSSL = false;
    std::string m_deviceCertMac = "";
    std::string m_deviceCertResult = "";
    bool        m_isCheckDeviceCert = false;
    std::string m_mac;

    DigestAuthenticator m_digestAuthenticator;
    RequestQueue m_queue;

    std::string m_eventBuffer;
    std::string m_multipartBoundary;
    std::string m_lastEventContentType;

    std::string m_logPrefix;
    HttpIoStructSharedPtr m_httpIO;
};

