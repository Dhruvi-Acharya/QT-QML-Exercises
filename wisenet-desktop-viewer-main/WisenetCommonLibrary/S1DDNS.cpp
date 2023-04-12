#include "S1DDNS.h"
#include <iostream>
#include <QDebug>

S1DDNS::S1DDNS(boost::asio::io_context& ioContext)
    : m_ioContext(ioContext)
{

}

S1DDNS::~S1DDNS()
{

}

int S1DDNS::Seed128CbcEncryption(const unsigned char* data, int data_length,
                                 unsigned char* encrypt_data,
                                 const unsigned char* key,
                                 const unsigned char* iv) const
{
    EVP_CIPHER_CTX* cipher_context;
    if (!(cipher_context = EVP_CIPHER_CTX_new()))
    {
        return 0;
    }

    if (EVP_EncryptInit_ex(cipher_context, EVP_seed_cbc(), NULL, key, iv) != 1)
    {
        EVP_CIPHER_CTX_free(cipher_context);
        return 0;
    }

    EVP_CIPHER_CTX_set_padding(cipher_context, 1);

    int encrypt_data_length = 0;

    if (EVP_EncryptUpdate(cipher_context, encrypt_data, &encrypt_data_length,
                          data, data_length) != 1)
    {
        EVP_CIPHER_CTX_free(cipher_context);
        return 0;
    }

    int finalized_data_length = 0;

    EVP_EncryptFinal_ex(cipher_context, encrypt_data + encrypt_data_length,
                        &finalized_data_length);

    encrypt_data_length += finalized_data_length;
    EVP_CIPHER_CTX_free(cipher_context);

    return encrypt_data_length;
}

int S1DDNS::EncryptS1Data(const unsigned char* data, int data_length, unsigned char* out_data)
{
    int encrypt_data_size = 0;

    encrypt_data_size = Seed128CbcEncryption(data, data_length, out_data, UserKey, IV);

    return encrypt_data_size;
}

int S1DDNS::Seed128CbcDecryption(const unsigned char* data, int data_length,
                                 unsigned char* decrypt_data,
                                 const unsigned char* key,
                                 const unsigned char* iv) const
{
    EVP_CIPHER_CTX* cipher_context;
    if (!(cipher_context = EVP_CIPHER_CTX_new()))
    {
        return 0;
    }

    if (EVP_DecryptInit_ex(cipher_context, EVP_seed_cbc(), NULL, key, iv) != 1)
    {
        EVP_CIPHER_CTX_free(cipher_context);
        return 0;
    }

    EVP_CIPHER_CTX_set_padding(cipher_context, 1);

    int decrypt_data_length = 0;
    if (EVP_DecryptUpdate(cipher_context, decrypt_data, &decrypt_data_length, data, data_length) != 1)
    {
        EVP_CIPHER_CTX_free(cipher_context);
        return 0;
    }

    int finalized_decrypt_data_length = 0;
    EVP_DecryptFinal_ex(cipher_context,
                        decrypt_data + decrypt_data_length,
                        &finalized_decrypt_data_length);

    decrypt_data_length += finalized_decrypt_data_length;

    EVP_CIPHER_CTX_free(cipher_context);

    return decrypt_data_length;
}

int S1DDNS::DecryptS1Data(const unsigned char* data, int data_length, unsigned char* out_data)
{
    int decrypt_data_size = 0;

    decrypt_data_size = Seed128CbcDecryption(data, data_length, out_data, UserKey, IV);

    return decrypt_data_size;
}

unsigned short S1DDNS::CalculateCRC16(const unsigned char* data, int length)
{
    unsigned short crc = 0xFFFF;
    unsigned short result = 0;

    if (length <= 0)
        return 0xffff;

    do
    {
        unsigned char byte = *data++;
        byte = (byte ^ (unsigned char)(crc & 0x00FF));
        byte = (byte ^ (byte << 4));
        crc = (crc >> 8) ^ ((unsigned short)byte << 8)
                ^ ((unsigned short)byte << 3) ^ ((unsigned short)byte >> 4);
    } while (--length);

    crc = ~crc;
    ((unsigned char*)&result)[0] = (unsigned char)(crc & 0xFF);
    ((unsigned char*)&result)[1] = (unsigned char)((crc >> 8) & 0xFF);

    return result;
}

void S1DDNS::init_ddns_request_packet(S1_DDNS_REQ_PACKET* packet, unsigned char* mac)
{
    unsigned short crc;

    memset(packet, 0, sizeof(S1_DDNS_REQ_PACKET));

    packet->header.length = htons(sizeof(S1_DDNS_REQ_PACKET) - sizeof(S1_DDNS_PACKET_HEADER));
    packet->header.majorversion = 0x01;
    packet->header.minorversion = 0x00;
    packet->header.seed = 0x00;

    crc = CalculateCRC16((unsigned char*)&packet->header, sizeof(S1_DDNS_PACKET_HEADER) - 2);
    packet->header.headerCRC16 = htons(crc);

    packet->command = 0x41;
    memcpy(packet->mac, mac, 6);

    crc = CalculateCRC16((unsigned char*)packet + sizeof(S1_DDNS_PACKET_HEADER), ntohs(packet->header.length) - 2);
    packet->dataCRC16 = htons(crc);
}

int S1DDNS::char2int(char input)
{
    if (input >= '0' && input <= '9')
        return input - '0';
    if (input >= 'A' && input <= 'F')
        return input - 'A' + 10;
    if (input >= 'a' && input <= 'f')
        return input - 'a' + 10;
    throw std::invalid_argument("Invalid input string");
}

void S1DDNS::ConvertMacAddress(const char* src, unsigned char* target)
{
    while (*src && src[1])
    {
        *(target++) = char2int(*src) * 16 + char2int(src[1]);
        src += 2;
    }
}

bool S1DDNS::GetPeerAddress(const std::string& macAddress, std::string& peerAddress,
                            unsigned short& httpPort, unsigned short& httpsPort,
                            unsigned short& rtspPort, std::string ddnsServerIp,
                            unsigned short ddnsPort)
{
    unsigned char szMac[6] = { 0 };

    ConvertMacAddress(macAddress.c_str(), szMac);

    boost::asio::ip::tcp::endpoint endpoint;

    if (ddnsServerIp == "")
    {
        boost::asio::ip::tcp::resolver resolver(m_ioContext);
        boost::asio::ip::tcp::resolver::query query(S1_DDNS_HTTP_ADDRESS, "http");

        boost::asio::ip::tcp::resolver::results_type results = resolver.resolve(query);


        for (boost::asio::ip::tcp::endpoint const& endpoint : results)
        {
            qDebug() << "[S1DDNS] Server address"<< QString::fromStdString(endpoint.address().to_string());
            ddnsServerIp = endpoint.address().to_string();
        }

        endpoint = boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(ddnsServerIp), S1_DDNS_PORT);
    }
    else
    {
        endpoint = boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(ddnsServerIp), ddnsPort);
    }

    boost::system::error_code connect_error;
    boost::asio::ip::tcp::socket socket(m_ioContext);

    socket.connect(endpoint, connect_error);

    if (connect_error)
    {
        qDebug() << "[S1DDNS] Connection failed" << connect_error.value() << ", Message: " << QString::fromStdString(connect_error.message());
        return false;
    }
    else
    {
        qDebug() << "[S1DDNS] Connection success";
    }

    S1_DDNS_REQ_PACKET	packet;
    S1_DDNS_ANS_PACKET	res;

    init_ddns_request_packet(&packet, szMac);
    memset(&res, 0, sizeof(S1_DDNS_ANS_PACKET));

    int length;

    unsigned long enc_length;
    unsigned long dec_length;
    unsigned short crc;

    // Send buffer
    unsigned char buf[1024];
    memset(buf, 0, sizeof(buf));
    length = sizeof(S1_DDNS_REQ_PACKET) - sizeof(S1_DDNS_PACKET_HEADER) - 2;

    enc_length = EncryptS1Data((unsigned char*)&packet + sizeof(S1_DDNS_PACKET_HEADER), length, buf + sizeof(S1_DDNS_PACKET_HEADER));

    /*
    // For test
    BYTE decbuf[200];
    int decLength = 0;
    decLength = DecryptS1Data(buf + sizeof(S1_DDNS_PACKET_HEADER), enc_length, decbuf);
    */

    ((S1_DDNS_PACKET_HEADER*)buf)->length = htons((unsigned short)enc_length + 2);
    ((S1_DDNS_PACKET_HEADER*)buf)->majorversion = 0x01;
    ((S1_DDNS_PACKET_HEADER*)buf)->minorversion = 0x00;
    ((S1_DDNS_PACKET_HEADER*)buf)->seed = 0x01;

    // Header CRC
    crc = CalculateCRC16(buf, sizeof(S1_DDNS_PACKET_HEADER) - 2);
    ((S1_DDNS_PACKET_HEADER*)buf)->headerCRC16 = htons(crc);

    // Body CRC
    crc = CalculateCRC16(buf + sizeof(S1_DDNS_PACKET_HEADER), enc_length);
    *(unsigned short*)(buf + sizeof(S1_DDNS_PACKET_HEADER) + enc_length) = htons(crc);

    // Total packet length
    length = sizeof(S1_DDNS_PACKET_HEADER) + enc_length + 2;

    boost::system::error_code ignored_error;
    socket.write_some(boost::asio::buffer(buf, length), ignored_error);

    boost::system::error_code error;
    size_t len = socket.read_some(boost::asio::buffer(buf), error);

    if (error)
    {
        if (error == boost::asio::error::eof)
        {
            qDebug() << "[S1DDNS] Server disconnected";
        }
        else
        {
            qDebug()<< "[S1DDNS] error No: " << error.value() << " error Message: " << QString::fromStdString(error.message());
        }

        return false;
    }

    dec_length = ntohs(((S1_DDNS_PACKET_HEADER*)buf)->length) - 2;

    unsigned char temp[128];
    memset(temp, 0, sizeof(temp));

    // dst, len
    length = DecryptS1Data(buf + sizeof(S1_DDNS_PACKET_HEADER), dec_length, temp + sizeof(S1_DDNS_PACKET_HEADER));

    memcpy(&res, temp, sizeof(res));

    if (socket.is_open())
    {
        socket.close();
    }

    res.header.length = ntohs(res.header.length);
    res.errcode = ntohs(res.errcode);
    res.ddnsport = ntohs(res.ddnsport);
    res.portnum = ntohs(res.portnum);

    res.port[0] = ntohs(res.port[0]);
    res.port[1] = ntohs(res.port[1]);
    res.port[2] = ntohs(res.port[2]);
    res.port[3] = ntohs(res.port[3]);
    res.port[4] = ntohs(res.port[4]);
    res.port[5] = ntohs(res.port[5]);
    res.port[6] = ntohs(res.port[6]);

    if (res.errcode == 0x00)		// no error
    {
        char new_ddnsip[20];
        char new_peerip[20];
        std::string newDdnsIp;

        snprintf(new_ddnsip, sizeof(new_ddnsip), "%d.%d.%d.%d", res.ddnsip[0], res.ddnsip[1], res.ddnsip[2], res.ddnsip[3]);
        newDdnsIp = new_ddnsip;

        if ((res.header.length == (sizeof(S1_DDNS_ANS_PACKET) - sizeof(S1_DDNS_PACKET_HEADER)) && (ddnsServerIp != newDdnsIp) || S1_DDNS_PORT != res.ddnsport))
            return GetPeerAddress(macAddress, peerAddress, httpPort, httpsPort, rtspPort, newDdnsIp, res.ddnsport);

        //success
        snprintf(new_peerip, sizeof(new_peerip), "%d.%d.%d.%d", res.peerip[0], res.peerip[1], res.peerip[2], res.peerip[3]);

        peerAddress = new_peerip;
        httpPort = res.port[0];
        httpsPort = res.port[1];
        rtspPort = res.port[2];

        return true;
    }

    return false;
}
