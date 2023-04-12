#pragma once

#include "QCoreServiceManager.h"

#include <QObject>
#include <QQuickItem>
#include <QQuickView>
#include <iostream>
#include <QDebug>
#include <QThread>
#include <QSettings>
using namespace std;

class LoginModel : public QObject
{
    Q_OBJECT

public:
    LoginModel();
    ~LoginModel();

    static void registerQml();
    Q_INVOKABLE void login(QString, QString, bool, bool);
    Q_INVOKABLE void setAdminPassword(QString, bool);
    Q_INVOKABLE void logout();

private:
    std::string password;
    QString id;
    QString qPassword;
    bool isSavePassword = false;
    bool isAutoLogin = false;
    void getServiceStatus();
    void getSettings();

signals:
    void doLoginResponse(bool result);
    void doGetServerStatusResponse(bool result);
    void doSetPassword(QString password, bool isSavePassword);
    void doLoadingVisible();
};

