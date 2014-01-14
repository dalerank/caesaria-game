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


#include "engine.hpp"

#include "core/exception.hpp"


GfxEngine* GfxEngine::_instance = NULL;

GfxEngine& GfxEngine::instance()
{
   if (_instance == NULL)
   {
      THROW("Error, no graphics engine instance");
   }
   return *_instance;
}


GfxEngine::GfxEngine()
{
  _srcSize = Size( 0 );
  _instance = this;
}

GfxEngine::~GfxEngine()
{
  _instance = NULL;
}


void GfxEngine::setScreenSize( Size size )
{
  _srcSize = size;
}

int GfxEngine::getScreenWidth() const
{
  return _srcSize.getWidth();
}

int GfxEngine::getScreenHeight() const
{
  return _srcSize.getHeight();
}

bool GfxEngine::isFullscreen() const
{
  return getFlag( fullscreen ) > 0;
}

void GfxEngine::setFullscreen(bool enabled)
{
  setFlag( fullscreen, enabled ? 1 : 0 );
}

Size GfxEngine::getScreenSize() const
{
  return _srcSize;
}

void GfxEngine::setFlag( int flag, int value )
{
  _flags[ flag ] = value;
}

int GfxEngine::getFlag(int flag) const
{
  std::map< int, int >::const_iterator it = _flags.find( flag );
  return it != _flags.end() ? it->second : 0;
}
