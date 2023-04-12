#ifndef DEVICETREETYPE_H
#define DEVICETREETYPE_H

#include <QObject>

class DeviceTreeItemType : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString ip READ ip WRITE setIp NOTIFY ipChanged )
    Q_PROPERTY(int type READ type WRITE setType NOTIFY typeChanged)
public:
    explicit DeviceTreeItemType(QObject *parent = nullptr);
    DeviceTreeItemType(DeviceTreeItemType &other);
    ~DeviceTreeItemType();

    QString name();
    void setName(const QString &name);

    QString ip();
    void setIp(const QString &ip);

    int type();
    void setType(int type);

signals:
    void nameChanged();
    void ipChanged();
    void typeChanged();

private:
    QString m_name;
    QString m_ip;
    int m_type;
};

#endif // DEVICETREETYPE_H
