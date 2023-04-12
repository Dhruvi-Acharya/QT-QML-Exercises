/*********************************************************************************
 * Copyright(c) 2023 Hanwha Vision Co., Ltd.  All rights reserved.
 *
 * This software is copyrighted by, and is the sole property of Hanwha Vision.
 * All rigths, title, ownership, or other interests in the software remain the
 * property of Hanwha Vision. This software may only be used in accordance with
 * the corresponding license agreement. Any unauthorized use, duplication,
 * transmission, distribution, or disclosure of this software is expressly
 * forbidden.
 *
 * This Copyright notice may not be removed or modified without prior written
 * consent of Hanwha Vision.
 *
 * Hanwha Vision reserves the right to modify this software without notice.
 *
 * Hanwha Vision Co., Ltd.
 * KOREA
 * https://www.hanwhavision.com/
 *********************************************************************************/
#pragma once

#include <QObject>
#include "QCoreServiceManager.h"
#include "../OpenSource/libs/smtpclient-for_qt/include/smtpclient.h"
#include "../OpenSource/libs/smtpclient-for_qt/include/mimemessage.h"
#include "../OpenSource/libs/smtpclient-for_qt/include/mimetext.h"

class EventEmailViewModel : public QObject
{
    Q_OBJECT
    // Test Email - Host
    Q_PROPERTY(QString serverAddress READ serverAddress WRITE setServerAddress NOTIFY serverAddressChanged)
    // Test Email - ID
    Q_PROPERTY(QString id READ id WRITE setId NOTIFY idChanged)
    // Test Email - Password
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    // Test Email - Port
    Q_PROPERTY(QString port READ port WRITE setPort NOTIFY portChanged)
    // Test Email - Sender (xxx@gmail.com)
    Q_PROPERTY(QString sender READ sender WRITE setSender NOTIFY senderChanged)
    // Test Email - Receiver (xxx@gmail.com)
    Q_PROPERTY(QString receiver READ receiver WRITE setReceiver NOTIFY receiverChanged)
    // Test Email - Authentication Check
    Q_PROPERTY(bool authentication READ authentication WRITE setAuthentication NOTIFY authenticationChanged)

public:
    explicit EventEmailViewModel(QObject *parent = nullptr);

public:
    Q_INVOKABLE int sendTestEmail(const QString& host, const QString& port, const QString& id, const QString& pwd, const QString& fromMail, const QString& toMail, const QString& subject, const QString& part, bool auth);
    Q_INVOKABLE bool saveEmail();
    Q_INVOKABLE void readEmail();

    // setter
    void setServerAddress(const QString& serverAddress);
    void setId(const QString& id);
    void setPassword(const QString& password);
    void setPort(const QString& port);
    void setSender(const QString& sender);
    void setReceiver(const QString& receiver);
    void setAuthentication(bool authentication);

    // getter
    QString serverAddress() { return m_serverAddress; }
    QString id() { return m_id; }
    QString password() { return m_password; }
    QString port() { return m_port; }
    QString sender() { return m_sender; }
    QString receiver() { return m_receiver; }
    bool authentication() { return m_authentication; }

signals:
    void serverAddressChanged();
    void idChanged();
    void passwordChanged();
    void portChanged();
    void senderChanged();
    void receiverChanged();
    void authenticationChanged();

private:
    //std::shared_ptr<Wisenet::Core::EventEmail> m_email;

    QString m_serverAddress = "";
    QString m_id = "";
    QString m_password = "";
    QString m_port = "";
    QString m_sender = "";
    QString m_receiver = "";
    bool m_authentication = false;
};
