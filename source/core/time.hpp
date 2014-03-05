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


#ifndef __CAESARIA_DATETIME_H_INCLUDE_
#define __CAESARIA_DATETIME_H_INCLUDE_

#include <time.h>

class DateTime
{
public:
    typedef enum { dateLess=-1, dateEquale=0, dateMore=1 } DATE_EQUALE_FEEL;

    static const DateTime invalid;
    static const int dayInWeek = 7;
    static const int monthInYear = 12;

    unsigned char hour() const;
    unsigned char month() const;
    int year() const;
    unsigned char minutes() const;
    unsigned char day() const;
    unsigned char dayOfWeek() const;
    unsigned char seconds() const;

    void setHour( unsigned char hour );
    void setMonth( unsigned char month );
    void setYear( unsigned int year );
    void setMinutes( unsigned char minute );
    void setDay( unsigned char day );
    void setSeconds( unsigned char second );

    DateTime( const char* strValue );

    DateTime( int year, unsigned char month, unsigned char day, 
              unsigned char hour=0, unsigned char minute=0, unsigned char sec=0 );

    DateTime( const DateTime& time );

    DateTime();

    DateTime( time_t time );

    DateTime getDate() const;

    DateTime getTime() const;

    int getDaysToDate( const DateTime& future ) const;

    int equale( const DateTime& b );

    int getMonthToDate( const DateTime& end );

    DateTime& appendMonth( int month=1 );

    DateTime& appendDay( int dayNumber=1 );

    //DateTime& appendMinutes( int minute );

    //DateTime& appendHour( int hour );

    DateTime& operator=( time_t t );
    DateTime& operator=( const DateTime& t );

    static const char* getDayName( unsigned char d );
    static const char* getMonthName( unsigned char d );
    int getDaysInMonth() const;

    //unsigned char getMonthLength() const;
    //unsigned char getWeekNumber() const;

    bool isValid() const;

    bool operator>( const DateTime& other ) const;
    bool operator>=( const DateTime& other ) const;
    bool operator<( const DateTime& other ) const;
    bool operator<=( const DateTime& other ) const;
    bool operator==( const DateTime& other ) const;
    bool operator!=( const DateTime& other ) const;

    static DateTime getCurrenTime();
    static unsigned int getElapsedTime();

private:
    unsigned int _seconds;
    unsigned int _minutes;
    unsigned int _hour;
    unsigned int _day;
    unsigned int _month;
    int _year;

    long _toJd() const;

    int _getMonthToDate( const long end );

    int _isEquale( const long b );

    int _getDaysToDate( const long other ) const;
    DateTime _JulDayToDate( const long lJD );
};

#endif //__CAESARIA_DATETIME_H_INCLUDE_
