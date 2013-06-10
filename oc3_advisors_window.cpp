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

#include "oc3_advisors_window.hpp"

#include "oc3_picture.hpp"
#include "oc3_pictureconverter.hpp"
#include "oc3_gfx_engine.hpp"
#include "oc3_event.hpp"
#include "oc3_texturedbutton.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_gui_paneling.hpp"
#include "oc3_label.hpp"
#include "oc3_gettext.hpp"
#include "oc3_stringhelper.hpp"
#include "oc3_advisor_employers_window.hpp"
#include "oc3_advisor_legion_window.hpp"

class AdvisorsWindow::Impl
{
public:
  PictureRef background;
  Widget* advisorPanel;

  Point offset;
  PictureRef tabBg;
};

PushButton* AdvisorsWindow::addButton( const int pos, const int picId )
{
  Point tabButtonPos( (getWidth() - 636) / 2 + 10, getHeight() / 2 + 192 + 10);

  PushButton* btn = new TexturedButton( this, tabButtonPos + Point( 48, 0 ) * pos, Size( 40 ), pos, picId, picId, picId + 13 );
  btn->setIsPushButton( true );
  return btn;
}

AdvisorsWindow::AdvisorsWindow( Widget* parent, int id )
: Widget( parent, id, Rect( Point(0, 0), parent->getSize() ) ), _d( new Impl )
{
  // use some clipping to remove the right and bottom areas
  _d->background.reset( Picture::create( getSize() ) );
  _d->advisorPanel = 0;

  Picture& backgr = Picture::load( "senate", 1 );

  _d->background->draw( backgr, Rect( Point( 0, 0), backgr.getSize() ), Rect( Point( 0,0), getSize() ) ); 

  Point tabButtonPos( (getWidth() - 636) / 2, getHeight() / 2 + 192);
  GuiPaneling::instance().draw_white_frame(*_d->background, tabButtonPos.getX(), tabButtonPos.getY(), 
                                                            636, 60 );
  PushButton* btn = addButton( ADV_EMPLOYERS, 255 );
  addButton( ADV_LEGION, 256 );
  addButton( ADV_EMPIRE, 257 );
  addButton( ADV_RATINGS, 258 );
  addButton( ADV_TRADING, 259 );
  addButton( ADV_POPULATION, 260 );
  addButton( ADV_HEALTH, 261 );
  addButton( ADV_EDUCATION, 262 );
  addButton( ADV_ENTERTAINMENT, 263 );
  addButton( ADV_RELIGION, 264 );
  addButton( ADV_FINANCE, 265 );
  addButton( ADV_MAIN, 266 );
  //!!! exit button have no pressed image
  btn = addButton( ADV_COUNT, 267 );
  btn->setPicture( &Picture::load( ResourceGroup::advisorwindow, 3 ), stPressed );
  btn->setIsPushButton( false );
  CONNECT( btn, onClicked(), this, AdvisorsWindow::deleteLater );
  //!!!

  showAdvisor( ADV_EMPLOYERS );
}

void AdvisorsWindow::showAdvisor( const AdvisorType type )
{
  const Widgets& childs = getChildren();
  for( ConstChildIterator it=childs.begin(); it != childs.end(); it++ )
  {
    if( PushButton* btn = safety_cast< PushButton* >( *it ) )
    {
      safety_cast< PushButton* >( *it )->setPressed( btn->getID() == type );
    }
  }

  if( _d->advisorPanel )
  {
    _d->advisorPanel->deleteLater();
    _d->advisorPanel = 0;
  }

  switch( type )
  {
  case ADV_EMPLOYERS: _d->advisorPanel = new AdvisorEmployerWindow( this, ADV_EMPLOYERS ); break;
  case ADV_LEGION: _d->advisorPanel = new AdvisorLegionWindow( this, ADV_LEGION ); break;

  default:
  break;
  }
}

void AdvisorsWindow::draw( GfxEngine& engine )
{
  if( !isVisible() )
    return;

  engine.drawPicture( *_d->background, Point( 0, 0 ) );

  Widget::draw( engine );
}

bool AdvisorsWindow::onEvent( const NEvent& event )
{
  if( event.EventType == OC3_MOUSE_EVENT && event.MouseEvent.Event == OC3_RMOUSE_LEFT_UP )
  {
    deleteLater();
    return true;
  }

  if( event.EventType == OC3_GUI_EVENT && event.GuiEvent.EventType == OC3_BUTTON_CLICKED )
  {
    const Widgets& childs = getChildren();
    for( ConstChildIterator it=childs.begin(); it != childs.end(); it++ )
    {
      if( PushButton* btn = safety_cast< PushButton* >( *it ) )
      {
        if( btn == event.GuiEvent.Caller )
        {
          showAdvisor( (AdvisorType)btn->getID() );
          continue;
        }

        btn->setPressed( false );
      }
    }
  }

  return Widget::onEvent( event );
}

AdvisorsWindow* AdvisorsWindow::create( Widget* parent, int id, const AdvisorType type )
{
  AdvisorsWindow* ret = new AdvisorsWindow( parent, id );
  ret->showAdvisor( type );

  return ret;
}