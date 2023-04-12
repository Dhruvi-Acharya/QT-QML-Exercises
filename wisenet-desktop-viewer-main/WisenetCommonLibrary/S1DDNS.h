#pragma once

#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <boost/asio.hpp>
#include <QString>

#pragma pack(push, 1)
typedef struct s1_ddns_packet_header_tag {
    unsigned short	length;
    unsigned char	majorversion;
    unsigned char	minorversion;
    unsigned char	seed;
    unsigned short	headerCRC16;
} S1_DDNS_PACKET_HEADER;

typedef struct s1_ddns_req_packet_tag {
    S1_DDNS_PACKET_HEADER header;

    unsigned char	command;
    unsigned char	mac[6];
    unsigned short	dataCRC16;
} S1_DDNS_REQ_PACKET;

typedef struct s1_ddns_ans_packet_tag {
    S1_DDNS_PACKET_HEADER header;

    unsigned char	command;
    unsigned short	errcode;
    unsigned char	reserved1[8];
    unsigned char	reserved2[8];
    unsigned char	reserved3[24];
    unsigned char	usepeerip;
    unsigned char	peerip[4];
    unsigned char	cntip[4];
    unsigned short	portnum;
    unsigned short	port[7];
    unsigned char	mac[6];
    unsigned char	ddnsip[4];
    unsigned short	ddnsport;
    unsigned short	dataCRC16;

} S1_DDNS_ANS_PACKET;
#pragma pack(pop)

class S1DDNS : public std::enable_shared_from_this<S1DDNS>
{
public:
    S1DDNS(boost::asio::io_context& ioContext);
    ~S1DDNS();

    bool GetPeerAddress(const std::string& macAddress, std::string& peerAddress,
                        unsigned short& httpPort, unsigned short& httpsPort,
                        unsigned short& rtspPort, std::string ddnsServerIp = "",
                        unsigned short ddnsPort = 0);

private:
    int EncryptS1Data(const unsigned char* data, int data_length, unsigned char* out_data);
    int DecryptS1Data(const unsigned char* data, int data_length, unsigned char* out_data);
    int Seed128CbcEncryption(const unsigned char* data, int data_length, unsigned char* encrypt_data, const unsigned char* key, const unsigned char* iv) const;
    int Seed128CbcDecryption(const unsigned char* data, int data_length, unsigned char* decrypt_data, const unsigned char* key, const unsigned char* iv) const;

    unsigned short CalculateCRC16(const unsigned char* data, int length);
    void init_ddns_request_packet(S1_DDNS_REQ_PACKET* packet, unsigned char* mac);
    int char2int(char input);
    void ConvertMacAddress(const char* src, unsigned char* target);

private:
    boost::asio::io_context& m_ioContext;

    unsigned char UserKey[16] = { 0x41,0x6E,0x3C,0x63,0x6A,0x35,0x3E,0x39,0x61,0x6F,0x51,0x6A,0x33,0x55,0x39,0x3B };
    unsigned char IV[16] = { 0x4E,0xEC,0xAA,0x88,0x66,0x44,0x22,0x11,0xDE,0xEC,0xAA,0x98,0x76,0x54,0x32,0x10 };
    std::string S1_DDNS_HTTP_ADDRESS = "apddns.s1.co.kr";
    unsigned short S1_DDNS_PORT = 11002;
};

