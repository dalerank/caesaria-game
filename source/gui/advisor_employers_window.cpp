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
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "advisor_employers_window.hpp"
#include "gfx/decorator.hpp"
#include "core/gettext.hpp"
#include "texturedbutton.hpp"
#include "label.hpp"
#include "game/resourcegroup.hpp"
#include "core/utils.hpp"
#include "objects/construction.hpp"
#include "gfx/engine.hpp"
#include "game/enums.hpp"
#include "core/foreach.hpp"
#include "city/helper.hpp"
#include "city/funds.hpp"
#include "world/empire.hpp"
#include "objects/constants.hpp"
#include "objects/working.hpp"
#include "city/statistic.hpp"
#include "core/logger.hpp"
#include "core/event.hpp"
#include "environment.hpp"
#include "dictionary.hpp"
#include "hire_priority_window.hpp"
#include "city/cityservice_workershire.hpp"
#include "widget_helper.hpp"

using namespace constants;
using namespace gfx;
using namespace city;

namespace gui
{

namespace advisorwnd
{

namespace {
static const Point employerButtonOffset = Point( 0, 25 );
static const Size  employerButtonSize = Size( 560, 22 );
static const int idBase = 0x100;
}

class EmployerButton : public PushButton
{
public:
  EmployerButton( Widget* parent, const Point& pos, int index, const std::string& caption, int need, int have )
    : PushButton( parent, Rect( pos + employerButtonOffset * index, employerButtonSize), "", index, false, PushButton::blackBorderUp ),
      _title( caption )
  {
    _needWorkers = need;
    _haveWorkers = have;
    _priority = 0;
    _lockPick = Picture::load( ResourceGroup::panelBackground, 238 );

    int percentage = math::percentage( have, need );
    std::string tooltip;
    if( percentage > 90 ) tooltip = "##empbutton_tooltip##";
    else if( percentage > 75 ) tooltip = "##empbutton_simple_work##";
    else if( percentage > 50 ) tooltip = "##empbutton_low_work##";
    else tooltip = "##empbutton_tooltip##";

    setTooltipText( _(tooltip) );
  }

  void setPriority( int priority )
  {
    _priority = priority;
    _resizeEvent();
  }

signals public:
  Signal1<industry::Type> onClickedSignal;

protected:

  virtual void _updateTextPic()
  {
    PushButton::_updateTextPic();

    PictureRef& pic = _textPictureRef();

    Font font = Font::create( FONT_1_WHITE );
    font.draw( *pic, _title, 130, 2, true, false );
    font.draw( *pic, utils::format( 0xff, "%d", _needWorkers ), 375, 2, true, false );

    if( _haveWorkers < _needWorkers )
    {
      font = Font::create( FONT_1_RED );
    }

    font.draw( *pic, utils::format( 0xff, "%d", _haveWorkers ), 480, 2, true, false );

    if( _priority > 0 )
    {
      font.setColor( DefaultColors::black );
      font.draw( *pic, utils::i2str( _priority ), Point( 60, 3 ), true, false );
    }

    pic->update();
  }

  virtual void draw(Engine &painter)
  {
    PushButton::draw( painter );

    if( _priority > 0 )
    {
      painter.draw( _lockPick, absoluteRect().lefttop() + Point( 45, 4), &absoluteClippingRectRef() );
    }
  }

  virtual void _btnClicked()
  {
    PushButton::_btnClicked();
    emit onClickedSignal( (industry::Type)ID() );
  }

private:
  std::string _title;
  Picture _lockPick;
  int _priority;
  int _needWorkers;
  int _haveWorkers;
};

class Employer::Impl
{
public:
  typedef std::vector< TileOverlay::Type > BldTypes;
  typedef std::vector< EmployerButton* > EmployerButtons;

  gui::Label* lbSalaries;
  gui::Label* lbYearlyWages;
  gui::Label* lbWorkersState;

  PlayerCityPtr city;
  EmployerButtons empButtons;

  struct EmployersInfo {
    unsigned int needWorkers;
    unsigned int currentWorkers;
  };

public:
  void increaseSalary();
  void decreaseSalary();
  void updateSalaryLabel();
  void updateWorkersState();
  void updateYearlyWages();
  void changeSalary( int relative );
  void showPriorityWindow( industry::Type industry);
  void setIndustryPriority( industry::Type industry, int priority );
  void update();
  EmployersInfo getEmployersInfo( industry::Type type );

  EmployerButton* addButton( Employer* parent, const Point& startPos, industry::Type priority, const std::string& title );
};

void Employer::Impl::increaseSalary() { changeSalary( +1 );}
void Employer::Impl::decreaseSalary() { changeSalary( -1 );}

void Employer::Impl::updateWorkersState()
{
  if( !lbWorkersState )
    return;

  int workers = statistic::getAvailableWorkersNumber( city );
  int worklessPercent = statistic::getWorklessPercent( city );
  int withoutWork = statistic::getWorklessNumber( city );

  std::string strWorkerState = utils::format( 0xff, "%d %s     %d %s  ( %d%% )",
                                              workers, _("##advemployer_panel_workers##"),
                                              withoutWork, _("##advemployer_panel_workless##"),
                                              worklessPercent );
  lbWorkersState->setText( strWorkerState );
}

void Employer::Impl::updateYearlyWages()
{
  if( !lbYearlyWages )
    return;

  int wages = statistic::getMonthlyWorkersWages( city ) * DateTime::monthsInYear;
  std::string wagesStr = utils::format( 0xff, "%s %d", _("##workers_yearly_wages_is##"), wages );

  lbYearlyWages->setText( wagesStr );
}

void Employer::Impl::changeSalary(int relative)
{
  int currentSalary = city->funds().workerSalary();
  city->funds().setWorkerSalary( currentSalary+relative );
  updateSalaryLabel();
  updateYearlyWages();
}

void Employer::Impl::showPriorityWindow( industry::Type industry )
{
  city::WorkersHirePtr wh;
  wh << city->findService( city::WorkersHire::defaultName() );

  int priority = wh->getPriority( industry );
  dialog::HirePriority* wnd = new dialog::HirePriority( lbSalaries->ui()->rootWidget(), industry, priority );
  CONNECT( wnd, onAcceptPriority(), this, Impl::setIndustryPriority );
}

void Employer::Impl::setIndustryPriority( industry::Type industry, int priority)
{
  city::WorkersHirePtr wh;
  wh << city->findService( city::WorkersHire::defaultName() );

  if( wh.isValid() )
  {
    wh->setIndustryPriority( industry, priority );
    empButtons[ industry ]->setPriority( priority );
  }

  update();
}

void Employer::Impl::update()
{
  city::WorkersHirePtr wh;
  wh << city->findService( city::WorkersHire::defaultName() );

  if( wh.isNull() )
    return;

  foreach( i, empButtons )
  {
    int priority = wh->getPriority( (industry::Type)(*i)->ID() );
    (*i)->setPriority( priority );
  }
}

void Employer::Impl::updateSalaryLabel()
{
  int pay = city->funds().workerSalary();
  int romePay = city->empire()->workerSalary();
  std::string salaryString = utils::format( 0xff, "%s %d (%s %d)",
                                                   _("##advemployer_panel_denaries##"), pay,
                                                   _("##advemployer_panel_romepay##"), romePay );

  if( lbSalaries )
  {
    lbSalaries->setText( salaryString );
  }
}

Employer::Impl::EmployersInfo Employer::Impl::getEmployersInfo(industry::Type type )
{
  std::vector<objects::Group> bldGroups = city::industry::toGroups( type );

  WorkingBuildingList buildings;
  city::Helper helper( city );
  foreach( buildingsGroup, bldGroups )
  {
    WorkingBuildingList sectorBuildings = helper.find<WorkingBuilding>( *buildingsGroup );
    buildings.insert( buildings.begin(), sectorBuildings.begin(), sectorBuildings.end() );
  }

  EmployersInfo ret = { 0, 0 };
  foreach( b, buildings )
  {
    ret.currentWorkers += (*b)->numberWorkers();
    ret.needWorkers += (*b)->maximumWorkers();
  }

  return ret;
}

EmployerButton* Employer::Impl::addButton( Employer* parent, const Point& startPos,
                                           industry::Type priority, const std::string& title )
{
  EmployersInfo info = getEmployersInfo( priority );

  EmployerButton* btn = new EmployerButton( parent, startPos, priority, title, info.needWorkers, info.currentWorkers );
  btn->setTooltipText( _("##empbutton_tooltip##") );
  btn->setText( "" );
  empButtons[ priority ] = btn;

  CONNECT( btn, onClickedSignal, this, Impl::showPriorityWindow );

  return btn;
}

Employer::Employer(PlayerCityPtr city, Widget* parent, int id )
  : Window( parent, Rect( 0, 0, 1, 1 ), "", id ), _d( new Impl )
{
  Widget::setupUI( ":/gui/employersadv.gui" );
  setPosition( Point( (parent->width() - width()) / 2, parent->height() / 2 - 242 ) );

  _d->city = city;
  _d->empButtons.resize( industry::count );

  INIT_WIDGET_FROM_UI( TexturedButton*, btnIncreaseSalary )
  INIT_WIDGET_FROM_UI( TexturedButton*, btnDecreaseSalary )

  CONNECT( btnIncreaseSalary, onClicked(), _d.data(), Impl::increaseSalary );
  CONNECT( btnDecreaseSalary, onClicked(), _d.data(), Impl::decreaseSalary );

  //buttons _d->_d->background
  Point startPos = Point( 32, 70 ) + Point( 8, 8 );
  _d->addButton( this, startPos, industry::factoryAndTrade, _("##adve_industry_and_trade##") );
  _d->addButton( this, startPos, industry::food, _("##adve_food##") );
  _d->addButton( this, startPos, industry::engineering, _("##adve_engineers##" ) );
  _d->addButton( this, startPos, industry::water, _("##adve_water##") );
  _d->addButton( this, startPos, industry::prefectures, _("##adve_prefectures##") );
  _d->addButton( this, startPos, industry::military, _("##adve_military##") );
  _d->addButton( this, startPos, industry::entertainment, _("##adve_entertainment##") );
  _d->addButton( this, startPos, industry::healthAndEducation, _("##adve_health_education##") );
  _d->addButton( this, startPos, industry::administrationAndReligion, _("##adve_administration_religion##") );

  GET_DWIDGET_FROM_UI( _d, lbSalaries )
  GET_DWIDGET_FROM_UI( _d, lbWorkersState )
  GET_DWIDGET_FROM_UI( _d, lbYearlyWages )

  TexturedButton* btnHelp = new TexturedButton( this, Point( 12, height() - 39), Size( 24 ), -1, ResourceMenu::helpInfBtnPicId );
  CONNECT( btnHelp, onClicked(), this, Employer::_showHelp );

  _d->updateSalaryLabel();
  _d->updateWorkersState();
  _d->updateYearlyWages();
  _d->update();
}

void Employer::draw(Engine& painter )
{
  if( !visible() )
    return;

  Window::draw( painter );
}

bool Employer::onEvent(const NEvent& event)
{
  if( event.EventType == sEventGui && event.gui.type == guiButtonClicked )
  {
    return true;
  }

  return Widget::onEvent( event );
}

void Employer::_showHelp() { DictionaryWindow::show( this, "labor_advisor" ); }

}

}//end namespace gui
