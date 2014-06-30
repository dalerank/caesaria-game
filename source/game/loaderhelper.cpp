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

#include "loaderhelper.hpp"
#include "city/city.hpp"
#include "gfx/tile.hpp"
#include "objects/objects_factory.hpp"
#include "core/logger.hpp"

using namespace constants;
using namespace gfx;

std::string LoaderHelper::getDefaultCityName(unsigned int location)
{
  switch( location )
  {
  case 0: return "Brundisium";
  case 1: return "Brundisium";
  case 2: return "Capua";
  case 3: return "Tarentum";
  case 4: return "Tarraco";
  case 5: return "Syracusae";
  case 6: return "Miletus";
  case 7: return "Mediolanum";
  case 8: return "Carthago";
  case 35: return "Caesarea";
  case 9: case 30: return "Carthago"; break;
  case 10: return "Cyrene"; break;
  case 11: case 15: case 25: return "Tarraco"; break;
  case 12: return "Hierosolyma"; break;
  case 13: case 28: return "Toletum"; break;
  case 14: case 26: return "Syracusae"; break;
  case 16: case 31: return "Tarsus"; break;
  case 17: case 32: return "Tingis"; break;
  case 18: return "Augusta Trevorum"; break;
  case 19: return "Carthago Nova"; break;
  case 20: return "Leptis Magna"; break;
  case 21: return "Athenae"; break;
  case 22: return "Brundisium"; break;
  case 23: return "Capua"; break;
  case 24: return "Tarentum"; break;
  case 27: return "Miletus"; break;
  case 29: return "Lugdunum"; break;
  case 33: return "Valentia"; break;
  case 34: return "Lutetia"; break;
  case 36: return "Sarmizegetusa"; break;
  case 37: return "Londinium"; break;
  case 38: return "Damascus"; break;
  case 39: return "Massilia"; break;
  case 40: return "Lindum"; break;
  }

  return "unknown city";
}

void LoaderHelper::decodeTerrain(Tile &oTile, PlayerCityPtr city )
{
  if (!oTile.isMasterTile() && oTile.masterTile()!=NULL)
    return;

  TileOverlayPtr overlay; // This is the overlay object, if any

  if( oTile.getFlag( Tile::tlRoad ) )   // road
  {
    overlay = TileOverlayFactory::instance().create( construction::road );
  }
  else /*if( oTile.getFlag( Tile::tlBuilding ) )*/
  {
    switch ( oTile.originalImgId() )
    {
      case 0xb0e:
      case 0xb0f:
      case 0xb0b:
      case 0xb0c:
        overlay = TileOverlayFactory::instance().create( building::nativeHut );
      break;

      case 0xb10:
      case 0xb0d:
        overlay =  TileOverlayFactory::instance().create( building::nativeCenter );
        Logger::warning( "creation of Native center at (%d,%d)", oTile.i(), oTile.j() );
      break;

      case 0xb11:
      case 0xb44:
      case 0xb45:
        overlay = TileOverlayFactory::instance().create( building::nativeField );
      break;

      case 0x34d:
      case 0x34e:
      case 0x34f:
      case 0x350:
        overlay = TileOverlayFactory::instance().create( building::elevation );
        overlay->setPicture( Picture::load( TileHelper::convId2PicName( oTile.originalImgId() ) ) );
      break;
    }
  }

  //terrain.setOverlay( overlay );
  if( overlay != NULL )
  {
    //Logger::warning( "Building at ( %d, %d ) with ID: %x", oTile.i(), oTile.j(), oTile.originalImgId() );
    overlay->build( city, oTile.pos() );
    city->overlays().push_back(overlay);
  }
}
