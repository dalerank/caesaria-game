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

#include "tile_trigger.hpp"
#include "core/variant_map.hpp"
#include "city/city.hpp"
#include "objects/construction.hpp"
#include "pathway/pathway_helper.hpp"
#include "core/gettext.hpp"
#include "gfx/tilemap.hpp"
#include "walker/constants.hpp"
#include "gfx/animation_bank.hpp"
#include "game/gamedate.hpp"
#include "walker/walkers_factory.hpp"

using namespace gfx;

REGISTER_CLASS_IN_WALKERFACTORY(walker::trigger, TileTrigger)

class TileTrigger::Impl
{
public:
  VariantMap exec;

  int delay;
  int lifeTime;
  int coolTime;
  int range;
  bool loop;
  bool once;
};

WalkerPtr TileTrigger::create(PlayerCityPtr city)
{
  TileTrigger* trigger = new TileTrigger( city );

  WalkerPtr ret( trigger );
  ret->drop();

  return ret;
}

TileTrigger::TileTrigger( PlayerCityPtr city ) : Walker( city ), _d( new Impl )
{
  _setType( walker::trigger );

  setName( _("##trigger##") );
  _setHealth( 0 );

  _d->delay = -1;
  _d->range = 0;
  _d->once = true;
  _d->coolTime = 0;
  _d->lifeTime = -1;

  setFlag( vividly, false );
}

bool TileTrigger::_checkExeCondition() { return false; }

void TileTrigger::_updateAnimation(unsigned int time)
{
  _animation().update( time );
}

void TileTrigger::_exec()
{
  if( _d->once )
    deleteLater();

  _d->delay += _d->coolTime;
}

TileTrigger::~TileTrigger(){}

void TileTrigger::timeStep(const unsigned long time)
{
  if( _d->delay > 0 )
  {
    _d->delay--;
  }
  else
  {
    if( _checkExeCondition() )
      _exec();
  }

  _updateAnimation( time );

  if( _d->lifeTime > 0 )
  {
    _d->lifeTime--;
  }

  if( _d->lifeTime == 0 )
  {
    deleteLater();
  }
}

void TileTrigger::save( VariantMap& stream ) const
{
  Walker::save( stream );

  stream[ "animation" ] = _animation().save();
  VARIANT_SAVE_ANY_D( stream, _d, exec )
  VARIANT_SAVE_ANY_D( stream, _d, loop )
  VARIANT_SAVE_ANY_D( stream, _d, range )
  VARIANT_SAVE_ANY_D( stream, _d, coolTime )
  VARIANT_SAVE_ANY_D( stream, _d, once )
  VARIANT_SAVE_ANY_D( stream, _d, lifeTime )
}

void TileTrigger::load( const VariantMap& stream )
{
  Walker::load( stream );

  VARIANT_LOAD_ANY_D( _d, loop, stream )
  VARIANT_LOAD_VMAP_D( _d, exec, stream )
  VARIANT_LOAD_ANY_D( _d, range, stream )
  VARIANT_LOAD_ANY_D( _d, coolTime, stream )
  VARIANT_LOAD_ANY_D( _d, once, stream )
  VARIANT_LOAD_ANY_D( _d, lifeTime, stream )

  _animation().load( stream.get( "animation" ).toMap() );
  if( _animation().delay() == 0 )
  {
    _animation().setDelay( Animation::middle );
    _animation().setLoop( false );
  }
}

const Picture& TileTrigger::getMainPicture()
{
  return _animation().currentFrame();
}
