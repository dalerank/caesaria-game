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

#include "playerarmy.hpp"
#include "empire.hpp"
#include "good/goodstore_simple.hpp"
#include "game/resourcegroup.hpp"
#include "core/logger.hpp"
#include "merchant.hpp"
#include "gfx/animation.hpp"
#include "core/variant_map.hpp"
#include "city.hpp"
#include "city/city.hpp"
#include "objects/fort.hpp"
#include "game/gamedate.hpp"
#include "core/utils.hpp"
#include "walker/walkers_factory.hpp"

using namespace gfx;

namespace world
{

struct SoldierInfo
{
int strike;
int resistance;
VariantMap save;
};

class PlayerArmy::Impl
{
public:  
  typedef std::vector<SoldierInfo> SoldiersInfo;

  PlayerArmy::Mode mode;
  RomeSoldierList waitSoldiers;
  SoldiersInfo soldiersInfo;
  TilePos fortPos;
  CityPtr base;

public:
  void updateStrength();
};

PlayerArmyPtr PlayerArmy::create(EmpirePtr empire, CityPtr city)
{
  PlayerArmyPtr ret( new PlayerArmy( empire ) );
  ret->_d->base = city;
  ret->setLocation( city->location() - Point( 0, 10 ));
  ret->drop();

  return ret;
}

std::string PlayerArmy::type() const { return CAESARIA_STR_EXT(PlayerArmy); }

void PlayerArmy::timeStep(const unsigned int time)
{
  if( _d->mode == PlayerArmy::wait )
  {
    if( game::Date::isDayChanged() )
    {
      for( RomeSoldierList::iterator it=_d->waitSoldiers.begin(); it != _d->waitSoldiers.end(); )
      {
        if( (*it)->isDeleted() )
        {
          SoldierInfo sInfo;
          (*it)->save( sInfo.save );
          sInfo.strike = (*it)->strike();
          sInfo.resistance = (*it)->resistance();

          _d->soldiersInfo.push_back( sInfo );

          it = _d->waitSoldiers.erase( it );

          _d->updateStrength();
        }
        else { ++it; }
      }

      if( _d->waitSoldiers.empty() )
      {
        _d->mode = PlayerArmy::go2location;
      }
    }
  }
  else
  {
    MovableObject::timeStep( time );
  }
}

void PlayerArmy::move2location(Point point)
{
  bool validWay = _findWay( location(), point);
  if( !validWay )
  {
    Logger::warning( "PlayerArmy: cant find way to point [%d,%d]", point.x(), point.y()  );
    deleteLater();
  }

  _d->mode = PlayerArmy::go2location;
}

void PlayerArmy::setFortPos(const TilePos& base)
{
  _d->fortPos = base;
  setName( utils::format( 0xff, "expedition_from_%dx%d", base.i(), base.j() ) );
}

void PlayerArmy::return2fort()
{
  if( _d->base.isValid() )
  {
    bool validWay = _findWay( location(), _d->base->location() );
    if( !validWay )
    {
      Logger::warning( "PlayerArmy: cant find way to base" );
      deleteLater();
    }

    _d->mode = PlayerArmy::go2home;
  }
  else
  {
    Logger::warning( "PlayerArmy: base is null delete army" );
    deleteLater();
  }
}

void PlayerArmy::save(VariantMap& stream) const
{
  MovableObject::save( stream );
}

void PlayerArmy::load(const VariantMap& stream)
{
  MovableObject::load( stream );
}

void PlayerArmy::killSoldiers(int percent)
{
  if( percent >= 100 )
  {
    _d->soldiersInfo.clear();
    deleteLater();
  }

  int curStrength = strength();
  int finishStrength = curStrength * percent / 100;
  while( !_d->soldiersInfo.empty() && curStrength > finishStrength )
  {
    _d->soldiersInfo.erase( _d->soldiersInfo.begin() );
    curStrength = strength();
  }
}

PlayerArmy::Mode PlayerArmy::mode() const { return _d->mode; }

int PlayerArmy::viewDistance() const { return 30; }

void PlayerArmy::addSoldiers(RomeSoldierList soldiers)
{
  _d->mode = PlayerArmy::wait;
  _d->waitSoldiers.insert( _d->waitSoldiers.end(), soldiers.begin(), soldiers.end() );
}

bool PlayerArmy::_isAgressiveArmy(ArmyPtr other) const
{
  return !is_kind_of<PlayerArmy>( other );
}

void PlayerArmy::_check4attack()
{
  MovableObjectList mobjects;
  mobjects << empire()->objects();
  mobjects.remove( this );

  std::map< int, MovableObjectPtr > distanceMap;

  foreach( it, mobjects )
  {
    float distance = location().distanceTo( (*it)->location() );    
    distanceMap[ (int)distance ] = *it;
  }

  foreach( it, distanceMap )
  {
    if( it->first < 20 )
    {
      _attackObject( ptr_cast<Object>( it->second ) );
      break;
    }
    else if( it->first < viewDistance() )
    {
      bool validWay = _findWay( location(), it->second->location() );
      if( validWay )
      {
        _d->mode = PlayerArmy::go2location;
        break;
      }
    }
  }

  if( _way().empty() )
  {
     CityList cities = empire()->cities();
     std::map< int, CityPtr > citymap;

     DateTime currentDate = game::Date::current();
     foreach( it, cities )
     {
       float distance = location().distanceTo( (*it)->location() );
       int month2lastAttack = math::clamp<int>( 12 - (*it)->lastAttack().monthsTo( currentDate ), 0, 12 );
       citymap[ month2lastAttack * 100 + (int)distance ] = *it;
     }

     foreach( it, citymap )
     {
       bool validWay = _findWay( location(), it->second->location() );
       if( validWay )
       {
         _d->mode = PlayerArmy::go2location;
         break;
       }
     }
  }
}

void PlayerArmy::_noWay()
{
  return2fort();
}

void PlayerArmy::_reachedWay()
{
  if( _d->mode == PlayerArmy::go2location )
  {
    _attackAny();
    return2fort();
  }
  else if( _d->mode == PlayerArmy::go2home )
  {
    PlayerCityPtr pCity = ptr_cast<PlayerCity>( _d->base );
    if( pCity.isValid() )
    {
      foreach( it, _d->soldiersInfo )
      {
        int type = (*it).save[ "type" ];
        WalkerPtr walker = WalkerManager::instance().create( (constants::walker::Type)type, pCity );
        walker->load( (*it).save );
        pCity->addWalker( walker );
      }

      FortPtr fort = ptr_cast<Fort>( pCity->getOverlay( _d->fortPos ) );
      if( fort.isValid() )
      {
        fort->returnSoldiers();
        fort->resetExpedition();
      }
    }

    deleteLater();
  }
}

void PlayerArmy::_attackAny()
{
  ObjectList objs = empire()->findObjects( location(), 20 );
  objs.remove( this );

  bool successAttack = false;
  foreach( i, objs )
  {
    successAttack = _attackObject( *i );
    if( successAttack )
      break;
  }
}

bool PlayerArmy::_attackObject(ObjectPtr obj)
{
  /*if( is_kind_of<Merchant>( obj ) )
  {
    obj->deleteLater();
    return true;
  }
  else if( is_kind_of<City>( obj ) )
  {
    CityPtr pcity = ptr_cast<City>( obj );

    pcity->addObject( this );

    return !pcity->strength();
  }*/

  return false;
}

PlayerArmy::PlayerArmy( EmpirePtr empire )
 : Army( empire ), _d( new Impl )
{
  _d->mode = PlayerArmy::wait;
  setSpeed( 4.f );

  Picture pic = Picture::load( ResourceGroup::empirebits, 37 );
  Size size = pic.size();
  pic.setOffset( Point( -size.width() / 2, size.height() / 2 ) );
  setPicture( pic );

  _animation().clear();
  _animation().load( ResourceGroup::empirebits, 72, 6 );
  _animation().setLoop( gfx::Animation::loopAnimation );
  _animation().setOffset( Point( 5, -10 ) );
}

void PlayerArmy::Impl::updateStrength()
{
  unsigned int result = 0;
  foreach( it, soldiersInfo )
  {
    result += (*it).strike;
  }
}


}
