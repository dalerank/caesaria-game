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

#include "desirability_updater.hpp"
#include "game/game.hpp"
#include "objects/construction.hpp"
#include "helper.hpp"
#include "city.hpp"
#include "core/variant_map.hpp"
#include "game/gamedate.hpp"
#include "objects/house.hpp"
#include "core/logger.hpp"
#include "events/dispatcher.hpp"
#include "gfx/tilemap.hpp"

using namespace constants;
using namespace gfx;

namespace city
{

class DesirabilityUpdater::Impl
{
public:
  DateTime endTime;
  bool isDeleted;
  int value;
  bool alsoInfluence;

  void update(PlayerCityPtr city, bool positive );
};

SrvcPtr DesirabilityUpdater::create( PlayerCityPtr city )
{
  DesirabilityUpdater* e = new DesirabilityUpdater( city );

  SrvcPtr ret( e );
  ret->drop();

  return ret;
}

void DesirabilityUpdater::timeStep( const unsigned int time )
{
  if( game::Date::isMonthChanged() )
  {
    _d->isDeleted = (_d->endTime < game::Date::current());

    if( !_d->alsoInfluence )
    {      
      _d->alsoInfluence = true;
      _d->update( _city(), true );
    }
  }
}

std::string DesirabilityUpdater::defaultName() { return "desirability_updater"; }
bool DesirabilityUpdater::isDeleted() const {  return _d->isDeleted; }

void DesirabilityUpdater::destroy( PlayerCityPtr city ){ _d->update( city, false ); }

void DesirabilityUpdater::load(const VariantMap& stream)
{
  _d->endTime = stream.get( "endTime" ).toDateTime();
  _d->value = stream.get( "value" );
  _d->alsoInfluence = stream.get( "alsoInfluence", false );
}

VariantMap DesirabilityUpdater::save() const
{
  VariantMap ret;
  ret[ "endTime" ] = _d->endTime;
  ret[ "value" ] = _d->value;
  ret[ "alsoInfluence" ] = _d->alsoInfluence;

  return ret;
}

DesirabilityUpdater::DesirabilityUpdater(PlayerCityPtr city)
  : Srvc( city, DesirabilityUpdater::defaultName() ), _d( new Impl )
{
  _d->isDeleted = false;
  _d->alsoInfluence = false;
}

void DesirabilityUpdater::Impl::update( PlayerCityPtr city, bool positive)
{
  const TilesArray& tiles = city->tilemap().allTiles();

  foreach( it, tiles )
  {
    (*it)->changeParam( Tile::pDesirability, positive ? value : -value );
  }
}

}//end namespace city
