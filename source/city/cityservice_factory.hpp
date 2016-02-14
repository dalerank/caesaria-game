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

#ifndef __CAESARIA_CITYSERVICE_FACTORY_H_INCLUDED__
#define __CAESARIA_CITYSERVICE_FACTORY_H_INCLUDED__

#include "cityservice.hpp"
#include "core/singleton.hpp"

namespace city
{

class ServiceCreator : public ReferenceCounted
{
public:
  virtual SrvcPtr create( PlayerCityPtr city ) = 0;
  virtual std::string serviceName() const = 0;
};

typedef SmartPtr<ServiceCreator> ServiceCreatorPtr;

template< class T >
class BaseServiceCreator : public ServiceCreator
{
public:
  virtual SrvcPtr create( PlayerCityPtr city )
  {
    return Srvc::create<T>( city );
  }

  virtual std::string serviceName() const { return T::defaultName(); }
};

/**
 * @brief Factory, which can create services for city
 *        from registered classes.
 */
class ServiceFactory : public StaticSingleton<ServiceFactory>
{
  SET_STATICSINGLETON_FRIEND_FOR(ServiceFactory)
public:
  static SrvcPtr create( PlayerCityPtr city, const std::string& name );
  void addCreator( ServiceCreatorPtr creator );

  template<class T>
  void addCreator()
  {
    ServiceCreatorPtr ret( new BaseServiceCreator<T>() );
    addCreator( ret );
  }

  virtual ~ServiceFactory();

private:
  ServiceFactory();

  class Impl;
  ScopedPtr<Impl> _d;
};

#define REGISTER_SERVICE_IN_FACTORY(type,a) \
namespace { \
struct Registrator_##a { Registrator_##a() { ServiceFactory::instance().addCreator<type>(); }}; \
static Registrator_##a rtor_##a; \
}

}//end namespace city

#endif//__CAESARIA_CITYSERVICE_FACTORY_H_INCLUDED__
