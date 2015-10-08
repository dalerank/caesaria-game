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
//
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "datetimehelper.hpp"
#include "core/utils.hpp"
#include "core/format.hpp"
#include "core/gettext.hpp"
#include "core/metric.hpp"

namespace utils
{


template<class T>
std::string _date2str(const T& time, bool drawDays, bool roman)
{
  std::string month = fmt::format( "##month_{0}_short##", (int)time.month() );
  std::string age = fmt::format( "##age_{0}##", time.age() );
  std::string year;
  std::string text;
  std::string dayStr;
  if( drawDays)
  {
    if( !roman )
      dayStr = utils::i2str( time.day() );
    else
      dayStr = utils::toRoman( time.day() );
  }

  int yearNum = abs( time.year() );
  if( !roman )
    year = utils::i2str( yearNum );
  else
    year = utils::toRoman( yearNum );

  text = utils::format( 0xff, "%s %s %s %s", dayStr.c_str(), _( month ), year.c_str(), _( age ) );
  return text;
}

std::string date2str(const RomanDate  &time, bool drawDays)
{
  return _date2str( time, drawDays, metric::Measure::isRoman() );
}

std::string date2str(const DateTime &time, bool drawDays){  return _date2str( time, drawDays, false );}

}//end namespace util
