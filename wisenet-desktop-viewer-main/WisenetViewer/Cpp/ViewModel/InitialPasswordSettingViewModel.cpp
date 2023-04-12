#include "InitialPasswordSettingViewModel.h"
#include "DigestAuthenticator.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

InitialPasswordSettingViewModel::InitialPasswordSettingViewModel(QObject *parent) : QObject(parent)
{
    qDebug() << "InitialPasswordSettingViewModel()";
}

InitialPasswordSettingViewModel::~InitialPasswordSettingViewModel()
{
    qDebug() << "~InitialPasswordSettingViewModel()";
}

void InitialPasswordSettingViewModel::initializePassword(QString password)
{
    auto request = std::make_shared<Wisenet::Core::SetupServiceRequest>();

    m_password = password;
    request->userName = "admin";
    request->userID = boost::uuids::to_string(boost::uuids::random_generator()());
    request->newPassword =  DigestAuthenticator::hashSpecialSha256(request->userID, m_password.toStdString());

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::SetupService,
                this, request,
                [this, request](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::ResponseBase>(reply->responseDataPtr);

        if(response->isFailed())
            return;
        else
        {
            SPDLOG_DEBUG("InitialPasswordSettingViewModel::response() m_savePasswordChecked={}", m_savePasswordChecked);
            if(m_savePasswordChecked)
            {
                QCoreServiceManager::Instance().Settings()->setId("admin");
                QCoreServiceManager::Instance().Settings()->setPassword(QString::fromStdString(request->newPassword));
                QCoreServiceManager::Instance().Settings()->setSavePassword(m_savePasswordChecked);
                QCoreServiceManager::Instance().Settings()->setHashPassword(true);
                QCoreServiceManager::Instance().Settings()->saveSetting();
            }

            ViewerRequestSharedPtr viewerRequest(new ViewerRequest);
            viewerRequest->requestType = RequestType::ShowLoginPage;
            SendRequest(viewerRequest);
        }
    });
}
