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

#include "message_stack_widget.hpp"
#include "label.hpp"
#include "gfx/decorator.hpp"
#include "widget_deleter.hpp"
#include "gfx/engine.hpp"
#include "game/resourcegroup.hpp"
#include "core/foreach.hpp"
#include "core/utils.hpp"
#include <list>

using namespace gfx;

namespace gui
{

const int WindowMessageStack::defaultID = utils::hash( CAESARIA_STR_EXT(WindowMessageStack) );

class WindowMessageStack::LabelA : public Label
{
public:
  LabelA( Widget* parent, const Rect& rectangle, const std::string& message )
    : Label( parent, rectangle, message )
  {
    setTextAlignment( align::center, align::center );
    new WidgetDeleter( this, 5000 );
  }

protected:
  virtual void _updateBackground(gfx::Engine& painter , bool& useAlpha4Text)
  {
    _backgroundRef().clear();
    Decorator::draw( _backgroundRef(), Rect( Point(), size() ), Decorator::brownPanelSmall );

    Picture& emlbPic = Picture::load( ResourceGroup::panelBackground, PicID::empireStamp );
    _backgroundRef().append( emlbPic, Point( 4, -2 ) );
    _backgroundRef().append( emlbPic, Point( width() - emlbPic.width()-4, -2 ) );
  }
};

WindowMessageStack::WindowMessageStack( Widget* parent, int id, const Rect& rectangle ) 
  : Widget( parent, id, rectangle )
{
}

void WindowMessageStack::draw(gfx::Engine& painter )
{
  if( !visible() || children().empty() )
    return;

  Widget::draw( painter );
}

void WindowMessageStack::_update()
{
  Point offsetLb( width() / 2, 12 );
  Point offset( 0, 23 );
  Widgets wds = children();
  foreach( widget, wds )
  {
    (*widget)->setCenter( offsetLb );
    offsetLb += offset;
  }
}

void WindowMessageStack::beforeDraw(gfx::Engine& painter)
{
  Widget::Widgets wds = children();
  unsigned int myWidth = width();
  int speed = std::max<int>( 20, 2 * myWidth / (painter.fps()+1) );

  foreach( widget, wds )
  {    
    unsigned int wd = (*widget)->width();
    if( wd != myWidth )
    {
      Point center = (*widget)->center();
      (*widget)->setWidth( math::clamp<unsigned int>( wd+speed, 0, myWidth ) );
      (*widget)->setCenter( center );
    }
  }

  Widget::beforeDraw( painter );
}

bool WindowMessageStack::onEvent( const NEvent& ) {  return false; }

void WindowMessageStack::addMessage( std::string message )
{
  if( children().size() > 3 )
  {
    removeChild( *children().begin() );
  }

  new LabelA( this, Rect( 0, 0, 2, 20), message );

  _update();
}

WindowMessageStack* WindowMessageStack::create( Widget* parent )
{
  WindowMessageStack* wnd = new WindowMessageStack( parent, WindowMessageStack::defaultID,
                                                    Rect( 0, 0, parent->width() / 2, 92 ) );
  wnd->setPosition( Point( parent->width() / 4, 33 ) );
  wnd->sendToBack();

  return wnd;
}

}//end namespace gui
