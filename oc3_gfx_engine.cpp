// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com


#include "oc3_gfx_engine.hpp"

#include "oc3_exception.hpp"


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

Size GfxEngine::getScreenSize() const
{
  return _srcSize;
}

void GfxEngine::setFlag( int flag, int value )
{
}
