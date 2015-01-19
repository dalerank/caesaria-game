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

#include "engineer_post.hpp"
#include "game/resourcegroup.hpp"
#include "core/position.hpp"
#include "constants.hpp"
#include "objects_factory.hpp"

using namespace gfx;
using namespace constants;

REGISTER_CLASS_IN_OVERLAYFACTORY( objects::engineering_post, EngineerPost)

EngineerPost::EngineerPost() : ServiceBuilding( Service::engineer, constants::objects::engineering_post, Size(1) )
{
  setPicture( MetaDataHolder::randomPicture( type(), size() ) );
  //setPicture( ResourceGroup::buildingEngineer, 56 );

  _animationRef().load( ResourceGroup::buildingEngineer, 57, 10 );
  _animationRef().setDelay( 4 );
  _animationRef().setOffset( Point( 10, 42 ) );
  _fgPicturesRef().resize(1);
}

void EngineerPost::timeStep(const unsigned long time)
{
  ServiceBuilding::timeStep( time );
}

void EngineerPost::deliverService()
{
  if( numberWorkers() > 0 && walkers().size() == 0 )
  {
    ServiceBuilding::deliverService();
  }
}

unsigned int EngineerPost::walkerDistance() const
{
  return 26;
}
