// This file is part of CaesarIA.
//
// CaesarIA is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CaesarIA is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CaesarIA.  If not, see <http://www.gnu.org/licenses/>.


#include "time.hpp"
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <stdint.h>
#include "requirements.hpp"
#include "core/math.hpp"

#if defined(CAESARIA_PLATFORM_WIN)
    #include "windows.h"
#elif defined(CAESARIA_PLATFORM_UNIX)
    #include <sys/time.h>
#endif //CAESARIA_PLATFORM_UNIX

using namespace std;

const char* age_ad = "ad";
const char* age_bc = "bc";
const char* age_uc = "uc";

const char* const dayNames[ DateTime::daysInWeek ] = { "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday" };
const char* const romanDayNames[ DateTime::daysInWeek  ] = { "Lunae", "Martis", "Mercuri", "Jovis", "Veneris", "Saturni", "Solis" };

const char* const monthNames[ DateTime::monthsInYear ] = { "January", "February", "March", "April",
                                                           "May", "June", "July", "August", "September",
                                                           "October", "November", "December" };

const char* romanMercMonth = "Mercedonius";
const char* const romanMonthNames[ DateTime::monthsInYear ] = { "Martius", "Aprilis", "Maius", "Junius",
                                                                "Quintilis", "Sextilis", "September", "October", "November",
                                                                "December", "Januarius", "Februarius" };


const char* const romanShortMonthNames[ DateTime::monthsInYear ] = { "Mar", "Apr",
                                                                     "May", "Jun", "Qin", "Sxt", "Sep",
                                                                     "Oct", "Nov", "Dec", "Jan", "Feb" };

const char* const shortMonthNames[ DateTime::monthsInYear ] = { "Jan", "Feb", "Mar", "Apr",
                                                                "May", "Jun", "Jul", "Aug",
                                                                "Sep", "Oct", "Nov", "Dec" };


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

int DateTime::_getDaysToDate( const long other ) const {    return abs( (int)(_toJd() - other ) );}
int DateTime::daysTo( const DateTime& future ) const{    return _getDaysToDate( future._toJd() );}
int DateTime::_isEquale( const long b ){    return _toJd() == b ? dateEquale : (_toJd() < b ? dateLess : dateMore ) ;}
int DateTime::equale( const DateTime& b ){    return _isEquale( b._toJd() );}
int DateTime::_getMonthToDate( const long end ) const {    return _getDaysToDate( end ) / 30;}
int DateTime::monthsTo( const DateTime& end ) const {    return _getMonthToDate( end._toJd() );}
bool DateTime::operator!=( const DateTime& other ) const{    return _toJd() != other._toJd();}
bool DateTime::operator==( const DateTime& other ) const{    return _toJd() == other._toJd();}
bool DateTime::operator<=( const DateTime& other ) const{    return _toJd() <= other._toJd();}
bool DateTime::operator<( const DateTime& other ) const{    return _toJd() < other._toJd();}
bool DateTime::operator>=( const DateTime& other ) const{    return _toJd() >= other._toJd();}
bool DateTime::operator>( const DateTime& other ) const{    return _toJd() > other._toJd();}
bool DateTime::isValid() const
{   
    return (_year > -4573 && _year < 9999)
            && (_month<12)
            && (_day<31)
            && (_hour < 24)
            && (_minutes < 60)
            && (_seconds < 60);
}

DateTime& DateTime::appendDay( int dayNumber/*=1 */ )
{
    *this = _JulDayToDate( _toJd() + dayNumber );
    return *this;
}

tm _getOsLocalTime( time_t date )
{
#if defined(CAESARIA_PLATFORM_WIN)
  time_t t;
  tm timeinfo;
  time(&t);
  memcpy( &timeinfo, localtime(&t), sizeof(tm) );
  return timeinfo;
#elif defined(CAESARIA_PLATFORM_UNIX)
  //time(&date);
  return *localtime( &date );
#endif //CAESARIA_PLATFORM_UNIX

  return tm();
}

DateTime& DateTime::appendMonth( int m/*=1 */ )
{
  int sumMonth = _month + m;
  _month = sumMonth % 12;
  _year += sumMonth / 12;

  return *this;
}

DateTime& DateTime::appendWeek(int weekNumber){  return appendDay( weekNumber * 7 ); }

DateTime DateTime::date() const
{
  DateTime ret( *this );
  ret._minutes = ret._hour = ret._seconds = 0;
  return ret;
}

DateTime::DateTime( const DateTime& time )
{
  _seconds = time._seconds;
  _minutes = time._minutes;
  _hour = time._hour;
  _day = time._day;
  _month = time._month;
  _year = time._year;
}

DateTime::DateTime( const char* strValue )
{
  sscanf( strValue, "%d.%u.%u:%u.%u.%u",
          &_year, &_month, &_day, &_hour, &_minutes, &_seconds );
}

DateTime::DateTime( int y, unsigned char m, unsigned char d, 
                    unsigned char h, unsigned char mm, unsigned char s )
{
  _year = y;
  _month = m;
  _day = d;
  _hour = h;
  _minutes = mm;
  _seconds = s;
}

DateTime::DateTime()
{
  _seconds = _minutes = _hour = 0;
  _year = -9999;
  _day = _month = 0;
}

unsigned char DateTime::hour() const {    return _hour;}
unsigned char DateTime::month() const {    return _month; }
int DateTime::year() const {     return _year; }
unsigned char DateTime::minutes() const {     return _minutes; }
unsigned char DateTime::day() const {     return _day; }
unsigned char DateTime::seconds() const  {     return _seconds; }
void DateTime::setHour( unsigned char h ) {     _hour = h; }
void DateTime::setMonth( unsigned char m ) {     _month = m; }
void DateTime::setYear( unsigned int y ) {     _year = y; }
void DateTime::setMinutes( unsigned char m )  {    _minutes = m; }
void DateTime::setDay( unsigned char d ) { _day = d; }
void DateTime::setSeconds( unsigned char s ) { _seconds = s; }

DateTime DateTime::currenTime()
{
	tm d;

#if defined(CAESARIA_PLATFORM_WIN)
    _getsystime( &d );
#elif defined(CAESARIA_PLATFORM_UNIX) || defined(CAESARIA_PLATFORM_HAIKU)
    time_t rawtime;
    ::time( &rawtime );

    d = *localtime( &rawtime );
#endif //CAESARIA_PLATFORM_UNIX

  return DateTime( d.tm_year+1900, d.tm_mon, d.tm_mday, d.tm_hour, d.tm_min, d.tm_sec );
}

unsigned char DateTime::dayOfWeek() const {  return ( (int) ( _toJd() % 7L ) ); }
const char* DateTime::dayName( unsigned char d ){   return dayNames[ math::clamp<int>( d, 0, 6 ) ];}
const char* DateTime::monthName( unsigned char d ){  return monthNames[ math::clamp<int>( d, 0, 11 ) ];}
const char* DateTime::shortMonthName(unsigned char d) { return shortMonthNames[ math::clamp<int>( d, 0, 11 ) ]; }

int DateTime::daysInMonth() const
{
  return ( _month!=2
              ?( (_month%2) ^ (_month>7) )+30
              :( ((!(_year % 400) || !( _year % 4 )) && ( _year % 25 )) ? 29 : 28 )
               );
}

const char *DateTime::age() const { return _year > 0 ? age_ad : age_bc; }

DateTime DateTime::time() const
{
  DateTime ret( *this );
  ret._year = ret._month = ret._day = 0;
  return ret;
}

unsigned int DateTime::elapsedTime()
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
  _set( val );

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
  ret._day = (int) ( t1 - 2447L * mo / 80L );
  t1 = mo / 11L;
  ret._month = (int) ( mo + 2L - 12L * t1 );
  ret._year = (int) ( 100L * ( t2 - 49L ) + yr + t1 );

  // Correct for BC years
  if ( ret._year <= 0 )
  {
    ret._year -= 1;
  }

  return ret;
}

long DateTime::_toJd() const
{
  long jul_day;

  long lmonth = (long)_month, lday = (long)_day, lyear = (long)_year;

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

void DateTime::_set(const DateTime& val )
{
  _seconds = val._seconds;
  _minutes = val._minutes;
  _hour = val._hour;
  _day = val._day;
  _month = val._month;
  _year = val._year;
}

const char* RomanDate::age() const { return age_uc; }
const char* RomanDate::dayName(unsigned char d) { return romanDayNames[ math::clamp<int>( d, 0, 6 ) ]; }
const char* RomanDate::monthName(unsigned char d) { return romanMonthNames[ math::clamp<int>( d, 0, 11 ) ];}
const char* RomanDate::shortMonthName(unsigned char d) { return romanShortMonthNames[ math::clamp<int>( d, 0, 11 ) ]; }

RomanDate::RomanDate(const DateTime& date)
{
  _set( date );
  _year += abUrbeCondita;
}


