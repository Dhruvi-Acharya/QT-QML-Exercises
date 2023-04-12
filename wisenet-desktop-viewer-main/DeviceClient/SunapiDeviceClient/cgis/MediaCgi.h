#pragma once

#include <string>
#include <unordered_map>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include "DeviceClient/DeviceStructure.h"
#include "cgis/BaseCommand.h"
#include "cgis/SunapiSyntaxBuilder.h"
//#include "LogSettings.h"

namespace Wisenet
{
namespace Device
{

//************************* MediaSessionkeyView ******************************//

class MediaSessionkeyView : public BaseCommand, public IniParser
{
public:
    explicit MediaSessionkeyView(const std::string& logPrefix)
        :BaseCommand(this,logPrefix,std::make_shared<DeviceConnectResponse>(),false,false)
    {
    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;
        builder.CreateCgi("media","sessionkey", "view");
        return builder.GetUrl();
    }

    ResponseBaseSharedPtr ProcessPost() override {
        //응답처리를 하지 않으므로 nullptr로 리턴
        return nullptr;
    }

    std::string parseResult = "";   // session key string

private:
    bool parseINI(NameValueText& iniDoc) override
    {
        parseResult = iniDoc.getString("SessionKey");

        return parseResult != "";
    }
};

//************************* MediaVideoSourceView ******************************//

struct MediaVideoSourceViewResult
{
    struct VideoSource
    {
        std::string status;
        std::string name;
    };
    std::unordered_map<int, VideoSource> videoSources;
};

class MediaVideoSourceView : public BaseCommand, public IniParser
{
public:
    explicit MediaVideoSourceView(const std::string& logPrefix)
        :BaseCommand(this,logPrefix,std::make_shared<DeviceConnectResponse>(),false,false)
    {
    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;
        builder.CreateCgi("media","videosource", "view");
        return builder.GetUrl();
    }

    ResponseBaseSharedPtr ProcessPost() override {
        //응답처리를 하지 않으므로 nullptr로 리턴
        return nullptr;
    }

    MediaVideoSourceViewResult parseResult;

private:
    bool parseINI(NameValueText& iniDoc) override
    {
        // Parse full response
        for (auto &item : iniDoc.m_listValue) {
            std::vector<std::string> tokens;
            int channelIndex = 0;

            boost::split(tokens, item.first, boost::is_any_of("."), boost::token_compress_on);
            if (tokens.size() != 3)
                continue;

            if (tokens[0] != "Channel")
                continue;

            if (!try_stoi(tokens[1], &channelIndex))
                continue;

            std::string& key = tokens[2];

            if (key == "Name")
                parseResult.videoSources[channelIndex].name = item.second;
            else if (key == "State")
                parseResult.videoSources[channelIndex].status = item.second;

        }
        return true;
    }
};

//************************* MediaVideoProfileView ******************************//

struct MediaVideoProfileViewResult
{
    struct VideoProfileInfo
    {
        std::string name;
        std::string encodingType;
        Resolution resolution;
        int framerate = 0;
        int bitrate = 0;
        int compressionLevel = 0;

        // H.264, H.265
        int h26xGovLength = 0;
        std::string h26xProfile;
        std::string h26xBitrateControlType;
    };

    std::map<int, std::map<int, VideoProfileInfo>> channels;
};

class MediaVideoProfileView : public BaseCommand, public IniParser
{
public:
    explicit MediaVideoProfileView(const std::string& logPrefix,
                                   ResponseBaseSharedPtr response,
                                   bool ignoreHttpErrorResponse = false,
                                   bool ignoreParsingErrorResponse = true)
        : BaseCommand(this,logPrefix,response,
                      ignoreHttpErrorResponse,
                      ignoreParsingErrorResponse)
    {
    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;
        builder.CreateCgi("media", "videoprofile", "view");
        return builder.GetUrl();
    }

    ResponseBaseSharedPtr ProcessPost() override {
        return nullptr;
    }

    MediaVideoProfileViewResult parseResult;

private:
    bool parseINI(NameValueText& iniDoc) override
    {
        for (auto &item : iniDoc.m_listValue) {
            std::vector<std::string> tokens;
            int cIndex = 0;
            int pIndex = 0;

            boost::split(tokens, item.first, boost::is_any_of("."), boost::token_compress_on);
            if (tokens.size() != 5 && tokens.size() != 6)
                continue;

            if (tokens[0] != "Channel")
                continue;

            if (tokens[2] != "Profile")
                continue;

            if (!try_stoi(tokens[1], &cIndex))
                continue;
            if (!try_stoi(tokens[3], &pIndex))
                continue;

            std::string& key = tokens[4];
            if (tokens.size() == 6)
                key = tokens[5];

            if (key == "Name")
                parseResult.channels[cIndex][pIndex].name = item.second;
            else if (key == "EncodingType")
                parseResult.channels[cIndex][pIndex].encodingType = item.second;
            else if (key == "Resolution") {
                std::vector<std::string> subTokens;
                boost::split(subTokens, item.second, boost::is_any_of("x"), boost::token_compress_on);
                if (subTokens.size() == 2)
                {
                    Resolution resolution;
                    resolution.width = try_stoi(subTokens[0]);
                    resolution.height = try_stoi(subTokens[1]);
                    parseResult.channels[cIndex][pIndex].resolution = resolution;
                }
            }
            else if (key == "Bitrate") {
                parseResult.channels[cIndex][pIndex].bitrate = try_stoi(item.second, 0);
            }
            else if (key == "FrameRate") {
                parseResult.channels[cIndex][pIndex].framerate = try_stoi(item.second, 0);
            }
            else if (key == "CompressionLevel") {
                parseResult.channels[cIndex][pIndex].compressionLevel = try_stoi(item.second, 0);
            }
            else if (key == "Profile") {
                parseResult.channels[cIndex][pIndex].h26xProfile = item.second;
            }
            else if (key == "GOVLength") {
                parseResult.channels[cIndex][pIndex].h26xGovLength = try_stoi(item.second, 0);
            }
            else if (key == "BitrateControlType") {
                parseResult.channels[cIndex][pIndex].h26xBitrateControlType = item.second;
            }
        }

        return true;
    }
};


//************************* MediaVideoCodecInfoView ******************************//

struct MediaVideoCodecInfoViewResult
{
    // first key => encoding type
    // second key => view mode type
    // third key => resolution key (3840x2160)
    // resolution => only General resolution list
    std::unordered_map<std::string,
    std::unordered_map<std::string,
    std::unordered_map<std::string, Resolution>>> resolutions;
};

class MediaVideoCodecInfoView : public BaseCommand, public IniParser
{
public:
    explicit MediaVideoCodecInfoView(const std::string& logPrefix,
                                     ResponseBaseSharedPtr response,
                                     int channelIndex,
                                     bool ignoreHttpErrorResponse = false,
                                     bool ignoreParsingErrorResponse = false)
        : BaseCommand(this, logPrefix, response,
                     ignoreHttpErrorResponse, ignoreParsingErrorResponse),
        m_channelIndex(channelIndex)
    {
    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;
        builder.CreateCgi("media", "videocodecinfo", "view");
        builder.AddParameter("Channel", m_channelIndex);
        return builder.GetUrl();
    }

    ResponseBaseSharedPtr ProcessPost() override {
        //응답처리를 하지 않으므로 nullptr로 리턴
        return nullptr;
    }

    MediaVideoCodecInfoViewResult parseResult;

private:
    bool parseINI(NameValueText& iniDoc) override
    {
        std::string encodingType="";
        std::string viewModeType;

        for (auto &item : iniDoc.m_listValue) {
            //SPDLOG_DEBUG("codec parser :: {}={}", item.first, item.second);
            std::vector<std::string> tokens;
            boost::split(tokens, item.first, boost::is_any_of("."), boost::token_compress_on);
            if (tokens.size() != 3 && tokens.size() != 4)
                continue;

            if (tokens[0] == "Channel") {
                if (tokens[2] == "EncodingType")
                    encodingType = item.second;
                else if (tokens[2] == "ViewMode")
                    viewModeType = item.second;
                continue;
            }

            //SPDLOG_DEBUG("encodingType={}", encodingType);

            if (tokens.size() != 4)
                continue;

            if (encodingType.empty())
                continue;

            if (tokens[1] != "General")
                continue;

            if (tokens[3] == "Width") {
                //SPDLOG_DEBUG("CODEC_INFO={}, {}, {}", encodingType, viewModeType, tokens[2]);
                parseResult.resolutions[encodingType][viewModeType][tokens[2]].width =
                    try_stoi(item.second, 0);
            }
            else if (tokens[3] == "Height") {
                parseResult.resolutions[encodingType][viewModeType][tokens[2]].height =
                    try_stoi(item.second, 0);
            }
        }
        return true;
    }

    int m_channelIndex;
};


////************************* MediaVideoProfileAdd ******************************//

struct VideoProfileAddParam
{
    std::string name = "Live4NVR";
    std::string encodingType = "H264";
    std::string bitrateControlType = "VBR";
    Resolution resolution/*{640, 360}*/;
    int framerate = 15;
    int govLength = 15;
    int bitrate = 512;
};

class MediaVideoProfileAdd : public BaseCommand, public IniParser
{
public:
    explicit MediaVideoProfileAdd(const std::string& logPrefix,
                                  ResponseBaseSharedPtr response,
                                  VideoProfileAddParam& addParam,
                                  int channelIndex,
                                  bool ignoreHttpErrorResponse = false,
                                  bool ignoreParsingErrorResponse = false)
        : BaseCommand(this, logPrefix, response,
                      ignoreHttpErrorResponse, ignoreParsingErrorResponse)
        , m_addParam(addParam), m_channelIndex(channelIndex)
    {
       
    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;
        builder.CreateCgi("media", "videoprofile", "add");
        builder.AddParameter("Channel", m_channelIndex);
        builder.AddParameter("Name", m_addParam.name);
        builder.AddParameter("Resolution", std::to_string(m_addParam.resolution.width) + "x" + std::to_string(m_addParam.resolution.height));
        builder.AddParameter("EncodingType", m_addParam.encodingType);
        builder.AddParameter(m_addParam.encodingType + ".GOVLength", m_addParam.govLength);
        builder.AddParameter("FrameRate", m_addParam.framerate);
        builder.AddParameter("Bitrate", m_addParam.bitrate);
        builder.AddParameter(m_addParam.encodingType + ".BitrateControlType", m_addParam.bitrateControlType);
        
        return builder.GetUrl();
    }

    ResponseBaseSharedPtr ProcessPost() override {
        //응답처리를 하지 않으므로 nullptr로 리턴
        return nullptr;
    }

private:
    bool parseINI(NameValueText& iniDoc) override
    {
        boost::ignore_unused(iniDoc);
        return true;
    }
    VideoProfileAddParam m_addParam;
    int m_channelIndex = 0;
};


//************************* MediaVideoProfilePolicyView ******************************//

struct MediaVideoProfilePolicyViewResult
{
    struct VideoProfilePolicy
    {
        int liveProfile = 0;
        int recordProfile = 0;
        int networkProfile = 0;
    };
    std::map<int, VideoProfilePolicy> channels;
};

class MediaVideoProfilePolicyView : public BaseCommand, public IniParser
{
public:
    explicit MediaVideoProfilePolicyView(const std::string& logPrefix,
                                         ResponseBaseSharedPtr response, std::string channelID = "")
        :BaseCommand(this,logPrefix,response,false,true), m_channelID(channelID)
    {
    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;
        builder.CreateCgi("media", "videoprofilepolicy", "view");

        if(!m_channelID.empty()) {
            builder.AddParameter("Channel", toSunapiChannel(m_channelID));
        }

        return builder.GetUrl();
    }

    ResponseBaseSharedPtr ProcessPost() override {
        //응답처리를 하지 않으므로 nullptr로 리턴
        return nullptr;
    }

    MediaVideoProfilePolicyViewResult parseResult;

private:
    bool parseINI(NameValueText& iniDoc) override
    {
        for (auto &item : iniDoc.m_listValue) {
            std::vector<std::string> tokens;
            int channelIndex = 0;

            boost::split(tokens, item.first, boost::is_any_of("."), boost::token_compress_on);
            if (tokens.size() != 3)
                continue;

            if (tokens[0] != "Channel")
                continue;

            if (!try_stoi(tokens[1], &channelIndex))
                continue;

            std::string& key = tokens[2];

            if (key == "NetworkProfile")
                parseResult.channels[channelIndex].networkProfile = try_stoi(item.second, 1);
            else if (key == "LiveProfile")
                parseResult.channels[channelIndex].liveProfile = try_stoi(item.second, 1);
            else if (key == "RecordProfile") {
                parseResult.channels[channelIndex].recordProfile = try_stoi(item.second, 1);
            }
        }
        return true;
    }

    std::string m_channelID;
};

//************************* MediaCameraRegisterView ******************************//

struct MediaCameraRegisterViewResult
{
    struct CameraRegister
    {
        bool isBypassSupported = false;
        std::string model;
        std::string ip;
    };
    std::map<int, CameraRegister> cameraRegisters;
};

class MediaCameraRegisterView : public BaseCommand, public IniParser
{
public:
    explicit MediaCameraRegisterView(const std::string& logPrefix,
                                     ResponseBaseSharedPtr response,
                                     bool ignoreHttpErrorResponse = false,
                                     bool ignoreParsingErrorResponse = false)
          : BaseCommand(this,logPrefix,response,
                        ignoreHttpErrorResponse,
                        ignoreParsingErrorResponse)
    {
    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;
        builder.CreateCgi("media","cameraregister", "view");
        return builder.GetUrl();
    }

    ResponseBaseSharedPtr ProcessPost() override {
        //응답처리를 하지 않으므로 nullptr로 리턴
        return nullptr;
    }

    MediaCameraRegisterViewResult parseResult;

private:
    bool parseINI(NameValueText& iniDoc) override
    {
        for (auto &item : iniDoc.m_listValue) {
            std::vector<std::string> tokens;
            int channelIndex = 0;

            boost::split(tokens, item.first, boost::is_any_of("."), boost::token_compress_on);
            if (tokens.size() != 3)
                continue;

            if (tokens[0] != "Channel")
                continue;

            if (!try_stoi(tokens[1], &channelIndex))
                continue;

            std::string& key = tokens[2];

            if (key == "Model")
                parseResult.cameraRegisters[channelIndex].model = item.second;
            else if (key == "IPAddress")
                parseResult.cameraRegisters[channelIndex].ip = item.second;
            else if (key == "IsBypassSupported") {
                if (boost::iequals(item.second, "True"))
                    parseResult.cameraRegisters[channelIndex].isBypassSupported = true;
                else
                    parseResult.cameraRegisters[channelIndex].isBypassSupported = false;
            }
        }
        return true;
    }
};

//************************* MediaCameraUpgradeView ******************************//

struct MediaCameraUpgradeViewResult
{
    struct CameraUpgrade
    {
        bool isUpgradeEnabled = false;
        std::string currentFwVersion = "";
    };
    std::map<int, CameraUpgrade> cameraUpagrades;
};

class MediaCameraUpgradeView : public BaseCommand, public IniParser
{
public:
    explicit MediaCameraUpgradeView(const std::string& logPrefix,
                                     ResponseBaseSharedPtr response,
                                     bool ignoreHttpErrorResponse = false,
                                     bool ignoreParsingErrorResponse = false)
          : BaseCommand(this,logPrefix,response,
                        ignoreHttpErrorResponse,
                        ignoreParsingErrorResponse)
    {
    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;
        builder.CreateCgi("media","cameraupgrade", "view");
        builder.AddParameter("SkipUpgradeServerCheck", true);
        return builder.GetUrl();
    }

    ResponseBaseSharedPtr ProcessPost() override {
        //응답처리를 하지 않으므로 nullptr로 리턴
        return nullptr;
    }

    MediaCameraUpgradeViewResult parseResult;

private:
    bool parseINI(NameValueText& iniDoc) override
    {
        for (auto &item : iniDoc.m_listValue) {
            std::vector<std::string> tokens;
            int channelIndex = 0;

            boost::split(tokens, item.first, boost::is_any_of("."), boost::token_compress_on);
            if (tokens.size() != 3)
                continue;

            if (tokens[0] != "Channel")
                continue;

            if (!try_stoi(tokens[1], &channelIndex))
                continue;

            std::string& key = tokens[2];

            if (key == "CurrentVersion")
                parseResult.cameraUpagrades[channelIndex].currentFwVersion = item.second;
            else if (key == "Upgradeable") {
                if (boost::iequals(item.second, "TRUE"))
                    parseResult.cameraUpagrades[channelIndex].isUpgradeEnabled = true;
                else
                    parseResult.cameraUpagrades[channelIndex].isUpgradeEnabled = false;
            }
        }
        return true;
    }
};

//************************* MediaAudioOutputView ******************************//

struct MediaAudioOutputViewResult
{
    struct AudioOutput
    {
        bool enable = false;
        int sampleRate = 0;
        int bitrate = 0;
        int gain = 0;
        std::string mode;
        std::string decodingType;
    };
    std::map<int, AudioOutput> audioOutputs;
};

class MediaAudioOutputView : public BaseCommand, public IniParser
{
public:
    explicit MediaAudioOutputView(const std::string& logPrefix,
                                  ResponseBaseSharedPtr response,
                                  bool ignoreHttpErrorResponse = false,
                                  bool ignoreParsingErrorResponse = false)
       : BaseCommand(this,logPrefix,response,
                     ignoreHttpErrorResponse,
                     ignoreParsingErrorResponse)
    {
    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;
        builder.CreateCgi("media","audiooutput", "view");
        return builder.GetUrl();
    }

    ResponseBaseSharedPtr ProcessPost() override {
        return nullptr;
    }

    MediaAudioOutputViewResult parseResult;
private:
    bool parseINI(NameValueText& iniDoc) override
    {
        for (auto &item : iniDoc.m_listValue) {
            std::vector<std::string> tokens;
            int channelIndex = 0;

            boost::split(tokens, item.first, boost::is_any_of("."), boost::token_compress_on);
            if (tokens.size() != 3)
                continue;

            if (tokens[0] != "Channel")
                continue;

            if (!try_stoi(tokens[1], &channelIndex))
                continue;

            std::string& key = tokens[2];
            if (key == "Enable") {
                if (boost::iequals(item.second, "True"))
                    parseResult.audioOutputs[channelIndex].enable = true;
                else
                    parseResult.audioOutputs[channelIndex].enable = false;
            }
            else if (key == "SampleRate") {
                parseResult.audioOutputs[channelIndex].sampleRate = try_stoi(item.second, 0);
            }
            else if (key == "Bitrate") {
                parseResult.audioOutputs[channelIndex].bitrate = try_stoi(item.second, 0);
            }
            else if (key == "Gain") {
                parseResult.audioOutputs[channelIndex].gain = try_stoi(item.second, 0);
            }
            else if (key == "Mode") {
                parseResult.audioOutputs[channelIndex].mode = item.second;
            }
            else if (key == "DecodingType"){
                parseResult.audioOutputs[channelIndex].decodingType = item.second;
            }
        }
        return true;
    }
};

//************************* MediaStreamUriView ******************************//

struct MediaStreamUriViewParam
{
    int Channel = -1;                       /**< Channel ID */
    int Profile = -1;                       /**< Stream URI profile
                                                 Note. Profile, MediaType, Mode, StreamType, TransportProtocol, and RTSPOverHTTP must be sent together with the view action. */
    std::string MediaType = "";				/**< Streaming media type */
    std::string Mode = "";					/**< Streaming mode */
    std::string ClientType = "";			/**< Client type(NVR only)
                                                 Note. Profile, MediaType, Mode, StreamType, TransportProtocol, RTSPOverHTTP and
                                                 ClientType must be sent together with the view action for NVR. */
    std::string StreamType = "";			/**< Streaming type */
    std::string TransportProtocol = "";     /**< Streaming transfer protocol */
    bool RTSPOverHTTP = false;              /**< Enables or disables the RTSP (Real Time Streaming Protocol)  over HTTP (Hyper Transfer Protocol) */
    int OverlappedID = -1;                  /**< Overlapped recording ID
                                                 For more information about overlapped recording,
                                                 please refer to ‘Chapter 5 Overlapped Recording’ of the ‘Recording’ document.
                                                 OverlappedID is valid only when MediaType is set to Backup.(Camera only) */
};

class MediaStreamUriView : public BaseCommand, public IniParser
{
public:
    explicit MediaStreamUriView(const std::string& logPrefix, std::string deviceUUID, MediaStreamUriViewParam param)
        : BaseCommand(this, logPrefix, std::make_shared<DeviceConnectResponse>(), false, false)
        , m_param(param)
    {
        boost::ignore_unused(deviceUUID);
    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;
        builder.CreateCgi("media","streamuri", "view");
        if(m_param.Channel != -1)
            builder.AddParameter("Channel", toSunapiChannel(m_param.Channel));
        builder.AddParameter("Profile", m_param.Profile);
        builder.AddParameter("MediaType", m_param.MediaType);
        builder.AddParameter("Mode", m_param.Mode);
        if(m_param.ClientType != "")
            builder.AddParameter("ClientType", m_param.ClientType);
        builder.AddParameter("StreamType", m_param.StreamType);
        builder.AddParameter("TransportProtocol", m_param.TransportProtocol);
        builder.AddParameter("RTSPOverHTTP", m_param.RTSPOverHTTP);
        if(m_param.OverlappedID != -1)
            builder.AddParameter("OverlappedID", m_param.OverlappedID);
        return builder.GetUrl();
    }

    ResponseBaseSharedPtr ProcessPost() override {
        //응답처리를 하지 않으므로 nullptr로 리턴
        return nullptr;
    }

    std::string parseResult = "";   // Media Stream URI string

private:
    bool parseINI(NameValueText& iniDoc) override
    {
        parseResult = iniDoc.getString("URI");

        return parseResult != "";
    }

    MediaStreamUriViewParam m_param;
};


}
}

