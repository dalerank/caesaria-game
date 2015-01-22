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
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

#include "goods_updater.hpp"
#include "game/game.hpp"
#include "objects/construction.hpp"
#include "helper.hpp"
#include "good/goodhelper.hpp"
#include "city.hpp"
#include "game/gamedate.hpp"
#include "core/variant_map.hpp"
#include "objects/house.hpp"
#include "good/goodstore.hpp"
#include "core/logger.hpp"
#include "events/dispatcher.hpp"
#include "objects/house_level.hpp"

using namespace constants;

namespace city
{

namespace {
CAESARIA_LITERALCONST(good)
}

class GoodsUpdater::Impl
{
public:
  typedef std::set<gfx::TileOverlay::Type> BuildingTypes;

  DateTime endTime;
  bool isDeleted;
  good::Product gtype;
  BuildingTypes supportBuildings;

  int value;
};

SrvcPtr GoodsUpdater::create( PlayerCityPtr city )
{
  SrvcPtr ret( new GoodsUpdater( city ) );
  ret->drop();

  return ret;
}

void GoodsUpdater::timeStep(const unsigned int time)
{
  if( game::Date::isWeekChanged() )
  {
    _d->isDeleted = (_d->endTime < game::Date::current());

    Logger::warning( "GoodsUpdater: execute service" );
    Helper helper( _city() );

    foreach( bldType, _d->supportBuildings )
    {
      BuildingList buildings = helper.find<Building>( *bldType );
      foreach( it, buildings )
      {
        good::Stock stock( _d->gtype, _d->value, _d->value );
        (*it)->storeGoods( stock, _d->value );
      }
    }
  }
}

bool GoodsUpdater::isDeleted() const {  return _d->isDeleted; }

void GoodsUpdater::load(const VariantMap& stream)
{
  VARIANT_LOAD_TIME_D( _d, endTime, stream )
  VARIANT_LOAD_ANY_D( _d, value, stream )

  _d->gtype = (good::Product)good::Helper::getType( stream.get( lc_good ).toString() );

  VariantList vl_buildings = stream.get( "buildings" ).toList();
  foreach( it, vl_buildings )
  {
    gfx::TileOverlay::Type type = MetaDataHolder::findType( it->toString() );
    if( type != objects::unknown )
    {
      _d->supportBuildings.insert( type );
    }
  }
}

VariantMap GoodsUpdater::save() const
{
  VariantMap ret;
  VARIANT_SAVE_ANY_D( ret, _d, endTime )
  VARIANT_SAVE_ANY_D( ret, _d, value )

  VariantList vl_buildings;
  foreach( it, _d->supportBuildings )
  {
    vl_buildings.push_back( Variant( MetaDataHolder::findTypename( (gfx::TileOverlay::Type)*it ) ));
  }

  ret[ lc_good    ] = Variant( good::Helper::getTypeName( _d->gtype ) );

  return ret;
}

std::string GoodsUpdater::defaultName() { return "goods_updater"; }
good::Product GoodsUpdater::goodType() const {  return _d->gtype; }

GoodsUpdater::GoodsUpdater( PlayerCityPtr city )
  : Srvc( city, GoodsUpdater::defaultName() ), _d( new Impl )
{
  _d->isDeleted = false;
}

}//end namespace city
