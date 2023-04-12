#include "WebpageAddViewModel.h"

WebpageAddViewModel::WebpageAddViewModel(QObject *parent) : QObject(parent)
{
    qDebug() << "WebpageAddViewModel()";
    m_useAuth = false;
}

WebpageAddViewModel::~WebpageAddViewModel()
{
    qDebug() << "~WebpageAddViewModel()";
}

void WebpageAddViewModel::saveWebpage()
{
    qDebug() << "WebpageAddViewModel::addWebpage() " << m_webpageName;

    QString webpageNameParam = m_webpageName;
    Wisenet::Core::Webpage webpage;
    webpage.name = m_webpageName.toUtf8().constData();

    if(m_webpageId == "")
        webpage.webpageID = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8().constData();
    else
        webpage.webpageID = m_webpageId.toUtf8().constData();

    webpage.url = m_url.toUtf8().constData();
    webpage.useAuth = m_useAuth;
    if(m_useAuth){
        webpage.userId = m_userId.toUtf8().constData();
        webpage.userPw = m_userPasswrd.toUtf8().constData();
    }

    auto request = std::make_shared<Wisenet::Core::SaveWebpageRequest>();
    request->webpage = webpage;

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::SaveWebpage,
                this, request,
                [this, webpageNameParam](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::ResponseBase>(reply->responseDataPtr);
        if (response->isFailed()) {
            qDebug() << "WebpageAddViewModel::addWebpage - RequestToCoreService : add fail " << webpageNameParam;
        }
        else {
            qDebug() << "WebpageAddViewModel::addWebpage - RequestToCoreService : add success " << webpageNameParam;
        }
    });
}

void WebpageAddViewModel::resetModel()
{
    m_webpageId = "";
    setWebpageName("");
    setUrl("");
    setUseAuth(false);
    setUserId("");
    setUserPassword("");
}

void WebpageAddViewModel::setWebpage(QString webpageId)
{
    // 2022.12.28. coverity
    auto db = QCoreServiceManager::Instance().DB();
    if (db == nullptr) {
        return;
    }

    m_webpageId = webpageId;

    // 2023.01.03. coverity
    Wisenet::Core::Webpage webpage;
    bool found = false;
    if (db != nullptr) {
        found = db->FindWebpage(webpageId.toStdString(), webpage);
    }

    if (!found) {
        return;
    }

    QString name = QString::fromUtf8(webpage.name.c_str());
    QString url = QString::fromUtf8(webpage.url.c_str());
    QString userId = QString::fromUtf8(webpage.userId.c_str());

    qDebug() << "WebpageAddViewModel::setWebpage " << webpageId << name << url << userId;

    setWebpageName(name);
    setUrl(url);
    setUseAuth(webpage.useAuth);

    if(webpage.useAuth)
        setUserId(userId);
    else
        setUserId("");

    setUserPassword("");
}
