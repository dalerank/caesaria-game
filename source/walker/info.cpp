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

#include "info.hpp"

#include "core/logger.hpp"
#include "core/saveadapter.hpp"

using namespace constants;

WalkerInfo& WalkerInfo::instance()
{
  static WalkerInfo inst;
  return inst;
}

VariantMap WalkerInfo::getOptions(const walker::Type type )
{
  std::string tname = WalkerHelper::getTypename( type );
  iterator mapIt = instance().find( tname );
  if (mapIt == instance().end())
  {
    Logger::warning("WalkerInfo: Unknown walker info for type %d", type );
    return VariantMap();
  }

  return mapIt->second.toMap();
}

/*void MetaDataHolder::addData(const MetaData &data)
{
  TileOverlay::Type buildingType = data.getType();

  if (hasData(buildingType))
  {
    Logger::warning( "Building is already set %s", data.getName().c_str() );
    return;
  }

  _d->buildings.insert(std::make_pair(buildingType, data));
}*/

WalkerInfo::WalkerInfo()
{}

void WalkerInfo::initialize( const vfs::Path& filename )
{
  const VariantMap& bm = SaveAdapter::load( filename );
  for( VariantMap::const_iterator it=bm.begin(); it != bm.end(); it++ )
  {
    (*this)[ it->first ] = it->second;
  }
}
