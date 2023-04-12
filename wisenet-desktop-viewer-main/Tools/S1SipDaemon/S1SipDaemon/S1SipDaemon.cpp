#include <LogSettings.h>
#include "SipControlServer.h"
#include "Protocol.h"
#include <boost/lexical_cast.hpp>

int main(int argc, char* argv[])
{
    std::string logPath = "";
    if (argc >= 2)
    {
        logPath = argv[1];
    }

    Wisenet::Common::InitializeLogSettings(logPath + "_sip", false);

    SPDLOG_INFO("argv log path : {}", logPath);

    unsigned short portNumber = PORT_NUMBER;
    if (argc >= 3)
    {
        portNumber = boost::lexical_cast<unsigned short>(argv[2]);
        SPDLOG_INFO("argv port : {}", portNumber);
    }

    boost::asio::io_context io_context;

    SipControlServer server(io_context, portNumber);
    server.Start();

    io_context.run();

    return 0;
}