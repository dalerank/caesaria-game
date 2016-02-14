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

#include "creamery.hpp"

#include "gfx/tile.hpp"
#include "good/helper.hpp"
#include "walker/cart_pusher.hpp"
#include "core/exception.hpp"
#include "gui/info_box.hpp"
#include "pathway/pathway_helper.hpp"
#include "core/gettext.hpp"
#include "game/resourcegroup.hpp"
#include "core/predefinitions.hpp"
#include "gfx/tilemap.hpp"
#include "core/variant_map.hpp"
#include "walker/cart_supplier.hpp"
#include "core/utils.hpp"
#include "good/storage.hpp"
#include "city/statistic.hpp"
#include "core/foreach.hpp"
#include "constants.hpp"
#include "game/gamedate.hpp"
#include "core/logger.hpp"
#include "objects_factory.hpp"

using namespace gfx;

REGISTER_CLASS_IN_OVERLAYFACTORY(object::oil_workshop, Creamery)

Creamery::Creamery() : Factory(good::olive, good::oil, object::oil_workshop, Size(2,2) )
{
  setPicture( info().randomPicture( size() ) );
  _fgPictures().resize(3);
}

bool Creamery::canBuild( const city::AreaInfo& areaInfo ) const
{
  return Factory::canBuild( areaInfo );
}

bool Creamery::build( const city::AreaInfo& info )
{
  Factory::build( info );

  bool haveOliveFarm = info.city->statistic()
                                  .objects
                                  .count( object::olive_farm ) > 0;

  _setError( haveOliveFarm ? "" : _("##need_olive_for_work##") );

  return true;
}

void Creamery::_storeChanged()
{
  _fgPicture(1) = _getSctockImage( inStock().qty() );
}
