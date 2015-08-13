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
#include "core/gettext.hpp"

namespace utils
{


template<class T>
std::string _date2str(const T& time, bool drawDays)
{
  std::string month = utils::format( 0xff, "##month_%d_short##", time.month() );
  std::string age = utils::format( 0xff, "##age_%s##", time.age() );
  std::string text;
  if( drawDays)
    text = utils::format( 0xff, "%d %s %d %s", time.day(), _( month ), abs( time.year() ), _( age ) );
  else
    text = utils::format( 0xff, "%s %d %s", _( month ), abs( time.year() ), _( age ) );

  return text;
}

std::string date2str(const RomanDate  &time, bool drawDays){  return _date2str( time, drawDays );}
std::string date2str(const DateTime &time, bool drawDays){  return _date2str( time, drawDays );}

}//end namespace util
