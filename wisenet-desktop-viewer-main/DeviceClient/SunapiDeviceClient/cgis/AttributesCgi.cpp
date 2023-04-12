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
#include "AttributesCgi.h"
#include "SunapiDeviceClientLogSettings.h"
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string.hpp>
#include <sstream>
#include <iostream>

namespace Wisenet
{
namespace Device
{

static pugi::xml_attribute attributeValue(const pugi::xml_node& node, const std::string& name)
{
    auto attr = node.find_child_by_attribute("attribute", "name", name.c_str());
    if (attr) {
        return attr.attribute("value");
    }
    return pugi::xml_attribute();
}


bool AttributesParser::parseXml(pugi::xml_document &xmlDoc)
{
    auto attributes = xmlDoc.child("capabilities").child("attributes");
    if (!attributes) {
        SLOG_DEBUG("AttributesParser::parseXml() can not find attributes");
        return false;
    }

    auto systemGroup = attributes.find_child_by_attribute("group", "name", "System");
    auto mediaGroup = attributes.find_child_by_attribute("group", "name", "Media");
    auto imageGroup = attributes.find_child_by_attribute("group", "name", "Image");
    auto ioGroup = attributes.find_child_by_attribute("group", "name", "IO");
    auto ptzSupportGroup = attributes.find_child_by_attribute("group", "name", "PTZSupport");
    auto recordingGroup = attributes.find_child_by_attribute("group", "name", "Recording");
    auto eventSourceGroup = attributes.find_child_by_attribute("group", "name", "Eventsource");

    // group/System
    if (systemGroup) {
        auto support = systemGroup.find_child_by_attribute("category", "name", "Support");
        if (support) {
            parseResult.systemAttribute.fwUpdate = attributeValue(support, "FWUpdate").as_bool(false);
            parseResult.systemAttribute.configBackup = attributeValue(support, "ConfigBackup").as_bool(false);
            parseResult.systemAttribute.configRestore = attributeValue(support, "ConfigRestore").as_bool(false);
            parseResult.systemAttribute.shutdown = attributeValue(support, "Shutdown").as_bool(false);
            parseResult.systemAttribute.aiFeature = attributeValue(support, "AIFeatures").as_bool(false);

            std::string quickPlayCleintType = attributeValue(support, "QuickPlayClientType").as_string("");
            parseResult.systemAttribute.quickPlay = ((quickPlayCleintType == "")?false:true);
        }
        auto limit = systemGroup.find_child_by_attribute("category", "name", "Limit");
        if (limit) {
            parseResult.systemAttribute.maxChannel = attributeValue(limit, "MaxChannel").as_int(1);
            int channelExpansionLimit = attributeValue(limit, "ChannelExpansionLimit").as_int(0);
            if (parseResult.systemAttribute.maxChannel < channelExpansionLimit)
                parseResult.systemAttribute.maxChannel = channelExpansionLimit;
            parseResult.systemAttribute.maxPOS = attributeValue(limit, "MaxPOS").as_int(0);
        }
    }

    // group/Media
    if (mediaGroup) {
        auto support = mediaGroup.find_child_by_attribute("category", "name", "Support");
        auto limit = mediaGroup.find_child_by_attribute("category", "name", "Limit");

        for (pugi::xml_node ch = support.child("channel") ; ch; ch = ch.next_sibling("channel")) {
            auto attr = ch.attribute("number");
            if (attr) {
                int channelIndex = attr.as_int();
                AttributesResult::MediaAttribute::Media mediaAttribute;
                mediaAttribute.channelAudioOut = attributeValue(ch, "ChannelAudioOutput").as_bool(false);
                mediaAttribute.audioIn = attributeValue(ch, "AudioIn").as_bool(false);
                mediaAttribute.videoSetting = attributeValue(ch, "VideoSetting").as_bool(false);
                mediaAttribute.live = attributeValue(ch, "Live").as_bool(false);
                mediaAttribute.metadata = attributeValue(ch, "Stream.Metadata").as_bool(false);
                parseResult.mediaAttribute.channels[channelIndex] = mediaAttribute;
            }
        }
        for (pugi::xml_node ch = limit.child("channel") ; ch; ch = ch.next_sibling("channel")) {
            auto attr = ch.attribute("number");
            if (attr) {
                int channelIndex = attr.as_int();
                int maxProfile = attributeValue(ch, "MaxProfile").as_int(1);
                if (parseResult.mediaAttribute.channels.find(channelIndex) != parseResult.mediaAttribute.channels.end()) {
                    parseResult.mediaAttribute.channels[channelIndex].maxProfile = maxProfile;
                }
            }
        }
    }

    // group/Image
    if (imageGroup) {
        auto support = imageGroup.find_child_by_attribute("category", "name", "Support");

        for (pugi::xml_node ch = support.child("channel") ; ch; ch = ch.next_sibling("channel")) {
            auto attr = ch.attribute("number");
            if (attr) {
                int channelIndex = attr.as_int();
                AttributesResult::ImageAttribute::Image imageAttribute;
                imageAttribute.defog = attributeValue(ch, "Defog").as_bool(false);
                imageAttribute.simpleFocus = attributeValue(ch, "SimpleFocus").as_bool(false);
                imageAttribute.resetFocus = attributeValue(ch, "ResetFocus").as_bool(false);
                imageAttribute.autoFocus = attributeValue(ch, "AutoFocus").as_bool(false);
                imageAttribute.iRLED = attributeValue(ch, "IRLED").as_bool(false);
                imageAttribute.diS = attributeValue(ch, "DIS").as_bool(false);
                imageAttribute.piris = attributeValue(ch, "P-Iris").as_bool(false);
                imageAttribute.iris = attributeValue(ch, "Iris").as_bool(false);
                imageAttribute.dewarpView = attributeValue(ch, "DewarpedView").as_bool(false);
                imageAttribute.profileBasedDewarpedView = attributeValue(ch, "ProfileBasedDewarpedView").as_bool(false);
                imageAttribute.multiImager = attributeValue(ch, "MultiImager").as_bool(false);
                imageAttribute.thermalFeatures = attributeValue(ch, "ThermalFeatures").as_bool(false);

                parseResult.imageAttribute.channels[channelIndex] = imageAttribute;
            }
        }
    }

    // group/IO
    if (ioGroup) {
        auto support = ioGroup.find_child_by_attribute("category", "name", "Support");
        if (support) {
            parseResult.ioAttribute.alarmOutput = attributeValue(support, "AlarmOutput").as_bool(false);
            parseResult.ioAttribute.aux = attributeValue(support, "Aux").as_bool(false);
            parseResult.ioAttribute.configurableIO = attributeValue(support, "ConfigurableIO").as_bool(false);
            parseResult.ioAttribute.alarmReset = attributeValue(support, "AlarmReset").as_bool(false);
        }
        auto limit = ioGroup.find_child_by_attribute("category", "name", "Limit");
        if (limit) {
            parseResult.ioAttribute.maxAlarmOutput = attributeValue(limit, "MaxAlarmOutput").as_int(0);
            parseResult.ioAttribute.maxAux = attributeValue(limit, "MaxAux").as_int(0);
            parseResult.ioAttribute.maxConfigurableIO = attributeValue(limit, "MaxConfigurableIO").as_int(0);
        }
    }

    // group/PTZSupport
    if (ptzSupportGroup) {
        auto support = ptzSupportGroup.find_child_by_attribute("category", "name", "Support");
        auto limit = ptzSupportGroup.find_child_by_attribute("category", "name", "Limit");

        for (pugi::xml_node ch = support.child("channel") ; ch; ch = ch.next_sibling("channel")) {
            auto attr = ch.attribute("number");
            if (attr) {
                int channelIndex = attr.as_int();
                AttributesResult::PtzSupportAttribute::PtzSupport ptzSupportAttribute;
                ptzSupportAttribute.realPtz = attributeValue(ch, "RealPTZ").as_bool(false);
                ptzSupportAttribute.preset = attributeValue(ch, "Preset").as_bool(false);
                ptzSupportAttribute.swing = attributeValue(ch, "Swing").as_bool(false);
                ptzSupportAttribute.group = attributeValue(ch, "Group").as_bool(false);
                ptzSupportAttribute.tour = attributeValue(ch, "Tour").as_bool(false);
                ptzSupportAttribute.trace = attributeValue(ch, "Trace").as_bool(false);
                ptzSupportAttribute.home = attributeValue(ch, "Home").as_bool(false);
                ptzSupportAttribute.areaZoom = attributeValue(ch, "AreaZoom").as_bool(false);
                ptzSupportAttribute.continiousPan = attributeValue(ch, "Continuous.Pan").as_bool(false);
                ptzSupportAttribute.continiousTilt = attributeValue(ch, "Continuous.Tilt").as_bool(false);
                ptzSupportAttribute.continiousZoom = attributeValue(ch, "Continuous.Zoom").as_bool(false);
                ptzSupportAttribute.continiousFocus = attributeValue(ch, "Continuous.Focus").as_bool(false);
                ptzSupportAttribute.continiousIris = attributeValue(ch, "Continuous.Iris").as_bool(false);
                ptzSupportAttribute.panTiltOnly = attributeValue(ch, "PanTiltOnly").as_bool(false);
                ptzSupportAttribute.zoomOnly = attributeValue(ch, "ZoomOnly").as_bool(false);

                std::string auxCommands = attributeValue(ch, "AuxCommands").as_string("");
                if(auxCommands != "")
                    boost::split(ptzSupportAttribute.auxCommands, auxCommands, boost::is_any_of(","));

                parseResult.ptzSupportAttribute.channels[channelIndex] = ptzSupportAttribute;
            }
        }
        for (pugi::xml_node ch = limit.child("channel") ; ch; ch = ch.next_sibling("channel")) {
            auto attr = ch.attribute("number");
            if (attr) {
                int channelIndex = attr.as_int();
                int maxPreset = attributeValue(ch, "MaxPreset").as_int(0);
                int maxGroupCount = attributeValue(ch, "MaxGroupCount").as_int(0);

                if (parseResult.ptzSupportAttribute.channels.find(channelIndex) != parseResult.ptzSupportAttribute.channels.end()) {
                    parseResult.ptzSupportAttribute.channels[channelIndex].maxPreset = maxPreset;
                    parseResult.ptzSupportAttribute.channels[channelIndex].maxGroupCount = maxGroupCount;
                }
            }
        }
    }

    // group/Recording
    if (recordingGroup) {
        auto support = recordingGroup.find_child_by_attribute("category", "name", "Support");
        if (support) {
            parseResult.recordingAttribute.overlapped = attributeValue(support, "Overlapped").as_bool(false);
            parseResult.recordingAttribute.searchPeriod = attributeValue(support, "SearchPeriod").as_bool(false);
            parseResult.recordingAttribute.searchByUTCTime = attributeValue(support, "SearchByUTCTime").as_bool(false);
            parseResult.recordingAttribute.dualTrackRecording = attributeValue(support, "DualTrackRecording").as_bool(false);
            parseResult.recordingAttribute.manualRecordingStart = attributeValue(support, "ManualRecordingStart").as_bool(false);
            std::string playbackSpeed = attributeValue(support, "PlaybackSpeed").as_string("-1x,-2x,-4x,-8x,1x,2x,4x,8x");
            boost::split(parseResult.recordingAttribute.playbackSpeeds, playbackSpeed, boost::is_any_of(","));
        }
        auto limit = recordingGroup.find_child_by_attribute("category", "name", "Limit");
        if (limit) {
            parseResult.recordingAttribute.maxPlaybackChannels = attributeValue(limit, "MaxPlaybackChannels").as_int(16);
        }
        for (pugi::xml_node ch = support.child("channel") ; ch; ch = ch.next_sibling("channel")) {
            auto attr = ch.attribute("number");
            if (attr) {
                int channelIndex = attr.as_int();
                AttributesResult::RecordingAttribute::Channel channelSupport;
                channelSupport.backup = attributeValue(ch, "Backup").as_bool(false);
                channelSupport.searchCalendar = attributeValue(ch, "SearchCalendar").as_bool(false);
                channelSupport.searchTimeline = attributeValue(ch, "SearchTimeline").as_bool(false);
                channelSupport.searchEvent = attributeValue(ch, "SearchEvent").as_bool(false);
                channelSupport.smartSearch = attributeValue(ch, "SmartSearch").as_bool(false);
                std::string aiSearchType =  attributeValue(ch, "AISearchTypes").as_string("");
                std::vector<std::string> aiTokens;
                boost::split(aiTokens, aiSearchType, boost::is_any_of(","));

                for(auto& token : aiTokens){
                    if(token == "PersonSearch") channelSupport.personSearch = true;
                    if(token == "FaceSearch") channelSupport.faceSearch = true;
                    if(token == "VehicleSearch") channelSupport.vehicleSearch = true;
                    if(token == "OCRSearch") channelSupport.ocrSearch = true;
                }

                parseResult.recordingAttribute.channels[channelIndex] = channelSupport;
            }
        }
    }

    // group/Eventsource
    if (eventSourceGroup) {
        auto support = eventSourceGroup.find_child_by_attribute("category", "name", "Support");
        if (support) {
            parseResult.eventSourceAttribute.alarmInput = attributeValue(support, "AlarmInput").as_bool(false);
            parseResult.eventSourceAttribute.networkAlarmInput = attributeValue(support, "NetworkAlarmInput").as_bool(false);
        }
        auto limit = eventSourceGroup.find_child_by_attribute("category", "name", "Limit");
        if (limit) {
            parseResult.eventSourceAttribute.maxAlarmInput = attributeValue(limit, "MaxAlarmInput").as_int(1);
            parseResult.eventSourceAttribute.maxNetworkAlarmInput = attributeValue(limit, "MaxNetworkAlarmInput").as_int(0);
        }
        for (pugi::xml_node ch = support.child("channel") ; ch; ch = ch.next_sibling("channel")) {
            auto attr = ch.attribute("number");
            if (attr) {
                int channelIndex = attr.as_int();
                AttributesResult::EventSourceAttribute::Channel channelSupport;
                channelSupport.tracking= attributeValue(ch, "Tracking").as_bool(false);
                parseResult.eventSourceAttribute.channels[channelIndex] = channelSupport;
            }
        }
    }

    // cgis
    auto cgis = xmlDoc.child("capabilities").child("cgis");
    if (cgis) {
        auto systemCgi = cgis.find_child_by_attribute("cgi", "name", "system");
        auto securityCgi = cgis.find_child_by_attribute("cgi", "name", "security");
        auto eventStatusCgi = cgis.find_child_by_attribute("cgi", "name", "eventstatus");
        auto mediaCgi = cgis.find_child_by_attribute("cgi", "name", "media");
        auto ptzControlCgi = cgis.find_child_by_attribute("cgi", "name", "ptzcontrol");
        auto imageCgi = cgis.find_child_by_attribute("cgi", "name", "image");
        auto eventrules = cgis.find_child_by_attribute("cgi", "name", "eventrules");
        auto recordingCgi = cgis.find_child_by_attribute("cgi", "name", "recording");
        auto aiCgi = cgis.find_child_by_attribute("cgi", "name", "ai");
        auto videoCgi = cgis.find_child_by_attribute("cgi", "name", "video");

        if (systemCgi) {
            auto submenu = systemCgi.find_child_by_attribute("submenu", "name", "configbackup");
            if (submenu) {
                auto action = submenu.find_child_by_attribute("action", "name", "control");
                if (action) {
                    parseResult.systemCgi.configBackupEncrypt = false;
                    if (action.find_child_by_attribute("parameter", "name", "IsEncryptionKeyEncrypted"))
                        parseResult.systemCgi.configBackupEncrypt = true;

                }
            }
            submenu = systemCgi.find_child_by_attribute("submenu", "name", "configrestore");
            if (submenu) {
                auto action = submenu.find_child_by_attribute("action", "name", "control");
                if (action) {
                    parseResult.systemCgi.configRestoreDecrypt = false;
                    if (action.find_child_by_attribute("parameter", "name", "IsDecryptionKeyEncrypted"))
                        parseResult.systemCgi.configRestoreDecrypt = true;
                }
            }
        }

        if (securityCgi) {
            auto submenu = securityCgi.find_child_by_attribute("submenu", "name", "users");
            if (submenu) {
                auto action = submenu.find_child_by_attribute("action", "name", "add/update");
                if (action) {
                    parseResult.securityCgi.addUpdateUsers = true;
                    if (action.find_child_by_attribute("parameter", "name", "IsPasswordEncrypted"))
                        parseResult.securityCgi.IsPasswordEncrypted = true;
                    
                }
            }
        }

        if (imageCgi) {
            auto submenu = imageCgi.find_child_by_attribute("submenu", "name", "imageenhancements");
            if (submenu) {
                auto action = submenu.find_child_by_attribute("action", "name", "set");
                if (action) {
                    auto defogMode = action.find_child_by_attribute("parameter", "name", "DefogMode");
                    if (defogMode) {
                        auto senum = defogMode.child("dataType").child("enum");
                        for (auto& entry : senum.children("entry")){
                            parseResult.imageCgi.defogMode.push_back(entry.attribute("value").value());
                        }
                    }
                    auto defogLevel = action.find_child_by_attribute("parameter", "name", "DefogLevel");
                    if (defogLevel) {
                        parseResult.imageCgi.defogLevel.max = defogLevel.child("dataType").child("int").attribute("max").as_int(0);
                        parseResult.imageCgi.defogLevel.min = defogLevel.child("dataType").child("int").attribute("min").as_int(0);
                    }
                }
            }
        }
        

        if (eventStatusCgi) {
            auto submenu = eventStatusCgi.find_child_by_attribute("submenu", "name", "eventstatus");
            if (submenu) {
                auto action = submenu.find_child_by_attribute("action", "name", "monitordiff");
                if (action) {
                    if (action.find_child_by_attribute("parameter", "name", "IncludeTimestamp"))
                        parseResult.eventStatusCgi.monitorDiff.includeTimeStamp = true;
                    if (action.find_child_by_attribute("parameter", "name", "SchemaBased"))
                        parseResult.eventStatusCgi.monitorDiff.schemaBased = true;
                }
            }
        }
        if (mediaCgi) {
            if (mediaCgi.find_child_by_attribute("submenu", "name", "cameraregister"))
                parseResult.mediaCgi.cameraRegister = true;
            if (mediaCgi.find_child_by_attribute("submenu", "name", "audiooutput"))
                parseResult.mediaCgi.audioOutput = true;
            if (mediaCgi.find_child_by_attribute("submenu", "name", "sessionkey"))
                parseResult.mediaCgi.sessionKey = true;
        }
        if (ptzControlCgi) {
            auto submenu = ptzControlCgi.find_child_by_attribute("submenu", "name", "continuous");
            if (submenu) {
                auto action = submenu.find_child_by_attribute("action", "name", "control");
                if (action) {
                     if (action.find_child_by_attribute("parameter", "name", "NormalizedSpeed"))
                         parseResult.ptzControlCgi.continous.normalizedSpeed = true;

                     parseResult.ptzControlCgi.continous.panRange.max =
                             action.find_child_by_attribute("parameter", "name", "Pan").child("dataType").child("int").attribute("max").as_int(0);
                     parseResult.ptzControlCgi.continous.panRange.min =
                             action.find_child_by_attribute("parameter", "name", "Pan").child("dataType").child("int").attribute("min").as_int(0);
                     parseResult.ptzControlCgi.continous.tiltRange.max =
                             action.find_child_by_attribute("parameter", "name", "Tilt").child("dataType").child("int").attribute("max").as_int(0);
                     parseResult.ptzControlCgi.continous.tiltRange.min =
                             action.find_child_by_attribute("parameter", "name", "Tilt").child("dataType").child("int").attribute("min").as_int(0);
                     parseResult.ptzControlCgi.continous.zoomRange.max =
                             action.find_child_by_attribute("parameter", "name", "Zoom").child("dataType").child("int").attribute("max").as_int(0);
                     parseResult.ptzControlCgi.continous.zoomRange.min =
                             action.find_child_by_attribute("parameter", "name", "Zoom").child("dataType").child("int").attribute("min").as_int(0);
                }
            }
        }
        if(ptzControlCgi) {
            auto submenu = ptzControlCgi.find_child_by_attribute("submenu", "name", "areazoom");
            if (submenu) {
                auto action = submenu.find_child_by_attribute("action", "name", "control");
                if (action) {
                    if (action.find_child_by_attribute("parameter", "name", "Profile"))
                        parseResult.ptzControlCgi.areazoom.profile = true;
                }
            }
        }
        if(ptzControlCgi) {
            auto submenu = ptzControlCgi.find_child_by_attribute("submenu", "name", "digitalautotracking");
            if (submenu) {
                auto action = submenu.find_child_by_attribute("action", "name", "control");
                if (action) {
                    if (action.find_child_by_attribute("parameter", "name", "Profile"))
                        parseResult.ptzControlCgi.digitalAutoTracking.profile = true;
                }
            }
        }

        if (eventrules) {
            auto submenu = eventrules.find_child_by_attribute("submenu", "name", "dynamicrules");
            if (submenu) {
                auto action = submenu.find_child_by_attribute("action", "name", "view");
                if (action) {
                    auto parameter = action.find_child_by_attribute("parameter", "name", "Rule.#.EventSource.#.DynamicEventName");
                    if (parameter) {
                        auto dataType = parameter.child("dataType");
                        if (dataType) {
                            auto dataTypeEnum = dataType.child("enum");
                            if (dataTypeEnum) {
                                for (auto entry = dataTypeEnum.child("entry"); entry; entry = entry.next_sibling("entry")) {
                                    auto value = entry.attribute("value");

                                    if (value) {
                                        std::string eventName = value.as_string();
                                        parseResult.eventRuleCgi.dynamicEventName.emplace_back(eventName);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        if(recordingCgi) {
            auto submenu = recordingCgi.find_child_by_attribute("submenu", "name", "timeline");
            if (submenu) {
                auto action = submenu.find_child_by_attribute("action", "name", "view");
                if (action) {
                    auto parameter = action.find_child_by_attribute("parameter", "name", "Type");
                    auto enumList = parameter.child("dataType").child("enum");
                    for (auto& entry : enumList.children("entry")){
                        parseResult.recordingCgi.recordingTypes.push_back(convertRecordingType(entry.attribute("value").value()));
                    }

                }
            }
        }
        if(aiCgi){
            auto submenu = aiCgi.find_child_by_attribute("submenu", "name", "metaattributesearch");
            if (submenu) {
                auto action = submenu.find_child_by_attribute("action", "name", "view");
                if (action) {
                    auto parameter = action.find_child_by_attribute("parameter", "name", "MaxResults");
                    parseResult.aiCgi.metaAttributeSearchMaxResults =
                            parameter.child("dataType").child("int").attribute("max").as_int(0);
                }
            }
            submenu = aiCgi.find_child_by_attribute("submenu", "name", "ocrsearch");
            if (submenu) {
                auto action = submenu.find_child_by_attribute("action", "name", "view");
                if (action) {
                    auto parameter = action.find_child_by_attribute("parameter", "name", "MaxResults");
                    parseResult.aiCgi.ocrSearchMaxResults =
                            parameter.child("dataType").child("int").attribute("max").as_int(0);
                }
            }
        }
        if(videoCgi) {
            if (videoCgi.find_child_by_attribute("submenu", "name", "snapshot"))
                parseResult.videoCgi.snapshot = true;
            if (videoCgi.find_child_by_attribute("submenu", "name", "thumbnail"))
                parseResult.videoCgi.thumbnail = true;
        }
    }

    if (spdlog::should_log(spdlog::level::debug)) {
        std::ostringstream msg;
        msg << std::endl << std::endl;
        msg << "[SYSTEM SUPPORT/LIMIT]" << std::endl <<
               "fwUpdate=" << std::boolalpha << parseResult.systemAttribute.fwUpdate << ", "
               "configBackup=" << std::boolalpha << parseResult.systemAttribute.configBackup << ", "
               "configRestore=" << std::boolalpha << parseResult.systemAttribute.configRestore << ", "
               "shutdown=" << std::boolalpha << parseResult.systemAttribute.shutdown << ", "
               "aiFeature=" << std::boolalpha << parseResult.systemAttribute.aiFeature << ", "
               "maxChannel=" << parseResult.systemAttribute.maxChannel << ", "
               "channelExtensionLimit=" << parseResult.systemAttribute.channelExpansionLimit;

        msg << std::endl << std::endl;
        msg << "[IO SUPPORT/LIMIT]" << std::endl <<
               "alarmOutput=" << std::boolalpha << parseResult.ioAttribute.alarmOutput << ", "
               "aux=" << std::boolalpha << parseResult.ioAttribute.aux << ", "
               "configurableIO=" << std::boolalpha << parseResult.ioAttribute.configurableIO << ", "
               "alarmReset=" << std::boolalpha << parseResult.ioAttribute.alarmReset << ", "
               "maxAlarmOutput=" << parseResult.ioAttribute.maxAlarmOutput << ", "
               "maxAux=" << parseResult.ioAttribute.maxAux << ", "
               "maxConfigurableIO=" << parseResult.ioAttribute.maxConfigurableIO;

        msg << std::endl << std::endl;
        msg << "[EVENTSOURCE SUPPORT/LIMIT]" << std::endl <<
               "alarmInput=" << std::boolalpha << parseResult.eventSourceAttribute.alarmInput << ", "
               "networkAlarmInput=" << std::boolalpha << parseResult.eventSourceAttribute.networkAlarmInput << ", "
               "maxAlarmInput=" << parseResult.eventSourceAttribute.maxAlarmInput << ", "
               "maxNetworkAlarmInput=" << parseResult.eventSourceAttribute.maxNetworkAlarmInput;

        msg << std::endl << std::endl;
        msg << "[RECORDING SUPPORT/LIMIT]" << std::endl <<
               "overlapped=" << std::boolalpha << parseResult.recordingAttribute.overlapped << ", "
               "searchPeriod=" << std::boolalpha << parseResult.recordingAttribute.searchPeriod << ", "
               "searchByUTCTime=" << std::boolalpha << parseResult.recordingAttribute.searchByUTCTime << ", "
               "dualTrackRecording=" << std::boolalpha << parseResult.recordingAttribute.dualTrackRecording << ", "
               "maxPlaybackChannels=" << parseResult.recordingAttribute.maxPlaybackChannels << std::endl <<
               "playbackSpeeds=";

        for (auto& speed : parseResult.recordingAttribute.playbackSpeeds)
            msg << ":" << speed;

        for (auto& channel : parseResult.recordingAttribute.channels) {
            msg << std::endl;
            msg << "    channel[" << channel.first <<
                   "] backup=" << std::boolalpha << channel.second.backup << ", " <<
                   "searchCaleandar=" << std::boolalpha << channel.second.searchCalendar << ", "
                   "searchTimeline=" << std::boolalpha << channel.second.searchTimeline << ", "
                   "searchEvent=" << std::boolalpha << channel.second.searchEvent << ", "
                   "personSearch=" << std::boolalpha << channel.second.personSearch << ", "
                   "faceSearch=" << std::boolalpha << channel.second.faceSearch << ", "
                   "vehicleSearch=" << std::boolalpha << channel.second.vehicleSearch << ", "
                   "ocrSearch=" << std::boolalpha << channel.second.ocrSearch << ", "
                   "smartSearch=" << std::boolalpha << channel.second.smartSearch ;
        }

        msg << std::endl << std::endl;
        msg << "[MEDIA SUPPORT/LIMIT]";
        for (auto& channel : parseResult.mediaAttribute.channels) {
            msg << std::endl;
            msg << "    channel[" << channel.first <<
                   "] channelAudioOut=" << std::boolalpha << channel.second.channelAudioOut << ", " <<
                   "audioIn=" << std::boolalpha << channel.second.audioIn << ", "
                   "videoSetting=" << std::boolalpha << channel.second.videoSetting << ", "
                   "live=" << std::boolalpha << channel.second.live << ", "
                   "metadata=" << std::boolalpha << channel.second.metadata << ", "
                   "maxProfile=" << channel.second.maxProfile;
        }

        msg << std::endl << std::endl;
        msg << "[IMAGE SUPPORT/LIMIT]";
        for (auto& channel : parseResult.imageAttribute.channels) {
            msg << std::endl;
            msg << "    channel[" << channel.first <<
                   "] defog=" << std::boolalpha << channel.second.defog << ", " <<
                   "simpleFocus=" << std::boolalpha << channel.second.simpleFocus << ", "
                   "fisheyeLens=" << std::boolalpha << channel.second.fisheyeLens << ", "
                   "dewarpView=" << std::boolalpha << channel.second.dewarpView;
        }

        msg << std::endl << std::endl;
        msg << "[PTZ SUPPORT/LIMIT]";
        for (auto& channel : parseResult.ptzSupportAttribute.channels) {
            msg << std::endl;
            msg << "    channel[" << channel.first <<
                   "] realPtz=" << std::boolalpha << channel.second.realPtz << ", " <<
                   "preset=" << std::boolalpha << channel.second.preset << ", "
                   "swing=" << std::boolalpha << channel.second.swing << ", "
                   "group=" << std::boolalpha << channel.second.group << ", "
                   "tour=" << std::boolalpha << channel.second.tour << ", "
                   "trace=" << std::boolalpha << channel.second.trace << ", "
                   "areaZoom=" << std::boolalpha << channel.second.areaZoom << ", " <<
                   "continiousPan=" << std::boolalpha << channel.second.continiousPan << ", " <<
                   "continiousTilt=" << std::boolalpha << channel.second.continiousTilt << ", " <<
                   "continiousZoom=" << std::boolalpha << channel.second.continiousZoom << ", " <<
                   "maxPreset=" << channel.second.maxPreset << ", " <<
                   "auxCommands=";
            for (auto& command : channel.second.auxCommands)
                msg << ":" << command;
        }

        msg << std::endl << std::endl;
        msg << "[EVENT STATUS CGI SUPPORT]" << std::endl <<
               "includeTimeStamp=" << std::boolalpha << parseResult.eventStatusCgi.monitorDiff.includeTimeStamp << ", "
               "schemaBased=" << std::boolalpha << parseResult.eventStatusCgi.monitorDiff.schemaBased ;

        msg << std::endl << std::endl;
        msg << "[MEDIA CGI SUPPORT]" << std::endl <<
               "cameraRegister=" << std::boolalpha << parseResult.mediaCgi.cameraRegister << ", "
               "audioOutput=" << std::boolalpha << parseResult.mediaCgi.audioOutput ;

        msg << std::endl << std::endl;
        msg << "[PTZCONFIG CGI SUPPORT]" << std::endl <<
               "normalizedSpeed=" << std::boolalpha << parseResult.ptzControlCgi.continous.normalizedSpeed << ", "
               "pan range=" << parseResult.ptzControlCgi.continous.panRange.min << "~" <<
                               parseResult.ptzControlCgi.continous.panRange.max << ", "
               "tilt range=" << parseResult.ptzControlCgi.continous.tiltRange.min << "~" <<
                                parseResult.ptzControlCgi.continous.tiltRange.max << ", "
               "zoom range=" << parseResult.ptzControlCgi.continous.zoomRange.min << "~" <<
                                parseResult.ptzControlCgi.continous.zoomRange.max << ", " <<
               "areazoom profile=" << std::boolalpha << parseResult.ptzControlCgi.areazoom.profile << ", " <<
               "digitalAutoTracking profile=" << std::boolalpha << parseResult.ptzControlCgi.digitalAutoTracking.profile;

        msg << std::endl << std::endl;
        msg << "[EVENTRULE CGI VIEW]" << std::endl <<
            "DynamicEventName=";
        for (auto& eventName : parseResult.eventRuleCgi.dynamicEventName)
            msg << ":" << eventName;

        SLOG_DEBUG("============== Attribute Debugging Information START ============={}{}", SPDLOG_EOL, msg.str());
        SLOG_DEBUG("============== Attribute Debugging Information END =============");
    }

    return true;
}


}
}
