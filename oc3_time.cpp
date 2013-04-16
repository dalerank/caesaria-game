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


#include "oc3_time.hpp"
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <stdint.h>

#ifdef _WIN32
    #include "oc3_Windows.hpp"
#else
    #include <sys/time.h>
#endif

using namespace std;

string dayNames[ DateTime::maxDayNumber ] = { "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday" };
string monthNames[ DateTime::maxMonthNumber ] = { "January", "February", "March", "April", 
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

int DateTime::getDaysToDate_( const time_t& future ) const
{
    return abs( (int)(to_time_t() - future) );
}

int DateTime::getDaysToDate( const DateTime& future ) const
{
    return getDaysToDate_( future.to_time_t() );
}

int DateTime::isEquale_( const time_t& b )
{
    return to_time_t() == b ? dateEquale : (to_time_t() < b ? dateLess : dateMore ) ;
}

int DateTime::equale( const DateTime& b )
{
    return isEquale_( b.to_time_t() );
}

int DateTime::getMonthToDate_( const time_t& end )
{
    return getDaysToDate_( end ) / 30;
}

int DateTime::getMonthToDate( const DateTime& end )
{
    return getMonthToDate_( end.to_time_t() );
}

bool DateTime::operator!=( const DateTime& other )
{
    return to_time_t() != other.to_time_t();
}

bool DateTime::operator==( const DateTime& other )
{
    return to_time_t() == other.to_time_t();
}

bool DateTime::operator==( const DateTime& other ) const
{
    return to_time_t() == other.to_time_t();
}

bool DateTime::operator<=( const DateTime& other )
{
    return to_time_t() <= other.to_time_t();
}

bool DateTime::operator<( const DateTime& other )
{
    return to_time_t() < other.to_time_t();
}

bool DateTime::operator>=( const DateTime& other ) const
{
    return to_time_t() >= other.to_time_t();
}

bool DateTime::operator>( const DateTime& other ) const
{
    return to_time_t() > other.to_time_t();
}

bool DateTime::isValid() const
{   
    return to_time_t() >= 0;
}

DateTime& DateTime::appendHour( int hour )
{
    return appendMinutes( 60 * hour );
}

DateTime& DateTime::appendMinutes( int minute )
{
    *this = (time_t)( to_time_t() + minute * 60 );
    return *this;
}

DateTime& DateTime::appendDay( int dayNumber/*=1 */ )
{
    *this = (time_t)( to_time_t() + dayNumber * 24 * 60 * 60 );
    return *this;
}

tm _getOsLocalTime( time_t date )
{
#ifdef _WIN32
    tm ret;
    localtime_s( &ret, &date );
    return ret;
#else
    //time(&date);
    return *localtime( &date );
#endif
}

DateTime& DateTime::appendMonth( int m/*=1 */ )
{
    int retMonth = year * 12 + month + m;
    year = (int)retMonth / 12;
    month = retMonth - year * 12;

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

DateTime::DateTime( const string& strValue )
{
    sscanf( strValue.c_str(), "y=%04d m=%02d d=%02d h=%02d mi=%02d",
               &year, &month, &day, &hour, &minutes );
}

DateTime::DateTime( unsigned char y, unsigned char m, unsigned char d, 
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
    day = month = 0;
    year = 1900;
}

unsigned char DateTime::getHour() const {    return hour;}
unsigned char DateTime::getMonth() const {    return month; }
unsigned char DateTime::getYear() const {     return year; }
unsigned char DateTime::getMinutes() const {     return minutes; }
unsigned char DateTime::getDay() const {     return day; }
unsigned char DateTime::getSeconds() const  {     return seconds; }
void DateTime::setHour( unsigned char h ) {     hour = h; }
void DateTime::setMonth( unsigned char m ) {     month = m; }
void DateTime::setYear( unsigned char y ) {     year = y; }
void DateTime::setMinutes( unsigned char m )  {    minutes = m; }
void DateTime::setDay( unsigned char d ) { day = d; }
void DateTime::setSeconds( unsigned char s ) { seconds = s; }

DateTime DateTime::getCurrenTime()
{
	tm d;

#ifdef _WIN32
    _getsystime( &d );
#else
    time_t rawtime;
    time ( &rawtime );

    d = *localtime( &rawtime );
#endif

    return DateTime( d.tm_year+1900, d.tm_mon, d.tm_mday, d.tm_hour, d.tm_min, d.tm_sec );
}

unsigned char DateTime::getDayOfWeek() const
{
    tm d = _getOsLocalTime( to_time_t() );
    return d.tm_wday;
}

string DateTime::getDayName( unsigned char d )
{
   return dayNames[ d ];
}

string DateTime::getMonthName( unsigned char d )
{
    return monthNames[ d ];
}

unsigned char DateTime::getMonthLength() const
{
    tm d = _getOsLocalTime( to_time_t() );
    int month = d.tm_mon+1;
    return ( month!=2
                ?( (month%2) ^ (month>7) )+30
                :( ((!(d.tm_year % 400) || !( d.tm_year % 4 )) && ( d.tm_year % 25 )) ? 29 : 28 )
           );
}

unsigned char DateTime::getWeekNumber() const
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
}

DateTime DateTime::getTime() const
{
    DateTime ret( *this );
    ret.year = ret.month = ret.day = 0;
    return ret;
}

unsigned int DateTime::getElapsedTime()
{
#ifdef _WIN32
        return ::GetTickCount();
#else
        timeval tv;
        gettimeofday(&tv, 0);
        return (uint32_t)(tv.tv_sec * 1000) + (tv.tv_usec / 1000);
#endif
}

DateTime& DateTime::operator= ( time_t t)
{
    _convertToDateTime( *this, _getOsLocalTime( t ) );
    return *this;
}


DateTime::DateTime( time_t time )
{
    *this = time;
}

time_t DateTime::to_time_t() const
{
    tm val;
    val.tm_sec = getSeconds();
    val.tm_min = getMinutes();
    val.tm_hour = getHour();
    val.tm_mday = getDay();
    val.tm_mon = getMonth();
    val.tm_year = getYear() - 1900;

    return mktime( &val );
}
