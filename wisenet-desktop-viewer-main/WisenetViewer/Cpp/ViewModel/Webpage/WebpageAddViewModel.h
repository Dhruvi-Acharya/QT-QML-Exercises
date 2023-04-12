#pragma once

#include <QObject>
#include <QDebug>
#include "QCoreServiceManager.h"

class WebpageAddViewModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString webpageName READ webpageName WRITE setWebpageName NOTIFY webpageNameChanged)
    Q_PROPERTY(QString url READ url WRITE setUrl NOTIFY urlChanged)
    Q_PROPERTY(bool useAuth READ useAuth WRITE setUseAuth NOTIFY useAuthChanged)
    Q_PROPERTY(QString userId READ userId WRITE setUserId NOTIFY userIdChanged)
    Q_PROPERTY(QString userPassword READ userPassword WRITE setUserPassword NOTIFY userPasswordChanged)

public:
    explicit WebpageAddViewModel(QObject* parent = nullptr);
    ~WebpageAddViewModel();

    static WebpageAddViewModel* getInstance(){
        static WebpageAddViewModel instance;
        return &instance;
    }

    QString webpageName() const
    {
        return m_webpageName;
    }

    void setWebpageName(const QString arg)
    {
        m_webpageName = arg;
        emit webpageNameChanged(arg);
    }

    QString url() const
    {
        return m_url;
    }

    void setUrl(const QString arg)
    {
        m_url = arg;
        emit urlChanged(arg);
    }

    bool useAuth() const
    {
        return m_useAuth;
    }

    void setUseAuth(const bool useAuth)
    {
        m_useAuth = useAuth;
        emit useAuthChanged(useAuth);
    }

    QString userId() const
    {
        return m_userId;
    }

    void setUserId(const QString userId)
    {
        m_userId = userId;
        emit userIdChanged(userId);
    }

    QString userPassword() const
    {
        return m_userPasswrd;
    }

    void setUserPassword(const QString userPassword)
    {
        m_userPasswrd = userPassword;
        emit userPasswordChanged(userPassword);
    }

public slots:
    void saveWebpage();
    void resetModel();
    void setWebpage(QString webpageId);

signals:
    void webpageNameChanged(QString name);
    void urlChanged(QString url);
    void useAuthChanged(bool userAuth);
    void userIdChanged(QString userId);
    void userPasswordChanged(QString userPassword);

private:
    QString m_webpageId;
    QString m_webpageName;
    QString m_url;
    bool m_useAuth;
    QString m_userId;
    QString m_userPasswrd;
};
