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
#include "city/helper.hpp"
#include "events/event.hpp"
#include "core/gettext.hpp"
#include "objects/constants.hpp"
#include "divinities.hpp"
#include "ceres.hpp"
#include "neptune.hpp"
#include "venus.hpp"
#include "mercury.hpp"
#include "core/variant_map.hpp"
#include "mars.hpp"

using namespace constants;

namespace religion
{

namespace rome
{

class Pantheon::Impl
{
public:  
  DivinityList divinties;
};

Pantheon& Pantheon::instance()
{
  static Pantheon inst;
  return inst;
}

Pantheon::Pantheon() : _d( new Impl )
{
  _d->divinties.push_back( rome::Ceres::create() );
  _d->divinties.push_back( rome::Neptune::create() );
  _d->divinties.push_back( rome::Mercury::create() );
  _d->divinties.push_back( rome::Venus::create() );
  _d->divinties.push_back( rome::Mars::create() );
}

DivinityPtr Pantheon::get( RomeDivinityType name )
{
  return get( baseDivinityNames[ name ] );
}

DivinityPtr Pantheon::get( const std::string& name)
{
  DivinityList divines = instance().all();
  foreach( current, divines )
  {
    if( (*current)->name() == name || (*current)->internalName() == name )
      return *current;
  }

  return DivinityPtr();
}

DivinityList Pantheon::all(){ return _d->divinties; }
DivinityPtr Pantheon::mars(){  return get( romeDivMars ); }
DivinityPtr Pantheon::neptune() { return get( romeDivNeptune ); }
DivinityPtr Pantheon::venus(){ return get( romeDivVenus ); }
DivinityPtr Pantheon::mercury(){  return get( romeDivMercury ); }
DivinityPtr Pantheon::ceres() {  return get( romeDivCeres );}

void Pantheon::load( const VariantMap& stream )
{  
  for( int index=0; baseDivinityNames[ index ] != 0; index++ )
  {
    DivinityPtr divn = get( baseDivinityNames[ index ] );

    if( divn.isNull() )
    {
      divn = DivinityPtr( new rome::RomeDivinity() );
      divn->setInternalName( baseDivinityNames[ index ] );
      divn->drop();
      _d->divinties.push_back( divn );
    }

    divn->load( stream.get( baseDivinityNames[ index ] ).toMap() );
  }
}

void Pantheon::save(VariantMap& stream)
{
  DivinityList divines = instance().all();

  foreach( current, divines )
  {
    stream[ (*current)->internalName() ] = (*current)->save();
  }
}

void Pantheon::doFestival( RomeDivinityType who, int type )
{
  DivinityPtr divn = get( who );
  if( divn.isValid() )
  {
    ptr_cast<rome::RomeDivinity>(divn)->assignFestival( type );
  }
}

void Pantheon::doFestival(const std::string& who, int type)
{
  DivinityPtr divn = get( who );
  if( divn.isValid() )
  {
    ptr_cast<rome::RomeDivinity>(divn)->assignFestival( type );
  }
}

}//end namespace rome

}//end namespace religion
