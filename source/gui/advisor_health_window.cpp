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

#include "advisor_health_window.hpp"
#include "gfx/decorator.hpp"
#include "core/gettext.hpp"
#include "pushbutton.hpp"
#include "events/showadvisorwindow.hpp"
#include "events/movecamera.hpp"
#include "label.hpp"
#include "game/resourcegroup.hpp"
#include "core/utils.hpp"
#include "widgetescapecloser.hpp"
#include "gfx/engine.hpp"
#include "environment.hpp"
#include "core/gettext.hpp"
#include "objects/construction.hpp"
#include "city/statistic.hpp"
#include "objects/house.hpp"
#include "texturedbutton.hpp"
#include "objects/constants.hpp"
#include "objects/health.hpp"
#include "dictionary.hpp"
#include "city/cityservice_health.hpp"
#include "core/logger.hpp"
#include "city/states.hpp"
#include "widget_helper.hpp"
#include "gfx/drawstate.hpp"

using namespace gfx;
using namespace city;

struct HealthcareInfo
{
  std::string building;
  std::string people;
  int buildingCount = 0;
  int buildingWork  = 0;
  int peoplesServed = 0;
  int needService   = 0;

  static const std::map<object::Type, HealthcareInfo> defaults;
  static const HealthcareInfo& find( const object::Type service )
  {
    auto it = defaults.find( service );
    if( it != defaults.end() )
      return it->second;

    static const HealthcareInfo invalid;
    return invalid;
  }

  HealthcareInfo() {}

  HealthcareInfo( const std::string& b, const std::string& p )
    : building(b), people(p)
  {}

  HealthcareInfo(PlayerCityPtr city, const object::Type objectType)
  {
    buildingWork = 0;
    peoplesServed = 0;
    buildingCount = 0;
    needService = 0;

    HealthBuildingList srvBuildings = city->statistic().objects.find<HealthBuilding>( objectType );
    for( auto b : srvBuildings )
    {
      buildingWork += b->numberWorkers() > 0 ? 1 : 0;
      peoplesServed += b->patientsCurrent();
      buildingCount++;
    }

    HouseList houses = city->statistic().houses.find();
    Service::Type serviceType = ServiceHelper::fromObject( objectType );
    for( auto h : houses )
    {
      if( h->isHealthNeed( serviceType ) )
        needService += h->habitants().count();
    }
  }
};

 const std::map<object::Type, HealthcareInfo> HealthcareInfo::defaults = {
    { object::baths,    {"##bath##",     "##peoples##"} },
    { object::barber,   {"##barber##",   "##peoples##"} },
    { object::hospital, {"##hospital##", "##patients##"} },
    { object::clinic,   {"##clinics##",  "##peoples##"} },
  };


enum { rowOffset=20, smallCityNormalhealthValue=85,
       minPopullation4healthCalc=100, smallCityPopulation=300 };

namespace gui
{

namespace advisorwnd
{

class HealthBldDetail : public PushButton
{
public:
  HealthBldDetail( Widget* parent, const Rect& rect, const object::Type service,
                   const HealthcareInfo& info  )
    : PushButton( parent, rect, "", -1, false, PushButton::noBackground )
  {
    _service = service;
    _info = info;

    setFont( FONT_1_WHITE );
    Decorator::draw( border, Rect( 0, 0, width(), height() ), Decorator::brownBorder );
  }

  virtual void _updateTexture()
  {
    PushButton::_updateTexture();

    HealthcareInfo info = HealthcareInfo::find( _service );

    std::string peoplesStrT = _("##health_no_info##");
    peoplesStrT = fmt::format( "{} ({}) {}", _info.peoplesServed, _info.needService, _(info.people) );

    std::string coverageStrT;
    int coveragePrcnt = _info.needService > 0
                          ? math::percentage( _info.peoplesServed, _info.needService )
                          : 100;
    math::clamp_to( coveragePrcnt, 0, 100 );
    coverageStrT = fmt::format( "{}%", coveragePrcnt );

    canvasDraw( fmt::format( "{} {}", _info.buildingCount, _(info.building )), Point() );
    canvasDraw( utils::i2str( _info.buildingWork ), Point( 165, 0 ) );
    canvasDraw( peoplesStrT, Point( 255, 0 ) );
    canvasDraw( coverageStrT, Point( 455, 0 ) );
  }

  virtual void draw(Engine &painter)
  {
    PushButton::draw( painter );

    DrawState pipe( painter, absoluteRect().lefttop(), &absoluteClippingRectRef() );
    if( _state() == stHovered )
     pipe.draw( border );
  }

  object::Type service() const { return _service; }

private:
  object::Type _service;
  HealthcareInfo _info;
  Pictures border;
};

Health::Health(PlayerCityPtr city, Widget* parent)
  : Base( parent, city, advisor::health, Rect( 0, 0, 640, 290 ) )
{
  setupUI( ":/gui/healthadv.gui" );
  setPosition( Point( (parent->width() - 640 )/2, parent->height() / 2 - 242 ) );

  _initUI();
  _updateAdvice();

  LINK_WIDGET_LOCAL_ACTION( PushButton*, btnHelp, onClicked(), Health::_showHelp );
}

void Health::draw( gfx::Engine& painter )
{
  if( !visible() )
    return;

  Window::draw( painter );
}

void Health::_showHelp() { ui()->add<DictionaryWindow>( "health_advisor" ); }

void Health::_updateAdvice()
{
  INIT_WIDGET_FROM_UI( Label*, lbAdvice )
  if( !lbAdvice )
    return;

  HealthCarePtr hc = _city->statistic().services.find<HealthCare>();
  if( !hc.isValid() )
  {
    Logger::warning( "WARNING !!! HealthCare service not exist" );
    return;
  }

  StringArray outText;

  if( _city->states().population < minPopullation4healthCalc )
  {
    outText << "##healthadv_not_need_health_service_now##";
  }
  else
  {
    if( _city->states().population < smallCityPopulation )
    {
      if( hc->value() > smallCityNormalhealthValue )
      {
        outText << "##healthadv_noproblem_small_city##";
      }
    }
    else
    {
      HouseList houses = _city->statistic().houses.find();

      unsigned int needBath = 0;
      unsigned int needBarbers = 0;
      unsigned int needDoctors = 0;
      unsigned int needHospital = 0;
      for( auto house : houses )
      {
        needBath += house->isHealthNeed( Service::baths ) ? 1 : 0;
        needDoctors += house->isHealthNeed( Service::doctor ) ? 1 : 0;
        needBarbers += house->isHealthNeed( Service::barber ) ? 1 : 0;
        needHospital += house->isHealthNeed( Service::hospital ) ? 1 : 0;
      }

      if( needBath > 0 )
      {
        outText << "##healthadv_some_regions_need_bath##";
        outText << "##healthadv_some_regions_need_bath_2##";
      }

      if( needDoctors > 0 )
      {          
        outText << "##healthadv_some_regions_need_doctors##";
        outText << "##healthadv_some_regions_need_doctors_2##";
      }

      if( needBarbers > 0 )
      {
        outText << "##healthadv_some_regions_need_barbers##";
        outText << "##healthadv_some_regions_need_barbers_2##";
      }

      if( needHospital > 0 )
      {
        outText << "##healthadv_some_regions_need_hospital##";
      }

      outText << hc->reason();
    }
  }

  std::string text = outText.empty()
                        ? "##healthadv_unknown_reason##"
                        : outText.random();
  lbAdvice->setText( _(text) );
}

void Health::_initUI()
{
  INIT_WIDGET_FROM_UI( Label*, lbBlackframe )
  if( !lbBlackframe )
  {
    Logger::warning( "WARNING !!! Can't initialize Health adwisor window" );
    return;
  }

  Point startPoint = Point( 3, 3 );
  Point offset( 0, rowOffset );
  Size labelSize( lbBlackframe->width() - 6, 20 );

  object::Types types{ object::baths, object::barber, object::clinic, object::hospital };

  for( auto type : types )
  {
    auto& btn = lbBlackframe->add<HealthBldDetail>( Rect( startPoint, labelSize ), type,
                                                    HealthcareInfo( _city, type ) );
    startPoint += offset;
    CONNECT_LOCAL( &btn, onClickedEx(), Health::_showDetailInfo )
  }
}

void Health::_showDetailInfo(Widget* widget)
{
  HealthBldDetail* detail = safety_cast<HealthBldDetail*>( widget );
  if( detail )
  {
    object::Type type = detail->service();
    const object::Info& info = object::Info::find( type );

    auto& window = parent()->add<Window>( Rect( 0, 0, 480, 480 ), _(info.prettyName()) );
    window.moveToCenter();
    WidgetClosers::insertTo( &window, KEY_RBUTTON );

    auto& frame = window.add<Label>( Rect( 20, 40, window.width() - 20, window.height() - 20 ), "",
                                     false, Label::bgBlackFrame );

    auto buildings = _city->statistic().objects.find<HealthBuilding>( type );
    int index = 0;
    Point offset( 2, 2 );
    Size size( frame.width()-6, 24 );
    for( auto b : buildings )
    {
      std::string text = fmt::format( "{} {} at [{},{}] {} workers, {} served",
                                      (b->info().prettyName()), index+1,
                                      b->pos().i(), b->pos().j(),
                                      b->numberWorkers(), b->patientsCurrent() );
      auto& btn = frame.add<PushButton>( Rect( offset, size ), text, -1, false, PushButton::whiteBorderUp );
      btn.addProperty( "pos", b->pos().hash() );
      btn.setFont( Font::create( FONT_1 ) ) ;
      offset = btn.leftbottom() + Point( 0, 2 );
      index++;
      CONNECT_LOCAL( &btn, onClickedEx(), Health::_moveCamera )
    }
  }
}

void Health::_moveCamera(Widget* widget)
{
  int hash = widget->getProperty( "pos" );
  TilePos pos( (hash >> 16)&0xff, hash&0xff );
  events::dispatch<events::MoveCamera>( pos );
  events::dispatch<events::ShowAdvisorWindow>( false, advisor::none );
}

}//end namespace advisor

}//end namespace gui
