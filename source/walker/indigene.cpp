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

#include "indigene.hpp"
#include "core/gettext.hpp"
#include "city/helper.hpp"
#include "objects/native.hpp"
#include "walkers_factory.hpp"

using namespace constants;
using namespace gfx;

REGISTER_CLASS_IN_WALKERFACTORY(walker::indigene, Indigene)

class Indigene::Impl
{
public:
  typedef enum { findField, randomWay, go2center, back2base, count } Action;
  TilePos startPos;
  Action action;
  unsigned int wheatQty;
  unsigned int tryCount;

public:
  Pathway findWay2bestField( PlayerCityPtr city, TilePos pos );
};

IndigenePtr Indigene::create(PlayerCityPtr city)
{
  IndigenePtr ret( new Indigene( city ) );
  ret->drop();

  return ret;
}

void Indigene::send2city(BuildingPtr base)
{
  TilesArray tiles = base->enterArea();

  if( !tiles.empty() )
  {
    setPos( tiles.random()->pos() );
    _d->startPos = pos();
    _d->action = Impl::findField;
    _updateState();
  }
  else
  {
    deleteLater();
  }

  if( !isDeleted() )
  {
    _city()->addWalker( this );
  }
}

void Indigene::_reachedPathway()
{
  Walker::_reachedPathway();

  _updateState();
}

void Indigene::_updateState()
{
  switch( _d->action )
  {
  case Impl::findField:
  {
    _d->tryCount++;

    TilePos offset( 1, 1 );
    city::Helper helper( _city() );
    NativeFieldList fields = helper.find<NativeField>( objects::native_field, pos() - offset, pos() + offset );
    foreach( i, fields )
    {
      _d->wheatQty += (*i)->catchCrops();
      if( _d->wheatQty >= 100 )
        break;
    }

    if( _d->wheatQty > 100 )
    {
      _d->action = Impl::go2center;
      _updateState();
    }
    else
    {
      Pathway way = _d->findWay2bestField( _city(), pos() );
      if( way.isValid() )
      {
        setPathway( way );
        go();
      }
      else
      {
        _d->action = Impl::randomWay;
        _updateState();
      }
    }
  }
  break;

  case Impl::go2center:
  {
    TilePos offset( 1, 1 );
    city::Helper helper( _city() );
    NativeCenterList centerList = helper.find<NativeCenter>( objects::native_center, pos() - offset, pos() + offset );
    if( !centerList.empty() )
    {
      centerList.front()->store( _d->wheatQty );
    }

    _d->action = Impl::back2base;
    _updateState();
  }
  break;

  case Impl::randomWay:
  {
    Pathway way;
    if( _d->tryCount > 3 )
    {
      _d->action = Impl::back2base;
      _updateState();
    }
    else
    {
      way = PathwayHelper::randomWay( _city(), pos(), 6 );

      if( way.isValid() )
      {
        _d->action = Impl::findField;
        setPathway( way );
        go();
      }
      else
      {
        deleteLater();
      }
    }
  }
  break;

  case Impl::back2base:
  {
    city::Helper helper( _city() );

    TilePos offset( 1, 1 );
    BuildingList huts = helper.find<Building>( objects::native_hut, pos() - offset, pos() + offset );

    Pathway way;
    if( huts.empty() )
    {
      way = PathwayHelper::create( pos(), _d->startPos, PathwayHelper::allTerrain );
    }

    if( way.isValid() )
    {
      setPathway( way );
      go();
    }
    else
    {
      deleteLater();
    }
  }
  break;

  default: break;
  }
}

Indigene::Indigene(PlayerCityPtr city)
  : Human( city ), _d( new Impl )
{
  _setType( walker::indigene );
  _d->wheatQty = 0;
  _d->tryCount = 0;

  setName( _("##indigene##") );
}


Pathway Indigene::Impl::findWay2bestField(PlayerCityPtr city, TilePos pos)
{
  city::Helper helper( city );
  TilePos offset( 5, 5 );
  NativeFieldList fields = helper.find<NativeField>( objects::native_field, pos - offset, pos + offset );

  Pathway way;
  if( !fields.empty() )
  {
    NativeFieldPtr field = fields.front();
    foreach( i, fields )
    {
      if( (*i)->progress() > field->progress() )
      {
        field = *i;
      }
    }

    way = PathwayHelper::create( pos, ptr_cast<Construction>( field ), PathwayHelper::allTerrain );
  }

  return way;
}
