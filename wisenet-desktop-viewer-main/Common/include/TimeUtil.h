#pragma once

#include <QDateTime>
#include <cstdint>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/algorithm/string.hpp>

namespace Wisenet
{
namespace Common
{

/*
 * Returns the number of milliseconds since 1970-01-01T00:00:00 Universal Coordinated Time.
 * This number is like the POSIX time_t variable, but expressed in milliseconds instead.
 */
inline int64_t currentUtcMsecs()
{
    return QDateTime::currentMSecsSinceEpoch();
}


/*
 * Returns the number of milliseconds since 1970-01-01T00:00:00 Universal Coordinated Time from iso dateTime
 */
inline int64_t utcMsecsFromIsoString(const std::string& isoDateTime)
{
    auto qString = QString::fromStdString(isoDateTime);
    auto dateTime = QDateTime::fromString(qString, Qt::DateFormat::ISODateWithMs);
    return dateTime.toMSecsSinceEpoch();
}

/*
 * Returns the number of milliseconds since 1970-01-01T00:00:00 Universal Coordinated Time from RFC3339
 */
inline long long utcMsecsFromRfc3339(const std::string& time)
{
    std::istringstream ss(time);
    struct std::tm stm = {};

    ss >> std::get_time(&stm, "%Y-%m-%dT%H:%M:%S");

    boost::posix_time::ptime pTime = boost::posix_time::ptime_from_tm(stm);
    boost::posix_time::ptime epoch(boost::gregorian::date(1970,1,1));
    boost::posix_time::time_duration::sec_type x = (pTime - epoch).total_seconds();

    return time_t(x)*1000;
}

/*
 * Returns local time string from utcMsecs
 * ISO 8601 extended format: either yyyy-MM-dd for dates or yyyy-MM-ddTHH:mm:ss (e.g. 2017-07-24T15:46:29),
 * or with a time-zone suffix (Z for UTC otherwise an offset as [+|-]HH:mm) where appropriate for combined dates and times.
 * ISO 8601 extended format, including milliseconds if applicable.
 */
inline std::string utcMsecsToLocalIsoString(int64_t msecs, bool displayMsec = false)
{
    auto dateTime = QDateTime::fromMSecsSinceEpoch(msecs);

    if (displayMsec)
        return dateTime.toString(Qt::ISODate).toStdString();

    return dateTime.toString(Qt::ISODateWithMs).toStdString();
}


/*
 * Returns utc time string from utcMsecs
 * ISO 8601 extended format: either yyyy-MM-dd for dates or yyyy-MM-ddTHH:mm:ss (e.g. 2017-07-24T15:46:29),
 * or with a time-zone suffix (Z for UTC otherwise an offset as [+|-]HH:mm) where appropriate for combined dates and times.
 * ISO 8601 extended format, including milliseconds if applicable.
 */
inline std::string utcMsecsToUtcIsoString(int64_t msecs, bool displayMsec = false)
{
    auto dateTime = QDateTime::fromMSecsSinceEpoch(msecs, Qt::TimeSpec::UTC);
    if (displayMsec)
        return dateTime.toString(Qt::ISODateWithMs).toStdString();

    return dateTime.toString(Qt::ISODate).toStdString();
}


/*
 * Returns the number of seconds since 1970-01-01T00:00:00 Universal Coordinated Time.
 * This number is like the POSIX time_t variable.
 */
inline int64_t currentUtcSecs()
{
    return QDateTime::currentSecsSinceEpoch();
}

/*
 * Returns the number of seconds since 1970-01-01T00:00:00 Universal Coordinated Time from iso dateTime
 */
inline int64_t utcSecsFromIsoString(const std::string& isoDateTime)
{
    auto qString = QString::fromStdString(isoDateTime);
    auto dateTime = QDateTime::fromString(qString, Qt::DateFormat::ISODate);
    return dateTime.toSecsSinceEpoch();
}


/*
 * Returns utc millseconds to seconds (POSIX time_t)
 */
inline int64_t msecsToSecs(int64_t msecs)
{
    return msecs/1000;
}

/*
 * Returns utc time string from utcSecs
 * ISO 8601 extended format: either yyyy-MM-dd for dates or yyyy-MM-ddTHH:mm:ss (e.g. 2017-07-24T15:46:29),
 * or with a time-zone suffix (Z for UTC otherwise an offset as [+|-]HH:mm) where appropriate for combined dates and times.
 * ISO 8601 extended format, including milliseconds if applicable.
 */
inline std::string utcSecsToUtcIsoString(int64_t secs)
{
    auto dateTime = QDateTime::fromSecsSinceEpoch(secs, Qt::TimeSpec::UTC);
    return dateTime.toString(Qt::ISODate).toStdString();
}


/*
 * Returns local time string from utcSecs
 * ISO 8601 extended format: either yyyy-MM-dd for dates or yyyy-MM-ddTHH:mm:ss (e.g. 2017-07-24T15:46:29),
 * or with a time-zone suffix (Z for UTC otherwise an offset as [+|-]HH:mm) where appropriate for combined dates and times.
 * ISO 8601 extended format, including milliseconds if applicable.
 */
inline std::string utcSecsToLocalIsoString(int64_t secs)
{
    auto dateTime = QDateTime::fromSecsSinceEpoch(secs);
    return dateTime.toString(Qt::ISODate).toStdString();
}


}
}

