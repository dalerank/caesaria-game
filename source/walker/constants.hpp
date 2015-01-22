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

#ifndef __CAESARIA_WALKER_CONSTANTS_H_INCLUDED__
#define __CAESARIA_WALKER_CONSTANTS_H_INCLUDED__

namespace constants
{

namespace walker
{

enum Type
{
  unknown=0,
  immigrant,
  emigrant,
  soldier,
  cartPusher,
  marketLady,
  marketKid,
  serviceman,                   //unknown service
  trainee,
  recruter,
  prefect,//10
  taxCollector,
  engineer,
  doctor,
  bathlady,
  merchant,
  actor,
  gladiator,
  lionTamer,
  charioteer,
  sheep,//20
  barber,
  surgeon,
  fishingBoat,
  priest,
  protestor,
  patrician,
  corpse,
  enemySoldier,
  britonSoldier,
  legionary,//30
  patrolPoint,
  romeGuard,
  lion,
  spear,  //34
  fishPlace,
  seaMerchant,
  scholar,
  librarian,
  teacher,
  locust, //40
  marketBuyer,
  bow_arrow, //42
  etruscanSoldier,
  etruscanArcher,
  citizen,
  waterGarbage,
  romeHorseman,
  romeSpearman,
  balista,
  criminal,//50
  mugger,
  rioter,
  wolf,
  dustCloud,
  romeChastenerSoldier,
  romeChastenerElephant,
  indigeneRioter,
  indigene,
  missioner,
  zebra,
  riverWave,
  supplier,
  circusCharioter,
  docker,
  gladiatorRiot,
  merchantCamel,
  all = 0xff,
  any = all
};

} //end namespace walker

} //end namespace constants

#endif //__CAESARIA_WALKER_CONSTANTS_H_INCLUDED__
