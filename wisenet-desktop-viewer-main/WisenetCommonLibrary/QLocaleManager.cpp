#include "QLocaleManager.h"

QLocaleManager::QLocaleManager(QObject *parent)
    : QObject(parent)
{

}

void QLocaleManager::setLocale(QString language)
{

    if(language == "Korean")
    {
        m_locale = QLocale(QLocale::Korean, QLocale::SouthKorea);
    }
    else if(language == "English-USA")
    {
        m_locale = QLocale(QLocale::English, QLocale::UnitedStates);
    }
    else if(language == "English-UK")
    {
        m_locale = QLocale(QLocale::English, QLocale::UnitedKingdom);
    }
    else if(language == "Chinese")
    {
        m_locale = QLocale(QLocale::Chinese, QLocale::China);
    }
    else if(language == "Croatian")
    {
        m_locale = QLocale(QLocale::Croatian, QLocale::Croatia);
    }
    else if(language == "Czech")
    {
        m_locale = QLocale(QLocale::Czech, QLocale::CzechRepublic);
    }
    else if(language == "Danish")
    {
        m_locale = QLocale(QLocale::Danish, QLocale::Denmark);
    }
    else if(language == "Dutch")
    {
        m_locale = QLocale(QLocale::Dutch, QLocale::Netherlands);
    }
    else if(language == "Finnish")
    {
        m_locale = QLocale(QLocale::Finnish, QLocale::Finland);
    }
    else if(language == "French")
    {
        m_locale = QLocale(QLocale::French, QLocale::France);
    }
    else if(language == "German")
    {
        m_locale = QLocale(QLocale::German, QLocale::Germany);
    }
    else if(language == "Greek")
    {
        m_locale = QLocale(QLocale::Greek, QLocale::Greece);
    }
    else if(language == "Hungarian")
    {
        m_locale = QLocale(QLocale::Hungarian, QLocale::Hungary);
    }
    else if(language == "Italian")
    {
        m_locale = QLocale(QLocale::Italian, QLocale::Italy);
    }
    else if(language == "Japanese")
    {
        m_locale = QLocale(QLocale::Japanese, QLocale::Japan);
    }
    else if(language == "Norwegian")
    {
        m_locale = QLocale(QLocale::NorwegianBokmal, QLocale::Norway);
    }
    else if(language == "Polish")
    {
        m_locale = QLocale(QLocale::Polish, QLocale::Poland);
    }
    else if(language == "Portuguese")
    {
        m_locale = QLocale(QLocale::Portuguese, QLocale::Portugal);
    }
    else if(language == "Romanian")
    {
        m_locale = QLocale(QLocale::Romanian, QLocale::Romania);
    }
    else if(language == "Russian")
    {
        m_locale = QLocale(QLocale::Russian, QLocale::Russia);
    }
    else if(language == "Serbian")
    {
        m_locale = QLocale(QLocale::Serbian, QLocale::Serbia);
    }
    else if(language == "Spanish")
    {
        m_locale = QLocale(QLocale::Spanish, QLocale::Spain);
    }
    else if(language == "Taiwanese")
    {
        m_locale = QLocale(QLocale::Chinese, QLocale::Taiwan);
    }
    else if(language == "Turkish")
    {
        m_locale = QLocale(QLocale::Turkish, QLocale::Turkey);
    }

    QString dateFormat = m_locale.dateFormat(QLocale::FormatType::ShortFormat);

    if(!dateFormat.contains("yyyy"))
    {
        dateFormat = dateFormat.replace("yy", "yyyy");
    }
    m_dateFormat = dateFormat;

    QString shortDateFormat = m_locale.dateFormat(QLocale::FormatType::NarrowFormat);
    m_shortDateFormat = shortDateFormat;

    QString dateTimeFormat = m_locale.dateTimeFormat(QLocale::FormatType::ShortFormat);

    if(!dateTimeFormat.contains("yyyy"))
    {
        dateTimeFormat = dateTimeFormat.replace("yy", "yyyy");
    }

    if(!dateTimeFormat.contains("s"))
    {
        dateTimeFormat = dateTimeFormat.replace("mm", "mm:ss");
    }

    m_dateTimeFormat = dateTimeFormat;


    QString dateTime24hFormat = dateTimeFormat;

    if(!dateTime24hFormat.contains("hh") && dateTime24hFormat.contains("h:") && (dateTime24hFormat.contains("AP") || dateTime24hFormat.contains("ap")))
    {
        dateTime24hFormat = dateTime24hFormat.replace("AP h:", "h:");
        dateTime24hFormat = dateTime24hFormat.replace("h:", "hh:");
        dateTime24hFormat = dateTime24hFormat.replace(":ss AP", ":ss");
        dateTime24hFormat = dateTime24hFormat.replace("AP", "");
        dateTime24hFormat = dateTime24hFormat.replace("ap", "");
    }

    m_dateTime24hFormat = dateTime24hFormat;

    QString clockFormat = m_locale.timeFormat(QLocale::FormatType::ShortFormat);

    if(!clockFormat.contains("s"))
    {
        clockFormat = clockFormat.replace("mm", "mm:ss");
    }

    m_clockFormat = clockFormat;

    qDebug() << "QLocaleManager::setLocale()" << m_dateTimeFormat << m_dateTime24hFormat << m_clockFormat;
}

QString QLocaleManager::getDate(QDateTime dateTime)
{
    return m_locale.toString(dateTime, m_dateFormat);
}

QString QLocaleManager::getShortDate(QDateTime dateTime)
{
    return m_locale.toString(dateTime, m_shortDateFormat);
}

QString QLocaleManager::getDateFromMsec(long long utcTimeMsec)
{
    QDateTime dateTime = QDateTime::fromMSecsSinceEpoch(utcTimeMsec);
    return m_locale.toString(dateTime, m_dateFormat);
}

QString QLocaleManager::getDateTime(QDateTime dateTime)
{
    return m_locale.toString(dateTime, m_dateTimeFormat);
}

QString QLocaleManager::getDateTimeFromMsec(long long utcTimeMsec)
{
    QDateTime dateTime = QDateTime::fromMSecsSinceEpoch(utcTimeMsec);
    return m_locale.toString(dateTime, m_dateTimeFormat);
}

QString QLocaleManager::getDateTimeFromSec(long long utcTimeMsec)
{
    QDateTime dateTime = QDateTime::fromSecsSinceEpoch(utcTimeMsec);
    return m_locale.toString(dateTime, m_dateTimeFormat);
}

QString QLocaleManager::getDateTime24h(QDateTime dateTime)
{
    return m_locale.toString(dateTime, m_dateTime24hFormat);
}

QString QLocaleManager::getDateTime24h(long long utcTimeMsec)
{
    QDateTime dateTime = QDateTime::fromMSecsSinceEpoch(utcTimeMsec);
    return m_locale.toString(dateTime, m_dateTime24hFormat);
}

QString QLocaleManager::getClock()
{
    QDateTime dateTime = QDateTime::currentDateTime();
    return m_locale.toString(dateTime, m_clockFormat);
}

QString QLocaleManager::getDateFormat()
{
    return m_dateFormat;
}

QString QLocaleManager::getYearMonth(QDateTime dateTime)
{
    int yearIndex = m_dateFormat.indexOf('y', 0, Qt::CaseInsensitive);
    int monthIndex = m_dateFormat.indexOf('M', 0, Qt::CaseInsensitive);

    if(yearIndex < monthIndex)
        return m_locale.toString(dateTime, "yyyy MMM");
    else
        return m_locale.toString(dateTime, "MMM yyyy");
}

QString QLocaleManager::getMonth(QDateTime dateTime)
{
    return m_locale.toString(dateTime, "MMM");
}

QString QLocaleManager::getYearMonthDay(QDateTime dateTime)
{
    int yearIndex = m_dateFormat.indexOf('y', 0, Qt::CaseInsensitive);
    int monthIndex = m_dateFormat.indexOf('M', 0, Qt::CaseInsensitive);

    if(yearIndex < monthIndex)
        return m_locale.toString(dateTime, "yyyy MM dd");
    else
        return m_locale.toString(dateTime, "MMM dd yyyy");
}

QString QLocaleManager::getDayName(int i)
{
    return m_locale.dayName(i, QLocale::ShortFormat);
}
