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

#include "computer_city.hpp"
#include "empire.hpp"
#include "trading.hpp"
#include "good/goodstore_simple.hpp"
#include "good/goodhelper.hpp"
#include "game/gamedate.hpp"
#include "core/foreach.hpp"
#include "merchant.hpp"
#include "city/funds.hpp"
#include "game/resourcegroup.hpp"
#include "empiremap.hpp"

using namespace gfx;

namespace world
{

namespace {
CAESARIA_LITERALCONST(location)
CAESARIA_LITERALCONST(distant)
}

class ComputerCity::Impl
{
public:
  Point location;
  std::string name;
  unsigned int tradeType;
  bool distantCity, romeCity;
  bool isAvailable;
  unsigned int tradeDelay;
  SimpleGoodStore sellStore;
  SimpleGoodStore buyStore;
  SimpleGoodStore realSells;
  DateTime lastTimeUpdate;
  DateTime lastTimeMerchantSend;
  unsigned int merchantsNumber;
  Picture empMapPicture;
  city::Funds funds;

public:
  void initPicture();
};

ComputerCity::ComputerCity( EmpirePtr empire, const std::string& name )
  : City( empire ), _d( new Impl )
{
  _d->name = name;
  _d->tradeDelay = 0;
  _d->distantCity = false;
  _d->merchantsNumber = 0;
  _d->isAvailable = true;
  _d->sellStore.setCapacity( 99999 );
  _d->buyStore.setCapacity( 99999 );
  _d->realSells.setCapacity( 99999 );
  _d->romeCity = false;

  _d->initPicture();
}

bool ComputerCity::_mayTrade() const { return _d->tradeDelay <= 0; }
std::string ComputerCity::name() const {  return _d->name;}
Point ComputerCity::location() const{  return _d->location;}
void ComputerCity::setLocation( const Point& location ){  _d->location = location;}
city::Funds&ComputerCity::funds() { return _d->funds; }
unsigned int ComputerCity::population() const { return 0; }
bool ComputerCity::isPaysTaxes() const { return true; }
bool ComputerCity::haveOverduePayment() const { return false; }
bool ComputerCity::isDistantCity() const{  return _d->distantCity;}
bool ComputerCity::isRomeCity() const{  return _d->romeCity;}
bool ComputerCity::isAvailable() const{  return _d->isAvailable;}
void ComputerCity::setAvailable(bool value){  _d->isAvailable = value;}

void ComputerCity::save( VariantMap& options ) const
{
  options[ lc_location ] = _d->location;

  VariantMap vm_sells;
  VariantMap vm_sold;
  VariantMap vm_buys;
  VariantMap vm_bought;

  for( int i=Good::none; i < Good::goodCount; i ++ )
  {
    Good::Type gtype = Good::Type ( i );
    std::string tname = GoodHelper::getTypeName( gtype );
    int maxSellStock = _d->sellStore.capacity( gtype );
    if( maxSellStock > 0 )
    {
      vm_sells[ tname ] = maxSellStock / 100;
    }

    int sold = _d->sellStore.qty( gtype );
    if( sold > 0 )
    {
      vm_sold[ tname ] = sold / 100;
    }

    int maxBuyStock = _d->buyStore.capacity( gtype );
    if( maxBuyStock > 0 )
    {
      vm_buys[ tname ] = maxBuyStock / 100;
    }

    int bought = _d->buyStore.qty( gtype );
    if( bought > 0 )
    {
      vm_bought[ tname ] = bought / 100;
    }
  }

  options[ "sells" ] = vm_sells;
  options[ "buys" ] = vm_buys;
  options[ "sold" ] = vm_sold;
  options[ "bought" ] = vm_bought;
  options[ "lastTimeMerchantSend" ] = _d->lastTimeMerchantSend;
  options[ "lastTimeUpdate" ] = _d->lastTimeUpdate;
  options[ "available" ] = _d->isAvailable;
  options[ "merchantsNumber" ] = _d->merchantsNumber;
  options[ "sea" ] = (_d->tradeType & EmpireMap::sea ? true : false);
  options[ "land" ] = (_d->tradeType & EmpireMap::land ? true : false);
  options[ lc_distant ] = _d->distantCity;
  options[ "romecity" ] = _d->romeCity;
  options[ "realSells" ] = _d->realSells.save();
  options[ "tradeDelay" ] = _d->tradeDelay;
}

void ComputerCity::load( const VariantMap& options )
{
  Variant v_location = options.get( lc_location );
  if( v_location.isValid() )
    setLocation( v_location.toPoint() );

  _d->isAvailable = (bool)options.get( "available", false );
  _d->lastTimeUpdate = options.get( "lastTimeUpdate", GameDate::current() ).toDateTime();
  _d->lastTimeMerchantSend = options.get( "lastTimeMerchantSend", GameDate::current() ).toDateTime();
  _d->merchantsNumber = (int)options.get( "merchantsNumber" );
  _d->distantCity = (bool)options.get( lc_distant );
  _d->romeCity = (bool)options.get( "romecity" );
  _d->tradeDelay = (int)options.get( "tradeDelay" );

  for( int i=Good::none; i < Good::goodCount; i ++ )
  {
    Good::Type gtype = Good::Type ( i );
    _d->sellStore.setCapacity( gtype, 0 );
    _d->buyStore.setCapacity( gtype, 0 );
    _d->realSells.setCapacity( gtype, 0 );
  }

  changeTradeOptions( options );

  const VariantMap& sold_vm = options.get( "sold" ).toMap();
  for( VariantMap::const_iterator it=sold_vm.begin(); it != sold_vm.end(); ++it )
  {
    Good::Type gtype = GoodHelper::getType( it->first );
    _d->sellStore.setQty( gtype, it->second.toInt() * 100 );
  }

  const VariantMap& bought_vm = options.get( "bought" ).toMap();
  for( VariantMap::const_iterator it=bought_vm.begin(); it != bought_vm.end(); ++it )
  {
    Good::Type gtype = GoodHelper::getType( it->first );
    _d->buyStore.setQty( gtype, it->second.toInt() * 100 );
  }

  _d->tradeType = (options.get( "sea" ).toBool() ? EmpireMap::sea : EmpireMap::unknown)
                  + (options.get( "land" ).toBool() ? EmpireMap::land : EmpireMap::unknown);

  Variant vm_rsold = options.get( "realSells" );

  if( vm_rsold.isValid() )
  {
    _d->realSells.load( vm_rsold.toMap() );
  }

  _d->initPicture();
}

const GoodStore& ComputerCity::importingGoods() const {  return _d->realSells;}
const GoodStore& ComputerCity::exportingGoods() const{  return _d->buyStore;}
void ComputerCity::delayTrade(unsigned int month){  _d->tradeDelay = month;}

void ComputerCity::empirePricesChanged(Good::Type gtype, int bCost, int sCost)
{
}

CityPtr ComputerCity::create( EmpirePtr empire, const std::string& name )
{
  CityPtr ret( new ComputerCity( empire, name ) );
  ret->drop();

  return ret;
}

void ComputerCity::addObject(ObjectPtr object )
{
  MerchantPtr merchant = ptr_cast<Merchant>( object );
  if( merchant.isValid() )
  {
    GoodStore& sellGoods = merchant->getSellGoods();
    GoodStore& buyGoods = merchant->getBuyGoods();

    _d->buyStore.storeAll( buyGoods );

    for( int i=Good::none; i < Good::goodCount; i ++ )
    {
      Good::Type gtype = Good::Type ( i );
      int qty = sellGoods.freeQty( gtype );
      GoodStock stock( gtype, qty, qty );
      _d->realSells.store( stock, qty );
    }

    _d->sellStore.storeAll( sellGoods );

    _d->merchantsNumber = std::max<int>( 0, _d->merchantsNumber-1);
  }
}

void ComputerCity::changeTradeOptions(const VariantMap& stream)
{
  VariantMap sells_vm = stream.get( "sells" ).toMap();
  foreach( it, sells_vm )
  {
    Good::Type gtype = GoodHelper::getType( it->first );
    _d->sellStore.setCapacity( gtype, it->second.toInt() * 100 );
    _d->realSells.setCapacity( gtype, it->second.toInt() * 100 );
  }

  VariantMap buys_vm = stream.get( "buys" ).toMap();
  foreach( it, buys_vm )
  {
    Good::Type gtype = GoodHelper::getType( it->first );
    _d->buyStore.setCapacity( gtype, it->second.toInt() * 100 );
  }
}

ComputerCity::~ComputerCity() {}

void ComputerCity::timeStep( unsigned int time )
{
  if( GameDate::isMonthChanged() )
  {
    _d->tradeDelay = math::clamp<int>( _d->tradeDelay-1, 0, 99 );
  }

  //one year before step need
  if( _d->lastTimeUpdate.monthsTo( GameDate::current() ) > 11 )
  {
    _d->merchantsNumber = math::clamp<unsigned int>( _d->merchantsNumber-1, 0, 2 );
    _d->lastTimeUpdate = GameDate::current();

    for( int i=Good::none; i < Good::goodCount; i ++ )
    {
      Good::Type gtype = Good::Type( i );
      _d->sellStore.setQty( gtype, _d->sellStore.capacity( gtype ) );     
      _d->buyStore.setQty( gtype, 0  );
      _d->realSells.setQty( gtype, 0 );
    }
  }

  if( _d->lastTimeMerchantSend.monthsTo( GameDate::current() ) > 2 ) 
  {
    TraderouteList routes = empire()->tradeRoutes( name() );
    _d->lastTimeMerchantSend = GameDate::current();

    if( _d->merchantsNumber >= routes.size() || !_mayTrade() )
    {
      return;
    }

    if( !routes.empty() )
    {
      SimpleGoodStore sellGoods, buyGoods;
      sellGoods.setCapacity( 2000 );
      buyGoods.setCapacity( 2000 );
      for( int i=Good::none; i < Good::goodCount; i ++ )
      {
        Good::Type gtype = Good::Type( i );

        buyGoods.setCapacity( gtype, _d->buyStore.capacity( gtype ) );

        //how much space left
        int maxQty = (std::min)( _d->sellStore.capacity( gtype ) / 4, sellGoods.freeQty() );
        
        //we want send merchants to all routes
        maxQty /= routes.size();

        int qty = math::clamp( _d->sellStore.qty( gtype ), 0, maxQty );

        //have no goods to sell
        if( qty == 0 )
          continue;

        GoodStock& stock = sellGoods.getStock( gtype );  
        stock.setCapacity( qty );
        
        //move goods to merchant's storage
        _d->sellStore.retrieve( stock, qty );
      }

      //send merchants to all routes
      foreach( route, routes )
      {
        _d->merchantsNumber++;
        (*route)->addMerchant( name(), sellGoods, buyGoods );
      }
    }
  }
}

gfx::Picture ComputerCity::picture() const { return _d->empMapPicture; }
unsigned int ComputerCity::tradeType() const { return _d->tradeType; }

void ComputerCity::Impl::initPicture()
{
  int index = 15;

  if( distantCity ) { index = 22; }
  else if( romeCity ) { index = 8; }

  empMapPicture = Picture::load( ResourceGroup::empirebits, index );
}

}//end namespace world
