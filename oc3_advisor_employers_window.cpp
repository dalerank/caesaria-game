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

static const Point employerButtonOffset = Point( 0, 25 );
static const Size  employerButtonSize = Size( 560, 22 );

class EmployerButton : public PushButton
{
public:
  EmployerButton( Widget* parent, const Point& pos, int index, const std::string& caption, int need, int have )
    : PushButton( parent, Rect( pos + employerButtonOffset * index, employerButtonSize), "", index, false, PushButton::BlackBorderUp )
  {
    _title = caption;
    _needWorkers = need;
    _haveWorkers = have;
  }

  void _updateTexture( ElementState state )
  {
    PushButton::_updateTexture( state );

    PictureRef& pic = _getPicture( state );

    Font font = Font::create( FONT_1_WHITE );
    font.draw( *pic, _title, 130, 2 );

    font.draw( *pic, StringHelper::format( 0xff, "%d", _needWorkers ), 375, 2 );
    font.draw( *pic, StringHelper::format( 0xff, "%d", _haveWorkers ), 480, 2 );
  }

private:
  std::string _title;
  int _needWorkers;
  int _haveWorkers;
};

class AdvisorEmployerWindow::Impl
{
public:
  PictureRef background;

  void showPriorityWindow( int id )
  {

  }
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

  Font font = Font::create( FONT_1 );
  font.draw( *_d->background, _("##advemployer_panel_priority##"), 56, 54 );
  font.draw( *_d->background, _("##advemployer_panel_sector##"), 164, 54 );
  font.draw( *_d->background, _("##advemployer_panel_needworkers##"), 400, 54 );
  font.draw( *_d->background, _("##advemployer_panel_haveworkers##"), 500, 54 );

  startPos += Point( 8, 8 );
  PushButton* btn = new EmployerButton( this, startPos, prIndustryAndTrade, "industry&trade", 0, 0 );
  btn = new EmployerButton( this, startPos, prFood, "food", 0, 0 );
  btn = new EmployerButton( this, startPos, prEngineers, "ingineers", 0, 0 );
  btn = new EmployerButton( this, startPos, prWater, "water", 0, 0 );
  btn = new EmployerButton( this, startPos, prPrefectures, "prefectures", 0, 0 );
  btn = new EmployerButton( this, startPos, prMilitary, "military", 0, 0 );
  btn = new EmployerButton( this, startPos, prEntertainment, "entertainment", 0, 0 );
  btn = new EmployerButton( this, startPos, prHealthAndEducation, "health&education", 0, 0 );
  btn = new EmployerButton( this, startPos, prAdministrationAndReligion, "administation&religion", 0, 0 );

  Picture pic = Picture::load( ResourceGroup::advisorwindow, 1 );
  btn = new PushButton( this, Rect( Point( 160, 356 ), Size( 24 ) ), "", -1 );
  btn->setPicture( &pic, stNormal );
  btn->setPicture( &pic, stHovered );
  btn->setPicture( &pic, stPressed );

  pic = Picture::load( ResourceGroup::advisorwindow, 2 );
  btn = new PushButton( this, Rect( Point( 160+24, 356 ), Size( 24 ) ), "", -1 );
  btn->setPicture( &pic, stNormal );
  btn->setPicture( &pic, stHovered );
  btn->setPicture( &pic, stPressed );

  Font font2 = Font::create( FONT_2 );
  font2.draw( *_d->background, _("##advemployer_panel_salary##"), salaryBgRect.UpperLeftCorner + Point( 4, 4) );

  int pay = 30, romePay = 30;
  std::string salaryString = StringHelper::format( 0xff, "%d %s (%s %d)", pay, _("##advemployer_panel_denaries##"),
    _("##advemployer_panel_romepay##"), romePay );
  new Label( this, Rect( salaryBgRect.UpperLeftCorner + Point( 168, 4), Size( 340, 26 )), salaryString );

  int workers = 0, withoutWork = 0;
  std::string strWorkerState = StringHelper::format( 0xff, "%d %s     %d %s  ( %d%% )", workers, _("##advemployer_panel_workers##"),
    withoutWork, _("##advemployer_panel_workless##"), (withoutWork * 100/ (workers+1)) );
  new Label( this, Rect( Point( 32, 310 ), Size( 550, 28) ), strWorkerState );

  Label* title = new Label( this, Rect( 10, 10, getWidth() - 10, 10 + 40) );
  title->setText( _("##advemployer_panel_title##") );
  title->setFont( Font::create( FONT_3 ) );
  title->setTextAlignment( alignCenter, alignCenter );
}

void AdvisorEmployerWindow::draw( GfxEngine& painter )
{
  if( !isVisible() )
    return;

  painter.drawPicture( *_d->background, getScreenLeft(), getScreenTop() );

  Widget::draw( painter );
}