/*********************************************************************************
 * Copyright(c) 2021 by Hanwha Techwin Co., Ltd.
 *
 * This software is copyrighted by, and is the sole property of Hanwha Techwin.
 * All rigths, title, ownership, or other interests in the software remain the
 * property of Hanwha Techwin. This software may only be used in accordance with
 * the corresponding license agreement. Any unauthorized use, duplication,
 * transmission, distribution, or disclosure of this software is expressly
 * forbidden.
 *
 * This Copyright notice may not be removed or modified without prior written
 * consent of Hanwha Techwin.
 *
 * Hanwha Techwin reserves the right to modify this software without notice.
 *
 * Hanwha Techwin Co., Ltd.
 * KOREA
 * https://www.hanwha-security.com/
 *********************************************************************************/
#pragma once

#include <QObject>
#include <QString>
#include "QCoreServiceManager.h"
#include "MediaLayoutViewModel.h"


class DeviceInfo
{
    Q_GADGET
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString ip READ ip WRITE setIp)
    Q_PROPERTY(QString deviceId READ deviceId WRITE setDeviceId)
    Q_PROPERTY(QString channelId READ channelId WRITE setChannelId)
    Q_PROPERTY(QString status READ status WRITE setStatus)

public:
    QString name() const {return m_name;}
    QString ip() const {return m_ip;}
    QString deviceId() const {return m_deviceId;}
    QString channelId() const {return m_channelId;}
    QString status() const {return m_status;}

    void setName(const QString &name) { m_name = name; }
    void setIp(const QString &ip) { m_ip = ip; }
    void setDeviceId(const QString &deviceId) { m_deviceId = deviceId; }
    void setChannelId(const QString &channelId) { m_channelId = channelId; }
    void setStatus(const QString &status) { m_status = status; }

private:
    QString m_name;
    QString m_ip;
    QString m_deviceId;
    QString m_channelId;
    QString m_status;
};
Q_DECLARE_METATYPE(DeviceInfo)


class WebpageInfo
{
    Q_GADGET
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString url READ url WRITE setUrl)
    Q_PROPERTY(QString webpageId READ webpageId WRITE setWebpageId)

public:
    QString name() const {return m_name;}
    QString url() const {return m_url;}
    QString webpageId() const {return m_webpageId;}

    void setName(const QString &name) { m_name = name; }
    void setUrl(const QString &url) { m_url = url; }
    void setWebpageId(const QString &webpageId) { m_webpageId = webpageId; }

private:
    QString m_name;
    QString m_url;
    QString m_webpageId;
};
Q_DECLARE_METATYPE(WebpageInfo)

class MainModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(MediaLayoutViewModel* layout READ layout NOTIFY layoutChanged)
public:
    explicit MainModel(QObject *parent = nullptr);

    void serviceStart();
    MediaLayoutViewModel* layout() const;

    Q_INVOKABLE void registerDevice(QString ip, QString pw);
    Q_INVOKABLE void addWebpage(QString name, QString url, bool useAuth, QString id, QString pw);
public slots:
    void onCoreServiceEventTriggered(QCoreServiceEventPtr event);

signals:
    void newDeviceRegistered(DeviceInfo deviceInfo);
    void deviceUpdated(DeviceInfo deviceInfo);
    void layoutChanged();
    void newWebpageAdded(WebpageInfo webpageInfo);

private:
    void setDevice(Wisenet::Device::Device& device, Wisenet::Device::DeviceStatus& deviceStatus);
    void updateDevice(Wisenet::Device::DeviceStatus& deviceStatus);
    void setLayout(Wisenet::Core::Layout &layout);
    void setWebpage(Wisenet::Core::Webpage& webpage);

    MediaLayoutViewModel* m_mediaLayout;
};

