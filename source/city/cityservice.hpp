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

#ifndef __CAESARIA_CITYSERVICE_H_INCLUDED__
#define __CAESARIA_CITYSERVICE_H_INCLUDED__

#include "core/smartptr.hpp"
#include "core/variant.hpp"
#include "predefinitions.hpp"
#include "core/scopedptr.hpp"

namespace city
{

PREDEFINE_CLASS_SMARTPOINTER(Srvc)

class Srvc : public ReferenceCounted
{
public:
  template<typename Class, typename... Args>
  static SrvcPtr create( const Args & ... args)
  {
    auto instance = ptr_make<Class>( args... );
    return ptr_cast<Srvc>( instance );
  }
  /**
   * @brief Call every frame
   * @param Current frame in city
   */
  virtual void timeStep( const unsigned int time );

  std::string name() const;
  void setName( const std::string& name  );

  /** Add this service to city **/
  void attach();

  /**
   * @brief Check service is alive
   * @return false if may to destroy service
   */
  virtual bool isDeleted() const;
  
  /**
   * @brief Calls before erasing from city
   */
  virtual void destroy();

  /** Serialization functions **/
  virtual VariantMap save() const;
  virtual void load(const VariantMap& stream);

  virtual ~Srvc();
protected:
  Srvc( PlayerCityPtr city, const std::string& name );
  PlayerCityPtr _city() const;

private:
  class Impl;
  ScopedPtr<Impl> _d;
};

}//end namespace city

#endif//__CAESARIA_CITYSERVICE_H_INCLUDED__
