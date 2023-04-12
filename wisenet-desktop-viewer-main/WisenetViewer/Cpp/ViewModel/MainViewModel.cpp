#include "MainViewModel.h"
#include <QOperatingSystemVersion>
#include <QDesktopServices>
//#include <../WisenetCommonLibrary/LanguageManager.h>
#include "LanguageManager.h"
#include "AppLocalDataLocation.h"

MainViewModel::MainViewModel(QObject *parent) : QObject(parent)
{
    qDebug() << "MainViewModel()";
    m_x = 0;
    m_y = 0;
}

MainViewModel::~MainViewModel()
{
    qDebug() << "~MainViewModel()";
}

void MainViewModel::getServiceInformation()
{
    if(_loginStatus == true){
        emit showMonitoringView();
        return;
    }

    qDebug() << "MainViewModel::getServiceInformation()";

    auto request = std::make_shared<Wisenet::Core::GetServiceInformationRequest>();
    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::GetServiceInformation,
                this, request,
                [this](const QCoreServiceReplyPtr& reply)
    {
        QOperatingSystemVersion::OSType osType = QOperatingSystemVersion::currentType();

        auto setting = QCoreServiceManager::Instance().Settings();

        if (setting == nullptr)  {
            return;
        }

        bool licenseAgreed = setting->licenseAgreed();

        if(!licenseAgreed || !setting->containCollectingAgreed())
        {
            emit showLicenseAgreementView();
            return;
        }

        auto response = std::static_pointer_cast<Wisenet::Core::GetServiceInformationResponse>(reply->responseDataPtr);

        if(0 == response->serviceID.compare("None"))
        {
            showPasswordChangeView();
        }
        else
        {
            showLoginView();
        }
    });
}

void MainViewModel::setLicenseAgreed()
{
    auto setting = QCoreServiceManager::Instance().Settings();

    if (setting != nullptr)
    {
        setting->setLicenseAgreed();
        getServiceInformation();
    }
}

void MainViewModel::setCollectingAgreed(const bool collectingAgreed)
{
    auto setting = QCoreServiceManager::Instance().Settings();

    if (setting != nullptr)
    {
        setting->setCollectingAgreed(collectingAgreed);
    }
}

void MainViewModel::setScreen(QStringList info)
{
    m_screenInfo = info;
}

void MainViewModel::setWindowPosition(int x, int y, int viewerMode)
{
    qDebug() << "MainViewModel::setWindowPosition" << x << y;
    m_x = x;
    m_y = y;
    _viewerMode = (MainViewModel::ViewerMode)viewerMode;
}

void MainViewModel::setWidth(int width)
{
    m_width = width;
}

void MainViewModel::setHeight(int height)
{
    m_height = height;
}

void MainViewModel::openManual()
{
    auto setting = QCoreServiceManager::Instance().Settings();

    if (setting == nullptr) {
        return;
    }

    QString appPath = QCoreApplication::applicationDirPath();
    QString language = setting->language();

    if(language != "Korean" && language != "Japanese" && language != "Russian")
        language = "English";

    QString manualPath = "/Manual/" + language + "/index.html";
    SPDLOG_DEBUG("openManual() = {}", (appPath + manualPath).toStdString());

#ifdef Q_OS_LINUX
    QString docPath = GetDefaultMediaLocation();
    SPDLOG_DEBUG("openManual() in Ubuntu = {}", (docPath + manualPath).toStdString());
    bool openResult = QDesktopServices::openUrl(QUrl::fromLocalFile(docPath + manualPath));
    SPDLOG_DEBUG("openUrl() result = {}", openResult);
#else
    QDesktopServices::openUrl(QUrl::fromLocalFile(appPath + manualPath));
#endif
}
