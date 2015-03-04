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

#ifndef __CAESARIA_SIGNAL_H_INCLUDE_
#define __CAESARIA_SIGNAL_H_INCLUDE_

#include "delegate.hpp"
#include "list.hpp"
#include "foreach.hpp"

#define signals
#define slots
#define emit

#define CONNECT( a, signal, b, slot ) \
{ \
	if( (a!=0) && (b!=0) ) { (a)->signal.connect( (b), &slot ); } \
	else if( (a==0) && (b==0) ) { Logger::warning( "Cannot connect null::%s to null::%s at %d:%s", CAESARIA_STR_A(signal), CAESARIA_STR_A(slot), __LINE__, __FILE__); } \
	else if( (b==0) ) { Logger::warning( "Cannot connect %s::%s to null::%s at %d:%s", CAESARIA_STR_A(a), CAESARIA_STR_A(signal), CAESARIA_STR_A(slot), __LINE__, __FILE__); }\
	else if( (a==0) ) { Logger::warning( "Cannot connect null::%s to %s::%s at %d:%s", CAESARIA_STR_A(signal), CAESARIA_STR_A(b), CAESARIA_STR_A(slot), __LINE__, __FILE__); }\
}

template< class Param0 = void >
class Signal0
{
public:
  typedef Delegate0< void > _Delegate;

private:
  typedef List<_Delegate> DelegateList;
  DelegateList delegateList;

public:
  void connect( _Delegate delegate ) { delegateList.push_back( delegate ); }

  template< class X, class Y >
  void connect( Y * obj, void (X::*func)() )
  {
    delegateList.push_back( makeDelegate( obj, func ) );
  }

  template< class X, class Y >
  void connect( Y * obj, void (X::*func)() const )
  {
    delegateList.push_back( makeDelegate( obj, func ) );
  }

  void disconnect( _Delegate delegate )
  {
    foreach( it, delegateList )
      if( *it == delegate )
      {
        delegateList.erase( it );
        return;
      }
  }

  template< class X, class Y >
  void disconnect( Y * obj, void (X::*func)() )
  {
    _Delegate& eq = makeDelegate( obj, func );
    foreach(it, delegateList)
      if( *it == eq )
        { delegateList.erase( it ); return; }
  }

  template< class X, class Y >
  void disconnect( Y * obj, void (X::*func)() const )
  {
    _Delegate& eq = makeDelegate( obj, func );
    foreach(it, delegateList)
      if( *it == eq )
        { delegateList.erase( it ); return; }
  }

  void _emit() const { foreach( it, delegateList ) (*it)(); }
  void operator() () const { _emit(); }
};


template< class Param1 >
class Signal1
{
public:
  typedef Delegate1< Param1 > _Delegate;

private:
  typedef List<_Delegate> DelegateList;
  DelegateList delegateList;

public:
  void connect( _Delegate delegate ) { delegateList.push_back( delegate ); }

  template< class X, class Y >
  void connect( Y * obj, void (X::*func)( Param1 p1 ) )
  {
    delegateList.push_back( makeDelegate( obj, func ) );
  }

  template< class X, class Y >
  void connect( Y * obj, void (X::*func)( Param1 p1 ) const )
  {
    delegateList.push_back( makeDelegate( obj, func ) );
  }

  void disconnectAll() { delegateList.clear(); }

  void disconnect( _Delegate delegate )
  {
    foreach( it, delegateList )
      if( *it == delegate )
      { delegateList.erase( it ); return; }
  }

  template< class X, class Y >
  void disconnect( Y * obj, void (X::*func)( Param1 p1 ) )
  {
    _Delegate& eq = makeDelegate( obj, func );
    foreach( it, delegateList)
      if( *it == eq )
        { delegateList.erase( it ); return; }
  }

  template< class X, class Y >
  void disconnect( Y * obj, void (X::*func)( Param1 p1 ) const )
  {
    _Delegate& eq = makeDelegate( obj, func );
    foreach( it, delegateList )
      if( *it == eq )
        { delegateList.erase( it ); return; }
  }

  void _emit( Param1 p1 ) const { foreach( it, delegateList ) (*it)( p1 ); }
  void operator() ( Param1 p1 ) const { _emit( p1 ); }
};


template< class Param1, class Param2 >
class Signal2
{
public:
  typedef Delegate2< Param1, Param2 > _Delegate;

private:
  typedef List<_Delegate> DelegateList;
  DelegateList delegates;

public:
  void connect( _Delegate delegate )  {    delegates.push_back( delegate );  }

  template< class X, class Y >
  void connect( Y * obj, void (X::*func)( Param1 p1, Param2 p2 ) )
  {
    delegates.push_back( makeDelegate( obj, func ) );
  }

  template< class X, class Y >
  void connect( Y * obj, void (X::*func)( Param1 p1, Param2 p2 ) const )
  {
    delegates.push_back( makeDelegate( obj, func ) );
  }

  void disconnect( _Delegate delegate )
  {
    foreach(it, delegates )
      if( *it == delegate )
        { delegates.erase( it ); return; }
  }

  template< class X, class Y >
  void disconnect( Y * obj, void (X::*func)( Param1 p1, Param2 p2 ) )
  {
    _Delegate& eq = makeDelegate( obj, func );
    foreach( it, delegates )
      if( *it == eq )
        { delegates.erase( it ); return; }
  }

  template< class X, class Y >
  void disconnect( Y * obj, void (X::*func)( Param1 p1, Param2 p2 ) const )
  {
    _Delegate& eq = makeDelegate( obj, func );
    foreach( it, delegates)
      if( *it == eq )
       { delegates.erase( it ); return; }
  }

  void _emit( Param1 p1, Param2 p2 ) const { foreach(it, delegates ) (*it)( p1, p2 ); }
  void operator() ( Param1 p1, Param2 p2 ) const { _emit( p1, p2 ); }
};


template< class Param1, class Param2, class Param3 >
class Signal3
{
public:
	typedef Delegate3< Param1, Param2, Param3 > _Delegate;

private:
	typedef List<_Delegate> DelegateList;
	typedef typename DelegateList::const_iterator DelegateIterator;
	DelegateList delegateList;

public:
	void connect( _Delegate delegate )
	{
		delegateList.push_back( delegate );
	}

	template< class X, class Y >
	void connect( Y * obj, void (X::*func)( Param1 p1, Param2 p2, Param3 p3 ) )
	{
		delegateList.push_back( makeDelegate( obj, func ) );
	}

	template< class X, class Y >
	void connect( Y * obj, void (X::*func)( Param1 p1, Param2 p2, Param3 p3 ) const )
	{
		delegateList.push_back( makeDelegate( obj, func ) );
	}

	void disconnect( _Delegate delegate )
	{
		for (DelegateIterator it = delegateList.begin(); it != delegateList.end(); ++it)
			if( *it == delegate )
			{
				delegateList.erase( it );
				return;
			}	
	}

	template< class X, class Y >
	void disconnect( Y * obj, void (X::*func)( Param1 p1, Param2 p2, Param3 p3 ) )
	{
		_Delegate& eq = makeDelegate( obj, func );
		for (DelegateIterator it = delegateList.begin(); it != delegateList.end(); ++it)
			if( *it == eq )
			{
				delegateList.erase( it );
				return;
			}
	}

	template< class X, class Y >
	void disconnect( Y * obj, void (X::*func)( Param1 p1, Param2 p2, Param3 p3 ) const )
	{
		_Delegate& eq = makeDelegate( obj, func );
		for (DelegateIterator it = delegateList.begin(); it != delegateList.end(); ++it)
			if( *it == eq )
			{
				delegateList.erase( it );
				return;
			}
	}

  void _emit( Param1 p1, Param2 p2, Param3 p3 ) const
	{
		for (DelegateIterator it = delegateList.begin(); it != delegateList.end(); ++it)
			( *it )( p1, p2, p3 );
	}

	void operator() ( Param1 p1, Param2 p2, Param3 p3 ) const
	{
    _emit( p1, p2, p3 );
	}
};


template< class Param1, class Param2, class Param3, class Param4 >
class Signal4
{
public:
	typedef Delegate4< Param1, Param2, Param3, Param4 > _Delegate;

private:
	typedef List<_Delegate> DelegateList;
        typedef typename DelegateList::const_iterator DelegateIterator;
	DelegateList delegateList;

public:
	void connect( _Delegate delegate )
	{
		delegateList.push_back( delegate );
	}

	template< class X, class Y >
	void connect( Y * obj, void (X::*func)( Param1 p1, Param2 p2, Param3 p3, Param4 p4 ) )
	{
		delegateList.push_back( makeDelegate( obj, func ) );
	}

	template< class X, class Y >
	void connect( Y * obj, void (X::*func)( Param1 p1, Param2 p2, Param3 p3, Param4 p4 ) const )
	{
		delegateList.push_back( makeDelegate( obj, func ) );
	}

	void disconnect( _Delegate delegate )
	{
		for (DelegateIterator it = delegateList.begin(); it != delegateList.end(); ++it)
			if( *it == delegate )
			{
				delegateList.erase( it );
				return;
			}
	}

	template< class X, class Y >
	void disconnect( Y * obj, void (X::*func)( Param1 p1, Param2 p2, Param3 p3, Param4 p4 ) )
	{
		_Delegate& eq = makeDelegate( obj, func );
		for (DelegateIterator it = delegateList.begin(); it != delegateList.end(); ++it)
			if( *it == eq )
			{
				delegateList.erase( it );
				return;
			}
	}

	template< class X, class Y >
	void disconnect( Y * obj, void (X::*func)( Param1 p1, Param2 p2, Param3 p3, Param4 p4 ) const )
	{
		_Delegate& eq = makeDelegate( obj, func );
		for (DelegateIterator it = delegateList.begin(); it != delegateList.end(); ++it)
			if( *it == eq )
			{
				delegateList.erase( it );
				return;
			}
	}

  void _emit( Param1 p1, Param2 p2, Param3 p3, Param4 p4 ) const
	{
		for (DelegateIterator it = delegateList.begin(); it != delegateList.end(); ++it)
                    ( *it )( p1, p2, p3, p4 );
	}

	void operator() ( Param1 p1, Param2 p2, Param3 p3, Param4 p4 ) const
	{
    _emit( p1, p2, p3, p4 );
	}
};


template< class Param1, class Param2, class Param3, class Param4, class Param5 >
class Signal5
{
public:
	typedef Delegate5< Param1, Param2, Param3, Param4, Param5 > _Delegate;

private:
	typedef List<_Delegate> DelegateList;
	typedef typename DelegateList::const_iterator DelegateIterator;
	DelegateList delegateList;

public:
	void connect( _Delegate delegate )
	{
		delegateList.push_back( delegate );
	}

	template< class X, class Y >
	void connect( Y * obj, void (X::*func)( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5 ) )
	{
		delegateList.push_back( makeDelegate( obj, func ) );
	}

	template< class X, class Y >
	void connect( Y * obj, void (X::*func)( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5 ) const )
	{
		delegateList.push_back( makeDelegate( obj, func ) );
	}

	void disconnect( _Delegate delegate )
	{
		for (DelegateIterator it = delegateList.begin(); it != delegateList.end(); ++it)
			if( *it == delegate )
			{
				delegateList.erase( it );
				return;
			}
	}

	template< class X, class Y >
	void disconnect( Y * obj, void (X::*func)( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5 ) )
	{
		_Delegate& eq = makeDelegate( obj, func );
		for (DelegateIterator it = delegateList.begin(); it != delegateList.end(); ++it)
			if( *it == eq )
			{
				delegateList.erase( it );
				return;
			}
	}

	template< class X, class Y >
	void disconnect( Y * obj, void (X::*func)( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5 ) const )
	{
		_Delegate& eq = makeDelegate( obj, func );
		for (DelegateIterator it = delegateList.begin(); it != delegateList.end(); ++it)
			if( *it == eq )
			{
				delegateList.erase( it );
				return;
			}
	}

  void _emit( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5 ) const
	{
		for (DelegateIterator it = delegateList.begin(); it != delegateList.end(); ++it)
			( *it )( p1, p2, p3, p4, p5 );
	}

	void operator() ( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5 ) const
	{
    _emit( p1, p2, p3, p4, p5 );
	}
};


template< class Param1, class Param2, class Param3, class Param4, class Param5, class Param6 >
class Signal6
{
public:
	typedef Delegate6< Param1, Param2, Param3, Param4, Param5, Param6 > _Delegate;

private:
	typedef List<_Delegate> DelegateList;
	typedef typename DelegateList::const_iterator DelegateIterator;
	DelegateList delegateList;

public:
	void connect( _Delegate delegate )
	{
		delegateList.push_back( delegate );
	}

	template< class X, class Y >
	void connect( Y * obj, void (X::*func)( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6 ) )
	{
		delegateList.push_back( makeDelegate( obj, func ) );
	}

	template< class X, class Y >
	void connect( Y * obj, void (X::*func)( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6 ) const )
	{
		delegateList.push_back( makeDelegate( obj, func ) );
	}

	void disconnect( _Delegate delegate )
	{
		for (DelegateIterator it = delegateList.begin(); it != delegateList.end(); ++it)
			if( *it == delegate )
			{
				delegateList.erase( it );
				return;
			}
	}

	template< class X, class Y >
	void disconnect( Y * obj, void (X::*func)( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6 ) )
	{
		_Delegate& eq = makeDelegate( obj, func );
		for (DelegateIterator it = delegateList.begin(); it != delegateList.end(); ++it)
			if( *it == eq )
			{
				delegateList.erase( it );
				return;
			}
	}

	template< class X, class Y >
	void disconnect( Y * obj, void (X::*func)( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6 ) const )
	{
		_Delegate& eq = makeDelegate( obj, func );
		for (DelegateIterator it = delegateList.begin(); it != delegateList.end(); ++it)
			if( *it == eq )
			{
				delegateList.erase( it );
				return;
			}
	}

  void _emit( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6 ) const
	{
		for (DelegateIterator it = delegateList.begin(); it != delegateList.end(); ++it)
			(*it)( p1, p2, p3, p4, p5, p6 );
	}

	void operator() ( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6 ) const
	{
    _emit( p1, p2, p3, p4, p5, p6 );
	}
};


template< class Param1, class Param2, class Param3, class Param4, class Param5, class Param6, class Param7 >
class Signal7
{
public:
	typedef Delegate7< Param1, Param2, Param3, Param4, Param5, Param6, Param7 > _Delegate;

private:
	typedef List<_Delegate> DelegateList;
	typedef typename DelegateList::const_iterator DelegateIterator;
	DelegateList delegateList;

public:
	void connect( _Delegate delegate )
	{
		delegateList.push_back( delegate );
	}

	template< class X, class Y >
	void connect( Y * obj, void (X::*func)( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7 ) )
	{
		delegateList.push_back( makeDelegate( obj, func ) );
	}

	template< class X, class Y >
	void connect( Y * obj, void (X::*func)( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7 ) const )
	{
		delegateList.push_back( makeDelegate( obj, func ) );
	}

	void disconnect( _Delegate delegate )
	{
    foreach( it, delegateList )
    {
      if( *it == delegate )
			{
        delegateList.erase( it );
				return;
			}
    }
	}

	template< class X, class Y >
	void disconnect( Y * obj, void (X::*func)( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7 ) )
	{
		_Delegate& eq = makeDelegate( obj, func );
		for (DelegateIterator it = delegateList.begin(); it != delegateList.end(); ++it)
			if( *it == eq )
			{
				delegateList.erase( it );
				return;
			}
	}

	template< class X, class Y >
	void disconnect( Y * obj, void (X::*func)( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7 ) const )
	{
		_Delegate& eq = makeDelegate( obj, func );
		for (DelegateIterator it = delegateList.begin(); it != delegateList.end(); ++it)
			if( *it == eq )
			{
				delegateList.erase( it );
				return;
			}
	}

  void _emit( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7 ) const
	{
		for (DelegateIterator it = delegateList.begin(); it != delegateList.end(); ++it)
                        (*it)( p1, p2, p3, p4, p5, p6, p7 );
	}

	void operator() ( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7 ) const
	{
    _emit( p1, p2, p3, p4, p5, p6, p7 );
	}
};


template< class Param1, class Param2, class Param3, class Param4, class Param5, class Param6, class Param7, class Param8 >
class Signal8
{
public:
	typedef Delegate8< Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8 > _Delegate;

private:
	typedef List<_Delegate> DelegateList;
	typedef typename DelegateList::const_iterator DelegateIterator;
	DelegateList delegateList;

public:
	void connect( _Delegate delegate )
	{
		delegateList.push_back( delegate );
	}

	template< class X, class Y >
	void connect( Y * obj, void (X::*func)( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8 ) )
	{
		delegateList.push_back( makeDelegate( obj, func ) );
	}

	template< class X, class Y >
	void connect( Y * obj, void (X::*func)( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8 ) const )
	{
		delegateList.push_back( makeDelegate( obj, func ) );
	}

	void disconnect( _Delegate delegate )
	{
		for (DelegateIterator it = delegateList.begin(); it != delegateList.end(); ++it)
			if( *it == delegate )
			{
				delegateList.erase( it );
				return;
			}
	}

	template< class X, class Y >
	void disconnect( Y * obj, void (X::*func)( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8 ) )
	{
		_Delegate& eq = makeDelegate( obj, func );
		for (DelegateIterator it = delegateList.begin(); it != delegateList.end(); ++it)
			if( *it == eq )
			{
				delegateList.erase( it );
				return;
			}
	}

	template< class X, class Y >
	void disconnect( Y * obj, void (X::*func)( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8 ) const )
	{
		_Delegate& eq = makeDelegate( obj, func );
		for (DelegateIterator it = delegateList.begin(); it != delegateList.end(); ++it)
			if( *it == eq )
			{
				delegateList.erase( it );
				return;
			}
	}

  void _emit( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8 ) const
	{
		for (DelegateIterator it = delegateList.begin(); it != delegateList.end(); ++it)
                        (*it)( p1, p2, p3, p4, p5, p6, p7, p8 );
	}

	void operator() ( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8 ) const
	{
    _emit( p1, p2, p3, p4, p5, p6, p7, p8 );
	}
};

#endif //__CAESARIA_SIGNAL_H_INCLUDE_
