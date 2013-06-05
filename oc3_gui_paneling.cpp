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

#include <memory>

#include "oc3_gui_paneling.hpp"
#include "oc3_exception.hpp"
#include "oc3_pic_loader.hpp"
#include "oc3_gettext.hpp"
#include "oc3_pushbutton.hpp"
#include "oc3_resourcegroup.hpp"

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

void GuiPaneling::configureTexturedButton( PushButton* oButton, const std::string& rcGroup, const int pic_index, bool pushButton )
{
    PicLoader& loader = PicLoader::instance();
    oButton->setPicture( &loader.getPicture( rcGroup, pic_index), stNormal );
    oButton->setPicture( &loader.getPicture( rcGroup, pic_index+1), stHovered );
    oButton->setPicture( &loader.getPicture( rcGroup, pic_index+2), stPressed );
    oButton->setPicture( &loader.getPicture( rcGroup, pic_index+3), stDisabled );
    oButton->setIsPushButton( pushButton );
}


void GuiPaneling::draw_white_area(Picture &dstpic, const int x, const int y, const int width, const int height)
{
   for (int j = 0; j<(height/16+1); ++j)
   {
      for (int i = 0; i<(width/16+1); ++i)
      {
         // use some clipping to remove the right and bottom areas
        Picture &srcpic = Picture::load( ResourceGroup::panelBackground, 348+i%10 + 12*(j%10));

        int dx = 16*i;
        int dy = 16*j;
        int sw = std::min(16, width-dx);
        int sh = std::min(16, height-dy);

        dstpic.draw( srcpic, Rect( 0, 0, sw, sh), Point( x+dx, y+dy ) );
      }
   }
}


void GuiPaneling::draw_black_area(Picture &dstpic, const int x, const int y, const int width, const int height)
{
   for (int j = 0; j<(height/16+1); ++j)
   {
      for (int i = 0; i<(width/16+1); ++i)
      {
         // use some clipping to remove the right and bottom areas
        Picture &srcpic = Picture::load( ResourceGroup::panelBackground, 487+i%5 + 7*(j%5));

        int dx = 16*i;
        int dy = 16*j;
        int sw = std::min(16, width-dx);
        int sh = std::min(16, height-dy);
        
        dstpic.draw( srcpic, Rect( 0, 0, sw, sh ), Point( x+dx, y+dy ) );
      }
   }
}


void GuiPaneling::draw_white_borders(Picture &dstpic, const int x, const int y, const int width, const int height)
{
   // draws horizontal borders
   for (int i = 0; i<(width/16-1); ++i)
   {
     // top border
     dstpic.draw( Picture::load( ResourceGroup::panelBackground, 336+i%10), x+16+16*i, y);
     // bottom border
     dstpic.draw( Picture::load( ResourceGroup::panelBackground, 468+i%10), x+16+16*i, y+height-16);
   }

   // draws vertical borders
   for (int i = 0; i<(height/16-1); ++i)
   {
     // left border
     dstpic.draw( Picture::load( ResourceGroup::panelBackground, 347+12*(i%10)), x, y+16+16*i);
     // right border
     dstpic.draw( Picture::load( ResourceGroup::panelBackground, 358+12*(i%10)), x+width-16, y+16+16*i);
   }

   // left-top corner
   dstpic.draw( Picture::load( ResourceGroup::panelBackground, 335), x, y);
   // left-bottom corner
   dstpic.draw( Picture::load( ResourceGroup::panelBackground, 467), x, y+height-16);
   // right-top corner
   dstpic.draw( Picture::load( ResourceGroup::panelBackground, 346), x+width-16, y);
   // right-bottom corner
   dstpic.draw( Picture::load( ResourceGroup::panelBackground, 478), x+width-16, y+height-16);
}


void GuiPaneling::draw_black_borders(Picture &dstpic, const int x, const int y, const int width, const int height)
{
   // draws horizontal borders
   for (int i = 0; i<(width/16-1); ++i)
   {
      // top border
      dstpic.draw(Picture::load( ResourceGroup::panelBackground, 480+i%5), x+16+16*i, y);
      // bottom border
      dstpic.draw(Picture::load( ResourceGroup::panelBackground, 522+i%5), x+16+16*i, y+height-16);
   }

   // draws vertical borders
   for (int i = 0; i<(height/16-1); ++i)
   {
      // left border
      dstpic.draw(Picture::load( ResourceGroup::panelBackground, 486+7*(i%5)), x, y+16+16*i);
      // right border
      dstpic.draw(Picture::load( ResourceGroup::panelBackground, 492+7*(i%5)), x+width-16, y+16+16*i);
   }

   // left-top corner
   dstpic.draw(Picture::load( ResourceGroup::panelBackground, 479), x, y);
   // left-bottom corner
   dstpic.draw(Picture::load( ResourceGroup::panelBackground, 521), x, y+height-16);
   // right-top corner
   dstpic.draw(Picture::load( ResourceGroup::panelBackground, 485), x+width-16, y);
   // right-bottom corner
   dstpic.draw(Picture::load( ResourceGroup::panelBackground, 527), x+width-16, y+height-16);
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

   // draws the inside of the box
   for (int j = 0; j<(height/16-1); ++j)
   {
      for (int i = 0; i<(width/16-1); ++i)
      {
         dstpic.draw(Picture::load( ResourceGroup::panelBackground, offset+4), x+16+16*i, y+16+16*j);
      }
   }

   // draws horizontal borders
   for (int i = 0; i<(width/16-1); ++i)
   {
      // top border
      dstpic.draw(Picture::load( ResourceGroup::panelBackground, offset+1), x+16+16*i, y);
      // bottom border
      dstpic.draw(Picture::load( ResourceGroup::panelBackground, offset+7), x+16+16*i, y+height-16);
   }

   // draws vertical borders
   for (int i = 0; i<(height/16-1); ++i)
   {
      // left border
      dstpic.draw(Picture::load( ResourceGroup::panelBackground, offset+3), x, y+16+16*i);
      // right border
      dstpic.draw(Picture::load( ResourceGroup::panelBackground, offset+5), x+width-16, y+16+16*i);
   }

   // topLeft corner
   dstpic.draw(Picture::load( ResourceGroup::panelBackground, offset+0), x, y);
   // topRight corner
   dstpic.draw(Picture::load( ResourceGroup::panelBackground, offset+2), x+width-16, y);
   // bottomLeft corner
   dstpic.draw(Picture::load( ResourceGroup::panelBackground, offset+6), x, y+height-16);
   // bottomRight corner
   dstpic.draw(Picture::load( ResourceGroup::panelBackground, offset+8), x+width-16, y+height-16);
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
   // draws horizontal borders
   for (int i = 0; i<(width/16-1); ++i)
   {
      // top border
      dstpic.draw(Picture::load( ResourceGroup::panelBackground, offset+1),x+16+16*i, y);
      // bottom border
      dstpic.draw(Picture::load( ResourceGroup::panelBackground, offset+5),x+16+16*i, y+height-16);
   }

   // draws vertical borders
   for (int i = 0; i<(height/16-1); ++i)
   {
      // left border
      dstpic.draw(Picture::load( ResourceGroup::panelBackground, offset+7), x, y+16+16*i);
      // right border
      dstpic.draw(Picture::load( ResourceGroup::panelBackground, offset+3), x+width-16, y+16+16*i);
   }

   // topLeft corner
   dstpic.draw(Picture::load( ResourceGroup::panelBackground, offset+0), x, y);
   // topRight corner
   dstpic.draw(Picture::load( ResourceGroup::panelBackground, offset+2), x+width-16, y);
   // bottomLeft corner
   dstpic.draw(Picture::load( ResourceGroup::panelBackground, offset+6), x, y+height-16);
   // bottomRight corner
   dstpic.draw(Picture::load( ResourceGroup::panelBackground, offset+4), x+width-16, y+height-16);
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
   // draws the inside
   for (int i = 0; i<(width/16-1); ++i)
   {
     dstpic.draw(Picture::load( ResourceGroup::panelBackground, offset+1), x+16+16*i, y);
   }

   // left side
   dstpic.draw(Picture::load( ResourceGroup::panelBackground, offset), x, y);

   // right side
   dstpic.draw(Picture::load( ResourceGroup::panelBackground, offset+2), x+width-16, y);
}

