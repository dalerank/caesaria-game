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

#include "movecamera.hpp"
#include "sound/engine.hpp"
#include "game/game.hpp"
#include "scene/level.hpp"

namespace events
{

GameEventPtr MoveCamera::create(const TilePos& pos, bool force)
{
  MoveCamera* e = new MoveCamera();
  e->_pos = pos;
  e->_force = force;

  GameEventPtr ret( e );
  ret->drop();

  return ret;
}

void MoveCamera::_exec(Game& game, unsigned int)
{
  scene::Level* level = safety_cast<scene::Level*>( game.scene() );

  if( level )
  {
    level->setCameraPos( _pos, _force );
  }
}

bool MoveCamera::_mayExec(Game& , unsigned int ) const { return true; }

}
