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
// along with CaesarIA.  If not, see <http://www.gnu.org/licenses/>
//
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "legion_target_window.hpp"
#include "core/event.hpp"
#include "world/empire.hpp"
#include "city/city.hpp"
#include "game/resourcegroup.hpp"
#include "gfx/engine.hpp"
#include "core/gettext.hpp"
#include "pushbutton.hpp"

using namespace constants;
using namespace gfx;

namespace gui
{

class LegionTargetWindow::Impl
{
public:
  Point lastCursor;
  Point location;
  Picture locationPic;

  void handleSendTroops();

public signals:
  Signal1<Point> onSelectLocationSignal;
};

LegionTargetWindow::LegionTargetWindow(Widget* parent, int id, PlayerCityPtr city )
  : EmpireMapWindow( parent, id, city ), _d( new Impl )
{
  _d->location = Point( -1, -1 );
  _d->locationPic = Picture::load( ResourceGroup::empirebits, 29 );
  Size s = _d->locationPic.originRect().size() ;
  _d->locationPic.setOffset( Point( -s.width(), s.height() ) / 2 );

  setFlag( showCityInfo, false );
}

void LegionTargetWindow::draw(gfx::Engine& engine )
{
  EmpireMapWindow::draw( engine );

  if( _d->location.x() > 0 )
  {
    engine.draw( _d->locationPic, _d->location + _offset() );
  }
}

bool LegionTargetWindow::onEvent( const NEvent& event )
{
  if( event.EventType == sEventMouse )
  {
    switch(event.mouse.type)
    {
    case mouseLbtnPressed:
      _d->lastCursor = event.mouse.pos();
    break;

    case mouseLbtnRelease:
      if( _d->lastCursor.distanceTo(event.mouse.pos() ) < 2 )
      {
        _d->lastCursor = event.mouse.pos();
        _d->location = _d->lastCursor - _offset();
        _changePosition();
      }
    break;

    default:
    break;
    }
  }

  return EmpireMapWindow::onEvent( event );
}

Signal1<Point>& LegionTargetWindow::onSelectLocation() { return _d->onSelectLocationSignal; }

void LegionTargetWindow::_changePosition()
{
  Widget* wdg = _resetInfoPanel();

  if( wdg )
  {
    Point startDraw = wdg->center() - wdg->lefttop();
    Point offset( 400, 20 );

    PushButton* btnSendTroops = new PushButton( wdg, Rect( startDraw - offset, startDraw ),
                                                _("##lgntrg_send##"), -1, false, PushButton::blackBorderUp );
    PushButton* btnExit = new PushButton( wdg, Rect( btnSendTroops->righttop(), btnSendTroops->righttop() + offset ) ,
                                          _("##lngtrg_exit"), -1, false, PushButton::blackBorderUp );

    CONNECT( btnSendTroops, onClicked(), _d.data(), Impl::handleSendTroops );
    CONNECT( btnExit, onClicked(), wdg->parent(), Widget::deleteLater );
  }
}

LegionTargetWindow* LegionTargetWindow::create(PlayerCityPtr city, Widget* parent, int id )
{
  LegionTargetWindow* ret = new LegionTargetWindow( parent, id, city );

  return ret;
}

LegionTargetWindow::~LegionTargetWindow()
{
}

void LegionTargetWindow::Impl::handleSendTroops()
{
  emit onSelectLocationSignal( location );
}

}//end namespace gui
