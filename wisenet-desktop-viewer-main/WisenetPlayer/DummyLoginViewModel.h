#pragma once

#include <QObject>

class DummyLoginViewModel : public QObject
{
    Q_OBJECT

public:
    Q_PROPERTY(QString id READ id NOTIFY idChanged)
    Q_PROPERTY(QString password READ password NOTIFY passwordChanged)
    Q_PROPERTY(bool logout READ logout NOTIFY logoutChanged)
    Q_PROPERTY(QString loginId READ loginId NOTIFY loginIdChanged)

    QString id() const { return ""; }
    QString password() const { return ""; }
    bool logout() const { return false; }
    QString loginId() { return ""; }

signals:
    void idChanged(QString& arg);
    void passwordChanged(QString& arg);
    void logoutChanged(bool logout);
    void loginIdChanged();
};
