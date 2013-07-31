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

#include "oc3_picture_decorator.hpp"
#include "oc3_exception.hpp"
#include "oc3_gettext.hpp"
#include "oc3_pushbutton.hpp"
#include "oc3_resourcegroup.hpp"

PictureDecorator& PictureDecorator::instance()
{
   static PictureDecorator inst;
   return inst;
}


void PictureDecorator::draw_white_borders(Picture &dstpic, const int x, const int y, const int width, const int height)
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


void PictureDecorator::draw_black_borders(Picture &dstpic, const int x, const int y, const int width, const int height)
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


void PictureDecorator::draw_white_frame(Picture &dstpic, const int x, const int y, const int width, const int height)
{
   // draws the inside of the box
   draw( dstpic, Rect( Point( x+16, y+16 ), Size( width-16, height-16 ) ), whiteArea );

   // draws borders
   draw_white_borders(dstpic, x, y, width, height);
}


void PictureDecorator::draw_black_frame(Picture &dstpic, const int x, const int y, const int width, const int height)
{
   // draws the inside of the box
   draw(dstpic, Rect( Point( x+16, y+16 ), Size( width-16, height-16 ) ), blackArea );

   // draws borders
   draw_black_borders(dstpic, x, y, width, height);
}


void PictureDecorator::draw_basic_frame(Picture &dstpic, const int x, const int y, const int width, const int height, const int offset)
{
   // pics are: 0TopLeft, 1Top, 2TopRight, 3Left, 4Center, 5Right, 6BottomLeft, 7Bottom, 8BottomRight

   // draws the inside of the box
  const Picture& bg = Picture::load( ResourceGroup::panelBackground, offset+4);
  for (int j = 0; j<(height/16-1); ++j)
  {
     for (int i = 0; i<(width/16-1); ++i)
     {
        dstpic.draw( bg, x+16+16*i, y+16+16*j);
     }
  }

  // draws horizontal borders
  const Picture& topBorder = Picture::load( ResourceGroup::panelBackground, offset+1);
  const Picture& bottomBorder = Picture::load( ResourceGroup::panelBackground, offset+7);
  for (int i = 0; i<(width/16-1); ++i)
  {
     dstpic.draw( topBorder, x+16+16*i, y);
     dstpic.draw( bottomBorder, x+16+16*i, y+height-16);
  }

  // draws vertical borders
  const Picture& leftBorder = Picture::load( ResourceGroup::panelBackground, offset+3);
  const Picture& rightBorder = Picture::load( ResourceGroup::panelBackground, offset+5);
  for (int i = 0; i<(height/16-1); ++i)
  {
     dstpic.draw( leftBorder, x, y+16+16*i);
     dstpic.draw( rightBorder, x+width-16, y+16+16*i);
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


void PictureDecorator::draw_brown_frame(Picture &dstpic, const int x, const int y, const int width, const int height)
{
   draw_basic_frame(dstpic, x, y, width, height, 28);
}

void PictureDecorator::draw_grey_frame(Picture &dstpic, const int x, const int y, const int width, const int height)
{
   draw_basic_frame(dstpic, x, y, width, height, 37);
}


void PictureDecorator::draw_basic0_borders(Picture &dstpic, const int x, const int y, const int width, const int height, const int offset)
{
  // pics are: 0TopLeft, 1Top, 2TopRight, 3Right, 4BottomRight, 5Bottom, 6BottomLeft, 7Left
  // draws horizontal borders
  const Picture& topborder = Picture::load( ResourceGroup::panelBackground, offset+1);
  const Picture& bottomBorder = Picture::load( ResourceGroup::panelBackground, offset+5);
  for (int i = 0; i<(width/16-1); ++i)
  {
     dstpic.draw( topborder, x+16+16*i, y);
     dstpic.draw( bottomBorder, x+16+16*i, y+height-16);
  }

  // draws vertical borders
  const Picture& leftborder = Picture::load( ResourceGroup::panelBackground, offset+7);
  const Picture& rightborder = Picture::load( ResourceGroup::panelBackground, offset+3);
  for (int i = 0; i<(height/16-1); ++i)
  {
     dstpic.draw( leftborder, x, y+16+16*i);
     dstpic.draw( rightborder, x+width-16, y+16+16*i);
  }

  // topLeft corner
  Picture& pic = Picture::load( ResourceGroup::panelBackground, offset+0 );
  dstpic.draw( pic, x, y);
  // topRight corner
  dstpic.draw(Picture::load( ResourceGroup::panelBackground, offset+2), x+width-16, y);
  // bottomLeft corner
  dstpic.draw(Picture::load( ResourceGroup::panelBackground, offset+6), x, y+height-16);
  // bottomRight corner
  dstpic.draw(Picture::load( ResourceGroup::panelBackground, offset+4), x+width-16, y+height-16);
}

void PictureDecorator::draw_brown0_borders(Picture &dstpic, const int x, const int y, const int width, const int height)
{
   PictureDecorator::instance().draw_basic0_borders(dstpic, x, y, width, height, 555);
}

void PictureDecorator::draw_white0_borders(Picture &dstpic, const int x, const int y, const int width, const int height)
{
   PictureDecorator::instance().draw_basic0_borders(dstpic, x, y, width, height, 547);
}

void PictureDecorator::draw_basic_text_button(Picture &dstpic, const int x, const int y, const int width, const int offset)
{
  // draws the inside
  const Picture& centerPic = Picture::load( ResourceGroup::panelBackground, offset+1);
  for (int i = 0; i<(width/16-1); ++i)
  {
    dstpic.draw( centerPic, x+16+16*i, y);
  }

  // left side
  dstpic.draw(Picture::load( ResourceGroup::panelBackground, offset), x, y);

  // right side
  dstpic.draw(Picture::load( ResourceGroup::panelBackground, offset+2), x+width-16, y);
}

PictureDecorator::PictureDecorator()
{
}

void PictureDecorator::draw( Picture& dstpic, const Rect& rectangle, Mode mode, bool useAlpha )
{
  switch( mode )
  {
  case whiteArea: drawArea( dstpic, rectangle, 348, 10, 12, useAlpha ); break;
  case blackArea: drawArea( dstpic, rectangle, 487, 5, 7, useAlpha ); break;
  }
}

void PictureDecorator::drawArea(Picture &dstpic, const Rect& rectangle, int picId, int picCount, int offset, bool useAlpha)
{
  for (int j = 0; j<(rectangle.getHeight()/16+1); ++j)
  {
    for (int i = 0; i<(rectangle.getWidth()/16+1); ++i)
    {
      // use some clipping to remove the right and bottom areas
      const Picture &srcpic = Picture::load( ResourceGroup::panelBackground, picId + (i%picCount) + offset*(j%picCount) );

      int dx = 16*i;
      int dy = 16*j;
      int sw = std::min(16, rectangle.getWidth()-dx);
      int sh = std::min(16, rectangle.getHeight()-dy);

      dstpic.draw( srcpic, Rect( 0, 0, sw, sh), rectangle.UpperLeftCorner + Point( dx, dy ) );
    }
  }
}