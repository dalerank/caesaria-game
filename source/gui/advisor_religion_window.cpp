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

#include "advisor_religion_window.hpp"
#include "gfx/picture.hpp"
#include "gfx/decorator.hpp"
#include "core/gettext.hpp"
#include "pushbutton.hpp"
#include "label.hpp"
#include "game/resourcegroup.hpp"
#include "core/stringhelper.hpp"
#include "gfx/engine.hpp"
#include "core/gettext.hpp"
#include "objects/construction.hpp"
#include "game/enums.hpp"
#include "objects/house_level.hpp"
#include "city/helper.hpp"
#include "objects/house.hpp"
#include "texturedbutton.hpp"
#include "religion/pantheon.hpp"
#include "game/gamedate.hpp"
#include "objects/constants.hpp"

using namespace constants;
using namespace religion;
using namespace gfx;

namespace gui
{

class ReligionInfoLabel : public Label
{
public:
  ReligionInfoLabel( Widget* parent, const Rect& rect, DivinityPtr divinity,
                     int smallTempleCount, int bigTempleCount  )
    : Label( parent, rect )
  {
    _divinity = divinity;
    _smallTempleCount = smallTempleCount;
    _bigTempleCount = bigTempleCount;
    _mood = 0;

    setFont( Font::create( FONT_1_WHITE ) );
  }

  virtual void _updateTexture( gfx::Engine& painter )
  {
    Label::_updateTexture( painter );

    PictureRef& texture = _textPictureRef();
    Font rfont = font();

    if( _divinity.isValid() )
    {
      _lastFestival = _divinity->lastFestivalDate().monthsTo( GameDate::current() );

      rfont.draw( *texture, _divinity->name(), 0, 0 );
      Font fontBlack = Font::create( FONT_1 );
      fontBlack.draw( *texture, StringHelper::format( 0xff, "(%s)", _( _divinity->shortDescription() ) ), 80, 0 );
      rfont.draw( *texture, StringHelper::format( 0xff, "%d", _smallTempleCount ), 220, 0 );
      rfont.draw( *texture, StringHelper::format( 0xff, "%d", _bigTempleCount ), 280, 0 );
      rfont.draw( *texture, StringHelper::format( 0xff, "%d", _lastFestival ), 350, 0 );

      int xOffset = 400, k=0;
      Picture pic = Picture::load( ResourceGroup::panelBackground, 334 );
      for( k; k < _divinity->wrathPoints() / 15; k++ )
      {
        texture->draw( pic, Point( xOffset + k * 15, 0), false );
      }

      rfont.draw( *texture, _( _divinity->moodDescription() ), xOffset + k * 15, 0 );
    }
    else
    {
      rfont.draw( *texture, _("##oracles_in_city##"), 0, 0 );
      rfont.draw( *texture, StringHelper::format( 0xff, "%d", _smallTempleCount ), 220, 0 );
    }
  }

private:
  DivinityPtr _divinity;
  int _smallTempleCount;
  int _bigTempleCount;
  int _lastFestival;
  int _mood;
};

class AdvisorReligionWindow::Impl
{
public:
  PictureRef background;

  ReligionInfoLabel* lbCeresInfo;
  ReligionInfoLabel* lbNeptuneInfo;
  ReligionInfoLabel* lbMercuryInfo;
  ReligionInfoLabel* lbMarsInfo;
  ReligionInfoLabel* lbVenusInfo;
  ReligionInfoLabel* lbOracleInfo;
  Label* religionAdvice;
  TexturedButton* btnHelp;

  struct InfrastructureInfo
  {
    int smallTemplCount;
    int bigTempleCount;
  };

  InfrastructureInfo getInfo( PlayerCityPtr city, const TileOverlay::Type small, const TileOverlay::Type big )
  {
    city::Helper helper( city );

    InfrastructureInfo ret;

    ret.smallTemplCount = helper.find<ServiceBuilding>( small ).size();
    ret.bigTempleCount = helper.find<ServiceBuilding>( big ).size();

    return ret;
  }

  void updateReligionAdvice( PlayerCityPtr city );
};


AdvisorReligionWindow::AdvisorReligionWindow(PlayerCityPtr city, Widget* parent, int id )
: Widget( parent, id, Rect( 0, 0, 1, 1 ) ), _d( new Impl )
{
  setGeometry( Rect( Point( (parent->width() - 640 )/2, parent->height() / 2 - 242 ),
               Size( 640, 280 ) ) );

  Label* title = new Label( this, Rect( 60, 10, 60 + 280, 10 + 40) );
  title->setText( _("##religion_advisor##") );
  title->setFont( Font::create( FONT_5 ) );
  title->setTextAlignment( align::upperLeft, align::center );

  _d->background.reset( Picture::create( size() ) );

  //main _d->_d->background
  Decorator::draw( *_d->background, Rect( Point( 0, 0 ), size() ), Decorator::whiteFrame );
  //buttons _d->_d->background
  Decorator::draw( *_d->background, Rect( 35, 62, width() - 35, 62 + 130 ), Decorator::blackFrame );

  Picture icon = Picture::load( ResourceGroup::panelBackground, 264 );
  _d->background->draw( icon, Point( 11, 11 ) );

  Font font = Font::create( FONT_1 );
  font.draw( *_d->background, _("##temples##"), 268, 32, false );
  font.draw( *_d->background, _("##small##"), 240, 47, false );
  font.draw( *_d->background, _("##large##"), 297, 47, false );
  font.draw( *_d->background, _("##test_t##"), 370, 47, false );
  font.draw( *_d->background, _("##rladv_mood##"), 450, 47, false );

  Point startPoint( 42, 65 );
  Size labelSize( 550, 20 );
  Impl::InfrastructureInfo info = _d->getInfo( city, building::templeCeres, building::cathedralCeres );
  _d->lbCeresInfo = new ReligionInfoLabel( this, Rect( startPoint, labelSize ), rome::Pantheon::ceres(),
                                           info.smallTemplCount, info.bigTempleCount );

  info = _d->getInfo( city, building::templeNeptune, building::cathedralNeptune );
  _d->lbNeptuneInfo = new ReligionInfoLabel( this, Rect( startPoint + Point( 0, 20), labelSize), rome::Pantheon::neptune(),
                                             info.smallTemplCount, info.bigTempleCount );

  info = _d->getInfo( city, building::templeMercury, building::cathedralMercury );
  _d->lbMercuryInfo = new ReligionInfoLabel( this, Rect( startPoint + Point( 0, 40), labelSize), rome::Pantheon::mercury(),
                                             info.smallTemplCount, info.bigTempleCount );

  info = _d->getInfo( city, building::templeMars, building::cathedralMars );
  _d->lbMarsInfo = new ReligionInfoLabel( this, Rect( startPoint + Point( 0, 60), labelSize), rome::Pantheon::mars(),
                                          info.smallTemplCount, info.bigTempleCount );

  info = _d->getInfo( city, building::templeVenus, building::cathedralVenus );
  _d->lbVenusInfo = new ReligionInfoLabel( this, Rect( startPoint + Point( 0, 80), labelSize), rome::Pantheon::venus(),
                                           info.smallTemplCount, info.bigTempleCount );

  info = _d->getInfo( city, building::oracle, building::oracle );
  _d->lbOracleInfo = new ReligionInfoLabel( this, Rect( startPoint + Point( 0, 100), labelSize), DivinityPtr(),
                                            info.smallTemplCount, 0 );

  _d->religionAdvice = new Label( this, Rect( 40, height() - 80, width() - 40, height() - 10 ) );
  _d->updateReligionAdvice( city );

  _d->btnHelp = new TexturedButton( this, Point( 12, height() - 39), Size( 24 ), -1, ResourceMenu::helpInfBtnPicId );
}

void AdvisorReligionWindow::draw(gfx::Engine& painter )
{
  if( !visible() )
    return;

  painter.draw( *_d->background, screenLeft(), screenTop() );

  Widget::draw( painter );
}

void AdvisorReligionWindow::Impl::updateReligionAdvice(PlayerCityPtr city)
{
  StringArray advices;
  city::Helper helper( city );
  HouseList houses = helper.find<House>( building::house );

  bool needBasicReligion = false;
  foreach( it, houses )
  {
    const HouseSpecification& spec = (*it)->spec();
    int curLevel = spec.computeReligionLevel( *it );
    int needLevel = spec.minReligionLevel();

    switch( needLevel )
    {
    case 1:
      if( curLevel == 0 )
      {
        needBasicReligion = true;
      }
    break;
    }
  }


}

}//end namespace gui
