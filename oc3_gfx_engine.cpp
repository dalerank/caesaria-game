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
  _screen_width = 0;
  _screen_height = 0;
  _instance = this;
}

GfxEngine::~GfxEngine()
{
  _instance = NULL;
}


void GfxEngine::setScreenSize(const int width, const int height)
{
  _screen_width = width;
  _screen_height = height;
}

int GfxEngine::getScreenWidth() const
{
  return _screen_width;
}

int GfxEngine::getScreenHeight() const
{
  return _screen_height;
}

void GfxEngine::load_pictures(const std::list<Picture*> &ioPictures)
{
  for (std::list<Picture*>::const_iterator it = ioPictures.begin(); it!= ioPictures.end(); ++it)
  {
    // for every picture
    Picture& pic = *(*it);
    load_picture(pic);
  }
}

Size GfxEngine::getScreenSize() const
{
  return Size( _screen_width, _screen_height );
}

