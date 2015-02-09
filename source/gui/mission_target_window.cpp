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

#include "mission_target_window.hpp"
#include "game/resourcegroup.hpp"
#include "gfx/decorator.hpp"
#include "gui/label.hpp"
#include "listbox.hpp"
#include "game/player.hpp"
#include "texturedbutton.hpp"
#include "core/gettext.hpp"
#include "gfx/engine.hpp"
#include "groupbox.hpp"
#include "environment.hpp"
#include "city/city.hpp"
#include "core/foreach.hpp"
#include "core/utils.hpp"
#include "city/victoryconditions.hpp"
#include "core/logger.hpp"
#include "gameautopause.hpp"
#include "widgetescapecloser.hpp"
#include "widget_helper.hpp"

using namespace gfx;

namespace gui
{

namespace dialog
{

class MissionTargets::Impl
{
public:
  GameAutoPause locker;
  PlayerCityPtr city;
  Label* lbTitle;
  Label* subTitle;
  Label* lbPopulation;
  Label* lbProsperity;
  Label* lbFavour;
  Label* lbCulture;
  Label* lbPeace;
  Label* lbShortDesc;

  ListBox* lbxHelp;
};

MissionTargets* MissionTargets::create(Widget* parent, PlayerCityPtr city, int id )
{
  MissionTargets* ret = new MissionTargets( parent, id, Rect( 0, 0, 610, 430 ) );
  ret->setCenter( parent->center() );
  ret->setCity( city );
  ret->setModal();

  return ret;
}

MissionTargets::~MissionTargets() {}

MissionTargets::MissionTargets( Widget* parent, int id, const Rect& rectangle )
  : Window( parent, rectangle, "", id ), _d( new Impl )
{
  Widget::setupUI( ":/gui/targets.gui" );
  _d->locker.activate();

  WidgetEscapeCloser::insertTo( this );

  INIT_WIDGET_FROM_UI( TexturedButton*, btnExit )
  CONNECT( btnExit, onClicked(), this, MissionTargets::deleteLater );

  GET_DWIDGET_FROM_UI( _d, lbTitle )
  GET_DWIDGET_FROM_UI( _d, lbPopulation )
  GET_DWIDGET_FROM_UI( _d, lbProsperity )
  GET_DWIDGET_FROM_UI( _d, lbFavour )
  GET_DWIDGET_FROM_UI( _d, lbCulture )
  GET_DWIDGET_FROM_UI( _d, lbPeace  )
  GET_DWIDGET_FROM_UI( _d, lbShortDesc )
  GET_DWIDGET_FROM_UI( _d, lbxHelp )
}

void MissionTargets::draw( gfx::Engine& painter )
{
  if( !visible() )
    return;

  Window::draw( painter );
}

void MissionTargets::setCity(PlayerCityPtr city)
{
  _d->city = city;
  const city::VictoryConditions& wint = _d->city->victoryConditions();

  if( _d->lbTitle ) _d->lbTitle->setText( _d->city->player()->name()  );

  std::string text;

  if( _d->lbProsperity )
  {
    text = utils::format( 0xff, "%s:%d", _("##senatepp_prsp_rating##"), wint.needProsperity() );
    _d->lbProsperity->setText( text );
    _d->lbProsperity->setVisible( wint.needProsperity() > 0 );
  }

  if( _d->lbPopulation )
  {
    text = utils::format( 0xff, "%s:%d", _("##mission_wnd_population##"), wint.needPopulation() );
    _d->lbPopulation->setText( text );
  }

  if( _d->lbFavour )
  {
    text = utils::format( 0xff, "%s:%d", _("##senatepp_favour_rating##"), wint.needFavour() );
    _d->lbFavour->setText( text );
    _d->lbFavour->setVisible( wint.needFavour() > 0 );
  }

  if( _d->lbCulture )
  {
    text = utils::format( 0xff, "%s:%d", _("##senatepp_clt_rating##"), wint.needCulture() );
    _d->lbCulture->setText( text );
    _d->lbCulture->setVisible( wint.needCulture() > 0 );
  }

  if( _d->lbPeace )
  {
    text = utils::format( 0xff, "%s:%d", _("##senatepp_peace_rating##"), wint.needPeace() );
    _d->lbPeace->setText( text );
    _d->lbPeace->setVisible( wint.needPeace() > 0 );
  }

  if( _d->lbxHelp )
  {
    _d->lbxHelp->setItemDefaultColor( ListBoxItem::simple, 0xffe0e0e0 );

    foreach( it, wint.overview() )
    {
      std::string text = *it;
      if( text.substr( 0, 5 ) == "@img=" )
      {
        Picture pic = Picture::load( text.substr( 5 ) );
        ListBoxItem& item = _d->lbxHelp->addItem( pic );
        item.setTextAlignment( align::center, align::upperLeft );
        int lineCount = pic.height() / _d->lbxHelp->itemHeight();
        StringArray lines;
        lines.resize( lineCount );
        _d->lbxHelp->addItems( lines );
      }
      else { _d->lbxHelp->fitText( _( text ) ); }
    }
  }

  if( _d->lbShortDesc )
  {
    _d->lbShortDesc->setText( _(wint.shortDesc()) );
    _d->lbShortDesc->setVisible( !wint.shortDesc().empty() );
  }
}

}//end namespace dialog

}//end namespace gui
