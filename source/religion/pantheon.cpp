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

#include "pantheon.hpp"
#include "gfx/picture.hpp"
#include "game/gamedate.hpp"
#include "city/helper.hpp"
#include "objects/farm.hpp"
#include "events/event.hpp"
#include "core/gettext.hpp"
#include "objects/constants.hpp"
#include "divinities.hpp"

using namespace constants;

namespace religion
{

class Pantheon::Impl
{
public:  
  Pantheon::Divinities divinties;
};

Pantheon& Pantheon::getInstance()
{
  static Pantheon inst;
  return inst;
}

Pantheon::Pantheon() : _d( new Impl )
{
}

RomeDivinityPtr Pantheon::ceres()
{
  return getInstance().get( romeDivCeres );
}

RomeDivinityPtr Pantheon::get( RomeDivinityType name )
{
  if( (unsigned int)name > getInstance()._d->divinties.size() )
    return RomeDivinityPtr();

  return getInstance()._d->divinties[ name ];
}

RomeDivinityPtr Pantheon::get(std::string name)
{
  Divinities divines = getInstance().getAll();
  foreach( current, divines )
  {
    if( (*current)->getName() == name || (*current)->getDebugName() == name )
      return *current;
  }

  return RomeDivinityPtr();
}

Pantheon::Divinities Pantheon::getAll(){ return _d->divinties; }
RomeDivinityPtr Pantheon::mars(){  return get( romeDivMars ); }
RomeDivinityPtr Pantheon::neptune() { return get( romeDivNeptune ); }
RomeDivinityPtr Pantheon::venus(){ return get( romeDivVenus ); }
RomeDivinityPtr Pantheon::mercury(){  return get( romeDivMercury ); }

void Pantheon::load( const VariantMap& stream )
{
  RomeDivinityPtr divn = get( divNames[ romeDivCeres ] );

  if( divn.isNull() )
  {
    _d->divinties.push_back( RomeDivinityCeres::create() );
  }

  for( int index=0; divNames[ index ] != 0; index++ )
  {
    divn = get( divNames[ index ] );

    if( divn.isNull() )
    {
      divn = RomeDivinityPtr( new RomeDivinityBase() );
      divn->setInternalName( divNames[ index ] );
      divn->drop();
      _d->divinties.push_back( divn );
    }

    divn->load( stream.get( divNames[ index ] ).toMap() );
  }
}

void Pantheon::save(VariantMap& stream)
{
  Divinities divines = getInstance().getAll();

  foreach( current, divines )
  {
    stream[ (*current)->getName() ] = (*current)->save();
  }
}

void Pantheon::doFestival( RomeDivinityType who, int type )
{
  RomeDivinityPtr divn = get( who );
  if( divn.isValid() )
  {
    ptr_cast<RomeDivinityBase>(divn)->assignFestival( type );
  }
}

}//end namespace religion
