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

#include "spear.hpp"
#include "core/gettext.hpp"
#include "city/city.hpp"
#include "game/resourcegroup.hpp"
#include "gfx/tilemap.hpp"
#include "core/foreach.hpp"

using namespace constants;
using namespace gfx;

SpearPtr Spear::create(PlayerCityPtr city)
{
  SpearPtr ret( new Spear( city ) );
  ret->drop();

  return ret;
}

void Spear::_onTarget()
{
  WalkerList walkers = _city()->walkers( walker::any, dstPos() );
  foreach( w, walkers )
  {
    (*w)->updateHealth( -10 );
    (*w)->acceptAction( Walker::acFight, startPos() );
  }

  TileOverlayPtr overlay = _city()->getOverlay( dstPos() );

  ConstructionPtr c = ptr_cast<Construction>( overlay );
  if( c.isValid() )
  {
    c->updateState( Construction::damage, 5 );
  }
}

const char* Spear::rcGroup() const {  return _picRc.c_str(); }
int Spear::_rcStartIndex() const { return _picIndex; }

void Spear::setPicInfo(const std::string& rc, unsigned int index)
{
  _picRc = rc;
  _picIndex = index;
}

Spear::Spear(PlayerCityPtr city) : ThrowingWeapon( city )
{
  _setType( walker::spear );
  _picIndex = 114;
  _picRc = ResourceGroup::sprites;

  setName( _("##spear##") );
}
