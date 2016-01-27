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
#include "statistic.hpp"
#include "good/helper.hpp"
#include "city.hpp"
#include "game/gamedate.hpp"
#include "core/variant_map.hpp"
#include "objects/house.hpp"
#include "good/store.hpp"
#include "core/logger.hpp"
#include "events/dispatcher.hpp"
#include "objects/house_level.hpp"
#include "cityservice_factory.hpp"
#include "core/variant_list.hpp"

namespace city
{

namespace {
GAME_LITERALCONST(good)
}

REGISTER_SERVICE_IN_FACTORY(GoodsUpdater,goodsUpdater)

class GoodsUpdater::Impl
{
public:
  DateTime endTime;
  bool isDeleted;
  good::Product gtype;
  object::TypeSet buildings;

  int value;
};

void GoodsUpdater::timeStep(const unsigned int time)
{
  if( game::Date::isWeekChanged() )
  {
    _d->isDeleted = (_d->endTime < game::Date::current());

    Logger::warning( "GoodsUpdater: execute service" );

    BuildingList buildings = _city()->statistic().objects.find<Building>( _d->buildings );
    for( auto building : buildings )
    {
      good::Stock stock( _d->gtype, _d->value, _d->value );
      building->storeGoods( stock, _d->value );
    }
  }
}

bool GoodsUpdater::isDeleted() const {  return _d->isDeleted; }

void GoodsUpdater::load(const VariantMap& stream)
{
  VARIANT_LOAD_TIME_D( _d, endTime, stream )
  VARIANT_LOAD_ANY_D( _d, value, stream )
  VARIANT_LOAD_CLASS_D_LIST( _d, buildings, stream )

  _d->gtype = (good::Product)good::Helper::type( stream.get( literals::good ).toString() );
}

VariantMap GoodsUpdater::save() const
{
  VariantMap ret;
  VARIANT_SAVE_ANY_D( ret, _d, endTime )
  VARIANT_SAVE_ANY_D( ret, _d, value )
  VARIANT_SAVE_CLASS_D( ret, _d, buildings )
  ret[ literals::good    ] = Variant( good::Helper::name( _d->gtype ) );

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
