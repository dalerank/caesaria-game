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
#include "city/statistic.hpp"
#include "objects/house.hpp"
#include "dictionary.hpp"
#include "texturedbutton.hpp"
#include "core/foreach.hpp"
#include "objects/house_spec.hpp"
#include "objects/constants.hpp"
#include "objects/service.hpp"
#include "city/states.hpp"
#include "core/logger.hpp"
#include "widget_helper.hpp"

using namespace gfx;
using namespace city;

struct SrvcInfo
{
  object::Type type;
  std::string building;
  std::string people;
  Service::Type service;
  int maxStudy;
  CitizenGroup::Age age;
};

static SrvcInfo services[] = {
                               {object::school, "##schools##", "##children##", Service::school, 75, CitizenGroup::scholar},
                               {object::academy, "##colleges##", "##students##", Service::academy, 100, CitizenGroup::student},
                               {object::library, "##libraries##", "##peoples##", Service::library, 800, CitizenGroup::mature},
                               {object::unknown, "", "", Service::srvCount, 0, CitizenGroup::longliver }
                             };

enum { maxDescriptionNumber = 10, badAccessValue=30, middleCoverage=75,
       awesomeAccessValue=100, awesomeCoverage=100, fantasticCoverage=150 };

const char* coverageDescriptions[ maxDescriptionNumber ] = {
                                                             "##edu_poor##", "##edu_very_bad##",
                                                             "##edu_bad##", "##edu_not_bad##",
                                                             "##edu_simple##", "##edu_above_simple##",
                                                             "##edu_good##", "##edu_very_good##",
                                                             "##edu_pretty##", "##edu_awesome##"
                                                           };

namespace gui
{

namespace advisorwnd
{

namespace {
  struct HealthcareInfo
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

static SrvcInfo findInfo( const object::Type service )
{
  for( int index=0; services[index].type != object::unknown; index++ )
  {
    if( service == services[index].type )
        return services[index];
  }

  SrvcInfo ret;
  ret.service = Service::srvCount;
  return ret;
}

class EducationInfoLabel : public Label
{
public:
  EducationInfoLabel( Widget* parent, const Rect& rect, const object::Type service,
                      const HealthcareInfo& info )
    : Label( parent, rect ), _service( service ), _info( info )
  {
    setFont( Font::create( FONT_1_WHITE ) );
  }

  const HealthcareInfo& getInfo() const   {    return _info;  }

  virtual void _updateTexture( gfx::Engine& painter )
  {
    Label::_updateTexture( painter );

    SrvcInfo info = findInfo( _service );

    Picture& texture = _textPicture();
    Font rfont = font();
    std::string buildingStrT = utils::format( 0xff, "%d %s", _info.buildingCount, _(info.building) );
    rfont.draw( texture, buildingStrT, 0, 0 );

    std::string buildingWorkT = utils::i2str( _info.buildingWork );
    rfont.draw( texture, buildingWorkT, 165, 0 );

    std::string peoplesStrT = utils::format( 0xff, "%d %s", _info.peoplesStuding, _(info.people) );
    rfont.draw( texture, peoplesStrT, 255, 0 );

    const char* coverageStr = _info.coverage > 0
                                  ? coverageDescriptions[ math::clamp( _info.coverage / maxDescriptionNumber, 0, maxDescriptionNumber-1 ) ]
                                  : "##non_cvrg##";
    rfont.draw( texture, _( coverageStr ), 440, 0 );
  }

private:
  object::Type _service;
  HealthcareInfo _info;
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

public:
  std::string getTrouble( PlayerCityPtr city );
  HealthcareInfo getInfo( PlayerCityPtr city, const object::Type service );
  void initUI(Education* parent, PlayerCityPtr city);
  void updateCityInfo( PlayerCityPtr city );
};

void Education::Impl::initUI( Education* parent, PlayerCityPtr city )
{
  Point startPoint( 2, 2 );
  Size labelSize( 550, 20 );
  HealthcareInfo info;
  info = getInfo( city, object::school );
  lbSchoolInfo = new EducationInfoLabel( lbBlackframe, Rect( startPoint, labelSize ), object::school, info );

  info = getInfo( city, object::academy );
  lbCollegeInfo = new EducationInfoLabel( lbBlackframe, Rect( startPoint + Point( 0, 20), labelSize), object::academy, info );

  info = getInfo( city, object::library );
  lbLibraryInfo = new EducationInfoLabel( lbBlackframe, Rect( startPoint + Point( 0, 40), labelSize), object::library, info );

  TexturedButton* btnHelp = new TexturedButton( parent, Point( 12, parent->height() - 39), Size( 24 ), -1, ResourceMenu::helpInfBtnPicId );
  CONNECT( btnHelp, onClicked(), parent, Education::_showHelp );
}

void Education::Impl::updateCityInfo(PlayerCityPtr city)
{
  int sumScholars = 0;
  int sumStudents = 0;
  HouseList houses = statistic::getHouses( city );
  foreach( house, houses )
  {
    sumScholars += (*house)->habitants().scholar_n();
    sumStudents += (*house)->habitants().student_n();
  }

  std::string cityInfoStr = utils::format( 0xff, "%d %s, %d %s, %d %s",
                                                  city->states().population, _("##people##"),
                                                  sumScholars, _("##scholars##"), sumStudents, _("##students##") );
  if( lbCityInfo ) { lbCityInfo->setText( cityInfoStr ); }

  std::string advice = getTrouble( city );
  if( lbTroubleInfo ) { lbTroubleInfo->setText( _(advice) ); }
}

Education::Education(PlayerCityPtr city, Widget* parent, int id )
: Base( parent, city, id ),
  __INIT_IMPL(Education)
{
  setupUI( ":/gui/educationadv.gui" );
  setHeight( 256 );
  
  __D_IMPL(_d,Education)
  GET_DWIDGET_FROM_UI( _d, lbBlackframe )
  GET_DWIDGET_FROM_UI( _d, lbCityInfo )
  GET_DWIDGET_FROM_UI( _d, lbTroubleInfo )

  _d->initUI( this, city );
  _d->updateCityInfo( city );
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

HealthcareInfo Education::Impl::getInfo(PlayerCityPtr city, const object::Type bType)
{
  HealthcareInfo ret;

  ret.buildingWork = 0;
  ret.peoplesStuding = 0;
  ret.buildingCount = 0;
  ret.need = 0;
  ret.nextLevel = 0;
  ret.coverage = 0;

  ServiceBuildingList servBuildings = statistic::getObjects<ServiceBuilding>( city, bType );

  ret.buildingCount = servBuildings.size();
  SrvcInfo info = findInfo( bType );
  if( info.service == Service::srvCount )
  {
    Logger::warning( "AdvisorEducationWindow: unknown building type %d", bType );
  }

  foreach( it, servBuildings )
  {
    ServiceBuildingPtr serv = *it;
    if( serv->numberWorkers() > 0 )
    {
      ret.buildingWork++;
      ret.peoplesStuding += info.maxStudy * serv->numberWorkers() / serv->maximumWorkers();
    }
  }

  HouseList houses = statistic::getHouses( city );
  int minAccessLevel = awesomeAccessValue;
  foreach( it, houses )
  {
    HousePtr house = *it;
    int habitantsCount = house->habitants().count();
    if( habitantsCount > 0 )
    {
      ret.need += ( house->habitants().count( info.age ) * ( house->isEducationNeed( info.service ) ? 1 : 0 ) );
      ret.nextLevel += (house->spec().next().evaluateEducationNeed( house, info.service ) == awesomeAccessValue ? 1 : 0);
      minAccessLevel = std::min<int>( house->getServiceValue( info.service ), minAccessLevel );
    }
  }

  ret.coverage = ret.need == 0
                    ? awesomeAccessValue
                    : math::percentage( ret.peoplesStuding, ret.need );
  return ret;
}

std::string Education::Impl::getTrouble(PlayerCityPtr city)
{
  StringArray advices;
  const HealthcareInfo& schInfo = lbSchoolInfo->getInfo();
  const HealthcareInfo& clgInfo = lbCollegeInfo->getInfo();
  const HealthcareInfo& lbrInfo = lbLibraryInfo->getInfo();
  if( schInfo.need == 0 && clgInfo.need == 0 && lbrInfo.need == 0 )
  {
    return "##not_need_education##";
  }

  if( schInfo.nextLevel > 0 ) { advices << "##have_no_access_school_colege##"; }  
  if( lbrInfo.nextLevel > 0 ) { advices << "##have_no_access_to_library##"; }


  if( schInfo.minAccessLevel < badAccessValue || clgInfo.minAccessLevel < badAccessValue )
  {
    advices << "##edadv_need_better_access_school_or_colege##";
  }

  if( schInfo.coverage < middleCoverage && clgInfo.coverage < middleCoverage && lbrInfo.coverage < middleCoverage )
  {
    advices << "##need_more_access_to_lbr_school_colege##";
  }

  if( schInfo.coverage < middleCoverage ) { advices << "##need_more_school_colege##"; }
  else if( schInfo.coverage >= awesomeCoverage && schInfo.coverage < fantasticCoverage ) { advices << "##school_access_perfectly##"; }

  if( clgInfo.coverage >= awesomeCoverage && clgInfo.coverage < fantasticCoverage ) { advices << "##colege_access_perfectly##"; }

  if( lbrInfo.coverage < middleCoverage ) { advices << "##need_more_access_to_library##"; }
  else if( lbrInfo.coverage > awesomeCoverage && lbrInfo.coverage < fantasticCoverage ) { advices << "##library_access_perfectrly##"; }

  if( lbrInfo.minAccessLevel < badAccessValue ) { advices << "##some_houses_need_better_library_access##"; }
  if( lbrInfo.nextLevel > 0 && clgInfo.nextLevel > 0 )
  {
    advices << "##some_houses_need_library_or_colege_access##";
  }

  return advices.empty() ? "##education_awesome##" : advices.random();
}

} //end namespace advisor

} //end namespace gui
