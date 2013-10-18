// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.

#include "oc3_building_native.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_tile.hpp"
#include "oc3_city.hpp"

NativeBuilding::NativeBuilding( const TileOverlayType type, const Size& size ) 
: Building( type, size )
{
}

void NativeBuilding::save( VariantMap& stream) const 
{
  Building::save(stream);
}

void NativeBuilding::load( const VariantMap& stream) {Building::load(stream);}

void NativeBuilding::build( CityPtr city, const TilePos& pos )
{
  Building::build( city, pos );
  getTile().setFlag( Tile::tlRock, true );
  getTile().setFlag( Tile::tlBuilding, false );
}

NativeHut::NativeHut() : NativeBuilding( B_NATIVE_HUT, Size(1) )
{
  setPicture( Picture::load( ResourceGroup::housing, 49));
  //setPicture(PicLoader::instance().get_picture("housng1a", 50));
}

void NativeHut::save( VariantMap& stream) const 
{
  Building::save(stream);
}

void NativeHut::load( const VariantMap& stream) {Building::load(stream);}

NativeCenter::NativeCenter() : NativeBuilding( B_NATIVE_CENTER, Size(2) )
{
  setPicture( Picture::load(ResourceGroup::housing, 51));
}

void NativeCenter::save( VariantMap&stream) const 
{
  Building::save(stream);
}

void NativeCenter::load( const VariantMap& stream) {Building::load(stream);}

NativeField::NativeField() : NativeBuilding( B_NATIVE_FIELD, Size(1) ) 
{
  setPicture(Picture::load(ResourceGroup::commerce, 13));  
}

void NativeField::save( VariantMap&stream) const 
{
  Building::save(stream);
}

void NativeField::load( const VariantMap& stream) {Building::load(stream);}
