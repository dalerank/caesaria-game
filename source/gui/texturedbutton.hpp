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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_TEXTURED_BUTTON_H_INCLUDED__
#define __CAESARIA_TEXTURED_BUTTON_H_INCLUDED__

#include "pushbutton.hpp"

namespace gui
{

class TexturedButton : public PushButton
{
public:
  static const int defaultSize = 24;
  struct States
  {
    int normal;
    int hover;
    int pressed;
    int disabled;
    States( int n, int h=-1, int p=-1, int d=-1 )
     : normal( n ), hover( h ), pressed( p ), disabled( d )
    {}
  };

  TexturedButton( Widget* parent, const Point& pos, const States& states );

  TexturedButton( Widget* parent, const Point& pos, const Size& size, int id,
                  const States& states );

  TexturedButton( Widget* parent, const Point& pos, const Size& size, int id,
                  const std::string& resourceGroup,
                  const States& states );

  TexturedButton( Widget* parent );

  void changeImageSet(const std::string& rc, int normal, int hover, int pressed, int disabled );
  void changeImageSet(const States& states, const std::string& rc="");
};

class HelpButton : public TexturedButton
{
public:
  HelpButton(Widget* parent);
  HelpButton(Widget* parent, const Point& pos, const std::string& helpId, int id=Widget::noId );

  virtual void setupUI( const VariantMap& ui );

protected:
  virtual void _btnClicked();
  std::string _helpid;
};

class ExitButton : public TexturedButton
{
public:
  ExitButton(Widget* parent);
  ExitButton(Widget* parent, const Point& pos, int id=Widget::noId);

protected:
  virtual void _btnClicked();
};

}//end namespace gui
#endif //__CAESARIA_TEXTURED_BUTTON_H_INCLUDED__
