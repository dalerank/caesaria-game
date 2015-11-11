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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#include "house.hpp"
#include "gfx/helper.hpp"
#include "objects/house_spec.hpp"
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
#include "good/storage.hpp"
#include "city/statistic.hpp"
#include "core/foreach.hpp"
#include "constants.hpp"
#include "events/build.hpp"
#include "events/fireworkers.hpp"
#include "core/gettext.hpp"
#include "core/logger.hpp"
#include "game/funds.hpp"
#include "city/build_options.hpp"
#include "city/statistic.hpp"
#include "walker/patrician.hpp"
#include "city/victoryconditions.hpp"
#include "house_plague.hpp"
#include "objects_factory.hpp"
#include "game/difficulty.hpp"
#include "house_habitants.hpp"

using namespace gfx;
using namespace events;
using namespace city;
REGISTER_CLASS_IN_OVERLAYFACTORY(object::house, House)

namespace {
  enum { needDegrade=-2, noEvolve=0, mayEvolve=2, happinessWeekChange=5,
         maxTableTax=25, defaultHappiness=50, maxHappiness=100 };

  static int happines4tax[maxTableTax] = { 10,  9,  7,  6,  4,
                                            2,  1,  0, -1, -2,
                                           -2, -3, -4, -5, -7,
                                           -9,-11,-13,-15,-17,
                                          -19,-21,-23,-27,-31 };

  int getHappines4tax( int tax )
  {
    tax = math::max( tax, 0 );
    return tax > maxTableTax
              ? -tax * 1.3
              : happines4tax[ tax ];
  }
}

class ISrvcAdapter
{
public:
  virtual ~ISrvcAdapter() {}
  virtual void set( float value ) = 0;
  virtual float value() const = 0;
  virtual float max() const = 0;
  virtual void setMax( float value ) = 0;
  virtual void consume( float value ) = 0;
};

template<class T>
class SrvcAdapter : public ISrvcAdapter
{
public:
  T* obj;

  SrvcAdapter() { obj = new T(); }
  SrvcAdapter(int) {}
  virtual ~SrvcAdapter() { if( obj ) delete obj; }
  virtual void set( float value ) { obj->set( value ); }
  virtual float value() const { return obj->value(); }
  virtual float max() const { return obj->max(); }
  virtual void setMax( float value ) { obj->setMax( value ); }
  virtual void consume( float delta ) { set( value() + delta ); }
};

class HbtAdapter : public SrvcAdapter<RecruterService>
{
public:
  HbtAdapter( Habitants& hbt ) : SrvcAdapter(0)
  {
    obj = new RecruterService( hbt );
  }

  virtual void consume( float delta ) {} //worker force not consumable
};

class Services : public std::map<Service::Type, ISrvcAdapter*>
{
public:
  ~Services()
  {
    foreach(it, *this)
      delete it->second;
  }

  Services()
  {
    for( int i = 0; i<Service::srvCount; ++i )
    {
      insert( std::make_pair( Service::Type(i), new SrvcAdapter<Service>() ) );
    }

    at( Service::crime )->set( 0 );
  }

  ISrvcAdapter* at( Service::Type t )
  {
    iterator it = find( t );
    if( it != end() )
      return it->second;
    else
    {
      ISrvcAdapter* newService = new SrvcAdapter<Service>();
      insert( std::make_pair( t, newService ) );
      return newService;
    }
  }

  void replace( Service::Type t, ISrvcAdapter* adapter )
  {
    iterator it = find( t );
    if( it != end() )
      delete it->second;

    it->second = adapter;
  }

  void load( const VariantList& stream )
  {
    for( unsigned int i=0; i < stream.size(); i+=2 )
    {
      Service::Type type = Service::Type( stream.get( i ).toInt() );
      at( type )->set( stream.get( i+1 ).toFloat() ); //serviceValue
    }
  }

  VariantList save() const
  {
     VariantList ret;
     foreach( mapItem, *this )
     {
       ret.push_back( Variant( (int)mapItem->first ) );
       ret.push_back( Variant( mapItem->second->value() ) );
     }

     return ret;
  }
};

class House::Impl
{
public:
  int houseLevel;
  int poverity;

  struct
  {
    float money, tax;
    float taxesThisYear;
    DateTime lastTaxationDate;
  } economy;

  HouseSpecification spec;  // characteristics of the current house level
  Desirability desirability;
  good::Storage goodstore;
  Services services;  // value=access to the service (0=no access, 100=good access)  
  Point randomOffset;
  std::string evolveInfo;
  Habitants habitants;
  Animation healthAnimation;

  bool isFlat;
  int currentYear;
  int changeCondition;
  int needHappiness;
  Pictures ground;

public:
  void initGoodStore( int size );
  void consumeServices();
  void consumeGoods(HousePtr house);
  void consumeFoods(HousePtr house);
  int getFoodLevel() const;
};

House::House( HouseLevel::ID level )
  : Building( object::house ), _d( new Impl )
{
  HouseSpecHelper& helper = HouseSpecHelper::instance();
  _d->houseLevel = level;
  _d->spec = helper.getSpec( _d->houseLevel );
  _d->desirability.base = -3;
  _d->desirability.range = 3;
  _d->desirability.step = 1;
  _d->economy.money = 0;
  _d->economy.tax = 0;
  _d->economy.taxesThisYear = 0;
  _d->currentYear = game::Date::current().year();

  setState( pr::health, 100 );
  setState( pr::fire, 0 );

  _d->changeCondition = 0;
  _d->needHappiness = 100;
  setState( pr::happiness, 100 );

  _d->initGoodStore( 1 );

  // init the service access
  _d->services.replace( Service::recruter, new HbtAdapter( _d->habitants ) );

  _update( true );
}

void House::_checkEvolve()
{
  bool validate = _d->spec.checkHouse( this, &_d->evolveInfo );
  if( !validate )
  {
    _d->changeCondition--;
    if( _d->changeCondition <= needDegrade )
    {
      _d->changeCondition = noEvolve;
      _levelDown();
    }
  }
  else
  {
    _d->evolveInfo = "";
    HouseSpecification nextSpec = _d->spec.next();

    bool mayUpgrade =  nextSpec.checkHouse( this, &_d->evolveInfo );

    object::Type needBuilding;
    TilePos rPos;

    bool haveUnwishBuildingsNearMe = nextSpec.findUnwishedBuildingNearby( this, needBuilding, rPos ) > 0;
    bool haveLowHouseNearMe = nextSpec.findLowLevelHouseNearby( this, rPos ) > 0;
    if( haveUnwishBuildingsNearMe || haveLowHouseNearMe )
    {
      _d->evolveInfo = "##nearby_building_negative_effect##";
      mayUpgrade = false;
    }

    if( mayUpgrade )
    {
      _d->changeCondition++;
      if( _d->changeCondition >= mayEvolve )
      {
        _d->changeCondition = noEvolve;
        _levelUp();
      }
    }
    else
    {
      _d->changeCondition = noEvolve;
    }
  }

  if( _d->changeCondition < noEvolve )
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
  else if( _d->changeCondition > noEvolve )
  {
    _d->evolveInfo = _("##house_evolves_at##");
  }
}

void House::_checkPatricianDeals()
{
  if( !spec().isPatrician() )
    return;

  const TilesArray& roads = roadside();
  if( !roads.empty() )
  {
    auto patrician = Walker::create<Patrician>( _city() );
    patrician->send2City( this );
  }
}

void House::_updateTax()
{
  int difficulty = _city()->getOption( PlayerCity::difficulty );
  float multiply = 1.0f;
  switch (difficulty)
  {
    case game::difficulty::fun: multiply = 3.0f; break;
    case game::difficulty::easy: multiply = 2.0f; break;
    case game::difficulty::simple: multiply = 1.5f; break;
    case game::difficulty::usual: multiply = 1.0f; break;
    case game::difficulty::nicety: multiply = 0.75f; break;
    case game::difficulty::hard: multiply = 0.5f; break;
    case game::difficulty::impossible: multiply = 0.25f; break;
  }

  float cityTax = _city()->treasury().taxRate() / 100.f;
  cityTax = (multiply * _d->habitants.mature_n() / _d->spec.taxRate()) * cityTax;

  _d->economy.money -= cityTax;
  _d->economy.tax += cityTax;
}

void House::_updateCrime()
{
  float cityKoeff = _city()->statistic().balance.koeff();

  const int currentHabtn = habitants().count();

  if( currentHabtn == 0 )
    return;

  int unemploymentPrc = math::percentage( _d->habitants.workers.current, _d->habitants.workers.max );
  int unemployedInfluence = 0; ///!!!
  if( unemploymentPrc > 25 ) { unemployedInfluence = -3; }
  else if( unemploymentPrc > 17 ) { unemployedInfluence = -2; }
  else if( unemploymentPrc > 10 ) { unemployedInfluence = -1; }
  else if( unemploymentPrc < 5 ) { unemployedInfluence = 1; }

  int workerWagesInfluence = 0; ///!!!
  if( !spec().isPatrician() )
  {
    int diffWages = _city()->statistic().workers.wagesDiff();
    if( diffWages < 0)
    {
      workerWagesInfluence = diffWages;
    }
    else if( diffWages > 0 )
    {
      workerWagesInfluence = std::min( diffWages, 8 ) / 2;
    }
  }

  int taxValue = _city()->treasury().taxRate();
  int taxrateInfluence = getHappines4tax( taxValue ); ///!!!
  if( spec().isPatrician() )
  {
    taxrateInfluence *= 1.5;
  }

  int foodAbundanceInfluence = 0; ///!!!
  int foodStockInfluence = 0; ///!!!
  int monthWithFood = 0;
  if( _d->spec.minFoodLevel() > 0 )
  {
    int foodStoreQty = 0;
    int foodTypeCount = 0;
    for( good::Product k=good::wheat; k <= good::vegetable; ++k )
    {
      int qty = _d->goodstore.qty( k );
      foodStoreQty += qty;
      foodTypeCount += (qty > 0 ? 1 : 0);
    }

    int diffFoodLevel = foodTypeCount - _d->spec.minFoodLevel();
    foodAbundanceInfluence = diffFoodLevel * (diffFoodLevel < 0 ? 4 : 2);

    const unsigned int habtnConsumeGoodQty = currentHabtn / 2;
    monthWithFood = foodStoreQty / (habtnConsumeGoodQty+1);

    foodStockInfluence = math::clamp<double>( -4 + 1.25 * monthWithFood, -4., 4. );
  }

  int poverityHouse = _d->spec.level() > HouseLevel::hut ? -_d->poverity / 2 : 0;
  int desirabilityInfluence = 0;

  if( level() > HouseLevel::hut )
  {
    TilePos offset( 4, 4 );
    TilePos sizeOffset( size().width(), size().height() );
    TilesArea tiles( _map(), pos() - offset, pos() + sizeOffset + offset );
    int averageDes = 0;

    for( auto it : tiles )
      averageDes += it->param( Tile::pDesirability );

    averageDes /= (tiles.size() + 1);

    desirabilityInfluence = math::clamp( averageDes - spec().minDesirabilityLevel(), -10, 10 );
    if( averageDes < spec().minDesirabilityLevel() )
      desirabilityInfluence -= 5;
    else if( averageDes > spec().maxDesirabilityLevel() )
      desirabilityInfluence += 2;

    if( monthWithFood < 2 ) //if we have no food, then break positive influence
      desirabilityInfluence = math::min( desirabilityInfluence, 0 );
  }

  int curHappiness = defaultHappiness
                  + unemployedInfluence
                  + workerWagesInfluence
                  + taxrateInfluence
                  + foodAbundanceInfluence
                  + foodStockInfluence
                  + poverityHouse
                  + desirabilityInfluence
                  + (int)state( pr::happinessBuff );  

  _d->needHappiness = curHappiness;

  int unhappyValue = maxHappiness - curHappiness;
  int signChange = math::signnum( unhappyValue - getServiceValue( Service::crime) );

  appendServiceValue( Service::crime, _d->spec.crime() * signChange * cityKoeff );
}

void House::_updateHappiness()
{
  int signChange = math::signnum( _d->needHappiness - state( pr::happiness ) );
  updateState( pr::happiness, signChange * happinessWeekChange );
}

void House::_updateHomeless()
{
  int homeless = _d->habitants.homeless();
  if( homeless > 0 )
  {
    homeless /= (homeless > 4 ? 2 : 1);
    CitizenGroup homelessGroup = removeHabitants( homeless );

    Emigrant::send2city( _city(), homelessGroup, tile(), "##emigrant_no_home##" );
  }
}

void House::_settleVacantLotIfNeed()
{
  if( _d->houseLevel == HouseLevel::vacantLot )
  {
    _d->houseLevel = HouseLevel::hovel;
    _d->spec = _d->spec.next();
    _update( true );

    Desirability::update( _city(), this, Desirability::on );
  }
}

void House::_updateConsumptions( const unsigned long time )
{
  if( time % spec().consumptionInterval( HouseSpecification::intv_service ) == 0 )
  {
    _d->consumeServices();
    _updateHealthLevel();
    cancelService( Service::recruter );
  }

  if( time % spec().consumptionInterval( HouseSpecification::intv_foods ) == 0 )
  {
    _d->consumeFoods( this );
  }

  if( time % spec().consumptionInterval( HouseSpecification::intv_goods ) == 0 )
  {
    _d->consumeGoods( this );
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
    _d->habitants.makeGeneration( *this );
    _d->economy.taxesThisYear = 0;
  }

  _updateConsumptions( time );

  if( game::Date::isMonthChanged() )
  {
    setState( pr::settleLock, 0 );
    _updateTax(); 

    if( _d->economy.money > 0 ) { _d->poverity--; }
    else { _d->poverity += 2; }

    _d->poverity = math::clamp( _d->poverity, 0, 100 );
  }

  if( game::Date::isWeekChanged() )
  {
    _checkEvolve();
    _updateHappiness();
    _updateCrime();
    _updateHomeless();
    _checkPatricianDeals();
  }

  Building::timeStep( time );
}

bool House::_tryEvolve_1_to_12_lvl( int level4grow, int growSize, const char desirability )
{
  if( size().width() == 1 )
  {
    TilesArea area( _map(), pos(), Size(2) );

    bool mayGrow = true;

    for( auto tile : area )
    {
      if( tile == NULL )
      {
        mayGrow = false;   //some broken, can't grow
        break;
      }

      auto house = tile->overlay<House>();
      if( house != NULL &&
          (house->level() == level4grow || house->habitants().count() == 0) )
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
      auto selfHouse = (*delIt)->overlay<House>( );

      _d->initGoodStore( Size( growSize ).area() );

      ++delIt; //don't remove himself
      for( ; delIt != area.end(); ++delIt )
      {
        auto house = (*delIt)->overlay<House>();
        if( house.isValid() )
        {          
          sumHabitants += house->habitants();

          house->deleteLater();
          house->_d->habitants.clear();

          sumFreeWorkers += house->getServiceValue( Service::recruter );

          house->_setServiceMaxValue( Service::recruter, 0 );

          selfHouse->store().storeAll( house->store() );
        }
      }

      _d->habitants.set( sumHabitants );
      setServiceValue( Service::recruter, sumFreeWorkers );

      //reset desirability level with old house size
      Desirability::update( _city(), this, Desirability::off );

      setSize( Size( growSize ) );
      //_update( false );

      city::AreaInfo info( _city(), pos() );
      build( info );
      //set new desirability level
      Desirability::update( _city(), this, Desirability::on );
    }
  }

  //that this house will be upgrade, we need decrease current desirability level
  Desirability::update( _city(), this, Desirability::off );

  _d->desirability.base = desirability;
  _d->desirability.step = desirability < 0 ? 1 : -1;
  //now upgrade groud area to new desirability
  Desirability::update( _city(), this, Desirability::on );

  return true;
}

bool House::_tryEvolve_12_to_20_lvl( int level4grow, int minSize, const char desirability )
{
  //startPic += math::random( 10 ) > 5 ? 1 : 0;
  bool mayGrow = true;
  TilePos buildPos = tile().pos();

  if( size() == Size( minSize-1 ) )
  {
    Tilemap& tmap = _map();
    std::map<TilePos, TilesArray> possibleAreas;

    TilePos sPos = tile().pos();
    possibleAreas[ sPos ] = tmap.area( sPos, Size(minSize) );
    sPos = tile().pos() - TilePos( 1, 0 );
    possibleAreas[ sPos ] = tmap.area( sPos, Size(minSize) );
    sPos = tile().pos() - TilePos( 1, 1 );
    possibleAreas[ sPos ] = tmap.area( sPos, Size(minSize) );
    sPos = tile().pos() - TilePos( 0, 1 );
    possibleAreas[ sPos ] = tmap.area( sPos, Size(minSize) );

    foreach( itArea, possibleAreas )
    {
      TilesArray& area = itArea->second;

      for( TilesArray::iterator it=area.begin(); it != area.end(); )
      {
        if( (*it)->overlay() == this ) { it = area.erase( it ); }
        else { ++it; }
      }

      for( auto& tile : area )
      {
        if( tile == NULL )
        {
          mayGrow = false;   //some broken, can't grow
          break;
        }

        OverlayPtr overlay = tile->overlay();
        if( overlay.isNull() )
        {
          if( !tile->getFlag( Tile::isConstructible ) )
          {
            mayGrow = false; //not constuctible, can't grow
            break;
          }
        }
        else
        {
          if( overlay->type() != object::garden )
          {
            mayGrow = false; //not garden, can't grow
            break;
          }
        }
      }

      if( mayGrow )
      {
        buildPos = itArea->first;
        Desirability::update( _city(), this, Desirability::off );
        setSize( Size( minSize ) );
        _update( true );
        city::AreaInfo info( _city(), buildPos );
        build( info );

        _d->desirability.base = desirability;
        _d->desirability.step = desirability < 0 ? 1 : -1;

        Desirability::update( _city(), this, Desirability::on );
        break;
      }
    }
  }

  if( mayGrow )
  {
    //that this house will be upgrade, we need decrease current desirability level
    Desirability::update( _city(), this, Desirability::off );

    _d->desirability.base = desirability;
    _d->desirability.step = desirability < 0 ? 1 : -1;

    //now upgrade groud area to new desirability
    Desirability::update( _city(), this, Desirability::on );
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

  if( _d->houseLevel >= _city()->victoryConditions().maxHouseLevel() )
  {
    _d->evolveInfo = "##emperor_limit_houseupgrade##";
    return;
  }

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
  case HouseLevel::beatyfullInsula:mayUpgrade=_tryEvolve_12_to_20_lvl( HouseLevel::bigInsula, HouseLevel::maxSize2, 1 ); break;
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
      events::dispatch<FireWorkers>( pos(), habitants().mature_n() );
    }

    _update( true );
  }
}

void House::_tryDegrage_12_to_2_lvl( const char desirability )
{
  //clear current desirability influence
  Desirability::update( _city(), this, Desirability::off );

  _d->desirability.base = desirability;
  _d->desirability.step = desirability < 0 ? 1 : -1;
  //set new desirability level
  Desirability::update( _city(), this, Desirability::on );
}

void House::_tryDegrade_20_to_12_lvl( int rsize, const char desirability )
{
  bool bigSize = size().width() > rsize;
  //_d->houseId = startPicId;
  //_d->picIdOffset = startPicId + ( math::random( 10 ) > 6 ? 1 : 0 );

  //clear current desirability influence
  Desirability::update( _city(), this, Desirability::off );

  _d->desirability.base = desirability;
  _d->desirability.step = desirability < 0 ? 1 : -1;

  TilePos bpos = pos();
  if( bigSize )
  {
    const TilesArray& roads = roadside();
    TilePos moveVector = TilePos( 1, 1 );
    if( !roads.empty() )
    {
      TilePos roadPos = roads.front()->pos();
      moveVector = TilePos( math::signnum( roadPos.i() - bpos.i() ),
                            math::signnum( roadPos.j() - bpos.j() ) );
    }

    TilesArray lastArea = area();
    for( auto tile : lastArea )
    {
      tile->setMaster( 0 );
      tile->setOverlay( 0 );
    }

    setSize( Size( rsize ) );
    city::AreaInfo info( _city(), bpos + moveVector );
    build( info );
  }
  //set new desirability level
  Desirability::update( _city(), this, Desirability::on );
}

void House::_setServiceMaxValue(Service::Type type, unsigned int value)
{
  _d->services.at( type )->setMax( value );
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
      CitizenGroup homeless = removeHabitants( currentPeople );

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
    Desirability::update( _city(), this, Desirability::off );
  }
  break;

  case HouseLevel::hovel:
  {
    if( size().area() > 1 )
    {
      TilesArray perimetr = _map().area( pos(), Size(2) );
      int peoplesPerHouse = habitants().count() / 4;
      for( auto tile : perimetr )
      {
        auto house = Overlay::create<House>( object::house );
        CitizenGroup moveGroup = removeHabitants( peoplesPerHouse );
        house->addHabitants( moveGroup );

        events::dispatch<BuildAny>( tile->pos(), ptr_cast<Overlay>( house ) );
      }

      _setServiceMaxValue( Service::recruter, 0 );
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
  MarketPtr market = _map().overlay<Market>( walker->baseLocation() );
  if( market.isNull() )
    return;

  good::Store& marketStore = market->goodStore();

  good::Store& houseStore = store();
  for( auto& goodType : good::all() )
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
         good::Stock stock( goodType, qty);
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
    updateState( pr::health, 10 );
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
    int svalue = getServiceValue( Service::recruter );
    if( !svalue )
      break;

    auto recuter = walker.as<Recruter>();
    if( recuter != NULL )
    {
      int hiredWorkers = math::min(svalue, recuter->needWorkers());
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
  case Service::engineer: res = state( pr::damage ); break;
  case Service::prefect: res = state( pr::fire ); break;

  case Service::market:
  {
    MarketPtr market = _map().overlay<Market>( walker->baseLocation() );
    good::Store& marketStore = market->goodStore();
    good::Store& houseStore = store();
    foreach( goodType, good::all() )
    {
      int houseQty = houseStore.qty( *goodType) / 10;
      int houseSafeQty = _d->spec.computeMonthlyGoodConsumption( this, *goodType, false)
                         + _d->spec.next().computeMonthlyGoodConsumption( this, *goodType, false );
      int marketQty = marketStore.qty( *goodType );
      if( houseQty < houseSafeQty && marketQty > 0)
      {
         res += std::min( houseSafeQty - houseQty, marketQty);
      }
    }
  }
  break;

  case Service::forum:
  case Service::senate:
    res = _d->economy.tax;
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

bool House::build( const city::AreaInfo& info )
{
  bool ret = Building::build( info );  
  _update( true );
  return ret;
}

const Pictures& House::pictures(Renderer::Pass pass) const
{
  switch( pass )
  {
  case Renderer::overlayGround: return _d->ground;
  default: break;
  }

  return Building::pictures( pass );
}

double House::state(Param param) const
{
  if( param == pr::food ) { return _d->getFoodLevel(); }
  else if( param == pr::health ) { return Building::state( pr::health ) + Building::state( pr::healthBuff ); }
  else return Building::state( param );
}

void House::_update( bool needChangeTexture )
{
  Picture pic = HouseSpecHelper::instance().getPicture( _d->houseLevel, size().width() );
  if( needChangeTexture )
  {
    if( !pic.isValid() )
    {
      Logger::warning( "WARNING!!! House: failed change texture for size {0}", size().width() );
      pic = Picture::getInvalid();
    }

    if( _city().isValid() && !_city()->getOption( PlayerCity::c3gameplay ) )
    {
      _d->randomOffset = Point( math::random( 15 ), math::random( 15 ) ) - Point( 7, 7 );
      pic.addOffset( _d->randomOffset );
    }

    _updateGround();

    setPicture( pic );
  }

  bool lastFlat = _d->isFlat;

  _d->isFlat = false;//picture().height() <= ( tilemap::cellPicSize().height() * size().width() );
  if( lastFlat != _d->isFlat && _city().isValid() )
    _city()->setOption( PlayerCity::updateTiles, true );

  _d->habitants.updateCapacity( *this );
  _d->initGoodStore( size().area() );
}

void House::_updateGround()
{
  if( _city().isValid() && !_city()->getOption( PlayerCity::c3gameplay ) )
  {
    _d->ground.clear();
    _d->ground << Picture( "housng1g", size().width() );
  }
}

int House::roadsideDistance() const { return 2; }

void House::addHabitants( CitizenGroup& arrived )
{
  int peoplesCount = _d->habitants.freeRoom();
  CitizenGroup newState = _d->habitants;
  newState += arrived.retrieve( peoplesCount );

  _d->habitants.update( *this, newState );
  _settleVacantLotIfNeed();
}

void House::removeHabitants( CitizenGroup& group )
{
  CitizenGroup retrieve = _d->habitants;
  retrieve.exclude( group );

  _d->habitants.update( *this, retrieve );
}

CitizenGroup House::removeHabitants(int count)
{
  count = math::clamp<int>( count, 0, _d->habitants.count() );
  CitizenGroup newState = _d->habitants;
  CitizenGroup retrieve = newState.retrieve( count );

  _d->habitants.update( *this, newState );

  return retrieve;
}

void House::destroy()
{
  _d->habitants.capacity = 0;

  const unsigned int maxCitizenInGroup = 8;
  do
  {
    CitizenGroup homeless = removeHabitants( std::min( _d->habitants.count(), maxCitizenInGroup ) );
    Emigrant::send2city( _city(), homeless, tile(), math::random( 10 ) > 5 ? "##emigrant_thrown_from_house##" : "##emigrant_no_home##" );
  }
  while( _d->habitants.count() >= maxCitizenInGroup );

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

void House::__debugMakeGeneration()
{
  _d->habitants.makeGeneration( *this );
}

void House::save( VariantMap& stream ) const
{
  Building::save( stream );

  stream[ "desirability" ] = _d->desirability.base;
  stream[ "healthLevel" ] = state( pr::health );
  VARIANT_SAVE_CLASS_D( stream, _d, habitants )
  VARIANT_SAVE_CLASS_D( stream, _d, goodstore )  
  VARIANT_SAVE_ANY_D  (stream, _d, houseLevel )
  VARIANT_SAVE_ANY_D  (stream, _d, changeCondition )
  VARIANT_SAVE_ANY_D  (stream, _d, economy.taxesThisYear)
  VARIANT_SAVE_ANY_D  (stream, _d, economy.lastTaxationDate)
  VARIANT_SAVE_ANY_D  (stream, _d, poverity)
  VARIANT_SAVE_ANY_D  (stream, _d, economy.money)
  VARIANT_SAVE_ANY_D  (stream, _d, economy.tax)
  VARIANT_SAVE_CLASS_D(stream, _d, services )
} 

void House::debugLoadOld( int saveFormat, const VariantMap& stream )
{
  if( saveFormat == 64 )
  {
    CitizenGroup group;
    group.load( stream.get( "currentHubitants" ).toList() );
    _d->habitants.set( group );
    _d->habitants.capacity = stream.get( "habitants.maximum" );

    VariantList vl_services = stream.get( "services" ).toList();

    for( unsigned int i=0; i < vl_services.size(); i++ )
    {
      Service::Type type = Service::Type( vl_services.get( i ).toInt() );
      int value = vl_services.get( i+1 ).toFloat(); //serviceValue

      if( type == Service::recruter )
      {
        _d->habitants.workers.current = value;
        _d->habitants.workers.max = _d->habitants.mature_n();
      }
    }
  }
}

void House::load( const VariantMap& stream )
{  
  Building::load( stream );

  VARIANT_LOAD_ANY_D( _d, houseLevel, stream )
  _d->spec = HouseSpecHelper::instance().getSpec(_d->houseLevel);

  _d->desirability.base = (int)stream.get( "desirability", 0 );
  _d->desirability.step = _d->desirability.base < 0 ? 1 : -1;

  VARIANT_LOAD_CLASS_D(_d, habitants, stream );
  VARIANT_LOAD_ANY_D  (_d, changeCondition,   stream )
  VARIANT_LOAD_ANY_D  (_d, poverity,          stream )
  VARIANT_LOAD_ANY_D  (_d, economy.money,     stream )
  VARIANT_LOAD_TIME_D (_d, economy.lastTaxationDate, stream )
  VARIANT_LOAD_ANY_D  (_d, economy.tax,       stream )
  VARIANT_LOAD_CLASS_D(_d, goodstore, stream )
  VARIANT_LOAD_ANY_D  (_d, economy.taxesThisYear, stream )
  VARIANT_LOAD_CLASS_D_LIST( _d, services, stream )

  _d->currentYear = game::Date::current().year();

  _d->initGoodStore( size().area() );

  city::AreaInfo info( _city(), pos() );
  Building::build( info );

  if( !picture().isValid() )
  {
    _update( true );
  }

  _updateGround();
}

void House::_disaster()
{
  //this really killed people, cant calculate their
  removeHabitants( math::random( _d->habitants.count() ) );
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
    for( auto goodType : foods )
    {
      int tmpQty = goodstore.qty( goodType );
      if( tmpQty > maxFoodQty )
      {
        maxFoodQty = tmpQty;
        maxFtype = goodType;
      }
    }

    ret += math::percentage( maxFoodQty, goodstore.capacity( maxFtype ) );
    foods.erase( maxFtype );
    foodLevel--;
  }

  ret /= spec.minFoodLevel();
  return ret;
}

unsigned int House::hired() const
{
  ISrvcAdapter* srvc = _d->services.at( Service::recruter );
  return srvc->max() - srvc->value();
}

unsigned int House::unemployed() const
{
  ISrvcAdapter* srvc = _d->services.at( Service::recruter );
  return srvc->value();
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
  float tax = _d->economy.tax;
  _d->economy.taxesThisYear += tax;
  _d->economy.tax = 0.f;
  _d->economy.lastTaxationDate = game::Date::current();
  return tax;
}

float House::taxesThisYear() const { return _d->economy.taxesThisYear; }

void House::appendMoney(float money)                             { _d->economy.money += money; }
DateTime House::lastTaxationDate() const                         { return _d->economy.lastTaxationDate;}
std::string House::evolveInfo() const                            { return _d->evolveInfo;}
bool House::isWalkable() const                                   { return size().width() == 1; }
bool House::isFlat() const                                       { return _d->isFlat; }
const CitizenGroup& House::habitants() const                     { return _d->habitants; }
good::Store& House::store()                                      { return _d->goodstore; }
const HouseSpecification& House::spec() const                    { return _d->spec; }
HouseLevel::ID House::level() const                              { return spec().level(); }
bool House::hasServiceAccess( Service::Type service)             { return getServiceValue(service) > 0; }
float House::getServiceValue( Service::Type service)             { return _d->services.at(service)->value(); }
void House::setServiceValue( Service::Type service, float value) { _d->services.at(service)->set( value ); }
unsigned int House::capacity()                                   { return _d->habitants.capacity; }
void House::appendServiceValue( Service::Type srvc, float value) { setServiceValue( srvc, getServiceValue( srvc ) + value ); }

const Desirability& House::desirability() const
{
  if( _d->habitants.empty() )
    return Desirability::invalid();
  else
    return _d->desirability;
}

std::string House::levelName() const
{
  std::string ret = spec().levelName();
  bool big = false;
  switch( level() )
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

void House::_updateHealthLevel()
{
  Services& s = _d->services;
  float delim = 1 + (((s[Service::well] > 0 || s[Service::fountain] > 0) ? 1 : 0)) //if we have water then decrease ill
      + ((s[Service::doctor] > 0 || s[Service::hospital] > 0) ? 1 : 0)             //doctor access also decrease ill
      + (s[Service::baths] > 0 ? 0.7 : 0)                                          //baths and barber some decrease ill
      + (s[Service::barber] > 0 ? 0.3 : 0);

  float decrease = 2.f / delim;

  updateState( pr::health, -decrease );

  if( state( pr::health ) < 25 && !_city()->getOption( PlayerCity::c3gameplay ))
  {
    HousePlague::create( _city(), pos(), game::Date::days2ticks( 5 ) );
  }
}

void House::Impl::initGoodStore(int size)
{
  int rsize = 25 * size * houseLevel;
  goodstore.setCapacity(rsize * 10 );  // no limit
  goodstore.setCapacity(good::wheat, rsize );
  goodstore.setCapacity(good::fish, rsize );
  goodstore.setCapacity(good::meat, rsize );
  goodstore.setCapacity(good::fruit, rsize );
  goodstore.setCapacity(good::vegetable, rsize );
  goodstore.setCapacity(good::pottery, rsize );
  goodstore.setCapacity(good::furniture, rsize);
  goodstore.setCapacity(good::oil, rsize );
  goodstore.setCapacity(good::wine, rsize );
}

void House::Impl::consumeServices()
{
  foreach( s, services )
    { s->second->consume( -1 ); } //consume services
}

void House::Impl::consumeGoods( HousePtr house )
{
  for( good::Product goodType = good::olive; goodType < good::any(); ++goodType)
  {
     int montlyGoodsQty = spec.computeMonthlyGoodConsumption( house, goodType, true );
     goodstore.setQty( goodType, std::max( goodstore.qty(goodType) - montlyGoodsQty, 0) );
  }
}

void House::Impl::consumeFoods(HousePtr house)
{
  const int foodLevel = spec.minFoodLevel();
  if( foodLevel == 0 )
    return;

  const int interval = spec.consumptionInterval( HouseSpecification::intv_foods );
  const int needFoodQty = spec.computeMonthlyFoodConsumption( house ) * interval / game::Date::days2ticks( 30 );

  int availableFoodLevel = 0;
  for( good::Product afl=good::wheat; afl <= good::vegetable; ++afl )
  {
    availableFoodLevel += ( goodstore.qty( afl ) > 0 ? 1 : 0 );
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
        int vQty = std::min( goodstore.qty( gType ), needFoodQty / availableFoodLevel );
        vQty = std::min( vQty, alsoNeedFood );
        if( vQty > 0 )
        {
          realConsumedQty += vQty;
          alsoNeedFood -= vQty;
          goodstore.setQty( gType, std::max( goodstore.qty( gType ) - vQty, 0) );
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
    Logger::warning( "House: [{0},{1}] have no food for habitants", house->pos().i(), house->pos().j() );
  }
}
