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

#ifndef __CAESARIA_SERVICEWALKERHELPER_H_INCLUDED__
#define __CAESARIA_SERVICEWALKERHELPER_H_INCLUDED__

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

    for( ServiceWalker::ReachedBuildings::iterator it=res.begin();it != res.end(); )
    {
      if( (*it)->type() != type )      {        res.erase( it++ );      }
      else                             {        ++it;      }
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

#endif //__CAESARIA_SERVICEWALKERHELPER_H_INCLUDED__
