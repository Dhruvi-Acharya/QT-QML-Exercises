#pragma once

#include <QObject>
#include <QLocale>
#include <QDateTime>
#include <QDebug>

class QLocaleManager : public QObject
{
    Q_OBJECT
public:
    static QLocaleManager* Instance()
    {
        static QLocaleManager instance;
        return &instance;
    }

    explicit QLocaleManager(QObject *parent = nullptr);

    void setLocale(QString language);

public slots:
    QString getDate(QDateTime dateTime);
    QString getShortDate(QDateTime dateTime);
    QString getDateFromMsec(long long utcTimeMsec);
    QString getDateTime(QDateTime dateTime);
    QString getDateTimeFromMsec(long long utcTimeMsec);
    QString getDateTimeFromSec(long long utcTimeSec);
    QString getDateTime24h(QDateTime dateTime);
    QString getDateTime24h(long long utcTimeMsec);
    QString getClock();
    QString getDateFormat();
    QString getYearMonth(QDateTime dateTime);
    QString getMonth(QDateTime dateTime);
    QString getYearMonthDay(QDateTime dateTime);
    QString getDayName(int i);

private:
    QLocale m_locale;
    QString m_dateFormat;
    QString m_dateTimeFormat;
    QString m_dateTime24hFormat;
    QString m_clockFormat;

    QString m_shortDateFormat;
};
