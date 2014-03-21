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
#include "core/stringhelper.hpp"
#include "gfx/engine.hpp"
#include "game/enums.hpp"
#include "city/helper.hpp"
#include "objects/house.hpp"
#include "core/foreach.hpp"
#include "game/settings.hpp"
#include "objects/house_level.hpp"
#include "objects/constants.hpp"
#include "objects/service.hpp"
#include "core/logger.hpp"

using namespace constants;
using namespace gfx;

namespace gui
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

  const InfrastructureInfo& getInfo() const
  {
    return _info;
  }

  virtual void _updateTexture( gfx::Engine& painter )
  {
    Label::_updateTexture( painter );

    std::string buildingStr, peoplesStr;
    switch( _service )
    {
    case building::school: buildingStr = _("##schools##"); peoplesStr = _("##children##"); break;
    case building::academy: buildingStr = _("##colleges##"); peoplesStr = _("##students##"); break;
    case building::library: buildingStr = _("##libraries##"); peoplesStr = _("##peoples##"); break;
    default: break;
    }

    PictureRef& texture = getTextPicture();
    Font font = getFont();
    std::string buildingStrT = StringHelper::format( 0xff, "%d %s", _info.buildingCount, buildingStr.c_str() );
    font.draw( *texture, buildingStrT, 0, 0 );

    std::string buildingWorkT = StringHelper::format( 0xff, "%d", _info.buildingWork );
    font.draw( *texture, buildingWorkT, 165, 0 );

    std::string peoplesStrT = StringHelper::format( 0xff, "%d %s", _info.peoplesStuding, peoplesStr.c_str() );
    font.draw( *texture, peoplesStrT, 255, 0 );

    const char* coverages[10] = { "##edu_poor##", "##edu_very_bad##", "##edu_bad##", "##edu_not_bad##", "##edu_simple##",
                                  "##edu_above_simple##", "##edu_good##", "##edu_very_good##", "##edu_pretty##", "##edu_awesome##" };
    const char* coverageStr = _info.coverage > 0
                                  ? coverages[ math::clamp( _info.coverage / 10, 0, 9 ) ]
                                  : "##non_cvrg##";
    font.draw( *texture, _( coverageStr ), 470, 0 );
  }

private:
  TileOverlay::Type _service;
  InfrastructureInfo _info;
};

class AdvisorEducationWindow::Impl
{
public:
  Label* lbCityInfo;
  Label* lbCityTrouble;
  Label* lbBackframe;

  EducationInfoLabel* lbSchoolInfo;
  EducationInfoLabel* lbCollegeInfo;
  EducationInfoLabel* lbLibraryInfo;

  InfrastructureInfo getInfo( PlayerCityPtr city, const TileOverlay::Type service );
  StringArray getTrouble( PlayerCityPtr city );
};

AdvisorEducationWindow::AdvisorEducationWindow(PlayerCityPtr city, Widget* parent, int id )
: Widget( parent, id, Rect( 0, 0, 1, 1 ) ),
  __INIT_IMPL(AdvisorEducationWindow)
{
  setGeometry( Rect( Point( (parent->width() - 640 )/2, parent->height() / 2 - 242 ),
               Size( 640, 256 ) ) );

  setupUI( GameSettings::rcpath( "/gui/educationadv.gui" ) );
  
  __D_IMPL(_d,AdvisorEducationWindow)
  _d->lbBackframe = findChildA<Label*>( "lbBlackframe", true, this );
  _d->lbCityInfo = findChildA<Label*>( "lbCityInfo", true, this );
  _d->lbCityTrouble = findChildA<Label*>( "lbCityTrouble", true, this);

  Point startPoint( 2, 2 );
  Size labelSize( 550, 20 );
  InfrastructureInfo info;
  info = _d->getInfo( city, building::school );
  _d->lbSchoolInfo = new EducationInfoLabel( _d->lbBackframe, Rect( startPoint, labelSize ), building::school, info );

  info = _d->getInfo( city, building::academy );
  _d->lbCollegeInfo = new EducationInfoLabel( _d->lbBackframe, Rect( startPoint + Point( 0, 20), labelSize), building::academy, info );

  info = _d->getInfo( city, building::library );
  _d->lbLibraryInfo = new EducationInfoLabel( _d->lbBackframe, Rect( startPoint + Point( 0, 40), labelSize), building::library, info );

  city::Helper helper( city );

  int sumScholars = 0;
  int sumStudents = 0;
  HouseList houses = helper.find<House>( building::house );
  foreach( house, houses )
  {
    sumScholars += (*house)->getHabitants().count( CitizenGroup::scholar );
    sumStudents += (*house)->getHabitants().count( CitizenGroup::student );
  }

  std::string cityInfoStr = StringHelper::format( 0xff, "%d %s, %d %s, %d %s", city->population(), _("##peoples##"),
                                                  sumScholars, _("##scholars##"), sumStudents, _("##students##") );
  if( _d->lbCityInfo ) { _d->lbCityInfo->setText( cityInfoStr ); }

  StringArray troubles = _d->getTrouble( city );
  if( _d->lbCityTrouble ) { _d->lbCityTrouble->setText( _( troubles[ rand() % troubles.size() ] ) ); }
}

void AdvisorEducationWindow::draw( gfx::Engine& painter )
{
  if( !isVisible() )
    return;

  Widget::draw( painter );
}

InfrastructureInfo AdvisorEducationWindow::Impl::getInfo(PlayerCityPtr city, const TileOverlay::Type bType)
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
  case building::school:  service = Service::school;  maxStuding = 75;  age = CitizenGroup::scholar; break;
  case building::academy: service = Service::academy; maxStuding = 100; age = CitizenGroup::student; break;
  case building::library: service = Service::library; maxStuding = 800; age = CitizenGroup::mature;  break;
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
      ret.peoplesStuding += maxStuding * serv->numberWorkers() / serv->maxWorkers();
    }
  }

  HouseList houses = helper.find<House>( building::house );
  foreach( it, houses )
  {
    HousePtr house = *it;
    ret.need += ( house->getHabitants().count( age ) * ( house->isEducationNeed( service ) ? 1 : 0 ) );
    ret.nextLevel += (house->getSpec().next().evaluateEducationNeed( house, service ) == 100 ? 1 : 0);
  }

  ret.coverage = ret.need > 0
                  ? ret.peoplesStuding * 100 / ret.need
                  : 0;

  return ret;
}

StringArray AdvisorEducationWindow::Impl::getTrouble(PlayerCityPtr city)
{
  StringArray ret;
  const InfrastructureInfo& schInfo = lbSchoolInfo->getInfo();
  const InfrastructureInfo& clgInfo = lbCollegeInfo->getInfo();
  const InfrastructureInfo& lbrInfo = lbLibraryInfo->getInfo();
  if( schInfo.need == 0 && clgInfo.need == 0 && lbrInfo.need == 0 )
  {
    ret.push_back( "##not_need_education##" );
    return ret;
  }

  if( schInfo.nextLevel > 0 ) { ret.push_back( "##have_no_access_school_colege##" ); }
  if( schInfo.coverage < 75 ) { ret.push_back( "##need_more_school_colege##" ); }
  if( lbrInfo.nextLevel > 0 ) { ret.push_back( "##have_no_access_to_library##" ); }
  if( lbrInfo.coverage < 75 ) { ret.push_back( "##need_more_access_to_library##"); }
  if( schInfo.coverage < 75 && clgInfo.coverage < 75 && lbrInfo.coverage < 75 )
  {
    ret.push_back( "##need_more_access_to_lbr_school_colege##" );
  }
  if( schInfo.coverage >= 100 && schInfo.coverage < 115 ) { ret.push_back( "##school_access_perfectly##"); }
  if( clgInfo.coverage >= 100 && clgInfo.coverage < 115 ) { ret.push_back( "##colege_access_perfectly##"); }
  if( clgInfo.coverage >= 100 && clgInfo.coverage < 115 ) { ret.push_back( "##academy_access_perfectly##"); }

  if( ret.empty() ) { ret.push_back( "##education_awesome##" ); }

  return ret;
}

} //end namespace gui
