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
#include "SunapiDeviceClientImpl.h"



namespace Wisenet
{
namespace Device
{

// Check if exist "Live4NVR" or "Low4SSM"
bool existLowProfile(
        const std::map<int, MediaVideoProfileViewResult::VideoProfileInfo>& profileList)
{
    for(auto& kv : profileList) {
        if (kv.second.name == LIVE4NVR_PROFILE || kv.second.name == LOW4SSM_PROFILE)
            return true;
    }
    return false;
}

// calculate profile param
VideoProfileAddParam createProfileParam(
        const std::shared_ptr<MediaVideoProfileView>& videoProfile,
        const std::shared_ptr<MediaVideoCodecInfoView>& videoCodecInfo,
        const int channelIndex)
{
    auto& channelProfile = videoProfile->parseResult.channels[channelIndex];
    if (channelProfile.size() == 0)
        return VideoProfileAddParam();

    // find default profile (H.264 name or H264 first profile)
    MediaVideoProfileViewResult::VideoProfileInfo &baseProfile = channelProfile.begin()->second;
    for (auto& profile : channelProfile) {
        MediaVideoProfileViewResult::VideoProfileInfo& profileInfo = profile.second;
        // 1. if default H264 profile exist
        if (profileInfo.encodingType == "H264") {
            baseProfile = profileInfo;
            // 2. H.264 name profile has highest priority
            if (profileInfo.name == "H.264") {
                break;
            }
        }
    }

    if (baseProfile.resolution.width == 0 || baseProfile.resolution.height == 0) {
        SPDLOG_DEBUG("invalid base profile information");
        return VideoProfileAddParam();
    }
    if (videoCodecInfo->parseResult.resolutions["H264"].empty()) {
        SPDLOG_DEBUG("no H264 codec info");
        return VideoProfileAddParam();
    }

    SPDLOG_DEBUG("Base profile, name={}, resolution={}x{}",
                 baseProfile.name,
                 baseProfile.resolution.width,
                 baseProfile.resolution.height);

    // get h264, general codec info
    std::unordered_map<std::string, Resolution> &rs =
            videoCodecInfo->parseResult.resolutions["H264"].begin()->second;
    if (rs.size() == 0) {
        SPDLOG_DEBUG("No H264 Overview profile");
        return VideoProfileAddParam();
    }

    Resolution targetResolution;
    bool isLandscape = baseProfile.resolution.width > baseProfile.resolution.height;
    if (isLandscape) {
        targetResolution.height = 640 * baseProfile.resolution.height / baseProfile.resolution.width;
        targetResolution.width = 640;
    }
    else {
        targetResolution.width = 640 * baseProfile.resolution.width / baseProfile.resolution.height;
        targetResolution.height = 640;
    }

    // check if resolution exist in codecinfo
    for (auto& res : rs) {
        if (res.second.width == targetResolution.width &&
                res.second.height == targetResolution.height) {
            VideoProfileAddParam addParam;
            addParam.resolution = targetResolution;
            SPDLOG_DEBUG("Find target profile exactly");
            return addParam;
        }
    }

    // if not exist, return next param in codecinfo
    for (auto& res : rs) {
        if (res.second.width <= targetResolution.width &&
                res.second.height <= targetResolution.height) {
            VideoProfileAddParam addParam;
            addParam.resolution = res.second;
            SPDLOG_DEBUG("Find target profile alternative");
            return addParam;
        }
    }

    SPDLOG_DEBUG("Find target profile default");
    return VideoProfileAddParam();
}

struct CreateProfileJob
{
    std::list<int> channels;
};


void SunapiDeviceClientImpl::createVideoProfile(const DeviceObjectComposerPtr& oc,
                                                const ResponseBaseHandler& responseHandler)
{
    auto mediaVideoProfileView = std::make_shared<MediaVideoProfileView>(
                m_logPrefix,
                std::make_shared<DeviceConnectResponse>(), true, true);

    asyncRequest(m_httpCommandSession,
                 mediaVideoProfileView,
                 responseHandler,
                 [this, oc, mediaVideoProfileView, responseHandler](){

        // create job
        std::shared_ptr<CreateProfileJob> profileJob = std::make_shared<CreateProfileJob>();

        for (auto& kv : mediaVideoProfileView->parseResult.channels) {
            // check profile existance
            if (existLowProfile(kv.second)) {
                SLOG_INFO("Live4NVR or Low4SSM already exist, skip channel={}", kv.first);
                continue;
            }
            profileJob->channels.push_back(kv.first);
        }

        if (profileJob->channels.size() == 0) {
            // go to final
            SLOG_INFO("Can not create video profile");
            createVideoProfileFinal(oc, responseHandler);
            return;
        }

        CreateProfileJob copyJob = *profileJob;

        // get video codec info for channels
        for (auto& channelIndex : copyJob.channels) {

            auto mediaVideoCodecInfoView = std::make_shared<MediaVideoCodecInfoView>(
                        m_logPrefix,
                        std::make_shared<DeviceConnectResponse>(),
                        channelIndex,
                        true, true);

            // get video codec info
            asyncRequest(m_httpCommandSession,
                         mediaVideoCodecInfoView,
                         responseHandler,
                         [this, oc, channelIndex,
                         mediaVideoProfileView, mediaVideoCodecInfoView, profileJob,
                         responseHandler](){

                auto addParam = createProfileParam(mediaVideoProfileView, mediaVideoCodecInfoView, channelIndex);
                auto mediaVideoProfileAdd = std::make_shared<MediaVideoProfileAdd>(
                            m_logPrefix,
                            std::make_shared<DeviceConnectResponse>(),
                            addParam,
                            channelIndex,
                            true, true);

                // add video profile "Live4NVR"
                SLOG_INFO("create video profile, resolution={}x{}", addParam.resolution.width, addParam.resolution.height);
                asyncRequest(m_httpCommandSession,
                             mediaVideoProfileAdd,
                             responseHandler,
                             [this, oc, channelIndex, mediaVideoProfileAdd, profileJob, responseHandler](){

                    // remove a job from job list
                    profileJob->channels.remove(channelIndex);
                    if (profileJob->channels.size() == 0) {
                        // goto final
                        createVideoProfileFinal(oc, responseHandler);
                        return;
                    }
                }, AsyncRequestType::HTTPGET, true);

            }, AsyncRequestType::HTTPGET, true);

        }

    }, AsyncRequestType::HTTPGET, true);
}


void SunapiDeviceClientImpl::createVideoProfileFinal(const DeviceObjectComposerPtr& oc,
                                                     const ResponseBaseHandler& responseHandler)
{
    getSystemDate(oc, responseHandler);
}

}
}
