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

#ifndef __CAESARIA_GUIENVIRONMENT_INCLUDE_
#define __CAESARIA_GUIENVIRONMENT_INCLUDE_

#include "widget.hpp"

namespace gfx { class Engine; }

namespace gui
{

class Ui : Widget
{
public:
  template<typename WidgetClass, typename... Args>
  WidgetClass& add( const Args& ... args)
  {
    WidgetClass* widget = new WidgetClass( this, args... );
    return *widget;
  }

  template<typename WidgetClass>
  WidgetClass* findWidget()
  {
    for( auto widget : children() )
    {
      WidgetClass* ret = safety_cast<WidgetClass*>( widget );
      if( ret )
        return ret;
    }

    return nullptr;
  }

  typedef enum { showTooltips=0, drawDebugArea } Flag;
  Ui( gfx::Engine& painter );

  virtual ~Ui();

  bool hasFocus( const Widget* element) const;
  virtual bool setFocus( Widget* element);
  virtual bool removeFocus( Widget* element);

  Widget* rootWidget();
  Widget* getFocus() const;
  Point cursorPos() const;

  virtual bool isHovered( const Widget* element );
  Widget* findWidget( int id );
  Widget* findWidget( const Point& p );
  Widget* hovered() const;

  virtual void draw();
  virtual void beforeDraw();

  void animate(unsigned int time);
  Size vsize() const;
  bool handleEvent(const NEvent& event);

  virtual void deleteLater( Widget* ptrElement );

  Widget* createWidget( const std::string& type, Widget* parent );

  void setFlag( Flag name, int value );
  bool hasFlag( Flag name );
  void clear();
   
private:    
  virtual void setFocus();
  virtual void removeFocus();
  virtual void beforeDraw( gfx::Engine& painter );
  virtual void draw( gfx::Engine& painter );
  virtual bool isHovered() const;

  void _updateHovered( const Point& mousePos);
  Widget* next(bool reverse, bool group);

  class Impl;
  ScopedPtr<Impl> _d;
};

}//end namespace gui
#endif //__CAESARIA_GUIENVIRONMENT_INCLUDE_
