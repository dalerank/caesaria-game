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


#include "oc3_gfx_gl_engine.hpp"

#include <cstdlib>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <SDL.h>
#include <SDL_image.h>

#include "oc3_exception.hpp"
#include "oc3_pic_loader.hpp"
#include "oc3_picture.hpp"
#include "oc3_sdl_facade.hpp"
#include "oc3_positioni.hpp"


GfxGlEngine::GfxGlEngine() : GfxEngine()
{
}

GfxGlEngine::~GfxGlEngine()
{ }


void GfxGlEngine::init()
{
   int rc;
   rc = SDL_Init(SDL_INIT_VIDEO);
   if (rc != 0) THROW("Unable to initialize SDL: " << SDL_GetError());
   rc = TTF_Init();
   if (rc != 0) THROW("Unable to initialize SDL: " << SDL_GetError());

   SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 0);

   _screen = SDL_SetVideoMode(_screen_width, _screen_height, 32, SDL_OPENGL | SDL_FULLSCREEN);
   if (_screen == NULL) THROW("Unable to set video mode: " << SDL_GetError());

   glEnable( GL_TEXTURE_2D );
   glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
   glViewport( 0, 0, _screen_width, _screen_height);
   glClear(GL_COLOR_BUFFER_BIT);  // black screen
   glDisable(GL_DEPTH_TEST);      // no depth test
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(0, _screen_width, _screen_height, 0, 0, 1);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   // Displacement trick for exact pixelization
   glTranslatef(0.375, 0.375, 0);

   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glEnable(GL_BLEND);
}


void GfxGlEngine::exit()
{
   TTF_Quit();
   SDL_Quit();
}


void GfxGlEngine::unload_picture(Picture &ioPicture)
{
   glDeleteTextures(1, &ioPicture._glTextureID);
   SDL_FreeSurface(ioPicture._surface);
   ioPicture._glTextureID = 0;
   ioPicture._surface = NULL;
}

void GfxGlEngine::load_picture(Picture& ioPicture)
{
   GLuint &texture = ioPicture._glTextureID;
   SDL_Surface *surface = ioPicture._surface;
   GLenum texture_format;
   GLint  nOfColors;

   // SDL_Surface *surface2
   // get the number of channels in the SDL surface
   nOfColors = surface->format->BytesPerPixel;
   if (nOfColors == 4)     // contains an alpha channel
   {
      if (surface->format->Rmask == 0x000000ff)
         texture_format = GL_RGBA;
      else
         texture_format = GL_BGRA;
   }
   else if (nOfColors == 3)     // no alpha channel
   {
      if (surface->format->Rmask == 0x000000ff)
         texture_format = GL_RGB;
      else
         texture_format = GL_BGR;
   }
   else
   {
      THROW("Invalid image format");
   }

   if (texture == 0)
   {
      // the picture has no texture ID!
      // generate a texture ID
      glGenTextures( 1, &texture );
   }

   // Bind the texture object
   glBindTexture( GL_TEXTURE_2D, texture );

   // Set the texture's stretching properties
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

   // Edit the texture object's image data using the information SDL_Surface gives us
   ioPicture.lock();
   glTexImage2D( GL_TEXTURE_2D, 0, nOfColors, surface->w, surface->h, 0,
                 texture_format, GL_UNSIGNED_BYTE, surface->pixels );
   ioPicture.unlock();
}


void GfxGlEngine::init_frame()
{
   glClear(GL_COLOR_BUFFER_BIT);  // black screen
}


void GfxGlEngine::exit_frame()
{
   SDL_GL_SwapBuffers(); //Refresh the screen
}


void GfxGlEngine::drawPicture(const Picture &picture, const int dx, const int dy)
{
   GLuint aTextureID = picture._glTextureID;
   int x0 = dx+picture.get_xoffset();
   int x1 = x0+picture.get_width();
   int y0 = dy-picture.get_yoffset();
   int y1 = y0+picture.get_height();


   // Bind the texture to which subsequent calls refer to
   glBindTexture( GL_TEXTURE_2D, aTextureID);

   glBegin( GL_QUADS );

   //Bottom-left vertex (corner)
   glTexCoord2i( 0, 0 );
   glVertex2f( x0, y0 );

   //Bottom-right vertex (corner)
   glTexCoord2i( 1, 0 );
   glVertex2f( x1, y0 );

   //Top-right vertex (corner)
   glTexCoord2i( 1, 1 );
   glVertex2f( x1, y1 );

   //Top-left vertex (corner)
   glTexCoord2i( 0, 1 );
   glVertex2f( x0, y1 );

   glEnd();
}

void GfxGlEngine::drawPicture( const Picture &picture, const Point& pos )
{
  drawPicture( picture, pos.getX(), pos.getY() );
}

void GfxGlEngine::setTileDrawMask( int rmask, int gmask, int bmask, int amask )
{

}

void GfxGlEngine::resetTileDrawMask()
{

}