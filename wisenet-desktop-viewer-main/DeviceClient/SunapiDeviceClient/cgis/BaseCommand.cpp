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
#include "BaseCommand.h"

#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/tokenizer.hpp>

#include "SunapiDeviceClientUtil.h"
#include "SunapiDeviceClientLogSettings.h"


namespace Wisenet
{
namespace Device
{


static Wisenet::ErrorCode toServiceErrorCodeBase(const int sr)
{
    boost::ignore_unused(sr);
//    if (sr == 0)
//        return Wisenet::ErrorCode::NoError;
//    else if (sr >= 600 && sr <=604)
//        return Wisenet::ErrorCode::InvalidRequest;
    if(sr == 702)
        return Wisenet::ErrorCode::SystemMenuUsed;

    return Wisenet::ErrorCode::InvalidRequest;
}

static bool checkIniNGBase(const std::string &content, ReturnCode &retCode)
{
    if (content.empty()) {
        retCode.isSuccess = true;
        return true;
    }

    if((content.size() >=4) &&
            ((0 == content.compare(0,3, "NG\n"))|| (0 == content.compare(0,4, "NG\r\n")))) {

        boost::char_separator<char> sep("\r\n");
        boost::tokenizer<boost::char_separator<char> > tok(content, sep);
        std::vector<std::string> line(tok.begin(), tok.end());
        if (line.size() == 4)
        {
            boost::char_separator<char> sep2(":");
            boost::tokenizer<boost::char_separator<char> > tok2(line[1], sep2);
            std::vector<std::string> v(tok2.begin(), tok2.end());
            if (v.size() == 2)
            {
                std::string key = boost::algorithm::trim_copy(v[0]);
                std::string value = boost::algorithm::trim_copy(v[1]);
                if (v[0] == "Error Code")
                {
                    int err = 0;
                    try_stoi(value, &err, 600);
                    retCode.errorCode = err;
                    retCode.serviceErrorCode = toServiceErrorCodeBase(retCode.errorCode);
                }
            }

            if (line[2].compare(0, 13, "Error Details") == 0)
            {
                retCode.errorDetails = line[3];
            }
        }

        SPDLOG_WARN("NG response : code={},details={}", retCode.errorCode, retCode.errorDetails);
        retCode.isSuccess = false;
        return false;
    }

    retCode.isSuccess = true;
    return true;
}


static bool checkJsonNGBase(rapidjson::Document& doc, ReturnCode &retCode)
{
    if(doc.HasMember("Response")) {
        std::string response = doc["Response"].GetString();

        if(0 == response.compare("Fail")) {
            const rapidjson::Value& error = doc["Error"];
            if(error.HasMember("Code")) {
                retCode.errorCode = error["Code"].GetInt();
                retCode.serviceErrorCode = toServiceErrorCodeBase(retCode.errorCode);
            }
            if(error.HasMember("Details")) {
                retCode.errorDetails = error["Details"].GetString();
            }

            SPDLOG_WARN("NG response(Json) : code={}, details={}", retCode.errorCode, retCode.errorDetails);
            retCode.isSuccess = false;
            return false;
        }
    }
    retCode.isSuccess = true;
    return true;
}

BaseCommand::BaseCommand(BaseParser* parser,
                         const std::string& logPrefix,
                         ResponseBaseSharedPtr responseBase,
                         bool ignoreHttpErrorResponse,
                         bool ignoreParsingErrorResponse)
        :m_parser(parser),
         m_logPrefix(logPrefix),
         m_responseBase(responseBase),
         m_ignoreHttpErrorResponse(ignoreHttpErrorResponse),
         m_ignoreParsingErrorResponse(ignoreParsingErrorResponse),
         m_serviceErrorCode(ErrorCode::NoError)
{
}

ReturnCode BaseCommand::ParseContent(const std::string &type, const std::string &content)
{
    ReturnCode ret;

    try {
        m_iniDoc.Clear();
        m_xmlDoc.reset();
        rapidjson::Document newJsonDoc;
        m_jsonDoc.Swap(newJsonDoc);

        ret.contentType = ReturnCode::ContentType::TextPlain;

        SLOG_DEBUG("BaseCommand::ParseContent() content type={}", type);
        if (boost::contains(type, "application/json"))
            ret.contentType = ReturnCode::ContentType::Json;
        else if (boost::contains(type, "text/xml"))
            ret.contentType = ReturnCode::ContentType::TextXml;
        else if(boost::contains(type, "application/octet-stream"))
            ret.contentType = ReturnCode::ContentType::OctetStream;
        else if(boost::contains(type, "image/jpeg"))
            ret.contentType = ReturnCode::ContentType::Jpeg;

        m_currentContentType = ret.contentType;

        if (ret.contentType == ReturnCode::ContentType::Json) {
            if (m_jsonDoc.Parse(content.c_str()).HasParseError()) {
                SLOG_WARN("json parse error.. ErrorOffset={}, ErrorMessage={}", m_jsonDoc.GetErrorOffset(), GetParseError_En(m_jsonDoc.GetParseError()));
                ret.errorDetails = "json parse error..";
                ret.serviceErrorCode = ErrorCode::InvalidRequest;
                ret.isSuccess = false;
                return ret;
            }
            if (checkJsonNGBase(m_jsonDoc, ret) && m_parser && (BaseParser::Type::JSON == m_parser->GetType())) {
                ret.isSuccess = (static_cast<JsonParser *>(m_parser))->parseJson(m_jsonDoc);
                if (!ret.isSuccess) {
                    ret.serviceErrorCode = m_serviceErrorCode;
                }
            }
        }
        else if (ret.contentType == ReturnCode::ContentType::TextPlain) {
            if (checkIniNGBase(content, ret) && m_parser&& (BaseParser::Type::INI == m_parser->GetType())) {
                if (!m_iniDoc.Read(content)) {
                    SLOG_WARN("ini read error..");
                    ret.errorDetails = "ini read error..";
                    ret.serviceErrorCode = ErrorCode::InvalidRequest;
                    ret.isSuccess = false;
                    return ret;
                }
                ret.isSuccess = (static_cast<IniParser *>(m_parser))->parseINI(m_iniDoc);
                if (!ret.isSuccess) {
                    SLOG_WARN("ini read isSuccess fail={} errorCode={} detail={}", ret.isSuccess, ret.errorCode, ret.errorDetails);
                    ret.serviceErrorCode = m_serviceErrorCode;
                    if (ErrorCode::NoError == ret.serviceErrorCode) {
                        ret.serviceErrorCode = ErrorCode::InvalidRequest;
                    }
                }
            }
        }
        else if (ret.contentType == ReturnCode::ContentType::TextXml){
            pugi::xml_parse_result result = m_xmlDoc.load_string(content.c_str());
            if (!result) {
                SLOG_ERROR("Can not load xml response result={}", result.description());
                ret.errorDetails = "Can not load xml response";
                ret.serviceErrorCode = m_serviceErrorCode;
                if (ErrorCode::NoError == ret.serviceErrorCode) {
                    ret.serviceErrorCode = ErrorCode::InvalidRequest;
                }
                ret.isSuccess = false;
                return ret;
            }

            if(m_parser && (BaseParser::Type::XML == m_parser->GetType())) {
                ret.isSuccess = (static_cast<XmlParser *>(m_parser))->parseXml(m_xmlDoc);
                if (!ret.isSuccess) {
                    ret.serviceErrorCode = m_serviceErrorCode;
                    if (ErrorCode::NoError == ret.serviceErrorCode) {
                        ret.serviceErrorCode = ErrorCode::InvalidRequest;
                    }
                }
            }

        }
        else if(ret.contentType == ReturnCode::ContentType::Jpeg) {

            if(m_parser && (BaseParser::Type::JPEG == m_parser->GetType())) {
                ret.isSuccess = (static_cast<JpegParser *>(m_parser))->setContent(content);
                if (!ret.isSuccess) {
                    ret.serviceErrorCode = m_serviceErrorCode;
                    if (ErrorCode::NoError == ret.serviceErrorCode) {
                        ret.serviceErrorCode = ErrorCode::InvalidRequest;
                    }
                }
            } else {
                SLOG_INFO("Dosen't have a jpeg parser instance.");
            }
        }
        else {
            ret.isSuccess = true;
        }
    }
    catch (const std::exception& e) {
        SLOG_ERROR("Failed to parse httpContent : e = {} , content = {})",
                     e.what(), content);
        ret.errorDetails = "Failed to parse httpContent";
        ret.serviceErrorCode = ErrorCode::InvalidRequest;
        ret.isSuccess = false;
    }

    return ret;
}

}
}
