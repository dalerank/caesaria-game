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

#include "infobox_legion.hpp"
#include "walker/patrolpoint.hpp"
#include "infobox_citizen_mgr.hpp"
#include "gfx/tilemap.hpp"
#include "infobox_citizen.hpp"
#include "core/logger.hpp"
#include "walker/helper.hpp"
#include <vector>

namespace gui
{

namespace infobox
{

namespace citizen
{

class PManager::Impl
{
public:
  typedef std::map< walker::Type, CzInfoboxCreatorPtr> Creators;
  Creators creators;
};

template< class T >
class SpecificCitizenInfoboxCreator : public CzInfoboxCreator
{
public:
  Infobox* create(  gui::Widget* parent, PlayerCityPtr city, const TilePos& pos )
  {
    return new T( parent, city, pos );
  }
};

PManager& PManager::instance()
{
  static PManager inst;
  return inst;
}

void PManager::loadInfoboxes()
{
  addCreator( walker::patrolPoint, new SpecificCitizenInfoboxCreator<AboutLegion>() );
}

PManager::~PManager() {}

void PManager::addCreator( walker::Type type, CzInfoboxCreatorPtr c)
{
  Impl::Creators::iterator it = _d->creators.find( type );
  if( it != _d->creators.end() )
  {
    Logger::warning( "InfoboxCitizenManager: also have infobox creator for type " + WalkerHelper::getTypename( type ) );
    return;
  }

  _d->creators[ type ] = c;
}

Infobox* PManager::show( gui::Widget* parent, PlayerCityPtr city, const TilePos& pos )
{
  WalkerList walkers = city->walkers( pos );
  Impl::Creators::iterator it = _d->creators.find( walkers.empty() ? walker::unknown : walkers.front()->type() );
  if( it == _d->creators.end() )
  {
    return &parent->add<citizen::AboutPeople>( city, pos );
  }
  else
  {
    return it->second->create( parent, city, pos );
  }
}

PManager::PManager() : _d( new Impl )
{
}

}//end namespace citizen

}//end namespace infobox

}//end namespace gui
