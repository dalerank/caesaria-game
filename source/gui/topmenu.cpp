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

#include "topmenu.hpp"

#include "core/gettext.hpp"
#include "label.hpp"
#include "game/resourcegroup.hpp"
#include "contextmenuitem.hpp"
#include "gfx/picturesarray.hpp"
#include "core/utils.hpp"
#include "game/datetimehelper.hpp"
#include "gfx/engine.hpp"
#include "game/gamedate.hpp"
#include "environment.hpp"
#include "widget_helper.hpp"
#include "core/logger.hpp"
#include "window.hpp"
#include "texturedbutton.hpp"
#include "game/advisor.hpp"
#include "widgetescapecloser.hpp"
#include "gfx/decorator.hpp"
#include "listbox.hpp"
#include "core/metric.hpp"
#include "gfx/drawstate.hpp"
#include "city/config.hpp"
#include "core/osystem.hpp"

using namespace gfx;

namespace gui
{

namespace {
static const int panelBgStatus = 15;
}

class TopMenu::Impl
{
public:
  struct {
    Batch batch;
    Pictures fallback;
    Rects rects;
  } bg;

  struct {
    Signal1<int> onShowExtentInfo;
  } signal;

slots public:
  void resolveExtentInfo(Widget* sender);
  void initBackground( const Size& size );
};

void TopMenu::draw(gfx::Engine& engine)
{
  if( !visible() )
    return;

  DrawState pipe( engine, absoluteRect().lefttop(), &absoluteClippingRectRef() );
  pipe.draw( _d->bg.batch )
      .fallback( _d->bg.fallback, _d->bg.rects );

  MainMenu::draw( engine );
}


void TopMenu::Impl::resolveExtentInfo(Widget *sender)
{
  int tag = sender->getProperty( TEXT(ExtentInfo) );
  if( tag != extentinfo::none )
  {
    emit signal.onShowExtentInfo( tag );
  }
}

void TopMenu::Impl::initBackground(const Size& size)
{
  Pictures p_marble;
  p_marble.load( gui::rc.panel, 1, 12 );

  unsigned int i = 0;
  int x = 0;

  float ykoef = size.height() / (float)p_marble.front().height();
  while( x < size.width() )
  {
    const Picture& pic = p_marble[i%12];
    bg.fallback.push_back( pic );
    bg.rects.push_back( Rect( Point( x, 0), pic.size() * ykoef ) );
    x += pic.width() * ykoef;
    i++;
  }

  bool batchOk = bg.batch.load( bg.fallback, bg.rects );
  if( !batchOk )
    bg.batch.destroy();
}

TopMenu::TopMenu(Widget* parent, const int height , bool useIcon)
: MainMenu( parent, Rect( 0, 0, parent->width(), height ) ),
  _d( new Impl )
{
  setGeometry( Rect( 0, 0, parent->width(), height ) );

  _d->initBackground( size() );
  setInternalName(TEXT(TopMenu));
}

Signal1<int> &TopMenu::onShowExtentInfo() { return _d->signal.onShowExtentInfo; }

}//end namespace gui
