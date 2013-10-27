#ifndef __OPENCAESAR3_SERVICEWALKERHELPER_H_INCLUDED__
#define __OPENCAESAR3_SERVICEWALKERHELPER_H_INCLUDED__

#include "serviceman.hpp"

class ServiceWalkerHelper
{
public:
  ServiceWalkerHelper( ServiceWalker& walker )
    : _walker( walker )
  {

  }

  ServiceWalker::ReachedBuildings getReachedBuildings( const TilePos& pos, const TileOverlayType type )
  {
    ServiceWalker::ReachedBuildings res = _walker.getReachedBuildings( pos );

    ServiceWalker::ReachedBuildings::iterator it=res.begin();
    while( it != res.end() )
    {
      if( (*it)->getType() != type )
      {
        res.erase( it++ );
      }
      else
      {
        it++;
      }
    }

    return res;
  }

  template< class T > 
  std::set< SmartPtr< T > > getReachedBuildings( const TilePos& pos )
  {
    ServiceWalker::ReachedBuildings buildings = _walker.getReachedBuildings( pos );

    std::set< SmartPtr< T > > ret;        
    for( ServiceWalker::ReachedBuildings::iterator it=buildings.begin(); it != buildings.end(); it++ )
    {
      SmartPtr< T > building = (*it).as<T>();
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
