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


#include <gfx_sdl_engine.hpp>

#include <cstdlib>
#include <string>
#include <sstream>
#include <iostream>

#include "exception.hpp"
#include "pic_loader.hpp"
#include "sdl_facade.hpp"


GfxSdlEngine::GfxSdlEngine() : GfxEngine()
{
   _screen = NULL;
}

GfxSdlEngine::~GfxSdlEngine()
{
}

void GfxSdlEngine::init()
{
   int rc;
   rc = SDL_Init(SDL_INIT_VIDEO);
   if (rc != 0) THROW("Unable to initialize SDL: " << SDL_GetError());
   rc = TTF_Init();
   if (rc != 0) THROW("Unable to initialize SDL: " << SDL_GetError());

   Uint32 aFlags = 0;
   aFlags |= SDL_DOUBLEBUF;
   aFlags |= SDL_SWSURFACE;

   _screen = SDL_SetVideoMode(_screen_width, _screen_height, 32, aFlags);  // 32bpp
   if (_screen == NULL) THROW("Unable to set video mode: " << SDL_GetError());
}


void GfxSdlEngine::exit()
{
   TTF_Quit();
   SDL_Quit();
}


void GfxSdlEngine::load_picture(Picture& ioPicture)
{
   // convert pixel format
   SDL_Surface *newImage;
   newImage = SDL_DisplayFormatAlpha(ioPicture._surface);
   SDL_FreeSurface(ioPicture._surface);
   if (newImage == NULL) THROW("Cannot convert surface, maybe out of memory");
   ioPicture._surface = newImage;
}

void GfxSdlEngine::unload_picture(Picture& ioPicture)
{
   SDL_FreeSurface(ioPicture._surface);
   ioPicture._surface = NULL;
}


void GfxSdlEngine::init_frame()
{
   SDL_FillRect(_screen, NULL, 0);  // black background for a complete redraw
}


void GfxSdlEngine::exit_frame()
{
   SDL_Flip(_screen); //Refresh the screen
}


void GfxSdlEngine::drawPicture(const Picture &picture, const int dx, const int dy)
{
   SdlFacade::instance().drawPicture(picture, _screen, dx, dy);
}


