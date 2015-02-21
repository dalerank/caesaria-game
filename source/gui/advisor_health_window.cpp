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

#include "advisor_health_window.hpp"
#include "gfx/decorator.hpp"
#include "core/gettext.hpp"
#include "pushbutton.hpp"
#include "label.hpp"
#include "game/resourcegroup.hpp"
#include "core/utils.hpp"
#include "gfx/engine.hpp"
#include "core/gettext.hpp"
#include "game/enums.hpp"
#include "objects/construction.hpp"
#include "city/helper.hpp"
#include "core/foreach.hpp"
#include "objects/house.hpp"
#include "texturedbutton.hpp"
#include "objects/constants.hpp"
#include "objects/service.hpp"
#include "dictionary.hpp"
#include "city/cityservice_health.hpp"
#include "core/logger.hpp"
#include "widget_helper.hpp"

using namespace constants;
using namespace gfx;

namespace gui
{

namespace advisorwnd
{

class HealthInfoLabel : public Label
{
public:
  HealthInfoLabel( Widget* parent, const Rect& rect, const TileOverlay::Type service,
                   int workBulding, int numberBuilding, int peoplesCount  )
    : Label( parent, rect )
  {
    _service = service;
    _workingBuilding = workBulding;
    _numberBuilding = numberBuilding;
    _peoplesCount = peoplesCount;

    setFont( Font::create( FONT_1 ) );
  }

  virtual void _updateTexture( gfx::Engine& painter )
  {
    Label::_updateTexture( painter );

    std::string buildingStr, peoplesStr;
    switch( _service )
    {
    case objects::baths: buildingStr = _("##bath##"); peoplesStr = _("##peoples##"); break;
    case objects::barber: buildingStr = _("##barber##"); peoplesStr = _("##peoples##"); break;
    case objects::hospital: buildingStr = _("##hospital##"); peoplesStr = _("##patients##"); break;
    case objects::clinic: buildingStr = _("##clinics##"); peoplesStr = _("##peoples##"); break;
    default: break;
    }

    PictureRef& texture = _textPictureRef();
    Font rfont = font();
    std::string buildingStrT = utils::format( 0xff, "%d %s", _numberBuilding, buildingStr.c_str() );
    rfont.draw( *texture, buildingStrT, 0, 0 );

    std::string buildingWorkT = utils::format( 0xff, "%d", _workingBuilding );
    rfont.draw( *texture, buildingWorkT, 165, 0 );

    std::string peoplesStrT = utils::format( 0xff, "%d %s", _peoplesCount, peoplesStr.c_str() );
    rfont.draw( *texture, peoplesStrT, 255, 0 );
  }

private:
  TileOverlay::Type _service;
  int _workingBuilding;
  int _numberBuilding;
  int _peoplesCount;
};

class Health::Impl
{
public:
  HealthInfoLabel* lbBathsInfo;
  HealthInfoLabel* lbBarbersInfo;
  HealthInfoLabel* lbDoctorInfo;
  HealthInfoLabel* lbHospitalInfo;
  Label* lbAdvice;
  TexturedButton* btnHelp;

  struct InfrastructureInfo
  {
    int buildingCount;
    int buildingWork;
    int peoplesServed;
  };

  InfrastructureInfo getInfo( PlayerCityPtr city, const TileOverlay::Type service );
  void updateAdvice( PlayerCityPtr city );
};


Health::Health(PlayerCityPtr city, Widget* parent, int id )
: Window( parent, Rect( 0, 0, 640, 290 ), "", id ), _d( new Impl )
{
  setupUI( ":/gui/healthadv.gui" );
  setPosition( Point( (parent->width() - 640 )/2, parent->height() / 2 - 242 ) );

  GET_DWIDGET_FROM_UI( _d, lbAdvice )
  INIT_WIDGET_FROM_UI( Label*, lbBlackframe )

  Point startPoint = lbBlackframe->lefttop() + Point( 3, 3 );
  Size labelSize( lbBlackframe->width() - 6, 20 );

  Impl::InfrastructureInfo info = _d->getInfo( city, objects::baths );
  _d->lbBathsInfo = new HealthInfoLabel( this, Rect( startPoint, labelSize ), objects::baths,
                                             info.buildingWork, info.buildingCount, info.peoplesServed );

  info = _d->getInfo( city, objects::barber );
  _d->lbBarbersInfo = new HealthInfoLabel( this, Rect( startPoint + Point( 0, 20), labelSize), objects::barber,
                                              info.buildingWork, info.buildingCount, info.peoplesServed );

  info = _d->getInfo( city, objects::clinic );
  _d->lbDoctorInfo = new HealthInfoLabel( this, Rect( startPoint + Point( 0, 40), labelSize), objects::clinic,
                                          info.buildingWork, info.buildingCount, info.peoplesServed );

  info = _d->getInfo( city, objects::hospital );
  _d->lbDoctorInfo = new HealthInfoLabel( this, Rect( startPoint + Point( 0, 60), labelSize), objects::hospital,
                                          info.buildingWork, info.buildingCount, info.peoplesServed );

  _d->btnHelp = new TexturedButton( this, Point( 12, height() - 39), Size( 24 ), -1, ResourceMenu::helpInfBtnPicId );

  _d->updateAdvice( city );

  TexturedButton* btnHelp = new TexturedButton( this, Point( 12, height() - 39), Size( 24 ), -1, ResourceMenu::helpInfBtnPicId );
  CONNECT( btnHelp, onClicked(), this, Health::_showHelp );
}

void Health::draw( gfx::Engine& painter )
{
  if( !visible() )
    return;

  Window::draw( painter );
}

void Health::_showHelp()
{
  DictionaryWindow::show( this, "health_advisor" );
}

Health::Impl::InfrastructureInfo Health::Impl::getInfo(PlayerCityPtr city, const TileOverlay::Type service)
{
  city::Helper helper( city );

  InfrastructureInfo ret;

  ret.buildingWork = 0;
  ret.peoplesServed = 0;
  ret.buildingCount = 0;

  ServiceBuildingList srvBuildings = helper.find<ServiceBuilding>( service );
  foreach( b, srvBuildings )
  {
    ret.buildingWork += (*b)->numberWorkers() > 0 ? 1 : 0;
    ret.buildingCount++;
  }

  return ret;
}

void Health::Impl::updateAdvice(PlayerCityPtr c)
{
  if( !lbAdvice )
    return;

  city::HealthCarePtr hc;
  hc << c->findService( city::HealthCare::defaultName() );

  StringArray outText;

  if( c->population() < 100 )
  {
    outText << "##healthadv_not_need_health_service_now##";
  }
  else
  {
    if( c->population() < 300 )
    {
      if( hc.isValid() && hc->value() > 85 )
      {
        outText << "##healthadv_noproblem_small_city##";
      }
    }
    else
    {
      city::Helper helper( c );
      HouseList houses =  helper.find<House>( objects::house );

      unsigned int needBath = 0;
      unsigned int needBarbers = 0;
      unsigned int needDoctors = 0;
      unsigned int needHospital = 0;
      foreach( it, houses )
      {
        HousePtr house = *it;
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
      if( needHospital > 0 ) { outText << "##healthadv_some_regions_need_hospital##"; }
    }
  }

  std::string text = outText.empty()
                        ? "##healthadv_unknown_reason##"
                        : outText.random();
  lbAdvice->setText( _(text) );
}

}

}//end namespace gui
