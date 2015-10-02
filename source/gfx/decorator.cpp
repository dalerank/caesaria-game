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

namespace gfx
{

void Decorator::drawFrame(Pictures& stack, const Rect& rectangle, const int picId )
{
   // pics are: 0TopLeft, 1Top, 2TopRight, 3Left, 4Center, 5Right, 6BottomLeft, 7Bottom, 8BottomRight

   // draws the inside of the box
  Picture bg( ResourceGroup::panelBackground, picId+4);
  const int sw = bg.width();
  const int sh = bg.height();
  for (int j = 0; j<(rectangle.height()/sh-1); ++j)
  {
     for (int i = 0; i<(rectangle.width()/sw-1); ++i)
     {
        stack.append( bg, rectangle.lefttop() + Point( sw+sw*i, sh+sh*j ) );
     }
  }

  // draws horizontal borders
  Picture topBorder( ResourceGroup::panelBackground, picId+1);
  Picture bottomBorder( ResourceGroup::panelBackground, picId+7);
  for (int i = 0; i<(rectangle.width()/sw-1); ++i)
  {
     stack.append( topBorder, rectangle.lefttop() + Point( sw+sw*i, 0 ));
     stack.append( bottomBorder, rectangle.lefttop() + Point( sw+sw*i, rectangle.height()-sh ) );
  }

  // draws vertical borders
  Picture leftBorder( ResourceGroup::panelBackground, picId+3);
  Picture rightBorder( ResourceGroup::panelBackground, picId+5);
  for (int i = 0; i<(rectangle.height()/sh-1); ++i)
  {
     stack.append( leftBorder, rectangle.lefttop() + Point( 0, sh+sh*i ) );
     stack.append( rightBorder, rectangle.lefttop() + Point( rectangle.width()-sw, sh+sh*i ) );
  }

  // topLeft corner
  stack.append( Picture( ResourceGroup::panelBackground, picId+0), rectangle.lefttop() );
  // topRight corner
  stack.append( Picture( ResourceGroup::panelBackground, picId+2), Point( rectangle.right()-sh, rectangle.top() ) );
  // bottomLeft corner
  stack.append( Picture( ResourceGroup::panelBackground, picId+6), Point( rectangle.left(), rectangle.bottom() - sh ) );
  // bottomRight corner
  stack.append( Picture( ResourceGroup::panelBackground, picId+8), rectangle.rightbottom() - Point( 16, 16 ) );
}

void Decorator::drawBorder(Pictures& stack, const Rect& rectangle, const int offset)
{
  // pics are: 0TopLeft, 1Top, 2TopRight, 3Right, 4BottomRight, 5Bottom, 6BottomLeft, 7Left  
  Picture topborder( ResourceGroup::panelBackground, offset+1);
  const int sw = topborder.width();
  const int sh = topborder.height();
  Picture bottomBorder( ResourceGroup::panelBackground, offset+5);

  if( !sw || !sh )
  {
    Logger::warning( "!!! WARNING: Cant draw border for sw=%d, sh=%d", sw, sh );
    return;
  }

  // draws horizontal borders
  for (int i = 0; i<(rectangle.width()/sw-1); ++i)
  {
     stack.append( topborder, rectangle.lefttop() + Point( sw+sw*i, 0 ) );
     stack.append( bottomBorder, rectangle.lefttop() + Point( sw+sw*i, -rectangle.height()+sh ) );
  }

  // draws vertical borders
  Picture leftborder( ResourceGroup::panelBackground, offset+7);
  Picture rightborder( ResourceGroup::panelBackground, offset+3);
  for (int i = 0; i<(rectangle.height()/sh+1); ++i)
  {
     stack.append( leftborder, rectangle.lefttop() + Point( 0, -rectangle.height()+sh*i ) );
     stack.append( rightborder, rectangle.lefttop() + Point( rectangle.width()-sw, -rectangle.height()+sh*i ) );
  }

  // topLeft corner
  stack.append( Picture( ResourceGroup::panelBackground, offset+0), rectangle.lefttop());
  // topRight corner
  stack.append( Picture( ResourceGroup::panelBackground, offset+2), Point( rectangle.right()-sw, rectangle.top() ) );
  // bottomLeft corner
  stack.append( Picture( ResourceGroup::panelBackground, offset+6), Point( rectangle.left(), -rectangle.bottom()+sh ) );
  // bottomRight corner
  stack.append( Picture( ResourceGroup::panelBackground, offset+4), Point( rectangle.right()-16, -rectangle.bottom()+sh ) );
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

void Decorator::reverseYoffset(Pictures& stack)
{
  for( auto&& pic : stack )
    pic.setOffset( Point( pic.offset().x(), -pic.offset().y() ) );
}

void Decorator::drawPanel( Pictures& stack, const Rect& rectangle, int picId, Rects* rects )
{
  // left side
  Picture startpic( ResourceGroup::panelBackground, picId);
  if( !pic.isValid() )
    picId = 68;

  float koeff = 1.f;
  startpic.load( ResourceGroup::panelBackground, picId );
  if( rects != nullptr )
     koeff = rectangle.height() / (float)startpic;

  int width = startpic.width() * koeff;

  stack.append( startpic, rectangle.lefttop() );
  if( rects != nullptr )
    rects->push_back( Rect( rectangle.lefttop(), startpic.size() * koeff ) );

  // draws the inside
  Picture centerPic( ResourceGroup::panelBackground, picId+1);
  for (int i = 0; i<(rectangle.width()/width-1); ++i)
  {
    stack.append( centerPic, rectangle.lefttop() + Point( (i+1)*width, 0 ) );
    if( rects != nullptr )
      rects->push_back( Rect( rectangle.lefttop() + Point( (i+1)*width, 0 ),
                              centerPic.size() * koeff ) );
  }

  // right side
  Picture endpic( ResourceGroup::panelBackground, picId+2);
  stack.append( endpic, rectangle.lefttop() + Point( rectangle.width()-width, 0) );
  if( rects != nullptr )
  {
    rects->push_back( Rect( rectangle.lefttop() + Point( rectangle.width()-width, 0),
                            endpic.size() * koeff ) );
  }
}

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
    Logger::warning( "!!!Warning: Unsupport draw instuctions" );
  break;
  }

  if( updateTexture )
    dstpic.unlock();
}

void Decorator::draw( Pictures& stack, const Rect& rectangle, Decorator::Mode mode, Rects* rects, bool negY )
{
  switch( mode )
  {
  case whiteArea: drawArea( stack, rectangle, 348, 10, 12 ); break;
  case blackArea: drawArea( stack, rectangle, 487, 5, 7 ); break;
  case greyPanel: drawPanel( stack, rectangle, 25 ); break;
  case lightgreyPanel: drawPanel( stack, rectangle, 22, rects ); break;
  case greyPanelBig: drawPanel( stack, rectangle, 631 ); break;
  case lightgreyPanelBig: drawPanel( stack, rectangle, 634 ); break;
  case greyPanelSmall: drawPanel( stack, rectangle, 68 ); break;
  case brownPanelSmall: drawPanel( stack, rectangle, 65 ); break;
  case greenPanelSmall: drawPanel( stack, rectangle, 62); break;
  case redPanelSmall: drawPanel( stack, rectangle, 1165 ); break;
  case whiteBorder: drawBorder( stack, rectangle, 336, 468, 347, 358, 10, 12, 335, 467, 346, 478 );  break;
  case blackBorder: drawBorder( stack, rectangle, 480, 522, 486, 492, 5, 7, 479, 521, 485, 527 ); break;
  case brownBorder: drawBorder(stack, rectangle, 555 ); break;
  case whiteBorderA: drawBorder( stack, rectangle, 547 ); break;
  case whiteFrame:
  {
    Point offset( 16, 16 );
    draw( stack, Rect( rectangle.lefttop() + offset, rectangle.rightbottom() - offset ), whiteArea );
    draw( stack, rectangle, whiteBorder );    // draws borders
  }
  break;

  case blackFrame:
    draw(stack, Rect( rectangle.lefttop(), rectangle.rightbottom() - Point( 16, 16 ) ), blackArea );    // draws the inside of the box
    draw(stack, rectangle, blackBorder );    // draws borders
  break;

  case brownFrame: drawFrame(stack, rectangle, 28); break;
  case greyFrame: drawFrame(stack, rectangle, 37); break;
  case pure: break;
  default: break;
  }

  if( !negY )
    reverseYoffset( stack );
}

void Decorator::drawBorder( Pictures& stack, const Rect& rectangle,
                            int tp, int bp, int lp, int rp,
                            int pCount, int hCount,
                            int ltc, int lbc, int rtc, int rbc )
{
  // draws horizontal borders
  Size size = Picture( ResourceGroup::panelBackground, tp ).size();
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
    stack.append( Picture( ResourceGroup::panelBackground, tp+i%pCount), offset );      // top border
    stack.append( Picture( ResourceGroup::panelBackground, bp+i%pCount), offset - Point( 0, rectangle.height()-sh ) );      // bottom border
    i++;
  }

  // draws vertical borders
  i = 0;
  for( ; yOff < rectangle.height()-sh; yOff += sh)
  {
    Point offset = rectangle.lefttop() + Point( 0, -yOff );
    stack.append( Picture( ResourceGroup::panelBackground, lp+hCount*(i%pCount)), offset );      // left border
    stack.append( Picture( ResourceGroup::panelBackground, rp+hCount*(i%pCount)), offset + Point( rectangle.width()-sw, 0 ) );      // right border
    i++;
  }

  stack.append( Picture( ResourceGroup::panelBackground, ltc), rectangle.lefttop() );    // left-top corner
  stack.append( Picture( ResourceGroup::panelBackground, lbc), Point( rectangle.left(), -rectangle.bottom()+sh ) );    // left-bottom corner
  stack.append( Picture( ResourceGroup::panelBackground, rtc), Point( rectangle.right() - sw, rectangle.top() ) );     // right-top corner
  stack.append( Picture( ResourceGroup::panelBackground, rbc), Point( rectangle.right() - sw, -rectangle.bottom()+sh ) );    // right-bottom corner
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
      Picture srcpic( ResourceGroup::panelBackground, picId + (di%picCount) + offset*(dj%picCount) );
      stack.append( srcpic, roffset + Point( dx, -dy) );
      di++;
    }
    dj++;
  } 
}

}//end namespace gfx
