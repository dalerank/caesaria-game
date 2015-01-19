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
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

#include "dock_pusher.hpp"
#include "city/city.hpp"
#include "walkers_factory.hpp"

using namespace constants;

REGISTER_CLASS_IN_WALKERFACTORY(walker::docker, Docker)

Docker::Docker( PlayerCityPtr city )
  : CartPusher( city )
{
  _setType( walker::docker );
  stock().setCapacity( bigCart );
}

DockerPtr Docker::create(PlayerCityPtr city )
{
  DockerPtr ret( new Docker( city ) );
  ret->drop(); //delete automatically

  return ret;
}
