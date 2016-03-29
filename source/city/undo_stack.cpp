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

#include "undo_stack.hpp"
#include "city/city.hpp"
#include "core/logger.hpp"
#include "game/gamedate.hpp"
#include "events/clearland.hpp"
#include "events/fundissue.hpp"
#include "core/timer.hpp"
#include "city/economy.hpp"
#include "objects/metadata.hpp"
#include "objects/metadata.hpp"
#include "events/build.hpp"

using namespace events;

namespace undo
{

PREDEFINE_CLASS_SMARTLIST(UndoAction,List)

const int UndoTimerId = Hash(TEXT(UStack));

class UndoAction : public ReferenceCounted
{
public:
  virtual ~UndoAction() {}
  virtual void undo( PlayerCityPtr city ) {}
  object::Type type;
  TilePos location;
  int money;
};

class UndoBuild : public UndoAction
{
public:

  UndoBuild( object::Type what, const TilePos& where, int dn )
  {
    type = what;
    location = where;
    money = dn;
  }

  virtual void undo( PlayerCityPtr city )
  {
    events::dispatch<ClearTile>( location );
    events::dispatch<Payment>( econ::Issue::buildConstruction, money );
  }
};

class UndoDestroy : public UndoAction
{
public:
  UndoDestroy( object::Type what, const TilePos& where, int dn )
  {
    type = what;
    location = where;
    money = dn;
  }

  virtual void undo( PlayerCityPtr city )
  {
    const object::Info& info = object::Info::find( type );
    TilePos undoOffset = info.getOption( "undoOffset" );
    events::dispatch<BuildAny>( location + undoOffset, type );
    events::dispatch<Payment>( econ::Issue::buildConstruction, money );
  }
};

class UStack::Impl
{
public:
  UndoActionList actions;
  PlayerCityPtr city;
  bool  needClear;

  Signal1<bool> onUndoChangeSignal;

  void checkFinished();
  void updateTimer();
  void clear();
};

UStack::UStack() : _d( new Impl ) {}
UStack::~UStack() {}
Signal1<bool>& UStack::onUndoChange() { return _d->onUndoChangeSignal; }

void UStack::init(PlayerCityPtr city)
{
  _d->city = city;
  _d->needClear = true;
}

void UStack::build(object::Type what, TilePos where, int money)
{
  _d->checkFinished();

  _d->actions.push_back( new UndoBuild( what, where, money ) );
  _d->actions.back()->drop();

  emit _d->onUndoChangeSignal( isAvailableUndo() );
}

void UStack::destroy(object::Type what, TilePos where, int money )
{
  _d->checkFinished();

  _d->actions.push_back( new UndoDestroy( what, where, money ) );
  _d->actions.back()->drop();

  emit _d->onUndoChangeSignal( isAvailableUndo() );
}

void UStack::finished()
{
  _d->needClear = true;
}

void UStack::clear() { _d->clear(); }

void UStack::undo()
{
  for( auto action : _d->actions )
    action->undo( _d->city );

  _d->actions.clear();
  emit _d->onUndoChangeSignal( isAvailableUndo() );
}

bool UStack::isAvailableUndo()
{
  return !_d->actions.empty();
}

void UStack::Impl::checkFinished()
{
  if( needClear )
    actions.clear();

  needClear = false;
}

void UStack::Impl::updateTimer()
{
  Timer::destroy( UndoTimerId );
  auto timer = Timer::create( game::Date::days2ticks( 15 ), Timer::singleShot, UndoTimerId );
  CONNECT( timer, onTimeout(), this, Impl::clear );
}

void UStack::Impl::clear()
{
  actions.clear();
  emit onUndoChangeSignal( false );
}

}//end namespace undo
