#ifndef __OPENCAESAR3_SERVICEWALKERHELPER_H_INCLUDED__
#define __OPENCAESAR3_SERVICEWALKERHELPER_H_INCLUDED__

#include "serviceman.hpp"
#include "core/foreach.hpp"

class ServiceWalkerHelper
{
public:
  ServiceWalkerHelper( ServiceWalker& walker )
    : _walker( walker )
  {

  }

  ServiceWalker::ReachedBuildings getReachedBuildings( const TilePos& pos, const gfx::TileOverlay::Type type )
  {
    ServiceWalker::ReachedBuildings res = _walker.getReachedBuildings( pos );

    ServiceWalker::ReachedBuildings::iterator it=res.begin();
    while( it != res.end() )
    {
      if( (*it)->type() != type )      {        res.erase( it++ );      }
      else      {        ++it;      }
    }

    return res;
  }

  template< class T > 
  std::set< SmartPtr< T > > getReachedBuildings( const TilePos& pos )
  {
    ServiceWalker::ReachedBuildings buildings = _walker.getReachedBuildings( pos );

    std::set< SmartPtr< T > > ret;        
    foreach( it, buildings )
    {
      SmartPtr< T > building = ptr_cast<T>(*it);
      if( building.isValid() )
      {
        ret.insert( building );
      }
    }

    return ret;
  }

private:
  ServiceWalker& _walker;
};

#endif
