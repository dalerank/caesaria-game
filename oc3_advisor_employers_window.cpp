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


#include "oc3_advisor_employers_window.hpp"
#include "oc3_picture.hpp"
#include "oc3_gui_paneling.hpp"
#include "oc3_gettext.hpp"
#include "oc3_pushbutton.hpp"
#include "oc3_label.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_stringhelper.hpp"
#include "oc3_gfx_engine.hpp"

class AdvisorEmployerWindow::Impl
{
public:
  PictureRef background;
};

AdvisorEmployerWindow::AdvisorEmployerWindow( Widget* parent, int id ) 
: Widget( parent, id, Rect( 0, 0, 1, 1 ) ), _d( new Impl )
{
  setGeometry( Rect( Point( (parent->getWidth() - 640 )/2, parent->getHeight() / 2 - 242 ),
    Size( 640, 416 ) ) );

  _d->background.reset( Picture::create( getSize() ) );
  //main _d->_d->background
  GuiPaneling::instance().draw_white_frame(*_d->background, 0, 0, getWidth(), getHeight() );

  //buttons _d->_d->background
  Point startPos( 32, 70 );
  GuiPaneling::instance().draw_black_frame(*_d->background, startPos.getX(), startPos.getY(), 576, 238 );

  //salary _d->_d->background
  Rect salaryBgRect( Point( 64, 352 ), Size( 510, 32 ) );
  GuiPaneling::instance().draw_black_frame(*_d->background, salaryBgRect.getLeft(), salaryBgRect.getTop(),
    salaryBgRect.getWidth(), salaryBgRect.getHeight() );

  Font font( FONT_1 );
  font.draw( *_d->background, _("##advemployer_panel_priority##"), 56, 54 );
  font.draw( *_d->background, _("##advemployer_panel_sector##"), 164, 54 );
  font.draw( *_d->background, _("##advemployer_panel_needworkers##"), 400, 54 );
  font.draw( *_d->background, _("##advemployer_panel_haveworkers##"), 500, 54 );

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

  btn = new PushButton( this, Rect( Point( 160, 356 ), Size( 24 ) ), "", -1 );
  btn->setPicture( &Picture::load( ResourceGroup::advisorwindow, 1 ), stNormal );
  btn->setPicture( &Picture::load( ResourceGroup::advisorwindow, 1 ), stHovered );
  btn->setPicture( &Picture::load( ResourceGroup::advisorwindow, 1 ), stPressed );

  btn = new PushButton( this, Rect( Point( 160+24, 356 ), Size( 24 ) ), "", -1 );
  btn->setPicture( &Picture::load( ResourceGroup::advisorwindow, 2 ), stNormal );
  btn->setPicture( &Picture::load( ResourceGroup::advisorwindow, 2 ), stHovered );
  btn->setPicture( &Picture::load( ResourceGroup::advisorwindow, 2 ), stPressed );

  Font font2( FONT_2 );
  font2.draw( *_d->background, _("##advemployer_panel_salary##"), salaryBgRect.UpperLeftCorner + Point( 4, 4) );

  int pay = 30, romePay = 30;
  std::string salaryString = StringHelper::format( 0xff, "%d %s (%s %d)", pay, _("##advemployer_panel_denaries##"),
    _("##advemployer_panel_romepay##"), romePay );
  new Label( this, Rect( salaryBgRect.UpperLeftCorner + Point( 168, 4), Size( 340, 26 )), salaryString, true, true );

  int workers = 0, withoutWork = 0;
  std::string strWorkerState = StringHelper::format( 0xff, "%d %s     %d %s  ( %d%% )", workers, _("##advemployer_panel_workers##"),
    withoutWork, _("##advemployer_panel_workless##"), (withoutWork * 100/ (workers+1)) );
  new Label( this, Rect( Point( 32, 310 ), Size( 550, 28) ), strWorkerState );

  Label* title = new Label( this, Rect( 10, 10, getWidth() - 10, 10 + 40) );
  title->setText( _("##advemployer_panel_title##") );
  title->setFont( Font( FONT_3 ) );
  title->setTextAlignment( alignCenter, alignCenter );
}

void AdvisorEmployerWindow::draw( GfxEngine& painter )
{
  if( !isVisible() )
    return;

  painter.drawPicture( *_d->background, getScreenLeft(), getScreenTop() );

  Widget::draw( painter );
}