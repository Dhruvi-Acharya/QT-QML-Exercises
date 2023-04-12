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

#include "DeviceClient/DeviceStructure.h"
#include "CoreServiceLogSettings.h"
#include <random>

namespace Wisenet
{
namespace Core
{

inline void CalculateLowHighLiveProfile(
        const Device::DeviceType deviceType,
        const std::map<std::string, Device::VideoProfile>& profiles,
        std::string& retHighProfileID, std::string& retLowProfileID,
        Wisenet::Device::Resolution& highRes, Wisenet::Device::Resolution& lowRes,
        int& highFramerate, int& lowFramerate,
        Wisenet::Media::VideoCodecType& highCodecType, Wisenet::Media::VideoCodecType& lowCodecType)
{
    retLowProfileID = "";
    retHighProfileID = "";

    //    SPDLOG_DEBUG("CalculateLowHighLiveProfile(), deviceType={}, profileCount={}",
    //                 deviceType, profiles.size());

    // Recorder //////////////////////////////////
    if (deviceType == Device::DeviceType::SunapiRecorder) {
        for(auto& kv : profiles) {
            //SPDLOG_DEBUG("RECORDER ALIAS NAME = {},{}", kv.second.aliasName, Device::RECORD_PROFILE_RECORER);
            if (kv.second.aliasName == Device::RECORD_PROFILE_RECORER ) {
                retHighProfileID = kv.second.aliasID;
                highRes = kv.second.resolution;
                highFramerate = kv.second.framerate;
                highCodecType = kv.second.codecType;
            }
            else if (kv.second.aliasName == Device::NETWORK_PROFILE_RECORER ) {
                retLowProfileID = kv.second.aliasID;
                lowRes = kv.second.resolution;
                lowFramerate = kv.second.framerate;
                lowCodecType = kv.second.codecType;
            }
        }
        return;
    }

    // Camera ////////////////////////////////////
    int maxResV = 0;
    int minResV = INT32_MAX;
    std::string H_264;
    std::string H_265;
    std::string Low4SSM;
    std::string Live4NVR;
    std::string WAVEPrimary;
    std::string WAVESecondary;
    std::string MOBILE;
    std::string highestRes;
    std::string lowestRes;


    for(auto& kv : profiles) {
        int curResV = kv.second.resolution.width * kv.second.resolution.height;
        if (curResV > maxResV) {
            maxResV = curResV;
            highestRes = kv.second.profileID;
        }
        else if (curResV < minResV) {
            minResV = curResV;
            lowestRes = kv.second.profileID;
        }

        // find default profile
        // fisheye camera(xnf-8010)의 경우 2번 프로파일 이름이 "FisheyeView"임
        if (kv.second.codecType == Wisenet::Media::VideoCodecType::H264 && kv.second.profileID == "2" ) {
            H_264 = kv.second.profileID;
        }
        else if (kv.second.codecType == Wisenet::Media::VideoCodecType::H265 && kv.second.profileID == "3" ) {
            H_265 = kv.second.profileID;
        }
        /*
        if (kv.second.profileName == "H.264" && kv.second.profileID == "2" ) {
            H_264 = kv.second.profileID;
        }
        else if (kv.second.profileName == "H.265" && kv.second.profileID == "3" ) {
            H_265 = kv.second.profileID;
        }
        */

        // find Low4SSM
        else if (kv.second.profileName == Device::LOW4SSM_PROFILE) {
            Low4SSM = kv.second.profileID;
        }
        // find Live4NVR
        else if (kv.second.profileName == Device::LIVE4NVR_PROFILE) {
            Live4NVR = kv.second.profileID;
        }
        // find WAVEPrimary
        else if (kv.second.profileName == Device::WAVEPRIMARY_PROFILE) {
            WAVEPrimary = kv.second.profileID;
        }
        // find WAVESecondary
        else if (kv.second.profileName == Device::WAVESECONDARY_PROFILE) {
            WAVESecondary = kv.second.profileID;
        }
        // find WAVESecondary
        else if (kv.second.profileName == Device::MOBILE_PROFILE) {
            MOBILE = kv.second.profileID;
        }
    }

    // estimate high profile
    // 1. H.264 -> WAVEPrimary -> H.265
    if (!H_264.empty())                 retHighProfileID = H_264;
    else if (!WAVEPrimary.empty())      retHighProfileID = WAVEPrimary;
    else if (!H_265.empty())            retHighProfileID = H_265;

    // estimate low profile
    // 1. Live4NVR -> Low4SSM -> WAVESecodary -> MOBILE
    if (!Live4NVR.empty())              retLowProfileID = Live4NVR;
    else if (!Low4SSM.empty())          retLowProfileID = Low4SSM;
    else if (!WAVESecondary.empty())    retLowProfileID = WAVESecondary;
    else if (!MOBILE.empty())           retLowProfileID = MOBILE;

    // if there is no predefined high profile, set higest resolution
    if (retHighProfileID.empty())
        retHighProfileID = highestRes;

    // if there is no predefined low profile, set lowest resolution
    if (retLowProfileID.empty())
        retLowProfileID = lowestRes;
}

inline void AssignChannelProfileInfo(const Device::DeviceProfileInfo& deviceProfileInfo,
                                     const Device::DeviceStatus& deviceStatus,
                                     Device::Device& device)
{
    for (auto& channel : device.channels) {

        auto channelItr = deviceStatus.channelsStatus.find(channel.first);
        if(channelItr != deviceStatus.channelsStatus.end()){
            if(channelItr->second.isGood())
                channel.second.use = true;
            auto itr = deviceProfileInfo.videoProfiles.find(channel.first);
            std::string highP = "1", lowP = "3";

            if (itr != deviceProfileInfo.videoProfiles.end()) {
                auto& profiles = itr->second;
                CalculateLowHighLiveProfile(device.deviceType,
                                            profiles,
                                            highP, lowP,
                                            channel.second.highResolution, channel.second.lowResolution,
                                            channel.second.highFramerate, channel.second.lowFramerate,
                                            channel.second.highCodecType, channel.second.lowCodecType);
            }

            channel.second.highProfile = highP;
            channel.second.lowProfile = lowP;
            channel.second.recordingProfile = highP;

            channel.second.recordingResolution = channel.second.highResolution;
            channel.second.recordingFramerate = channel.second.highFramerate;
            channel.second.recordingCodecType = channel.second.highCodecType;

            SPDLOG_DEBUG("SET PROFILE :: Channel={}, activate={} highP={} - {}:{}, lowP={} - {}:{}",
                         channel.first, channel.second.use,
                         highP, channel.second.highResolution.width, channel.second.highResolution.width,
                         lowP, channel.second.lowResolution.width, channel.second.lowResolution.width);
        }
    }
}

inline int numDigits(size_t number)
{
    int digits = 0;
    if (number < 0)
        digits = 1;
    while (number) {
        number /= 10;
        digits++;
    }
    return digits;
}

inline std::string fillDigits(size_t number, std::string id)
{
    int digits = Wisenet::Core::numDigits(number);

    std::stringstream ss;

    ss << std::setw(digits) << std::setfill('0') << id;

    return ss.str();
}

inline int random(int min, int max) {
    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> gen(min, max);
    int a = gen(rng);
    return a;
}

}
}
