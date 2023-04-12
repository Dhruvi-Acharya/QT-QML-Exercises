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
#pragma once

#include <memory>
#include <string>
#include <functional>
#include <vector>

#include <JsonArchiver.h>

namespace Wisenet
{
namespace Library
{

struct CloudErrorInfo
{
    int code = 0;
    std::string message;
};

template <typename Archiver>
Archiver& operator&(Archiver& ar, CloudErrorInfo& info)
{
    ar.StartObject();
    ar.Member("code") & info.code;
    ar.Member("message") & info.message;
    return ar.EndObject();
}

struct CloudUserInfo
{
    std::string userName;
    std::string password;
};

template <typename Archiver>
Archiver& operator&(Archiver& ar, CloudUserInfo& info)
{
    ar.StartObject();
    ar.Member("wid") & info.userName;
    ar.Member("password") & info.password;
    return ar.EndObject();
}

struct CloudTokenInfo
{
    std::string accessToken;
    std::string accessTokenIotHub;
    std::string refreshToken;
    std::string tokenType;
    int expiresIn = 0;
    std::string userId;
    std::string jti;
    std::string scope;
    std::string sessionToken;
};

template <typename Archiver>
Archiver& operator&(Archiver& ar, CloudTokenInfo& info)
{
    ar.StartObject();
    ar.Member("access_token") & info.accessToken;
    ar.Member("access_token_iothub") & info.accessTokenIotHub;
    ar.Member("refresh_token") & info.refreshToken;
    ar.Member("token_type") & info.tokenType;
    ar.Member("expires_in") & info.expiresIn;
    ar.Member("userId") & info.userId;
    ar.Member("jti") & info.jti;
    ar.Member("scope") & info.scope;
    ar.Member("sessionToken") & info.sessionToken;
    return ar.EndObject();
}

struct CloudDeviceRegisterInfo
{
    std::string rid;
    std::string deviceUser;
    std::string devicePassword;
};

template <typename Archiver>
Archiver& operator&(Archiver& ar, CloudDeviceRegisterInfo& info)
{
    ar.StartObject();
    ar.Member("rId") & info.rid;
    ar.Member("deviceUser") & info.deviceUser;
    ar.Member("devicePassword") & info.devicePassword;
    return ar.EndObject();
}

struct CloudLinksInfo
{
    std::string self;
};

template <typename Archiver>
Archiver& operator&(Archiver& ar, CloudLinksInfo& info)
{
    ar.StartObject();
    ar.Member("self") & info.self;
    return ar.EndObject();
}

struct CloudDeviceIdDataInfo
{
    std::string deviceId;
};

template <typename Archiver>
Archiver& operator&(Archiver& ar, CloudDeviceIdDataInfo& info)
{
    ar.StartObject();
    ar.Member("deviceId") & info.deviceId;
    return ar.EndObject();
}

struct CloudDeviceIdInfo
{
    CloudLinksInfo links;
    CloudDeviceIdDataInfo data;
};

template <typename Archiver>
Archiver& operator&(Archiver& ar, CloudDeviceIdInfo& info)
{
    ar.StartObject();
    ar.Member("links") & info.links;
    ar.Member("data") & info.data;
    return ar.EndObject();
}

struct CloudDeviceInfo
{
    std::string deviceId;
    std::string firmware;
    std::string model;
    int maxChannels = 0;
    std::string rId;
    std::vector<std::string> encryption;
    bool latestIssuedDeviceId = false;
};

template <typename Archiver>
Archiver& operator&(Archiver& ar, CloudDeviceInfo& info)
{
    ar.StartObject();
    ar.Member("deviceId") & info.deviceId;
    ar.Member("firmware") & info.firmware;
    ar.Member("model") & info.model;
    ar.Member("maxChannels") & info.maxChannels;
    ar.Member("rId") & info.rId;

    if(ar.Member("encryption")) {
        size_t encryptionCount =0;
        ar.StartArray(&encryptionCount);
        if(encryptionCount > 0){
            if(ar.IsReader){
                info.encryption.resize(encryptionCount);
            }

            for(size_t i=0; i < encryptionCount; i++){
                ar & info.encryption[i];
            }
        }
        ar.EndArray();
    }
    ar.Member("latestIssuedDeviceId") & info.latestIssuedDeviceId;

    return ar.EndObject();
}

struct CloudDevicesDataInfo
{
    int nextPageNumber = 0;
    std::vector<CloudDeviceInfo> devices;
};

template <typename Archiver>
Archiver& operator&(Archiver& ar, CloudDevicesDataInfo& info)
{
    ar.StartObject();
    ar.Member("nextPageNumber") & info.nextPageNumber;
    ar.Member("devices");

    size_t deviceCount = info.devices.size();
    ar.StartArray(&deviceCount);

    if(ar.IsReader){
        info.devices.resize(deviceCount);
    }

    for(size_t i=0; i < deviceCount; i++){
        ar & info.devices[i];
    }
    ar.EndArray();

    return ar.EndObject();
}

struct CloudDevicesInfo
{
    CloudLinksInfo links;
    CloudDevicesDataInfo data;
};

template <typename Archiver>
Archiver& operator&(Archiver& ar, CloudDevicesInfo& info)
{
    ar.StartObject();
    ar.Member("links") & info.links;
    ar.Member("data") & info.data;
    return ar.EndObject();
}

struct CloudIceServerInfo
{
    std::vector<std::string> urls;
    std::string userName;
    std::string credential;
};

template <typename Archiver>
Archiver& operator&(Archiver& ar, CloudIceServerInfo& info)
{
    ar.StartObject();
    ar.Member("urls");

    size_t urlCount = info.urls.size();
    ar.StartArray(&urlCount);

    if(ar.IsReader){
        info.urls.resize(urlCount);
    }

    for(size_t i=0; i < urlCount; i++){
        ar & info.urls[i];
    }
    ar.EndArray();

    if(ar.HasMember("username")){
        ar.Member("username") & info.userName;
    }

    if(ar.HasMember("credential")){
        ar.Member("credential") & info.credential;
    }

    return ar.EndObject();
}

struct CloudTurnDataInfo
{
    std::vector<CloudIceServerInfo> iceServers;
};

template <typename Archiver>
Archiver& operator&(Archiver& ar, CloudTurnDataInfo& info)
{
    ar.StartObject();
    ar.Member("iceServers");

    size_t iceServerCount = info.iceServers.size();
    ar.StartArray(&iceServerCount);

    if(ar.IsReader){
        info.iceServers.resize(iceServerCount);
    }

    for(size_t i=0; i < iceServerCount; i++){
        ar & info.iceServers[i];
    }
    ar.EndArray();

    return ar.EndObject();
}
struct CloudTurnInfo
{
    CloudLinksInfo links;
    CloudTurnDataInfo data;
};

template <typename Archiver>
Archiver& operator&(Archiver& ar, CloudTurnInfo& info)
{
    ar.StartObject();
    ar.Member("links") & info.links;
    ar.Member("data") & info.data;
    return ar.EndObject();
}

struct CloudTurnDataInfo2
{
    std::string stunAddress;
    std::string turnAddress;
    std::string turnUserName;
    std::string turnCredential;
};

template <typename Archiver>
Archiver& operator&(Archiver& ar, CloudTurnDataInfo2& info)
{
    ar.StartObject();
    ar.Member("stunAddress")& info.stunAddress;
    ar.Member("turnAddress")& info.turnAddress;
    ar.Member("turnUserName")& info.turnUserName;
    ar.Member("turnCredential")& info.turnCredential;
    return ar.EndObject();
}

struct CloudTurnInfo2
{
    CloudLinksInfo links;
    CloudTurnDataInfo2 data;
};

template <typename Archiver>
Archiver& operator&(Archiver& ar, CloudTurnInfo2& info)
{
    ar.StartObject();
    ar.Member("links")& info.links;
    ar.Member("data")& info.data;
    return ar.EndObject();
}

struct CloudDeleteDevicesInfo
{
    std::vector<std::string> deviceIds;
};

template <typename Archiver>
Archiver& operator&(Archiver& ar, CloudDeleteDevicesInfo& info)
{
    ar.StartObject();
    ar.Member("deviceIds");

    size_t deviceIdCount = info.deviceIds.size();
    ar.StartArray(&deviceIdCount);

    if (ar.IsReader) {
        info.deviceIds.resize(deviceIdCount);
    }

    for (size_t i = 0; i < deviceIdCount; i++) {
        ar& info.deviceIds[i];
    }
    ar.EndArray();

    return ar.EndObject();
}

}
}
