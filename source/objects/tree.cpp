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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#include "tree.hpp"
#include "game/resourcegroup.hpp"
#include "gfx/helper.hpp"
#include "city/statistic.hpp"
#include "gfx/tilemap.hpp"
#include "objects/construction.hpp"
#include "objects_factory.hpp"
#include "game/gamedate.hpp"
#include "gfx/animation_bank.hpp"
#include "core/variant_map.hpp"

using namespace gfx;

REGISTER_CLASS_IN_OVERLAYFACTORY(object::tree, Tree)

enum class State { well=0, burning, burnt };
class Tree::Impl
{
public:
  bool flat;
  int age;
  int health;
  State state;
  bool spreadFire;
  DateTime lastTimeGrow;
};

Tree::Tree()
  : Overlay( object::tree, Size(1) ), _d( new Impl )
{
  _d->age = math::random( 15 );
  _d->flat = false;
  _d->health = 100;
  _d->state = State::well;
  _d->lastTimeGrow = game::Date::current();
  _d->spreadFire = false;
}

void Tree::timeStep( const unsigned long time )
{
  Overlay::timeStep( time );
  if( _d->state == State::burning )
  {
    if( game::Date::isDayChanged() )
    {
      _d->health-=5;
      if( !_d->spreadFire && _d->health < 50 )
        _burnAround();
    }

    _animationRef().update( time );
    _fgPictures().back() = _animationRef().currentFrame();

    if( _d->health <= 0 )
      _die();
  }
  else if( game::Date::isMonthChanged() )
  {
    _d->health = math::clamp( _d->health+5, 0, 100 );
    bool mayHealth4grow = _d->health > 50;
    bool growLast2years = _d->lastTimeGrow.monthsTo( game::Date::current() ) < DateTime::monthsInYear * 2;
    if(mayHealth4grow && !growLast2years)
      _growAround();
  }
}

bool Tree::isFlat() const { return _d->flat; }

void Tree::initTerrain(Tile& terrain)
{
  terrain.setFlag( Tile::tlTree, true );
}

bool Tree::build( const city::AreaInfo& info )
{
  std::string picname = imgid::toResource( info.city->tilemap().at( info.pos ).originalImgId() );
  _picture().load( picname );
  _d->flat = (picture().height() <= tilemap::cellPicSize().height());
  return Overlay::build( info );
}

void Tree::save(VariantMap& stream) const
{
  Overlay::save( stream );

  VARIANT_SAVE_ANY_D( stream, _d, flat )
  VARIANT_SAVE_ANY_D( stream, _d, age )
  VARIANT_SAVE_ANY_D( stream, _d, health )
  VARIANT_SAVE_ENUM_D( stream, _d, state )
  VARIANT_SAVE_ANY_D( stream, _d, spreadFire )
}

void Tree::load(const VariantMap& stream)
{
  Overlay::load( stream );

  VARIANT_LOAD_ANY_D( _d, flat, stream )
  VARIANT_LOAD_ANY_D( _d, age, stream )
  VARIANT_LOAD_ANY_D( _d, health, stream )
  VARIANT_LOAD_ENUM_D( _d, state, stream )
  VARIANT_LOAD_ANY_D( _d, spreadFire, stream )

  if( _d->state == State::burning )
    _startBurning();
}

void Tree::destroy()
{
  TilesArray tiles = area();
  for( auto it : tiles )
    it->setFlag( Tile::tlTree, false );
}

void Tree::burn()
{
  if( _city()->getOption( PlayerCity::forestFire ) == 0 )
    return;

  if( _d->state != State::well )
    return;

  _startBurning();
}

void Tree::_startBurning()
{
  _d->state = State::burning;
  _animationRef() = AnimationBank::instance().simple( AnimationBank::animFire + 0 );
  _fgPictures().resize(1);
}

void Tree::_burnAround()
{
   _d->spreadFire = true;

  OverlayList ovs = _city()->tilemap().getNeighbors( pos() )
                                      .overlays();
  for( auto overlay : ovs )
  {
    if( math::probably( 0.5f ) )
      overlay->burn();
  }
}

void Tree::_growAround()
{
  TilesArray tiles = _city()->tilemap().getNeighbors( pos() );
  for( auto tile : tiles )
  {
    if( math::probably( 0.3f ) && tile->getFlag( Tile::isConstructible ) )
    {
      ;
    }
  }
}

void Tree::_die()
{
  _d->state = State::burnt;
  setPicture( "burnedTree", 1 );
  _d->flat = false;
  _animationRef().clear();
  _fgPictures().clear();
}
