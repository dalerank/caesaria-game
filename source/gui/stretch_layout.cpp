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

#include "stretch_layout.hpp"
#include "core/variant_map.hpp"
#include "widget_factory.hpp"

namespace gui
{

REGISTER_CLASS_IN_WIDGETFACTORY(HLayout)
REGISTER_CLASS_IN_WIDGETFACTORY(VLayout)

bool compare_tag(Widget* one, Widget* two)
{
  return one->ID() < two->ID();
}

typedef std::vector< Widget* > WidgetsArray;

void Layout::updateLayout()
{
  if( !_needUpdate )
     return;

  _needUpdate = false;
  if( children().size() == 0 )
      return;

  unsigned int side = (( _vertical ? height() : width() ) ) / children().size();

  if( _side > 0 )
    side = math::min(side, _side);

  WidgetsArray ch;
  foreach( it, children() )
    ch.push_back( *it );

  std::sort( ch.begin(), ch.end(), compare_tag );

  unsigned int lastPos = _offset;
  while( ch.size() > 0 )
  {
    Widget* elm = ch.front();
    Rect rectangle( Point( _vertical ? 0 : lastPos, _vertical ? lastPos : 0 ),
                           Size( _vertical ? width() : side,
                                 _vertical ? side : height() ) );
    elm->setGeometry( rectangle );

    lastPos += ( _vertical ? elm->height() : elm->width() );

    WidgetsArray::iterator delIt = ch.begin();
    ch.erase( delIt );
    if( ch.size() )
    {
      WidgetsArray fixSizeElms( ch );

      unsigned int elmsWithothMaxsizeNumber = 0;
      unsigned int afterSide = _vertical ? height() : width();
      while( fixSizeElms.size() > 0 )
      {
        Widget* adapter = *(fixSizeElms.begin());
        afterSide -= ( _vertical ? adapter->maxSize().height() : adapter->maxSize().width() );
        elmsWithothMaxsizeNumber += (( _vertical ? adapter->maxSize().height() : adapter->maxSize().width() ) == 0 ? 1 : 0);

        delIt = fixSizeElms.begin();
        fixSizeElms.erase( delIt );
      }

      side = elmsWithothMaxsizeNumber > 0 ? (( afterSide - lastPos ) / elmsWithothMaxsizeNumber) : 9999;
    }
  }
}

void Layout::beforeDraw( gfx::Engine& painter )
{
  if( !visible() )
    return;

  if( children().size() != _lastElementNumber )
  {
    _lastElementNumber = children().size();
    _needUpdate = true;
  }

  updateLayout();
  Widget::beforeDraw( painter );
}

void Layout::setupUI(const VariantMap &stream)
{
  _side = stream.get( "side" );
  _offset = stream.get( "offset" );

  Widget::setupUI( stream );
}

void Layout::_resizeEvent()
{
  _needUpdate = true;

  Widget::_resizeEvent();
}

Layout::Layout(Widget *parent)
  : Widget( parent, -1, Rect() )
{

}

Layout::Layout( Widget* parent, const Rect& rectangle, bool vertical, int id )
: Widget( parent, id, rectangle ), _vertical( vertical )
{
   _lastElementNumber = 0;
   _needUpdate = false;
}

Layout::~Layout()
{

}

HLayout::HLayout(Widget *parent) : Layout( parent, Rect(), false ) {}

HLayout::HLayout(Widget *parent, const Rect &rect, int id) : Layout( parent, rect, false, id )
{

}

VLayout::VLayout(Widget *parent) : Layout( parent, Rect(), true ) {}

VLayout::VLayout(Widget *parent, const Rect &rect, int id) : Layout( parent, rect, true, id )
{

}

}//end namespac gui
