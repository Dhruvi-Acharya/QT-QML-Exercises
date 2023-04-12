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

#include "cgis/BaseCommand.h"
#include "cgis/SunapiSyntaxBuilder.h"

namespace Wisenet
{
namespace Device
{
struct PwInitStatusCheckViewResult
{
    bool           isInitialized;
    std::string    language;
    int            maxChannel;
    std::string    publickey;
};

class PwInitStatusCheckView : public BaseCommand, public IniParser
{
public:
    explicit PwInitStatusCheckView(const std::string& logPrefix)
        :BaseCommand(this,logPrefix,std::make_shared<DeviceConnectResponse>(),false,false)
    {
    }

    std::string RequestUri() override {
        return u8"/init-cgi/pw_init.cgi?msubmenu=statuscheck&action=view";
    };

    ResponseBaseSharedPtr ProcessPost() override {
        //응답처리를 하지 않으므로 nullptr로 리턴
        return nullptr;
    }

    PwInitStatusCheckViewResult parseResult;

private:
    bool parseINI(NameValueText& iniDoc) override
    {
        parseResult.isInitialized = iniDoc.getBool("Initialized");
        parseResult.language = iniDoc.getString("Language");
        parseResult.maxChannel = iniDoc.getInt("MaxChannel");
        parseResult.publickey = iniDoc.getString("PublicKey");

        return true;
    }
};

}
}
