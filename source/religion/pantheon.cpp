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

#include "objects/farm.hpp"
#include "pantheon.hpp"
#include "gfx/picture.hpp"
#include "game/gamedate.hpp"
#include "events/event.hpp"
#include "core/gettext.hpp"
#include "objects/constants.hpp"
#include "divinities.hpp"
#include "ceres.hpp"
#include "neptune.hpp"
#include "city/city.hpp"
#include "venus.hpp"
#include "mercury.hpp"
#include "core/variant_map.hpp"
#include "mars.hpp"

namespace religion
{

namespace rome
{

class Pantheon::Impl
{
public:  
  DivinityList divinties;

  template<typename T>
  void addDivn()
  {
    auto divn = RomeDivinity::create<T>();
    divinties.push_back( divn.object() );
  }
};

Pantheon::Pantheon() : _d( new Impl )
{
  _d->addDivn<Ceres>();
  _d->addDivn<Neptune>();
  _d->addDivn<Mercury>();
  _d->addDivn<Venus>();
  _d->addDivn<Mars>();
}

DivinityPtr Pantheon::get(RomeDivinity::Type name )
{
  return get( RomeDivinity::findIntName( name ) );
}

DivinityPtr Pantheon::get( const std::string& name)
{
  DivinityList divines = instance().all();
  for( auto& current : divines )
  {
    if( current->name() == name || current->internalName() == name )
      return current;
  }

  return DivinityPtr();
}

Pantheon::~Pantheon() {}
DivinityList Pantheon::all(){ return _d->divinties; }
DivinityPtr Pantheon::mars(){  return get( RomeDivinity::Mars ); }
DivinityPtr Pantheon::neptune() { return get( RomeDivinity::Neptune ); }
DivinityPtr Pantheon::venus(){ return get( RomeDivinity::Venus ); }
DivinityPtr Pantheon::mercury(){  return get( RomeDivinity::Mercury ); }
DivinityPtr Pantheon::ceres() {  return get( RomeDivinity::Ceres );}

void Pantheon::load( const VariantMap& stream )
{  
  for( auto name : RomeDivinity::getIntNames() )
  {
    DivinityPtr divn = get( name );

    if( divn.isNull() )
    {
      divn.attachObject( new RomeDivinity( RomeDivinity::Count ) );
      divn->drop();
      _d->divinties.push_back( divn );
    }

    divn->load( stream.get( name ).toMap() );
  }
}

void Pantheon::save(VariantMap& stream)
{
  DivinityList divines = instance().all();

  for( auto current : divines )
  {
    stream[ current->internalName() ] = current->save();
  }
}

void Pantheon::doFestival(RomeDivinity::Type who, int type )
{
  auto divn = get( who ).as<RomeDivinity>();
  if( divn.isValid() )
  {
    divn->assignFestival( type );
  }
}

void Pantheon::doFestival(const std::string& who, int type)
{
  auto divn = get( who ).as<RomeDivinity>();
  if( divn.isValid() )
  {
    divn->assignFestival( type );
  }
}

}//end namespace rome

}//end namespace religion
