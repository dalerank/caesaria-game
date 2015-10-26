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
// Copyright 2012-2014 dalerank, dalerankn8@gmail.com

#include "corpse.hpp"
#include "core/variant_map.hpp"
#include "city/city.hpp"
#include "objects/construction.hpp"
#include "pathway/pathway_helper.hpp"
#include "core/gettext.hpp"
#include "gfx/tilemap.hpp"
#include "constants.hpp"
#include "gfx/animation_bank.hpp"
#include "game/gamedate.hpp"
#include "walkers_factory.hpp"

using namespace gfx;

REGISTER_CLASS_IN_WALKERFACTORY(walker::corpse, Corpse)

class Corpse::Impl
{
public:
  unsigned int time;
  int updateInterval;
  int lastFrameIndex;
  bool loop;
};

WalkerPtr Corpse::create( PlayerCityPtr city, WalkerPtr wlk )
{
  AnimationBank::MovementAnimation ma = AnimationBank::find( wlk->type() );
  DirectedAction action( acDie, direction::north );
  Animation animation = ma[ action ];

  if( animation.isValid() )
  {
    Corpse* corpse = new Corpse( city );
    corpse->setPos( wlk->pos() );
    corpse->_animation() = animation;
    corpse->_animation().setDelay( 1 );
    corpse->_animation().setLoop( false );

    WalkerPtr ret( corpse );
    ret->drop();

    return ret;
  }

  return WalkerPtr();
}

WalkerPtr Corpse::create( PlayerCityPtr city, TilePos pos,
                          std::string rcGroup, int startIndex, int stopIndex,
                          bool loop )
{
  Corpse* corpse = new Corpse( city );
  corpse->setPos( pos );
  corpse->_d->time = 0;
  corpse->_d->loop = loop;

  if( !rcGroup.empty() )
  {
    corpse->_animation().load( rcGroup, startIndex, stopIndex - startIndex );
    corpse->_animation().setLoop( false );
    corpse->_animation().setDelay( Animation::fast );
    corpse->_d->lastFrameIndex = corpse->_animation().index();
  }
  else
  {
    corpse->deleteLater();
  }

  WalkerPtr ret( corpse );
  ret->drop();
  ret->attach();

  return ret;
}

Corpse::Corpse( PlayerCityPtr city ) : Walker( city ), _d( new Impl )
{
  _setType( walker::corpse );

  _d->time = 0;
  _d->updateInterval = game::Date::days2ticks( 1 );
  _d->loop = false;

  setName( _("##corpse##") );
  _setHealth( 0 );

  setFlag( vividly, false );
}

Corpse::~Corpse(){}

void Corpse::timeStep(const unsigned long time)
{
  _animation().update( time );
  if( !_animation().isLoop() && _d->time >= _animation().delay() )
  {
    _d->time = 0;
    _animation().setDelay( _animation().delay() * 2 );
  }

  _d->time++;

  if( ( time % _d->updateInterval == 1 ) && health() <= 0 )
  {
    updateHealth( -1 );

    if( health() <= -100 )
    {
      deleteLater();
    }
  }
}

void Corpse::save( VariantMap& stream ) const
{
  Walker::save( stream );

  stream[ "animation" ] = _animation().save();
  VARIANT_SAVE_ANY_D( stream, _d, time )
  VARIANT_SAVE_ANY_D( stream, _d, loop )
}

void Corpse::load( const VariantMap& stream )
{
  Walker::load( stream );

  VARIANT_LOAD_ANY_D( _d, time, stream )
  VARIANT_LOAD_ANY_D( _d, loop, stream )

  _animation().load( stream.get( "animation" ).toMap() );
  if( _animation().delay() == 0 )
  {
    _animation().setDelay( Animation::middle );
    _animation().setLoop( false );
  }
}

const Picture& Corpse::getMainPicture()
{
  return _animation().currentFrame();
}
