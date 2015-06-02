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

#include "roman_celebrates.hpp"
#include "core/variant_map.hpp"
#include "core/saveadapter.hpp"

namespace game
{

struct CelebrateInfo
{
  int month;
  int day;
  std::string description;

  int hash() const { return month * 100 + day; }
  bool operator<(const CelebrateInfo& a)
  {
    return hash() < a.hash();
  }
};

typedef std::map<int, CelebrateInfo> Items;

class Celebrates::Impl
{
public:
  Items items;
};

Celebrates& Celebrates::instance()
{
  static Celebrates inst;
  return inst;
}

void Celebrates::load(vfs::Path path)
{
  VariantMap conf = config::load( path );
  foreach( it, conf )
  {
    VariantMap infoVm = it->second.toMap();
    int day = infoVm.get( "day" );
    int month = infoVm.get( "month" );
    std::string description = infoVm.get( "desc" );

    CelebrateInfo info = { month, day, description };
    _d->items[ info.hash() ] = info;
  }
}

std::string Celebrates::getDescription(int day, int month) const
{
  CelebrateInfo test = { month, day, "" };
  Items::iterator it = _d->items.find( test.hash() );
  if( it != _d->items.end() )
    return it->second.description;

  return "##roman_date_not_celebrate##";
}

Celebrates::Celebrates() : _d( new Impl )
{

}

}//end namespace game
