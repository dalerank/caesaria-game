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
#include "objects/house_spec.hpp"
#include "objects/constants.hpp"
#include "objects/service.hpp"
#include "city/states.hpp"
#include "core/logger.hpp"
#include "widget_helper.hpp"

using namespace gfx;
using namespace city;

struct EducationInfo
{
  std::string building;
  std::string people;
  Service::Type service;
  int maxStudy;
  CitizenGroup::Age age;
  int buildingCount;
  int buildingWork;
  int peoplesStuding;
  int need;
  int nextLevel;
  int coverage;
  int minAccessLevel;
};

static std::map<object::Type, EducationInfo> enterInfos =
  {
    {object::school,  {"##schools##", "##children##", Service::school, 75, CitizenGroup::scholar} },
    {object::academy, {"##colleges##", "##students##", Service::academy, 100, CitizenGroup::student} },
    {object::library, {"##libraries##", "##peoples##", Service::library, 800, CitizenGroup::mature} },
    {object::unknown, {"", "", Service::srvCount, 0, CitizenGroup::longliver} }
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

static EducationInfo findInfo( const object::Type objs )
{
  auto it = enterInfos.find( objs );
  if( it != enterInfos.end() )
    return it->second;

  return enterInfos[ object::unknown ];
}

class EducationInfoLabel : public Label
{
public:
  EducationInfoLabel( Widget* parent, const Rect& rect, const object::Type service,
                      const EducationInfo& info )
    : Label( parent, rect ), _service( service ), _info( info )
  {
    setFont( Font::create( FONT_1_WHITE ) );
    setID( (int)service );
  }

  const EducationInfo& getInfo() const   {    return _info;  }

  virtual void _updateTexture( gfx::Engine& painter )
  {
    Label::_updateTexture( painter );

    EducationInfo info = findInfo( _service );

    std::string strBuildings = fmt::format( "{} {}", _info.buildingCount, _(info.building) );
    std::string strPeoples = fmt::format( "{} {}", _info.peoplesStuding, _(info.people) );
    std::string strWorking = utils::i2str( _info.buildingWork );
    std::string coverageStr = _info.coverage > 0
                                  ? coverageDescriptions[ math::clamp( _info.coverage / maxDescriptionNumber, 0, maxDescriptionNumber-1 ) ]
                                  : "##non_cvrg##";

    canvasDraw( strBuildings, Point() );
    canvasDraw( strWorking, Point( 165, 0 ) );
    canvasDraw( strPeoples, Point( 255, 0 ) );
    canvasDraw( _( coverageStr ), Point( 440, 0 ) );
  }

private:
  object::Type _service;
  EducationInfo _info;
};

class Education::Impl
{
public:
  Label* lbCityInfo;
  Label* lbTroubleInfo;
  Label* lbBlackframe;
public:
  EducationInfo getInfo( const object::Type service );
  std::string getTrouble();
  EducationInfo getInfo( PlayerCityPtr city, const object::Type service );
  void initUI(Education* parent, PlayerCityPtr city);
  void updateCityInfo( PlayerCityPtr city );
};

void Education::Impl::initUI( Education* parent, PlayerCityPtr city )
{
  Point startPoint( 2, 2 );
  Point offset( 0, 20 );
  Size labelSize( 550, 20 );
  Rect rect( startPoint, labelSize );
  EducationInfo info;
  info = getInfo( city, object::school );
  lbBlackframe->add<EducationInfoLabel>( rect, object::school, info );

  info = getInfo( city, object::academy );
  lbBlackframe->add<EducationInfoLabel>( rect + offset , object::academy, info );

  info = getInfo( city, object::library );
  lbBlackframe->add<EducationInfoLabel>( rect + offset * 2, object::library, info );

  auto&& btnHelp = parent->add<TexturedButton>( Point( 12, parent->height() - 39), Size( 24 ), -1, config::id.menu.helpInf );
  CONNECT( &btnHelp, onClicked(), parent, Education::_showHelp );
}

void Education::Impl::updateCityInfo(PlayerCityPtr city)
{
  int sumScholars = 0;
  int sumStudents = 0;
  auto houses = city->statistic().houses.find();
  for( auto house : houses )
  {
    sumScholars += house->habitants().scholar_n();
    sumStudents += house->habitants().student_n();
  }

  std::string cityInfoStr = fmt::format( "{} {}, {} {}, {} {}",
                                         city->states().population, _("##people##"),
                                         sumScholars, _("##scholars##"),
                                         sumStudents, _("##students##") );
  if( lbCityInfo ) { lbCityInfo->setText( cityInfoStr ); }

  std::string trouble = getTrouble();
  if( lbTroubleInfo ) { lbTroubleInfo->setText( _(trouble) ); }
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

EducationInfo Education::Impl::getInfo(PlayerCityPtr city, const object::Type bType)
{
  EducationInfo ret = findInfo( bType );

  ret.buildingWork = 0;
  ret.peoplesStuding = 0;
  ret.buildingCount = 0;
  ret.need = 0;
  ret.nextLevel = 0;
  ret.coverage = 0;

  ServiceBuildingList servBuildings = city->statistic().objects.find<ServiceBuilding>( bType );

  ret.buildingCount = servBuildings.size();
  if( ret.service == Service::srvCount )
  {
    Logger::warning( "AdvisorEducationWindow: unknown building type {}", bType );
  }

  for( auto serv : servBuildings )
  {
    if( serv->numberWorkers() > 0 )
    {
      ret.buildingWork++;
      ret.peoplesStuding += ret.maxStudy * serv->numberWorkers() / serv->maximumWorkers();
    }
  }

  auto habitable = city->statistic().houses.habitable();
  int minAccessLevel = awesomeAccessValue;
  for( auto house : habitable )
  {
    ret.need += ( house->habitants().count( ret.age ) * ( house->isEducationNeed( ret.service ) ? 1 : 0 ) );
    ret.nextLevel += (house->spec().next().evaluateEducationNeed( house, ret.service ) == awesomeAccessValue ? 1 : 0);
    minAccessLevel = std::min<int>( house->getServiceValue( ret.service ), minAccessLevel );
  }

  ret.coverage = ret.need == 0
                    ? awesomeAccessValue
                    : math::percentage( ret.peoplesStuding, ret.need );
  return ret;
}

EducationInfo Education::Impl::getInfo(const object::Type service)
{
  auto row = lbBlackframe->findChild<EducationInfoLabel>( service );
  if( row )
    return row->getInfo();

  return EducationInfo();
}

std::string Education::Impl::getTrouble()
{
  StringArray advices;
  auto schoolInfo = getInfo( object::school );
  auto collegeInfo = getInfo( object::academy );
  auto libraryInfo = getInfo( object::library );
  if( schoolInfo.need == 0 && collegeInfo.need == 0 && libraryInfo.need == 0 )
  {
    return "##not_need_education##";
  }

  if( schoolInfo.nextLevel > 0 ) { advices << "##have_no_access_school_colege##"; }
  if( libraryInfo.nextLevel > 0 ) { advices << "##have_no_access_to_library##"; }


  if( schoolInfo.minAccessLevel < badAccessValue || collegeInfo.minAccessLevel < badAccessValue )
  {
    advices << "##edadv_need_better_access_school_or_colege##";
  }

  if( schoolInfo.coverage < middleCoverage && collegeInfo.coverage < middleCoverage && libraryInfo.coverage < middleCoverage )
  {
    advices << "##need_more_access_to_lbr_school_colege##";
  }

  if( schoolInfo.coverage < middleCoverage ) { advices << "##need_more_school_colege##"; }
  else if( schoolInfo.coverage >= awesomeCoverage && schoolInfo.coverage < fantasticCoverage ) { advices << "##school_access_perfectly##"; }

  if( collegeInfo.coverage >= awesomeCoverage && collegeInfo.coverage < fantasticCoverage ) { advices << "##colege_access_perfectly##"; }

  if( libraryInfo.coverage < middleCoverage ) { advices << "##need_more_access_to_library##"; }
  else if( libraryInfo.coverage > awesomeCoverage && libraryInfo.coverage < fantasticCoverage ) { advices << "##library_access_perfectrly##"; }

  if( libraryInfo.minAccessLevel < badAccessValue ) { advices << "##some_houses_need_better_library_access##"; }
  if( libraryInfo.nextLevel > 0 && collegeInfo.nextLevel > 0 )
  {
    advices << "##some_houses_need_library_or_colege_access##";
  }

  return advices.empty() ? "##education_awesome##" : advices.random();
}

} //end namespace advisor

} //end namespace gui
