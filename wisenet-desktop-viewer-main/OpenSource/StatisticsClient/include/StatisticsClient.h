#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QEventLoop>
#include <QThread>

class StatisticsClient : public QObject
{
    Q_OBJECT
public:
    using StatisticsLogger = void (*)(const QString&);

    explicit StatisticsClient(QObject *parent = nullptr);
    ~StatisticsClient();

    void setDefaultKey(const QString &machineId, const QString& version = "10400", const bool test = false);
    void webRequest(const QByteArray& payload);
    void setLogger(StatisticsLogger logger){ m_logFunction = logger; }

public slots:
    void networkReplyReadyRead();
    void performAuthenticatedDatabaseCall();

private:
    void writeLog(QString log)
    {
        if(m_logFunction != nullptr)
            m_logFunction(log);
    }

private:
    QString m_machineUniqueId;
    QString m_version;

    QString m_idToken;

    QNetworkAccessManager* m_networkAccessManager;
    QNetworkReply* m_networkReply;
    QEventLoop m_loop;

    QByteArray m_jsonData;
    StatisticsLogger m_logFunction = nullptr;

    bool m_bTest = false;
};
