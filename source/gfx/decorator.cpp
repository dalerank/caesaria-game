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

#include <memory>

#include "decorator.hpp"
#include "core/exception.hpp"
#include "core/gettext.hpp"
#include "core/rectangle.hpp"
#include "game/resourcegroup.hpp"

PictureDecorator& PictureDecorator::instance()
{
   static PictureDecorator inst;
   return inst;
}

void PictureDecorator::drawFrame(Picture &dstpic, const Rect& rectangle, const int picId, bool useAlpha)
{
   // pics are: 0TopLeft, 1Top, 2TopRight, 3Left, 4Center, 5Right, 6BottomLeft, 7Bottom, 8BottomRight

   // draws the inside of the box
  const Picture& bg = Picture::load( ResourceGroup::panelBackground, picId+4);
  const int sw = bg.getWidth();
  const int sh = bg.getHeight();
  for (int j = 0; j<(rectangle.getHeight()/sh-1); ++j)
  {
     for (int i = 0; i<(rectangle.getWidth()/sw-1); ++i)
     {
        dstpic.draw( bg, rectangle.UpperLeftCorner + Point( sw+sw*i, sh+sh*j ), useAlpha );
     }
  }

  // draws horizontal borders
  const Picture& topBorder = Picture::load( ResourceGroup::panelBackground, picId+1);
  const Picture& bottomBorder = Picture::load( ResourceGroup::panelBackground, picId+7);
  for (int i = 0; i<(rectangle.getWidth()/sw-1); ++i)
  {
     dstpic.draw( topBorder, rectangle.UpperLeftCorner + Point( sw+sw*i, 0 ), useAlpha);
     dstpic.draw( bottomBorder, rectangle.UpperLeftCorner + Point( sw+sw*i, rectangle.getHeight()-sh ), useAlpha );
  }

  // draws vertical borders
  const Picture& leftBorder = Picture::load( ResourceGroup::panelBackground, picId+3);
  const Picture& rightBorder = Picture::load( ResourceGroup::panelBackground, picId+5);
  for (int i = 0; i<(rectangle.getHeight()/sh-1); ++i)
  {
     dstpic.draw( leftBorder, rectangle.UpperLeftCorner + Point( 0, sh+sh*i ), useAlpha );
     dstpic.draw( rightBorder, rectangle.UpperLeftCorner + Point( rectangle.getWidth()-sw, sh+sh*i ), useAlpha );
  }

  // topLeft corner
  dstpic.draw(Picture::load( ResourceGroup::panelBackground, picId+0), rectangle.UpperLeftCorner, useAlpha );
  // topRight corner
  dstpic.draw(Picture::load( ResourceGroup::panelBackground, picId+2), Point( rectangle.right()-sh, rectangle.top() ), useAlpha );
  // bottomLeft corner
  dstpic.draw(Picture::load( ResourceGroup::panelBackground, picId+6), Point( rectangle.left(), rectangle.bottom() - sh ), useAlpha );
  // bottomRight corner
  dstpic.draw(Picture::load( ResourceGroup::panelBackground, picId+8), rectangle.LowerRightCorner - Point( 16, 16 ), useAlpha );
}

void PictureDecorator::drawBorder(Picture &dstpic, const Rect& rectangle, const int offset, bool useAlpha)
{
  // pics are: 0TopLeft, 1Top, 2TopRight, 3Right, 4BottomRight, 5Bottom, 6BottomLeft, 7Left
  // draws horizontal borders
  const Picture& topborder = Picture::load( ResourceGroup::panelBackground, offset+1);
  const int sw = topborder.getWidth();
  const int sh = topborder.getHeight();
  const Picture& bottomBorder = Picture::load( ResourceGroup::panelBackground, offset+5);
  for (int i = 0; i<(rectangle.getWidth()/sw-1); ++i)
  {
     dstpic.draw( topborder, rectangle.UpperLeftCorner + Point( sw+sw*i, 0 ), useAlpha);
     dstpic.draw( bottomBorder, rectangle.UpperLeftCorner + Point( sw+sw*i, rectangle.getHeight()-sh ), useAlpha );
  }

  // draws vertical borders
  const Picture& leftborder = Picture::load( ResourceGroup::panelBackground, offset+7);
  const Picture& rightborder = Picture::load( ResourceGroup::panelBackground, offset+3);
  for (int i = 0; i<(rectangle.getHeight()/sh-1); ++i)
  {
     dstpic.draw( leftborder, rectangle.UpperLeftCorner + Point( 0, sh+sh*i ), useAlpha );
     dstpic.draw( rightborder, rectangle.UpperLeftCorner + Point( rectangle.getWidth()-sw, sh+sh*i ), useAlpha );
  }

  // topLeft corner
  dstpic.draw( Picture::load( ResourceGroup::panelBackground, offset+0), rectangle.UpperLeftCorner, useAlpha);
  // topRight corner
  dstpic.draw(Picture::load( ResourceGroup::panelBackground, offset+2), rectangle.right()-sw, rectangle.top(), useAlpha );
  // bottomLeft corner
  dstpic.draw(Picture::load( ResourceGroup::panelBackground, offset+6), rectangle.left(), rectangle.bottom()-sh, useAlpha);
  // bottomRight corner
  dstpic.draw(Picture::load( ResourceGroup::panelBackground, offset+4), rectangle.right()-16, rectangle.bottom()-sh, useAlpha);
}

void PictureDecorator::drawPanel( Picture &dstpic, const Rect& rectangle, int picId, bool useAlpha )
{
  // left side
  dstpic.draw( Picture::load( ResourceGroup::panelBackground, picId), rectangle.UpperLeftCorner );

  // draws the inside
  const Picture& centerPic = Picture::load( ResourceGroup::panelBackground, picId+1);
  for (int i = 0; i<(rectangle.getWidth()/16-1); ++i)
  {
    dstpic.draw( centerPic, rectangle.UpperLeftCorner + Point( 16+16*i, 0 ), useAlpha );
  }

  // right side
  dstpic.draw( Picture::load( ResourceGroup::panelBackground, picId+2), 
               rectangle.UpperLeftCorner + Point( rectangle.getWidth()-16, 0) );
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
  case greyPanel: drawPanel( dstpic, rectangle, 25, useAlpha ); break;
  case lightgreyPanel: drawPanel( dstpic, rectangle, 22, useAlpha ); break;
  case smallBrownPanel: drawPanel( dstpic, rectangle, 65, useAlpha ); break;
  case smallGreyPanel: drawPanel( dstpic, rectangle, 68, useAlpha ); break;
  case whiteBorder: drawBorder( dstpic, rectangle, 336, 468, 347, 358, 10, 12, 335, 467, 346, 478, useAlpha );  break;
  case blackBorder: drawBorder( dstpic, rectangle, 480, 522, 486, 492, 5, 7, 479, 521, 485, 527, useAlpha ); break;
  case brownBorder: drawBorder(dstpic, rectangle, 555, useAlpha ); break;
  case whiteBorderA: drawBorder( dstpic, rectangle, 547, useAlpha ); break;
  case whiteFrame:
    draw( dstpic, Rect( rectangle.UpperLeftCorner + Point( 16, 16 ), rectangle.LowerRightCorner - Point( 16, 16 ) ), whiteArea );    // draws the inside of the box
    draw( dstpic, rectangle, whiteBorder );    // draws borders
  break;

  case blackFrame:
    draw(dstpic, Rect( rectangle.UpperLeftCorner + Point( 16, 16 ), rectangle.LowerRightCorner - Point( 16, 16 ) ), blackArea );    // draws the inside of the box
    draw(dstpic, rectangle, blackBorder );    // draws borders
  break;

  case brownFrame: drawFrame(dstpic, rectangle, 28, useAlpha); break;
  case greyFrame: drawFrame(dstpic, rectangle, 37, useAlpha); break;
  }
}

void PictureDecorator::drawBorder( Picture &dstpic, const Rect& rectangle, 
                                   int tp, int bp, int lp, int rp, 
                                   int pCount, int hCount,
                                   int ltc, int lbc, int rtc, int rbc, bool useAlpha )
{
  // draws horizontal borders
  Size size = Picture::load( ResourceGroup::panelBackground, tp ).getSize();
  const int sw = size.width();
  const int sh = size.height();
  for (int i = 0; i<(rectangle.getWidth()/size.width()-1); ++i)
  {
    Point offset = rectangle.UpperLeftCorner + Point( sw+sw*i, 0 );
    dstpic.draw( Picture::load( ResourceGroup::panelBackground, tp+i%pCount), offset, useAlpha );      // top border
    dstpic.draw( Picture::load( ResourceGroup::panelBackground, bp+i%pCount), offset + Point( 0, rectangle.getHeight()-sh ), useAlpha );      // bottom border
  }

  // draws vertical borders
  for (int i = 0; i<(rectangle.getHeight()/size.height()-1); ++i)
  {
    Point offset = rectangle.UpperLeftCorner + Point( 0, sh+sh*i );
    dstpic.draw( Picture::load( ResourceGroup::panelBackground, lp+hCount*(i%pCount)), offset, useAlpha );      // left border
    dstpic.draw( Picture::load( ResourceGroup::panelBackground, rp+hCount*(i%pCount)), offset + Point( rectangle.getWidth()-sw, 0 ), useAlpha );      // right border
  }

  dstpic.draw( Picture::load( ResourceGroup::panelBackground, ltc), rectangle.UpperLeftCorner );    // left-top corner
  dstpic.draw( Picture::load( ResourceGroup::panelBackground, lbc), Point( rectangle.left(), rectangle.bottom()-sh ), useAlpha );    // left-bottom corner
  dstpic.draw( Picture::load( ResourceGroup::panelBackground, rtc ), Point( rectangle.right() - sw, rectangle.top() ), useAlpha );     // right-top corner
  dstpic.draw( Picture::load( ResourceGroup::panelBackground, rbc), rectangle.LowerRightCorner - Point( sw, sh ), useAlpha );    // right-bottom corner
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
