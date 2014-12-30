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
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include <memory>

#include "decorator.hpp"
#include "core/exception.hpp"
#include "core/gettext.hpp"
#include "core/rectangle.hpp"
#include "game/resourcegroup.hpp"
#include "primitives.hpp"
#include "core/color.hpp"
#include "core/logger.hpp"
#include "primitives.hpp"

namespace gfx
{

/*void Decorator::drawFrame(Picture &dstpic, const Rect& rectangle, const int picId, bool useAlpha )
{
   // pics are: 0TopLeft, 1Top, 2TopRight, 3Left, 4Center, 5Right, 6BottomLeft, 7Bottom, 8BottomRight

   // draws the inside of the box
  const Picture& bg = Picture::load( ResourceGroup::panelBackground, picId+4);
  const int sw = bg.width();
  const int sh = bg.height();
  for (int j = 0; j<(rectangle.height()/sh-1); ++j)
  {
     for (int i = 0; i<(rectangle.width()/sw-1); ++i)
     {
        dstpic.draw( bg, rectangle.UpperLeftCorner + Point( sw+sw*i, sh+sh*j ), useAlpha );
     }
  }

  // draws horizontal borders
  const Picture& topBorder = Picture::load( ResourceGroup::panelBackground, picId+1);
  const Picture& bottomBorder = Picture::load( ResourceGroup::panelBackground, picId+7);
  for (int i = 0; i<(rectangle.width()/sw-1); ++i)
  {
     dstpic.draw( topBorder, rectangle.UpperLeftCorner + Point( sw+sw*i, 0 ), useAlpha);
     dstpic.draw( bottomBorder, rectangle.UpperLeftCorner + Point( sw+sw*i, rectangle.height()-sh ), useAlpha );
  }

  // draws vertical borders
  const Picture& leftBorder = Picture::load( ResourceGroup::panelBackground, picId+3);
  const Picture& rightBorder = Picture::load( ResourceGroup::panelBackground, picId+5);
  for (int i = 0; i<(rectangle.height()/sh-1); ++i)
  {
     dstpic.draw( leftBorder, rectangle.UpperLeftCorner + Point( 0, sh+sh*i ), useAlpha );
     dstpic.draw( rightBorder, rectangle.UpperLeftCorner + Point( rectangle.width()-sw, sh+sh*i ), useAlpha );
  }

  // topLeft corner
  dstpic.draw(Picture::load( ResourceGroup::panelBackground, picId+0), rectangle.UpperLeftCorner, useAlpha );
  // topRight corner
  dstpic.draw(Picture::load( ResourceGroup::panelBackground, picId+2), Point( rectangle.right()-sh, rectangle.top() ), useAlpha );
  // bottomLeft corner
  dstpic.draw(Picture::load( ResourceGroup::panelBackground, picId+6), Point( rectangle.left(), rectangle.bottom() - sh ), useAlpha );
  // bottomRight corner
  dstpic.draw(Picture::load( ResourceGroup::panelBackground, picId+8), rectangle.LowerRightCorner - Point( 16, 16 ), useAlpha );
}*/

void Decorator::drawFrame(Pictures& stack, const Rect& rectangle, const int picId )
{
   // pics are: 0TopLeft, 1Top, 2TopRight, 3Left, 4Center, 5Right, 6BottomLeft, 7Bottom, 8BottomRight

   // draws the inside of the box
  const Picture& bg = Picture::load( ResourceGroup::panelBackground, picId+4);
  const int sw = bg.width();
  const int sh = bg.height();
  for (int j = 0; j<(rectangle.height()/sh-1); ++j)
  {
     for (int i = 0; i<(rectangle.width()/sw-1); ++i)
     {
        stack.append( bg, rectangle.UpperLeftCorner + Point( sw+sw*i, sh+sh*j ) );
     }
  }

  // draws horizontal borders
  const Picture& topBorder = Picture::load( ResourceGroup::panelBackground, picId+1);
  const Picture& bottomBorder = Picture::load( ResourceGroup::panelBackground, picId+7);
  for (int i = 0; i<(rectangle.width()/sw-1); ++i)
  {
     stack.append( topBorder, rectangle.UpperLeftCorner + Point( sw+sw*i, 0 ));
     stack.append( bottomBorder, rectangle.UpperLeftCorner + Point( sw+sw*i, rectangle.height()-sh ) );
  }

  // draws vertical borders
  const Picture& leftBorder = Picture::load( ResourceGroup::panelBackground, picId+3);
  const Picture& rightBorder = Picture::load( ResourceGroup::panelBackground, picId+5);
  for (int i = 0; i<(rectangle.height()/sh-1); ++i)
  {
     stack.append( leftBorder, rectangle.UpperLeftCorner + Point( 0, sh+sh*i ) );
     stack.append( rightBorder, rectangle.UpperLeftCorner + Point( rectangle.width()-sw, sh+sh*i ) );
  }

  // topLeft corner
  stack.append(Picture::load( ResourceGroup::panelBackground, picId+0), rectangle.UpperLeftCorner );
  // topRight corner
  stack.append(Picture::load( ResourceGroup::panelBackground, picId+2), Point( rectangle.right()-sh, rectangle.top() ) );
  // bottomLeft corner
  stack.append(Picture::load( ResourceGroup::panelBackground, picId+6), Point( rectangle.left(), rectangle.bottom() - sh ) );
  // bottomRight corner
  stack.append(Picture::load( ResourceGroup::panelBackground, picId+8), rectangle.LowerRightCorner - Point( 16, 16 ) );
}


/*void Decorator::drawBorder(Picture &dstpic, const Rect& rectangle, const int offset, bool useAlpha)
{
  // pics are: 0TopLeft, 1Top, 2TopRight, 3Right, 4BottomRight, 5Bottom, 6BottomLeft, 7Left
  // draws horizontal borders
  const Picture& topborder = Picture::load( ResourceGroup::panelBackground, offset+1);
  const int sw = topborder.width();
  const int sh = topborder.height();
  const Picture& bottomBorder = Picture::load( ResourceGroup::panelBackground, offset+5);
  for (int i = 0; i<(rectangle.width()/sw-1); ++i)
  {
     dstpic.draw( topborder, rectangle.UpperLeftCorner + Point( sw+sw*i, 0 ), useAlpha);
     dstpic.draw( bottomBorder, rectangle.UpperLeftCorner + Point( sw+sw*i, rectangle.height()-sh ), useAlpha );
  }

  // draws vertical borders
  const Picture& leftborder = Picture::load( ResourceGroup::panelBackground, offset+7);
  const Picture& rightborder = Picture::load( ResourceGroup::panelBackground, offset+3);
  for (int i = 0; i<(rectangle.height()/sh-1); ++i)
  {
     dstpic.draw( leftborder, rectangle.UpperLeftCorner + Point( 0, sh+sh*i ), useAlpha );
     dstpic.draw( rightborder, rectangle.UpperLeftCorner + Point( rectangle.width()-sw, sh+sh*i ), useAlpha );
  }

  // topLeft corner
  dstpic.draw( Picture::load( ResourceGroup::panelBackground, offset+0), rectangle.UpperLeftCorner, useAlpha);
  // topRight corner
  dstpic.draw(Picture::load( ResourceGroup::panelBackground, offset+2), rectangle.right()-sw, rectangle.top(), useAlpha );
  // bottomLeft corner
  dstpic.draw(Picture::load( ResourceGroup::panelBackground, offset+6), rectangle.left(), rectangle.bottom()-sh, useAlpha);
  // bottomRight corner
  dstpic.draw(Picture::load( ResourceGroup::panelBackground, offset+4), rectangle.right()-16, rectangle.bottom()-sh, useAlpha);
}*/

void Decorator::drawBorder(Pictures& stack, const Rect& rectangle, const int offset)
{
  // pics are: 0TopLeft, 1Top, 2TopRight, 3Right, 4BottomRight, 5Bottom, 6BottomLeft, 7Left  
  const Picture& topborder = Picture::load( ResourceGroup::panelBackground, offset+1);
  const int sw = topborder.width();
  const int sh = topborder.height();
  const Picture& bottomBorder = Picture::load( ResourceGroup::panelBackground, offset+5);

  // draws horizontal borders
  for (int i = 0; i<(rectangle.width()/sw-1); ++i)
  {
     stack.append( topborder, rectangle.UpperLeftCorner + Point( sw+sw*i, 0 ) );
     stack.append( bottomBorder, rectangle.UpperLeftCorner + Point( sw+sw*i, -rectangle.height()+sh ) );
  }

  // draws vertical borders
  const Picture& leftborder = Picture::load( ResourceGroup::panelBackground, offset+7);
  const Picture& rightborder = Picture::load( ResourceGroup::panelBackground, offset+3);
  for (int i = 0; i<(rectangle.height()/sh+1); ++i)
  {
     stack.append( leftborder, rectangle.lefttop() + Point( 0, -rectangle.height()+sh*i ) );
     stack.append( rightborder, rectangle.lefttop() + Point( rectangle.width()-sw, -rectangle.height()+sh*i ) );
  }

  // topLeft corner
  stack.append( Picture::load( ResourceGroup::panelBackground, offset+0), rectangle.UpperLeftCorner);
  // topRight corner
  stack.append( Picture::load( ResourceGroup::panelBackground, offset+2), Point( rectangle.right()-sw, rectangle.top() ) );
  // bottomLeft corner
  stack.append( Picture::load( ResourceGroup::panelBackground, offset+6), Point( rectangle.left(), -rectangle.bottom()+sh ) );
  // bottomRight corner
  stack.append( Picture::load( ResourceGroup::panelBackground, offset+4), Point( rectangle.right()-16, -rectangle.bottom()+sh ) );
}

void Decorator::drawLine( Picture& dstpic, const Point& p1, const Point& p2, NColor color)
{
  if( dstpic.isValid() )
    lineColor( dstpic.surface(), p1.x(), p1.y(), p2.x(), p2.y(), color.rgba() );
}

void Decorator::basicText(Picture& dstpic, const Point& pos, const std::string& text, NColor color)
{
  stringColor( dstpic.surface(), pos.x(), pos.y(), text.c_str(), color.color );
  dstpic.update();
}

void Decorator::drawPanel( Pictures& stack, const Rect& rectangle, int picId )
{
  // left side
  stack.append( Picture::load( ResourceGroup::panelBackground, picId), rectangle.UpperLeftCorner );

  // draws the inside
  const Picture& centerPic = Picture::load( ResourceGroup::panelBackground, picId+1);
  for (int i = 0; i<(rectangle.width()/16-1); ++i)
  {
    stack.append( centerPic, rectangle.UpperLeftCorner + Point( 16+16*i, 0 ) );
  }

  // right side
  stack.append( Picture::load( ResourceGroup::panelBackground, picId+2),
             rectangle.UpperLeftCorner + Point( rectangle.width()-16, 0) );
}

/*void Decorator::drawPanel( Picture &dstpic, const Rect& rectangle, int picId, bool useAlpha )
{
  // left side
  dstpic.draw( Picture::load( ResourceGroup::panelBackground, picId), rectangle.UpperLeftCorner );

  // draws the inside
  const Picture& centerPic = Picture::load( ResourceGroup::panelBackground, picId+1);
  for (int i = 0; i<(rectangle.width()/16-1); ++i)
  {
    dstpic.draw( centerPic, rectangle.UpperLeftCorner + Point( 16+16*i, 0 ), useAlpha );
  }

  // right side
  dstpic.draw( Picture::load( ResourceGroup::panelBackground, picId+2), 
               rectangle.UpperLeftCorner + Point( rectangle.width()-16, 0) );
}*/

void Decorator::draw( Picture& dstpic, const Rect& rectangle, Mode mode, bool useAlpha, bool updateTexture )
{
  if( updateTexture )
    dstpic.lock();

  switch( mode )
  {
  /*case whiteArea: drawArea( dstpic, rectangle, 348, 10, 12, useAlpha ); break;
  case blackArea: drawArea( dstpic, rectangle, 487, 5, 7, useAlpha ); break;
  case greyPanel: drawPanel( dstpic, rectangle, 25, useAlpha ); break;
  case lightgreyPanel: drawPanel( dstpic, rectangle, 22, useAlpha ); break;
  case greyPanelBig: drawPanel( dstpic, rectangle, 631, useAlpha ); break;
  case lightgreyPanelBig: drawPanel( dstpic, rectangle, 634, useAlpha ); break;
  case greyPanelSmall: drawPanel( dstpic, rectangle, 68, useAlpha ); break;
  case brownPanelSmall: drawPanel( dstpic, rectangle, 65, useAlpha ); break;
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
  case greyFrame: drawFrame(dstpic, rectangle, 37, useAlpha); break;*/

  case lineBlackBorder:
  case lineWhiteBorder:
  {
    NColor color = mode == lineBlackBorder ? DefaultColors::black : DefaultColors::white;
    drawLine( dstpic, rectangle.lefttop(), rectangle.righttop(), color );
    drawLine( dstpic, rectangle.righttop()-Point(1,0), rectangle.rightbottom()-Point(1,0), color );
    drawLine( dstpic, rectangle.rightbottom()-Point(0,1), rectangle.leftbottom()-Point(0,1), color );
    drawLine( dstpic, rectangle.leftbottom(), rectangle.lefttop(), color );
  }
  break;

  default:
    Logger::warning( "WARNING !!!: Unsupport draw instuctions" );
  break;
  }

  if( updateTexture )
    dstpic.unlock();
}

void Decorator::draw( Pictures& stack, const Rect& rectangle, Decorator::Mode mode)
{
  switch( mode )
  {
  case whiteArea: drawArea( stack, rectangle, 348, 10, 12 ); break;
  case blackArea: drawArea( stack, rectangle, 487, 5, 7 ); break;
  case greyPanel: drawPanel( stack, rectangle, 25 ); break;
  case lightgreyPanel: drawPanel( stack, rectangle, 22 ); break;
  case greyPanelBig: drawPanel( stack, rectangle, 631 ); break;
  case lightgreyPanelBig: drawPanel( stack, rectangle, 634 ); break;
  case greyPanelSmall: drawPanel( stack, rectangle, 68 ); break;
  case brownPanelSmall: drawPanel( stack, rectangle, 65 ); break;
  case whiteBorder: drawBorder( stack, rectangle, 336, 468, 347, 358, 10, 12, 335, 467, 346, 478 );  break;
  case blackBorder: drawBorder( stack, rectangle, 480, 522, 486, 492, 5, 7, 479, 521, 485, 527 ); break;
  case brownBorder: drawBorder(stack, rectangle, 555 ); break;
  case whiteBorderA: drawBorder( stack, rectangle, 547 ); break;
  case whiteFrame:
  {
    Point offset( 16, 16 );
    draw( stack, Rect( rectangle.UpperLeftCorner + offset, rectangle.LowerRightCorner - offset ), whiteArea );
    draw( stack, rectangle, whiteBorder );    // draws borders
  }
  break;

  case blackFrame:
    draw(stack, Rect( rectangle.UpperLeftCorner, rectangle.LowerRightCorner - Point( 16, 16 ) ), blackArea );    // draws the inside of the box
    draw(stack, rectangle, blackBorder );    // draws borders
  break;

  case brownFrame: drawFrame(stack, rectangle, 28); break;
  case greyFrame: drawFrame(stack, rectangle, 37); break;
  case pure: break;
  default: break;
  }
}

/*void Decorator::drawBorder( Picture &dstpic, const Rect& rectangle,
                                   int tp, int bp, int lp, int rp, 
                                   int pCount, int hCount,
                                   int ltc, int lbc, int rtc, int rbc, bool useAlpha )
{
  // draws horizontal borders
  Size size = Picture::load( ResourceGroup::panelBackground, tp ).size();
  const int sw = size.width();
  const int sh = size.height();
  for (int i = 0; i<(rectangle.width()/size.width()-1); ++i)
  {
    Point offset = rectangle.UpperLeftCorner + Point( sw+sw*i, 0 );
    dstpic.draw( Picture::load( ResourceGroup::panelBackground, tp+i%pCount), offset, useAlpha );      // top border
    dstpic.draw( Picture::load( ResourceGroup::panelBackground, bp+i%pCount), offset + Point( 0, rectangle.height()-sh ), useAlpha );      // bottom border
  }

  // draws vertical borders
  for (int i = 0; i<(rectangle.height()/size.height()-1); ++i)
  {
    Point offset = rectangle.UpperLeftCorner + Point( 0, sh+sh*i );
    dstpic.draw( Picture::load( ResourceGroup::panelBackground, lp+hCount*(i%pCount)), offset, useAlpha );      // left border
    dstpic.draw( Picture::load( ResourceGroup::panelBackground, rp+hCount*(i%pCount)), offset + Point( rectangle.width()-sw, 0 ), useAlpha );      // right border
  }

  dstpic.draw( Picture::load( ResourceGroup::panelBackground, ltc), rectangle.UpperLeftCorner );    // left-top corner
  dstpic.draw( Picture::load( ResourceGroup::panelBackground, lbc), Point( rectangle.left(), rectangle.bottom()-sh ), useAlpha );    // left-bottom corner
  dstpic.draw( Picture::load( ResourceGroup::panelBackground, rtc ), Point( rectangle.right() - sw, rectangle.top() ), useAlpha );     // right-top corner
  dstpic.draw( Picture::load( ResourceGroup::panelBackground, rbc), rectangle.LowerRightCorner - Point( sw, sh ), useAlpha );    // right-bottom corner
}*/

void Decorator::drawBorder( Pictures& stack, const Rect& rectangle,
                                   int tp, int bp, int lp, int rp,
                                   int pCount, int hCount,
                                   int ltc, int lbc, int rtc, int rbc )
{
  // draws horizontal borders
  Size size = Picture::load( ResourceGroup::panelBackground, tp ).size();
  const int sw = size.width();
  const int sh = size.height();
  if( !sw || !sh )
  {
    Logger::warning( "Decorator::drawBorder() can't finf texture %s %d", ResourceGroup::panelBackground, tp );
    return;
  }

  int xOff=sw, yOff=sh, i=0;
  for( ; xOff < rectangle.width()-sw; xOff += sw)
  {
    Point offset = Point( xOff, 0 );
    stack.append( Picture::load( ResourceGroup::panelBackground, tp+i%pCount), offset );      // top border
    stack.append( Picture::load( ResourceGroup::panelBackground, bp+i%pCount), offset - Point( 0, rectangle.height()-sh ) );      // bottom border
    i++;
  }

  // draws vertical borders
  i = 0;
  for( ; yOff < rectangle.height()-sh; yOff += sh)
  {
    Point offset = rectangle.lefttop() + Point( 0, -yOff );
    stack.append( Picture::load( ResourceGroup::panelBackground, lp+hCount*(i%pCount)), offset );      // left border
    stack.append( Picture::load( ResourceGroup::panelBackground, rp+hCount*(i%pCount)), offset + Point( rectangle.width()-sw, 0 ) );      // right border
    i++;
  }

  stack.append( Picture::load( ResourceGroup::panelBackground, ltc), rectangle.lefttop() );    // left-top corner
  stack.append( Picture::load( ResourceGroup::panelBackground, lbc), Point( rectangle.left(), -rectangle.bottom()+sh ) );    // left-bottom corner
  stack.append( Picture::load( ResourceGroup::panelBackground, rtc), Point( rectangle.right() - sw, rectangle.top() ) );     // right-top corner
  stack.append( Picture::load( ResourceGroup::panelBackground, rbc), Point( rectangle.right() - sw, -rectangle.bottom()+sh ) );    // right-bottom corner
}

void Decorator::drawArea( Pictures &stack, const Rect& rectangle, int picId, int picCount, int offset )
{
  Point roffset( rectangle.left(), -rectangle.top() );
  int dj = 0;
  for( int dy = 0; dy < rectangle.height(); dy += 16 )
  {
    int di = 0;
    for (int dx = 0; dx < rectangle.width(); dx += 16 )
    {
      // use some clipping to remove the right and bottom areas
      const Picture &srcpic = Picture::load( ResourceGroup::panelBackground, picId + (di%picCount) + offset*(dj%picCount) );
      stack.append( srcpic, roffset + Point( dx, -dy) );
      di++;
    }
    dj++;
  } 
}

/*void Decorator::drawArea(Picture &dstpic, const Rect& rectangle, int picId, int picCount, int offset, bool useAlpha)
{
  for (int j = 0; j<(rectangle.height()/16+1); ++j)
  {
    for (int i = 0; i<(rectangle.width()/16+1); ++i)
    {
      // use some clipping to remove the right and bottom areas
      const Picture &srcpic = Picture::load( ResourceGroup::panelBackground, picId + (i%picCount) + offset*(j%picCount) );

      int dx = 16*i;
      int dy = 16*j;
      int sw = std::min(16, rectangle.width()-dx);
      int sh = std::min(16, rectangle.height()-dy);

      dstpic.draw( srcpic, Rect( 0, 0, sw, sh), rectangle.UpperLeftCorner + Point( dx, dy ) );
    }
  }
}*/

}//end namespace gfx
