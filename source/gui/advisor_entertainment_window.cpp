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

#include "advisor_entertainment_window.hpp"
#include "gfx/decorator.hpp"
#include "core/gettext.hpp"
#include "texturedbutton.hpp"
#include "label.hpp"
#include "game/resourcegroup.hpp"
#include "core/utils.hpp"
#include "objects/construction.hpp"
#include "gfx/engine.hpp"
#include "core/gettext.hpp"
#include "game/enums.hpp"
#include "city/helper.hpp"
#include "core/foreach.hpp"
#include "objects/house.hpp"
#include "festival_planing_window.hpp"
#include "objects/house_level.hpp"
#include "objects/entertainment.hpp"
#include "city/cityservice_festival.hpp"
#include "religion/romedivinity.hpp"
#include "game/gamedate.hpp"
#include "core/logger.hpp"
#include "objects/constants.hpp"
#include "objects/hippodrome.hpp"
#include "widget_helper.hpp"

using namespace constants;
using namespace gfx;

namespace gui
{

namespace advisorwnd
{

struct InfrastructureInfo
{
  int buildingCount;
  int partlyWork;
  int buildingWork;
  int buildingShow;
  int peoplesServed;
};

class EntertainmentInfoLabel : public Label
{
public:
  EntertainmentInfoLabel( Widget* parent, const Rect& rect,
                          const TileOverlay::Type service, InfrastructureInfo info  )
    : Label( parent, rect ),
      _service( service ),
      _info( info )
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
    case objects::theater: buildingStr = _("##theaters##"); peoplesStr = _("##peoples##"); break;
    case objects::amphitheater: buildingStr = _("##amphitheatres##"); peoplesStr = _("##peoples##"); break;
    case objects::colloseum: buildingStr = _("##colloseum##"); peoplesStr = _("##peoples##"); break;
    case objects::hippodrome: buildingStr = _("##hippodromes##"); peoplesStr = "-"; break;
    default:
    break;
    }

    PictureRef& texture = _textPictureRef();
    Font rfont = font();
    rfont.draw( *texture, utils::format( 0xff, "%d %s", _info.buildingCount, buildingStr.c_str() ), 0, 0 );
    rfont.draw( *texture, utils::format( 0xff, "%d", _info.buildingWork ), 165, 0 );
    rfont.draw( *texture, utils::format( 0xff, "%d", _info.buildingShow ), 245, 0 );
    rfont.draw( *texture, utils::format( 0xff, "%d %s", _info.peoplesServed, peoplesStr.c_str() ), 305, 0 );
  }

private:
  TileOverlay::Type _service;
  InfrastructureInfo _info;
};

class Entertainment::Impl
{
public:
  PlayerCityPtr city;

  EntertainmentInfoLabel* lbTheatresInfo;
  EntertainmentInfoLabel* lbAmphitheatresInfo;
  EntertainmentInfoLabel* lbColisseumInfo;
  EntertainmentInfoLabel* lbHippodromeInfo;
  Label* lbBlackframe;
  Label* lbTroubleInfo;
  PushButton* btnNewFestival;
  Label* lbInfoAboutLastFestival;
  TexturedButton* btnHelp;
  Label* lbMonthFromLastFestival;
  city::FestivalPtr srvc;
  int monthFromLastFestival;

  InfrastructureInfo getInfo(const TileOverlay::Type service );
  void updateInfo();
  void updateFestivalInfo();
};


Entertainment::Entertainment(PlayerCityPtr city, Widget* parent, int id )
: Window( parent, Rect( 0, 0, 1, 1 ), "", id ), _d( new Impl )
{
  _d->city = city;
  _d->srvc << city->findService( city::Festival::defaultName() );

  if( _d->srvc.isNull() )
  {
    Logger::warning( "WARNING!!!: city have no entertainment service" );
    return;
  }

  setupUI( ":/gui/entertainmentadv.gui" );

  setPosition( Point( (parent->width() - width() )/2, parent->height() / 2 - 242 ) );

  _d->monthFromLastFestival = _d->srvc->lastFestivalDate().monthsTo( game::Date::current() );

  GET_DWIDGET_FROM_UI( _d, lbBlackframe )
  GET_DWIDGET_FROM_UI( _d, lbTroubleInfo )
  GET_DWIDGET_FROM_UI( _d, btnHelp )
  GET_DWIDGET_FROM_UI( _d, btnNewFestival )
  GET_DWIDGET_FROM_UI( _d, lbMonthFromLastFestival )
  GET_DWIDGET_FROM_UI( _d, lbInfoAboutLastFestival )

  Point startPoint( 2, 2 );
  Size labelSize( 550, 20 );
  InfrastructureInfo info;
  info = _d->getInfo( objects::theater );
  _d->lbTheatresInfo = new EntertainmentInfoLabel( _d->lbBlackframe, Rect( startPoint, labelSize ), objects::theater, info );

  info = _d->getInfo( objects::amphitheater );
  _d->lbAmphitheatresInfo = new EntertainmentInfoLabel( _d->lbBlackframe, Rect( startPoint + Point( 0, 20), labelSize), objects::amphitheater,
                                                        info );
  info = _d->getInfo( objects::colloseum );
  _d->lbColisseumInfo = new EntertainmentInfoLabel( _d->lbBlackframe, Rect( startPoint + Point( 0, 40), labelSize), objects::colloseum, info );

  info = _d->getInfo( objects::hippodrome );
  _d->lbHippodromeInfo = new EntertainmentInfoLabel( _d->lbBlackframe, Rect( startPoint + Point( 0, 60), labelSize), objects::hippodrome, info );

  CONNECT( _d->btnNewFestival, onClicked(), this, Entertainment::_showFestivalWindow );

  _d->updateInfo();
  _d->updateFestivalInfo();
}

void Entertainment::draw( Engine& painter )
{
  if( !visible() )
    return;

  Window::draw( painter );
}

void Entertainment::_showFestivalWindow()
{
  dialog::FestivalPlaning* wnd = dialog::FestivalPlaning::create( this, _d->city, -1 );
  CONNECT( wnd, onFestivalAssign(), this, Entertainment::_assignFestival );
}

InfrastructureInfo Entertainment::Impl::getInfo( const TileOverlay::Type service)
{
  city::Helper helper( city );

  InfrastructureInfo ret;

  ret.buildingWork = 0;
  ret.peoplesServed = 0;
  ret.buildingShow = 0;
  ret.buildingCount = 0;
  ret.partlyWork = 0;

  ServiceBuildingList servBuildings = helper.find<ServiceBuilding>( service );
  foreach( b, servBuildings )
  {
    ServiceBuildingPtr building = *b;
    if( building->numberWorkers() > 0 )
    {
      ret.buildingWork++;

      int maxServing = 0;
      switch( service )
      {
      case objects::theater: maxServing = 500; break;
      case objects::amphitheater: maxServing = 800; break;
      case objects::colloseum: maxServing = 1500; break;
      default:
      break;
      }

      ret.peoplesServed += maxServing * building->numberWorkers() / building->maximumWorkers();
    }
    ret.buildingCount++;
    ret.partlyWork += (building->numberWorkers() != building->maximumWorkers() ? 1 : 0);
  }

  return ret;
}

void Entertainment::_assignFestival( int divinityType, int festSize)
{
  if( _d->srvc.isValid() )
  {
    _d->srvc->assignFestival( (religion::RomeDivinityType)divinityType, festSize );
    _d->updateFestivalInfo();
  }
}

void Entertainment::Impl::updateInfo()
{ 
  StringArray troubles;
  if( !lbTroubleInfo )
    return;

  const InfrastructureInfo& thInfo = lbTheatresInfo->getInfo();
  const InfrastructureInfo& amthInfo = lbAmphitheatresInfo->getInfo();
  const InfrastructureInfo& clsInfo = lbColisseumInfo->getInfo();
  //const InfrastructureInfo& hpdInfo = lbHippodromeInfo->getInfo();

  city::Helper helper( city );
  int theatersNeed = 0, amptNeed = 0, clsNeed = 0, hpdNeed = 0;
  int minTheaterSrvc = 100;
  int theatersServed = 0, amptServed = 0, clsServed = 0, hpdServed = 0;
  int nextLevelMin = 0;
  int nextLevelAmph = 0;
  int nextLevelColloseum = 0;
  int maxHouseLevel = 0;

  HouseList houses = helper.find<House>( objects::house );
  foreach( it, houses )
  {
    HousePtr house = *it;

    maxHouseLevel = std::max<int>( maxHouseLevel, house->spec().level() );
    int habitants = house->habitants().count( CitizenGroup::mature );

    const HouseSpecification& lspec = house->spec();

    if( house->isEntertainmentNeed( Service::theater ) )
    {
      if( habitants > 0 )
      {
        theatersNeed += habitants;
        theatersServed += house->hasServiceAccess( Service::theater );
        minTheaterSrvc = std::min<int>( house->getServiceValue( Service::theater), minTheaterSrvc );
      }
    }

    if(house->isEntertainmentNeed( Service::amphitheater ))
    {
      amptNeed +=  habitants;
      amptServed += (house->hasServiceAccess( Service::amphitheater ) ? habitants : 0 );
    }

    if(house->isEntertainmentNeed( Service::colloseum ))
    {
      clsNeed += habitants;
      clsServed += (house->hasServiceAccess( Service::colloseum) ? habitants : 0);
    }

    if( house->isEntertainmentNeed( Service::hippodrome ) )
    {
      hpdNeed += habitants;
      hpdServed += (house->hasServiceAccess( Service::hippodrome) ? habitants : 0);
    }

    int needEntert = ((lspec.computeEntertainmentLevel( house ) - lspec.minEntertainmentLevel()) < 0 ? 1 : 0);

    if( needEntert )
    {
      switch( lspec.minEntertainmentLevel() )
      {
      case 1: nextLevelMin++; break;
      case 2: nextLevelAmph++; break;
      case 3: nextLevelColloseum++; break;
      }
    }
  }

  int allNeed = theatersNeed + amptNeed + clsNeed + hpdNeed;
  int allServed = theatersServed + amptServed + clsServed + hpdServed;

  int entertCoverage = math::percentage( allServed, allNeed);

  if( hpdNeed > hpdServed ) { troubles << "##citizens_here_are_bored_for_chariot_races##"; }

  if( entertCoverage > 80 && entertCoverage <= 100 )     { troubles << "##citizens_like_chariot_races##"; }
  else if( entertCoverage > 50 && entertCoverage <= 80 ) { troubles << "##entertainment_50_80##"; }
  else if( allNeed > 0 && entertCoverage <= 50 )         { troubles << "##entertainment_less_50##"; }

  if( minTheaterSrvc < 30 )   { troubles << "##some_houses_inadequate_entertainment##"; }
  if( thInfo.partlyWork > 0 ) { troubles << "##some_theaters_need_actors##"; }
  if( amthInfo.partlyWork > 0){ troubles << "##some_amphitheaters_no_actors##"; }
  if( amthInfo.buildingCount == 0 ) { troubles << "##blood_sports_add_spice_to_life##"; }
  if( clsInfo.partlyWork > 0 ){ troubles << "##small_colloseum_show##"; }

  HippodromeList hippodromes = helper.find<Hippodrome>( objects::hippodrome );
  foreach( h, hippodromes )
  {
    if( (*h)->evaluateTrainee( walker::charioteer ) == 100 ) { troubles << "##no_chariots##"; }
  }

  if( nextLevelMin > 0 )  { troubles << "##entertainment_need_for_upgrade##";  }
  if( nextLevelAmph > 0 ) { troubles << "##some_houses_need_amph_for_grow##"; }
  if( theatersNeed == 0 ) { troubles << "##entertainment_not_need##";  }

  if( troubles.empty() )
  {
    if( maxHouseLevel < HouseLevel::bigDomus ) { troubles << "##entadv_small_city_not_need_entert##"; }
    else if( maxHouseLevel < HouseLevel::mansion ) { troubles << "##small_city_not_need_entertainment##"; }
    else if( maxHouseLevel < HouseLevel::insula ) { troubles << "##etertadv_as_city_grow_you_need_more_entert##"; }

    if( thInfo.buildingCount > 0 ) { troubles << "##citizens_enjoy_drama_and_comedy##"; }

    troubles << "##entertainment_full##";
  }

  lbTroubleInfo->setText( _( troubles.random() ) );
}

void Entertainment::Impl::updateFestivalInfo()
{
  if( srvc.isValid() )
  {    
    std::string text = utils::format( 0xff, "%d %s", monthFromLastFestival, _("##month_from_last_festival##") );

    if( lbMonthFromLastFestival ) { lbMonthFromLastFestival->setText( text ); }

    bool prepare2Festival = srvc->nextFestivalDate() >= game::Date::current();
    btnNewFestival->setText( prepare2Festival ? _("##prepare_to_festival##") : _("##new_festival##") );
    btnNewFestival->setEnabled( !prepare2Festival );

    int strIndex[32] = { 0, 4, 4, 4,
                         4, 4, 4, 8,
                         8, 8, 8, 12,
                         12, 12, 12, 12,
                         12, 16, 16, 16,
                         16, 16, 16, 24,
                         24, 24, 24, 24,
                         24, 24, 31, 31 };

    text = utils::format( 0xff, "##more_%d_month_from_festival##", strIndex[ math::clamp( monthFromLastFestival, 0, 32) ] );
    if( lbInfoAboutLastFestival ) { lbInfoAboutLastFestival->setText( _( text ) ); }
  }
}

}

}//end namespace gui
