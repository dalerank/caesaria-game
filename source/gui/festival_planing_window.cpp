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
#include "core/utils.hpp"
#include "religion/pantheon.hpp"
#include "core/logger.hpp"
#include "city/statistic.hpp"
#include "widget_helper.hpp"

using namespace religion;
using namespace gfx;
using namespace city;

namespace gui
{

namespace dialog
{

class FestivalPlaning::Impl
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

public signals:
  Signal2<int, int> onFestivalAssignSignal;

public:
  void assignFestival()
  {    
    emit onFestivalAssignSignal( (int)currentDivinity, festivalType );
  }

  void addImage( Widget* parent, RomeDivinityType type, int column, int startPic )
  {
    Size imgSize( 81, 91 );
    godBtns.push_back( new TexturedButton( parent, Point( column * 100 + 60, 48),
                                           imgSize, divId + type, ResourceGroup::festivalimg,
                                           startPic, startPic, startPic+5, startPic+5 ) );
    godBtns.back()->setIsPushButton( true );
    godBtns.back()->setTooltipText( _("##arrange_festiable_for_this_god##") );
    divines[ divId + type ] = type;    
  }

  void updateTitle()
  {
    DivinityPtr divinity = rome::Pantheon::get( currentDivinity );

    std::string text = utils::format( 0xff, "##hold_%s_festival##", divinity.isValid()
                                                                              ? divinity->debugName().c_str()
                                                                              : "unknown" );
    lbTitle->setText( _(text) );
  }
};

FestivalPlaning* FestivalPlaning::create(Widget* parent, PlayerCityPtr city, int id )
{
  FestivalPlaning* ret = new FestivalPlaning( parent, id, Rect( 0, 0, 1, 1 ), city);

  return ret;
}

FestivalPlaning::~FestivalPlaning(){}

FestivalPlaning::FestivalPlaning( Widget* parent, int id, const Rect& rectangle, PlayerCityPtr city)
  : Window( parent, rectangle, "", id ), _d( new Impl )
{
  setupUI( ":/gui/festivalplaning.gui" );
  move( Point( 8, 30) );
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
  _d->festivalCost = city::statistic::getFestivalCost( city, smallFest );

  if( _d->btnSmallFestival )
  {
    _d->btnSmallFestival->setID( Impl::festId+smallFest );
    _d->btnSmallFestival->setText( utils::format( 0xff, "%s %d", _("##small_festival##"), _d->festivalCost ) );
  }

  _d->festivalCost = city::statistic::getFestivalCost( city, middleFest );
  if( _d->btnMiddleFestival )
  {
    _d->btnMiddleFestival->setID( Impl::festId+middleFest );
    _d->btnMiddleFestival->setText( utils::format( 0xff, "%s %d", _("##middle_festival##"), _d->festivalCost ));
  }

  _d->festivalCost = city::statistic::getFestivalCost( city, greatFest );
  if( _d->btnGreatFestival )
  {
    _d->btnGreatFestival->setID( Impl::festId+greatFest );
    _d->btnGreatFestival->setText( utils::format( 0xff, "%s %d", _("##great_festival##"), _d->festivalCost ));
  }

  _d->btnYes = new TexturedButton( this, Point( 350, height() - 50 ), Size( 39, 26), -1, ResourceMenu::okBtnPicId );
  _d->btnNo = new TexturedButton( this, Point( 350 + 43, height() - 50 ), Size( 39, 26), -1, ResourceMenu::cancelBtnPicId );
  _d->btnNo->setTooltipText( "##donot_organize_festival##" );

  CONNECT( _d->btnExit,onClicked(), this, FestivalPlaning::deleteLater );
  CONNECT( _d->btnNo,  onClicked(), this, FestivalPlaning::deleteLater );
  CONNECT( _d->btnYes, onClicked(), _d.data(), Impl::assignFestival );
  CONNECT( _d->btnYes, onClicked(), this, FestivalPlaning::deleteLater );
}

void FestivalPlaning::draw( gfx::Engine& painter )
{
  if( !visible() )
    return;

  Window::draw( painter );
}

bool FestivalPlaning::onEvent(const NEvent& event)
{
  if( event.EventType == sEventGui && event.gui.type == guiButtonClicked )
  {
    PushButton* btn = safety_cast< PushButton* >( event.gui.caller );

    if( btn && btn->ID() != Widget::noId )
    {
      if( (btn->ID() & Impl::divId) == Impl::divId )
      {
        foreach ( abtn, _d->godBtns )  { (*abtn)->setPressed( false ); }

        btn->setPressed( true );
        _d->currentDivinity = _d->divines[ btn->ID() ];

        _d->updateTitle();
      }
      else if( btn->ID() & Impl::festId  )
      {
        _d->btnSmallFestival->setPressed( false );
        _d->btnMiddleFestival->setPressed( false );
        _d->btnGreatFestival->setPressed( false );

        StringArray titles;
        titles << "" << "##small_festival##" << "##middle_festival##" << "##great_festival##";
        _d->festivalType = math::clamp<int>( btn->ID() & 0xf, 0, titles.size() - 1);
        _d->lbFestivalName->setText( _( titles[ _d->festivalType ] ) );

        btn->setPressed( true );
      }

      return true;
    }
  }

  return Widget::onEvent( event );
}

Signal2<int,int>& FestivalPlaning::onFestivalAssign() {  return _d->onFestivalAssignSignal;}

}//end namespace dialog

}//end namespace gui
