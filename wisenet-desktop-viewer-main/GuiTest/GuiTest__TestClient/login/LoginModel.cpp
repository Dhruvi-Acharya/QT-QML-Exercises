#include "LoginModel.h"

LoginModel::LoginModel()
{
    getServiceStatus();
}

LoginModel::~LoginModel()
{

}

void LoginModel::registerQml()
{
    qmlRegisterType<LoginModel>("Wisenet.Qmls", 0, 1, "LoginModel");
}

void LoginModel::login(QString id, QString pw, bool savePassword, bool autoLogin)
{
    emit doLoadingVisible();

    QSettings settings("config/iosettings.ini", QSettings::IniFormat);
    settings.setValue("login/id", id);
    settings.setValue("login/password",pw);
    settings.setValue("login/isSavePassword", savePassword);
    settings.setValue("login/isAutoLogin", autoLogin);

    QCoreServiceManager::Instance().Login(
                id, pw, this,
                [this](const QCoreServiceReplyPtr& reply)
    {
        QVariantList values;
        auto response = std::static_pointer_cast<Wisenet::ResponseBase>(reply->responseDataPtr);

        emit doLoginResponse(!(response->isFailed()));
    });
}

void LoginModel::setAdminPassword(QString pw, bool savePassword)
{
    auto request = std::make_shared<Wisenet::Core::SetupServiceRequest>();

    request->userName = "admin";
    request->newPassword = pw.toStdString();

    isSavePassword = savePassword;

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::SetupService,
                this, request,
                [this, pw](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::ResponseBase>(reply->responseDataPtr);

        if(response->isFailed())
            return;
        else
            login("admin", pw, this->isSavePassword, false);
    });
}

void LoginModel::logout()
{
    auto request = std::make_shared<Wisenet::Core::LogoutRequest>();
    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::Logout,
                this, request, nullptr);

    QSettings settings("config/iosettings.ini", QSettings::IniFormat);
    settings.setValue("login/isAutoLogin", false);

    getSettings();

    if(isSavePassword)
        emit doSetPassword(qPassword, isSavePassword);
}

void LoginModel::getServiceStatus()
{
    auto request = std::make_shared<Wisenet::Core::GetServiceInformationRequest>();
    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::GetServiceInformation,
                this, request,
                [this](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::Core::GetServiceInformationResponse>(reply->responseDataPtr);

        if(0 == response->serviceID.compare("None")) {
            emit doGetServerStatusResponse(false);
            return;
        }

        getSettings();

        if(isAutoLogin) {
            login(id, qPassword, isSavePassword, isAutoLogin);
            emit doLoadingVisible();
        } else if(isSavePassword) {
            emit doSetPassword(qPassword, isSavePassword);
        } else {
            emit doGetServerStatusResponse(true);
        }
    });
}

void LoginModel::getSettings()
{
    QSettings settings("config/iosettings.ini", QSettings::IniFormat);
    id = settings.value("login/id","").toString();
    qPassword = settings.value("login/password","").toString();
    password = qPassword.toStdString();
    isSavePassword = settings.value("login/isSavePassword",false).toBool();
    isAutoLogin = settings.value("login/isAutoLogin",false).toBool();
}


