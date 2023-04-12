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
#include <string>
#include <iostream>
#include <boost/core/ignore_unused.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/tokenizer.hpp>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/error/en.h>
#include "NameValueText.h"
#include "DeviceClient/DeviceRequestResponse.h"
#include "SunapiDeviceClientUtil.h"

#define PUGIXML_HEADER_ONLY
#include <pugixml.hpp>

namespace Wisenet
{
namespace Device
{

struct ReturnCode
{
    enum class ContentType
    {
        TextPlain,
        Json,
        TextXml,
        OctetStream,
        Jpeg,
    };

    bool        isSuccess = true;
    int         errorCode = 0;
    std::string errorDetails;
    Wisenet::ErrorCode serviceErrorCode = Wisenet::ErrorCode::NoError;
    ContentType contentType = ContentType::TextPlain;

    bool operator!() const
    {
        return !isSuccess;
    }
};


class BaseCommand;
class BaseParser
{
public:
    enum class Type{
        UNKNWON,
        INI,
        JSON,
        XML,
        JPEG,
    };

    explicit BaseParser(Type type)
        :m_type(type)
    {

    };

    virtual ~BaseParser(){};


    Type GetType(){ return m_type; };
    Type m_type = Type::UNKNWON;
};

class IniParser : public BaseParser
{
public:
    IniParser()
        :BaseParser(Type::INI)
    {

    };
    friend BaseCommand;
private:
    virtual bool parseINI(NameValueText& iniDoc) = 0;
};

class JsonParser : public BaseParser
{
public:
    JsonParser()
        :BaseParser(Type::JSON)
    {

    };
    friend BaseCommand;
private:
    virtual bool parseJson(rapidjson::Document& jsonDoc) = 0;
};

class XmlParser : public BaseParser
{
public:
    XmlParser()
        :BaseParser(Type::XML)
    {

    };
    friend BaseCommand;
private:
    virtual bool parseXml(pugi::xml_document& xmlDoc) = 0;
};

class JpegParser : public BaseParser
{
public:
    JpegParser()
        :BaseParser(Type::JPEG)
    {

    };
    friend BaseCommand;
private:
    virtual bool setContent(const std::string& content) = 0;
};

class BaseCommand
{
public:
    // parser: IniParser, JsonParser, XmlParser 중 하나의 객체 포인터. 파싱을 하지 않으면 nullptr.
    // responseBase: 응답에 사용될 Response객체
    // ignoreHttpErrorResponse: true인 경우, HTTP 에러 발생시에도 에러에 대한 응답 처리를 하지 않습니다.
    // ignoreParsingErrorResponse: true인 경우, ParseContent 에러 발생시에도 에러에 대한 응답 처리를 하지 않습니다.
    explicit BaseCommand(BaseParser* parser,
                         const std::string& logPrefix,
                         ResponseBaseSharedPtr responseBase,
                         bool ignoreHttpErrorResponse,
                         bool ignoreParsingErrorResponse);

    virtual ~BaseCommand(){};


    virtual std::string RequestUri() = 0; ///< 요청할 URI를 획득합니다.
    virtual std::string GetPostData(){ return ""; } ///< HTTP POST로 보내는 데이터가 있으면 재정의해서 데이터를 리턴해야 합니다.
    virtual std::string GetFilePath(){ return ""; } ///< HttpGetDownload사용 시 백업할 파일패스
    virtual void RenewResponse() {} ///응답객체를 새롭게 생성한다.
    virtual void IsContinue(bool isContinue){ boost::ignore_unused(isContinue); } ///< Continue 값 설정.
    ReturnCode ParseContent(const std::string& type, const std::string& content); ///< type에 맞게 content를 파싱합니다.
    virtual ResponseBaseSharedPtr ProcessPost() = 0; ///< ParseContent 후 Response를 보내기 위한 작업을 합니다.

    bool IgnoreHttpErrorResponse(){ return m_ignoreHttpErrorResponse; }; ///< true로 리턴하는 경우, HTTP 에러 발생시에도 에러에 대한 응답 처리를 하지 않습니다.
    bool IgnoreParsingErrorResponse(){ return m_ignoreParsingErrorResponse; }; ///< true로 리턴하는 경우, ParseContent 에러 발생시에도 에러에 대한 응답 처리를 하지 않습니다.

    ///< true로 리턴하는 경우, Json응답을 받을 수 있도록 요청합니다.
    bool NeedJsonResponse(){
        if(m_parser && (BaseParser::Type::JSON == m_parser->GetType())){
            return true;
        }
        return false;
    }

    //< ResponseBase 객체를 획득합니다.
    ResponseBaseSharedPtr ResponseBase() {
        return m_responseBase;
    }

    ErrorCode GetServiceErrorCode() { return m_serviceErrorCode; };
protected:
    rapidjson::Document m_jsonDoc;
    NameValueText m_iniDoc;
    pugi::xml_document  m_xmlDoc;
    ReturnCode::ContentType m_currentContentType = ReturnCode::ContentType::TextPlain;

    BaseParser * m_parser;
    std::string m_logPrefix;
    ResponseBaseSharedPtr m_responseBase;

    bool m_ignoreHttpErrorResponse;
    bool m_ignoreParsingErrorResponse;

    ErrorCode m_serviceErrorCode;
};

typedef std::shared_ptr<BaseCommand> BaseCommandSharedPtr;

}
}
