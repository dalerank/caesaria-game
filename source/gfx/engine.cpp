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


#include "engine.hpp"

#include "core/exception.hpp"

namespace gfx
{

Engine* Engine::_instance = NULL;

Engine& Engine::instance()
{
   if (_instance == NULL)
   {
      THROW("Error, no graphics engine instance");
   }
   return *_instance;
}


Engine::Engine()
{
  _srcSize = Size( 0 );
  _instance = this;
}

Engine::~Engine() {  _instance = NULL; }

void Engine::setScreenSize( Size size ) { _srcSize = size; }
Size Engine::virtualSize() const { return _virtualSize; }
bool Engine::isFullscreen() const{ return getFlag( fullscreen ) > 0; }
void Engine::setFullscreen(bool enabled){ setFlag( fullscreen, enabled ? 1 : 0 ); }
Size Engine::screenSize() const{ return _srcSize; }
void Engine::setFlag( int flag, int value ) { _flags[ flag ] = value;}

int Engine::getFlag(int flag) const
{
  std::map< int, int >::const_iterator it = _flags.find( flag );
  return it != _flags.end() ? it->second : 0;
}

}//end namespace gfx
