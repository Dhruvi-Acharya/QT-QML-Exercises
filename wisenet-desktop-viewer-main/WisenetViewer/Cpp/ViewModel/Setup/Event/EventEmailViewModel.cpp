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
#include <boost/asio/ip/host_name.hpp>

#include "EventEmailViewModel.h"
#include "QCoreServiceManager.h"
#include "EmailManager.h"
#include "LogSettings.h"

EventEmailViewModel::EventEmailViewModel(QObject *parent)
{
    Q_UNUSED(parent);
}

int EventEmailViewModel::sendTestEmail(const QString& host, const QString& port, const QString& id, const QString& pwd, const QString& fromMail, const QString& toMail, const QString& subject, const QString& part, bool auth)
{
    SmtpClient smtpClient(host, port.toInt(), SmtpClient::SslConnection);

    // id
    smtpClient.setUser(id);

    // pwd
    smtpClient.setPassword(pwd);

    MimeMessage mimeMessage;
    MimeText mimeText;

    // from
    auto host_name = boost::asio::ip::host_name();
    auto name = qgetenv("USER");
    if (name.isEmpty()) {
        name = qgetenv("USERNAME");
    }

    QString senderName = "Wisenet Viewer[" + QString::fromStdString(host_name) + "/" + name + "]";

    mimeMessage.setSender(new EmailAddress(fromMail, senderName));

    // to
    mimeMessage.addRecipient(new EmailAddress(toMail, toMail));

    // subject
    mimeMessage.setSubject(subject);

    // content
    mimeText.setText(part);

    mimeMessage.addPart(&mimeText);

    // connect
    if (!smtpClient.connectToHost()) {
        SPDLOG_INFO("[EventEmailViewModel] sendTestEmail - fail (connect)");
        return -1; // fail - connect
    }

    // login
    if (auth) {
        if (!smtpClient.login()) {
            SPDLOG_INFO("[EventEmailViewModel] sendTestEmail - fail (login)");
            return -2; // fail - login
        }
    }

    // send
    if (!smtpClient.sendMail(mimeMessage)) {
        SPDLOG_INFO("[EventEmailViewModel] sendTestEmail - fail (send)");
        return -3; // fail - send
    }

    SPDLOG_INFO("[EventEmailViewModel] sendTestEmail - success !!!");
    smtpClient.quit();

    setServerAddress(host);
    setPort(port);
    setId(id);
    setPassword(pwd);
    setSender(fromMail);
    setReceiver(toMail);
    setAuthentication(auth);

    return 1; // success
}

bool EventEmailViewModel::saveEmail()
{
    auto request = std::make_shared<Wisenet::Core::SaveEventEmailRequest>();
    request->eventEmail = std::make_shared<Wisenet::Core::EventEmail>();

    request->eventEmail->serverAddress = m_serverAddress.toStdString();
    request->eventEmail->isAuthentication = m_authentication;
    request->eventEmail->id = m_id.toStdString();
    request->eventEmail->password = m_password.toStdString();
    request->eventEmail->sender = m_sender.toStdString();
    request->eventEmail->receiver = m_receiver.toStdString();
    request->eventEmail->port = m_port.toStdString();

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::SaveEventEmail,
                this, request,
                [=](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::ResponseBase>(reply->responseDataPtr);
        if(response->isSuccess()){
            return true;
        }
        else {
            return false;
        }
    });

    return true;
}

void EventEmailViewModel::readEmail()
{
    auto email = QCoreServiceManager::Instance().DB()->GetEventEmail();

    if (nullptr == email)
    {
        setId("");
        setPassword("");
        setServerAddress("");
        setPort("");
        setSender("");
        setReceiver("");
        setAuthentication(false);

        return;
    }

//    SPDLOG_INFO("=====> [EventEmailViewModel] id: {}", email->id.c_str());
//    SPDLOG_INFO("=====> [EventEmailViewModel] password: {}", email->password.c_str());
//    SPDLOG_INFO("=====> [EventEmailViewModel] address: {}", email->serverAddress.c_str());
//    SPDLOG_INFO("=====> [EventEmailViewModel] port: {}", email->port.c_str());
//    SPDLOG_INFO("=====> [EventEmailViewModel] sender: {}", email->sender.c_str());
//    SPDLOG_INFO("=====> [EventEmailViewModel] receiver: {}", email->receiver.c_str());
//    SPDLOG_INFO("=====> [EventEmailViewModel] authentication: {}", email->isAuthentication);

    setId(QString::fromStdString(email->id));
    setPassword(QString::fromStdString(email->password));
    setServerAddress(QString::fromStdString(email->serverAddress));
    setPort(QString::fromStdString(email->port));
    setSender(QString::fromStdString(email->sender));
    setReceiver(QString::fromStdString(email->receiver));
    setAuthentication(email->isAuthentication);
}

void EventEmailViewModel::setServerAddress(const QString& serverAddress)
{
    //SPDLOG_DEBUG("EventEmailViewModel::setServerAddress:{}", serverAddress.toStdString());
    if(serverAddress != m_serverAddress){
        m_serverAddress = serverAddress;
        emit this->serverAddressChanged();
    }
}

void EventEmailViewModel::setId(const QString& id)
{
    //SPDLOG_DEBUG("EventEmailViewModel::setId:{}", id.toStdString());
    if(id != m_id){
        m_id = id;
        emit this->idChanged();
    }
}

void EventEmailViewModel::setPassword(const QString& password)
{
    //SPDLOG_DEBUG("EventEmailViewModel::setPassword:{}", password.toStdString());
    if(password != m_password){
        m_password = password;
        emit this->passwordChanged();
    }
}

void EventEmailViewModel::setPort(const QString& port)
{
    //SPDLOG_DEBUG("EventEmailViewModel::setPort:{}", port.toStdString());
    if(port != m_port){
        m_port = port;
        emit this->portChanged();
    }
}

void EventEmailViewModel::setSender(const QString& sender)
{
    //SPDLOG_DEBUG("EventEmailViewModel::setSender:{}", sender.toStdString());
    if(sender != m_sender){
        m_sender = sender;
        emit this->senderChanged();
    }
}

void EventEmailViewModel::setReceiver(const QString& receiver)
{
    //SPDLOG_DEBUG("EventEmailViewModel::setReceiver:{}", receiver.toStdString());
    if(receiver != m_receiver){
        m_receiver = receiver;
        emit this->receiverChanged();
    }
}

void EventEmailViewModel::setAuthentication(bool authentication)
{
    //SPDLOG_DEBUG("EventEmailViewModel::setAuthentication:{}", authentication);
    if(authentication != m_authentication){
        m_authentication = authentication;
        emit this->authenticationChanged();
    }
}
