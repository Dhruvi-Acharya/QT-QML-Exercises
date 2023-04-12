#include "LogoManager.h"
#include <QDir>
#include "QCoreServiceManager.h"
#include <AppLocalDataLocation.h>

LogoManager::LogoManager(QObject *parent) : QObject(parent)
{
    QString appDataPath = GetAppDataLocation() + "/logo";
    QStringList supportedFiles = {"*.jpeg", "*.jpg", "*.png", "*.bmp", "*.svg"};

    QDir logoPath;
    logoPath.setPath(appDataPath);

    QStringList fileList = logoPath.entryList(supportedFiles, QDir::Files);
    qDebug() << "LogoManager() logo folder fileList size" << fileList.size();

    if (fileList.size() == 0) {
        m_useCustomLogo = false;
    }
    else {
        m_useCustomLogo = true;
        m_customLogoPath = appDataPath + "/" + fileList[0];
        qDebug() << "LogoManager() m_customLogoPath" <<  m_customLogoPath;
    }

    qDebug() << "LogoManager() m_useCustomLogo" << m_useCustomLogo;
}
