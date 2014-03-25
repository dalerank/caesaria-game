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
  _registerGfxType( citizenGo );
  _registerGfxType( bathladyGo );
  _registerGfxType( priest );
  _registerGfxType( actor );
  _registerGfxType( tamer );
  _registerGfxType( taxCollector );
  _registerGfxType( scholar );
  _registerGfxType( marketlady );
  _registerGfxType( cartPusher );
  _registerGfxType( cartPusher2 );
  _registerGfxType( engineer );
  _registerGfxType( gladiator );
  _registerGfxType( gladiator2 );
  _registerGfxType( protestor );
  _registerGfxType( barber );
  _registerGfxType( prefect );
  _registerGfxType( prefectDragWater );
  _registerGfxType( prefectFightFire );
  _registerGfxType( prefectFight );
  _registerGfxType( homeless );
  _registerGfxType( patrician );
  _registerGfxType( patrician2 );
  _registerGfxType( doctor );
  _registerGfxType( teacher );
  _registerGfxType( javelineer );
  _registerGfxType( soldier );
  _registerGfxType( horseman );
  _registerGfxType( horseMerchant );
  _registerGfxType( camelMerchant );
  _registerGfxType( marketkid );
  _registerGfxType( sheep );
  _registerGfxType( fishingBoat );
  _registerGfxType( fishingBoatWork );
  _registerGfxType( homelessSit );
  _registerGfxType( lion );
  _registerGfxType( britonSoldier );
  _registerGfxType( britonSoldierFight );
  _registerGfxType( legionaryMove );
  _registerGfxType( legionaryFight );
  _registerGfxType( charioterMove );
  _registerGfxType( guardMove );
  _registerGfxType( guardFigth );
  _registerGfxType( seaMerchantMove );
  _registerGfxType( etruscanArcherMove );
  _registerGfxType( etruscanArcherFight );
}


