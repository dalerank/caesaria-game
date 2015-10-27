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

#include "native.hpp"
#include "game/resourcegroup.hpp"
#include "gfx/tile.hpp"
#include "city/city.hpp"
#include "constants.hpp"
#include "core/variant_map.hpp"
#include "game/gamedate.hpp"
#include "walker/serviceman.hpp"
#include "walker/rioter.hpp"
#include "walker/indigene.hpp"
#include "objects_factory.hpp"

using namespace gfx;

REGISTER_CLASS_IN_OVERLAYFACTORY(object::native_hut, NativeHut)
REGISTER_CLASS_IN_OVERLAYFACTORY(object::native_center, NativeCenter)
REGISTER_CLASS_IN_OVERLAYFACTORY(object::native_field, NativeField)

namespace {
static const int rioterGenerateLevel = 80;
}

NativeBuilding::NativeBuilding(const object::Type type, const Size& size )
: Building( type, size )
{
  setState( pr::inflammability, 0 );
  setState( pr::collapsibility, 0 );
}

void NativeBuilding::save( VariantMap& stream) const 
{
  Building::save(stream);
}

void NativeBuilding::load( const VariantMap& stream)
{
  Building::load(stream);
}

bool NativeBuilding::build( const city::AreaInfo& info )
{
  tile().setFlag( Tile::tlRock, true );
  return Building::build( info );
}

bool NativeBuilding::canDestroy() const { return false; }

NativeHut::NativeHut() : NativeBuilding( object::native_hut, Size(1) )
{
  _picture().load( ResourceGroup::housing, 49 );
  _discontent = 0;
  _day2look = math::random( 90 );
}

void NativeHut::save( VariantMap& stream) const 
{
  Building::save(stream);

  stream[ "discontent" ] = _discontent;
  stream[ "day2look"   ] = _day2look;
}

void NativeHut::load( const VariantMap& stream) {Building::load(stream);}

void NativeHut::timeStep(const unsigned long time)
{
  NativeBuilding::timeStep( time );
  if( game::Date::isDayChanged() )
  {
    _discontent = math::clamp<float>( _discontent+0.5, 0.f, 100.f );
    _day2look--;

    if( math::random( _discontent ) > rioterGenerateLevel )
    {
      auto rioter = Walker::create<NativeRioter>( _city() );
      rioter->send2City( this );
      _discontent = 0;
    }

    if( _day2look < 0 )
    {
      _day2look = 30 + math::random( 60 );
      auto indigene = Walker::create<Indigene>( _city() );
      indigene->send2city( this );
    }
  }
}

void NativeHut::applyService(ServiceWalkerPtr walker)
{
  if( walker->serviceType() == Service::missionary )
  {
    _discontent -= walker->serviceValue();
  }

  NativeBuilding::applyService( walker );
}

float NativeHut::evaluateService(ServiceWalkerPtr walker)
{
  float res = NativeBuilding::evaluateService( walker );

  switch( walker->serviceType() )
  {
  case Service::missionary: res = math::clamp( 100.f - _discontent, 0.f, 100.f ); break;
  default: break;
  }
  return res;
}

float NativeHut::discontent() const { return _discontent; }

NativeCenter::NativeCenter() : NativeBuilding( object::native_center, Size(2) )
{
  _picture().load( ResourceGroup::housing, 51 );
}

void NativeCenter::save( VariantMap&stream) const 
{
  Building::save(stream);
}

void NativeCenter::load( const VariantMap& stream) {Building::load(stream);}

void NativeCenter::store(unsigned int qty)
{

}

NativeField::NativeField() : NativeBuilding( object::native_field, Size(1) )
{
  _progress = 0;
  _picture().load( ResourceGroup::commerce, 13 );
}

void NativeField::save( VariantMap&stream) const 
{
  Building::save(stream);
}

void NativeField::load( const VariantMap& stream) {Building::load(stream);}

void NativeField::timeStep(const unsigned long time)
{
  if( game::Date::isDayChanged() )
  {
    int lastState = _progress / 20;
    _progress = math::clamp( _progress+1, 0u, 100u );

    int currentState = _progress / 20;
    if( lastState != currentState )
    {
      _updatePicture();
    }
  }
}

unsigned int NativeField::progress() const { return _progress; }

unsigned int NativeField::catchCrops()
{
  unsigned int ret = progress();
  _progress = 0;
  _updatePicture();

  return ret;
}

void NativeField::_updatePicture()
{
  _picture().load( ResourceGroup::commerce, 13 + _progress / 20 );
}
