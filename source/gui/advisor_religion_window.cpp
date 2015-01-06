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
#include "core/utils.hpp"
#include "gfx/engine.hpp"
#include "core/gettext.hpp"
#include "objects/construction.hpp"
#include "game/enums.hpp"
#include "objects/house_level.hpp"
#include "city/helper.hpp"
#include "objects/house.hpp"
#include "texturedbutton.hpp"
#include "dictionary.hpp"
#include "religion/pantheon.hpp"
#include "game/gamedate.hpp"
#include "objects/constants.hpp"
#include "widget_helper.hpp"
#include "core/logger.hpp"

using namespace constants;
using namespace religion;
using namespace gfx;

namespace gui
{

namespace advisorwnd
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
    _xWrathOffset = 400;

    setFont( Font::create( FONT_1_WHITE ) );
  }

  virtual void _updateTexture( gfx::Engine& painter )
  {
    Label::_updateTexture( painter );

    PictureRef& texture = _textPictureRef();
    Font rfont = font();

    if( _divinity.isValid() )
    {
      _lastFestival = _divinity->lastFestivalDate().monthsTo( game::Date::current() );

      rfont.draw( *texture, _divinity->name(), 0, 0 );
      Font fontBlack = Font::create( FONT_1 );
      fontBlack.draw( *texture, utils::format( 0xff, "(%s)", _( _divinity->shortDescription() ) ), 80, 0 );
      rfont.draw( *texture, utils::format( 0xff, "%d", _smallTempleCount ), 220, 0 );
      rfont.draw( *texture, utils::format( 0xff, "%d", _bigTempleCount ), 280, 0 );
      rfont.draw( *texture, utils::format( 0xff, "%d", _lastFestival ), 350, 0 );

      rfont.draw( *texture, _( _divinity->moodDescription() ), _xWrathOffset + _divinity->wrathPoints() / 15 * 15, 0 );
    }
    else
    {
      rfont.draw( *texture, _("##oracles_in_city##"), 0, 0 );
      rfont.draw( *texture, utils::format( 0xff, "%d", _smallTempleCount ), 220, 0 );
    }
  }

  virtual void draw(Engine &painter)
  {
    Label::draw( painter );

    Picture pic = Picture::load( ResourceGroup::panelBackground, 334 );
    if( _divinity.isValid() )
    {
      for( int k=0; k < _divinity->wrathPoints() / 15; k++ )
      {
        painter.draw( pic, absoluteRect().lefttop() + Point( _xWrathOffset + k * 15, 0), &absoluteClippingRectRef() );
      }
    }
  }

private:
  DivinityPtr _divinity;
  int _smallTempleCount;
  int _bigTempleCount;
  int _lastFestival;
  int _mood;
  int _xWrathOffset;
};

class Religion::Impl
{
public:
  ReligionInfoLabel* lbCeresInfo;
  ReligionInfoLabel* lbNeptuneInfo;
  ReligionInfoLabel* lbMercuryInfo;
  ReligionInfoLabel* lbMarsInfo;
  ReligionInfoLabel* lbVenusInfo;
  ReligionInfoLabel* lbOracleInfo;
  Label* lbReligionAdvice;
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


Religion::Religion(PlayerCityPtr city, Widget* parent, int id )
: Window( parent, Rect( 0, 0, 640, 280 ), "", id ), _d( new Impl )
{
  setupUI( ":/gui/religionadv.gui" );
  setPosition( Point( (parent->width() - 640 )/2, parent->height() / 2 - 242 ) );

  Point startPoint( 42, 65 );
  Size labelSize( 550, 20 );
  Impl::InfrastructureInfo info = _d->getInfo( city, objects::small_ceres_temple, objects::big_ceres_temple );
  _d->lbCeresInfo = new ReligionInfoLabel( this, Rect( startPoint, labelSize ), rome::Pantheon::ceres(),
                                           info.smallTemplCount, info.bigTempleCount );

  info = _d->getInfo( city, objects::small_neptune_temple, objects::big_neptune_temple );
  _d->lbNeptuneInfo = new ReligionInfoLabel( this, Rect( startPoint + Point( 0, 20), labelSize), rome::Pantheon::neptune(),
                                             info.smallTemplCount, info.bigTempleCount );

  info = _d->getInfo( city, objects::small_mercury_temple, objects::big_mercury_temple );
  _d->lbMercuryInfo = new ReligionInfoLabel( this, Rect( startPoint + Point( 0, 40), labelSize), rome::Pantheon::mercury(),
                                             info.smallTemplCount, info.bigTempleCount );

  info = _d->getInfo( city, objects::small_mars_temple, objects::big_mars_temple );
  _d->lbMarsInfo = new ReligionInfoLabel( this, Rect( startPoint + Point( 0, 60), labelSize), rome::Pantheon::mars(),
                                          info.smallTemplCount, info.bigTempleCount );

  info = _d->getInfo( city, objects::small_venus_temple, objects::big_venus_temple );
  _d->lbVenusInfo = new ReligionInfoLabel( this, Rect( startPoint + Point( 0, 80), labelSize), rome::Pantheon::venus(),
                                           info.smallTemplCount, info.bigTempleCount );

  info = _d->getInfo( city, objects::oracle, objects::oracle );
  _d->lbOracleInfo = new ReligionInfoLabel( this, Rect( startPoint + Point( 0, 100), labelSize), DivinityPtr(),
                                            info.smallTemplCount, 0 );

  GET_DWIDGET_FROM_UI( _d, lbReligionAdvice )
  GET_DWIDGET_FROM_UI( _d, btnHelp );

  _d->updateReligionAdvice( city );
  CONNECT( _d->btnHelp, onClicked(), this, Religion::_showHelp );
}

void Religion::draw(gfx::Engine& painter )
{
  if( !visible() )
    return;

  Window::draw( painter );
}

void Religion::_showHelp()
{
  DictionaryWindow::show( this, "religion_advisor" );
}

void Religion::Impl::updateReligionAdvice(PlayerCityPtr city)
{
  StringArray advices;
  city::Helper helper( city );
  HouseList houses = helper.find<House>( objects::house );

  int needBasicReligion = 0;
  int needSecondReligion = 0;
  int needThirdReligion = 0;
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
        needBasicReligion++;
      }
    break;

    case 2:
      if( curLevel < 2)
      {
        needSecondReligion++;
      }
    break;

    case 3:
      if( curLevel < 3 )
      {
        needThirdReligion++;
      }
    break;
    }
  }

  std::string text = "##religionadv_unknown_reason##";
  if( !needSecondReligion && !needThirdReligion && !needBasicReligion)
  {
    text = "##this_time_you_city_not_need_religion##";

    DivinityList gods = rome::Pantheon::instance().all();

    bool haveDispleasengGod = false;
    foreach( it, gods )
    {
      if( (*it)->relation() < 75 )
      {
        haveDispleasengGod = true;
        break;
      }
    }

    if( !haveDispleasengGod )
    {
      text = "##religion_in_your_city_is_flourishing##";
    }
  }
  else
  {
    if( needBasicReligion > 0 ) { advices << "##religionadv_need_basic_religion##";}
    if( needSecondReligion > 0 ){ advices << "##religionadv_need_second_religion##"; }
    if( needThirdReligion > 0 ) { advices << "##religionadv_need_third_religion##"; }

    if( rome::Pantheon::neptune()->relation() < 40 )
    {
      advices << "##neptune_despleasure_tip##";

      if( rome::Pantheon::neptune()->wrathPoints() > 0 )
      {
        advices << "##neptune_wrath_of_you##";
      }
    }

    if( rome::Pantheon::mars()->relation() < 40 )
    {
      advices << "##mars_watches_over_soldiers##";
    }

    text = advices.empty()
            ? "##religionadv_unknown_reason##"
            : advices.random();

  }

  lbReligionAdvice->setText( _(text) );
}

}

}//end namespace gui
