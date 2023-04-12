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
#include "SunapiDeviceClientLogSettings.h"
#include "cgis/SystemCgi.h"
#include "cgis/NetworkCgi.h"
#include "cgis/MediaCgi.h"
#include "cgis/AttributesCgi.h"
#include "cgis/RecordingCgi.h"

namespace Wisenet
{
namespace Device
{

const unsigned int CONFIG_URI_CHANGED_NONE                              = 0x00000000;
const unsigned int CONFIG_URI_CHANGED_SYSTEM_DATE                       = 0x00000001 << 0;
const unsigned int CONFIG_URI_CHANGED_MEDIA_VIDEOPROFILE                = 0x00000001 << 1;
const unsigned int CONFIG_URI_CHANGED_MEDIA_VIDEOSOURCE					= 0x00000001 << 2;
const unsigned int CONFIG_URI_CHANGED_MEDIA_VIDEOPROFILEPOLICY          = 0x00000001 << 3;
//const unsigned int CONFIG_URI_CHANGED_MEDIA_AUDIOINPUT                  = 0x00000001 << 4;
const unsigned int CONFIG_URI_CHANGED_SYSTEM_DEVICEINFO					= 0x00000001 << 5;
const unsigned int CONFIG_URI_CHANGED_RECORDING_GENERAL					= 0x00000001 << 6;
//const unsigned int CONFIG_URI_CHANGED_MEDIA_AUDIOOUTPUT					= 0x00000001 << 7;
const unsigned int CONFIG_URI_CHANGED_SECURITY_USERGROUPS               = 0x00000001 << 8;
const unsigned int CONFIG_URI_CHANGED_SECURITY_USERS                    = 0x00000001 << 9;
const unsigned int CONFIG_URI_CHANGED_SECURITY_AUTHORITY                = 0x00000001 << 10;
const unsigned int CONFIG_URI_CHANGED_SECURITY_POSCONF                  = 0x00000001 << 11;
//const unsigned int CONFIG_URI_CHANGED_ALARM_INPUT                       = 0x00000001 << 12;
const unsigned int CONFIG_URI_CHANGED_RECORDING_STORAGE					= 0x00000001 << 14;
//const unsigned int CONFIG_URI_CHANGED_NETWORK_ALARM_INPUT               = 0x00000001 << 15;
//const unsigned int CONFIG_URI_CHANGED_IO_PORT                           = 0x00000001 << 16;
const unsigned int CONFIG_URI_CHANGED_ATTRIBUTE                         = 0x00000001 << 17;
const unsigned int CONFIG_URI_EVENT_RESTART                             = 0x00000001 << 18;
const unsigned int CONFIG_URI_NETWORK_RTSP                              = 0x00000001 << 19;

const unsigned int CONFIG_URI_FULL_INFO =
        CONFIG_URI_CHANGED_ATTRIBUTE |
        CONFIG_URI_CHANGED_MEDIA_VIDEOPROFILE |
        CONFIG_URI_CHANGED_MEDIA_VIDEOSOURCE |
        CONFIG_URI_CHANGED_MEDIA_VIDEOPROFILEPOLICY |
        CONFIG_URI_CHANGED_RECORDING_GENERAL;

/*
 * 현재는 카메라만 지원
 * Attribute 호출후, 이벤트 재시작해야 하는 케이스 (OpenSDK 등)
 */
const unsigned int CONFIG_URI_FULL_AND_RESTART =
        CONFIG_URI_FULL_INFO |
        CONFIG_URI_EVENT_RESTART;


static std::map<std::string, unsigned int> create_map()
{
    std::map<std::string, unsigned int> m;
    m["media.cgi?msubmenu=videoprofile"] = CONFIG_URI_CHANGED_MEDIA_VIDEOPROFILE;
    m["media.cgi?msubmenu=videosource"] = CONFIG_URI_FULL_INFO; // NVR에 카메라 등록시 attributes.cgi 호출해서 cap 확인 필요.
    m["media.cgi?msubmenu=videoprofilepolicy"] = CONFIG_URI_CHANGED_MEDIA_VIDEOPROFILEPOLICY;
    m["recording.cgi?msubmenu=manualrecording"] = CONFIG_URI_CHANGED_RECORDING_GENERAL;
    m["security.cgi?msubmenu=usergroups"] = CONFIG_URI_FULL_INFO;
    m["security.cgi?msubmenu=users"] = CONFIG_URI_FULL_INFO;
    m["security.cgi?msubmenu=authority"] = CONFIG_URI_FULL_INFO;
    m["recording.cgi?msubmenu=storage"] = CONFIG_URI_CHANGED_RECORDING_STORAGE;
    m["network.cgi?msubmenu=rtsp"] = CONFIG_URI_NETWORK_RTSP;
    m["system.cgi?msubmenu=date"] = CONFIG_URI_CHANGED_SYSTEM_DATE;
//    m["media.cgi?msubmenu=audioinput"] = CONFIG_URI_CHANGED_MEDIA_AUDIOINPUT;
//    m["media.cgi?msubmenu=audiooutput"] = CONFIG_URI_CHANGED_MEDIA_AUDIOOUTPUT;

    // general과 deviceinfo는 현재는 사용하지 않는다. (2021-12-02)
    // 추후 추가 검토 (이벤트가 너무 자주 날아오고 있음
//    m["system.cgi?msubmenu=deviceinfo"] = CONFIG_URI_CHANGED_SYSTEM_DEVICEINFO;
//    m["recording.cgi?msubmenu=general"] = CONFIG_URI_CHANGED_RECORDING_GENERAL;

    m["recording.cgi?msubmenu=posconf"] = CONFIG_URI_CHANGED_SECURITY_POSCONF;
//    m["eventsources.cgi?msubmenu=alarminput"] = CONFIG_URI_CHANGED_ALARM_INPUT;
//    m["eventsources.cgi?msubmenu=networkalarminput"] = CONFIG_URI_CHANGED_NETWORK_ALARM_INPUT;
//    m["io.cgi?msubmenu=ioport"] = CONFIG_URI_CHANGED_IO_PORT;
//    m["AttributeUpdate"] = CONFIG_URI_FULL_AND_RESTART;
    return m;
}
std::map<std::string, unsigned int> g_configUriMap = create_map();

void SunapiDeviceClientImpl::changeConfigUri(const std::string& cgiQuery)
{
    SLOG_DEBUG("[CONFIG] changeConfigUri : " + cgiQuery);
    if (!m_isCompletedConnection) {
        SPDLOG_DEBUG("[CONFIG] device connection is not completed, discard changeConfigUri, configUri : ", cgiQuery);
        return;
    }

    auto it = g_configUriMap.find(cgiQuery);
    if (it == g_configUriMap.end()) {
        return;
    }
    m_lastChangeConfigUriCap |= it->second;
    StartChangeConfigUriTimer();
}

void SunapiDeviceClientImpl::StartChangeConfigUriTimer(long long durationMsec)
{
    m_changeConfigUriTimer.cancel();
    m_changeConfigUriTimer.expires_after(std::chrono::milliseconds(durationMsec));
    m_changeConfigUriTimer.async_wait( WeakCallback(shared_from_this(),
                                              [this](const boost::system::error_code& ec) {
        if (ec) {
            return;
        }

        boost::asio::post(m_strand, WeakCallback(shared_from_this(), [this]() {
            SLOG_DEBUG("[CONFIG] START REBUILD OBJECT INFORMATION >>");
            ConfigUriComposerPtr cuc = std::make_shared<ConfigUriComposer>();
            dispatchConfigUri(cuc, m_lastChangeConfigUriCap);
            m_lastChangeConfigUriCap = 0;
        }));
    }));
}

void SunapiDeviceClientImpl::StopChangeConfigUriTimer()
{
    asio::dispatch(m_strand, WeakCallback(shared_from_this(), [this]()
    {
        m_changeConfigUriTimer.cancel();
        m_lastChangeConfigUriCap = 0;
    }));
}


void SunapiDeviceClientImpl::dispatchConfigUri(const ConfigUriComposerPtr& cuc, const unsigned int configCap)
{
    SLOG_DEBUG("[CONFIG] REBUILD OBJECT INFORMATION, cap={}", configCap);

    if (configCap & CONFIG_URI_CHANGED_ATTRIBUTE) {
        SLOG_DEBUG("[CONFIG] CONFIG_URI_CHANGED_ATTRIBUTE");
        auto newCap = configCap & ~CONFIG_URI_CHANGED_ATTRIBUTE;
        reloadAttribute(cuc, newCap);
        return;
    }
    if (configCap & CONFIG_URI_CHANGED_SYSTEM_DATE) {
        SLOG_DEBUG("[CONFIG] CONFIG_URI_CHANGED_SYSTEM_DATE");
        auto newCap = configCap & ~CONFIG_URI_CHANGED_SYSTEM_DATE;
        reloadSystemDate(cuc, newCap);
        return;
    }
    if (configCap & CONFIG_URI_CHANGED_RECORDING_GENERAL) {
        SLOG_DEBUG("[CONFIG] CONFIG_URI_CHANGED_RECORDING_GENERAL");
        auto newCap = configCap & ~CONFIG_URI_CHANGED_RECORDING_GENERAL;
        reloadRecordingGeneral(cuc, newCap);
        return;
    }

    if (configCap & CONFIG_URI_CHANGED_MEDIA_VIDEOPROFILE) {
        SLOG_DEBUG("[CONFIG] CONFIG_URI_CHANGED_MEDIA_VIDEOPROFILE");
        auto newCap = configCap & ~CONFIG_URI_CHANGED_MEDIA_VIDEOPROFILE;
        reloadMediaVideoProfile(cuc, newCap);
        return;
    }

    if (configCap & CONFIG_URI_CHANGED_MEDIA_VIDEOSOURCE) {
        SLOG_DEBUG("[CONFIG] CONFIG_URI_CHANGED_MEDIA_VIDEOSOURCE");
        auto newCap = configCap & ~CONFIG_URI_CHANGED_MEDIA_VIDEOSOURCE;
        reloadMediaVideoSource(cuc, newCap);
        return;
    }
    if (configCap & CONFIG_URI_CHANGED_MEDIA_VIDEOPROFILEPOLICY) {
        SLOG_DEBUG("[CONFIG] CONFIG_URI_CHANGED_MEDIA_VIDEOPROFILEPOLICY");
        auto newCap = configCap & ~CONFIG_URI_CHANGED_MEDIA_VIDEOPROFILEPOLICY;
        reloadMediaVideoProfilePolicy(cuc, newCap);
        return;
    }
    if (configCap & CONFIG_URI_CHANGED_RECORDING_STORAGE) {
        SLOG_DEBUG("[CONFIG] CONFIG_URI_CHANGED_RECORDING_STORAGE");
        auto newCap = configCap & ~CONFIG_URI_CHANGED_RECORDING_STORAGE;
        reloadRecordingStorage(cuc, newCap);
        return;
    }
    if (configCap & CONFIG_URI_NETWORK_RTSP) {
        SLOG_DEBUG("[CONFIG] CONFIG_URI_NETWORK_RTSP");
        auto newCap = configCap & ~CONFIG_URI_NETWORK_RTSP;
        reloadNetworkRtsp(cuc, newCap);
        return;
    }

    if (configCap != CONFIG_URI_CHANGED_NONE) {
        SLOG_DEBUG("[CONFIG] discard unknown config uri, {}", configCap);
    }

    // RTSP 포트 변경시에는 장비 재접속 루틴을 탄다.
    if (cuc->m_networkRtspView.has_value()) {
        SLOG_DEBUG("[CONFIG] DEVICE RTSP PORT CHANGED EVENT, RECONNECT DEVICE");
        disconnect(DeviceStatusType::DisconnectedWithError);
        return;
    }

    if (cuc->m_setObjectComposeFlag) {
        SLOG_DEBUG("[CONFIG] START REBUILD OBJECT COMPOSER");
        DeviceObjectComposerEvent composerEvent = m_repos->rebuildDeviceWithConfigUri(cuc);
        if (composerEvent.deviceStatusEvent) {
            SLOG_DEBUG("[CONFIG] NEW DEVICE STATUS EVENT");
            eventHandler(composerEvent.deviceStatusEvent);
        }
        else {
            if (composerEvent.channelStatusEvent) {
                SLOG_DEBUG("[CONFIG] NEW CHANNELS STATUS EVENT");
                eventHandler(composerEvent.channelStatusEvent);
            }
            if (composerEvent.profileInfoEvent) {
                SLOG_DEBUG("[CONFIG] NEW PROFILE INFO EVENT");
                eventHandler(composerEvent.profileInfoEvent);
            }
        }
    }

}

void SunapiDeviceClientImpl::reloadAttribute(const ConfigUriComposerPtr &cuc, const unsigned int configCap)
{
    SLOG_DEBUG("[CONFIG] reloadAttribute()");
    auto response = std::make_shared<Wisenet::ResponseBase>();
    auto attributesParser = std::make_shared<AttributesParser>(m_logPrefix, response);

    asyncRequest(m_httpCommandSession,
                 attributesParser,
                 [this, configCap, cuc](const ResponseBaseSharedPtr& responseBase) {
        boost::ignore_unused(responseBase);
        SLOG_DEBUG("[CONFIG] AttributesParser failed");
        dispatchConfigUri(cuc, configCap);

    },
    [this, configCap, cuc, attributesParser](){
         cuc->setAttributes(attributesParser->parseResult);
         dispatchConfigUri(cuc, configCap);
    }, AsyncRequestType::HTTPGET, false);
}


void SunapiDeviceClientImpl::reloadSystemDate(const ConfigUriComposerPtr& cuc, const unsigned int configCap)
{
    SLOG_DEBUG("[CONFIG] reloadSystemDate()");
    auto systemDataView = std::make_shared<SystemDateView>(m_logPrefix);
    asyncRequest(m_httpCommandSession,
                 systemDataView,
                 [this, configCap, cuc](const ResponseBaseSharedPtr& responseBase) {
        boost::ignore_unused(responseBase);
        SLOG_DEBUG("[CONFIG] SystemDateView failed");
        dispatchConfigUri(cuc, configCap);

    },
    [this, configCap, cuc, systemDataView](){
         cuc->setSystemDateView(systemDataView->parseResult);
         dispatchConfigUri(cuc, configCap);
    }, AsyncRequestType::HTTPGET, false);
}

void SunapiDeviceClientImpl::reloadRecordingGeneral(const ConfigUriComposerPtr& cuc, const unsigned int configCap)
{
    SLOG_DEBUG("[CONFIG] reloadRecordingGeneral()");

    auto response = std::make_shared<Wisenet::ResponseBase>();
    auto recordingGeneralView =
            std::make_shared<RecordingGeneralView>(m_logPrefix, response);
    asyncRequest(m_httpCommandSession,
                 recordingGeneralView,
                 [this, configCap, cuc](const ResponseBaseSharedPtr& responseBase) {
        boost::ignore_unused(responseBase);
        SLOG_DEBUG("[CONFIG] RecordingGeneralView failed");
        dispatchConfigUri(cuc, configCap);
    },
    [this, recordingGeneralView, configCap, cuc](){
        cuc->setRecordingGeneralView(recordingGeneralView->parseResult);
        dispatchConfigUri(cuc, configCap);
    }, AsyncRequestType::HTTPGET, false);
}

void SunapiDeviceClientImpl::reloadMediaVideoProfile(const ConfigUriComposerPtr& cuc, const unsigned int configCap)
{
    SLOG_DEBUG("[CONFIG] reloadMediaVideoProfile()");

    auto response = std::make_shared<Wisenet::ResponseBase>();
    auto mediaVideoProfileView = std::make_shared<MediaVideoProfileView>(m_logPrefix, response);
    asyncRequest(m_httpCommandSession,
                 mediaVideoProfileView,
                 [this, configCap, cuc](const ResponseBaseSharedPtr& responseBase) {
        boost::ignore_unused(responseBase);
        SLOG_DEBUG("[CONFIG] MediaVideoProfileView failed");
        dispatchConfigUri(cuc, configCap);
    },
    [this, mediaVideoProfileView, configCap, cuc](){
        cuc->setMediaVideoProfileView(mediaVideoProfileView->parseResult);
        dispatchConfigUri(cuc, configCap);
    }, AsyncRequestType::HTTPGET, false);
}

void SunapiDeviceClientImpl::reloadMediaVideoSource(const ConfigUriComposerPtr& cuc, const unsigned int configCap)
{
    SLOG_DEBUG("[CONFIG] reloadMediaVideoSource()");

    auto mediaVideoSourceView = std::make_shared<MediaVideoSourceView>(m_logPrefix);
    asyncRequest(m_httpCommandSession,
                 mediaVideoSourceView,
                 [this, configCap, cuc](const ResponseBaseSharedPtr& responseBase) {
        boost::ignore_unused(responseBase);
        SLOG_DEBUG("[CONFIG] MediaVideoSourceView failed");
        dispatchConfigUri(cuc, configCap);
    },
    [this, mediaVideoSourceView, configCap, cuc](){
        cuc->setMediaVideoSourceView(mediaVideoSourceView->parseResult);
        dispatchConfigUri(cuc, configCap);
    }, AsyncRequestType::HTTPGET, false);
}

void SunapiDeviceClientImpl::reloadMediaVideoProfilePolicy(const ConfigUriComposerPtr& cuc, const unsigned int configCap)
{
    SLOG_DEBUG("[CONFIG] reloadMediaVideoProfilePolicy()");
    auto response = std::make_shared<Wisenet::ResponseBase>();
    auto mediaVideoProfilePolicyView = std::make_shared<MediaVideoProfilePolicyView>(m_logPrefix, response);
    asyncRequest(m_httpCommandSession,
                 mediaVideoProfilePolicyView,
                 [this, configCap, cuc](const ResponseBaseSharedPtr& responseBase) {
        boost::ignore_unused(responseBase);
        SLOG_DEBUG("[CONFIG] MediaVideoProfilePolicyView failed");
        dispatchConfigUri(cuc, configCap);
    },
    [this, mediaVideoProfilePolicyView, configCap, cuc](){
        cuc->setMediaVideoProfilePolicyView(mediaVideoProfilePolicyView->parseResult);
        dispatchConfigUri(cuc, configCap);
    }, AsyncRequestType::HTTPGET, false);
}

void SunapiDeviceClientImpl::reloadRecordingStorage(const ConfigUriComposerPtr& cuc, const unsigned int configCap)
{
    SLOG_DEBUG("[CONFIG] reloadRecordingStorage()");
    auto response = std::make_shared<Wisenet::ResponseBase>();
    auto recordingStorageView = std::make_shared<RecordingStorageView>(m_logPrefix, response);

    asyncRequest(m_httpCommandSession,
                 recordingStorageView,
                 [this, configCap, cuc](const ResponseBaseSharedPtr& responseBase) {
        boost::ignore_unused(responseBase);
        SLOG_DEBUG("[CONFIG] RecordingStorageView failed");
        dispatchConfigUri(cuc, configCap);
    },
    [this, recordingStorageView, configCap, cuc](){
        cuc->setRecordingStorageView(recordingStorageView->parseResult);
        dispatchConfigUri(cuc, configCap);
    }, AsyncRequestType::HTTPGET, false);
}

void SunapiDeviceClientImpl::reloadNetworkRtsp(const ConfigUriComposerPtr &cuc, const unsigned int configCap)
{
    SLOG_DEBUG("[CONFIG] reloadNetworkRtsp()");
    auto networkRtspView = std::make_shared<NetworkRtspView>(m_logPrefix);

    asyncRequest(m_httpCommandSession,
                 networkRtspView,
                 [this, configCap, cuc](const ResponseBaseSharedPtr& responseBase) {
        boost::ignore_unused(responseBase);
        SLOG_DEBUG("[CONFIG] NetworkRtspView failed");
        dispatchConfigUri(cuc, configCap);
    },
    [this, networkRtspView, configCap, cuc](){
        cuc->setNetworkRtspView(networkRtspView->parseResult);
        dispatchConfigUri(cuc, configCap);
    }, AsyncRequestType::HTTPGET, false);
}


}
}
