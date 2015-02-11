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
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "house.hpp"
#include "gfx/helper.hpp"
#include "objects/house_level.hpp"
#include "core/utils.hpp"
#include "core/exception.hpp"
#include "walker/workerhunter.hpp"
#include "walker/immigrant.hpp"
#include "objects/market.hpp"
#include "objects/objects_factory.hpp"
#include "game/resourcegroup.hpp"
#include "core/variant_map.hpp"
#include "gfx/tilemap.hpp"
#include "game/gamedate.hpp"
#include "good/goodstore_simple.hpp"
#include "city/helper.hpp"
#include "core/foreach.hpp"
#include "constants.hpp"
#include "events/build.hpp"
#include "events/fireworkers.hpp"
#include "core/gettext.hpp"
#include "core/logger.hpp"
#include "city/funds.hpp"
#include "city/build_options.hpp"
#include "city/statistic.hpp"
#include "walker/patrician.hpp"
#include "objects_factory.hpp"

using namespace constants;
using namespace gfx;
using namespace events;
using namespace city;
REGISTER_CLASS_IN_OVERLAYFACTORY(objects::house, House)

namespace {
  enum { maxNegativeStep=-2, maxPositiveStep=2 };

  static int happines4tax[25] = { 10,  9,  7,  6,  4,
                                   2,  1,  0, -1, -2,
                                  -2, -3, -4, -5, -7,
                                  -9,-11,-13,-15, -17,
                                 -19,-21,-23,-27, -31 };
}

class House::Impl
{
public:
  typedef std::map< Service::Type, Service > Services;
  int houseLevel;
  float money, tax;
  int poverity;
  HouseSpecification spec;  // characteristics of the current house level
  Desirability desirability;
  good::SimpleStore goodStore;
  Services services;  // value=access to the service (0=no access, 100=good access)
  unsigned int maxHabitants;
  DateTime lastTaxationDate;
  std::string evolveInfo;
  CitizenGroup habitants;
  Animation healthAnimation;
  unsigned int taxesThisYear;
  bool isFlat;
  int currentYear;
  int changeCondition;

public:
  void updateHealthLevel( HousePtr house );
  void initGoodStore( int size );
  void consumeServices();
  void consumeGoods(HousePtr house);
  void consumeFoods(HousePtr house);
  int getFoodLevel() const;
};

House::House( HouseLevel::ID level ) : Building( objects::house ), _d( new Impl )
{
  HouseSpecHelper& helper = HouseSpecHelper::instance();
  _d->houseLevel = level;
  _d->spec = helper.getSpec( _d->houseLevel );
  _d->desirability.base = -3;
  _d->desirability.range = 3;
  _d->desirability.step = 1;
  _d->changeCondition = 0;
  _d->money = 0;
  _d->tax = 0;
  _d->taxesThisYear = 0;
  _d->currentYear = game::Date::current().year();

  setState( House::health, 100 );
  setState( House::fire, 0 );
  setState( House::happiness, 100 );

  _d->initGoodStore( 1 );

  // init the service access
  for( int i = 0; i<Service::srvCount; ++i )
  {
    // for every service type
    Service::Type service = Service::Type(i);
    _d->services[service] = Service();
  }

  _d->services[ Service::recruter ].setMax( 0 );
  _d->services[ Service::crime ] = 0;

  _update( true );
  //_animationRef()
}

void House::_makeOldHabitants()
{
  CitizenGroup newHabitants = _d->habitants;
  newHabitants.makeOld();

  unsigned int houseHealth = state( House::health );

  newHabitants[ CitizenGroup::longliver ] = 0; //death-health function from oldest habitants count
  unsigned int agedPeoples = newHabitants.count( CitizenGroup::aged );
  unsigned int peoples2remove = math::random( agedPeoples * ( 100 - houseHealth ) / 100 );
  newHabitants.retrieve( CitizenGroup::aged, peoples2remove+1 );

  unsigned int studentNumber = newHabitants.count( 10, 19 );
  unsigned int youngNumber = newHabitants.count( 20, 29);
  unsigned int matureNumber = newHabitants.count( 30, 39 );
  unsigned int oldNumber = newHabitants.count( 40, 49 );
  unsigned int newBorn = studentNumber * math::random( 3 ) / 100 + //at 3% of student add newborn
                         youngNumber * math::random( 16 ) / 100 + //at 16% of young people add newborn
                         matureNumber * math::random( 9 ) / 100 + //at 9% of matures add newborn
                         oldNumber * math::random( 2 ) / 100;     //at 2% of aged peoples add newborn

  newBorn = newBorn * houseHealth / 100 ;  //house health add compensation for newborn citizens

  unsigned int vacantRoom = maxHabitants() - newHabitants.count();
  newBorn = math::clamp( newBorn, 0u, vacantRoom );

  newHabitants[ CitizenGroup::newborn ] = newBorn; //birth+health function from mature habitants count

  _updateHabitants( newHabitants );
}

void House::_updateHabitants( const CitizenGroup& group )
{
  int deltaWorkersNumber = group.count( CitizenGroup::mature ) - _d->habitants.count( CitizenGroup::mature );

  _d->habitants = group;

  _d->services[ Service::recruter ].setMax( _d->habitants.count( CitizenGroup::mature ) );

  int firedWorkersNumber = _d->services[ Service::recruter ] + deltaWorkersNumber;
  _d->services[ Service::recruter ] += deltaWorkersNumber;

  if( firedWorkersNumber < 0 )
  {
    GameEventPtr e = FireWorkers::create( pos(), abs( firedWorkersNumber ) );
    e->dispatch();
  }
}

void House::_checkEvolve()
{
  bool validate = _d->spec.checkHouse( this, &_d->evolveInfo );
  if( !validate )
  {
    _d->changeCondition--;
    if( _d->changeCondition <= maxNegativeStep )
    {
      _d->changeCondition = 0;
      _levelDown();
    }
  }
  else
  {
    _d->evolveInfo = "";
    bool mayUpgrade =  _d->spec.next().checkHouse( this, &_d->evolveInfo );
    if( mayUpgrade )
    {
      _d->changeCondition++;
      if( _d->changeCondition >= maxPositiveStep )
      {
        _d->changeCondition = 0;
        _levelUp();
      }
    }
    else
    {
      _d->changeCondition = 0;
    }
  }

  if( _d->changeCondition < 0 )
  {
    std::string why;
    _d->spec.checkHouse( this, &why );
    if( !why.empty() )
    {
      why = why.substr( 0, why.size() - 2 );
      why += "_degrade##";
    }
    else
    {
      why = "##house_willbe_degrade##";
    }

    _d->evolveInfo = why;
  }
  else if( _d->changeCondition > 0 )
  {
    _d->evolveInfo = _("##house_evolves_at##");
  }
}

void House::_checkPatricianDeals()
{
  if( !spec().isPatrician() )
    return;

  TilesArray roads = getAccessRoads();
  if( !roads.empty() )
  {
    PatricianPtr patric = Patrician::create( _city() );
    patric->send2City( roads.front()->pos() );
  }
}

void House::_updateTax()
{
  float cityTax = _city()->funds().taxRate() / 100.f;
  cityTax = cityTax * _d->spec.taxRate() * _d->habitants.count( CitizenGroup::mature ) / (float)DateTime::monthsInYear;
  cityTax = math::clamp<float>( cityTax, 0, _d->money );

  _d->money -= cityTax;
  _d->tax += cityTax;
}

void House::_updateCrime()
{
  float cityKoeff = statistic::getBalanceKoeff( _city() );

  const int currentHabtn = habitants().count();

  if( currentHabtn == 0 )
    return;

  const Service& srvc = _d->services[ Service::recruter ];

  int unemploymentPrc = math::percentage( srvc.value(), srvc.max() );
  int unempInfluence4happiness = 0; ///!!!
  if( unemploymentPrc > 25 ) { unempInfluence4happiness = -3; }
  else if( unemploymentPrc > 17 ) { unempInfluence4happiness = -2; }
  else if( unemploymentPrc > 10 ) { unempInfluence4happiness = -1; }
  else if( unemploymentPrc < 5 ) { unempInfluence4happiness = 1; }

  int wagesInfluence4happiness = 0; ///!!!
  if( !spec().isPatrician() )
  {
    int diffWages = statistic::getWagesDiff( _city() );
    if( diffWages < 0)
    {
      wagesInfluence4happiness = diffWages;
    }
    else if( diffWages > 0 )
    {
      wagesInfluence4happiness = std::min( diffWages, 8 ) / 2;
    }
  }

  int taxValue = statistic::getTaxValue( _city() );
  int taxInfluence4happiness = happines4tax[ math::clamp( taxValue, 0, 25 ) ]; ///!!!
  if( spec().isPatrician() )
  {
    taxInfluence4happiness *= 1.5;
  }

  int foodAbundanceInfluence4happines = 0; ///!!!
  int foodStockInfluence4happines = 0; ///!!!
  int monthWithFood = 0;
  if( _d->spec.minFoodLevel() > 0 )
  {
    int foodStoreQty = 0;
    int foodTypeCount = 0;
    for( good::Product k=good::wheat; k <= good::vegetable; ++k )
    {
      int qty = _d->goodStore.qty( k );
      foodStoreQty += qty;
      foodTypeCount += (qty > 0 ? 1 : 0);
    }

    int diffFoodLevel = foodTypeCount - _d->spec.minFoodLevel();
    foodAbundanceInfluence4happines = diffFoodLevel * (diffFoodLevel < 0 ? 4 : 2);

    const unsigned int habtnConsumeGoodQty = currentHabtn / 2;
    monthWithFood = foodStoreQty / (habtnConsumeGoodQty+1);

    foodStockInfluence4happines = math::clamp<double>( -4 + 1.25 * monthWithFood, -4., 4. );
  }

  int poverity4happiness = _d->spec.level() > HouseLevel::hut ? -_d->poverity / 2 : 0;

  int curHappiness = 50
                  + unempInfluence4happiness
                  + wagesInfluence4happiness
                  + taxInfluence4happiness
                  + foodAbundanceInfluence4happines
                  + foodStockInfluence4happines
                  + poverity4happiness
                  + (int)state( happinessBuff );

  if( monthWithFood > 0 )
  {
    TilePos offset( 4, 4 );
    TilePos sizeOffset( size().width(), size().height() );
    TilesArray tiles = _city()->tilemap().getArea( pos() - offset, pos() + sizeOffset + offset );
    int averageDes = 0;
    foreach( it, tiles ) { averageDes += (*it)->param( Tile::pDesirability ); }
    averageDes /= (tiles.size() + 1);

    int desInfluence4happines = math::clamp( averageDes - spec().minDesirabilityLevel(), -10, 10 );
    if( averageDes < spec().minDesirabilityLevel() )
      desInfluence4happines -= 5;
    else if( averageDes > spec().maxDesirabilityLevel() )
      desInfluence4happines += 2;

    curHappiness += desInfluence4happines;
  }

  setState( House::happiness, curHappiness );

  int unhappyValue = 100 - curHappiness;
  int signChange = math::signnum( unhappyValue - getServiceValue( Service::crime) );

  appendServiceValue( Service::crime, _d->spec.crime() * signChange * cityKoeff );
}

void House::_checkHomeless()
{
  int homelessCount = math::clamp<int>( _d->habitants.count() - _d->maxHabitants, 0, 0xff );
  if( homelessCount > 0 )
  {
    homelessCount /= (homelessCount > 4 ? 2 : 1);
    CitizenGroup homeless = _d->habitants.retrieve( homelessCount );

    int workersFireCount = homeless.count( CitizenGroup::mature );
    if( workersFireCount > 0 )
    {
      GameEventPtr e = FireWorkers::create( pos(), workersFireCount );
      e->dispatch();
    }

    Emigrant::send2city( _city(), homeless, tile(), "##emigrant_no_home##" );
  }
}

void House::timeStep(const unsigned long time)
{
  if( _d->habitants.empty()  )
  {
    if( game::Date::isMonthChanged() )
    {
      _levelDown();
    }
    return; 
  }

  if( _d->currentYear != game::Date::current().year() )
  {
    _d->currentYear = game::Date::current().year();
    _makeOldHabitants();    
    _d->taxesThisYear = 0;
  }

  if( time % spec().getServiceConsumptionInterval() == 0 )
  {
    _d->consumeServices();
    _d->updateHealthLevel( this );
    cancelService( Service::recruter );
  }

  if( time % spec().foodConsumptionInterval() == 0 )
  {
    _d->consumeFoods( this );
  }

  if( time % spec().getGoodConsumptionInterval() == 0 )
  {
    _d->consumeGoods( this );
  }

  if( game::Date::isMonthChanged() )
  {
    setState( settleLock, 0 );
    _updateTax(); 

    if( _d->money > 0 ) { _d->poverity--; }
    else { _d->poverity += 2; }

    _d->poverity = math::clamp( _d->poverity, 0, 100 );
  }

  if( game::Date::isWeekChanged() )
  {
    _checkEvolve();
    _updateCrime();
    _checkHomeless();
    _checkPatricianDeals();
  }

  Building::timeStep( time );
}

bool House::_tryEvolve_1_to_12_lvl( int level4grow, int growSize, const char desirability )
{
  city::Helper helper( _city() );

  if( size().width() == 1 )
  {
    Tilemap& tmap = _city()->tilemap();
    TilesArray area = tmap.getArea( tile().pos(), Size(2) );
    bool mayGrow = true;

    foreach( it, area )
    {
      if( *it == NULL )
      {
        mayGrow = false;   //some broken, can't grow
        break;
      }

      HousePtr house = ptr_cast<House>( (*it)->overlay() );
      if( house != NULL &&
          (house->spec().level() == level4grow || house->habitants().count() == 0) )
      {
        if( house->size().width() > 1 )  //bigger house near, can't grow
        {
          mayGrow = false;
          break;
        }            
      }
      else
      {
        mayGrow = false; //no house near, can't grow
        break;
      }
    }

    if( mayGrow )
    {
      CitizenGroup sumHabitants = habitants();
      int sumFreeWorkers = getServiceValue( Service::recruter );
      TilesArray::iterator delIt=area.begin();
      HousePtr selfHouse = ptr_cast<House>( (*delIt)->overlay() );

      _d->initGoodStore( Size( growSize ).area() );

      ++delIt; //don't remove himself
      for( ; delIt != area.end(); ++delIt )
      {
        HousePtr house = ptr_cast<House>( (*delIt)->overlay() );
        if( house.isValid() )
        {          
          sumHabitants += house->habitants();

          house->deleteLater();
          house->_d->habitants.clear();

          sumFreeWorkers += house->getServiceValue( Service::recruter );

          house->_d->services[ Service::recruter ].setMax( 0 );

          selfHouse->goodStore().storeAll( house->goodStore() );
        }
      }

      _d->habitants = sumHabitants;
      setServiceValue( Service::recruter, sumFreeWorkers );

      //reset desirability level with old house size
      helper.updateDesirability( this, city::Helper::offDesirability );

      setSize( growSize  );
      //_update( false );

      CityAreaInfo info = { _city(), pos(), TilesArray() };
      build( info );
      //set new desirability level
      helper.updateDesirability( this, city::Helper::onDesirability );
    }
  }

  //that this house will be upgrade, we need decrease current desirability level
  helper.updateDesirability( this, city::Helper::offDesirability );

  _d->desirability.base = desirability;
  _d->desirability.step = desirability < 0 ? 1 : -1;
  //now upgrade groud area to new desirability
  helper.updateDesirability( this, city::Helper::onDesirability );

  return true;
}

bool House::_tryEvolve_12_to_20_lvl( int level4grow, int minSize, const char desirability )
{
  city::Helper helper( _city() );
  //startPic += math::random( 10 ) > 5 ? 1 : 0;
  bool mayGrow = true;
  TilePos buildPos = tile().pos();

  if( size() == minSize-1 )
  {
    Tilemap& tmap = _city()->tilemap();
    std::map<TilePos, TilesArray> possibleAreas;

    TilePos sPos = tile().pos();
    possibleAreas[ sPos ] = tmap.getArea( sPos, Size(minSize) );
    sPos = tile().pos() - TilePos( 1, 0 );
    possibleAreas[ sPos ] = tmap.getArea( sPos, Size(minSize) );
    sPos = tile().pos() - TilePos( 1, 1 );
    possibleAreas[ sPos ] = tmap.getArea( sPos, Size(minSize) );
    sPos = tile().pos() - TilePos( 0, 1 );
    possibleAreas[ sPos ] = tmap.getArea( sPos, Size(minSize) );

    foreach( itArea, possibleAreas )
    {
      TilesArray& area = itArea->second;

      for( TilesArray::iterator it=area.begin(); it != area.end(); )
      {
        if( (*it)->overlay() == this ) { it = area.erase( it ); }
        else { ++it; }
      }

      foreach( it, area )
      {
        if( *it == NULL )
        {
          mayGrow = false;   //some broken, can't grow
          break;
        }

        TileOverlayPtr overlay = (*it)->overlay();
        if( overlay.isNull() )
        {
          if( !(*it)->getFlag( Tile::isConstructible ) )
          {
            mayGrow = false; //not constuctible, can't grow
            break;
          }
        }
        else
        {
          if( overlay->type() != objects::garden )
          {
            mayGrow = false; //not garden, can't grow
            break;
          }
        }
      }

      if( mayGrow )
      {
        buildPos = itArea->first;
        helper.updateDesirability( this, city::Helper::offDesirability );
        setSize( minSize );
        _update( true );
        CityAreaInfo info = { _city(), buildPos, TilesArray() };
        build( info );

        _d->desirability.base = desirability;
        _d->desirability.step = desirability < 0 ? 1 : -1;

        helper.updateDesirability( this, city::Helper::onDesirability );
        break;
      }
    }
  }

  if( mayGrow )
  {
    //that this house will be upgrade, we need decrease current desirability level
    helper.updateDesirability( this, city::Helper::offDesirability );

    _d->desirability.base = desirability;
    _d->desirability.step = desirability < 0 ? 1 : -1;

    //now upgrade groud area to new desirability
    helper.updateDesirability( this, city::Helper::onDesirability );
    return true;
  }
  else
  {
    _d->evolveInfo = "##no_space_for_evolve##";
    return false;
  }
}


void House::_levelUp()
{
  if( _d->houseLevel >= HouseLevel::greatPalace )
    return;

  int nextLevel = math::clamp<int>( _d->houseLevel+1, HouseLevel::vacantLot, HouseLevel::greatPalace );
  bool mayUpgrade = false;

  switch( nextLevel )
  {
  case HouseLevel::hovel:
    _d->desirability.base = -3;
    _d->desirability.step = 1;
    mayUpgrade = true;
  break;

  case HouseLevel::tent:        mayUpgrade = _tryEvolve_1_to_12_lvl( HouseLevel::hovel, HouseLevel::maxSize2, -3); break;
  case HouseLevel::shack:       mayUpgrade = _tryEvolve_1_to_12_lvl( HouseLevel::tent, HouseLevel::maxSize2, -3 );   break;
  case HouseLevel::hut:         mayUpgrade = _tryEvolve_1_to_12_lvl( HouseLevel::shack, HouseLevel::maxSize2, -2 );  break;
  case HouseLevel::domus:       mayUpgrade = _tryEvolve_1_to_12_lvl( HouseLevel::hut, HouseLevel::maxSize2, -2 );  break;
  case HouseLevel::bigDomus:    mayUpgrade = _tryEvolve_1_to_12_lvl( HouseLevel::domus, HouseLevel::maxSize2, -2 ); break;
  case HouseLevel::mansion:     mayUpgrade = _tryEvolve_1_to_12_lvl( HouseLevel::bigDomus, HouseLevel::maxSize2, -2 );   break;
  case HouseLevel::bigMansion:  mayUpgrade = _tryEvolve_1_to_12_lvl( HouseLevel::mansion, HouseLevel::maxSize2, -1 );  break;
  case HouseLevel::insula:      mayUpgrade = _tryEvolve_1_to_12_lvl( HouseLevel::bigMansion, HouseLevel::maxSize2, -1 );   break;
  case HouseLevel::middleInsula:mayUpgrade = _tryEvolve_1_to_12_lvl( HouseLevel::insula, HouseLevel::maxSize2, 0 );   break;
  case HouseLevel::bigInsula:   mayUpgrade = _tryEvolve_12_to_20_lvl( HouseLevel::middleInsula, HouseLevel::maxSize2, 0 );  break;
  case HouseLevel::beatyfullInsula: mayUpgrade = _tryEvolve_12_to_20_lvl( HouseLevel::bigInsula, HouseLevel::maxSize2, 1 ); break;
  case HouseLevel::smallVilla:  mayUpgrade = _tryEvolve_12_to_20_lvl( HouseLevel::beatyfullInsula, HouseLevel::maxSize2, 2 ); break;
  case HouseLevel::middleVilla: mayUpgrade = _tryEvolve_12_to_20_lvl( HouseLevel::smallVilla, HouseLevel::maxSize2, 2 ); break;
  case HouseLevel::bigVilla:    mayUpgrade = _tryEvolve_12_to_20_lvl( HouseLevel::middleVilla, HouseLevel::maxSize3, 3 );  break;
  case HouseLevel::greatVilla:  mayUpgrade = _tryEvolve_12_to_20_lvl( HouseLevel::bigVilla, HouseLevel::maxSize3, 3 );  break;
  case HouseLevel::smallPalace: mayUpgrade = _tryEvolve_12_to_20_lvl( HouseLevel::greatVilla, HouseLevel::maxSize3, 4 );  break;
  case HouseLevel::middlePalace:mayUpgrade = _tryEvolve_12_to_20_lvl( HouseLevel::smallPalace, HouseLevel::maxSize3, 4 );  break;
  case HouseLevel::bigPalace:   mayUpgrade = _tryEvolve_12_to_20_lvl( HouseLevel::middlePalace, HouseLevel::maxSize4, 5 );  break;
  case HouseLevel::greatPalace: mayUpgrade = _tryEvolve_12_to_20_lvl( HouseLevel::bigPalace, HouseLevel::maxSize4, 6 );  break;
  }

  if( mayUpgrade )
  {
    _d->houseLevel = nextLevel;

    _d->spec = HouseSpecHelper::instance().getSpec(_d->houseLevel);

    if( _d->houseLevel == HouseLevel::smallVilla )
    {
      events::GameEventPtr e = events::FireWorkers::create( pos(), habitants().count( CitizenGroup::mature ) );
      e->dispatch();
    }

    _update( true );
  }
}

void House::_tryDegrage_12_to_2_lvl( const char desirability )
{
  city::Helper helper( _city() );
  //clear current desirability influence
  helper.updateDesirability( this, city::Helper::offDesirability );

  _d->desirability.base = desirability;
  _d->desirability.step = desirability < 0 ? 1 : -1;
  //set new desirability level
  helper.updateDesirability( this, city::Helper::onDesirability );
}

void House::_tryDegrade_20_to_12_lvl( int rsize, const char desirability )
{
  bool bigSize = size().width() > rsize;
  //_d->houseId = startPicId;
  //_d->picIdOffset = startPicId + ( math::random( 10 ) > 6 ? 1 : 0 );

  city::Helper helper( _city() );  

  //clear current desirability influence
  helper.updateDesirability( this, city::Helper::offDesirability );

  _d->desirability.base = desirability;
  _d->desirability.step = desirability < 0 ? 1 : -1;

  TilePos bpos = pos();
  if( bigSize )
  {
    TilesArray roads = getAccessRoads();
    TilePos moveVector = TilePos( 1, 1 );
    if( !roads.empty() )
    {
      TilePos roadPos = roads.front()->pos();
      moveVector = TilePos( math::signnum( roadPos.i() - bpos.i() ),
                            math::signnum( roadPos.j() - bpos.j() ) );
    }

    TilesArray lastArea = helper.getArea( this );
    foreach( tile, lastArea )
    {
      (*tile)->setMasterTile( 0 );
      (*tile)->setOverlay( 0 );
    }

    setSize( rsize );
    CityAreaInfo info = { _city(), bpos + moveVector, TilesArray() };
    build( info );
  }
  //set new desirability level
  helper.updateDesirability( this, city::Helper::onDesirability );
}

void House::_levelDown()
{
  if( _d->houseLevel <= HouseLevel::vacantLot )
    return;

  _d->houseLevel = math::clamp<int>( _d->houseLevel-1, HouseLevel::vacantLot, 0xff );

  if( _d->houseLevel == HouseLevel::beatyfullInsula )
  {
    int homelessCount = _d->habitants.count();
    while( homelessCount > 0 )
    {
      int currentPeople = math::clamp( math::random( homelessCount+1 ), 0, 8 );

      homelessCount -= currentPeople;
      CitizenGroup homeless = _d->habitants.retrieve( currentPeople );

      EmigrantPtr em = Emigrant::send2city( _city(), homeless, tile(), "##emigrant_no_home##" );

      if( em.isValid() )
      {
        em->leaveCity( tile() );
      }
    }

    _d->habitants.clear();
  }

  _d->spec = HouseSpecHelper::instance().getSpec(_d->houseLevel );

  switch (_d->houseLevel)
  {
  case HouseLevel::vacantLot:
  {
    city::Helper helper( _city() );
    helper.updateDesirability( this, city::Helper::offDesirability );
  }
  break;

  case HouseLevel::hovel:
  {
    Tilemap& tmap = _city()->tilemap();

    if( size().area() > 1 )
    {
      TilesArray perimetr = tmap.getArea( pos(), Size(2) );
      int peoplesPerHouse = habitants().count() / 4;
      foreach( tile, perimetr )
      {
        HousePtr house = ptr_cast<House>( TileOverlayFactory::instance().create( objects::house ) );
        house->_d->habitants = _d->habitants.retrieve( peoplesPerHouse );
        //house->_d->houseId = HouseLevel::smallHovel;
        //house->_update( true );

        GameEventPtr event = BuildAny::create( (*tile)->pos(), house.object() );
        event->dispatch();
      }

      _d->services[ Service::recruter ].setMax( 0 );
      deleteLater();
    }
  }
  break;

  case HouseLevel::tent: _tryDegrage_12_to_2_lvl( -3 ); break;
  case HouseLevel::shack: _tryDegrage_12_to_2_lvl( -3 ); break;
  case HouseLevel::hut: _tryDegrage_12_to_2_lvl( -2 ); break;
  case HouseLevel::domus: _tryDegrage_12_to_2_lvl( -2 ); break;
  case HouseLevel::bigDomus: _tryDegrage_12_to_2_lvl( -2 ); break;
  case HouseLevel::mansion: _tryDegrage_12_to_2_lvl( -2 );  break;
  case HouseLevel::bigMansion: _tryDegrage_12_to_2_lvl( -1 );  break;
  case HouseLevel::insula: _tryDegrage_12_to_2_lvl( -1 );  break;
  case HouseLevel::middleInsula: _tryDegrage_12_to_2_lvl( 0 );  break;
  case HouseLevel::bigInsula: _tryDegrage_12_to_2_lvl( 0 );  break;
  case HouseLevel::beatyfullInsula: _tryDegrade_20_to_12_lvl( HouseLevel::maxSize2, 1 ); break;
  case HouseLevel::smallVilla: _tryDegrade_20_to_12_lvl( HouseLevel::maxSize2, 2 ); break;
  case HouseLevel::middleVilla: _tryDegrade_20_to_12_lvl( HouseLevel::maxSize2, 2 ); break;
  case HouseLevel::bigVilla: _tryDegrade_20_to_12_lvl( HouseLevel::maxSize3, 3 ); break;
  case HouseLevel::greatVilla: _tryDegrade_20_to_12_lvl( HouseLevel::maxSize3, 3 ); break;
  case HouseLevel::smallPalace: _tryDegrade_20_to_12_lvl( HouseLevel::maxSize3, 4 ); break;
  case HouseLevel::middlePalace: _tryDegrade_20_to_12_lvl( HouseLevel::maxSize3, 4 ); break;
  case HouseLevel::bigPalace: _tryDegrade_20_to_12_lvl( HouseLevel::maxSize4, 5 ); break;
  case HouseLevel::greatPalace: _tryDegrade_20_to_12_lvl( HouseLevel::maxSize4, 6 ); break;
  }

  _update( true );
}

void House::buyMarket( ServiceWalkerPtr walker )
{
  MarketPtr market = ptr_cast<Market>( walker->base() );
  if( market.isNull() )
    return;

  good::Store& marketStore = market->goodStore();

  good::Store& houseStore = goodStore();
  for (good::Product goodType = good::none; goodType < good::goodCount; ++goodType)
  {
    int houseQty = houseStore.qty(goodType);
    int houseSafeQty = _d->spec.computeMonthlyGoodConsumption( this, goodType, false )
                       + _d->spec.next().computeMonthlyGoodConsumption( this, goodType, false );
    houseSafeQty *= 6;

    int marketQty = marketStore.qty(goodType);
    if( houseQty < houseSafeQty && marketQty > 0  )
    {
       int qty = std::min( houseSafeQty - houseQty, marketQty);
       qty = math::clamp( qty, 0, houseStore.freeQty( goodType ) );

       if( qty > 0 )
       {
         good::Stock stock(goodType, qty);
         marketStore.retrieve(stock, qty);

         stock.setCapacity( qty );
         stock.setQty( stock.capacity() );

         houseStore.store(stock, stock.qty() );
       }
    }
  }
}

void House::applyService( ServiceWalkerPtr walker )
{
  Building::applyService(walker);  // handles basic services, and remove service reservation

  Service::Type service = walker->serviceType();
  switch (service)
  {
  case Service::well:
  case Service::fountain:
  case Service::religionNeptune:
  case Service::religionCeres:
  case Service::religionVenus:
  case Service::religionMars:
  case Service::religionMercury:
  case Service::barber:
  case Service::baths:
  case Service::school:
  case Service::library:
  case Service::academy:
  case Service::theater:
  case Service::amphitheater:
  case Service::colloseum:
  case Service::hippodrome:
    setServiceValue(service, 100);
  break;

  case Service::hospital:
  case Service::doctor:
    updateState( (Construction::Param)House::health, 10 );
    setServiceValue(service, 100);
  break;

  case Service::forum:
  case Service::senate:
    setServiceValue(Service::forum,100);
  break;
  
  case Service::market:
    setServiceValue( Service::market, 100 );
    buyMarket(walker);
  break;
 
  case Service::prefect:
    appendServiceValue(Service::crime, -25);
  break;

  case Service::oracle:
  case Service::engineer:
  case Service::srvCount:
  break;

  case Service::recruter:
  {
    int svalue = getServiceValue( service );
    if( !svalue )
      break;

    RecruterPtr recuter = ptr_cast<Recruter>( walker );
    if( recuter.isValid() )
    {
      int hiredWorkers = math::clamp( svalue, 0, recuter->needWorkers() );
      appendServiceValue( service, -hiredWorkers );
      recuter->hireWorkers( hiredWorkers );
    }
  }
  break;

  default:
  break;
  }
}

float House::evaluateService(ServiceWalkerPtr walker)
{
  float res = 0.0;
  Service::Type service = walker->serviceType();
  if( isServiceReserved( service ) )
  {
     // service is already reserved
    return 0.0;
  }

  switch(service)
  {
  case Service::engineer: res = state( Construction::damage ); break;
  case Service::prefect: res = state( Construction::fire ); break;

  case Service::market:
  {
    MarketPtr market = ptr_cast<Market>( walker->base() );
    good::Store& marketStore = market->goodStore();
    good::Store& houseStore = goodStore();
    for( good::Product goodType = good::none; goodType < good::goodCount; ++goodType)
    {
      int houseQty = houseStore.qty(goodType) / 10;
      int houseSafeQty = _d->spec.computeMonthlyGoodConsumption( this, goodType, false)
                         + _d->spec.next().computeMonthlyGoodConsumption( this, goodType, false );
      int marketQty = marketStore.qty(goodType);
      if( houseQty < houseSafeQty && marketQty > 0)
      {
         res += std::min( houseSafeQty - houseQty, marketQty);
      }
    }
  }
  break;

  case Service::forum:
  case Service::senate:
    res = _d->tax;
  break;

  case Service::recruter:
    res = spec().isPatrician() ? 0 : getServiceValue( service );
  break;   

  default:
  {
    return _d->spec.evaluateServiceNeed( this, service);
  }
  break;
  }

  return res;
}

TilesArray House::enterArea() const
{
  if( isWalkable() )
  {
    TilesArray ret;
    ret.push_back( &tile() );
    return ret;
  }
  else
  {
    return Building::enterArea();
  }
}

bool House::build( const CityAreaInfo& info )
{
  bool ret = Building::build( info );
  _update( true );
  return ret;
}

double House::state( ParameterType param) const
{
  switch( (int)param )
  {
  case House::food: return _d->getFoodLevel();
  case House::health: return Building::state( House::health ) + Building::state( House::healthBuff );

  default: return Building::state( param );
  }
}

void House::_update( bool needChangeTexture )
{
  Picture pic = HouseSpecHelper::instance().getPicture( _d->houseLevel, size().width() );
  if( needChangeTexture )
  {
    if( !pic.isValid() )
    {
      Logger::warning( "WARNING!!! House: failed change texture for size %d", size().width() );
      pic = Picture::getInvalid();
    }
    setPicture( pic );
  }

  bool lastFlat = _d->isFlat;

  _d->isFlat = false;//picture().height() <= ( tilemap::cellPicSize().height() * size().width() );
  if( lastFlat != _d->isFlat && _city().isValid() )
    _city()->setOption( PlayerCity::updateTiles, true );

  _d->maxHabitants = _d->spec.getMaxHabitantsByTile() * size().area();
  _d->initGoodStore( size().area() );
}

int House::roadAccessDistance() const {  return 2; }

void House::addHabitants( CitizenGroup& habitants )
{
  int peoplesCount = math::max(_d->maxHabitants - _d->habitants.count(), 0u);
  CitizenGroup newState = _d->habitants;
  newState += habitants.retrieve( peoplesCount );

  _updateHabitants( newState );

  if( _d->houseLevel == HouseLevel::vacantLot )
  {
    _d->houseLevel = HouseLevel::hovel;
    _d->spec = _d->spec.next();
    _update( true );

    city::Helper helper( _city() );
    helper.updateDesirability( this, city::Helper::onDesirability );
  }
}

CitizenGroup House::remHabitants(int count)
{
  count = math::clamp<int>( count, 0, _d->habitants.count() );
  CitizenGroup hb = _d->habitants.retrieve( count );

  _updateHabitants( _d->habitants );

  return hb;
}

void House::destroy()
{
  _d->maxHabitants = 0;

  const unsigned int maxCitizenInGroup = 8;
  const unsigned int workers2fire = workersCount();
  do
  {
    CitizenGroup homeless = _d->habitants.retrieve( std::min( _d->habitants.count(), maxCitizenInGroup ) );
    Emigrant::send2city( _city(), homeless, tile(), math::random( 10 ) > 5 ? "##emigrant_thrown_from_house##" : "##emigrant_no_home##" );
  }
  while( _d->habitants.count() >= maxCitizenInGroup );

  if( workers2fire > 0 )
  {
    GameEventPtr e = FireWorkers::create( pos(), workersCount() );
    e->dispatch();
  }

  _d->habitants.clear();

  Building::destroy();
}

std::string House::sound() const
{
  if( !_d->habitants.count() )
    return "";

  return utils::format( 0xff, "house_%05d", _d->houseLevel*10+1 );
}

std::string House::troubleDesc() const
{
  std::string ret = Building::troubleDesc();

  if( ret.empty() && _d->changeCondition <= 0 )
  {  
    ret = _d->evolveInfo;
  }

  return ret;
}

bool House::isCheckedDesirability() const {  return _city()->buildOptions().isCheckDesirability(); }

void House::__debugChangeLevel(int change)
{
  change > 0
      ? _levelUp()
      : _levelDown();
}

void House::save( VariantMap& stream ) const
{
  Building::save( stream );

  stream[ "desirability" ] = _d->desirability.base;
  stream[ "currentHubitants" ] = _d->habitants.save();
  stream[ "goodstore" ] = _d->goodStore.save();
  stream[ "healthLevel" ] = state( (Construction::Param)House::health );
  VARIANT_SAVE_ANY_D(stream, _d, maxHabitants )
  VARIANT_SAVE_ANY_D(stream, _d, houseLevel )
  VARIANT_SAVE_ANY_D(stream, _d, changeCondition )
  VARIANT_SAVE_ANY_D(stream, _d, taxesThisYear)
  VARIANT_SAVE_ANY_D(stream, _d, poverity)
  VARIANT_SAVE_ANY_D(stream, _d, money)
  VARIANT_SAVE_ANY_D(stream, _d, tax)

  VariantList vl_services;
  foreach( mapItem, _d->services )
  {
    vl_services.push_back( Variant( (int)mapItem->first) );
    vl_services.push_back( Variant( mapItem->second ) );
  }

  stream[ "services" ] = vl_services;
} 

void House::load( const VariantMap& stream )
{
  Building::load( stream );

  VARIANT_LOAD_ANY_D( _d, houseLevel, stream )
  _d->spec = HouseSpecHelper::instance().getSpec(_d->houseLevel);

  _d->desirability.base = (int)stream.get( "desirability", 0 );
  _d->desirability.step = _d->desirability.base < 0 ? 1 : -1;

  _d->habitants.load( stream.get( "currentHubitants" ).toList() );
  VARIANT_LOAD_ANY_D(_d,maxHabitants, stream )
  VARIANT_LOAD_ANY_D(_d,changeCondition, stream )
  VARIANT_LOAD_ANY_D(_d,poverity, stream)
  VARIANT_LOAD_ANY_D(_d,money, stream)
  VARIANT_LOAD_ANY_D(_d,tax, stream )

  _d->goodStore.load( stream.get( "goodstore" ).toMap() );
  _d->currentYear = game::Date::current().year();
  VARIANT_LOAD_ANY_D(_d,taxesThisYear, stream)

  _d->initGoodStore( size().area() );

  _d->services[ Service::recruter ].setMax( _d->habitants.count( CitizenGroup::mature ) );
  VariantList vl_services = stream.get( "services" ).toList();

  for( unsigned int i=0; i < vl_services.size(); i++ )
  {
    Service::Type type = Service::Type( vl_services.get( i ).toInt() );
    _d->services[ type ] = vl_services.get( i+1 ).toFloat(); //serviceValue
  }

  CityAreaInfo info = { _city(), pos(), TilesArray() };
  Building::build( info );

  if( !picture().isValid() )
  {
    _update( true );
  }
}

void House::_disaster()
{
  unsigned int habitantsNuumber = _d->habitants.count();
  unsigned int buriedCitizens = habitantsNuumber - math::random( habitantsNuumber );

  CitizenGroup buriedGroup = _d->habitants.retrieve( buriedCitizens );

  GameEventPtr e = FireWorkers::create( pos(), buriedGroup.count( CitizenGroup::mature ) );
  e->dispatch();
}

void House::collapse()
{
  Building::collapse();
  _disaster();
}

void House::burn()
{
  Building::burn();
  _disaster();
}

int House::Impl::getFoodLevel() const
{
  const good::Product f[] = { good::wheat, good::fish, good::meat, good::fruit, good::vegetable };
  std::set<good::Product> foods( f, f+5 );

  int ret = 0;
  int foodLevel = spec.minFoodLevel();
  if( foodLevel == 0 )
    return 0;

  while( foodLevel > 0 )
  {
    good::Product maxFtype = good::none;
    int maxFoodQty = 0;
    foreach( ft, foods )
    {
      int tmpQty = goodStore.qty( *ft );
      if( tmpQty > maxFoodQty )
      {
        maxFoodQty = tmpQty;
        maxFtype = *ft;
      }
    }

    ret += math::percentage( maxFoodQty, goodStore.capacity( maxFtype ) );
    foods.erase( maxFtype );
    foodLevel--;
  }

  ret /= spec.minFoodLevel();
  return ret;
}

unsigned int House::workersCount() const
{
  const Service& srvc = _d->services[ Service::recruter ];
  return srvc.max() - srvc.value();
}

bool House::isEducationNeed(Service::Type type) const
{
  int lvl = _d->spec.minEducationLevel();
  switch( type )
  {
  case Service::school: return (lvl>0);
  case Service::academy: return (lvl>1);
  case Service::library: return (lvl>2);
  default: break;
  }

  return false;
}

bool House::isEntertainmentNeed(Service::Type type) const
{
  int lvl = _d->spec.minEntertainmentLevel();
  switch( type )
  {
  case Service::theater: return (lvl>=10);
  case Service::amphitheater: return (lvl>=30);
  case Service::colloseum: return (lvl>=60);
  case Service::hippodrome: return (lvl>=80);
  default: break;
  }

  return false;
}

bool House::isHealthNeed(Service::Type type) const
{
  int lvl = _d->spec.minHealthLevel();
  switch( type )
  {
  case Service::baths: return (lvl>0);
  case Service::doctor: return (lvl>=1);
  case Service::hospital: return (lvl>=2);
  default: break;
  }

  return false;
}

float House::collectTaxes()
{
  float tax = _d->tax;
  _d->taxesThisYear += tax;
  _d->tax = 0.f;
  _d->lastTaxationDate = game::Date::current();
  return tax;
}

float House::taxesThisYear() const { return _d->taxesThisYear; }

void House::appendMoney(float money) {  _d->money += money; }
DateTime House::lastTaxationDate() const{  return _d->lastTaxationDate;}
std::string House::evolveInfo() const{  return _d->evolveInfo;}
bool House::isWalkable() const{  return size().width() == 1; }
bool House::isFlat() const { return _d->isFlat; }
const CitizenGroup& House::habitants() const  {  return _d->habitants; }
good::Store& House::goodStore(){ return _d->goodStore; }
const HouseSpecification& House::spec() const{   return _d->spec; }
bool House::hasServiceAccess( Service::Type service) {  return (_d->services[service] > 0); }
float House::getServiceValue( Service::Type service){  return _d->services[service]; }
void House::setServiceValue( Service::Type service, float value) {  _d->services[service] = value; }
unsigned int House::maxHabitants() {  return _d->maxHabitants; }
void House::appendServiceValue( Service::Type srvc, float value){  setServiceValue( srvc, getServiceValue( srvc ) + value ); }

Desirability House::desirability() const
{
  Desirability ret = _d->desirability;
  if( _d->habitants.empty() )
  {
    ret.base = 0;
    ret.range = 0;
  }
  return ret;
}

std::string House::levelName() const
{
  std::string ret = spec().levelName();
  bool big = false;
  switch( spec().level() )
  {
  case HouseLevel::hovel:
  case HouseLevel::tent:
  case HouseLevel::shack:
  case HouseLevel::hut:
      big = size().width() > 1; break;
  default:
      break;
  }

  if( big )
  {
    ret = utils::replace( ret, "small", "big" );
  }

  return ret;
}

void House::Impl::updateHealthLevel( HousePtr house )
{
  float delim = 1 + (((services[Service::well] > 0 || services[Service::fountain] > 0) ? 1 : 0))
      + ((services[Service::doctor] > 0 || services[Service::hospital] > 0) ? 1 : 0)
      + (services[Service::baths] > 0 ? 0.7 : 0)
      + (services[Service::barber] > 0 ? 0.3 : 0);

  float decrease = 2.f / delim;

  house->updateState( (Construction::Param)House::health, -decrease );
  int value = 100 - house->state( House::health );
  if( value > 25 )
  {

  }
}

void House::Impl::initGoodStore(int size)
{
  int rsize = 25 * size * houseLevel;
  goodStore.setCapacity( rsize * 10 );  // no limit
  goodStore.setCapacity(good::wheat, rsize );
  goodStore.setCapacity(good::fish, rsize );
  goodStore.setCapacity(good::meat, rsize );
  goodStore.setCapacity(good::fruit, rsize );
  goodStore.setCapacity(good::vegetable, rsize );
  goodStore.setCapacity(good::pottery, rsize );
  goodStore.setCapacity(good::furniture, rsize);
  goodStore.setCapacity(good::oil, rsize );
  goodStore.setCapacity(good::wine, rsize );
}

void House::Impl::consumeServices()
{
  int currentWorkersPower = services[ Service::recruter ];       //save available workers number

  foreach( s, services ) { s->second -= 1; } //consume services

  services[ Service::recruter ] = currentWorkersPower;     //restore available workers number
}

void House::Impl::consumeGoods( HousePtr house )
{
  for( good::Product goodType = good::olive; goodType < good::goodCount; ++goodType)
  {
     int montlyGoodsQty = spec.computeMonthlyGoodConsumption( house, goodType, true );
     goodStore.setQty( goodType, std::max( goodStore.qty(goodType) - montlyGoodsQty, 0) );
  }
}

void House::Impl::consumeFoods(HousePtr house)
{
  const int foodLevel = spec.minFoodLevel();
  if( foodLevel == 0 )
    return;

  const int needFoodQty = spec.computeMonthlyFoodConsumption( house ) * spec.foodConsumptionInterval() / game::Date::days2ticks( 30 );

  int availableFoodLevel = 0;
  for( good::Product afl=good::wheat; afl <= good::vegetable; ++afl )
  {
    availableFoodLevel += ( goodStore.qty( afl ) > 0 ? 1 : 0 );
  }
  availableFoodLevel = std::min( availableFoodLevel, foodLevel );
  bool haveFoods4Eating = ( availableFoodLevel > 0 );

  if( haveFoods4Eating )
  {
    int alsoNeedFood = needFoodQty;
    while( alsoNeedFood > 0 )
    {
      int realConsumedQty = 0;
      for( good::Product gType=good::wheat; gType <= good::vegetable; ++gType )
      {
        int vQty = std::min( goodStore.qty( gType ), needFoodQty / availableFoodLevel );
        vQty = std::min( vQty, alsoNeedFood );
        if( vQty > 0 )
        {
          realConsumedQty += vQty;
          alsoNeedFood -= vQty;
          goodStore.setQty( gType, std::max( goodStore.qty( gType ) - vQty, 0) );
        }
      }

      if( realConsumedQty == 0 )
      {
        haveFoods4Eating = false;
        break;
      }
    }
  }

  if( !haveFoods4Eating )
  {
    Logger::warning( "House: [%dx%d] have no food for habitants", house->pos().i(), house->pos().j() );
  }
}
