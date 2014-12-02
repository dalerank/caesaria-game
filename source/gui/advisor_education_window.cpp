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

#include "advisor_education_window.hpp"
#include "gfx/decorator.hpp"
#include "core/gettext.hpp"
#include "pushbutton.hpp"
#include "label.hpp"
#include "objects/construction.hpp"
#include "game/resourcegroup.hpp"
#include "core/utils.hpp"
#include "gfx/engine.hpp"
#include "game/enums.hpp"
#include "city/helper.hpp"
#include "objects/house.hpp"
#include "dictionary.hpp"
#include "texturedbutton.hpp"
#include "core/foreach.hpp"
#include "objects/house_level.hpp"
#include "objects/constants.hpp"
#include "objects/service.hpp"
#include "core/logger.hpp"
#include "widget_helper.hpp"

using namespace constants;
using namespace gfx;

namespace gui
{

namespace advisorwnd
{

namespace {
  struct InfrastructureInfo
  {
    int buildingCount;
    int buildingWork;
    int peoplesStuding;
    int need;
    int nextLevel;
    int coverage;
    int minAccessLevel;
  };
}

class EducationInfoLabel : public Label
{
public:
  EducationInfoLabel( Widget* parent, const Rect& rect, const TileOverlay::Type service,
                      const InfrastructureInfo& info )
    : Label( parent, rect ), _service( service ), _info( info )
  {
    setFont( Font::create( FONT_1_WHITE ) );
  }

  const InfrastructureInfo& getInfo() const   {    return _info;  }

  virtual void _updateTexture( gfx::Engine& painter )
  {
    Label::_updateTexture( painter );

    std::string buildingStr, peoplesStr;
    switch( _service )
    {
    case objects::school: buildingStr = _("##schools##"); peoplesStr = _("##children##"); break;
    case objects::academy: buildingStr = _("##colleges##"); peoplesStr = _("##students##"); break;
    case objects::library: buildingStr = _("##libraries##"); peoplesStr = _("##peoples##"); break;
    default: break;
    }

    PictureRef& texture = _textPictureRef();
    Font rfont = font();
    std::string buildingStrT = utils::format( 0xff, "%d %s", _info.buildingCount, buildingStr.c_str() );
    rfont.draw( *texture, buildingStrT, 0, 0 );

    std::string buildingWorkT = utils::format( 0xff, "%d", _info.buildingWork );
    rfont.draw( *texture, buildingWorkT, 165, 0 );

    std::string peoplesStrT = utils::format( 0xff, "%d %s", _info.peoplesStuding, peoplesStr.c_str() );
    rfont.draw( *texture, peoplesStrT, 255, 0 );

    const char* coverages[10] = { "##edu_poor##", "##edu_very_bad##", "##edu_bad##", "##edu_not_bad##", "##edu_simple##",
                                  "##edu_above_simple##", "##edu_good##", "##edu_very_good##", "##edu_pretty##", "##edu_awesome##" };
    const char* coverageStr = _info.coverage > 0
                                  ? coverages[ math::clamp( _info.coverage / 10, 0, 9 ) ]
                                  : "##non_cvrg##";
    rfont.draw( *texture, _( coverageStr ), 440, 0 );
  }

private:
  TileOverlay::Type _service;
  InfrastructureInfo _info;
};

class Education::Impl
{
public:
  Label* lbCityInfo;
  Label* lbTroubleInfo;
  Label* lbBlackframe;

  EducationInfoLabel* lbSchoolInfo;
  EducationInfoLabel* lbCollegeInfo;
  EducationInfoLabel* lbLibraryInfo;

  InfrastructureInfo getInfo( PlayerCityPtr city, const TileOverlay::Type service );
  std::string getTrouble( PlayerCityPtr city );
};

Education::Education(PlayerCityPtr city, Widget* parent, int id )
: Window( parent, Rect( 0, 0, 640, 256 ), "", id ),
  __INIT_IMPL(Education)
{
  setupUI( ":/gui/educationadv.gui" );
  setPosition( Point( (parent->width() - 640 )/2, parent->height() / 2 - 242 ) );
  
  __D_IMPL(_d,Education)
  GET_DWIDGET_FROM_UI( _d, lbBlackframe )
  GET_DWIDGET_FROM_UI( _d, lbCityInfo )
  GET_DWIDGET_FROM_UI( _d, lbTroubleInfo )

  Point startPoint( 2, 2 );
  Size labelSize( 550, 20 );
  InfrastructureInfo info;
  info = _d->getInfo( city, objects::school );
  _d->lbSchoolInfo = new EducationInfoLabel( _d->lbBlackframe, Rect( startPoint, labelSize ), objects::school, info );

  info = _d->getInfo( city, objects::academy );
  _d->lbCollegeInfo = new EducationInfoLabel( _d->lbBlackframe, Rect( startPoint + Point( 0, 20), labelSize), objects::academy, info );

  info = _d->getInfo( city, objects::library );
  _d->lbLibraryInfo = new EducationInfoLabel( _d->lbBlackframe, Rect( startPoint + Point( 0, 40), labelSize), objects::library, info );

  city::Helper helper( city );

  int sumScholars = 0;
  int sumStudents = 0;
  HouseList houses = helper.find<House>( objects::house );
  foreach( house, houses )
  {
    sumScholars += (*house)->habitants().count( CitizenGroup::scholar );
    sumStudents += (*house)->habitants().count( CitizenGroup::student );
  }

  std::string cityInfoStr = utils::format( 0xff, "%d %s, %d %s, %d %s", city->population(), _("##people##"),
                                                  sumScholars, _("##scholars##"), sumStudents, _("##students##") );
  if( _d->lbCityInfo ) { _d->lbCityInfo->setText( cityInfoStr ); }

  std::string advice = _d->getTrouble( city );
  if( _d->lbTroubleInfo ) { _d->lbTroubleInfo->setText( _(advice) ); }

  TexturedButton* btnHelp = new TexturedButton( this, Point( 12, height() - 39), Size( 24 ), -1, ResourceMenu::helpInfBtnPicId );
  CONNECT( btnHelp, onClicked(), this, Education::_showHelp );
}

void Education::draw( gfx::Engine& painter )
{
  if( !visible() )
    return;

  Window::draw( painter );
}

void Education::_showHelp()
{
  DictionaryWindow::show( this, "education_advisor" );
}

InfrastructureInfo Education::Impl::getInfo(PlayerCityPtr city, const TileOverlay::Type bType)
{
  city::Helper helper( city );

  InfrastructureInfo ret;

  Service::Type service;

  ret.buildingWork = 0;
  ret.peoplesStuding = 0;
  ret.buildingCount = 0;
  ret.need = 0;
  ret.nextLevel = 0;
  ret.coverage = 0;

  ServiceBuildingList servBuildings = helper.find<ServiceBuilding>( bType );

  ret.buildingCount = servBuildings.size();
  int maxStuding = 0;
  CitizenGroup::Age age;
  switch( bType )
  {
  case objects::school:  service = Service::school;  maxStuding = 75;  age = CitizenGroup::scholar; break;
  case objects::academy: service = Service::academy; maxStuding = 100; age = CitizenGroup::student; break;
  case objects::library: service = Service::library; maxStuding = 800; age = CitizenGroup::mature;  break;
  default:
    age=CitizenGroup::newborn;
    service=Service::srvCount;
    Logger::warning( "AdvisorEducationWindow: unknown building type %d", bType );
  break;
  }

  foreach( it, servBuildings )
  {
    ServiceBuildingPtr serv = *it;
    if( serv->numberWorkers() > 0 )
    {
      ret.buildingWork++;
      ret.peoplesStuding += maxStuding * serv->numberWorkers() / serv->maximumWorkers();
    }
  }

  HouseList houses = helper.find<House>( objects::house );
  int minAccessLevel = 100;
  foreach( it, houses )
  {
    HousePtr house = *it;
    int habitantsCount = house->habitants().count();
    if( habitantsCount > 0 )
    {
      ret.need += ( house->habitants().count( age ) * ( house->isEducationNeed( service ) ? 1 : 0 ) );
      ret.nextLevel += (house->spec().next().evaluateEducationNeed( house, service ) == 100 ? 1 : 0);
      minAccessLevel = std::min<int>( house->getServiceValue( service ), minAccessLevel );
    }
  }

  ret.coverage = math::percentage( ret.peoplesStuding, ret.need );
  return ret;
}

std::string Education::Impl::getTrouble(PlayerCityPtr city)
{
  StringArray advices;
  const InfrastructureInfo& schInfo = lbSchoolInfo->getInfo();
  const InfrastructureInfo& clgInfo = lbCollegeInfo->getInfo();
  const InfrastructureInfo& lbrInfo = lbLibraryInfo->getInfo();
  if( schInfo.need == 0 && clgInfo.need == 0 && lbrInfo.need == 0 )
  {
    return "##not_need_education##";
  }

  if( schInfo.nextLevel > 0 ) { advices << "##have_no_access_school_colege##"; }  
  if( lbrInfo.nextLevel > 0 ) { advices << "##have_no_access_to_library##"; }


  if( schInfo.minAccessLevel < 30 || clgInfo.minAccessLevel < 30 )
  {
    advices << "##edadv_need_better_access_school_or_colege##";
  }

  if( schInfo.coverage < 75 && clgInfo.coverage < 75 && lbrInfo.coverage < 75 )
  {
    advices << "##need_more_access_to_lbr_school_colege##";
  }

  if( schInfo.coverage < 75 ) { advices << "##need_more_school_colege##"; }
  else if( schInfo.coverage >= 100 && schInfo.coverage < 150 ) { advices << "##school_access_perfectly##"; }

  if( clgInfo.coverage >= 100 && clgInfo.coverage < 150 ) { advices << "##colege_access_perfectly##"; }

  if( lbrInfo.coverage < 75 ) { advices << "##need_more_access_to_library##"; }
  else if( lbrInfo.coverage > 100 && lbrInfo.coverage < 150 ) { advices << "##library_access_perfectrly##"; }

  if( lbrInfo.minAccessLevel < 30 ) { advices << "##some_houses_need_better_library_access##"; }
  if( lbrInfo.nextLevel > 0 && clgInfo.nextLevel > 0 )
  {
    advices << "##some_houses_need_library_or_colege_access##";
  }

  return advices.empty() ? "##education_awesome##" : advices.random();
}

}

} //end namespace gui
