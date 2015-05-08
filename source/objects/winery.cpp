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

#include "winery.hpp"

#include "core/logger.hpp"
#include "good/stock.hpp"
#include "objects_factory.hpp"
#include "city/statistic.hpp"
#include "game/resourcegroup.hpp"

using namespace gfx;

REGISTER_CLASS_IN_OVERLAYFACTORY(object::wine_workshop, Winery)

Winery::Winery() : Factory(good::grape, good::wine, object::wine_workshop, Size(2) )
{
  _picture().load( ResourceGroup::commerce, 86 );

  _fgPictures().resize(3);
}

bool Winery::canBuild( const city::AreaInfo& areaInfo ) const
{
  return Factory::canBuild( areaInfo );
}

bool Winery::build( const city::AreaInfo& info )
{
  Factory::build( info );

  bool haveVinegrad = !city::statistic::getObjects<Building>( info.city, object::vinard ).empty();

  _setError( haveVinegrad ? "" : "##need_grape##" );

  return true;
}

void Winery::_storeChanged()
{
  _fgPicture(1) = inStockRef().empty() ? Picture() : Picture( ResourceGroup::commerce, 153 );
  _fgPicture(1).setOffset( 40, -10 );
}
