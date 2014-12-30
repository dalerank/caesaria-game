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

#include "extension.hpp"
#include "factory.hpp"
#include "fort.hpp"
#include "game/gamedate.hpp"
#include "core/logger.hpp"
#include "objects/house.hpp"
#include "walker/soldier.hpp"
#include "warehouse.hpp"

void ConstructionExtension::save(VariantMap &stream) const
{
  stream[ "type" ] = Variant( type() );
  stream[ "deleted" ] = _isDeleted;
  stream[ "options" ] = _options;
}

void ConstructionExtension::load(const VariantMap &stream)
{
  _isDeleted = stream.get( "deleted" );
  _options = stream.get( "options" ).toMap();
}

void ConstructionExtension::timeStep(ConstructionPtr, unsigned int)
{
  _isDeleted = game::Date::current() > _finishDate;
}

ConstructionExtensionPtr FactoryProgressUpdater::create()
{
  ConstructionExtensionPtr ret( new FactoryProgressUpdater() );
  ret->drop();

  return ret;
}

ConstructionExtensionPtr FactoryProgressUpdater::assignTo(FactoryPtr factory, float value, int week2finish)
{
  FactoryProgressUpdater* updater = new FactoryProgressUpdater();
  updater->_options[ "value" ] = value;

  updater->_finishDate = game::Date::current();
  updater->_finishDate.appendWeek( week2finish );

  ConstructionExtensionPtr ret( updater );
  ret->drop();

  factory->addExtension( ret );

  return ret;
}

void FactoryProgressUpdater::timeStep( ConstructionPtr parent, unsigned int time)
{
  if( game::Date::isWeekChanged() )
  {
    FactoryPtr factory = ptr_cast<Factory>( parent );
    if( factory.isValid() )
    {
      factory->updateProgress( _options["value"] );
    }    
  }

  ConstructionExtension::timeStep( parent, time );
}

std::string FactoryProgressUpdater::type() const { return CAESARIA_STR_EXT(FactoryProgressUpdater); }

FactoryProgressUpdater::FactoryProgressUpdater()
{
}

ConstructionExtensionPtr FortCurseByMars::create()
{
  ConstructionExtensionPtr ret( new FortCurseByMars() );
  ret->drop();

  return ret;
}

ConstructionExtensionPtr FortCurseByMars::assignTo(FortPtr fort, unsigned int monthsCurse)
{
  FortCurseByMars* curse = new FortCurseByMars();
  DateTime gdate = game::Date::current();
  gdate.appendMonth( monthsCurse );
  curse->_finishDate = gdate;

  ConstructionExtensionPtr ret( curse );
  ret->drop();

  fort->addExtension( ret );

  return ret;
}

void FortCurseByMars::timeStep(ConstructionPtr parent, unsigned int time)
{
  if( game::Date::isWeekChanged() )
  {
    FortPtr base = ptr_cast<Fort>( parent );
    if( !base.isValid() )
    {
      Logger::warning( "FortCurseByMars::run base is null ");
      _isDeleted = true;
      return;
    }

    SoldierList sldrs = base->soldiers();

    foreach( it, sldrs )
    {
      (*it)->updateMorale( -100 );
    }
  }

  ConstructionExtension::timeStep( parent, time );
}

std::string FortCurseByMars::type() const{ return CAESARIA_STR_EXT(FortCurseByMars); }

FortCurseByMars::FortCurseByMars() {}

class BaseExtensionCreator : public ReferenceCounted
{
public:
  virtual ConstructionExtensionPtr create() = 0;
};

template<class T>
class ExtensionCreator : public BaseExtensionCreator
{
public:
  virtual ConstructionExtensionPtr create()
  {
    return T::create();
  }
};

typedef SmartPtr<BaseExtensionCreator> ExtensionCreatorPtr;

class ExtensionsFactory::Impl
{
public:
  typedef std::map<std::string, ExtensionCreatorPtr > Creators;
  Creators creators;

  template<class T>
  void addCreator( std::string name )
  {
    ExtensionCreatorPtr cr( new ExtensionCreator<T>() );
    cr->drop();

    creators[ name ] = cr;
  }
};

ExtensionsFactory::~ExtensionsFactory()
{

}

ExtensionsFactory& ExtensionsFactory::instance()
{
  static ExtensionsFactory inst;
  return inst;
}

ConstructionExtensionPtr ExtensionsFactory::create(std::string type)
{
  Impl::Creators::iterator it =  instance()._d->creators.find( type );

  return it != instance()._d->creators.end()
           ? it->second->create()
           : ConstructionExtensionPtr();
}

ConstructionExtensionPtr ExtensionsFactory::create(const VariantMap& stream)
{
  std::string type = stream.get( "type" ).toString();
  ConstructionExtensionPtr ret = create( type );
  if( ret.isValid() )
  {
    ret->load( stream );
  }

  return ret;
}

ExtensionsFactory::ExtensionsFactory() : _d( new Impl )
{
#define ADD_CREATOR(T) _d->addCreator<T>( CAESARIA_STR_EXT(T) );

  ADD_CREATOR(FortCurseByMars)
  ADD_CREATOR(FactoryProgressUpdater)
  ADD_CREATOR(WarehouseBuff)
  ADD_CREATOR(ConstructionParamUpdater)

#undef ADD_CREATOR
}


ConstructionExtensionPtr WarehouseBuff::create()
{
  ConstructionExtensionPtr ret( new WarehouseBuff() );
  ret->drop();

  return ret;
}

ConstructionExtensionPtr WarehouseBuff::assignTo(WarehousePtr warehouse, int group, float value, int week2finish)
{
  WarehouseBuff* buff = new WarehouseBuff();
  buff->_options[ "value" ] = value;
  buff->_options[ "group" ] = group;
  buff->_finishDate = game::Date::current();
  buff->_finishDate.appendWeek( week2finish );

  warehouse->addExtension( buff );
  buff->drop(); //automatic delete

  return buff;
}

void WarehouseBuff::timeStep(ConstructionPtr parent, unsigned int time)
{
  ConstructionExtension::timeStep( parent, time );
}

std::string WarehouseBuff::type() const {  return CAESARIA_STR_EXT(WarehouseBuff); }
float WarehouseBuff::value() const { return _options.get( "value" ).toFloat(); }
int WarehouseBuff::group() const { return _options.get( "group" ).toInt(); }

WarehouseBuff::WarehouseBuff() {}

ConstructionExtensionPtr ConstructionParamUpdater::create()
{
  ConstructionExtensionPtr ret( new ConstructionParamUpdater() );
  ret->drop();

  return ret;
}

ConstructionExtensionPtr ConstructionParamUpdater::assignTo(ConstructionPtr construction, int paramName, bool relative, int value, int week2finish)
{
  ConstructionParamUpdater* buff = new ConstructionParamUpdater();
  buff->_options[ "value" ] = value;
  buff->_options[ "relative" ] = relative;
  buff->_options[ "finishValue" ] = value;
  buff->_options[ "param" ] = paramName;
  buff->_finishDate = game::Date::current();
  buff->_finishDate.appendWeek( week2finish );

  construction->addExtension( buff );
  construction->updateState( paramName, value );
  buff->drop(); //automatic delete

  return buff;
}

void ConstructionParamUpdater::timeStep(ConstructionPtr parent, unsigned int time)
{
  ConstructionExtension::timeStep( parent,time );

  if( game::Date::isWeekChanged() )
  {
    if( _options[ "relative" ].toBool() )
    {
      int value = _options[ "value" ];
      int finishValue = _options[ "finishValue" ];
      parent->updateState( _options[ "param" ], value );
      finishValue += value;
    }
  }
}

std::string ConstructionParamUpdater::type() const { return CAESARIA_STR_EXT(ConstructionParamUpdater); }

void ConstructionParamUpdater::destroy(ConstructionPtr parent)
{
  if( parent.isValid() )
  {
    int finishValue = _options[ "finishValue" ];
    parent->updateState( _options[ "param" ], -finishValue );
  }
}

ConstructionParamUpdater::ConstructionParamUpdater() {}
