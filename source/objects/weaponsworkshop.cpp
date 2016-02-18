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

#include "weaponsworkshop.hpp"
#include "constants.hpp"
#include "gfx/picture.hpp"
#include "good/stock.hpp"
#include "game/resourcegroup.hpp"
#include "objects_factory.hpp"
#include "city/statistic.hpp"

using namespace gfx;

REGISTER_CLASS_IN_OVERLAYFACTORY( object::weapons_workshop, WeaponsWorkshop)

WeaponsWorkshop::WeaponsWorkshop()
  : Factory(good::iron, good::weapon, object::weapons_workshop, Size::square(2) )
{
  setPicture( info().randomPicture( size() ) );

  //_animationRef().load( ResourceGroup::commerce, 109, 6);
  _fgPictures().resize(2);
}

bool WeaponsWorkshop::canBuild( const city::AreaInfo& areaInfo ) const
{
  return Factory::canBuild( areaInfo );
}

bool WeaponsWorkshop::build( const city::AreaInfo& info )
{
  Factory::build( info );

  bool haveIronMine = info.city->statistic().objects.count( object::iron_mine ) > 0;

  _setError( haveIronMine ? "" : "##need_iron_for_work##" );

  return true;
}
