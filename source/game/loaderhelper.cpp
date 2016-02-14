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

#include "loaderhelper.hpp"
#include "city/city.hpp"
#include "gfx/imgid.hpp"
#include "objects/objects_factory.hpp"
#include "resourcegroup.hpp"
#include "core/logger.hpp"
#include "gfx/tilesarray.hpp"
#include "game/settings.hpp"
#include "core/saveadapter.hpp"

using namespace gfx;

namespace {
GAME_LITERALCONST(Brundisium)
GAME_LITERALCONST(map)
GAME_LITERALCONST(sav)

std::string findCityById(int id, const std::string& section)
{
  VariantMap vm = config::load( SETTINGS_RC_PATH(citiesIdModel) );
  VariantMap idsVm = vm.get( section ).toMap();
  return idsVm.get( utils::i2str( id ), std::string( literals::Brundisium ) );
}
}

std::string LoaderHelper::getDefaultCityName(unsigned int location)
{
  return findCityById(location, literals::map);
}

std::string LoaderHelper::getDefaultCityNameSav(unsigned int location)
{
  return findCityById(location, literals::sav);
}

object::Type LoaderHelper::convImgId2ovrType( unsigned int imgId )
{
  object::Type ovType = object::unknown;
  switch ( imgId )
  {
    case 0xc88:                                 ovType = object::hospital; break;
    case 0xc11:                                 ovType = object::gladiatorSchool; break;
    case 0xc39:                                 ovType = object::chariotSchool; break;
    case 0xc54:                                 ovType = object::statue_middle; break;
    case 0xc56:                                 ovType = object::senate; break;
    case 0xc89:                                 ovType = object::small_ceres_temple;    break;
    case 0xb8b:                                 ovType = object::library; break;
    case 0x2b8:                                 ovType = object::tower; break;
    case 0xb10:  case 0xb0d:                    ovType = object::native_center;    break;
    case 0xb0e:                                 ovType = object::well;    break;
    case 0xb0f:  case 0xb0b:  case 0xb0c:       ovType = object::native_hut;    break;
    case 0xbdf:                                 ovType = object::amphitheater; break;
    case 0xb8c:                                 ovType = object::academy;  break;
    case 0xb17:  case 0xb56:                    ovType = object::fountain;    break;
    case 0xbeb:                                 ovType = object::theater ;    break;
    case 0xb11: case 0xb44: case 0xb45: case 0xb46: ovType = object::native_field;    break;
    case 0xb43: case 0xb53: case 0xb57:         ovType = object::olive_farm;    break;
    case 0xb4e: case 0xb52:                     ovType = object::fig_farm;    break;
    case 0xb38:                                 ovType = object::market;    break;
    case 0xb7c + 71:                            ovType = object::granery;    break;  //0xbc3 - (71 is id granary in original game)
    case 0xb2f:                                 ovType = object::reservoir;    break;
    case 0xb9a:                                 ovType = object::oil_workshop;    break;
    case 0x34d: case 0x34e: case 0x34f: case 0x350: ovType = object::elevation;    break;
    case 0xc50 + 74:                            ovType = object::shipyard;    break;
    case 0xc5c:                                 ovType = object::forum; break;
    case 0xc6d:                                 ovType = object::barracks;    break;
    case 0xc6f:                                 ovType = object::barber;     break;
    case 0xc68:                                 ovType = object::fort_horse; break;
    case 0xc69:                                 ovType = object::fort_legionaries; break;
    case 0xae0 + 0:  case 0xae1: case 0xae2: case 0xae4: case 0xae5:
    case 0xaeb: case 0xae6:
                                                ovType = object::house;      break;
    case 0xc70:                                 ovType = object::clinic;      break;
    case 0xc53:                                 ovType = object::statue_small; break;
    case 0xc7c:                                 ovType = object::baths;      break;
    case 0xadd: case 0xadb: case 0xade: case 0xadf:
    case 0xb07: case 0xadc:
                                                ovType = object::house;    break;
    case 0xc4c: case 0xc4d: case 0xc4e: case 0xc4f:
                                                ovType = object::garden;    break;

    case 0x29c: case 0x29d: case 0x2a1:         ovType = object::aqueduct; break;
    case 0xc5d: case 0xc52:                     ovType = object::prefecture; break;
    case 0xcd1:                                 ovType = object::engineering_post;    break;
    case 0xc81 + 76:                            ovType = object::wharf;    break;
    case 0xb8a:                                 ovType = object::school;    break;
    case 0xc2f:                                 ovType = object::actorColony;    break;
    case 0xc91:                                 ovType = object::small_venus_temple;    break;
    case 0xc8b:                                 ovType = object::small_neptune_temple; break;
    case 0xc8d:                                 ovType = object::small_mercury_temple;    break;
    case 0xc8f:                                 ovType = object::small_mars_temple; break;
    case 0xcf6:                                 ovType = object::missionaryPost; break;
    case 0xcf7: case 0xd17: case 0xd09:         ovType = object::warehouse;    break;
    case 0xc55: ovType = object::statue_big; break;
    case 0xb08: ovType = object::governorHouse; break;
    case 0xb09: ovType = object::governorVilla; break;
    case 0xb0a: ovType = object::governorPalace; break;
    case 0xb8d: ovType = object::wine_workshop; break;
    case 0xc6e: ovType = object::military_academy; break;
    case 0x2b9: ovType = object::gatehouse; break;
    case 0xc1c: ovType = object::lionsNursery; break;
    case 0xbac: ovType = object::furniture_workshop; break;
    case 0xba3: ovType = object::weapons_workshop; break;
    case 0xbbb: ovType = object::pottery_workshop; break;
  }

  return ovType;
}

void LoaderHelper::decodeTerrain( Tile &oTile, PlayerCityPtr city, unsigned int forceId )
{
  int changeId = 0;
  unsigned int imgId = oTile.imgId();
  object::Type ovType = object::unknown;
  if( oTile.getFlag( Tile::tlRoad ) )   // road
  {
    ovType = object::road;
    Picture pic = object::Info::find( object::terrain ).randomPicture( Size::square(1) );
    oTile.setPicture( pic );
    changeId = imgid::fromResource( pic.name() );
  }
  else if( oTile.getFlag( Tile::tlTree ) )
  {
    ovType = object::tree;
    Picture pic = object::Info::find( object::terrain ).randomPicture( Size::square(1) );
    oTile.setPicture( pic );
    changeId = imgid::fromResource( pic.name() );
  }
  else if( oTile.getFlag( Tile::tlMeadow ) )
  {
  } 
  else if( imgId >= 0x29c && imgId <= 0x2a1 ) //aqueduct
  {
    ovType = object::aqueduct;
    Picture pic = object::Info::find( object::terrain ).randomPicture( Size::square(1) );
    oTile.setPicture( pic );
    oTile.setFlag( Tile::clearAll, true );
    changeId = imgid::fromResource( pic.name() );
  }
  else if( imgId >= 372 && imgId <= 427 )
  {
    oTile.setFlag( Tile::tlCoast, true );
    if( imgId >= 388 )
      oTile.setFlag( Tile::tlRubble, true );
  }
  else if( imgId >= 863 && imgId <= 870 )
  {
    Picture pic = object::Info::find( object::terrain ).randomPicture( Size::square(1) );
    oTile.setPicture( pic );
    oTile.setFlag( Tile::clearAll, true );    
    changeId = imgid::fromResource( pic.name() );
    oTile.setImgId( changeId );
  }
  else
  {
    unsigned id = forceId > 0 ? forceId : oTile.imgId();
    ovType = convImgId2ovrType( id );
  }

  if( ovType == object::unknown )
    return;

  OverlayPtr overlay = Overlay::create( ovType );
  if( ovType == object::elevation )
  {
    std::string elevationPicName = imgid::toResource( oTile.imgId() );
    overlay->setPicture( Picture( elevationPicName ) );
  }

  if( overlay != NULL )
  {
    //Logger::warning( "Building at ( %d, %d ) with ID: %x", oTile.i(), oTile.j(), oTile.originalImgId() );
    if( oTile.overlay().isValid() )
      return;

    city::AreaInfo info( city, oTile.pos() );
    overlay->build( info );
    city->addOverlay( overlay );
  }  

  if( changeId > 0 )
  {
    oTile.setImgId( changeId );
  }
}
