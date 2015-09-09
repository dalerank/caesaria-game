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

#ifndef _CAESARIA_SINGLETON_H_INCLUDE_
#define _CAESARIA_SINGLETON_H_INCLUDE_

#include "requirements.hpp"

/** The "classic" Singleton.
 */
template <typename T>
class StaticSingleton
{
public:
  static T& instance()
  {
    static T inst;
    return inst;
  }

protected:
  StaticSingleton() {}
  virtual ~StaticSingleton() {}

private:
  StaticSingleton(const StaticSingleton<T>&) {}
  StaticSingleton<T>& operator=(const StaticSingleton<T>&) {return this;}
};

#if _MSC_VER >= 1300 
#define SET_STATICSINGLETON_FRIEND_FOR(a) template<class a> friend class StaticSingleton;
#else
#define SET_STATICSINGLETON_FRIEND_FOR(a) friend class StaticSingleton;
#endif

template <typename T>
class DynamicSingleton
{
public:
  static T* instance()
  {
    _CAESARIA_DEBUG_BREAK_IF( !_instance );
    return _instance;
  }

  DynamicSingleton()
  {
    _CAESARIA_DEBUG_BREAK_IF( _instance );
    _instance = static_cast<T*>(this);
  }

  virtual ~DynamicSingleton()
  {
    _instance = 0;
  }

private:
  static T* _instance;

  DynamicSingleton(const DynamicSingleton<T>&) {}
  DynamicSingleton<T&> operator=(const DynamicSingleton<T>&) {}
};

template <typename T> T* DynamicSingleton<T>::_instance = 0;

#endif //_CAESARIA_SINGLETON_H_INCLUDE_
