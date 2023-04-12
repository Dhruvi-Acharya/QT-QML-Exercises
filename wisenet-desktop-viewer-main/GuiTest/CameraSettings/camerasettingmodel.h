#ifndef CAMERASETTINGMODEL_H
#define CAMERASETTINGMODEL_H
#include <QObject>
#include <QVariant>

class QQmlApplicationEngine;


class Profile : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString codec READ getCodec WRITE setCodec)
    Q_PROPERTY(QVariantList  codecList READ getCodecList)
    Q_PROPERTY(QString resolution READ getResolution WRITE setResolution)
    Q_PROPERTY(QVariantList  resolutionList READ getResolutionList)
    Q_PROPERTY(QString bitrateControlType READ getBitrateControlType WRITE setBitrateControlType)
    Q_PROPERTY(QString encodigPriority READ getEncodingPriority WRITE setEncodingPriority)

    Q_PROPERTY(QString maxGov READ getMaxGov)
    Q_PROPERTY(QString govLength READ getGovLength WRITE setGovLength)
    Q_PROPERTY(QString h264Profile READ getH264Profile WRITE setH264Profile)
    Q_PROPERTY(QString entropyCoding READ getEntropyCoding WRITE setEntropyCoding)
public:

    QString getCodec(){ return m_codec;}   
    void setCodec(QString codec){ m_codec = codec; }
    QVariantList  getCodecList() { return m_codecList;}

    QString getResolution(){ return m_resolution;}
    void setResolution(QString resolution){ m_resolution = resolution; }
    QVariantList  getResolutionList() { return m_resolutionList;}

    QString getBitrateControlType(){ return m_bitrateControlType;}
    void setBitrateControlType(QString bitrateControlType){ m_bitrateControlType = bitrateControlType; }

    QString getEncodingPriority(){ return m_encodingPriority;}
    void setEncodingPriority(QString encodingPriority){ m_encodingPriority = encodingPriority; }

    QString getMaxGov(){ return m_maxGov;}
    QString getGovLength(){ return m_govLength;}
    void setGovLength(QString goveLength){ m_govLength= goveLength; }

    QString getH264Profile(){ return m_h264Profile;}
    void setH264Profile(QString h264Profile){ m_h264Profile= h264Profile; }

    QString getEntropyCoding(){ return m_entropyCoding;}
    void setEntropyCoding(QString entropyCoding){ m_entropyCoding= entropyCoding; }

public:
    explicit Profile(QObject *parent = nullptr)
    {
        Q_UNUSED(parent);
        m_codecList.append("MJPEG");
        m_codecList.append("H264");

        m_resolutionList.append("640*480");
        m_resolutionList.append("320*240");

        m_codec = QString("H264");
        m_resolution = QString("320*240");

        m_bitrateControlType = QString("CBR");
        m_encodingPriority = QString("Compression Level");
        m_maxGov = QString("240");
        m_govLength = QString("10");
        m_h264Profile = QString("High");
        m_entropyCoding = QString("CAVLC");
    }

    virtual ~Profile(){;}

private:
    QString m_codec;
    QVariantList  m_codecList;
    QString m_resolution;
    QVariantList  m_resolutionList;

    QString m_bitrateControlType;
    QString m_encodingPriority;
    QString m_maxGov;
    QString m_govLength;
    QString m_h264Profile;
    QString m_entropyCoding;
};

class CameraSettingModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ getName WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString vendor READ getVendor)
    Q_PROPERTY(QString model READ getModel)
    Q_PROPERTY(QString firmware READ getFirmware)
    Q_PROPERTY(QString ipAddress READ getIpAddress)
    Q_PROPERTY(QString webPage READ getWebPage)
    Q_PROPERTY(QString macAddress READ getMacAddress)
    Q_PROPERTY(QString cameraID READ getCameraID)

    Q_PROPERTY(QString id READ getID WRITE setID NOTIFY idChanged)
    Q_PROPERTY(QString password READ getPassword WRITE setPassword NOTIFY passwordChanged)

    Q_PROPERTY(QString primaryStream READ getPrimaryStream)
    Q_PROPERTY(QString secondaryStream READ getSecondaryStream)


public:
    explicit CameraSettingModel(QQmlApplicationEngine* view, QObject *parent = nullptr);
    virtual ~CameraSettingModel();

    QQmlApplicationEngine* m_view;

    void setName(const QString &name);
    void setID(const QString &id);
    void setPassword(const QString &pw);

    QString getVendor() const { return m_vendor; }
    QString getName() const { return m_name; }
    QString getModel() const { return m_model; }
    QString getFirmware() const { return m_firmware; }
    QString getIpAddress() const { return m_ipAddress; }
    QString getWebPage() const { return m_webPage; }
    QString getMacAddress() const { return m_macAddress; }
    QString getCameraID() const { return m_cameraID; }
    QString getID() const { return m_id; }
    QString getPassword() const { return m_password; }

    QString getPrimaryStream() const { return m_primaryStream; }
    QString getSecondaryStream() const { return m_secondaryStream; }

signals:
    void nameChanged();
    void idChanged();
    void passwordChanged();

private:
    QString m_vendor;
    QString m_id;
    QString m_password;
    QString m_name;
    QString m_model;
    QString m_firmware;
    QString m_ipAddress;
    QString m_webPage;
    QString m_macAddress;
    QString m_cameraID;
    QString m_primaryStream;
    QString m_secondaryStream;

    Profile m_primary;
    Profile m_secondary;

};

#endif // CAMERASETTINGMODEL_H
