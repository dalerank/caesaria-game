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
#include "walker/soldier.hpp"

void ConstructionExtension::save(VariantMap &stream) const
{
  stream[ "type" ] = Variant( type() );
  stream[ "deleted" ] = _isDeleted;
}

void ConstructionExtension::load(const VariantMap &stream)
{
  _isDeleted = stream.get( "deleted" );
}

void ConstructionExtension::timeStep(ConstructionPtr, unsigned int)
{
  _isDeleted = GameDate::current() > _finishDate;
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
  updater->_value = value;

  DateTime gdate = GameDate::current();
  gdate.appendWeek( week2finish );
  updater->_finishDate = gdate;

  ConstructionExtensionPtr ret( updater );
  ret->drop();

  factory->addExtension( ret );

  return ret;
}

void FactoryProgressUpdater::timeStep( ConstructionPtr parent, unsigned int time)
{
  if( GameDate::isWeekChanged() )
  {
    FactoryPtr factory = ptr_cast<Factory>( parent );
    if( factory.isValid() )
    {
      factory->updateProgress( _value );
    }    
  }

  ConstructionExtension::timeStep( parent, time );
}

std::string FactoryProgressUpdater::type() const { return CAESARIA_STR_EXT(FactoryProgressUpdater); }

void FactoryProgressUpdater::save(VariantMap &stream) const
{

}

void FactoryProgressUpdater::load(const VariantMap &stream)
{

}

FactoryProgressUpdater::FactoryProgressUpdater() : _value( 0 )
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
  DateTime gdate = GameDate::current();
  gdate.appendMonth( monthsCurse );
  curse->_finishDate = gdate;

  ConstructionExtensionPtr ret( curse );
  ret->drop();

  fort->addExtension( ret );

  return ret;
}

void FortCurseByMars::timeStep(ConstructionPtr parent, unsigned int time)
{
  if( GameDate::isWeekChanged() )
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

void FortCurseByMars::save(VariantMap &stream) const
{

}

void FortCurseByMars::load(const VariantMap &stream)
{

}

FortCurseByMars::FortCurseByMars()
{
}

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

class ExtensionFactory::Impl
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

ExtensionFactory::~ExtensionFactory()
{

}

ExtensionFactory& ExtensionFactory::instance()
{
  static ExtensionFactory inst;
  return inst;
}

ConstructionExtensionPtr ExtensionFactory::create(std::string type)
{
  Impl::Creators::iterator it =  instance()._d->creators.find( type );

  return it != instance()._d->creators.end()
           ? it->second->create()
           : ConstructionExtensionPtr();
}

ConstructionExtensionPtr ExtensionFactory::create(const VariantMap& stream)
{
  std::string type = stream.get( "type" ).toString();
  ConstructionExtensionPtr ret = create( type );
  if( ret.isValid() )
  {
    ret->load( stream );
  }

  return ret;
}

ExtensionFactory::ExtensionFactory() : _d( new Impl )
{
#define ADD_CREATOR(T) _d->addCreator<T>( CAESARIA_STR_EXT(T) );

  ADD_CREATOR(FortCurseByMars)
  ADD_CREATOR(FactoryProgressUpdater)

#undef ADD_CREATOR
}
