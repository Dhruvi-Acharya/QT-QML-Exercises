#pragma once

#include <QObject>
#include <QDebug>
#include "QCoreServiceManager.h"
#include "WisenetViewerDefine.h"

using namespace WisenetViewerDefine;

class InitialPasswordSettingViewModel : public QObject
{
    Q_OBJECT
public:
    explicit InitialPasswordSettingViewModel(QObject* parent = nullptr);
    ~InitialPasswordSettingViewModel();
    static InitialPasswordSettingViewModel* getInstance(){
        static InitialPasswordSettingViewModel instance;
        return &instance;
    }

    Q_INVOKABLE void initializePassword(QString password);
public:
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChangedEvent)
    Q_PROPERTY(bool savePasswordChecked READ savePasswordChecked WRITE setSavePasswordChecked NOTIFY savePasswordCheckedChanged)

    QString password() const
    {
        return m_password;
    }

    void setPassword(const QString& arg)
    {
        m_password = arg;
        emit passwordChangedEvent(m_password);
    }

    bool savePasswordChecked() const
    {
        return m_savePasswordChecked;
    }

    void setSavePasswordChecked(const bool arg)
    {
        m_savePasswordChecked = arg;
        emit savePasswordCheckedChanged(m_savePasswordChecked);
    }

signals:
    void passwordChangedEvent(QString& arg);
    void savePasswordCheckedChanged(bool arg);
    void SendRequest(const WisenetViewerDefine::ViewerRequestSharedPtr&);

public slots:


private:
    QString m_password = "";
    bool m_savePasswordChecked = false;
};
