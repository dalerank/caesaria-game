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
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "timber_logger.hpp"
#include "constants.hpp"
#include "game/resourcegroup.hpp"
#include "city/city.hpp"
#include "gfx/tilemap.hpp"
#include "core/gettext.hpp"
#include "objects_factory.hpp"

using namespace constants;
using namespace gfx;

REGISTER_CLASS_IN_OVERLAYFACTORY(objects::lumber_mill, TimberLogger)

TimberLogger::TimberLogger()
  : Factory(good::none, good::timber, objects::lumber_mill, Size(2) )
{
  setPicture( ResourceGroup::commerce, 72 );

  _animationRef().load( ResourceGroup::commerce, 73, 10);
  _fgPicturesRef().resize(2);
}

bool TimberLogger::canBuild( const CityAreaInfo& areaInfo ) const
{
   bool is_constructible = WorkingBuilding::canBuild( areaInfo );
   bool near_forest = false;  // tells if the factory is next to a forest

   Tilemap& tilemap = areaInfo.city->tilemap();
   TilesArray area = tilemap.getRectangle( areaInfo.pos + TilePos( -1, -1 ), size() + Size( 2 ), Tilemap::checkCorners );
   foreach( tile, area )
   {
     near_forest |= (*tile)->getFlag( Tile::tlTree );
   }

   const_cast< TimberLogger* >( this )->_setError( near_forest ? "" : _("##lumber_mill_need_forest_near##"));

   return (is_constructible && near_forest);
}
