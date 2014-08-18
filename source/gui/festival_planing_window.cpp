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

#include "festival_planing_window.hpp"
#include "label.hpp"
#include "game/enums.hpp"
#include "texturedbutton.hpp"
#include "gfx/decorator.hpp"
#include "core/event.hpp"
#include "gui/environment.hpp"
#include "core/foreach.hpp"
#include "city/city.hpp"
#include "core/gettext.hpp"
#include "city/funds.hpp"
#include "gfx/engine.hpp"
#include "core/stringhelper.hpp"
#include "religion/pantheon.hpp"
#include "core/logger.hpp"
#include "city/statistic.hpp"
#include "widget_helper.hpp"

using namespace religion;
using namespace gfx;

namespace gui
{

class FestivalPlaningWindow::Impl
{
public:
  typedef enum { divId=0x200, festId=0x400 } FestID;
  Label* lbTitle;
  Label* lbFestivalName;
  int festivalType;
  unsigned int festivalCost;
  std::vector<TexturedButton*> godBtns;
  std::map<int, RomeDivinityType > divines;

  PushButton* btnHelp;
  PushButton* btnExit;
  PushButton* btnSmallFestival;
  PushButton* btnMiddleFestival;
  PushButton* btnGreatFestival;

  TexturedButton* btnYes;
  TexturedButton* btnNo;

  PlayerCityPtr city;
  RomeDivinityType currentDivinity;

public oc3_signals:
  Signal2<int, int> onFestivalAssignSignal;

public:
  void assignFestival()
  {    
    oc3_emit onFestivalAssignSignal( (int)currentDivinity, festivalType );
  }

  void addImage( Widget* parent, RomeDivinityType type, int column, int startPic )
  {
    Size imgSize( 81, 91 );
    godBtns.push_back( new TexturedButton( parent, Point( column * 100 + 60, 48),
                                           imgSize, divId + type, ResourceGroup::festivalimg,
                                           startPic, startPic, startPic+5, startPic+5 ) );
    godBtns.back()->setIsPushButton( true );
    divines[ divId + type ] = type;
  }

  void updateTitle()
  {
    DivinityPtr divinity = rome::Pantheon::get( currentDivinity );

    std::string text = StringHelper::format( 0xff, "##hold_%s_festival##", divinity.isValid()
                                                                              ? divinity->getDebugName().c_str()
                                                                              : "unknown" );
    lbTitle->setText( _(text) );
  }
};

FestivalPlaningWindow* FestivalPlaningWindow::create(Widget* parent, PlayerCityPtr city, int id )
{
  Size size( 625, 320 );

  Rect rectangle( Point( (parent->width() - size.width())/2, (parent->height() - size.height())/2 ), size );
  FestivalPlaningWindow* ret = new FestivalPlaningWindow( parent, id, rectangle, city);

  return ret;
}

FestivalPlaningWindow::~FestivalPlaningWindow(){}

FestivalPlaningWindow::FestivalPlaningWindow( Widget* parent, int id, const Rect& rectangle, PlayerCityPtr city)
  : Window( parent, rectangle, "", id ), _d( new Impl )
{
  setupUI( ":/gui/festivalplaning.gui" );
  setCenter( parent->center() );
  _d->city = city;

  _d->addImage( this, romeDivCeres, 0, 17 );
  _d->addImage( this, romeDivNeptune, 1, 18 );
  _d->addImage( this, romeDivMercury, 2, 19 );
  _d->addImage( this, romeDivMars, 3, 20 );
  _d->addImage( this, romeDivVenus, 4, 21 );

  _d->godBtns.front()->setPressed( true );
  _d->currentDivinity = romeDivCeres;

  GET_DWIDGET_FROM_UI( _d, lbTitle )
  GET_DWIDGET_FROM_UI( _d, lbFestivalName )
  GET_DWIDGET_FROM_UI( _d, btnSmallFestival )
  GET_DWIDGET_FROM_UI( _d, btnMiddleFestival )
  GET_DWIDGET_FROM_UI( _d, btnGreatFestival )

  _d->updateTitle();
  _d->festivalType = smallFest;

  _d->btnHelp = new TexturedButton( this, Point( 52, height() - 52 ), Size( 24 ), -1, ResourceMenu::helpInfBtnPicId );
  _d->btnExit = new TexturedButton( this, Point( width() - 74, height() - 52 ), Size( 24 ), -1, ResourceMenu::exitInfBtnPicId );

  /*int money = _d->city->getFunds().getValue();*/
  _d->festivalCost = city::Statistic::getFestivalCost( city, smallFest );

  if( _d->btnSmallFestival )
  {
    _d->btnSmallFestival->setID( Impl::festId+smallFest );
    _d->btnSmallFestival->setText( StringHelper::format( 0xff, "%s %d", _("##small_festival##"), _d->festivalCost ) );
  }

  _d->festivalCost = city::Statistic::getFestivalCost( city, middleFest );
  if( _d->btnMiddleFestival )
  {
    _d->btnMiddleFestival->setID( Impl::festId+middleFest );
    _d->btnMiddleFestival->setText( StringHelper::format( 0xff, "%s %d", _("##middle_festival##"), _d->festivalCost ));
  }

  _d->festivalCost = city::Statistic::getFestivalCost( city, greatFest );
  if( _d->btnGreatFestival )
  {
    _d->btnGreatFestival->setID( Impl::festId+greatFest );
    _d->btnGreatFestival->setText( StringHelper::format( 0xff, "%s %d", _("##great_festival##"), _d->festivalCost ));
  }

  _d->btnYes = new TexturedButton( this, Point( 350, height() - 50 ), Size( 39, 26), -1, ResourceMenu::okBtnPicId );
  _d->btnNo = new TexturedButton( this, Point( 350 + 43, height() - 50 ), Size( 39, 26), -1, ResourceMenu::cancelBtnPicId );

  CONNECT( _d->btnExit,onClicked(), this, FestivalPlaningWindow::deleteLater );
  CONNECT( _d->btnNo,  onClicked(), this, FestivalPlaningWindow::deleteLater );
  CONNECT( _d->btnYes, onClicked(), _d.data(), Impl::assignFestival );
  CONNECT( _d->btnYes, onClicked(), this, FestivalPlaningWindow::deleteLater );
}

void FestivalPlaningWindow::draw( gfx::Engine& painter )
{
  if( !visible() )
    return;

  Window::draw( painter );
}

bool FestivalPlaningWindow::onEvent(const NEvent& event)
{
  if( event.EventType == sEventGui && event.gui.type == guiButtonClicked )
  {
    PushButton* btn = safety_cast< PushButton* >( event.gui.caller );
    if( btn && (btn->ID() & Impl::divId ) )
    {
      foreach ( abtn, _d->godBtns )  { (*abtn)->setPressed( false ); }

      btn->setPressed( true );
      _d->currentDivinity = _d->divines[ btn->ID() ];

      _d->updateTitle();
    }
    else if( btn && (btn->ID() & Impl::festId ) )
    {
      _d->btnSmallFestival->setPressed( false );
      _d->btnMiddleFestival->setPressed( false );
      _d->btnGreatFestival->setPressed( false );

      StringArray titles;
      titles << "" << "##small_festival##" << "##middle_festival##" << "##great_festival##";
      _d->festivalType = (btn->ID() & 0xf);
      _d->lbFestivalName->setText( _( titles[ _d->festivalType ] ) );

      btn->setPressed( true );
    }

    return true;
  }

  return Widget::onEvent( event );
}

Signal2<int,int>& FestivalPlaningWindow::onFestivalAssign() {  return _d->onFestivalAssignSignal;}

}//end namespace gui
