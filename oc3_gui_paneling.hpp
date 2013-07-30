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


#ifndef GUI_PANELING_HPP
#define GUI_PANELING_HPP

#include "oc3_picture.hpp"

class PushButton;

// utility class to draw the user interface
class GuiPaneling
{
public:
   static GuiPaneling& instance();

   // draws a white marble rectangular area
   void draw_white_area(Picture &dstpic, const int x, const int y, const int width, const int height);

   // draws a black marble rectangular area
   void draw_black_area(Picture &dstpic, const int x, const int y, const int width, const int height);

   // draws a white marble border
   void draw_white_borders(Picture &dstpic, const int x, const int y, const int width, const int height);

   // draws a black marble border
   void draw_black_borders(Picture &dstpic, const int x, const int y, const int width, const int height);

   // draws a white marble rectangular area with borders
   void draw_white_frame(Picture &dstpic, const int x, const int y, const int width, const int height);

   // draws a black marble rectangular area with borders
   void draw_black_frame(Picture &dstpic, const int x, const int y, const int width, const int height);

   // draws a brown rectangular area with borders
   void draw_brown_frame(Picture &dstpic, const int x, const int y, const int width, const int height);

   // draws a grey rectangular area with borders
   void draw_grey_frame(Picture &dstpic, const int x, const int y, const int width, const int height);

   // draws brown borders
   void draw_brown0_borders(Picture &dstpic, const int x, const int y, const int width, const int height);

   // draws white borders
   void draw_white0_borders(Picture &dstpic, const int x, const int y, const int width, const int height);

   // draws a text button background  offset=22(lightgray), 25(gray), 62(small_green), 65(small_brown), 68(small_grey)
   void draw_basic_text_button(Picture &dstpic, const int x, const int y, const int width, const int offset);

   // draws a rectangular area
   void draw_basic_frame(Picture &dstpic, const int x, const int y, const int width, const int height, const int offset);

   // draws a rectangular perimeter
   void draw_basic0_borders(Picture &dstpic, const int x, const int y, const int width, const int height, const int offset);

private:
   GuiPaneling();
};

#endif
