#include <memory.h>
#include <stdio.h>
#include "MacConverter.h"
#include <string>
#include <algorithm>

namespace alllink
{

static const unsigned short crc16tab[256] = {
    0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
    0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,
    0x1231,0x0210,0x3273,0x2252,0x52b5,0x4294,0x72f7,0x62d6,
    0x9339,0x8318,0xb37b,0xa35a,0xd3bd,0xc39c,0xf3ff,0xe3de,
    0x2462,0x3443,0x0420,0x1401,0x64e6,0x74c7,0x44a4,0x5485,
    0xa56a,0xb54b,0x8528,0x9509,0xe5ee,0xf5cf,0xc5ac,0xd58d,
    0x3653,0x2672,0x1611,0x0630,0x76d7,0x66f6,0x5695,0x46b4,
    0xb75b,0xa77a,0x9719,0x8738,0xf7df,0xe7fe,0xd79d,0xc7bc,
    0x48c4,0x58e5,0x6886,0x78a7,0x0840,0x1861,0x2802,0x3823,
    0xc9cc,0xd9ed,0xe98e,0xf9af,0x8948,0x9969,0xa90a,0xb92b,
    0x5af5,0x4ad4,0x7ab7,0x6a96,0x1a71,0x0a50,0x3a33,0x2a12,
    0xdbfd,0xcbdc,0xfbbf,0xeb9e,0x9b79,0x8b58,0xbb3b,0xab1a,
    0x6ca6,0x7c87,0x4ce4,0x5cc5,0x2c22,0x3c03,0x0c60,0x1c41,
    0xedae,0xfd8f,0xcdec,0xddcd,0xad2a,0xbd0b,0x8d68,0x9d49,
    0x7e97,0x6eb6,0x5ed5,0x4ef4,0x3e13,0x2e32,0x1e51,0x0e70,
    0xff9f,0xefbe,0xdfdd,0xcffc,0xbf1b,0xaf3a,0x9f59,0x8f78,
    0x9188,0x81a9,0xb1ca,0xa1eb,0xd10c,0xc12d,0xf14e,0xe16f,
    0x1080,0x00a1,0x30c2,0x20e3,0x5004,0x4025,0x7046,0x6067,
    0x83b9,0x9398,0xa3fb,0xb3da,0xc33d,0xd31c,0xe37f,0xf35e,
    0x02b1,0x1290,0x22f3,0x32d2,0x4235,0x5214,0x6277,0x7256,
    0xb5ea,0xa5cb,0x95a8,0x8589,0xf56e,0xe54f,0xd52c,0xc50d,
    0x34e2,0x24c3,0x14a0,0x0481,0x7466,0x6447,0x5424,0x4405,
    0xa7db,0xb7fa,0x8799,0x97b8,0xe75f,0xf77e,0xc71d,0xd73c,
    0x26d3,0x36f2,0x0691,0x16b0,0x6657,0x7676,0x4615,0x5634,
    0xd94c,0xc96d,0xf90e,0xe92f,0x99c8,0x89e9,0xb98a,0xa9ab,
    0x5844,0x4865,0x7806,0x6827,0x18c0,0x08e1,0x3882,0x28a3,
    0xcb7d,0xdb5c,0xeb3f,0xfb1e,0x8bf9,0x9bd8,0xabbb,0xbb9a,
    0x4a75,0x5a54,0x6a37,0x7a16,0x0af1,0x1ad0,0x2ab3,0x3a92,
    0xfd2e,0xed0f,0xdd6c,0xcd4d,0xbdaa,0xad8b,0x9de8,0x8dc9,
    0x7c26,0x6c07,0x5c64,0x4c45,0x3ca2,0x2c83,0x1ce0,0x0cc1,
    0xef1f,0xff3e,0xcf5d,0xdf7c,0xaf9b,0xbfba,0x8fd9,0x9ff8,
    0x6e17,0x7e36,0x4e55,0x5e74,0x2e93,0x3eb2,0x0ed1,0x1ef0
};

//crc-16 ccitt버전을 계산하는 함수
unsigned short crc16_ccitt(const void* buf, int len)
{
    unsigned char* buf2 = (unsigned char*)buf;
    register int counter;
    register unsigned short crc = 0xFFFF; //initial value(ccitt default value)
    for (counter = 0; counter < len; counter++)
    {
        crc = (crc << 8) ^ crc16tab[((crc >> 8) ^ *buf2) & 0x00FF];
        buf2++;
    }
    return crc;
}

//MAC주소를 구성하는 문자하나를 시리얼(SN) 문자하나로 치환한다.
unsigned char transform_mac_char(unsigned char ch)
{
    if (ch >= '0' && ch <= '9')
    {
        char ret = 'A' + (ch - '0'); //문자열 'A'~ 'J'변환
        return ret;
    }
    else if (ch >= 'a' && ch <= 'f')
    {
        return 'R' + (ch - 'a');//문자열 'R' ~'V'변환
    }
    else if (ch >= 'A' && ch <= 'F')
    {
        return 'R' + (ch - 'A');//문자열 'R' ~'V'변환
    }
    return 0x00;
}

//시리얼(SN)주소를 구성하는 문자하나를 MAC 문자하나로 치환한다.
unsigned char transform_sn_char(unsigned char ch)
{
    if (ch >= 'A' && ch <= 'J')
    {
        return '0' + (ch - 'A'); //문자열 'A'~ 'J'변환
    }
    else if (ch >= 'R' && ch <= 'V')
    {
        return 'A' + (ch - 'R');//문자열 'R' ~'V'변환
    }

    return 0x00;
}

CMacConverter::CMacConverter(void)
{
    memset(mac_buf, 0x00, sizeof(mac_buf));
    memset(sn_buf, 0x00, sizeof(sn_buf));
}

CMacConverter::~CMacConverter(void)
{
}


//MAC주소가 담겨있는 문자열을 시리얼(S/N)으로 변환한다.
//오류발생 시, 문자열의 길이가 0인 것이 리턴된다.
const char* CMacConverter::ConvertToSN(const char* mac)
{
    memset(sn_buf, 0x00, sizeof(sn_buf));
    if (mac == NULL || strlen(mac) != 12)
    {//입력파라미터 오류, NULL값 또는 mac 주소 문자열의 길이가 잘못됐다.
        return (const char*)sn_buf;
    }

    //mac문자열을 시리얼(SN)표시 문자열로 치환한다.
    for (int i = 0; i < 12; i++)
    {
        sn_buf[i] = transform_mac_char(mac[i]);
        if (sn_buf[i] == 0x00)
        {//MAC주소 구성 문자열 오류
            memset(sn_buf, 0x00, sizeof(sn_buf));
            return (const char*)sn_buf;
        }
    }

    unsigned short crc = crc16_ccitt(sn_buf, 12); //crc16을 계산한다.(MAC주소가 치환된 문자열이 입력됨)
    unsigned char crc_temp[5] = { 0x00, };
    sprintf((char*)crc_temp, "%04X", crc);//crc16을 문자열로 변환한다.

    char sn_string[1024] = { 0x00, };//SN문자열
    //MAC이 치환된 문자열의 길이는 12자리 + CRC문자열 4자리 총16자리인데
    //치환문자열 3자리에 crc문자 1자리를 합쳐서 시리얼(SN) 문자열을 재구성하여 완성한다.
    sprintf(sn_string, "%c%c%c%c" //첫번째 그룹
        "%c%c%c%c" //두번째 그룹
        "%c%c%c%c" //세번째 그룹
        "%c%c%c%c" //네번째 그룹
        , sn_buf[0], sn_buf[1], sn_buf[2], crc_temp[0]
        , sn_buf[3], sn_buf[4], sn_buf[5], crc_temp[1]
        , sn_buf[6], sn_buf[7], sn_buf[8], crc_temp[2]
        , sn_buf[9], sn_buf[10], sn_buf[11], crc_temp[3]);

    //sn_string문자열을 그냥 return하면 메모리가 깨질 수 있으므로
    //임시버퍼에 복사한 후 리턴한다.
    strncpy(sn_buf, sn_string, 16);

    return sn_buf;
}

//시리얼(S/N)이 담겨있는 문자열을 MAC주소 문자열로 변환한다.
//오류발생 시, 문자열의 길이가 0인 것이 리턴된다.
const char* CMacConverter::ConvertToMAC(const char* sn)
{
    memset(mac_buf, 0x00, sizeof(mac_buf));
    if (sn == NULL || strlen(sn) != 16)
    {
        return mac_buf;
    }

    //MAC이 치환된 문자열의 길이는 12자리 + CRC문자열 4자리 총16자리인데
    //치환문자열 3자리에 crc문자 1자리를 합쳐서 시리얼(SN) 문자열을 재구성되어 있는데
    //이를 분해하여 MAC이 치환된 문자열과 CRC 문자열을 분리한다.

    unsigned char crc_temp[5] = { 0x00, };//CRC문자열 저장
    char sn_string[1024] = { 0x00, };//MAC주소가 치환된 문자열

    //1그룹
    strncpy(&sn_string[0], &sn[0], 3);
    crc_temp[0] = sn[3];
    //2그룹
    strncpy(&sn_string[3], &sn[4], 3);
    crc_temp[1] = sn[7];
    //3그룹
    strncpy(&sn_string[6], &sn[8], 3);
    crc_temp[2] = sn[11];
    //4그룹
    strncpy(&sn_string[9], &sn[12], 3);
    crc_temp[3] = sn[15];


    unsigned short crc = crc16_ccitt(sn_string, 12); //crc16을 계산한다.(MAC주소가 치환된 문자열이 입력됨
    char calc_crc[5] = { 0x00, };
    sprintf(calc_crc, "%04X", crc);

    if (memcmp(crc_temp, calc_crc, 4) != 0)
    {//입력한 시리얼(SN)의 CRC와 계산한 CRC가 다르다면에러
        memset(mac_buf, 0x00, sizeof(mac_buf));
        return mac_buf;
    }

    //mac문자열을 시리얼(SN)표시 문자열로 치환한다.
    for (int i = 0; i < 12; i++)
    {
        mac_buf[i] = transform_sn_char(sn_string[i]);
        if (mac_buf[i] == 0x00)
        {//시리얼(SN)구성 문자열 오류
            memset(mac_buf, 0x00, sizeof(mac_buf));
            return (const char*)mac_buf;
        }
    }

    return mac_buf;//MAC주소 문자열을 리턴한다.
}


bool ParseUserID(const std::string UserID, std::string& outSerial, std::string& outType, std::string& outGroupID, std::string& outDomainName)
{
    char Serial[100] = { 0 }, DevType[100] = { 0 }, GroupID[100] = { 0 }, DomainName[100] = { 0 };

    sscanf(UserID.c_str(), "sips:%99[^.].%99[^.].%99[^@]@%99[^\n]", Serial, DevType, GroupID, DomainName);
    if (strlen(Serial) == 0 || strlen(DevType) == 0 || strlen(GroupID) == 0 || strlen(DomainName) == 0)
        return false;

    outSerial = Serial;
    outType = DevType;
    outGroupID = GroupID;
    outDomainName = DomainName;

    return true;;
}

std::string GenerateSerialNumber(std::string Mac)
{
    CMacConverter convert;
    Mac.erase(std::remove(Mac.begin(), Mac.end(), ':'), Mac.end());  // remove ":" from "11:aa:22:bb:33:cc" string.

    return convert.ConvertToSN(Mac.c_str());
}

std::string ToUid(std::string mac)
{
    std::string serialNumber = GenerateSerialNumber(mac);
    std::string domainID = serialNumber + ".NVR.TECHWIN";

    return "sips:" + domainID + "@s1.co.kr";
}

std::string ToMac(std::string uid)
{
    std::string Serial, Type, GroupID, DomainName;
    const char* mac = nullptr;
    std::string macStr;

    if (ParseUserID(uid, Serial, Type, GroupID, DomainName) == false)
    {
        return "";
    }

    CMacConverter convert;

    mac = convert.ConvertToMAC(Serial.c_str());
    if (mac != nullptr)
    {
        macStr.push_back(mac[0]);
        macStr.push_back(mac[1]);
        macStr.push_back(':');
        macStr.push_back(mac[2]);
        macStr.push_back(mac[3]);
        macStr.push_back(':');
        macStr.push_back(mac[4]);
        macStr.push_back(mac[5]);
        macStr.push_back(':');
        macStr.push_back(mac[6]);
        macStr.push_back(mac[7]);
        macStr.push_back(':');
        macStr.push_back(mac[8]);
        macStr.push_back(mac[9]);
        macStr.push_back(':');
        macStr.push_back(mac[10]);
        macStr.push_back(mac[11]);
        return macStr;
    }

    return "";
}

}
