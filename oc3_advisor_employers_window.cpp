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
#include "oc3_picture_decorator.hpp"
#include "oc3_gettext.hpp"
#include "oc3_texturedbutton.hpp"
#include "oc3_gui_label.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_stringhelper.hpp"
#include "oc3_gfx_engine.hpp"
#include "oc3_enums.hpp"
#include "oc3_foreach.hpp"
#include "oc3_city.hpp"

static const Point employerButtonOffset = Point( 0, 25 );
static const Size  employerButtonSize = Size( 560, 22 );

class EmployerButton : public PushButton
{
public:
  EmployerButton( Widget* parent, const Point& pos, int index, const std::string& caption, int need, int have )
    : PushButton( parent, Rect( pos + employerButtonOffset * index, employerButtonSize), "", index, false, PushButton::blackBorderUp )
  {
    _title = caption;
    _needWorkers = need;
    _haveWorkers = have;
  }

  void _updateTexture( ElementState state )
  {
    PushButton::_updateTexture( state );

    PictureRef& pic = _getTextPicture( state );

    Font font = Font::create( FONT_1_WHITE );
    font.draw( *pic, _title, 130, 2 );
    font.draw( *pic, StringHelper::format( 0xff, "%d", _needWorkers ), 375, 2 );

    if( _haveWorkers < _needWorkers )
    {
      font = Font::create( FONT_1_RED );
    }

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
  typedef std::vector< BuildingType > BldTypes;
  enum PriorityIndex
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
  };

  PictureRef background;
  CityPtr city;

  void showPriorityWindow( int id );

  struct EmployersInfo { 
    unsigned int needWorkers;
    unsigned int currentWorkers;
  };

  EmployersInfo getEmployersInfo( PriorityIndex type );

  EmployerButton* addButton( Widget* parent, const Point& startPos, PriorityIndex priority, const std::string& title );
};

void AdvisorEmployerWindow::Impl::showPriorityWindow( int id )
{

}

AdvisorEmployerWindow::Impl::EmployersInfo AdvisorEmployerWindow::Impl::getEmployersInfo( PriorityIndex type )
{
  std::vector< BuildingClass > bldClasses;
  switch( type )
  {
  case prIndustryAndTrade: bldClasses.push_back( BC_INDUSTRY ); bldClasses.push_back( BC_TRADE ); break;
  case prFood: bldClasses.push_back( BC_FOOD ); break;
  case prEngineers: bldClasses.push_back( BC_ENGINEERING ); break;
  case prWater: bldClasses.push_back( BC_WATER ); break;
  case prPrefectures: bldClasses.push_back( BC_SECURITY ); break;
  case prMilitary: bldClasses.push_back( BC_MILITARY ); break;
  case prEntertainment: bldClasses.push_back( BC_ENTERTAINMENT ); break;
  case prHealthAndEducation: bldClasses.push_back( BC_HEALTH ); bldClasses.push_back( BC_EDUCATUION ); break;
  case prAdministrationAndReligion: bldClasses.push_back( BC_ADMINISTRATION ); bldClasses.push_back( BC_RELIGION ); break;
  default: break;
  }

  WorkingBuildings buildings;
  CityHelper helper( city );
  foreach( BuildingClass cl, bldClasses )
  {
    WorkingBuildings sectorBuildings = helper.getBuildings<WorkingBuilding>( cl );
    buildings.insert( buildings.begin(), sectorBuildings.begin(), sectorBuildings.end() );
  }

  EmployersInfo ret = { 0, 0 };
  foreach( WorkingBuildingPtr building, buildings )
  {
    ret.currentWorkers += building->getWorkers();
    ret.needWorkers += building->getMaxWorkers();
  }

  return ret;
}

EmployerButton* AdvisorEmployerWindow::Impl::addButton( Widget* parent, const Point& startPos, 
                                                        PriorityIndex priority, const std::string& title )
{
  EmployersInfo info = getEmployersInfo( priority );

  return new EmployerButton( parent, startPos, priority, title, info.needWorkers, info.currentWorkers );
}

AdvisorEmployerWindow::AdvisorEmployerWindow( CityPtr city, Widget* parent, int id ) 
: Widget( parent, id, Rect( 0, 0, 1, 1 ) ), _d( new Impl )
{
  setGeometry( Rect( Point( (parent->getWidth() - 640 )/2, parent->getHeight() / 2 - 242 ), Size( 640, 416 ) ) );

  _d->city = city;
  _d->background.reset( Picture::create( getSize() ) );
  //main _d->_d->background
  PictureDecorator::draw( *_d->background, Rect( Point( 0, 0 ), getSize() ), PictureDecorator::whiteFrame );

  //buttons _d->_d->background
  Point startPos( 32, 70 );
  PictureDecorator::draw( *_d->background, Rect( startPos, Size( 576, 238 ) ), PictureDecorator::blackFrame );

  //salary _d->_d->background
  Rect salaryBgRect( Point( 64, 352 ), Size( 510, 32 ) );
  PictureDecorator::draw( *_d->background, salaryBgRect, PictureDecorator::blackFrame );

  Font font = Font::create( FONT_1 );
  font.draw( *_d->background, _("##advemployer_panel_priority##"), 56, 54, false );
  font.draw( *_d->background, _("##advemployer_panel_sector##"), 164, 54, false );
  font.draw( *_d->background, _("##advemployer_panel_needworkers##"), 400, 54, false );
  font.draw( *_d->background, _("##advemployer_panel_haveworkers##"), 500, 54, false );

  startPos += Point( 8, 8 );
  _d->addButton( this, startPos, Impl::prIndustryAndTrade, "industry&trade" );
  _d->addButton( this, startPos, Impl::prFood, "food" );
  _d->addButton( this, startPos, Impl::prEngineers, "engineers" );
  _d->addButton( this, startPos, Impl::prWater, "water" );
  _d->addButton( this, startPos, Impl::prPrefectures, "prefectures" );
  _d->addButton( this, startPos, Impl::prMilitary, "military" );
  _d->addButton( this, startPos, Impl::prEntertainment, "entertainment" );
  _d->addButton( this, startPos, Impl::prHealthAndEducation, "health&education" );
  _d->addButton( this, startPos, Impl::prAdministrationAndReligion, "administration&religion" );

  new TexturedButton( this, Point( 160, 356 ), Size( 24 ), -1, 601 );
  new TexturedButton( this, Point( 160+24, 356 ), Size( 24 ), -1, 605 );

  Font font2 = Font::create( FONT_2 );
  font2.draw( *_d->background, _("##advemployer_panel_salary##"), salaryBgRect.UpperLeftCorner + Point( 4, 4), false );

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
