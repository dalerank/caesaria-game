// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.


#include "time.hpp"
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <stdint.h>
#include "requirements.hpp"

#if defined(CAESARIA_PLATFORM_WIN)
    #include "windows.h"
#elif defined(CAESARIA_PLATFORM_UNIX)
    #include <sys/time.h>
#endif //CAESARIA_PLATFORM_UNIX

using namespace std;

const char* dayNames[ DateTime::dayInWeek ] = { "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday" };
const char* monthNames[ DateTime::monthInYear ] = { "January", "February", "March", "April",
                                                    "May", "June", "July", "August", "September",
                                                    "October", "November", "December" };

const DateTime DateTime::invalid = DateTime();

void _convertToDateTime( DateTime& dateTime, const tm& val )
{
    dateTime.setSeconds(val.tm_sec);
    dateTime.setMinutes(val.tm_min);
    dateTime.setHour(val.tm_hour);
    dateTime.setDay(val.tm_mday);
    dateTime.setMonth(val.tm_mon);
    dateTime.setYear(val.tm_year + 1900);
}

int DateTime::_getDaysToDate( const long other ) const
{
    return abs( (int)(_toJd() - other ) );
}

int DateTime::getDaysToDate( const DateTime& future ) const
{
    return _getDaysToDate( future._toJd() );
}

int DateTime::_isEquale( const long b )
{
    return _toJd() == b ? dateEquale : (_toJd() < b ? dateLess : dateMore ) ;
}

int DateTime::equale( const DateTime& b )
{
    return _isEquale( b._toJd() );
}

int DateTime::_getMonthToDate( const long end )
{
    return _getDaysToDate( end ) / 30;
}

int DateTime::getMonthToDate( const DateTime& end )
{
    return _getMonthToDate( end._toJd() );
}

bool DateTime::operator!=( const DateTime& other ) const
{
    return _toJd() != other._toJd();
}

bool DateTime::operator==( const DateTime& other ) const
{
    return _toJd() == other._toJd();
}

bool DateTime::operator<=( const DateTime& other ) const
{
    return _toJd() <= other._toJd();
}

bool DateTime::operator<( const DateTime& other ) const
{
    return _toJd() < other._toJd();
}

bool DateTime::operator>=( const DateTime& other ) const
{
    return _toJd() >= other._toJd();
}

bool DateTime::operator>( const DateTime& other ) const
{
    return _toJd() > other._toJd();
}

bool DateTime::isValid() const
{   
    return (year > -4573 && year < 9999)
            && (month<12)
            && (day<31)
            && (hour < 24)
            && (minutes < 60)
            && (seconds < 60);
}

/*DateTime& DateTime::appendHour( int hour )
{
    return appendMinutes( 60 * hour );
}*/

/*DateTime& DateTime::appendMinutes( int minute )
{
    *this = (time_t)( to_time_t() + minute * 60 );
    return *this;
}*/

DateTime& DateTime::appendDay( int dayNumber/*=1 */ )
{
    *this = _JulDayToDate( _toJd() + dayNumber );
    return *this;
}

tm _getOsLocalTime( time_t date )
{
#if defined(CAESARIA_PLATFORM_WIN)
  tm ret;
  localtime_s( &ret, &date );
  return ret;
#elif defined(CAESARIA_PLATFORM_UNIX)
  //time(&date);
  return *localtime( &date );
#endif //CAESARIA_PLATFORM_UNIX

  return tm();
}

DateTime& DateTime::appendMonth( int m/*=1 */ )
{
  int sumMonth = month + m;
  month = sumMonth % 12;
  year += sumMonth / 12;

  return *this;
}

DateTime DateTime::getDate() const
{
    DateTime ret( *this );
    ret.minutes = ret.hour = ret.seconds = 0;
    return ret;
}

DateTime::DateTime( const DateTime& time )
{
    seconds = time.seconds;
    minutes = time.minutes;
    hour = time.hour;
    day = time.day;
    month = time.month;
    year = time.year;
}

DateTime::DateTime( const char* strValue )
{
  sscanf( strValue, "%04d.%02d.%02d:%02d.%02d.%02d",
          &year, &month, &day, &hour, &minutes, &seconds );
}

DateTime::DateTime( int y, unsigned char m, unsigned char d, 
                    unsigned char h, unsigned char mm, unsigned char s )
{
   year = y;
   month = m;
   day = d;
   hour = h;
   minutes = mm;
   seconds = s;
}

DateTime::DateTime()
{
    seconds = minutes = hour = 0;
    year = day = month = 0;
}

unsigned char DateTime::getHour() const {    return hour;}
unsigned char DateTime::getMonth() const {    return month; }
int DateTime::getYear() const {     return year; }
unsigned char DateTime::getMinutes() const {     return minutes; }
unsigned char DateTime::getDay() const {     return day; }
unsigned char DateTime::getSeconds() const  {     return seconds; }
void DateTime::setHour( unsigned char h ) {     hour = h; }
void DateTime::setMonth( unsigned char m ) {     month = m; }
void DateTime::setYear( unsigned int y ) {     year = y; }
void DateTime::setMinutes( unsigned char m )  {    minutes = m; }
void DateTime::setDay( unsigned char d ) { day = d; }
void DateTime::setSeconds( unsigned char s ) { seconds = s; }

DateTime DateTime::getCurrenTime()
{
	tm d;

#if defined(CAESARIA_PLATFORM_WIN)
    _getsystime( &d );
#elif defined(CAESARIA_PLATFORM_UNIX)
    time_t rawtime;
    time ( &rawtime );

    d = *localtime( &rawtime );
#endif //CAESARIA_PLATFORM_UNIX

  return DateTime( d.tm_year+1900, d.tm_mon, d.tm_mday, d.tm_hour, d.tm_min, d.tm_sec );
}

unsigned char DateTime::getDayOfWeek() const
{
  return ( (int) ( _toJd() % 7L ) );
}

const char* DateTime::getDayName( unsigned char d )
{
   return dayNames[ d ];
}

const char* DateTime::getMonthName( unsigned char d )
{
    return monthNames[ d ];
}

/*unsigned char DateTime::getMonthLength() const
{
    tm d = _getOsLocalTime( to_time_t() );
    int month = d.tm_mon+1;
    return ( month!=2
                ?( (month%2) ^ (month>7) )+30
                :( ((!(d.tm_year % 400) || !( d.tm_year % 4 )) && ( d.tm_year % 25 )) ? 29 : 28 )
           );
}*/

/*unsigned char DateTime::getWeekNumber() const
{
    tm beginTime;
    beginTime.tm_hour = beginTime.tm_min = beginTime.tm_sec = 0;
    beginTime.tm_year = getYear() - 1900;
    beginTime.tm_mon = 0;
    beginTime.tm_mday = 1; //
    time_t crtDay = mktime( &beginTime );
    tm d = _getOsLocalTime( crtDay );
    int _1_jan_day_of_week = (d.tm_wday+5)%7; //
    return (int)((to_time_t()-crtDay)/(24*60*60) + _1_jan_day_of_week)/7; //
}*/

DateTime DateTime::getTime() const
{
    DateTime ret( *this );
    ret.year = ret.month = ret.day = 0;
    return ret;
}

unsigned int DateTime::getElapsedTime()
{
#if defined(CAESARIA_PLATFORM_WIN)
  return ::GetTickCount();
#elif defined(CAESARIA_PLATFORM_UNIX)
  timeval tv;
  gettimeofday(&tv, 0);
  return (uint32_t)(tv.tv_sec * 1000) + (tv.tv_usec / 1000);
#endif //CAESARIA_PLATFORM_UNIX

  return 0;
}

DateTime& DateTime::operator= ( time_t t)
{
    _convertToDateTime( *this, _getOsLocalTime( t ) );
    return *this;
}

DateTime& DateTime::operator=( const DateTime& val )
{ 
  seconds = val.seconds;
  minutes = val.minutes;
  hour = val.hour;
  day = val.day;
  month = val.month;
  year = val.year;

  return *this;
}

DateTime::DateTime( time_t time )
{
    *this = time;
}

DateTime DateTime::_JulDayToDate( const long lJD )
{
  DateTime ret;

  long t1, t2, yr, mo;

  t1 = lJD + 68569L;
  t2 = 4L * t1 / 146097L;
  t1 = t1 - ( 146097L * t2 + 3L ) / 4L;
  yr = 4000L * ( t1 + 1L ) / 1461001L;
  t1 = t1 - 1461L * yr / 4L + 31L;
  mo = 80L * t1 / 2447L;
  ret.day = (int) ( t1 - 2447L * mo / 80L );
  t1 = mo / 11L;
  ret.month = (int) ( mo + 2L - 12L * t1 );
  ret.year = (int) ( 100L * ( t2 - 49L ) + yr + t1 );

  // Correct for BC years
  if ( ret.year <= 0 )
  {
    ret.year -= 1;
  }

  return ret;
}

long DateTime::_toJd() const
{
  long jul_day;

  long lmonth = (long)month, lday = (long)day, lyear = (long)year;

  // Adjust BC years
  if ( lyear < 0 )
  {
    lyear++;
  }

  jul_day = lday - 32075L +
    1461L * ( lyear + 4800L + ( lmonth - 14L ) / 12L ) / 4L +
    367L * ( lmonth - 2L - ( lmonth - 14L ) / 12L * 12L ) / 12L -
    3L * ( ( lyear + 4900L + ( lmonth - 14L ) / 12L ) / 100L ) / 4L;

  return jul_day;
}
