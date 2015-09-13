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

#include "furniture_workshop.hpp"

#include "city/statistic.hpp"
#include "core/gettext.hpp"
#include "game/resourcegroup.hpp"
#include "timber_logger.hpp"
#include "good/stock.hpp"
#include "objects_factory.hpp"

using namespace gfx;

REGISTER_CLASS_IN_OVERLAYFACTORY( object::furniture_workshop, FurnitureWorkshop)

bool FurnitureWorkshop::canBuild( const city::AreaInfo& areaInfo ) const
{
  return Factory::canBuild( areaInfo );
}

bool FurnitureWorkshop::build( const city::AreaInfo& info )
{
  Factory::build( info );

  bool haveTimberLogger = info.city->statistic().objects.count<TimberLogger>() > 0;

  _setError( haveTimberLogger ? "" : _("##need_timber_for_work##") );

  return true;
}

FurnitureWorkshop::FurnitureWorkshop() : Factory(good::timber, good::furniture, object::furniture_workshop, Size(2) )
{
  _picture().load( ResourceGroup::commerce, 117 );
  _fgPictures().resize( 3 );
}

void FurnitureWorkshop::_storeChanged()
{
  _fgPictures()[1] = inStockRef().empty() ? Picture() : Picture( ResourceGroup::commerce, 155 );
  _fgPictures()[1].setOffset( 47, 0 );
}
