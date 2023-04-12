#pragma once

#include <QObject>
#include <QDebug>
#include <QRegularExpression>
#include "WisenetViewerDefine.h"

using namespace WisenetViewerDefine;

class PasswordStrengthChecker : public QObject
{
    Q_OBJECT
public:
    static PasswordStrengthChecker* getInstance()
    {
        static PasswordStrengthChecker instance;
        return &instance;
    }

    explicit PasswordStrengthChecker(QObject* parent = nullptr);
    ~PasswordStrengthChecker();

    bool isRepeatedValue(QString input, int count);
    bool isConsecutiveValue(QString input, int count);

    Q_INVOKABLE int test(QString password);
    Q_INVOKABLE bool exportPasswordTest(QString password);

private:
    const QString numbers = "[0-9]+";
    const QString letters = "[a-zA-Z]+";
    const QString upperLetters = "[A-Z]+";
    const QString lowerLetters = "[a-z]+";
    const QString symbols = "~!@#$%^&*()_+`-=,./;'[]\<>?:\"{}|";


    const int MinimumLength = 8;
    const int MinimumLengthWithoutSymbols = 10;

    const int RepeatLimitation = 4;
    const int ConsecutiveLimitation = 4;
};
