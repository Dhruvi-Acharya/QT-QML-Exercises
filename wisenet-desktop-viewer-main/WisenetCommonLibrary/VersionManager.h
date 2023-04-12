#pragma once

#include <QObject>

class VersionManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int s1Support READ s1Support WRITE setS1Support NOTIFY s1SupportChanged)

public:
    static VersionManager* Instance()
    {
        static VersionManager instance;
        return &instance;
    }

    explicit VersionManager(QObject *parent = nullptr): QObject(parent)
    {

    }

    bool s1Support() const{
        return m_s1Support;
    }
    void setS1Support(bool supportS1){
        if (m_s1Support != supportS1) {
            m_s1Support = supportS1;
            emit s1SupportChanged(m_s1Support);
        }
    }

signals:
    void s1SupportChanged(bool s1Support);

private:
    bool m_s1Support = false;
};
