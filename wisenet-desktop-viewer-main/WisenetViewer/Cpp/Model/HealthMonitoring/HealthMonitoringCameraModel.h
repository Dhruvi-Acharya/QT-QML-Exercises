#pragma once

#include <QObject>

class HealthMonitoringCameraModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int channelId READ channelId WRITE setChannelId NOTIFY channelIdChanged)
    Q_PROPERTY(QString modelName READ modelName WRITE setModelName NOTIFY modelNameChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(bool status READ status WRITE setStatus NOTIFY statusChanged)
    Q_PROPERTY(int firmwareVersion READ firmwareVersion WRITE setFirmwareVersion NOTIFY firmwareVersionChanged)
    Q_PROPERTY(QString primaryProfile READ primaryProfile WRITE setPrimaryProfile NOTIFY primaryProfileChanged)
    Q_PROPERTY(QString secondaryProfile READ secondaryProfile WRITE setSecondaryProfile NOTIFY secondaryProfileChanged)
    Q_PROPERTY(QString recordingProfile READ recordingProfile WRITE setRecordingProfile NOTIFY recordingProfileChanged)
    Q_PROPERTY(QString ipAddress READ ipAddress WRITE setIpAddress NOTIFY ipAddressChanged)
public:
    explicit HealthMonitoringCameraModel(QObject *parent = nullptr);

    int channelId(){ return m_channelId; }
    QString modelName(){ return m_modelName; }
    QString name(){ return m_name; }
    bool status(){ return m_status; }
    int firmwareVersion(){ return m_firmwareVersion; }
    QString primaryProfile(){ return m_primaryProfile; }
    QString secondaryProfile(){ return m_secondaryProfile; }
    QString recordingProfile(){ return  m_recordingProfile; }
    QString ipAddress(){ return m_ipAddress; }

    void setChannelId(int channelId){ m_channelId = channelId; }
    void setModelName(QString modelName){ m_modelName = modelName; }
    void setName(QString name){ m_name = name; }
    void setStatus(bool status){ m_status = status; }
    void setFirmwareVersion(int firmwareVersion){ m_firmwareVersion = firmwareVersion; }
    void setPrimaryProfile(QString primaryProfile){ m_primaryProfile = primaryProfile; }
    void setSecondaryProfile(QString secondaryProfile){ m_secondaryProfile = secondaryProfile; }
    void setRecordingProfile(QString recordingProfile){ m_recordingProfile = recordingProfile; }
    void setIpAddress(QString ipAddress){ m_ipAddress = ipAddress; }

signals:
    void channelIdChanged();
    void modelNameChanged();
    void nameChanged();
    void statusChanged();
    void firmwareVersionChanged();
    void primaryProfileChanged();
    void secondaryProfileChanged();
    void recordingProfileChanged();
    void ipAddressChanged();

private:
    int m_channelId = 0;
    QString m_modelName = "";
    QString m_name = "";
    bool m_status = false;
    int m_firmwareVersion = 0;
    QString m_primaryProfile = "";
    QString m_secondaryProfile = "";
    QString m_recordingProfile = "";
    QString m_ipAddress = "";
};
