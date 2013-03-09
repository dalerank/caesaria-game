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



#include <gui_paneling.hpp>
#include <SDL.h>
#include <exception.hpp>
#include <pic_loader.hpp>
#include <sdl_facade.hpp>
#include <gettext.hpp>

#include <iostream>


GuiPaneling* GuiPaneling::_instance = NULL;

GuiPaneling& GuiPaneling::instance()
{
   if (_instance == NULL)
   {
      _instance = new GuiPaneling();
      if (_instance == NULL) THROW("Memory error, cannot instantiate object");
   }
   return *_instance;
}


void GuiPaneling::draw_white_area(Picture &dstpic, const int x, const int y, const int width, const int height)
{
   SdlFacade &sdlFacade = SdlFacade::instance();

   for (int j = 0; j<(height/16+1); ++j)
   {
      for (int i = 0; i<(width/16+1); ++i)
      {
         // use some clipping to remove the right and bottom areas
         Picture &srcpic = PicLoader::instance().get_picture("paneling", 348+i%10 + 12*(j%10));
         SDL_Surface *srcimg = srcpic.get_surface();
         SDL_Surface *dstimg = dstpic.get_surface();
         int dx = 16*i;
         int dy = 16*j;
         int sw = std::min(16, width-dx);
         int sh = std::min(16, height-dy);
         sdlFacade.drawImage(srcimg, 0, 0, sw, sh, dstimg, x+dx, y+dy);
      }
   }
}


void GuiPaneling::draw_black_area(Picture &dstpic, const int x, const int y, const int width, const int height)
{
   SdlFacade &sdlFacade = SdlFacade::instance();

   for (int j = 0; j<(height/16+1); ++j)
   {
      for (int i = 0; i<(width/16+1); ++i)
      {
         // use some clipping to remove the right and bottom areas
         Picture &srcpic = PicLoader::instance().get_picture("paneling", 487+i%5 + 7*(j%5));
         SDL_Surface *srcimg = srcpic.get_surface();
         SDL_Surface *dstimg = dstpic.get_surface();
         int dx = 16*i;
         int dy = 16*j;
         int sw = std::min(16, width-dx);
         int sh = std::min(16, height-dy);
         sdlFacade.drawImage(srcimg, 0, 0, sw, sh, dstimg, x+dx, y+dy);
      }
   }
}


void GuiPaneling::draw_white_borders(Picture &dstpic, const int x, const int y, const int width, const int height)
{
   SdlFacade &sdlFacade = SdlFacade::instance();

   // draws horizontal borders
   for (int i = 0; i<(width/16-1); ++i)
   {
      // top border
      sdlFacade.drawPicture(PicLoader::instance().get_picture("paneling", 336+i%10), dstpic, x+16+16*i, y);
      // bottom border
      sdlFacade.drawPicture(PicLoader::instance().get_picture("paneling", 468+i%10), dstpic, x+16+16*i, y+height-16);
   }

   // draws vertical borders
   for (int i = 0; i<(height/16-1); ++i)
   {
      // left border
      sdlFacade.drawPicture(PicLoader::instance().get_picture("paneling", 347+12*(i%10)), dstpic, x, y+16+16*i);
      // right border
      sdlFacade.drawPicture(PicLoader::instance().get_picture("paneling", 358+12*(i%10)), dstpic, x+width-16, y+16+16*i);
   }

   // left-top corner
   sdlFacade.drawPicture(PicLoader::instance().get_picture("paneling", 335), dstpic, x, y);
   // left-bottom corner
   sdlFacade.drawPicture(PicLoader::instance().get_picture("paneling", 467), dstpic, x, y+height-16);
   // right-top corner
   sdlFacade.drawPicture(PicLoader::instance().get_picture("paneling", 346), dstpic, x+width-16, y);
   // right-bottom corner
   sdlFacade.drawPicture(PicLoader::instance().get_picture("paneling", 478), dstpic, x+width-16, y+height-16);
}


void GuiPaneling::draw_black_borders(Picture &dstpic, const int x, const int y, const int width, const int height)
{
   SdlFacade &sdlFacade = SdlFacade::instance();

   // draws horizontal borders
   for (int i = 0; i<(width/16-1); ++i)
   {
      // top border
      sdlFacade.drawPicture(PicLoader::instance().get_picture("paneling", 480+i%5), dstpic, x+16+16*i, y);
      // bottom border
      sdlFacade.drawPicture(PicLoader::instance().get_picture("paneling", 522+i%5), dstpic, x+16+16*i, y+height-16);
   }

   // draws vertical borders
   for (int i = 0; i<(height/16-1); ++i)
   {
      // left border
      sdlFacade.drawPicture(PicLoader::instance().get_picture("paneling", 486+7*(i%5)), dstpic, x, y+16+16*i);
      // right border
      sdlFacade.drawPicture(PicLoader::instance().get_picture("paneling", 492+7*(i%5)), dstpic, x+width-16, y+16+16*i);
   }

   // left-top corner
   sdlFacade.drawPicture(PicLoader::instance().get_picture("paneling", 479), dstpic, x, y);
   // left-bottom corner
   sdlFacade.drawPicture(PicLoader::instance().get_picture("paneling", 521), dstpic, x, y+height-16);
   // right-top corner
   sdlFacade.drawPicture(PicLoader::instance().get_picture("paneling", 485), dstpic, x+width-16, y);
   // right-bottom corner
   sdlFacade.drawPicture(PicLoader::instance().get_picture("paneling", 527), dstpic, x+width-16, y+height-16);
}


void GuiPaneling::draw_white_frame(Picture &dstpic, const int x, const int y, const int width, const int height)
{
   // draws the inside of the box
   draw_white_area(dstpic, x+16, y+16, width-32, height-32);

   // draws borders
   draw_white_borders(dstpic, x, y, width, height);
}


void GuiPaneling::draw_black_frame(Picture &dstpic, const int x, const int y, const int width, const int height)
{
   // draws the inside of the box
   draw_black_area(dstpic, x+16, y+16, width-32, height-32);

   // draws borders
   draw_black_borders(dstpic, x, y, width, height);
}


void GuiPaneling::draw_basic_frame(Picture &dstpic, const int x, const int y, const int width, const int height, const int offset)
{
   // pics are: 0TopLeft, 1Top, 2TopRight, 3Left, 4Center, 5Right, 6BottomLeft, 7Bottom, 8BottomRight

   SdlFacade &sdlFacade = SdlFacade::instance();

   // draws the inside of the box
   for (int j = 0; j<(height/16-1); ++j)
   {
      for (int i = 0; i<(width/16-1); ++i)
      {
         sdlFacade.drawPicture(PicLoader::instance().get_picture("paneling", offset+4), dstpic, x+16+16*i, y+16+16*j);
      }
   }

   // draws horizontal borders
   for (int i = 0; i<(width/16-1); ++i)
   {
      // top border
      sdlFacade.drawPicture(PicLoader::instance().get_picture("paneling", offset+1), dstpic, x+16+16*i, y);
      // bottom border
      sdlFacade.drawPicture(PicLoader::instance().get_picture("paneling", offset+7), dstpic, x+16+16*i, y+height-16);
   }

   // draws vertical borders
   for (int i = 0; i<(height/16-1); ++i)
   {
      // left border
      sdlFacade.drawPicture(PicLoader::instance().get_picture("paneling", offset+3), dstpic, x, y+16+16*i);
      // right border
      sdlFacade.drawPicture(PicLoader::instance().get_picture("paneling", offset+5), dstpic, x+width-16, y+16+16*i);
   }

   // topLeft corner
   sdlFacade.drawPicture(PicLoader::instance().get_picture("paneling", offset+0), dstpic, x, y);
   // topRight corner
   sdlFacade.drawPicture(PicLoader::instance().get_picture("paneling", offset+2), dstpic, x+width-16, y);
   // bottomLeft corner
   sdlFacade.drawPicture(PicLoader::instance().get_picture("paneling", offset+6), dstpic, x, y+height-16);
   // bottomRight corner
   sdlFacade.drawPicture(PicLoader::instance().get_picture("paneling", offset+8), dstpic, x+width-16, y+height-16);
}


void GuiPaneling::draw_brown_frame(Picture &dstpic, const int x, const int y, const int width, const int height)
{
   draw_basic_frame(dstpic, x, y, width, height, 28);
}

void GuiPaneling::draw_grey_frame(Picture &dstpic, const int x, const int y, const int width, const int height)
{
   draw_basic_frame(dstpic, x, y, width, height, 37);
}


void GuiPaneling::draw_basic0_borders(Picture &dstpic, const int x, const int y, const int width, const int height, const int offset)
{
   // pics are: 0TopLeft, 1Top, 2TopRight, 3Right, 4BottomRight, 5Bottom, 6BottomLeft, 7Left

   SdlFacade &sdlFacade = SdlFacade::instance();

   // draws horizontal borders
   for (int i = 0; i<(width/16-1); ++i)
   {
      // top border
      sdlFacade.drawPicture(PicLoader::instance().get_picture("paneling", offset+1), dstpic, x+16+16*i, y);
      // bottom border
      sdlFacade.drawPicture(PicLoader::instance().get_picture("paneling", offset+5), dstpic, x+16+16*i, y+height-16);
   }

   // draws vertical borders
   for (int i = 0; i<(height/16-1); ++i)
   {
      // left border
      sdlFacade.drawPicture(PicLoader::instance().get_picture("paneling", offset+7), dstpic, x, y+16+16*i);
      // right border
      sdlFacade.drawPicture(PicLoader::instance().get_picture("paneling", offset+3), dstpic, x+width-16, y+16+16*i);
   }

   // topLeft corner
   sdlFacade.drawPicture(PicLoader::instance().get_picture("paneling", offset+0), dstpic, x, y);
   // topRight corner
   sdlFacade.drawPicture(PicLoader::instance().get_picture("paneling", offset+2), dstpic, x+width-16, y);
   // bottomLeft corner
   sdlFacade.drawPicture(PicLoader::instance().get_picture("paneling", offset+6), dstpic, x, y+height-16);
   // bottomRight corner
   sdlFacade.drawPicture(PicLoader::instance().get_picture("paneling", offset+4), dstpic, x+width-16, y+height-16);
}

void GuiPaneling::draw_brown0_borders(Picture &dstpic, const int x, const int y, const int width, const int height)
{
   GuiPaneling::instance().draw_basic0_borders(dstpic, x, y, width, height, 555);
}

void GuiPaneling::draw_white0_borders(Picture &dstpic, const int x, const int y, const int width, const int height)
{
   GuiPaneling::instance().draw_basic0_borders(dstpic, x, y, width, height, 547);
}

void GuiPaneling::draw_basic_text_button(Picture &dstpic, const int x, const int y, const int width, const int offset)
{
   SdlFacade &sdlFacade = SdlFacade::instance();

   // draws the inside
   for (int i = 0; i<(width/16-1); ++i)
   {
      sdlFacade.drawPicture(PicLoader::instance().get_picture("paneling", offset+1), dstpic, x+16+16*i, y);
   }

   // left side
   sdlFacade.drawPicture(PicLoader::instance().get_picture("paneling", offset), dstpic, x, y);

   // right side
   sdlFacade.drawPicture(PicLoader::instance().get_picture("paneling", offset+2), dstpic, x+width-16, y);
}

