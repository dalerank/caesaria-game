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

#include "typehelper.hpp"

using namespace constants;

GfxTypeHelper&GfxTypeHelper::instance()
{
  static GfxTypeHelper inst;
  return inst;
}

GfxTypeHelper::GfxTypeHelper() : EnumsHelper<gfx::Type>( gfx::unknown )
{
#define _registerGfxType(a) append(gfx::a, #a )
  _registerGfxType( citizenMove );
  _registerGfxType( bathladyMove );
  _registerGfxType( priestMove );
  _registerGfxType( actorMove );
  _registerGfxType( tamerMove );
  _registerGfxType( taxCollectorMove );
  _registerGfxType( scholarMove );
  _registerGfxType( marketladyMove );
  _registerGfxType( cartPusherMove );
  _registerGfxType( cartPusher2Move );
  _registerGfxType( engineerMove );
  _registerGfxType( gladiatorMove );
  _registerGfxType( gladiator2Move );
  _registerGfxType( protestorMove );
  _registerGfxType( barberMove );
  _registerGfxType( prefectMove );
  _registerGfxType( prefectDragWater );
  _registerGfxType( prefectFightFire );
  _registerGfxType( prefectFight );
  _registerGfxType( homelessMove );
  _registerGfxType( patricianMove );
  _registerGfxType( patrician2Move );
  _registerGfxType( doctorMove );
  _registerGfxType( teacherMove );
  _registerGfxType( javelineerMove );
  _registerGfxType( soldierMove );
  _registerGfxType( horsemanMove );
  _registerGfxType( horseMerchantMove );
  _registerGfxType( camelMerchantMove );
  _registerGfxType( marketkidMove );
  _registerGfxType( sheepMove );
  _registerGfxType( fishingBoatMove );
  _registerGfxType( fishingBoatWork );
  _registerGfxType( homelessSit );
  _registerGfxType( lionMove );
  _registerGfxType( britonSoldierMove );
  _registerGfxType( britonSoldierFight );
  _registerGfxType( legionaryMove );
  _registerGfxType( legionaryFight );
  _registerGfxType( charioterMove );
  _registerGfxType( guardMove );
  _registerGfxType( guardFigth );
  _registerGfxType( seaMerchantMove );
  _registerGfxType( etruscanSoldierMove );
  _registerGfxType( etruscanSoldierFight );
  _registerGfxType( etruscanArcherMove );
  _registerGfxType( etruscanArcherFight );
}


