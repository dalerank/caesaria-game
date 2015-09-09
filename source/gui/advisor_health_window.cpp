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
#include "label.hpp"
#include "game/resourcegroup.hpp"
#include "core/utils.hpp"
#include "gfx/engine.hpp"
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

using namespace gfx;
using namespace city;

struct HealthcareInfo
{
  object::Type type;
  std::string building;
  std::string people;
  int buildingCount;
  int buildingWork;
  int peoplesServed;
  int needService;
};

static HealthcareInfo healthDescrs[] = {
  { object::baths, "##bath##", "##peoples##" },
  { object::barber, "##barber##", "##peoples##" },
  { object::hospital, "##hospital##", "##patients##" },
  { object::clinic, "##clinics##", "##peoples##" },
  { object::unknown, "", "" }
};

enum { idxBarber=1, idxDoctor=2, idxHospital=3, rowOffset=20, smallCityNormalhealthValue=85,
       minPopullation4healthCalc=100, smallCityPopulation=300 };

namespace gui
{

namespace advisorwnd
{

static HealthcareInfo findInfo( const object::Type service )
{
  for( int index=0; healthDescrs[index].type != object::unknown; index++ )
  {
    if( service == healthDescrs[index].type )
        return healthDescrs[index];
  }

  return HealthcareInfo();
}

class HealthInfoLabel : public Label
{
public:
  HealthInfoLabel( Widget* parent, const Rect& rect, const object::Type service,
                   const HealthcareInfo& info  )
    : Label( parent, rect )
  {
    _service = service;
    _info = info;

    setFont( Font::create( FONT_1 ) );
  }

  virtual void _updateTexture( gfx::Engine& painter )
  {
    Label::_updateTexture( painter );

    HealthcareInfo info = findInfo( _service );

    Picture& texture = _textPicture();
    Font rfont = font();
    std::string buildingStrT = utils::i2str( _info.buildingCount ) + " " + _(info.building);
    rfont.draw( texture, buildingStrT, 0, 0 );

    rfont.draw( texture, utils::i2str( _info.buildingWork ), 165, 0 );

    std::string peoplesStrT = _("##health_no_info##");
    if( _info.buildingCount > 0 )
    {
      peoplesStrT = utils::format( 0xff, "%d (%d) %s", _info.peoplesServed, _info.needService, _(info.people) );
    }

    rfont.draw( texture, peoplesStrT, 255, 0 );

    std::string coverageStrT;
    if( _info.buildingCount > 0 )
    {
      int coveragePrcnt = _info.needService > 0
                            ? math::percentage( _info.peoplesServed, _info.needService )
                            : 100;
      coverageStrT = utils::format( 0xff, "%d %%", coveragePrcnt );
    }
    rfont.draw( texture, coverageStrT, 455, 0 );
  }

private:
  object::Type _service;
  HealthcareInfo _info;
};

class Health::Impl
{
public:
  HealthInfoLabel* lbBathsInfo;
  HealthInfoLabel* lbBarbersInfo;
  HealthInfoLabel* lbDoctorInfo;
  HealthInfoLabel* lbHospitalInfo;
  Label* lbBlackframe;
  Label* lbAdvice;
  TexturedButton* btnHelp;

  HealthcareInfo getInfo(PlayerCityPtr city, const object::Type objectType );
  void updateAdvice( PlayerCityPtr city );
  void initUI(Health* parent , PlayerCityPtr c);
};

Health::Health(PlayerCityPtr city, Widget* parent, int id )
  : Window( parent, Rect( 0, 0, 640, 290 ), "", id ), _d( new Impl )
{
  setupUI( ":/gui/healthadv.gui" );
  setPosition( Point( (parent->width() - 640 )/2, parent->height() / 2 - 242 ) );

  GET_DWIDGET_FROM_UI( _d, lbAdvice )
  GET_DWIDGET_FROM_UI( _d, lbBlackframe )
  GET_DWIDGET_FROM_UI( _d, btnHelp )

  _d->initUI( this, city );
  _d->updateAdvice( city );

  CONNECT( _d->btnHelp, onClicked(), this, Health::_showHelp );
}

void Health::draw( gfx::Engine& painter )
{
  if( !visible() )
    return;

  Window::draw( painter );
}

void Health::_showHelp() { DictionaryWindow::show( this, "health_advisor" ); }

HealthcareInfo Health::Impl::getInfo(PlayerCityPtr city, const object::Type objectType)
{
  HealthcareInfo ret;

  ret.buildingWork = 0;
  ret.peoplesServed = 0;
  ret.buildingCount = 0;
  ret.needService = 0;

  HealthBuildingList srvBuildings = city->statistic().objects.find<HealthBuilding>( objectType );
  for( auto b : srvBuildings )
  {
    ret.buildingWork += b->numberWorkers() > 0 ? 1 : 0;
    ret.peoplesServed += b->patientsNumber();
    ret.buildingCount++;
  }

  HouseList houses = city->statistic().houses.find();
  Service::Type serviceType = ServiceHelper::fromObject( objectType );
  for( auto h : houses )
  {
    if( h->isHealthNeed( serviceType ) )
    ret.needService += h->habitants().count();
  }

  return ret;
}

void Health::Impl::updateAdvice(PlayerCityPtr c)
{
  if( !lbAdvice )
    return;

  HealthCarePtr hc = c->statistic().services.find<HealthCare>();

  StringArray outText;

  if( c->states().population < minPopullation4healthCalc )
  {
    outText << "##healthadv_not_need_health_service_now##";
  }
  else
  {
    if( c->states().population < smallCityPopulation )
    {
      if( hc.isValid() && hc->value() > smallCityNormalhealthValue )
      {
        outText << "##healthadv_noproblem_small_city##";
      }
    }
    else
    {
      HouseList houses = c->statistic().houses.find();

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

      if( hc.isValid() )
      {
        outText << hc->reason();
      }
    }
  }

  std::string text = outText.empty()
                        ? "##healthadv_unknown_reason##"
                        : outText.random();
  lbAdvice->setText( _(text) );
}

void Health::Impl::initUI(Health* parent, PlayerCityPtr c)
{
  Point startPoint = lbBlackframe->lefttop() + Point( 3, 3 );
  Size labelSize( lbBlackframe->width() - 6, 20 );

  HealthcareInfo info = getInfo( c, object::baths );
  lbBathsInfo = new HealthInfoLabel( parent, Rect( startPoint, labelSize ), object::baths, info );

  info = getInfo( c, object::barber );
  lbBarbersInfo = new HealthInfoLabel( parent, Rect( startPoint + Point( 0, rowOffset * idxBarber ), labelSize), object::barber,
                                       info );

  info = getInfo( c, object::clinic );
  lbDoctorInfo = new HealthInfoLabel( parent, Rect( startPoint + Point( 0, rowOffset * idxDoctor ), labelSize), object::clinic,
                                      info );

  info = getInfo( c, object::hospital );
  lbHospitalInfo = new HealthInfoLabel( parent, Rect( startPoint + Point( 0, rowOffset * idxHospital), labelSize), object::hospital,
                                        info );
}

}//end namespace advisor

}//end namespace gui
