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
#include "city/statistic.hpp"
#include "objects/house.hpp"
#include "festival_planing_window.hpp"
#include "objects/house_spec.hpp"
#include "objects/entertainment.hpp"
#include "city/cityservice_festival.hpp"
#include "religion/romedivinity.hpp"
#include "game/gamedate.hpp"
#include "core/logger.hpp"
#include "objects/constants.hpp"
#include "objects/hippodrome.hpp"
#include "widget_helper.hpp"

using namespace city;
using namespace gfx;

struct EntertInfo
{
  object::Type type;
  std::string building;
  std::string people;
  int peoplesServed;
  int buildingCount;
  int buildingWork;
  int buildingShow;
  int coverity;
  int partlyWork;
};

static EntertInfo infos[] = {
                              { object::theater, "##theaters##", "##peoples##" },
                              { object::amphitheater, "##amphitheaters##", "##peoples##" },
                              { object::colloseum, "##colloseum##", "##peoples##" },
                              { object::hippodrome, "##hippodromes##",  "-" },
                              { object::unknown, "", "" }
                            };


enum { idxTheater=0, idxAmph=1, idxColosseum=2, idxHippodrome=3,
       rowOffset=20,
       maxFestivalDelay=32,
       badCoverage=50, normalCoverage=80, maxCoverage=100,
       maxServiceValue=100 };

enum { ofNumberInCity=10, ofWorkInCity=165, ofHaveShow=245, ofHowmuchServed=305, ofCoverity=445 };

namespace gui
{

namespace advisorwnd
{

static EntertInfo findInfo( const object::Type service )
{
  for( int index=0; infos[index].type != object::unknown; index++ )
  {
    if( service == infos[index].type )
        return infos[index];
  }

  return EntertInfo();
}

class EntertainmentInfoLabel : public Label
{
public:
  EntertainmentInfoLabel( Widget* parent, const Rect& rect,
                          const object::Type service, EntertInfo info  )
    : Label( parent, rect ),
      _service( service ),
      _info( info )
  {    
    setFont( Font::create( FONT_1_WHITE ) );
  }

  const EntertInfo& getInfo() const { return _info; }

  virtual void _updateTexture( gfx::Engine& painter )
  {
    Label::_updateTexture( painter );

    EntertInfo info = findInfo( _service );

    Picture& texture = _textPicture();
    Font rfont = font();
    rfont.draw( texture, utils::format( 0xff, "%d %s", _info.buildingCount, _(info.building)), ofNumberInCity, 0 );
    rfont.draw( texture, utils::i2str( _info.buildingWork ), ofWorkInCity, 0 );
    rfont.draw( texture, utils::i2str( _info.buildingShow ), ofHaveShow, 0 );
    rfont.draw( texture, utils::format( 0xff, "%d %s",_info.peoplesServed, _(info.people)), ofHowmuchServed, 0 );

    std::string coverityText = "none";
    if( _info.buildingCount > 0 )
      utils::format( 0xff, "%d %%", _info.coverity );

    rfont.draw( texture, coverityText, ofCoverity, 0 );
  }

private:
  object::Type _service;
  EntertInfo _info;
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
  int monthFromLastFestival;

public:
  EntertInfo getInfo(const object::Type service );
  void updateInfo();
  void updateFestivalInfo();
  void initUI(Entertainment* parent);
};


Entertainment::Entertainment(PlayerCityPtr city, Widget* parent, int id )
: Base( parent, city, id ), _d( new Impl )
{
  _d->city = city;
  FestivalPtr fest = city->statistic().services.find<Festival>();

  setupUI( ":/gui/entertainmentadv.gui" );
  _d->monthFromLastFestival = fest->lastFestival().monthsTo( game::Date::current() );

  GET_DWIDGET_FROM_UI( _d, lbBlackframe )
  GET_DWIDGET_FROM_UI( _d, lbTroubleInfo )
  GET_DWIDGET_FROM_UI( _d, btnHelp )
  GET_DWIDGET_FROM_UI( _d, btnNewFestival )
  GET_DWIDGET_FROM_UI( _d, lbMonthFromLastFestival )
  GET_DWIDGET_FROM_UI( _d, lbInfoAboutLastFestival )

  _d->initUI( this );
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
  dialog::FestivalPlanning* wnd = dialog::FestivalPlanning::create( this, _d->city, -1 );
  CONNECT( wnd, onFestivalAssign(), this, Entertainment::_assignFestival );
}

EntertInfo Entertainment::Impl::getInfo( const object::Type service)
{
  EntertInfo ret = findInfo( service );

  ret.buildingWork = 0;
  ret.peoplesServed = 0;
  ret.buildingShow = 0;
  ret.buildingCount = 0;
  ret.partlyWork = 0;

  EntertainmentBuildingList servBuildings = city->statistic().objects.find<EntertainmentBuilding>( service );
  for( auto b : servBuildings )
  {
    if( b->numberWorkers() > 0 )
    {
      ret.buildingWork++;
      ret.peoplesServed += b->maxVisitors() * b->numberWorkers() / b->maximumWorkers();
    }

    ret.buildingCount++;
    ret.partlyWork += (b->numberWorkers() != b->maximumWorkers() ? 1 : 0);
  }

  return ret;
}

void Entertainment::_assignFestival( int divinityType, int festSize)
{
  FestivalPtr fest = _d->city->statistic().services.find<Festival>();
  if( fest.isValid() )
  {
    fest->assign( (religion::RomeDivinityType)divinityType, festSize );
    _d->updateFestivalInfo();
  }
}

void Entertainment::Impl::updateInfo()
{ 
  StringArray troubles;
  if( !lbTroubleInfo )
    return;

  const EntertInfo& thInfo = lbTheatresInfo->getInfo();
  const EntertInfo& amthInfo = lbAmphitheatresInfo->getInfo();
  const EntertInfo& clsInfo = lbColisseumInfo->getInfo();
  //const InfrastructureInfo& hpdInfo = lbHippodromeInfo->getInfo();

  int theatersNeed = 0, amptNeed = 0, clsNeed = 0, hpdNeed = 0;
  int minTheaterSrvc = maxServiceValue;
  int theatersServed = 0, amptServed = 0, clsServed = 0, hpdServed = 0;
  int nextLevelMin = 0;
  int nextLevelAmph = 0;
  int nextLevelColloseum = 0;
  int maxHouseLevel = 0;

  HouseList houses = city->statistic().houses.find();
  for( auto house : houses )
  {
    maxHouseLevel = std::max<int>( maxHouseLevel, house->level() );
    int habitants = house->habitants().mature_n();

    const HouseSpecification& lspec = house->spec();

    if( house->isEntertainmentNeed( Service::theater ) )
    {
      if( habitants > 0 )
      {
        theatersNeed += habitants;
        theatersServed += house->hasServiceAccess( Service::theater );
        minTheaterSrvc = std::min<int>( house->getServiceValue( Service::theater ), minTheaterSrvc );
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
      case HouseSpecification::needTheater: nextLevelMin++; break;
      case HouseSpecification::needAmphitheater: nextLevelAmph++; break;
      case HouseSpecification::needColosseum: nextLevelColloseum++; break;
      }
    }
  }

  int allNeed = theatersNeed + amptNeed + clsNeed + hpdNeed;
  int allServed = theatersServed + amptServed + clsServed + hpdServed;

  int entertCoverage = math::percentage( allServed, allNeed);

  if( hpdNeed > hpdServed ) { troubles << "##citizens_here_are_bored_for_chariot_races##"; }

  if( entertCoverage > normalCoverage && entertCoverage <= maxCoverage )     { troubles << "##citizens_like_chariot_races##"; }
  else if( entertCoverage > badCoverage && entertCoverage <= normalCoverage ) { troubles << "##entertainment_50_80##"; }
  else if( allNeed > 0 && entertCoverage <= badCoverage )         { troubles << "##entertainment_less_50##"; }

  if( minTheaterSrvc < 30 )   { troubles << "##some_houses_inadequate_entertainment##"; }
  if( thInfo.partlyWork > 0 ) { troubles << "##some_theaters_need_actors##"; }
  if( thInfo.buildingCount == 0 ) { troubles << "##your_city_need_theaters##"; }
  if( amthInfo.partlyWork > 0){ troubles << "##some_amphitheaters_no_actors##"; }
  if( amthInfo.buildingCount == 0 ) { troubles << "##blood_sports_add_spice_to_life##"; }
  if( clsInfo.partlyWork > 0 ){ troubles << "##small_colloseum_show##"; }

  HippodromeList hippodromes = city->statistic().objects.find<Hippodrome>( object::hippodrome );
  for( auto h : hippodromes )
  {
    if( h->evaluateTrainee( walker::charioteer ) == 100 ) { troubles << "##no_chariots##"; }
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
  if( !lbInfoAboutLastFestival )
    return;

  FestivalPtr fest = city->statistic().services.find<Festival>();
  if( fest.isValid() )
  {    
    std::string text = utils::i2str( monthFromLastFestival ) + " " +  _("##month_from_last_festival##");

    if( lbMonthFromLastFestival ) { lbMonthFromLastFestival->setText( text ); }

    bool prepare2Festival = fest->nextFestival() >= game::Date::current();
    btnNewFestival->setText( prepare2Festival ? _("##prepare_to_festival##") : _("##new_festival##") );
    btnNewFestival->setEnabled( !prepare2Festival );

    int strIndex[maxFestivalDelay] = { 0, 4, 4, 4,
                                       4, 4, 4, 8,
                                       8, 8, 8, 12,
                                       12, 12, 12, 12,
                                       12, 16, 16, 16,
                                       16, 16, 16, 24,
                                       24, 24, 24, 24,
                                       24, 24, 31, 31 };

    int currentThinkIndex = math::clamp<int>( monthFromLastFestival, 0, maxFestivalDelay-1);
    text = utils::format( 0xff, "##more_%d_month_from_festival##", strIndex[ currentThinkIndex ] );
    lbInfoAboutLastFestival->setText( _( text ) );
    }
}

void Entertainment::Impl::initUI( Entertainment* parent )
{
  Point startPoint( 2, 2 );
  Size labelSize( 550, 20 );
  EntertInfo info;
  info = getInfo( object::theater );
  lbTheatresInfo = new EntertainmentInfoLabel( lbBlackframe, Rect( startPoint, labelSize ), object::theater, info );

  info = getInfo( object::amphitheater );
  lbAmphitheatresInfo = new EntertainmentInfoLabel( lbBlackframe, Rect( startPoint + Point( 0, rowOffset*idxAmph), labelSize), object::amphitheater,
                                                    info );
  info = getInfo( object::colloseum );
  lbColisseumInfo = new EntertainmentInfoLabel( lbBlackframe, Rect( startPoint + Point( 0, rowOffset*idxColosseum), labelSize), object::colloseum, info );

  info = getInfo( object::hippodrome );
  lbHippodromeInfo = new EntertainmentInfoLabel( lbBlackframe, Rect( startPoint + Point( 0, rowOffset*idxHippodrome), labelSize), object::hippodrome, info );

  CONNECT( btnNewFestival, onClicked(), parent, Entertainment::_showFestivalWindow );
}

}//end namespace advisor

}//end namespace gui
