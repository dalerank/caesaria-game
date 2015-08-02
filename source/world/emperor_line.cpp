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

#include "emperor_line.hpp"
#include "core/saveadapter.hpp"
#include "core/variant_map.hpp"

namespace world
{

struct EmperorInfo
{
  std::string name;
  DateTime beginReign;
  VariantMap options;

  void load( const VariantMap& stream )
  {
    beginReign = stream.get( "date" ).toDateTime();
    name = stream.get( "name" ).toString();
    options = stream;
  }
};

class EmperorLine::Impl
{
public:
  typedef std::map< DateTime, EmperorInfo> ChangeInfo;

  ChangeInfo changes;
};

EmperorLine& EmperorLine::instance()
{
  static EmperorLine inst;
  return inst;
}

std::string EmperorLine::getEmperor(DateTime time)
{
  for( auto it : _d->changes )
  {
    if( it.first >= time )
      return it.second.name;
  }

  return "";
}

VariantMap EmperorLine::getInfo( const std::string& name) const
{
  for( auto it : _d->changes )
  {
    if( name == it.second.name )
      return it.second.options;
  }

  return VariantMap();
}

void EmperorLine::load(vfs::Path filename)
{
  _d->changes.clear();

  VariantMap opts = config::load( filename );
  for( auto it : opts )
  {
    EmperorInfo info;
    info.load( it.second.toMap() );

    _d->changes[ info.beginReign ] = info;
  }
}

EmperorLine::EmperorLine() : _d( new Impl )
{

}

}//end namespace world
