#include "LoginViewModel.h"
#include "DigestAuthenticator.h"

LoginViewModel::LoginViewModel()
    : QObject(nullptr)
{
    loadLocalSetting();

    qDebug() << "LoginViewModel()";
}

LoginViewModel::~LoginViewModel()
{
    qDebug() << "~LoginViewModel()";
}

void LoginViewModel::loginExecute(QString id, QString password, bool saveIdPass, bool autologin)
{
    m_id = id;
    m_password = password;
    m_saveIdChecked = saveIdPass;
    m_autologinChecked = autologin;
    m_hashPassword = false;
    loginExecute();
}

void LoginViewModel::loginExecute()
{
    QCoreServiceManager::Instance().Settings()->setLoginId(m_id);
    emit loginIdChanged();

    QCoreServiceManager::Instance().Login(
                m_id, m_password, m_hashPassword, this,
                [this](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::Core::LoginResponse>(reply->responseDataPtr);
        setUserGuid(QString::fromStdString(response->userId));

        if(!m_hashPassword && !response->isLdapUser){
            m_password = QString::fromStdString(DigestAuthenticator::hashSpecialSha256(response->userId ,m_password.toStdString()));
            m_hashPassword = true;
        }

        if(m_autologinChecked){
            QCoreServiceManager::Instance().Settings()->setId(m_id);
            QCoreServiceManager::Instance().Settings()->setPassword(m_password);
            QCoreServiceManager::Instance().Settings()->setHashPassword(m_hashPassword);
        }
        else if(m_saveIdChecked)
        {
            QCoreServiceManager::Instance().Settings()->setId(m_id);
            QCoreServiceManager::Instance().Settings()->setPassword("");
            QCoreServiceManager::Instance().Settings()->setHashPassword(false);
        }
        else
        {
            QCoreServiceManager::Instance().Settings()->setId("");
            QCoreServiceManager::Instance().Settings()->setPassword("");
            QCoreServiceManager::Instance().Settings()->setHashPassword(false);
        }

        QCoreServiceManager::Instance().Settings()->setSavePassword(m_saveIdChecked);
        QCoreServiceManager::Instance().Settings()->setAutoLogin(m_autologinChecked);
        QCoreServiceManager::Instance().Settings()->saveSetting();

        // WNV 로그인 성공
        if(response->errorCode == Wisenet::ErrorCode::NoError)
        {
            ViewerRequestSharedPtr showMonitoringRequest(new ViewerRequest);
            showMonitoringRequest->requestType = RequestType::ShowMonitoringPage;
            SendRequest(showMonitoringRequest);

            setLogout(false);

            // 업데이트 매니저 실행
            QCoreServiceManager::Instance().updateManagerInstance()->Start();

#ifdef WISENET_S1_VERSION
            // 모든 디바이스 패스워드 만료 여부 확인
            checkAllDevicePasswordExpiry();
#endif
        }
        else if(response->errorCode == Wisenet::ErrorCode::LdapConnectionError)
        {
            openErrorMessageDialog((int)LoginErrorCode::LdapConnectionFailed);
        }
        else if(response->errorCode == Wisenet::ErrorCode::AccountBlocked)
        {
            openErrorMessageDialog((int)LoginErrorCode::AccountBlocked, response->lockedSeconds);
        }
        else
        {
            openErrorMessageDialog((int)LoginErrorCode::InvalidIdOrPassword);
        }

        loginFinished();
    });
}

void LoginViewModel::logoutExecute()
{
    qDebug() << "LoginViewModel::logoutExecute()";

    setLogout(true);

    QCoreServiceManager::Instance().Settings()->setPassword("");
    QCoreServiceManager::Instance().Settings()->saveSetting();

    QCoreServiceManager::Instance().Logout();
    ViewerRequestSharedPtr request(new ViewerRequest);
    request->requestType = RequestType::ShowLoginPage;
    emit SendRequest(request);

    ViewerRequestSharedPtr logoutRequest(new ViewerRequest);
    logoutRequest->requestType = RequestType::Logout;
    emit SendRequest(logoutRequest);

    setPassword("");
    setHashPassword(false);
    emit resetPassword();
}

void LoginViewModel::loadLocalSetting()
{
    qDebug() << "LoginViewModel::loadLocalSetting()";
    setSavePasswordChecked(QCoreServiceManager::Instance().Settings()->savePassword());
    setAutologinChecked(QCoreServiceManager::Instance().Settings()->autoLogin());

    if(m_autologinChecked){
        setId(QCoreServiceManager::Instance().Settings()->id());
        setPassword(QCoreServiceManager::Instance().Settings()->password());
        setHashPassword(QCoreServiceManager::Instance().Settings()->hashPassword());
    }
    else if(m_saveIdChecked)
    {
        setId(QCoreServiceManager::Instance().Settings()->id());
    }
    else
    {
        setId("");
        setPassword("");
        setHashPassword(false);
    }
}

QString LoginViewModel::loginId()
{
    return QCoreServiceManager::Instance().Settings()->loginId();
}

void LoginViewModel::checkAllDevicePasswordExpiry()
{
    auto deviceList = QCoreServiceManager::Instance().DB()->GetDevices();
    for(auto& deviceIter : deviceList)
    {
        auto request = std::make_shared<Wisenet::Device::DeviceRequestBase>();
        request->deviceID = deviceIter.second.deviceID;

        QCoreServiceManager::Instance().RequestToCoreService(
                    &Wisenet::Core::ICoreService::DeviceGetPasswordExpiry,
                    this, request,
                    [this, request](const QCoreServiceReplyPtr& reply)
        {
            // 패스워드가 만료됐다면 noti에 알림 추가
            // noti 알림을 클릭하면 설정-비밀번호 변경 메뉴로 이동
            auto response = std::static_pointer_cast<Wisenet::Device::DeviceGetPasswordExpiryResponse>(reply->responseDataPtr);

            SPDLOG_DEBUG("LoginViewModel::loginExecute() - DeviceGetPasswordExpiry, isSuccess:{} isPasswordExpired:{} expiryDurationSettingMonths:{}", response->isSuccess(), response->isPasswordExpired, response->expiryDurationSettingMonths);

            if(response->isSuccess())
            {
                // 비밀번호 만료되었다면
                if(true/*response->isPasswordExpired*/)
                {
                    // 알림 리스트에 비밀번호 변경 안내 알림 추가
                    Wisenet::Core::EventLog eventLog;
                    eventLog.isDevice = true;
                    eventLog.serviceUtcTimeMsec = QDateTime::currentMSecsSinceEpoch();
                    eventLog.type = "SystemEvent.DevicePasswordExpiry";
                    eventLog.deviceID = request->deviceID;

                    sendSystemLog(eventLog);
                }
            }
        });
    }
}

void LoginViewModel::sendSystemLog(const Wisenet::Core::EventLog& eventLog)
{
    auto addLogRequest = std::make_shared<Wisenet::Core::AddLogRequest>();
    addLogRequest->eventLogs.push_back(eventLog);

    QCoreServiceManager::Instance().RequestToCoreService(&Wisenet::Core::ICoreService::AddLog,
                                                         this,
                                                         addLogRequest,
                                                         nullptr);
}
