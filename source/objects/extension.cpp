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
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

#include "extension.hpp"
#include "factory.hpp"

ConstructionExtensionPtr FactoryProgressUpdater::assignTo(FactoryPtr factory, float value, int time)
{
  FactoryProgressUpdater* updater = new FactoryProgressUpdater();
  updater->_value = value;
  updater->_time = time;

  ConstructionExtensionPtr ret( updater );
  ret->drop();

  factory->addExtension( ret );

  return ret;
}

void FactoryProgressUpdater::run(gfx::TileOverlayPtr parent, unsigned int time)
{
  FactoryPtr factory = ptr_cast<Factory>( parent );
  if( factory.isValid() )
  {
    factory->updateProgress( _value );
  }

  time--;
}

bool FactoryProgressUpdater::isDeleted() const { return time > 0; }

FactoryProgressUpdater::FactoryProgressUpdater() : _value( 0 ), _time( 0 )
{

}
