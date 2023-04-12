#pragma once

#include <map>
#include <string>
#include <bitset>
#include <functional>

#ifdef WIN32
	#ifdef _WINDLL
		#ifdef sipproxy_EXPORTS
			#define SIPPROXY_API __declspec(dllexport)
		#else
			#define SIPPROXY_API __declspec(dllimport)
		#endif
	#else	// static library
		#define SIPPROXY_API
	#endif
#else
	#define SIPPROXY_API
#endif

typedef void* SIPPROXY_HANDLE;

#ifndef EC_HANDLE
typedef void* EC_HANDLE;
#endif

enum class SIPPROXY_LOG_LEVEL
{
	eNone	= 0,
	eApp	= 1,
	eFatal	= 2,
	eError	= 4,
	eDebug	= 8,
	eWarn	= 16,
	eInfo	= 32,
	ePrint	= 64,
	eCmd    = 128,
	eSdk	= 256,
	eProfile= 512,
	eAll	= 0xffff
};

enum class SIPPROXY_CONNECTION_TYPE
{
	eUnknown=-1,
	eP2P,
	eRelay
};

using CB_LOG_PRINT = void(*)(char* msg);

#ifdef __cplusplus
extern "C" {
#endif
	SIPPROXY_API void SIPPROXY_SetLogLevel(std::string name, unsigned logBitMask, CB_LOG_PRINT cbLogPrint = nullptr);
	SIPPROXY_API EC_HANDLE SIPPROXY_Open(std::string uid, const std::map<std::string, std::string>& config);
	SIPPROXY_API bool SIPPROXY_Close(EC_HANDLE hEasy);
	SIPPROXY_API SIPPROXY_CONNECTION_TYPE SIPPROXY_GetConnectionType(EC_HANDLE hEasy);
	SIPPROXY_API SIPPROXY_HANDLE SIPPROXY_StartServer(EC_HANDLE hEasy, std::string relayDeviceID, int localPort, int devPort);
	SIPPROXY_API bool SIPPROXY_StopServer(SIPPROXY_HANDLE hProxy);
	SIPPROXY_API bool SIPPROXY_GetDeviceInfo(std::string uid, std::string relayDeviceID, std::string& deviceInfo);
	SIPPROXY_API bool SIPPROXY_IsAlive(std::string uid);
	SIPPROXY_API bool SIPPROXY_Reconnect(std::string uid);
#ifdef __cplusplus
}
#endif
