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
#include "oc3_pushbutton.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_gui_paneling.hpp"
#include "oc3_label.hpp"
#include "oc3_gettext.hpp"

class AdvisorEmployerPanel : public Widget
{
public:
  typedef enum 
  {
    prIndustryAndTrade=0,
    prFood,
    prEngineers,
    prWater,
    prPrefectures,
    prMilitary,
    prEntertainment,
    prHealthAndEducation,
    prAdministrationAndReligion,
    prCount
  } PriorityIndex;
  AdvisorEmployerPanel( Widget* parent, int id )
    : Widget( parent, id, Rect( 0, 0, 1, 1 ) )
  {
    setGeometry( Rect( Point( (parent->getWidth() - 640 )/2, parent->getHeight() / 2 - 242 ),
                       Size( 640, 416 ) ) );

    _background.reset( Picture::create( getSize() ) );
    //main background
    GuiPaneling::instance().draw_white_frame(*_background, 0, 0, getWidth(), getHeight() );

    //buttons background
    Point startPos( 32, 70 );
    GuiPaneling::instance().draw_black_frame(*_background, startPos.getX(), startPos.getY(), 576, 238 );

    startPos += Point( 8, 8 );
    Size btnSize( 560, 22 );
    Point btnOffset( 0, 25 );
    PushButton* btn = new PushButton( this, Rect( startPos, btnSize ), "industry&trade", prIndustryAndTrade );
    btn = new PushButton( this, Rect( startPos + btnOffset, btnSize ), "food", prFood );
    btn->setFont( Font( FONT_1 ) );
    btn = new PushButton( this, Rect( startPos + btnOffset * 2, btnSize ), "ingineers", prEngineers );
    btn->setFont( Font( FONT_1 ) );
    btn = new PushButton( this, Rect( startPos + btnOffset * 3, btnSize ), "water", prWater );
    btn->setFont( Font( FONT_1 ) );
    btn = new PushButton( this, Rect( startPos + btnOffset * 4, btnSize ), "prefectures", prPrefectures );
    btn->setFont( Font( FONT_1 ) );
    btn = new PushButton( this, Rect( startPos + btnOffset * 5, btnSize ), "military", prMilitary );
    btn->setFont( Font( FONT_1 ) );
    btn = new PushButton( this, Rect( startPos + btnOffset * 6, btnSize ), "entertainment", prEntertainment );
    btn->setFont( Font( FONT_1 ) );
    btn = new PushButton( this, Rect( startPos + btnOffset * 7, btnSize ), "health&education", prHealthAndEducation );
    btn->setFont( Font( FONT_1 ) );
    btn = new PushButton( this, Rect( startPos + btnOffset * 8, btnSize ), "administation&religion", prAdministrationAndReligion );

    Label* title = new Label( this, Rect( 10, 10, getWidth() - 10, 10 + 40) );
    title->setText( _("##advemployer_panel_title##") );
    title->setFont( Font( FONT_3 ) );
    title->setTextAlignment( alignCenter, alignCenter );
  }

  void draw( GfxEngine& painter )
  {
    if( !isVisible() )
      return;

    painter.drawPicture( *_background, getScreenLeft(), getScreenTop() );

    Widget::draw( painter );
  }

private:
  PictureRef _background;
};

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
  Point tabButtonPos( (getWidth() - 636) / 2, getHeight() / 2 + 192);

  Rect buttonRect( tabButtonPos + Point( 10, 10), Size( 40 ) );
  buttonRect += Point( 48, 0 ) * pos;
  PushButton* btn = new PushButton( this, buttonRect, "", pos );
  btn->setPicture( &Picture::load( ResourceGroup::panelBackground, picId ), stNormal );
  btn->setPicture( &Picture::load( ResourceGroup::panelBackground, picId ), stHovered );
  btn->setPicture( &Picture::load( ResourceGroup::panelBackground, picId + 13 ), stPressed );
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
  PushButton* btn = addButton( advisorEmployers, 255 );
  addButton( advisorLegion, 256 );
  addButton( advisorEmpire, 257 );
  addButton( advisorRatings, 258 );
  addButton( advisorTrading, 259 );
  addButton( advisorHistory, 260 );
  addButton( advisorHealth, 261 );
  addButton( advisorEducation, 262 );
  addButton( advisorEntertainment, 263 );
  addButton( advisorReligion, 264 );
  addButton( advisorFinance, 265 );
  addButton( advisorMain, 266 );
  //!!! exit button have no pressed image
  btn = addButton( advisorCount, 267 );
  btn->setPicture( &Picture::load( ResourceGroup::panelBackground, 266 + 12 ), stPressed );
  //!!!

  showAdvisor( advisorEmployers );
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
  case advisorEmployers: _d->advisorPanel = new AdvisorEmployerPanel( this, advisorEmployers ); break;

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

AdvisorsWindow* AdvisorsWindow::create( Widget* parent, int id )
{
  return new AdvisorsWindow( parent, id );
}