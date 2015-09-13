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
// Copyright 2012-2015 dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_INFOBOX_MANAGER_H_INCLUDE_
#define __CAESARIA_INFOBOX_MANAGER_H_INCLUDE_

#include "core/smartptr.hpp"
#include "core/referencecounted.hpp"
#include "core/scopedptr.hpp"
#include "core/predefinitions.hpp"
#include "gfx/tilemap.hpp"
#include "core/singleton.hpp"
#include "gui/info_box.hpp"

namespace gui
{

namespace infobox
{

class Manager;
typedef SmartPtr< Manager > InfoBoxManagerPtr;

class InfoboxCreator
{
public:
  virtual gui::infobox::Infobox* create( PlayerCityPtr, gui::Widget*, TilePos ) = 0;
};

template< class T >
class BaseInfoboxCreator : public InfoboxCreator
{
public:
  Infobox* create( PlayerCityPtr city, gui::Widget* parent, TilePos pos )
  {
    return new T( parent, city, city->tilemap().at( pos ) );
  }
};

class StaticInfoboxCreator : public InfoboxCreator
{
public:
  StaticInfoboxCreator( const std::string& caption,
                       const std::string& desc );

  virtual ~StaticInfoboxCreator() {}

  Infobox* create( PlayerCityPtr city, gui::Widget* parent, TilePos pos );

  std::string title, text;
};

class ServiceInfoboxCreator : public InfoboxCreator
{
public:
  ServiceInfoboxCreator( const std::string& caption,
                             const std::string& descr,
                             bool drawWorkers=false );

  virtual ~ServiceInfoboxCreator() {}

  Infobox* create( PlayerCityPtr city, gui::Widget* parent, TilePos pos );

  std::string title, text;
  bool isDrawWorkers;
};

class Manager : public StaticSingleton<Manager>
{
  SET_STATICSINGLETON_FRIEND_FOR(Manager)
public:
  void showHelp( PlayerCityPtr city, gui::Ui* gui, TilePos tile );
  void setShowDebugInfo( const bool showInfo );

  void addInfobox(const object::Type& type, InfoboxCreator* ctor );
  bool canCreate( const object::Type type ) const;

  void setBoxLock( bool lock );
private:
  Manager();
  virtual ~Manager();
   
  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namespace infobox

}//end namespave gui

#define REGISTER_OBJECT_INFOBOX(name,a) \
namespace { \
struct Registrator_##name { Registrator_##name() { Manager::instance().addInfobox( object::name, a ); }}; \
static Registrator_##name rtor_##name; \
}

#define REGISTER_OBJECT_BASEINFOBOX(name,a) \
namespace { \
struct Registrator_##name { Registrator_##name() { Manager::instance().addInfobox( object::name, new BaseInfoboxCreator<a>() ); }}; \
static Registrator_##name rtor_##name; \
}

#define REGISTER_OBJECT_STATICINFOBOX(name,a,b) \
namespace { \
struct Registrator_##name { Registrator_##name() { Manager::instance().addInfobox( object::name, new StaticInfoboxCreator(a,b) ); }}; \
static Registrator_##name rtor_##name; \
}

#define REGISTER_OBJECT_SERVICEINFOBOX(name,a,b) \
namespace { \
struct Registrator_##name { Registrator_##name() { Manager::instance().addInfobox( object::name, new ServiceInfoboxCreator(a,b) ); }}; \
static Registrator_##name rtor_##name; \
}

#endif //__CAESARIA_INFOBOX_MANAGER_H_INCLUDE_
