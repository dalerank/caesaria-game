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

void FactoryProgressUpdater::run( ConstructionPtr parent, unsigned int time)
{
  if( GameDate::isWeekChanged() )
  {
    FactoryPtr factory = ptr_cast<Factory>( parent );
    if( factory.isValid() )
    {
      factory->updateProgress( _value );
    }

    _isDeleted = GameDate::current() > _finishDate;
  }
}

bool FactoryProgressUpdater::isDeleted() const { return _isDeleted; }

FactoryProgressUpdater::FactoryProgressUpdater() : _value( 0 ), _isDeleted( false )
{
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

void FortCurseByMars::run(ConstructionPtr parent, unsigned int time)
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

    _isDeleted = GameDate::current() > _finishDate;
  }
}

bool FortCurseByMars::isDeleted() const { return _isDeleted; }

FortCurseByMars::FortCurseByMars() :_isDeleted( false )
{
}

