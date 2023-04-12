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
#include <unordered_map>

#include <boost/core/ignore_unused.hpp>

#include "cgis/BaseCommand.h"
#include "cgis/SunapiSyntaxBuilder.h"

namespace Wisenet
{
namespace Device
{

struct AttributesResult
{
    struct SystemAttribute
    {
        bool fwUpdate = false;
        bool configBackup = false;
        bool configRestore = false;
        bool shutdown = false;
        bool aiFeature = false;
        bool quickPlay = false;
        int maxChannel = 0;
        int channelExpansionLimit = 0;
        int maxPOS = 0;
    };

    struct MediaAttribute
    {
        struct Media
        {
            bool channelAudioOut = false;
            bool audioIn = false;
            bool videoSetting = false;
            bool live = false;
            bool metadata = false;
            int  maxProfile = 0;
        };
        std::unordered_map<int, struct MediaAttribute::Media>   channels;
    };

    struct ImageAttribute
    {
        struct Image
        {
            bool defog = false;
            bool simpleFocus = false;
            bool resetFocus = false;
            bool autoFocus = false;
            bool iRLED = false;
            bool diS = false;
            bool piris = false;
            bool iris = false;
            bool fisheyeLens = false;
            bool dewarpView = false;
            bool profileBasedDewarpedView = false;
            bool multiImager = false;
            bool thermalFeatures = false;
        };
        std::unordered_map<int, struct ImageAttribute::Image>   channels;
    };

    struct IoAttribute
    {
        bool alarmOutput = false;
        bool aux = false;
        bool configurableIO = false;
        bool alarmReset = false;
        int maxAlarmOutput = 0;
        int maxAux = 0;
        int maxConfigurableIO = 0;
    };

    struct PtzSupportAttribute
    {
        struct PtzSupport
        {
            bool realPtz = false;
            bool preset = false;
            bool swing = false;
            bool group = false;
            bool tour = false;
            bool trace = false;
            bool home = false;
            bool areaZoom = false;
            bool continiousPan = false;
            bool continiousTilt = false;
            bool continiousZoom = false;
            bool continiousFocus = false;
            bool continiousIris = false;
            bool panTiltOnly = false;
            bool zoomOnly = false;
            std::vector<std::string> auxCommands;

            int maxPreset = 0;
            int maxGroupCount = 0;
        };
        std::unordered_map<int, struct PtzSupportAttribute::PtzSupport>     channels;
    };

    struct RecordingAttribute
    {
        struct Channel
        {
            bool backup = false;
            bool searchCalendar = false;
            bool searchTimeline = false;
            bool searchEvent = false;
            bool personSearch = false;
            bool faceSearch = false;
            bool vehicleSearch = false;
            bool ocrSearch = false;
            bool smartSearch = false;
        };
        std::unordered_map<int, struct Channel> channels;

        bool overlapped = false;
        bool searchPeriod = false;
        bool searchByUTCTime = false;
        bool dualTrackRecording = false;
        bool manualRecordingStart = false;
        std::vector<std::string> playbackSpeeds;
        int maxPlaybackChannels = 16;
    };

    struct EventSourceAttribute
    {
        struct Channel
        {
            bool tracking = false;
        };
        std::unordered_map<int, struct Channel> channels;

        bool alarmInput = false;
        bool networkAlarmInput = false;
        int maxAlarmInput = 0;
        int maxNetworkAlarmInput = 0;
    };

    struct SystemCgi
    {
        bool configBackupEncrypt = false;
        bool configRestoreDecrypt = false;
    };

    struct MediaCgi
    {
        bool cameraRegister = false;
        bool audioOutput = false;
        bool sessionKey = false;
    };

    struct SecurityCgi
    {
        bool addUpdateUsers = false;
        bool IsPasswordEncrypted = false;
    };

    struct EventStatusCgi
    {
        struct MonitorDiff
        {
            bool includeTimeStamp = false;
            bool schemaBased = false;
        };
        MonitorDiff monitorDiff;
    };

    struct Range
    {
        int min = 0;
        int max = 0;
    };

    struct PtzControlCgi
    {
        struct Continous
        {
            bool normalizedSpeed = false;
            Range panRange;
            Range tiltRange;
            Range zoomRange;
        };

        Continous continous;

        struct AreaZoom{
            bool profile = false;
        };

        AreaZoom areazoom;

        struct DigitalAutoTracking{
            bool profile = false;
        };

        DigitalAutoTracking digitalAutoTracking;
    };

    struct ImageCgi
    {
        std::vector<std::string> defogMode;
        Range defogLevel;
    };

    struct EventRuleCgi
    {
        std::vector<std::string> dynamicEventName;
    };

    struct RecordingCgi
    {
        std::vector<Wisenet::Device::RecordingType> recordingTypes;
    };

    struct AiCgi
    {
        int metaAttributeSearchMaxResults =0;
        int ocrSearchMaxResults = 0;
    };

    struct VideoCgi
    {
        bool snapshot = false;
        bool thumbnail = false;
    };

    // attributes
    SystemAttribute      systemAttribute;
    IoAttribute          ioAttribute;
    RecordingAttribute   recordingAttribute;
    EventSourceAttribute eventSourceAttribute;
    MediaAttribute       mediaAttribute;
    ImageAttribute       imageAttribute;
    PtzSupportAttribute  ptzSupportAttribute;

    // cgis
    SystemCgi        systemCgi;
    SecurityCgi      securityCgi;
    ImageCgi         imageCgi;
    EventStatusCgi   eventStatusCgi;
    MediaCgi         mediaCgi;
    PtzControlCgi    ptzControlCgi;
    EventRuleCgi     eventRuleCgi;
    RecordingCgi     recordingCgi;
    AiCgi            aiCgi;
    VideoCgi         videoCgi;
};

class AttributesParser : public BaseCommand, public XmlParser
{
public:
    explicit AttributesParser(const std::string& logPrefix, ResponseBaseSharedPtr response)
        :BaseCommand(this, logPrefix, response, false, false)
    {
    }

    std::string RequestUri() override
    {
        return u8"/stw-cgi/attributes.cgi";
    }

    ResponseBaseSharedPtr ProcessPost() override {
        //응답처리를 하지 않으므로 nullptr로 리턴
        return nullptr;
    }

    AttributesResult parseResult;

private:
    bool parseXml(pugi::xml_document& xmlDoc) override;
};

}
}

