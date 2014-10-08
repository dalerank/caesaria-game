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
#include "objects/house.hpp"
#include "good/goodstore.hpp"
#include "core/logger.hpp"
#include "events/dispatcher.hpp"
#include "objects/house_level.hpp"

using namespace constants;

namespace city
{

namespace {
CAESARIA_LITERALCONST(endTime)
CAESARIA_LITERALCONST(good)
}

class GoodsUpdater::Impl
{
public:
  DateTime endTime;
  bool isDeleted;
  Good::Type gtype;
  int value;
};

SrvcPtr GoodsUpdater::create()
{
  SrvcPtr ret( new GoodsUpdater() );
  ret->drop();

  return ret;
}

void GoodsUpdater::timeStep(PlayerCityPtr city, const unsigned int time)
{
  if( GameDate::isWeekChanged() )
  {
    _d->isDeleted = (_d->endTime < GameDate::current());

    Logger::warning( "GoodsUpdater: execute service" );
    Helper helper( city );
    HouseList houses = helper.find<House>( building::house );
    foreach( it, houses )
    {
      int qty = 5 * (*it)->spec().level();
      GoodStock stock( _d->gtype, qty, qty );
      (*it)->goodStore().store( stock, qty );
    }
  }
}

bool GoodsUpdater::isDeleted() const {  return _d->isDeleted; }

void GoodsUpdater::load(const VariantMap& stream)
{
  _d->endTime = stream.get( lc_endTime ).toDateTime();
  VARIANT_LOAD_ANY_D( _d, value, stream );
  _d->gtype = (Good::Type)GoodHelper::getType( stream.get( lc_good ).toString() );
}

VariantMap GoodsUpdater::save() const
{
  VariantMap ret;
  ret[ lc_endTime ] = _d->endTime;
  VARIANT_SAVE_ANY_D(ret, _d, value );
  ret[ lc_good    ] = Variant( GoodHelper::getTypeName( _d->gtype ) );

  return ret;
}

std::string GoodsUpdater::defaultName() { return "goods_updater"; }
Good::Type GoodsUpdater::goodType() const {  return _d->gtype; }

GoodsUpdater::GoodsUpdater()
  : Srvc( GoodsUpdater::defaultName() ), _d( new Impl )
{
  _d->isDeleted = false;
}

}//end namespace city
