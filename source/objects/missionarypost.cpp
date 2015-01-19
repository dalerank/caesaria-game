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

#include "missionarypost.hpp"
#include "game/resourcegroup.hpp"
#include "walker/walker.hpp"
#include "constants.hpp"
#include "objects_factory.hpp"

using namespace constants;

REGISTER_CLASS_IN_OVERLAYFACTORY( objects::missionaryPost, MissionaryPost)

MissionaryPost::MissionaryPost()
  : ServiceBuilding( Service::missionary, objects::missionaryPost, Size(2) )
{
  setMaximumWorkers(20);
  setWorkers( 0 );
  setPicture( ResourceGroup::transport, 93 );

  setState( inflammability, 0 );
  setState( collapsibility, 0 );
}

void MissionaryPost::deliverService()
{
  if( mayWork() )
  {
    ServiceBuilding::deliverService();

    WalkerList ws = walkers();
    if( !ws.empty() )
    {
      if( ws.front()->type() == walker::missioner )
      {
        ws.front()->setThinks( "##missioner_high_barbarian_risk##" );
      }
    }
  }
}

