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


#include <cstdio>

#include "infobox_citizen.hpp"
#include "label.hpp"
#include "walker/walker.hpp"
#include "walker/merchant.hpp"
#include "walker/constants.hpp"
#include "walker/helper.hpp"
#include "gfx/picture.hpp"
#include "city/helper.hpp"
#include "core/gettext.hpp"
#include "events/playsound.hpp"
#include "gfx/decorator.hpp"
#include "gfx/engine.hpp"
#include "core/logger.hpp"
#include "widget_helper.hpp"
#include "events/movecamera.hpp"

using namespace constants;

namespace gui
{

namespace infobox
{

namespace citizen
{

namespace {
static const char* sound_ext = ".ogg";
}

class CitizenScreenshot : public Label
{
public:
  CitizenScreenshot( Widget* parent, Rect rectangle, WalkerPtr wlk )
    : Label( parent, rectangle, "", false, Label::bgBlackFrame )
  {
    _wlkPicture.init( rectangle.size() - Size( 6 ) );
    _wlkPicture->fill( DefaultColors::green, Rect() );
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

    if( !_wlkPicture.isNull() )
    {
      painter.draw( *_wlkPicture, absoluteRect().lefttop() + Point( 3, 3 ), &absoluteClippingRectRef() );
      gfx::Pictures pics;
      _walker->getPictures( pics );
      painter.draw( pics, absoluteRect().lefttop() + Point( 30, 30 ), &absoluteClippingRectRef() );
    }
  }

  WalkerPtr _walker;
  gfx::PictureRef _wlkPicture;
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
  PlayerCityPtr city;

  std::vector<CitizenScreenshot*> screenshots;
  TilePos baseBuildingPos;

public:
  void updateCurrentAction( const std::string& action );
  void updateBaseBuilding(TilePos pos );
  void moveCamera2base();
};

AboutPeople::AboutPeople( Widget* parent, PlayerCityPtr city, const TilePos& pos )
  : Simple( parent, Rect( 0, 0, 460, 350 ), Rect( 18, 40, 460 - 18, 350 - 120 ) ),
    _d( new Impl)
{
  _d->walkers = city->walkers( pos );
  _d->city = city;

  Widget::setupUI( ":/gui/infoboxcitizen.gui" );

  new Label( this, Rect( 25, 100, width() - 25, height() - 130), "", false, Label::bgWhiteBorderA );
  new Label( this, Rect( 28, 103, width() - 28, height() - 133), "", false, Label::bgBlack );

  _d->lbName = new Label( this, Rect( 90, 108, width() - 30, 108 + 20) );
  _d->lbName->setFont( Font::create( FONT_2 ));
  _d->lbType = new Label( this, Rect( 90, 128, width() - 30, 128 + 20) );
  _d->lbType->setFont( Font::create( FONT_1 ));

  _d->lbThinks = new Label( this, Rect( 90, 148, width() - 30, height() - 140),
                            "##citizen_thoughts_will_be_placed_here##" );

  _d->lbThinks->setWordwrap( true );
  _d->lbCitizenPic = new Label( this, Rect( 30, 112, 30 + 55, 112 + 80) );

  GET_DWIDGET_FROM_UI( _d, lbCurrentAction )
  GET_DWIDGET_FROM_UI( _d, lbBaseBuilding )
  GET_DWIDGET_FROM_UI( _d, btnMove2base )

  CONNECT( _d->btnMove2base, onClicked(), _d.data(), Impl::moveCamera2base )

  if( !_d->walkers.empty() )
   _setWalker( _d->walkers.front() );
}

void AboutPeople::_setWalker( WalkerPtr wlk )
{
  if( wlk.isNull() )
    return;

  _d->lbName->setText( wlk->name() );

  std::string walkerType = WalkerHelper::getPrettyTypename( wlk->type() );
  _d->lbType->setText( _(walkerType) );
  _d->lbCitizenPic->setBackgroundPicture( WalkerHelper::getBigPicture( wlk->type() ) );

  std::string thinks = wlk->thoughts( Walker::thCurrent );
  _d->lbThinks->setText( _( thinks ) );

  if( !thinks.empty() )
  {
    std::string sound = thinks.substr( 2, thinks.size() - 4 );
    events::GameEventPtr e = events::PlaySound::create( sound + sound_ext, 100 );
    e->dispatch();
  }

  switch( wlk->type() )
  {
  case walker::merchant:
  {
    MerchantPtr m = ptr_cast<Merchant>( wlk );
    setTitle( _("##trade_caravan_from##") + m->parentCity() );
  }
  break;

  default: setTitle( _("##citizen##") );
  }

  foreach( it, _d->screenshots )
  {
    (*it)->deleteLater();
  }
  _d->screenshots.clear();

  gfx::TilesArray tiles = _d->city->tilemap().getNeighbors(wlk->pos(), gfx::Tilemap::AllNeighbors);
  Rect lbRect( 25, 45, 25 + 52, 45 + 52 );
  Point lbOffset( 60, 0 );
  foreach( itTile, tiles )
  {
    WalkerList tileWalkers = _d->city->walkers( (*itTile)->pos() );
    if( !tileWalkers.empty() )
    {
      //mini screenshot from citizen pos need here
      CitizenScreenshot* lb = new CitizenScreenshot( this, lbRect, tileWalkers.front() );
      _d->screenshots.push_back( lb );
      lbRect += lbOffset;

      CONNECT( lb, _onClickedSignal, this, AboutPeople::_setWalker );
    }
  }

  _d->updateCurrentAction( wlk->thoughts( Walker::thAction ) );
  _d->updateBaseBuilding( wlk->places( Walker::plDestination ) );
}

AboutPeople::~AboutPeople() {}

void AboutPeople::Impl::updateCurrentAction(const std::string& action)
{
  if( lbCurrentAction )
  {
    lbCurrentAction->setPrefixText( _("##wlk_state##") );
    lbCurrentAction->setText( action );
  }
}

void AboutPeople::Impl::updateBaseBuilding( TilePos pos )
{
  baseBuildingPos = pos;
  gfx::TileOverlayPtr ov = city->getOverlay( pos );
  if( ov.isValid() )
  {
    std::string text = MetaDataHolder::findPrettyName( ov->type() );
    if( lbBaseBuilding ) lbBaseBuilding->setText( text );
    if( btnMove2base ) btnMove2base->setVisible( !text.empty() );
  }
}

void AboutPeople::Impl::moveCamera2base()
{
  if( baseBuildingPos != TilePos( -1, -1 ) )
  {
    events::GameEventPtr e = events::MoveCamera::create( baseBuildingPos );
    e->dispatch();
  }
}

}

}

}//end namespace gui
