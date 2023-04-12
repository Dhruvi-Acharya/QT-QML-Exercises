#pragma once

#include <QObject>
#include <QQuickItem>
#include <QQuickView>
#include <QDebug>
#include <QThread>
#include <QSettings>
#include "QCoreServiceManager.h"
#include "WisenetViewerDefine.h"

using namespace WisenetViewerDefine;

class LoginViewModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString userGuid READ userGuid WRITE setUserGuid NOTIFY userGuidChanged)
    Q_PROPERTY(QString id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(bool savePasswordChecked READ savePasswordChecked WRITE setSavePasswordChecked NOTIFY savePasswordCheckedChanged)
    Q_PROPERTY(bool autologinChecked READ autologinChecked WRITE setAutologinChecked NOTIFY autologinCheckedChanged)
    Q_PROPERTY(bool logout READ logout WRITE setLogout NOTIFY logoutChanged)
    Q_PROPERTY(bool hashPassword READ hashPassword WRITE setHashPassword NOTIFY hashPasswordChanged)
    Q_PROPERTY(QString loginId READ loginId NOTIFY loginIdChanged)
    Q_PROPERTY(bool exportAuthenticated READ exportAuthenticated WRITE setExportAuthenticated NOTIFY exportAuthenticatedChanged)

public:
    LoginViewModel();
    ~LoginViewModel();

    static LoginViewModel* getInstance(){
        static LoginViewModel instance;
        return &instance;
    }

    Q_INVOKABLE void loginExecute(QString id, QString password, bool saveIdPass, bool autologin);
    Q_INVOKABLE void loginExecute();
    Q_INVOKABLE void loadLocalSetting();
signals:
    void userGuidChanged();
    void idChanged(QString& arg);
    void passwordChanged(QString& arg);
    void savePasswordCheckedChanged(bool arg);
    void autologinCheckedChanged(bool arg);
    void logoutChanged(bool logout);
    void hashPasswordChanged(bool hash);
    void hashPasswordDisplayedChanged(bool hashDisplayed);
    void loginIdChanged();
    void exportAuthenticatedChanged();
    // QML
    void openErrorMessageDialog(int errorCode, int lockedSeconds=0);
    void loginFinished();

    // C++
    void SendRequest(const WisenetViewerDefine::ViewerRequestSharedPtr&);
    void resetPassword();

public slots:
    // QML

    void logoutExecute();


public:
    QString userGuid() {return m_userGuid;}
    QString id() const { return m_id; }
    QString password() const { return m_password; }
    bool savePasswordChecked() const { return m_saveIdChecked; }
    bool autologinChecked() const { return m_autologinChecked; }
    bool logout() const { return m_logout; }
    QString loginId();
    void checkAllDevicePasswordExpiry();
    void sendSystemLog(const Wisenet::Core::EventLog& eventLog);
    bool hashPassword() { return m_hashPassword; }
    bool exportAuthenticated() { return m_exportAuthenticated; }

    void setUserGuid(QString guid)
    {
        if(m_userGuid != guid){
            m_userGuid = guid;
            emit userGuidChanged();
        }
    }

    void setId(const QString& arg)
    {
        m_id = arg;
        emit idChanged(m_id);
    }

    void setPassword(const QString& arg)
    {
        m_password = arg;
        emit passwordChanged(m_password);
    }

    void setSavePasswordChecked(const bool arg)
    {
        m_saveIdChecked = arg;
        emit savePasswordCheckedChanged(m_saveIdChecked);
    }

    void setAutologinChecked(const bool arg)
    {
        m_autologinChecked = arg;
        emit autologinCheckedChanged(m_autologinChecked);
    }

    void setLogout(const bool logout)
    {
        m_logout = logout;
        emit logoutChanged(logout);

        if(m_logout)
            setExportAuthenticated(false);
    }

    void setHashPassword(const bool hash)
    {
        m_hashPassword = hash;
        emit hashPasswordChanged(hash);
    }

    void setExportAuthenticated(const bool authenticated)
    {
        m_exportAuthenticated = authenticated;
        emit exportAuthenticatedChanged();
    }

private:
    QString m_userGuid="";
    QString m_id = "";
    QString m_password = "";
    bool m_hashPassword = false;
    bool m_saveIdChecked = false;
    bool m_autologinChecked = false;
    bool m_logout = false;
    bool m_exportAuthenticated = false;
};
