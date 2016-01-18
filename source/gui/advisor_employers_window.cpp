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
#include "city/statistic.hpp"
#include "game/funds.hpp"
#include "world/empire.hpp"
#include "objects/constants.hpp"
#include "core/color_list.hpp"
#include "objects/working.hpp"
#include "city/statistic.hpp"
#include "core/logger.hpp"
#include "walker/helper.hpp"
#include "core/event.hpp"
#include "widgetescapecloser.hpp"
#include "environment.hpp"
#include "dictionary.hpp"
#include "hire_priority_window.hpp"
#include "city/cityservice_workershire.hpp"
#include "widget_helper.hpp"

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
enum { badAccess=50, simpleAccess=75, goodAccess=90 };
enum { ofLockOffset=45, ofPriority=60, ofBranchName=130, ofNeedWorkers=375, ofHaveWorkers=480 };
}

class SalaryButton : public PushButton
{
public:
  SalaryButton( Widget* parent, const Rect& rectangle, walker::Type wType, int value )
    : PushButton( parent, rectangle, "", -1, false, PushButton::whiteBorderUp )
  {
    walkerType = wType;
    salary = value;
    auto& btnDecrease = add<TexturedButton>( Point( 220, 3 ), gui::button.rowDown );
    btnDecrease.setTooltipText( _("##advslr_btn_tooltip##") );

    auto& btnIncrease = add<TexturedButton>( btnDecrease.righttop(), gui::button.rowUp );
    btnIncrease.setTooltipText( _("##advslr_btn_tooltip##") );
  }

  virtual void _updateTexture()
  {
    PushButton::_updateTexture();

    Font f = font( _state() );
    std::string text = WalkerHelper::getPrettyTypename( walkerType );
    Rect textRect = f.getTextRect( _(text), Rect( 0, 0, width() / 2, height() ), horizontalTextAlign(), verticalTextAlign() );
    f.draw( _textPicture(), _(text), textRect.lefttop(), true );

    text = fmt::format( "{} {}", salary, _("##advemployer_panel_denaries##"));
    textRect = f.getTextRect( text, Rect( width() / 2 + 24 * 2, 0, width(), height() ), horizontalTextAlign(), verticalTextAlign() );
    f.draw( _textPicture(), text, textRect.lefttop(), true );
  }

  void update()
  {
    _finalizeResize();
  }

  int salary;
  walker::Type walkerType;
};

class SalariesWindow : public Window
{
public:
  SalariesWindow( Widget* parent, PlayerCityPtr city )
    : Window( parent, Rect( 0, 0, 480, 480 ), "" )
  {
    Widget::setupUI( ":/gui/advsalaries.gui" );
    _city = city;

    INIT_WIDGET_FROM_UI( Label*, grayArea )
    Point offset( 0, 35 );
    Rect rect( 6, 6, grayArea->width() - 6, 6+30 );
    grayArea->add<SalaryButton>( rect+offset*0, walker::legionary, 30 );
    grayArea->add<SalaryButton>( rect+offset*1, walker::romeGuard, 30 );
    grayArea->add<SalaryButton>( rect+offset*2, walker::romeHorseman, 30 );
    grayArea->add<SalaryButton>( rect+offset*3, walker::romeSpearman, 30 );

    moveTo( Widget::parentCenter );
    WidgetClose::insertTo( this, KEY_RBUTTON );
    setModal();
  }

  PlayerCityPtr _city;
};

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
    _lockPick.load( gui::rc.panel, gui::id.lockpick );

    int percentage = math::percentage( have, need );
    std::string tooltip;
    if( percentage > goodAccess ) tooltip = "##empbutton_tooltip##";
    else if( percentage > simpleAccess ) tooltip = "##empbutton_simple_work##";
    else if( percentage > badAccess ) tooltip = "##empbutton_low_work##";
    else tooltip = "##empbutton_terrible_tooltip##";

    setTooltipText( _(tooltip) );
  }

  void setPriority( int priority )
  {
    _priority = priority;
    _finalizeResize();
  }

signals public:
  Signal1<industry::Type> onClickedSignal;

protected:

  virtual void _updateTexture()
  {
    PushButton::_updateTexture();

    Font font = Font::create( FONT_1_WHITE );

    canvasDraw( _title, Point( ofBranchName, 2), font );
    canvasDraw( utils::i2str( _needWorkers ), Point( ofNeedWorkers, 2), font );
    canvasDraw( utils::i2str( _haveWorkers ), Point( ofHaveWorkers, 2 ),
                _haveWorkers < _needWorkers ? font.withColor(  ColorList::caesarRed ) : font );

    if( _priority > 0 )
      canvasDraw( utils::i2str( _priority ), Point( ofPriority, 3 ), font.withColor( ColorList::black ) );
  }

  virtual void draw(Engine &painter)
  {
    PushButton::draw( painter );

    if( _priority > 0 )
    {
      painter.draw( _lockPick, absoluteRect().lefttop() + Point( ofLockOffset, 4), &absoluteClippingRectRef() );
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
  typedef std::vector< EmployerButton* > EmployerButtons;

  Label* lbSalaries;
  Label* lbYearlyWages;
  Label* lbWorkersState;

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

  int workers = city->statistic().workers.available();
  int worklessPercent = city->statistic().workers.worklessPercent();
  int withoutWork = city->statistic().workers.workless();

  std::string strWorkerState = fmt::format( "{0} {1}     {2} {3}  ( {4}% )",
                                            workers, _("##advemployer_panel_workers##"),
                                            withoutWork, _("##advemployer_panel_workless##"),
                                            worklessPercent );
  lbWorkersState->setText( strWorkerState );
}

void Employer::Impl::updateYearlyWages()
{
  if( !lbYearlyWages )
    return;

  int wages = city->statistic().workers.monthlyWages() * DateTime::monthsInYear;
  std::string wagesStr = fmt::format( "{} {}", _("##workers_yearly_wages_is##"), wages );

  lbYearlyWages->setText( wagesStr );
}

void Employer::Impl::changeSalary(int relative)
{
  int currentSalary = city->treasury().workerSalary();
  city->treasury().setWorkerSalary( currentSalary+relative );
  updateSalaryLabel();
  updateYearlyWages();
}

void Employer::Impl::showPriorityWindow( industry::Type industry )
{
  WorkersHirePtr wh = city->statistic().services.find<WorkersHire>();

  int priority = wh->getPriority( industry );
  auto& wnd = lbSalaries->ui()->add<dialog::HirePriority>( industry, priority );
  CONNECT_LOCAL( &wnd, onAcceptPriority(), Impl::setIndustryPriority );
}

void Employer::Impl::setIndustryPriority( industry::Type industry, int priority)
{
  WorkersHirePtr wh = city->statistic().services.find<WorkersHire>();

  if( wh.isValid() )
  {
    wh->setIndustryPriority( industry, priority );
    empButtons[ industry ]->setPriority( priority );
  }

  update();
}

void Employer::Impl::update()
{
  auto recruter = city->statistic().services.find<WorkersHire>();

  if( recruter.isNull() )
    return;

  for( auto button : empButtons )
  {
    int priority = recruter->getPriority( (industry::Type)button->ID() );
    button->setPriority( priority );
  }
}

void Employer::Impl::updateSalaryLabel()
{
  int pay = city->treasury().workerSalary();
  int romePay = city->empire()->workerSalary();
  std::string salaryString = fmt::format( "{} {} ({} {})",
                                          _("##advemployer_panel_denaries##"), pay,
                                          _("##advemployer_panel_romepay##"), romePay );

  if( lbSalaries )
    lbSalaries->setText( salaryString );
}

Employer::Impl::EmployersInfo Employer::Impl::getEmployersInfo(industry::Type type )
{
  object::Groups groups = industry::toGroups( type );

  WorkingBuildingList buildings;
  for( const auto& gr : groups )
  {
    WorkingBuildingList sectorBuildings = city->statistic().objects.find<WorkingBuilding>( gr );
    buildings.insert( buildings.begin(), sectorBuildings.begin(), sectorBuildings.end() );
  }

  EmployersInfo ret = { 0, 0 };
  for( auto b : buildings )
  {
    ret.currentWorkers += b->numberWorkers();
    ret.needWorkers += b->maximumWorkers();
  }

  return ret;
}

EmployerButton* Employer::Impl::addButton( Employer* parent, const Point& startPos,
                                           industry::Type priority, const std::string& title )
{
  EmployersInfo info = getEmployersInfo( priority );

  auto& employeeBtn = parent->add<EmployerButton>( startPos, priority, title, info.needWorkers, info.currentWorkers );
  employeeBtn.setTooltipText( _("##empbutton_tooltip##") );
  employeeBtn.setText( "" );
  empButtons[ priority ] = &employeeBtn;

  CONNECT_LOCAL( &employeeBtn, onClickedSignal, Impl::showPriorityWindow );

  return &employeeBtn;
}

Employer::Employer(PlayerCityPtr city, Widget* parent, int id )
  : Base( parent, city, id ), _d( new Impl )
{
  Widget::setupUI( ":/gui/employersadv.gui" );

  _d->city = city;
  _d->empButtons.resize( industry::count );

  INIT_WIDGET_FROM_UI( TexturedButton*, btnDecreaseSalary )
  INIT_WIDGET_FROM_UI( PushButton*, btnAdvSalaries )

  LINK_WIDGET_ACTION( TexturedButton*, btnIncreaseSalary, onClicked(), _d.data(), Impl::increaseSalary )
  LINK_WIDGET_ACTION( TexturedButton*, btnDecreaseSalary, onClicked(), _d.data(), Impl::decreaseSalary )
  CONNECT_LOCAL( btnAdvSalaries, onClicked(), Employer::_showAdvSalaries )

  if( btnAdvSalaries )
    btnAdvSalaries->bringToFront();

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

  add<HelpButton>( Point( 12, height() - 39), "labor_advisor" );

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

void Employer::_showAdvSalaries() { ui()->add<SalariesWindow>( _d->city ); }

}//end namespace advisorwnd

}//end namespace gui
