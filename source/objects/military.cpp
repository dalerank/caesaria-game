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

#include "military.hpp"
#include "constants.hpp"
#include "game/resourcegroup.hpp"
#include "city/helper.hpp"
#include "gfx/tilemap.hpp"
#include "walker/romesoldier.hpp"
#include "core/logger.hpp"
#include "events/event.hpp"
#include "walker/patrolpoint.hpp"
#include "barracks.hpp"
#include "game/gamedate.hpp"
#include "game/settings.hpp"
#include "core/saveadapter.hpp"
#include "walker/romehorseman.hpp"
#include "walker/helper.hpp"
#include "walker/romearcher.hpp"
#include "objects_factory.hpp"

using namespace constants;
using namespace gfx;

REGISTER_CLASS_IN_OVERLAYFACTORY(objects::fort_javelin, FortJaveline)
REGISTER_CLASS_IN_OVERLAYFACTORY(objects::fort_horse, FortMounted)
REGISTER_CLASS_IN_OVERLAYFACTORY(objects::fort_legionaries, FortLegionary)

FortLegionary::FortLegionary()
  : Fort( objects::fort_legionaries, 16 )
{
  setPicture( ResourceGroup::security, 12 );
  _setFlagIndex( 21 );

  _addFormation( frmNorthDblLine );
  _addFormation( frmWestDblLine );
  _addFormation( frmSquad );
  _addFormation( frmOpen );
}

void FortLegionary::_readyNewSoldier()
{
  TilesArray tiles = enterArea();

  if( !tiles.empty() )
  {
    RomeSoldierPtr soldier = RomeSoldier::create( _city(), walker::legionary );
    soldier->send2city( this, tiles.front()->pos() );
    addWalker( soldier.object() );
  }
}

FortMounted::FortMounted()
  : Fort( constants::objects::fort_horse, 15 )
{
  setPicture( ResourceGroup::security, 12 );
  _setFlagIndex( 39 );

  _addFormation( frmNorthLine );
  _addFormation( frmWestLine );
  _addFormation( frmNorthDblLine );
  _addFormation( frmWestDblLine );
  _addFormation( frmOpen );
}

bool FortMounted::build( const CityAreaInfo& info )
{
  return Fort::build( info );
}

void FortMounted::_readyNewSoldier()
{
  TilesArray tiles = enterArea();

  if( !tiles.empty() )
  {
    RomeHorsemanPtr soldier = RomeHorseman::create( _city() );
    soldier->send2city( this, tiles.front()->pos() );
    addWalker( soldier.object() );
  }
}

FortJaveline::FortJaveline()
  : Fort( objects::fort_javelin, 14 )
{
  setPicture( ResourceGroup::security, 12 );
  _setFlagIndex( 30 );

  _addFormation( frmNorthDblLine );
  _addFormation( frmWestDblLine );
  _addFormation( frmOpen );
}

void FortJaveline::_readyNewSoldier()
{
  TilesArray tiles = enterArea();

  if( !tiles.empty() )
  {
    RomeArcherPtr soldier = RomeArcher::create( _city() );
    soldier->send2city( this, tiles.front()->pos() );
    addWalker( soldier.object() );
  }
}
