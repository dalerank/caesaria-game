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
#include "texturedbutton.hpp"
#include "gfx/decorator.hpp"
#include "objects/construction.hpp"
#include "core/event.hpp"
#include "gui/environment.hpp"
#include "city/city.hpp"
#include "core/gettext.hpp"
#include "game/funds.hpp"
#include "gfx/engine.hpp"
#include "core/utils.hpp"
#include "religion/pantheon.hpp"
#include "core/logger.hpp"
#include "game/resourcegroup.hpp"
#include "city/statistic.hpp"
#include "widget_helper.hpp"

using namespace religion;
using namespace gfx;
using namespace city;

namespace gui
{

namespace dialog
{

class FestivalPlanning::Impl
{
public:
  typedef enum { divId=0x200, festId=0x400 } FestID;
  Label* lbTitle;
  Label* lbFestivalName;
  int type;
  unsigned int cost;
  std::vector<TexturedButton*> godBtns;
  std::map<int, RomeDivinity::Type > divines;

  PushButton* btnSmallFestival;
  PushButton* btnMiddleFestival;
  PushButton* btnGreatFestival;

  PlayerCityPtr city;
  RomeDivinity::Type currentDivinity;

public signals:
  Signal2<int, int> onFestivalAssignSignal;

public:
  void addImage( Widget* parent, RomeDivinity::Type type, int column, int startPic )
  {
    Size imgSize( 81, 91 );
    auto& btn = parent->add<TexturedButton>( Point( column * 100 + 60, 48),
                                             imgSize, divId + type, gui::miniature.rc,
                                             TexturedButton::States( startPic, startPic, startPic+5, startPic+5 ) );
    btn.setIsPushButton( true );
    btn.setTooltipText( _("##arrange_festiable_for_this_god##") );
    godBtns.push_back( &btn );
    divines[ divId + type ] = type;    
  }

  void updateTitle()
  {
    DivinityPtr divinity = rome::Pantheon::get( currentDivinity );

    std::string text = fmt::format( "##hold_{}_festival##", divinity.isValid()
                                                               ? divinity->debugName()
                                                               : "unknown" );
    lbTitle->setText( _(text) );
  }
};

FestivalPlanning::~FestivalPlanning(){}

FestivalPlanning::FestivalPlanning( Widget* parent, int id, const Rect& rectangle, PlayerCityPtr city)
  : Window( parent, rectangle, "", id ), _d( new Impl )
{
  setupUI( ":/gui/festivalplaning.gui" );
  move( Point( 8, 30) );
  _d->city = city;

  _d->addImage( this, RomeDivinity::Ceres, 0, 17 );
  _d->addImage( this, RomeDivinity::Neptune, 1, 18 );
  _d->addImage( this, RomeDivinity::Mercury, 2, 19 );
  _d->addImage( this, RomeDivinity::Mars, 3, 20 );
  _d->addImage( this, RomeDivinity::Venus, 4, 21 );

  _d->godBtns.front()->setPressed( true );
  _d->currentDivinity = RomeDivinity::Ceres;

  GET_DWIDGET_FROM_UI( _d, lbTitle )
  GET_DWIDGET_FROM_UI( _d, lbFestivalName )
  GET_DWIDGET_FROM_UI( _d, btnSmallFestival )
  GET_DWIDGET_FROM_UI( _d, btnMiddleFestival )
  GET_DWIDGET_FROM_UI( _d, btnGreatFestival )

  _d->updateTitle();
  _d->type = smallFest;


  /*int money = _d->city->getFunds().getValue();*/
  _d->cost = city->statistic().festival.calcCost( smallFest );

  if( _d->btnSmallFestival )
  {
    _d->btnSmallFestival->setID( Impl::festId+smallFest );
    _d->btnSmallFestival->setText( fmt::format( "{} {}", _("##small_festival##"), _d->cost ) );
  }

  _d->cost = city->statistic().festival.calcCost( middleFest );
  if( _d->btnMiddleFestival )
  {
    _d->btnMiddleFestival->setID( Impl::festId+middleFest );
    _d->btnMiddleFestival->setText( fmt::format( "{} {}", _("##middle_festival##"), _d->cost ));
  }

  _d->cost = city->statistic().festival.calcCost( greatFest );
  if( _d->btnGreatFestival )
  {
    _d->btnGreatFestival->setID( Impl::festId+greatFest );
    _d->btnGreatFestival->setText( fmt::format( "{} {}", _("##great_festival##"), _d->cost ));
  }

  add<HelpButton>( Point( 52, height() - 52 ), "festival_adv" );
  auto& btnYes = add<TexturedButton>( Point( 350, height() - 50 ), Size( 39, 26), -1, gui::button.ok );
  auto& btnNo = add<TexturedButton>( Point( 350 + 43, height() - 50 ), Size( 39, 26), -1, gui::button.cancel );
  add<ExitButton>( Point( width() - 74, height() - 52 ), Widget::noId );

  btnYes.setTooltipText( _("##new_festival##") );
  btnNo.setTooltipText( _("##donot_organize_festival##") );

  CONNECT_LOCAL( &btnNo,  onClicked(), FestivalPlanning::deleteLater );
  CONNECT_LOCAL( &btnYes, onClicked(), FestivalPlanning::_assignFestival );
  CONNECT_LOCAL( &btnYes, onClicked(), FestivalPlanning::deleteLater );
}

void FestivalPlanning::draw( gfx::Engine& painter )
{
  if( !visible() )
    return;

  Window::draw( painter );
}

bool FestivalPlanning::onEvent(const NEvent& event)
{
  if( event.EventType == sEventGui && event.gui.type == guiButtonClicked )
  {
    PushButton* btn = safety_cast< PushButton* >( event.gui.caller );

    if( btn && btn->ID() != Widget::noId )
    {
      if( (btn->ID() & Impl::divId) == Impl::divId )
      {
        for( auto abtn : _d->godBtns ) { abtn->setPressed( false ); }

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
        _d->type = math::clamp<int>( btn->ID() & 0xf, 0, titles.size() - 1);
        _d->lbFestivalName->setText( _( titles[ _d->type ] ) );

        btn->setPressed( true );
      }

      return true;
    }
  }

  return Widget::onEvent( event );
}

Signal2<int,int>& FestivalPlanning::onFestivalAssign() { return _d->onFestivalAssignSignal; }

void FestivalPlanning::_assignFestival()
{
  emit _d->onFestivalAssignSignal( (int)_d->currentDivinity, _d->type );
}

}//end namespace dialog

}//end namespace gui
