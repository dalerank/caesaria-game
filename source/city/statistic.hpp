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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_CITYSTATISTIC_H_INCLUDED__
#define __CAESARIA_CITYSTATISTIC_H_INCLUDED__

#include "core/scopedptr.hpp"
#include "core/variant.hpp"
#include "objects/overlay.hpp"
#include "good/productmap.hpp"
#include "predefinitions.hpp"
#include "festivaltype.hpp"
#include "objects/constants.hpp"
#include "game/citizen_group.hpp"
#include "game/service.hpp"
#include "gfx/tilemap_config.hpp"
#include "walker/walker.hpp"
#include "gfx/tilearea.hpp"
#include "industry.hpp"
#include "pathway/pathway_helper.hpp"
#include "city.hpp"

namespace city
{

class Statistic
{
public:
  Statistic( PlayerCity& rcity );
  void update( const unsigned long time );

  struct _Walkers
  {
    const WalkerList& find( walker::Type type ) const;

    template< class T >
    SmartList<T> find() const;

    template< class T >
    SmartPtr<T> find( walker::Type type, Walker::UniqueId id ) const;

    template<class T>
    Pathway freeTile( TilePos target, const TilePos& currentPos, const int range ) const;

    template<class T>
    SmartList< T > neighbors( TilePos start, walker::Type type=walker::any ) const;

    template< class T >
    SmartList< T > find( walker::Type type,
                         const TilePos& start=TilePos::invalid(),
                         const TilePos& stop=TilePos::invalid() ) const;

    template< class T >
    int count( TilePos start=TilePos::invalid(),
               TilePos stop=TilePos::invalid() ) const;

    int count( walker::Type type,
               const TilePos& start=TilePos::invalid(),
               const TilePos& stop=TilePos::invalid() ) const;


    Statistic& _parent;
    std::map<int, WalkerList> cached;
  } walkers;

  struct _Objects
  {
    template< class T >
    SmartList< T > find( std::set<object::Type> which ) const;

    template< class T >
    SmartList<T> find() const;

    template< class T >
    SmartList< T > find( object::Group group, const TilePos& start, const TilePos& stop );

    template< class T >
    SmartList<T> find( object::Group group ) const;

    template< class T >
    SmartList< T > find( object::Type type ) const;

    template< class T >
    size_t count() const;

    size_t count( object::Type type ) const;

    template< class T >
    SmartList< T > find( object::TypeSet types ) const;

    template< class T >
    SmartList< T > find( const object::Type type, const TilePos& start, const TilePos& stop ) const;

    template< class T >
    SmartList< T > find( const TilePos& center, int radius ) const;

    template< class T >
    SmartList< T > find( const object::Type type, const TilePos& center, int radius ) const;

    template< class T >
    SmartPtr< T > next( SmartPtr< T > current ) const;

    template<class T>
    SmartPtr<T> prew( SmartPtr<T> current) const;

    template<class T, class B>
    SmartList<T> neighbors( SmartPtr<B> overlay) const;

    template< class T >
    SmartList<T> producers(const good::Product goodtype) const;

    template< class T >
    SmartList< T > findNotIn( const std::set<object::Group>& which ) const;

    enum { maxLaborDistance=8 };

    OverlayList neighbors( OverlayPtr overlay, bool v ) const;
    OverlayList neighbors(const TilePos& pos ) const;
    FarmList farms(std::set<object::Type> which=std::set<object::Type>() ) const;
    int laborAccess( WorkingBuildingPtr wb ) const;    

    Statistic& _parent;
  } objects;

  struct _Tax
  {
    unsigned int possible() const;
    unsigned int payersPercent() const;
    Statistic& _parent;

    static const int minServiceValue=25;
  } tax;

  struct WorkersInfo
  {
    int current = 0;
    int need = 0;
    std::map<walker::Type,int> map;
  };

  struct _Workers
  {
    WorkersInfo details() const;
    size_t need() const;
    int wagesDiff() const;
    unsigned int monthlyWages() const;
    float monthlyOneWorkerWages() const;
    size_t available() const;
    unsigned int worklessPercent() const;
    unsigned int workless() const;
    HirePriorities hirePriorities() const;

    Statistic& _parent;
  } workers;

  struct _Population
  {
    CitizenGroup details() const;
    unsigned int current() const;

    Statistic& _parent;
  } population;

  struct _Food
  {
    size_t inGranaries() const;
    size_t monthlyConsumption() const;
    size_t possibleProducing() const;

    Statistic& _parent;
  } food;

  struct _Services
  {
    template<class T> SmartPtr<T> find() const;
    template<class Srvc> int value( int defaultValue = 0 ) const;

    Statistic& _parent;
  } services;

  struct _Festival
  {
    unsigned int calcCost( FestivalType type ) const;

    enum { greatFestivalCostLimiter=5,
           middleFestivalCostLimiter=10,
           smallFestivalMinCost=10,
           smallFestivalCostLimiter=20,
           middleFestivalMinCost=20,
           greatFestivalMinCost=40 };

    Statistic& _parent;
  } festival;

  struct _Crime
  {
    unsigned int level() const;

    Statistic& _parent;
  } crime;

  struct _Goods
  {
    good::ProductMap details(bool includeGranary) const;
    good::ProductMap inCity() const;
    good::ProductMap inWarehouses() const;

    bool canImport( good::Product type ) const;
    bool canProduce( good::Product type ) const;
    bool isRomeSend( good::Product type ) const;

    Statistic& _parent;
  } goods;

  struct _Health
  {
    unsigned int value() const;

    Statistic& _parent;
  } health;

  struct _Military
  {
    int months2lastAttack() const;
    FortList forts() const;

    Statistic& _parent;
  } military;

  struct _Map
  {
    gfx::TilesArray perimetr( const TilePos& lu, const TilePos& rb) const;

    template< class T >
    gfx::TilesArray around( T overlay ) const;

    template<class T>
    bool isTileBusy( const TilePos& p, WalkerPtr caller, bool& needMeMove ) const;

    void updateTilePics() const;

    Statistic& _parent;
  } map;

  struct _Houses
  {
    static const int pop4shacksCalc=300;
    static const int minShacksDesirability =-10;

    HouseList all() const;
    HouseList find( std::set<int> levels=std::set<int>() ) const;
    HouseList ready4evolve(const object::TypeSet& checkTypes) const;
    HouseList ready4evolve(const object::Type checkTypes) const;
    HouseList habitable() const;
    HouseList patricians(bool habitabl) const;
    HouseList plebs(bool habitabl) const;
    unsigned int terribleNumber() const;

    Statistic& _parent;
  } houses;

  struct _Religion
  {
    TempleList temples() const;
    TempleOracleList oracles() const;
    Statistic& _parent;
  } religion;


  struct _Entertainment
  {
    int coverage(Service::Type service) const;

    Statistic& _parent;
  } entertainment;

  struct _Education
  {
    EducationBuildingList find(Service::Type service) const;

    Statistic& _parent;
  } education;

  struct _Balance
  {
    float koeff() const;

    Statistic& _parent;
  } balance;

  PlayerCity& rcity;
};


/** Implementations **/
template< class T >
inline SmartList< T > Statistic::_Objects::find( const object::Type type, const TilePos& center, int radius ) const
{
  TilePos offset( radius, radius );
  return find<T>( type, center - offset, center + offset );
}

template< class T >
inline SmartList< T > Statistic::_Objects::find( const TilePos& center, int radius ) const
{
  TilePos offset( radius, radius );
  return find<T>( object::any, center - offset, center + offset );
}


template< class T >
inline SmartList< T > Statistic::_Objects::find( const object::Type type, const TilePos& start, const TilePos& stop ) const
{
  SmartList< T > ret;

  gfx::TilesArea area( _parent.rcity.tilemap(), start, stop );
  for( auto tile : area )
  {
    SmartPtr<T> obj = ptr_cast< T >( tile->overlay() );
    if( object::typeOrDefault( obj ) == type || type == object::any )
    {
      ret.addIfValid( obj );
    }
  }

  return ret;
}

template< class T >
inline SmartPtr<T> Statistic::_Walkers::find( walker::Type type, Walker::UniqueId id ) const
{
  const WalkerList& all = _parent.rcity.walkers();

  if( type != walker::any )
  {
    for( auto wlk : all )
    {
      if( wlk->type() == type && wlk->uniqueId() == id )
        return wlk.as<T>();
    }
  }
  else
  {
    for( auto wlk : all )
    {
      if( wlk->uniqueId() == id )
        return wlk.as<T>();
    }
  }

  return SmartPtr<T>();
}

template<class T>
inline Pathway Statistic::_Walkers::freeTile( TilePos target, const TilePos& currentPos, const int range ) const
{
  for( int currentRange=1; currentRange <= range; currentRange++ )
  {
    TilePos offset( currentRange, currentRange );
    gfx::TilesArray tiles = _parent.map.perimetr( currentPos - offset, currentPos + offset );
    tiles = tiles.walkables( true );

    float crntDistance = target.distanceFrom( currentPos );
    for( auto tile : tiles )
    {
      SmartList<T> eslist = _parent.rcity.walkers( tile->pos() ).select<T>();

      if( !eslist.empty() )
        continue;

      if( target.distanceFrom( tile->pos() ) > crntDistance )
        continue;

      Pathway pathway = PathwayHelper::create( currentPos, tile->pos(), PathwayHelper::allTerrain );
      if( pathway.isValid() )
      {
        return pathway;
      }
    }
  }

  return Pathway();
}

template< class T >
inline SmartList<T> Statistic::_Walkers::neighbors( TilePos start, walker::Type type ) const
{
  static TilePos offset( 1, 1 );
  return find<T>( type, start - offset, start + offset );
}

template< class T >
inline SmartList<T> Statistic::_Walkers::find( walker::Type type,
                                               const TilePos& start, const TilePos& stop ) const
{
  WalkerList walkersInArea;

  if( start == TilePos::invalid() )
  {
    const WalkerList& all =_parent.rcity.walkers();
    walkersInArea.insert( walkersInArea.end(), all.begin(), all.end() );
  }
  else if( stop == TilePos::invalid() )
  {
    const WalkerList& wlkOnTile = _parent.rcity.walkers( start );
    walkersInArea.insert( walkersInArea.end(), wlkOnTile.begin(), wlkOnTile.end() );
  }
  else
  {
    gfx::TilesArea area( _parent.rcity.tilemap(), start, stop );
    for( auto& tile : area)
    {
      const WalkerList& wlkOnTile = _parent.rcity.walkers( tile->pos() );
      walkersInArea.insert( walkersInArea.end(), wlkOnTile.begin(), wlkOnTile.end() );
    }
  }

  SmartList< T > result;
  for( auto& w : walkersInArea )
  {
    if( w->type() == type || type == walker::any )
      result.addIfValid( w.as<T>() );
  }

  return result;
}

template< class T >
inline int Statistic::_Walkers::count( TilePos start, TilePos stop ) const
{
  int result = 0;
  TilePos stopPos = stop;

  if( start == TilePos::invalid() )
  {
    const WalkerList& all =_parent.rcity.walkers();
    result = all.count<T>();
  }
  else if( stopPos == TilePos::invalid() )
  {
    const WalkerList& wlkOnTile = _parent.rcity.walkers( start );
    result = wlkOnTile.count<T>();
  }
  else
  {
    gfx::TilesArea area( _parent.rcity.tilemap(), start, stop );
    for( auto tile : area)
    {
      const WalkerList& wlkOnTile = _parent.rcity.walkers( tile->pos() );
      result += wlkOnTile.count<T>();
    }
  }

  return result;
}

template< class T >
inline SmartList< T > Statistic::_Objects::find( std::set<object::Type> which ) const
{
  SmartList< T > ret;
  auto ovs = _parent.rcity.overlays();

  for( auto ov : ovs )
  {
    if( which.count( ov->type() ) > 0 )
    {
      ret << ov;
    }
  }

  return ret;
}

template< class T >
inline SmartList<T> Statistic::_Objects::find() const
{
  return _parent.rcity.overlays().select<T>();
}

template< class T >
inline SmartList< T > Statistic::_Objects::find( object::Group group, const TilePos& start, const TilePos& stop )
{
  SmartList< T > ret;

  gfx::TilesArea area( _parent.rcity.tilemap(), start, stop );

  for( auto tile : area )
  {
    SmartPtr<T> obj = tile->overlay<T>();
    if( obj.isValid() && (obj->getClass() == group || group == object::group::any ) )
    {
      ret.push_back( obj );
    }
  }

  return ret;
}

template< class T >
inline SmartList<T> Statistic::_Objects::find( object::Group group ) const
{
  SmartList<T> ret;
  auto buildings = _parent.rcity.overlays();
  for( auto item : buildings )
  {
    SmartPtr<T> b = item.as<T>();
    if( b.isValid() && (b->group() == group || group == object::group::any ) )
    {
      ret.push_back( b );
    }
  }

  return ret;
}

template< class T >
inline SmartList< T > Statistic::_Objects::find( object::TypeSet types ) const
{
  SmartList< T > ret;
  auto buildings = _parent.rcity.overlays();
  for( auto bld : buildings )
  {
    if( bld.isValid() && types.count( bld->type() ) > 0 )
      ret.addIfValid( bld.as<T>() );
  }

  return ret;
}

template< class T >
inline size_t Statistic::_Objects::count() const
{
  size_t result = 0;
  auto buildings = _parent.rcity.overlays();
  for( auto bld : buildings )
  {
    if( is_kind_of<T>( bld ) )
      result++;
  }

  return result;
}

template< class T >
inline SmartList< T > Statistic::_Objects::find( object::Type type ) const
{
  SmartList< T > ret;
  auto buildings = _parent.rcity.overlays();
  for( auto bld : buildings )
  {
    if( object::typeOrDefault( bld ) == type || type == object::any )
      ret.addIfValid( bld.as<T>() );
  }

  return ret;
}

template<class T>
inline SmartPtr<T> Statistic::_Objects::prew( SmartPtr<T> current) const
{
  if( current.isNull() )
    return SmartPtr<T>();

  SmartList< T > objects = find<T>( current->type() );
  foreach( obj, objects )
  {
    if( current == *obj )
    {
      if (obj == objects.begin()) // MSVC compiler doesn't support circular lists. Neither standart does.
      {
        obj = objects.end();
      }
      obj--;
      return *obj;
    }
  }

  return SmartPtr<T>();
}

template< class T >
inline SmartPtr< T > Statistic::_Objects::next( SmartPtr< T > current ) const
{
  if( current.isNull() )
    return SmartPtr<T>();

  SmartList< T > objects = find<T>( current->type() );
  foreach( obj, objects )
  {
    if( current == *obj )
    {
      obj++;
      if( obj == objects.end() ) { return *objects.begin(); }
      else { return *obj; }
    }
  }

  return SmartPtr<T>();
}

template<class T>
inline SmartPtr<T> Statistic::_Services::find() const
{
  return ptr_cast<T>( _parent.rcity.findService( T::defaultName() ) );
}

template<class Srvc>
int Statistic::_Services::value( int defaultValue ) const
{
  auto ptr = find<Srvc>();
  return ptr.isValid() ? ptr->value() : defaultValue;
}

template< class T >
SmartList<T> Statistic::_Objects::producers(const good::Product goodtype) const
{
  SmartList< T > ret;
  const OverlayList& overlays = _parent.rcity.overlays();
  for( auto ov : overlays )
  {
    SmartPtr< T > b = ov.as<T>();
    if( b.isValid() && b->produceGoodType() == goodtype )
    {
      ret.push_back( b );
    }
  }

  return ret;
}

template<class T, class B>
SmartList<T> Statistic::_Objects::neighbors( SmartPtr<B> overlay ) const
{
  OverlayList overlays = neighbors( ptr_cast<Overlay>( overlay ), true );
  return overlays.select<T>();
}

template< class T >
inline SmartList< T > Statistic::_Objects::findNotIn( const std::set<object::Group>& which ) const
{
  SmartList< T > ret;
  auto& overlays = _parent.rcity.overlays();

  for( auto ov : overlays )
  {
    if( which.count( ov->group() ) == 0 )
    {
      ret.addIfValid( ov.as<T>() );
    }
  }

  return ret;
}

template<class T>
inline bool Statistic::_Map::isTileBusy( const TilePos& p, WalkerPtr caller, bool& needMeMove ) const
{
  needMeMove = false;
  auto walkers = _parent.rcity.walkers( p ).select<T>();

  if( !walkers.empty() )
  {
    needMeMove = (caller.object() != walkers.front().object());
  }

  return walkers.size() > 1;
}

template< class T >
inline gfx::TilesArray Statistic::_Map::around( T overlay ) const
{
  TilePos start = overlay->pos() - config::tilemap.unitLocation();
  TilePos stop = start + TilePos( overlay->size().width(), overlay->size().height() );
  return perimetr( start, stop );
}

template<class T>
inline SmartList<T> Statistic::_Walkers::find() const
{
  return _parent.rcity.walkers().select<T>();
}

}//end namespace city

#endif //__CAESARIA_CITYSTATISTIC_H_INCLUDED__
