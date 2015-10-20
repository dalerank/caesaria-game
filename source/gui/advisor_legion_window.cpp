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

#include "advisor_legion_window.hpp"
#include "gfx/decorator.hpp"
#include "core/gettext.hpp"
#include "gui/pushbutton.hpp"
#include "gui/label.hpp"
#include "game/resourcegroup.hpp"
#include "core/utils.hpp"
#include "gfx/engine.hpp"
#include "objects/military.hpp"
#include "city/cityservice_military.hpp"
#include "texturedbutton.hpp"
#include "walker/soldier.hpp"
#include "core/logger.hpp"
#include "events/movecamera.hpp"
#include "widget_helper.hpp"
#include "legion_target_window.hpp"
#include "world/playerarmy.hpp"
#include "dialogbox.hpp"
#include "dictionary.hpp"
#include "city/statistic.hpp"
#include "environment.hpp"

using namespace gfx;
using namespace city;

namespace gui
{

namespace advisorwnd
{

namespace {
  Point legionButtonOffset = Point( 4, 4 );
  Size legionButtonSize = Size( 565, 42 );

  enum { gotoLegionX=360, return2fortX=450, send2empireX=530 };
  Size btnSize(32, 32);
}

class LegionButton : public PushButton
{
public:
  LegionButton( Widget* parent, const Point& pos, int index, FortPtr fort )
    : PushButton( parent, Rect( pos + legionButtonOffset * index, legionButtonSize), "", -1, false, PushButton::blackBorderUp )
  {
    _fort = fort;
    _finalizeResize();

    PushButton* gotoLegion    = new PushButton( this, Rect( Point( gotoLegionX, 5), btnSize ), "", -1, false, PushButton::blackBorderUp );
    gotoLegion->setIcon( ResourceGroup::panelBackground, 563 );
    gotoLegion->setIconOffset( Point( 4, 4 ) );
    PushButton* return2fort   = new PushButton( this, Rect( Point( return2fortX, 5), btnSize ), "", -1, false, PushButton::blackBorderUp );
    return2fort->setIcon(  ResourceGroup::panelBackground, 564 );
    return2fort->setIconOffset( Point( 4, 4 ) );
    return2fort->setTooltipText( _("##return_2_fort##") );
    PushButton* empireService = new PushButton( this, Rect( Point( send2empireX, 5), btnSize ), "", -1, false, PushButton::blackBorderUp );
    empireService->setIcon( ResourceGroup::panelBackground, 566 );
    empireService->setIconOffset( Point( 4, 4 ) );
    empireService->setTooltipText( "##empire_service_tip##");

    CONNECT( gotoLegion, onClicked(), this, LegionButton::_resolveMove2Legion );
    CONNECT( return2fort, onClicked(), this, LegionButton::_resolveReturnLegion2Fort );
    CONNECT( empireService, onClicked(), this, LegionButton::_resolveEmpireService );
  }

  virtual void _updateTextPic()
  {
    PushButton::_updateTextPic();

    Picture& pic = _textPicture();

    Font fontW = Font::create( FONT_1_WHITE );
    Font fontB = Font::create( FONT_1 );

    if( _fort.isValid() )
    {
      fontW.draw( pic, _( _fort->legionName() ), 70, 4 );

      std::string qtyStr = utils::i2str( _fort->soldiers().size() ) +  _("##soldiers##");
      fontB.draw( pic, qtyStr, 70, 22 );

      int moraleValue = _fort->legionMorale() / 10;
      std::string moraleStr = fmt::format( "##legion_morale_{0}##", moraleValue );
      fontB.draw( pic, _( moraleStr ), 180, 15 );
    }    
  }

  virtual void draw(Engine &painter)
  {
    PushButton::draw( painter );

    painter.draw( _fort->legionEmblem(), absoluteRect().lefttop() + Point( 6, 4 ), &absoluteClippingRectRef() );
  }

  struct {
    Signal1<FortPtr> onShowLegion;
    Signal1<FortPtr> onLegionRetreat;
    Signal1<FortPtr> onEmpireService;
  } signal;

private slots:
  void _resolveMove2Legion() { emit signal.onShowLegion( _fort ); }
  void _resolveReturnLegion2Fort() { emit signal.onLegionRetreat( _fort ); }
  void _resolveEmpireService() { emit signal.onEmpireService( _fort ); }

private:
  FortPtr _fort;
};

class Legion::Impl
{
public:
  gui::Label* lbAlarm;
  gui::Label* helpRequest;
  gui::Label* lbBlackframe;
  FortPtr currentFort;
  PlayerCityPtr city;

public:
  void updateAlarms( PlayerCityPtr city );
};

Legion::Legion( Widget* parent, int id, PlayerCityPtr city, FortList forts )
: Window( parent, Rect( 0, 0, 1, 1 ), "", id ), _d( new Impl )
{
  Widget::setupUI( ":/gui/legionadv.gui" );
  setPosition( Point( (parent->width() - 640 )/2, parent->height() / 2 - 242 ) );

  //buttons background
  Point startLegionArea( 32, 70 );
  _d->city = city;

  GET_DWIDGET_FROM_UI( _d, lbAlarm )
  GET_DWIDGET_FROM_UI( _d, helpRequest )
  GET_DWIDGET_FROM_UI( _d, lbBlackframe )

  int index=0;
  for( auto fort : forts )
  {
    auto buttonLegion = new LegionButton( this, startLegionArea + legionButtonOffset, index++, fort );

    CONNECT( buttonLegion, signal.onShowLegion, this, Legion::_handleMove2Legion );
    CONNECT( buttonLegion, signal.onLegionRetreat, this, Legion::_handleRetreaLegion );
    CONNECT( buttonLegion, signal.onEmpireService, this, Legion::_handleServiceEmpire );
  }

  if( _d->lbBlackframe && forts.empty() )
  {
    _d->lbBlackframe->setText( _("##legionadv_no_legions##") );
  }

  _d->updateAlarms( city );

  auto btnHelp = new TexturedButton( this, Point( 12, height() - 39), Size( 24 ), -1, config::id.menu.helpInf );
  CONNECT( btnHelp, onClicked(), this, Legion::_showHelp );
}

void Legion::draw( Engine& painter )
{
  if( !visible() )
    return;

  Window::draw( painter );
}

void Legion::_handleMove2Legion(FortPtr fort)
{
  parent()->deleteLater();
  events::GameEventPtr e = events::MoveCamera::create( fort->patrolLocation() );
  e->dispatch();
}

void Legion::_handleRetreaLegion(FortPtr fort)
{
  if( fort.isValid() )
    fort->returnSoldiers();
}

void Legion::_handleServiceEmpire(FortPtr fort)
{
  bool maySendExpedition = true;
  std::string reasonFailed;
  if( fort->legionMorale() < 25 )
  {
    reasonFailed = "##legion_morale_is_too_low##";
    maySendExpedition = false;
  }

  if( fort->soldiers().empty() )
  {
    reasonFailed = "##legion_havenot_soldiers##";
    maySendExpedition = false;
  }

  if( !maySendExpedition )
  {
    dialog::Information( ui(), "", _(reasonFailed) );
    return;
  }

  LegionTargetWindow* dlg = LegionTargetWindow::create( _d->city, ui()->rootWidget(), -1 );
  dlg->show();

  CONNECT( dlg, onSelectLocation(), fort.object(), Fort::sendExpedition );
}

void Legion::_showHelp()
{
  DictionaryWindow::show( this, "legion_advisor" );
}

void Legion::Impl::updateAlarms(PlayerCityPtr city)
{
  MilitaryPtr mil = city->statistic().services.find<Military>();

  int chasteners_n = city->statistic().walkers.count( walker::romeChastenerSoldier );
  int elephants_n = city->statistic().walkers.count( walker::romeChastenerElephant );

  if( chasteners_n > 0 || elephants_n > 0 )
  {
    lbAlarm->setText( _("##emperror_legion_at_out_gates##") );
    return;
  }

  if( mil->haveNotification( notification::barbarian ) )
  {
    lbAlarm->setText( _("##barbarian_are_closing_city##") );
    return;
  }

  world::PlayerArmyList expeditions = mil->expeditions();
  for( auto expedition : expeditions )
  {
    if( expedition->mode() == world::PlayerArmy::go2location )
    {
      lbAlarm->setText( _("##out_legion_go_to_location##") );
      return;
    }
  }

  for( auto expedition : expeditions )
  {
    if( expedition->mode() == world::PlayerArmy::go2home )
    {
      lbAlarm->setText( _("##out_legion_back_to_city##") );
      return;
    }
  }
}

}//end namespace advisorwnd

}//end namespace gui
