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

#include "advisor_employers_window.hpp"
#include "gfx/decorator.hpp"
#include "core/gettext.hpp"
#include "texturedbutton.hpp"
#include "label.hpp"
#include "game/resourcegroup.hpp"
#include "core/stringhelper.hpp"
#include "gfx/engine.hpp"
#include "game/enums.hpp"
#include "core/foreach.hpp"
#include "city/helper.hpp"
#include "city/funds.hpp"
#include "game/settings.hpp"
#include "world/empire.hpp"
#include "objects/constants.hpp"
#include "objects/working.hpp"
#include "city/statistic.hpp"

using namespace constants;

namespace gui
{

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

  virtual void _updateTexture( ElementState state )
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
  typedef std::vector< TileOverlay::Type > BldTypes;
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

  gui::Label* lbSalary;
  gui::Label* lbYearlyWages;
  gui::Label* lbWorkersState;

  PlayerCityPtr city;

  void showPriorityWindow( int id );
  void increaseSalary();
  void decreaseSalary();
  void updateSalaryLabel();
  void updateWorkersState();
  void updateYearlyWages();
  void changeSalary( int relative );

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

void AdvisorEmployerWindow::Impl::increaseSalary()
{
  changeSalary( +1 );
}

void AdvisorEmployerWindow::Impl::decreaseSalary()
{
  changeSalary( -1 );
}

void AdvisorEmployerWindow::Impl::updateWorkersState()
{
  int workers = city::Statistic::getAvailableWorkersNumber( city );
  int worklessPercent = city::Statistic::getWorklessPercent( city );
  int withoutWork = city::Statistic::getWorklessNumber( city );
  std::string strWorkerState = StringHelper::format( 0xff, "%d %s     %d %s  ( %d%% )",
                                                     workers, _("##advemployer_panel_workers##"),
                                                     withoutWork, _("##advemployer_panel_workless##"),
                                                     worklessPercent );

  if( lbWorkersState )
  {
    lbWorkersState->setText( strWorkerState );
  }
}

void AdvisorEmployerWindow::Impl::updateYearlyWages()
{
  if( lbYearlyWages )
  {
    int wages = city::Statistic::getMontlyWorkersWages( city ) * DateTime::monthInYear;
    std::string wagesStr = StringHelper::format( 0xff, "%s %d", _("##workers_yearly_wages_is##"), wages );

    lbYearlyWages->setText( wagesStr );
  }
}

void AdvisorEmployerWindow::Impl::changeSalary(int relative)
{
  int currentSalary = city->getFunds().getWorkerSalary();
  city->getFunds().setWorkerSalary( currentSalary+relative );
  updateSalaryLabel();
}

void AdvisorEmployerWindow::Impl::updateSalaryLabel()
{
  int pay = 30;
  int romePay = city->getEmpire()->getWorkerSalary();
  std::string salaryString = StringHelper::format( 0xff, "%s %d (%s %d)",
                                                   _("##advemployer_panel_denaries##"), pay,
                                                   _("##advemployer_panel_romepay##"), romePay );

  if( lbSalary )
  {
    lbSalary->setText( salaryString );
  }
}

AdvisorEmployerWindow::Impl::EmployersInfo AdvisorEmployerWindow::Impl::getEmployersInfo( PriorityIndex type )
{
  std::vector< building::Group > bldClasses;
  switch( type )
  {
  case prIndustryAndTrade: bldClasses.push_back( building::industryGroup ); bldClasses.push_back( building::tradeGroup ); break;
  case prFood: bldClasses.push_back( building::foodGroup ); break;
  case prEngineers: bldClasses.push_back( building::engineeringGroup ); break;
  case prWater: bldClasses.push_back( building::waterGroup ); break;
  case prPrefectures: bldClasses.push_back( building::securityGroup ); break;
  case prMilitary: bldClasses.push_back( building::militaryGroup ); break;
  case prEntertainment: bldClasses.push_back( building::entertainmentGroup ); break;
  case prHealthAndEducation: bldClasses.push_back( building::healthGroup ); bldClasses.push_back( building::educationGroup ); break;
  case prAdministrationAndReligion: bldClasses.push_back( building::administrationGroup ); bldClasses.push_back( building::religionGroup ); break;
  default: break;
  }

  WorkingBuildingList buildings;
  city::Helper helper( city );
  foreach( buildingsGroup, bldClasses )
  {
    WorkingBuildingList sectorBuildings = helper.find<WorkingBuilding>( *buildingsGroup );
    buildings.insert( buildings.begin(), sectorBuildings.begin(), sectorBuildings.end() );
  }

  EmployersInfo ret = { 0, 0 };
  foreach( b, buildings )
  {
    ret.currentWorkers += (*b)->numberWorkers();
    ret.needWorkers += (*b)->maxWorkers();
  }

  return ret;
}

EmployerButton* AdvisorEmployerWindow::Impl::addButton( Widget* parent, const Point& startPos, 
                                                        PriorityIndex priority, const std::string& title )
{
  EmployersInfo info = getEmployersInfo( priority );

  EmployerButton* btn = new EmployerButton( parent, startPos, priority, title, info.needWorkers, info.currentWorkers );
  btn->setText( "" );

  return btn;
}

AdvisorEmployerWindow::AdvisorEmployerWindow(PlayerCityPtr city, Widget* parent, int id )
: Widget( parent, id, Rect( 0, 0, 1, 1 ) ), _d( new Impl )
{
  setupUI( GameSettings::rcpath( "/gui/employersadv.gui" ) );
  setPosition( Point( (parent->width() - width()) / 2, parent->height() / 2 - 242 ) );

  _d->city = city;

  //buttons _d->_d->background
  Point startPos = Point( 32, 70 ) + Point( 8, 8 );
  _d->addButton( this, startPos, Impl::prIndustryAndTrade, _("##adve_industry_and_trade##") );
  _d->addButton( this, startPos, Impl::prFood, _("##adve_food##") );
  _d->addButton( this, startPos, Impl::prEngineers, _("##adve_engineers##" ) );
  _d->addButton( this, startPos, Impl::prWater, _("##adve_water##") );
  _d->addButton( this, startPos, Impl::prPrefectures, _("##adve_prefectures##") );
  _d->addButton( this, startPos, Impl::prMilitary, _("##adve_military##") );
  _d->addButton( this, startPos, Impl::prEntertainment, _("##adve_entertainment##") );
  _d->addButton( this, startPos, Impl::prHealthAndEducation, _("##adve_health_education##") );
  _d->addButton( this, startPos, Impl::prAdministrationAndReligion, _("##adve_administration_religion##") );

  _d->lbSalary = findChildA<Label*>( "lbSalaries", true, this );
  _d->lbWorkersState = findChildA<Label*>( "lbWorkersState", true, this );
  _d->lbYearlyWages = findChildA<Label*>( "lbYearlyWages", true, this );

  _d->updateSalaryLabel();
  _d->updateWorkersState();
  _d->updateYearlyWages();
}

void AdvisorEmployerWindow::draw( GfxEngine& painter )
{
  if( !isVisible() )
    return;

  Widget::draw( painter );
}

}//end namespace gui
