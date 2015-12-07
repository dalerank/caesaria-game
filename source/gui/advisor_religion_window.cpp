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
#include "objects/house_spec.hpp"
#include "city/statistic.hpp"
#include "objects/house.hpp"
#include "dictionary.hpp"
#include "environment.hpp"
#include "texturedbutton.hpp"
#include "religion/pantheon.hpp"
#include "game/gamedate.hpp"
#include "objects/constants.hpp"
#include "widget_helper.hpp"
#include "core/logger.hpp"
#include "advisor_religion_info.hpp"

using namespace religion;
using namespace gfx;

namespace gui
{

namespace advisorwnd
{

class Religion::Impl
{
public:
  Label* lbReligionAdvice;
  Size labelSize = Size( 550, 20 );

  ReligionInfoLabel* addInfo( Widget* parent, PlayerCityPtr city,
                              const object::Type small, const object::Type big,
                              DivinityPtr divinity,
                              const Point& offset )
  {
    int small_n = city->statistic().objects.find<ServiceBuilding>( small ).size();
    int big_n = city->statistic().objects.find<ServiceBuilding>( big ).size();

    return new ReligionInfoLabel( parent, Rect( offset, labelSize ),
                                  divinity,
                                  small_n, big_n );
  }

  void updateReligionAdvice( PlayerCityPtr city );
};


Religion::Religion(PlayerCityPtr city, Widget* parent, int id )
: Window( parent, Rect( 0, 0, 640, 280 ), "", id ), __INIT_IMPL(Religion)
{
  __D_REF(d,Religion)
  setupUI( ":/gui/religionadv.gui" );
  setPosition( Point( (parent->width() - 640 )/2, parent->height() / 2 - 242 ) );

  Point startPoint( 42, 65 );

  d.addInfo( this, city, object::small_ceres_temple, object::big_ceres_temple, rome::Pantheon::ceres(), startPoint );
  d.addInfo( this, city, object::small_neptune_temple, object::big_neptune_temple, rome::Pantheon::neptune(), startPoint + Point( 0, 20) );
  d.addInfo( this, city, object::small_mercury_temple, object::big_mercury_temple, rome::Pantheon::mercury(), startPoint + Point( 0, 40) );
  d.addInfo( this, city, object::small_mars_temple, object::big_mars_temple, rome::Pantheon::mars(), startPoint + Point( 0, 60) );
  d.addInfo( this, city, object::small_venus_temple, object::big_venus_temple, rome::Pantheon::venus(), startPoint + Point( 0, 80) );
  d.addInfo( this, city, object::oracle, object::oracle, DivinityPtr(), startPoint + Point( 0, 100) );

  GET_DWIDGET_FROM_UI( &d, lbReligionAdvice )

  d.updateReligionAdvice( city );

  LINK_WIDGET_LOCAL_ACTION( TexturedButton*, btnHelp, onClicked(), Religion::_showHelp );
}

void Religion::draw(gfx::Engine& painter )
{
  if( !visible() )
    return;

  Window::draw( painter );
}

void Religion::_showHelp() { ui()->add<DictionaryWindow>( "religion_advisor" ); }

void Religion::Impl::updateReligionAdvice(PlayerCityPtr city)
{
  StringArray advices;
  HouseList houses = city->statistic().houses.find();

  int needBasicReligion = 0;
  int needSecondReligion = 0;
  int needThirdReligion = 0;
  for( auto house : houses )
  {
    const HouseSpecification& spec = house->spec();
    int curLevel = spec.computeReligionLevel( house );
    int needLevel = spec.minReligionLevel();

    switch( needLevel )
    {
    case 1: needBasicReligion += (curLevel == 0 ? 1 : 0); break;
    case 2: needSecondReligion += ( curLevel < 2 ? 1 : 0); break;
    case 3: needThirdReligion += ( curLevel < 3 ? 1 : 0); break;
    }
  }

  std::string text = "##religionadv_unknown_reason##";
  if( !needSecondReligion && !needThirdReligion && !needBasicReligion)
  {
    text = "##this_time_you_city_not_need_religion##";

    DivinityList gods = rome::Pantheon::instance().all();

    bool haveDispleasengGod = false;
    for( auto god : gods )
    {
      if( god->relation() < 75 )
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

}//end namespace advisorwnd

}//end namespace gui
