#pragma once

#include <string>

namespace alllink
{
/*
 * UID means SIP_PUIBLIC_URI in AllLink SDK.
 * UID Syntax conforms to "sip:<SerialNumber>.<Type>.<GroupID>@<DomainName>" rule.
 * ex) sip:AAB4BFW5SDC3IBDE.NVR.TECHWIN@s1.co.kr
*/
bool ParseUserID(const std::string UID, std::string& outSerial, std::string& outType, std::string& outGroupID, std::string& outDomainName);
std::string ToUid(std::string mac);
std::string ToMac(std::string uid);

class CMacConverter
{
protected:
	char		mac_buf[1024];
	char		sn_buf[1024];
public:
	CMacConverter(void);
	virtual ~CMacConverter(void);

	//MAC주소가 담겨있는 문자열을 시리얼(S/N)으로 변환한다.
	const char* ConvertToSN(const char* mac);
	//시리얼(S/N)이 담겨있는 문자열을 MAC주소 문자열로 변환한다.
	const char* ConvertToMAC(const char* sn);
};

}
