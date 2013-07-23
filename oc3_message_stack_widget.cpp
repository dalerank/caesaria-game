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

#include "oc3_message_stack_widget.hpp"
#include "oc3_gui_label.hpp"
#include "oc3_gui_paneling.hpp"
#include "oc3_widget_deleter.hpp"
#include "oc3_gfx_engine.hpp"
#include "oc3_resourcegroup.hpp"
#include <list>

class WindowMessageStack::Impl
{
public:
  PictureRef lbBackgorund;

  void updatePositions( Widget::Widgets& childs, const Rect& rectangle )
  {
    Point offsetLb( 0, 0 );
    Point offset( 0, 23 );
    for( Widget::ChildIterator it=childs.begin(); it != childs.end(); it++ )
    {
      (*it)->setPosition( offsetLb );
      offsetLb += offset;
    }
  }
};

WindowMessageStack::WindowMessageStack( Widget* parent, int id, const Rect& rectangle ) 
  : Widget( parent, id, rectangle ), _d( new Impl )
{
  _d->lbBackgorund.reset( Picture::create( Size( rectangle.getWidth(), 20 ) ) );
  GuiPaneling::instance().draw_basic_text_button( *_d->lbBackgorund, 0, 0, rectangle.getWidth(), 65 );
  Picture& emlbPic = Picture::load( ResourceGroup::panelBackground, 543 );
  _d->lbBackgorund->draw( emlbPic, 4, 2 );
  _d->lbBackgorund->draw( emlbPic, getWidth() - emlbPic.getWidth()-4, 2 );
}

void WindowMessageStack::draw( GfxEngine& painter )
{
  if( !isVisible() || getChildren().empty() )
    return;

  Widget::draw( painter );
}

bool WindowMessageStack::onEvent( const NEvent& event )
{
  return false;
}

void WindowMessageStack::addMessage( std::string message )
{
  if( getChildren().size() > 3 )
  {
    removeChild( *getChildren().begin() );
  }

  Label* lbMessage = new Label( this, Rect( 0, 0, getWidth(), 20), message );
  lbMessage->setTextAlignment( alignCenter, alignCenter );
  lbMessage->setBackgroundPicture( *_d->lbBackgorund );
  new WidgetDeleter( lbMessage, 5000 );

  _d->updatePositions( const_cast< Widget::Widgets& >( getChildren() ), getRelativeRect() );
}

WindowMessageStack* WindowMessageStack::create( Widget* parent, int id )
{
  Rect rectangle( 0, 0, parent->getWidth() / 2, 92 );

  WindowMessageStack* wnd = new WindowMessageStack( parent, id, rectangle );
  return wnd;
}