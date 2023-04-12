#include <QDebug>
#include <QPointer>

class LogoManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY (bool useCustomLogo READ useCustomLogo CONSTANT)
    Q_PROPERTY (QString customLogoPath READ customLogoPath CONSTANT)

public:
    explicit LogoManager(QObject *parent = nullptr);

    static LogoManager* getInstance() {
        static LogoManager instance;
        return &instance;
    }

    bool useCustomLogo()
    {
        return m_useCustomLogo;
    }

    QString customLogoPath()
    {
        return m_customLogoPath;
    }

private:
    bool m_useCustomLogo = false;
    QString m_customLogoPath = "";
};
