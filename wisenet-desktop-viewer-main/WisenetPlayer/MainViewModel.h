#pragma once

#include <QObject>
#include <QDebug>
#include "SignatureVerifier.h"

class MainViewModel : public QObject
{
    Q_OBJECT

public:
    explicit MainViewModel(QObject* parent = nullptr);
    ~MainViewModel();

    static MainViewModel* getInstance(){
        static MainViewModel instance;
        return &instance;
    }

    Q_PROPERTY (QString buildDate READ buildDate CONSTANT)

    enum ViewerMode
    {
        Windowed = 0,
        Maximized,
        Minimized,
        FullScreen
    };
    Q_ENUM(ViewerMode)

    void setBuildDate(const QString& dateString);
    QString buildDate() const;
    QPointer<SignatureVerifier> Signature();

    Q_INVOKABLE void openSourceLicense();
    Q_INVOKABLE void setUseHwDecoding(bool useHwDecoding);
    Q_INVOKABLE void checkVerifyResult(QString filePath);

signals:
    void verifyResultChecked(QString filePath, int verifyResult);

private:
    QString _buildDate;
    QPointer<SignatureVerifier> m_signatureVerifier;
};
