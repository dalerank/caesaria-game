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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#include "notification.hpp"
#include "gamedate.hpp"
#include "core/variant_list.hpp"

namespace notification
{

VariantList Base::save() const
{
  VariantList ret;
  ret << type << date << Variant( objectName ) << Variant( message ) << location;

  return ret;
}

void Base::load(const VariantList& stream)
{
  type = (Type)stream.get( ftype ).toInt();
  date = stream.get( fdate ).toDateTime();
  objectName = stream.get( fname ).toString();
  message = stream.get( fmessage ).toString();
  location = stream.get( flocation ).toPoint();
}

Notification create(const VariantList &stream)
{
  Notification ret;
  ret.load( stream );
  return ret;
}

void Array::eraseOld(const DateTime& date, int ageMonth)
{
  for( auto it=begin(); it != end(); )
  {
    if( it->date.monthsTo( date ) > ageMonth ) { it = erase( it ); }
    else { ++it; }
  }
}

bool Array::contain(Base::Type type) const
{
  for( auto& notification : *this )
  {
    if( notification.type == type )
      return true;
  }

  return false;
}

}//end namespace notification

