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

#ifndef __CAESARIA_SMARTLIST_H_INCLUDE__
#define __CAESARIA_SMARTLIST_H_INCLUDE__

#include "smartptr.hpp"
#include "foreach.hpp"
#include <list>

template <class T>
class SmartList : public std::list< SmartPtr< T > >
{
public:
  template< class Src >
  SmartList& operator<<( const SmartList<Src>& srcList )
  {
    foreach( it, srcList )
    {
      SmartPtr<T> ptr = ptr_cast<T>( *it );
      if( ptr.isValid() )
          this->push_back( ptr );
    }

    return *this;
  }

  template< class W >
  SmartList exclude()
  {
    SmartList<T> ret;
    foreach( it, *this )
    {
      SmartPtr<W> ptr = ptr_cast<W>( *it );
      if( ptr.isNull() )
      {
        ret.push_back( *it );
      }
    }

    return ret;
  }
};

#endif //__CAESARIA_SMARTLIST_H_INCLUDE__
