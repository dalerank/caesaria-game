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

#ifndef _CAESARIA_PICTURE_DECORATOR_INCLUDE_H_
#define _CAESARIA_PICTURE_DECORATOR_INCLUDE_H_

#include "picturesarray.hpp"

namespace gfx
{

// utility class to draw the user interface
class Decorator
{
public:
  typedef enum { whiteArea=0,   //white marble
                 blackArea,      //black marble rectangular area
                 lightgreyPanel, //lightgray text button background
                 greyPanel,        //gray text button background
                 greyPanelSmall,   //
                 brownPanelSmall,
                 whiteBorder,       //white marble border
                 blackBorder,      //black marble border
                 brownBorder,      // draws brown borders
                 whiteBorderA,      // draws white borders
                 whiteFrame,       //white marble rectangular area with borders
                 blackFrame,         //black marble rectangular area with borders
                 brownFrame,        //brown rectangular area with borders
                 greyFrame,          //grey rectangular area with borders
                 greyPanelBig,
                 lightgreyPanelBig,
                 lineBlackBorder,
                 lineWhiteBorder,
                 pure,
               } Mode;

  static void draw(Picture& dstpic, const Rect& rectangle, Mode mode, bool useAlpha=true, bool updateTexture=false);
  static void draw(Pictures& stack, const Rect& rectangle, Mode mode);

  /*static void drawArea( Picture& dstpic, const Rect& rectangle,  int picId, int picCount, int offset, bool useAlpha );*/
  static void drawArea(Pictures& stack, const Rect& rectangle, int picId, int picCount, int offset);

  // draws a text button background  offset=22(lightgray), 25(gray), 62(small_green), 65(small_brown), 68(small_grey)
  /*static void drawPanel( Picture &dstpic, const Rect& rectangle, int picId, bool useAlpha );*/
  static void drawPanel( Pictures& stack, const Rect& rectangle, int picId );

  /*static void drawBorder( Picture &dstpic, const Rect& rectangle, int tp, int bp, int lp, int rp,
                          int pCount, int hCount, int ltc, int lbc, int rtc, int rbc, bool useAlpha ); */


  // draws a rectangular area
  static void drawFrame(Picture& dstpic, const Rect& rectangle, const int picId, bool useAlpha);
  static void drawFrame(Pictures& stack, const Rect& rectangle, const int picId );

  // draws a rectangular perimeter
  /*static void drawBorder(Picture &dstpic, const Rect& rectangle, const int picId, bool useAlpha); */
  static void drawBorder(Pictures& stack, const Rect& rectangle, const int picId );
  static void drawBorder( Pictures& stack, const Rect& rectangle, int tp, int bp, int lp, int rp,
                          int pCount, int hCount, int ltc, int lbc, int rtc, int rbc );
  static void drawLine( Picture &dstpic, const Point& p1, const Point& p2, NColor color );

  static void basicText( Picture &dstpic, const Point& po, const std::string& text, NColor color );
};

}//end namespace gfx

#endif //_CAESARIA_PICTURE_DECORATOR_INCLUDE_H_
