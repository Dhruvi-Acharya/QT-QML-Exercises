#include "MainViewModel.h"
#include "DecoderManager.h"
#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include <QDesktopServices>
#include <QUrl>

MainViewModel::MainViewModel(QObject *parent) : QObject(parent)
{
    qDebug() << "MainViewModel()";
    m_signatureVerifier = new SignatureVerifier(this);
}

MainViewModel::~MainViewModel()
{
    qDebug() << "~MainViewModel()";

    // 2022.12.27. converity
    if (m_signatureVerifier != nullptr) {
        m_signatureVerifier->Stop();
    }
}

void MainViewModel::setBuildDate(const QString& dateString)
{
    _buildDate = dateString;
}

QString MainViewModel::buildDate() const
{
    return _buildDate;
}

void MainViewModel::openSourceLicense()
{
    QString fileName = QCoreApplication::applicationDirPath() + "/OpenSourceLicense_WisenetPlayer.txt";

    QFile file(fileName);
    if(QFileInfo::exists(fileName)){
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
    }
}

void MainViewModel::setUseHwDecoding(bool useHwDecoding)
{
    DecoderManager::getInstance()->SetUseHwDecoding(useHwDecoding);
}

QPointer<SignatureVerifier> MainViewModel::Signature()
{
    return m_signatureVerifier;
}

void MainViewModel::checkVerifyResult(QString filePath)
{
    // 2022.12.27. coverity
    if (m_signatureVerifier == nullptr) {
        return;
    }

    // 2023.01.05. coverity
    QMap<QString, int> resultMap;
    if (m_signatureVerifier != nullptr) {
        std::mutex mutex;
        mutex.lock();
        resultMap = m_signatureVerifier->getVerifyResultMap();
        mutex.unlock();
    }

    if(resultMap.contains(filePath)) {
        emit verifyResultChecked(filePath, resultMap[filePath]);
    }
    else {
        emit verifyResultChecked(filePath, 1);
    }
}
