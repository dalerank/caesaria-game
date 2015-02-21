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

#ifndef __CAESARIA_FLAGHOLDER_H_INCLUDE_
#define __CAESARIA_FLAGHOLDER_H_INCLUDE_

template< class T >
class FlagHolder 
{
public:
  FlagHolder() : _flags( 0 ) {}

  void setFlag( T flag, bool enabled=true )
  {
    if( enabled )
      _flags |= flag;
    else
      _flags &= ~flag;
  }

  void resetFlag( T flag )
  {
    setFlag( flag, false );
  }

  bool isFlag( T flag ) const
  {
    return (_flags & flag) == flag;
  }

  bool anyFlag( T flag ) const
  {
    return (_flags & flag) > 0;
  }

  void toggle( T flag )
  {
    setFlag( flag, !isFlag( flag ) );
  }

  void setFlags( int val )
  {
    _flags = val;
  }
private:
  int _flags;
};

#endif //__CAESARIA_FLAGHOLDER_H_INCLUDE_

