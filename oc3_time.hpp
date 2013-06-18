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


#ifndef __OPENCAESAR3_DATETIME_H_INCLUDE_
#define __OPENCAESAR3_DATETIME_H_INCLUDE_

#include <time.h>

class DateTime
{
public:
    typedef enum { dateLess=-1, dateEquale=0, dateMore=1 } DATE_EQUALE_FEEL;

    static const DateTime invalid;
    static const int maxDayNumber = 7;
    static const int maxMonthNumber = 12;

    unsigned char getHour() const;
    unsigned char getMonth() const;
    int getYear() const;
    unsigned char getMinutes() const;
    unsigned char getDay() const;
    unsigned char getDayOfWeek() const;
    unsigned char getSeconds() const;

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

    DateTime& appendMinutes( int minute );

    DateTime& appendHour( int hour );

    DateTime& operator=( time_t t );

    static const char* getDayName( unsigned char d );
    static const char* getMonthName( unsigned char d );

    unsigned char getMonthLength() const;
    unsigned char getWeekNumber() const;

    bool isValid() const;

    bool operator > ( const DateTime& other ) const;

    bool operator >= ( const DateTime& other ) const;

    bool operator < ( const DateTime& other );

    bool operator <= ( const DateTime& other );

    bool operator == ( const DateTime& other );
    bool operator == ( const DateTime& other ) const;

    bool operator != ( const DateTime& other );

    static DateTime getCurrenTime();
    static unsigned int getElapsedTime();

private:
    unsigned int seconds;
    unsigned int minutes;
    unsigned int hour;
    unsigned int day;
    unsigned int month;
    int year;

    time_t to_time_t() const;

    int getMonthToDate_( const time_t& end );

    int isEquale_( const time_t& b );

    int getDaysToDate_( const time_t& future ) const;
};

#endif
