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

//************************* DeviceConfigBackup ******************************//

class ConfigBackupControl : public BaseCommand
{
public:
    explicit ConfigBackupControl(const std::string& logPrefix, DeviceConfigBackupRequestSharedPtr request)
        :BaseCommand(nullptr,logPrefix,std::make_shared<Wisenet::ResponseBase>(),false,false),
         m_request(request)
    {
    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;
        builder.CreateCgi("system","configbackup", "control");
        if(m_request->password.size() > 0)
            builder.AddParameter("IsEncryptionKeyEncrypted", "True");
        return builder.GetUrl();
    };

    std::string GetFilePath() override {
        return m_request->backupFilePath;
    }

    std::string GetPostData() override
    {
        return m_request->password;
    }


    ResponseBaseSharedPtr ProcessPost() override {
        m_responseBase->errorCode = Wisenet::ErrorCode::NoError;
        return m_responseBase;
    }

private:
   DeviceConfigBackupRequestSharedPtr m_request;

};


void SunapiDeviceClientImpl::DeviceConfigBackup(const DeviceConfigBackupRequestSharedPtr &request,
                                                const ResponseBaseHandler &responseHandler)
{
    if(m_repos->attributes().systemCgi.configBackupEncrypt && request->password.size() > 0){
       DeviceConfigBackupGetRsa(request, responseHandler);
    }
    else
        DeviceConfigBackupInternal(request, responseHandler);
}

void SunapiDeviceClientImpl::DeviceConfigBackupInternal(const DeviceConfigBackupRequestSharedPtr &request,
                                                const ResponseBaseHandler &responseHandler)
{
    SLOG_INFO("SunapiDeviceClientImpl::DeviceConfigBackupInternal");
    asio::post(m_strand, WeakCallback(shared_from_this(),
                                      [this, request, responseHandler]()
    {
        m_httpConfigSession->Close();

        AsyncRequestType type =  AsyncRequestType::HTTPGETDOWNLOAD;
        if(request->password.size() > 0)
            type = AsyncRequestType::HTTPPOSTDOWNLOAD;

        asyncRequest(m_httpConfigSession,
                     std::make_shared<ConfigBackupControl>(m_logPrefix,request),
                     responseHandler,
                     nullptr,
                     type, false);
    }));
}

void SunapiDeviceClientImpl::DeviceConfigBackupGetRsa(const DeviceConfigBackupRequestSharedPtr &request,
                                                const ResponseBaseHandler &responseHandler)
{
    SLOG_INFO("SunapiDeviceClientImpl::DeviceConfigBackupGetRsa");
    DeviceChangeUserPasswordRequestSharedPtr request2;
    auto securityRsa = std::make_shared<SecurityRsa>(m_logPrefix,request2);

    asyncRequest(m_httpCommandSession,
                 securityRsa,
                 responseHandler,
                 [this, securityRsa, request, responseHandler](){

        auto encryptedPassword = Wisenet::Library::SecurityManager::EncryptPassword(securityRsa->m_publicKey, request->password);

        SPDLOG_DEBUG("SunapiDeviceClientImpl::DeviceConfigBackup pass:{} // enc:{}", request->password, encryptedPassword);

        request->password = "";
        request->password = encryptedPassword;

        DeviceConfigBackupInternal(request, responseHandler);

    },AsyncRequestType::HTTPGET, false);
}

//************************* DeviceConfigRestore ******************************//

class ConfigRestoreControl : public BaseCommand
{
public:
    explicit ConfigRestoreControl(const std::string& logPrefix, DeviceConfigRestoreRequestSharedPtr request)
        :BaseCommand(nullptr,logPrefix,std::make_shared<Wisenet::ResponseBase>(),false,false),
         m_request(request)
    {
    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;
        builder.CreateCgi("system","configrestore", "control");

        if(m_request->excludeNetworkSettings)
            builder.AddParameter("ExcludeSettings", "Network");

        if(m_request->password.size() > 0)
            builder.AddParameter("IsDecryptionKeyEncrypted", "True");
        return builder.GetUrl();
    };

    std::string GetFilePath() override {
        return m_request->restoreFilePath;
    }

    std::string GetPostData() override
    {
        return m_request->password;
    }

    ResponseBaseSharedPtr ProcessPost() override {
        m_responseBase->errorCode = Wisenet::ErrorCode::NoError;
        return m_responseBase;
    }

private:
   DeviceConfigRestoreRequestSharedPtr m_request;

};

void SunapiDeviceClientImpl::DeviceConfigRestore(const DeviceConfigRestoreRequestSharedPtr &request,
                                             const ResponseBaseHandler &responseHandler)
{
    if(m_repos->attributes().systemCgi.configRestoreDecrypt && request->password.size() > 0){
       DeviceConfigRestoreGetRsa(request, responseHandler);
    }
    else
        DeviceConfigRestoreInternal(request, responseHandler);
}

void SunapiDeviceClientImpl::DeviceConfigRestoreGetRsa(const DeviceConfigRestoreRequestSharedPtr &request,
                                                const ResponseBaseHandler &responseHandler)
{
    SLOG_INFO("SunapiDeviceClientImpl::DeviceConfigRestoreGetRsa");
    DeviceChangeUserPasswordRequestSharedPtr request2;
    auto securityRsa = std::make_shared<SecurityRsa>(m_logPrefix,request2);

    asyncRequest(m_httpCommandSession,
                 securityRsa,
                 responseHandler,
                 [this, securityRsa, request, responseHandler](){

        auto encryptedPassword = Wisenet::Library::SecurityManager::EncryptPassword(securityRsa->m_publicKey, request->password);

        SPDLOG_DEBUG("SunapiDeviceClientImpl::DeviceConfigBackup pass:{} // enc:{}", request->password, encryptedPassword);

        request->password = "";
        request->password = encryptedPassword;

        DeviceConfigRestoreInternal(request, responseHandler);

    },AsyncRequestType::HTTPGET, false);
}

void SunapiDeviceClientImpl::DeviceConfigRestoreInternal(const DeviceConfigRestoreRequestSharedPtr &request,
                                                const ResponseBaseHandler &responseHandler)
{
    SLOG_INFO("SunapiDeviceClientImpl::DeviceConfigRestoreInternal");
    asio::post(m_strand, WeakCallback(shared_from_this(),
                                      [this, request, responseHandler]()
    {
        SLOG_INFO("Start to restore device configruation!!");

        m_httpConfigSession->Close();

        AsyncRequestType type =  AsyncRequestType::HTTPPOSTENCODEDUPLOAD;
        if(request->password.size() > 0)
            type = AsyncRequestType::HTTPPOSTENCODEDUPLOADPASSWORD;

        asyncRequest(m_httpConfigSession,
                     std::make_shared<ConfigRestoreControl>(m_logPrefix,request),
                     responseHandler,
                     nullptr,
                     type, false);
    }));
}

}
}
