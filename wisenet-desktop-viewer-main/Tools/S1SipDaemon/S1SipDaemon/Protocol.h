#pragma once 
const unsigned short PORT_NUMBER = 1881;
const int MAX_RECEIVE_BUFFER_LEN = 512;
const int MAX_MESSAGE_LEN = 129;
const int MAC_LENGTH = 6;

struct PACKET_HEADER
{
    short nCommand;
    short nSize;
};

const short REQ_SIP_CONNECT = 6;
const short RES_SIP_CONNECT = 7;
const short REQ_SIP_DISCONNECT = 8;
const short REQ_STOP_SERVICE = 9;

struct PKT_REQ_SIP_CONNECT : public PACKET_HEADER
{
    void Init()
    {
        nCommand = REQ_SIP_CONNECT;
        nSize = sizeof(PKT_REQ_SIP_CONNECT);
        memset(szMac, 0, MAC_LENGTH);
    }

    unsigned char szMac[MAC_LENGTH];
};

struct PKT_REQ_SIP_DISCONNECT : public PACKET_HEADER
{
    void Init()
    {
        nCommand = REQ_SIP_DISCONNECT;
        nSize = sizeof(PKT_REQ_SIP_DISCONNECT);
        memset(szMac, 0, MAC_LENGTH);
    }

    unsigned char szMac[MAC_LENGTH];
};

struct PKT_RES_SIP_CONNECT : public PACKET_HEADER
{
    void Init()
    {
        nCommand = RES_SIP_CONNECT;
        nSize = sizeof(PKT_RES_SIP_CONNECT);
        nPort = 0;
        memset(szMac, 0, MAC_LENGTH);
    }

    unsigned short nPort;
    unsigned short nHttpsPort;
    short nConnectionType;
    unsigned char szMac[MAC_LENGTH];
};

struct PKT_REQ_STOP_SERVICE : public PACKET_HEADER
{
    void Init()
    {
        nCommand = REQ_STOP_SERVICE;
        nSize = sizeof(PKT_REQ_STOP_SERVICE);
    }
};