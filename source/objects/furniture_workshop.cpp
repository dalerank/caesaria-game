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

#include "city/helper.hpp"
#include "core/gettext.hpp"
#include "game/resourcegroup.hpp"
#include "timber_logger.hpp"
#include "objects_factory.hpp"

using namespace constants;
using namespace gfx;

REGISTER_CLASS_IN_OVERLAYFACTORY( objects::furniture_workshop, FurnitureWorkshop)

bool FurnitureWorkshop::canBuild( const CityAreaInfo& areaInfo ) const
{
  return Factory::canBuild( areaInfo );
}

bool FurnitureWorkshop::build( const CityAreaInfo& info )
{
  Factory::build( info );

  city::Helper helper( info.city );
  bool haveTimberLogger = !helper.find<TimberLogger>( objects::lumber_mill ).empty();

  _setError( haveTimberLogger ? "" : _("##need_timber_for_work##") );

  return true;
}

FurnitureWorkshop::FurnitureWorkshop() : Factory(good::timber, good::furniture, objects::furniture_workshop, Size(2) )
{
  setPicture( ResourceGroup::commerce, 117 );
  _fgPicturesRef().resize( 3 );
}

void FurnitureWorkshop::_storeChanged()
{
  _fgPicturesRef()[1] = inStockRef().empty() ? Picture() : Picture::load( ResourceGroup::commerce, 155 );
  _fgPicturesRef()[1].setOffset( 47, 0 );
}
