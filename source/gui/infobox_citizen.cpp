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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#include <cstdio>

#include "infobox_citizen.hpp"
#include "label.hpp"
#include "walker/walker.hpp"
#include "walker/merchant.hpp"
#include "walker/constants.hpp"
#include "walker/helper.hpp"
#include "gfx/picture.hpp"
#include "core/gettext.hpp"
#include "good/productmap.hpp"
#include "walker/animals.hpp"
#include "events/playsound.hpp"
#include "gfx/decorator.hpp"
#include "walker/enemysoldier.hpp"
#include "good/helper.hpp"
#include "gfx/engine.hpp"
#include "walker/merchant_sea.hpp"
#include "core/logger.hpp"
#include "widget_helper.hpp"
#include "gfx/tilemap.hpp"
#include "gfx/tilemap_config.hpp"
#include "core/color_list.hpp"
#include "core/metric.hpp"
#include "events/movecamera.hpp"

using namespace events;

namespace gui
{

namespace infobox
{

namespace citizen
{

class CitizenScreenshot : public Label
{
public:
  CitizenScreenshot( Widget* parent, Rect rectangle, WalkerPtr wlk )
    : Label( parent, rectangle, "", false, Label::bgBlackFrame )
  {
    _wlkPicture = gfx::Picture( rectangle.size() - Size(6,6), 0, true );
    _wlkPicture.fill( ColorList::clear, Rect() );
    _walker = wlk;
  }

  virtual void _handleClick()
  {
    emit _onClickedSignal( _walker );
  }

  virtual void draw(gfx::Engine &painter)
  {
    if ( !visible() )
      return;

    Label::draw( painter );

    if( _wlkPicture.isValid() )
    {
      Rect clipRect = absoluteClippingRect();
      clipRect._lefttop += Point(3, 3);
      clipRect._bottomright -= Point(3, 3);
      painter.draw( _wlkPicture, absoluteRect().lefttop() + Point(3, 3), &clipRect );
      gfx::Pictures pics;
      _walker->getPictures( pics );
      painter.draw( pics, absoluteRect().lefttop() + Point(30, 30), &clipRect );
    }
  }

  WalkerPtr _walker;
  gfx::Picture _wlkPicture;
  Signal1<WalkerPtr> _onClickedSignal;
};

class AboutPeople::Impl
{
public:
  WalkerList walkers;
  Label* lbName;
  Label* lbType;
  Label* lbThinks;
  Label* lbCitizenPic;
  Label* lbCurrentAction;
  Label* lbBaseBuilding;
  PushButton* btnMove2base;
  PushButton* btnMove2dst;
  PlayerCityPtr city;

  std::vector<CitizenScreenshot*> screenshots;
  TilePos baseBuildingPos;
  TilePos destinationPos;
  WalkerPtr object;

public:
  void updateCurrentAction( const std::string& action, TilePos pos );
  void updateBaseBuilding(TilePos pos );
  void moveCamera2base();
  void moveCamera2dst();
};

AboutPeople::AboutPeople( Widget* parent, PlayerCityPtr city, const TilePos& pos )
  : Infobox( parent, Rect( 0, 0, 460, 350 ), Rect( 18, 40, 460 - 18, 350 - 120 ) ),
    _d( new Impl)
{
  _init( city, pos, ":/gui/infoboxcitizen.gui" );
}

AboutPeople::AboutPeople( Widget* parent, const Rect& window, const Rect& blackArea )
  : Infobox( parent, window, blackArea ), _d( new Impl)
{
}

void AboutPeople::_setWalker( WalkerPtr wlk )
{
  if( _d->object.isValid() )
  {
    _d->object->setFlag( Walker::showPath, false );
  }

  if( wlk.isNull() )
    return;

  _d->object = wlk;
  _d->lbName->setText( wlk->name() );

  _d->lbType->setText( _(wlk->info().prettyName()) );
  _d->lbCitizenPic->setBackgroundPicture( wlk->info().bigPicture() );

  std::string thinks = wlk->thoughts( Walker::thCurrent );
  _d->lbThinks->setText( _( thinks ) );

  if( !thinks.empty() )
  {
    std::string sound = thinks.substr( 2, thinks.size() - 4 );
    events::dispatch<PlaySound>( sound, 100 );
  }

  _updateTitle();
  _updateExtInfo();
  _updateNeighbors();

  _d->updateCurrentAction( wlk->thoughts( Walker::thAction ), wlk->places( Walker::plDestination ) );
  _d->updateBaseBuilding( wlk->places( Walker::plOrigin ) );
}

void AboutPeople::_updateNeighbors()
{
  if( _d->object.isNull() )
    return;

  for( auto s : _d->screenshots )
    s->deleteLater();

  _d->screenshots.clear();

  auto tiles = _d->city->tilemap().getNeighbors( _d->object->pos(), gfx::Tilemap::AllNeighbors);
  Rect lbRect( 25, 45, 25 + 52, 45 + 52 );
  Point lbOffset( 60, 0 );
  for( auto tile : tiles )
  {
    const auto& tileWalkers = _d->city->walkers( tile->pos() );
    if( !tileWalkers.empty() )
    {
      //mini screenshot from citizen pos need here
      auto& ctzScreenshot = add<CitizenScreenshot>( lbRect, tileWalkers.front() );
      ctzScreenshot.setTooltipText( _("##click_here_to_talk_person##") );
      _d->screenshots.push_back( &ctzScreenshot );
      lbRect += lbOffset;

      CONNECT( &ctzScreenshot, _onClickedSignal, this, AboutPeople::_setWalker );
    }
  }
}

void AboutPeople::_init( PlayerCityPtr city, const TilePos& pos, const std::string& model )
{
  _d->walkers = city->walkers( pos );
  _d->city = city;

  Widget::setupUI( model );

  _d->lbName = &add<Label>( Rect( 90, 108, width() - 30, 108 + 20) );
  _d->lbName->setFont( "FONT_2" );

  _d->lbType = &add<Label>( Rect( 90, 128, width() - 30, 128 + 20) );
  _d->lbType->setFont( "FONT_1" );

  _d->lbThinks = &add<Label>( Rect( 90, 148, width() - 30, height() - 140),
                              "##citizen_thoughts_will_be_placed_here##" );

  _d->lbThinks->setWordwrap( true );
  _d->lbCitizenPic = &add<Label>( Rect( 30, 112, 30 + 55, 112 + 80) );

  GET_DWIDGET_FROM_UI( _d, lbCurrentAction )
  GET_DWIDGET_FROM_UI( _d, lbBaseBuilding )
  GET_DWIDGET_FROM_UI( _d, btnMove2base )
  GET_DWIDGET_FROM_UI( _d, btnMove2dst )

  CONNECT( _d->btnMove2base, onClicked(), _d.data(), Impl::moveCamera2base )
  CONNECT( _d->btnMove2dst, onClicked(), _d.data(), Impl::moveCamera2dst )
}

void AboutPeople::_updateExtInfo(){}
Label *AboutPeople::_lbThinks(){ return _d->lbThinks; }

typedef Delegate2< WalkerPtr, bool& > Condition;

void AboutPeople::_updateTitle()
{
  if( _d->object.isNull() )
    return;

  std::vector<Condition> conditions{ makeDelegate( this, &AboutPeople::_checkEnemy ),
                                     makeDelegate( this, &AboutPeople::_checkUnvividly ),
                                     makeDelegate( this, &AboutPeople::_checkAnimal ),
                                     makeDelegate( this, &AboutPeople::_checkMerchant ),
                                     makeDelegate( this, &AboutPeople::_checkDefault ) };

  for( auto& condition : conditions )
  {
    bool found = false;
    condition( _d->object, found );
    if( found )
      break;
  }
}

void AboutPeople::_checkEnemy(WalkerPtr walker, bool& found)
{
  if( !walker.is<EnemySoldier>() )
    return;

  std::string title = world::toString( walker->nation() );
  title.insert( title.size()-2, "_soldier" );
  setTitle( _(title) );
  found = true;
}

void AboutPeople::_checkUnvividly(WalkerPtr walker, bool& found)
{
  if( walker->getFlag( Walker::vividly ) )
    return;

  setTitle( _("##object##") );
  found = true;
}

void AboutPeople::_checkAnimal(WalkerPtr walker, bool& found)
{
  if( !walker.is<Animal>() )
    return;

  setTitle( _("##animal##") );
  found = true;
}

void AboutPeople::_checkMerchant(WalkerPtr walker, bool& found)
{
  switch( walker->type() )
  {
  case walker::merchant:
  {
    auto landMerchant = walker.as<Merchant>();
    setTitle( _("##trade_caravan_from##") + std::string(" ") + landMerchant->parentCity() );
    found = true;
  }
  break;

  case walker::seaMerchant:
  {
    auto seaMerchant = walker.as<SeaMerchant>();
    setTitle( _("##trade_ship_from##") + std::string(" ") + seaMerchant->parentCity() );
    found = true;
  }
  break;

  default: break;
  }
}

void AboutPeople::_checkDefault(WalkerPtr walker, bool& found)
{
  setTitle( _("##citizen##") );
}

AboutPeople::~AboutPeople()
{
  if( _d->object.isValid() )
  {
    _d->object->setFlag( Walker::showPath, false );
  }
}

void AboutPeople::draw(gfx::Engine& engine)
{
  if( _d->object.isNull() )
  {
    _setWalker( _d->walkers.valueOrEmpty( 0 ) );
  }

  Infobox::draw( engine );
}

const WalkerList& AboutPeople::_walkers() const { return _d->walkers; }

void AboutPeople::Impl::updateCurrentAction(const std::string& action, TilePos pos)
{
  destinationPos = pos;
  OverlayPtr ov = city->getOverlay( pos );
  if( btnMove2dst )
    btnMove2dst->setVisible( ov.isValid() );

  if( lbCurrentAction )
  {
    std::string text = ov.isValid() ? ov->info().prettyName() : "";
    if( !action.empty() || !text.empty() )
    {
      lbCurrentAction->setPrefixText( _("##wlk_state##") );
      lbCurrentAction->setText( action + "(" + _(text) + ")" );
    }
  }
}

void AboutPeople::Impl::updateBaseBuilding( TilePos pos )
{
  baseBuildingPos = pos;
  OverlayPtr ov = city->getOverlay( pos );

  if( lbBaseBuilding )
    lbBaseBuilding->setText( ov.isValid() ? ov->info().prettyName() : "" );

  if( btnMove2base )
    btnMove2base->setVisible( ov.isValid() );
}

void AboutPeople::Impl::moveCamera2base()
{
  if( baseBuildingPos != TilePos::invalid() )
  {
    events::dispatch<MoveCamera>( baseBuildingPos );
  }
}

void AboutPeople::Impl::moveCamera2dst()
{
  if( destinationPos != TilePos::invalid() )
  {
    events::dispatch<MoveCamera>( destinationPos );
  }

  if( object.isValid() )
  {
    object->setFlag( Walker::showPath, true );
  }
}

}//end namespace citizen

}//end namespace infobox

}//end namespace gui
