#pragma once

#include <QObject>
#include <QDebug>

class MenuViewModel : public QObject
{
    Q_OBJECT
public:
    explicit MenuViewModel(QObject* parent = nullptr);
    ~MenuViewModel();

public:
    Q_PROPERTY (bool LoginStatus READ GetLoginStatus WRITE SetLoginStatus NOTIFY LoginStatusChangedEvent);

    bool GetLoginStatus() const
    {
        return _loginStatus;
    }

    void SetLoginStatus(const bool arg)
    {
        _loginStatus = arg;
        emit LoginStatusChangedEvent(_loginStatus);
    }

signals:
    void LoginStatusChangedEvent(bool arg);

private:
    bool _loginStatus = false;
};
