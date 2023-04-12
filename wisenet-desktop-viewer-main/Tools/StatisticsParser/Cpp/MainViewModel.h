#pragma once
#include <QMap>
#include <QObject>

enum class ColumnNumber
{
    Guid = 1,
    Country,
    MediaResponseMsec,
    InstalledDate,
    LastUpdatedDate,
    ExecutionCount,
    ExecutionMinutes,
    LayoutOpenCount,
    LayoutChannelCount,
    OS,
    Version,
    License,
    Devices,
};

enum class DeviceColumnNumber
{
    Mac = 0,
    Model,
    Channels,
    ChannelsUsed,
    Method,
    RegistrationTime,
};

class MainViewModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)
public:
    explicit MainViewModel(QObject *parent = nullptr);

    QString path(){ return m_path; }
    void setPath(QString path){
        m_path = path;
        emit pathChanged(path);
    }

    std::string QStringToStdString(QString str);
    QString StdStringToQString(std::string str);

public slots:
    void loadJson();
    void openFolder();

signals:
    void pathChanged(QString path);
    void generatedSuccessfully();

private:
    QString m_path;
    QMap<QString, int> m_countryInfo;
    QMap<QString, int> m_osInfo;
};
