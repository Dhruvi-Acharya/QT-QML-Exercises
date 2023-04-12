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

void SunapiDeviceClientImpl::DeviceChangeUserPassword(DeviceChangeUserPasswordRequestSharedPtr const& request,
                                   ResponseBaseHandler const& responseHandler)
{
    auto securityRsa = std::make_shared<SecurityRsa>(m_logPrefix,request);

    asyncRequest(m_httpCommandSession,
                securityRsa,
                responseHandler,
                [this, securityRsa, request, responseHandler](){

                    getUser(securityRsa->m_publicKey, request, responseHandler);

                },AsyncRequestType::HTTPGET, false);
}

void SunapiDeviceClientImpl::getUser(std::string publicKey, DeviceChangeUserPasswordRequestSharedPtr const& request, ResponseBaseHandler const& responseHandler)
{
    auto securityUserView = std::make_shared<SecurityUserView>(m_logPrefix, request, m_connectionInfo.user);

    asyncRequest(m_httpCommandSession,
                    securityUserView,
                    responseHandler,
                    [this, publicKey, request, responseHandler, securityUserView](){

        auto itor = securityUserView->m_users.find(m_connectionInfo.user);

        if(itor == securityUserView->m_users.end()) {
            auto response = securityUserView->ResponseBase();
            response->errorCode = Wisenet::ErrorCode::InvalidRequest;
            responseHandler(response);
            return;
        }

        //Encrypt Password (public key가 존재하면 패스워드 암호화를 진행함.)
        if(0 < publicKey.size()) {
            auto encryptedPassword = Wisenet::Library::SecurityManager::EncryptPassword(publicKey, request->password);
            if(!encryptedPassword.empty()) {
                updateEncryptedUserPassword(itor->second.index, encryptedPassword, request, responseHandler);
                return;
            }
        }
        //Update User Password
        updateUserPassword(itor->second.index, request->password, request, responseHandler);

    },AsyncRequestType::HTTPGET,false);
}


void SunapiDeviceClientImpl::updateUserPassword(const std::string& index, const std::string& password,DeviceChangeUserPasswordRequestSharedPtr const& request, ResponseBaseHandler const& responseHandler)
{
    asyncRequest(m_httpCommandSession,
                    std::make_shared<SecurityUserUpdate>(m_logPrefix, request, index, m_connectionInfo.user, false, password),
                    responseHandler,
                    [](){
       // 재접속 진행.
    },AsyncRequestType::HTTPGET,false);
}

void SunapiDeviceClientImpl::updateEncryptedUserPassword(const std::string& index, const std::string& password, DeviceChangeUserPasswordRequestSharedPtr const& request, ResponseBaseHandler const& responseHandler)
{
    asyncRequest(m_httpCommandSession,
                    std::make_shared<SecurityUserUpdate>(m_logPrefix, request, index, m_connectionInfo.user, true, password),
                    responseHandler,
                    [](){
        // 재접속 진행.
    },AsyncRequestType::HTTPPOSTENCODEDDATA,false);
}

}
}
