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

#include "fortification.hpp"

#include "core/stringhelper.hpp"
#include "core/time.hpp"
#include "core/position.hpp"
#include "game/resourcegroup.hpp"
#include "core/safetycast.hpp"
#include "constants.hpp"
#include "game/city.hpp"
#include "gfx/tilemap.hpp"
#include "game/road.hpp"
#include "core/direction.hpp"
#include "core/logger.hpp"
#include "tower.hpp"

using namespace constants;

Fortification::Fortification() : Wall()
{
  setType( building::fortification );
  setPicture( ResourceGroup::wall, 178 ); // default picture for wall
}

void Fortification::build(PlayerCityPtr city, const TilePos& pos )
{
  Tilemap& tilemap = city->getTilemap();
  Tile& terrain = tilemap.at( pos );

  // we can't build if already have wall here
  WallPtr wall = terrain.getOverlay().as<Wall>();
  if( wall.isValid() )
  {
    return;
  }

  Building::build( city, pos );

  CityHelper helper( city );
  FortificationList fortifications = helper.find<Fortification>( building::fortification );
  foreach( FortificationPtr frt, fortifications )
  {
    frt->updatePicture( city );
  }

  updatePicture( city );
}

void Fortification::destroy()
{
  Construction::destroy();

  if( _getCity().isValid() )
  {
    TilesArray area = _getCity()->getTilemap().getArea( getTilePos() - TilePos( 2, 2), Size( 5 ) );
    foreach( Tile* tile, area )
    {
      FortificationPtr f = tile->getOverlay().as<Fortification>();
      if( f.isValid()  )
      {
        f->updatePicture( _getCity() );
      }
    }
  }
}

const Picture& Fortification::getPicture(PlayerCityPtr city, TilePos pos,
                                             const TilesArray& tmp) const
{
  // find correct picture as for roads
  Tilemap& tmap = city->getTilemap();

  int directionFlags = 0;  // bit field, N=1, E=2, S=4, W=8

  const TilePos tile_pos = (tmp.empty()) ? getTilePos() : pos;

  if (!tmap.isInside(tile_pos))
    return Picture::load( ResourceGroup::aqueduct, 121 );

  TilePos tile_pos_d[countDirection];
  bool is_border[countDirection] = { 0 };
  bool is_busy[countDirection] = { 0 };

  tile_pos_d[north] = tile_pos + TilePos(  0,  1);
  tile_pos_d[east]  = tile_pos + TilePos(  1,  0);
  tile_pos_d[south] = tile_pos + TilePos(  0, -1);
  tile_pos_d[west]  = tile_pos + TilePos( -1,  0);
  tile_pos_d[northEast] = tile_pos + TilePos( 1, 1 );
  tile_pos_d[southEast] = tile_pos + TilePos( 1, -1 );
  tile_pos_d[southWest] = tile_pos + TilePos( -1, -1 );
  tile_pos_d[northWest] = tile_pos + TilePos( -1, 1 );


  // all tiles must be in map range
  for (int i = 0; i < countDirection; ++i)
  {
    is_border[i] = !tmap.isInside(tile_pos_d[i]);
    if (is_border[i])
      tile_pos_d[i] = tile_pos;
  }

  // get overlays for all directions
  TileOverlayPtr overlay_d[countDirection];
  overlay_d[north] = tmap.at( tile_pos_d[north] ).getOverlay();
  overlay_d[east] = tmap.at( tile_pos_d[east]  ).getOverlay();
  overlay_d[south] = tmap.at( tile_pos_d[south] ).getOverlay();
  overlay_d[west] = tmap.at( tile_pos_d[west]  ).getOverlay();
  overlay_d[northEast] = tmap.at( tile_pos_d[northEast]  ).getOverlay();
  overlay_d[southEast] = tmap.at( tile_pos_d[southEast]  ).getOverlay();
  overlay_d[southWest] = tmap.at( tile_pos_d[southWest]  ).getOverlay();
  overlay_d[northWest] = tmap.at( tile_pos_d[northWest]  ).getOverlay();

  // if we have a TMP array with wall, calculate them
  if( !tmp.empty())
  {
    for( TilesArray::const_iterator it = tmp.begin(); it != tmp.end(); ++it)
    {
      if( (*it)->getOverlay().isNull()
          || (*it)->getOverlay()->getType() != building::fortification )
        continue;

      TilePos rpos = (*it)->getIJ();
      int i = (*it)->getI();
      int j = (*it)->getJ();

      if( (pos + TilePos( 0, 1 )) == rpos ) is_busy[north] = true;
      else if(i == pos.getI() && j == (pos.getJ() - 1)) is_busy[south] = true;
      else if(j == pos.getJ() && i == (pos.getI() + 1)) is_busy[east] = true;
      else if(j == pos.getJ() && i == (pos.getI() - 1)) is_busy[west] = true;
      else if((pos + TilePos(1, 1)) == rpos ) is_busy[northEast] = true;
      else if((pos + TilePos(1, -1)) == rpos ) is_busy[southEast] = true;
      else if((pos + TilePos(-1, -1)) == rpos ) is_busy[southWest] = true;
      else if((pos + TilePos(-1, 1)) == rpos ) is_busy[northWest] = true;
    }
  }

  // calculate directions
  for (int i = 0; i < countDirection; ++i)
  {
    if (!is_border[i] &&
        ( (overlay_d[i].isValid() && overlay_d[i]->getType() == building::fortification) || is_busy[i]))
    {
      switch (i)
      {
      case north: directionFlags += 0x1; break;
      case east:  directionFlags += 0x2; break;
      case south: directionFlags += 0x4; break;
      case west:  directionFlags += 0x8; break;
      case northEast: directionFlags += 0x10; break;
      case southEast: directionFlags += 0x20; break;
      case southWest: directionFlags += 0x40; break;
      case northWest: directionFlags += 0x80; break;
      default: break;
      }
    }
  }

  bool towerNorth = tmap.at( pos + TilePos( 0, 1 ) ).getOverlay().is<Tower>();

  Fortification& th = *const_cast< Fortification* >( this );
  Point mainPicOffset;
  th._fgPicturesRef().clear();
  int index;
  switch( directionFlags & 0xf )
  {  
  case 0: index = 178; break;  // no neighbours!

  case 1: index = 178; // N
  break;

  case 2: index = 178; // E
  break;

  case 3: index = 157;// N + E
    /*if( (directionFlags & 0x40 ) == 0x40 )
    {
      th._fgPicturesRef().push_back( Picture::load( ResourceGroup::wall, 174 ));
      th._fgPicturesRef().back().addOffset( -28, 0 );
    }
    else
    {
      th._fgPicturesRef().push_back( Picture::load( ResourceGroup::wall, 157 ));
      th._fgPicturesRef().back().addOffset( -28, 0 );
    }*/
  break;

  case 4: index = 162; // S
  break;

  case 5: index = 156;  // N + S
  break;

  case 6: index = 175; // E + S
    mainPicOffset = Point( 0, 12 );
    if( (directionFlags & 0x80) == 0x80 )
    {
      index = 169;
    }
  break;

  case 8: index = 178; // W
    /*if( (directionFlags & 0x80) == 0x80 )
    {
      index = 173;
    }

    if( towerNorth )
    {
      index = 164;
    }
    else
    {
      mainPicOffset = Point( 0, 17 );
      if( (directionFlags & 0x40) == 0x40 )
      {
        th._fgPicturesRef().push_back( Picture::load( ResourceGroup::wall, 174 ));
        th._fgPicturesRef().back().addOffset( -28, 2 );
      }
      else
      {
        th._fgPicturesRef().push_back( Picture::load( ResourceGroup::wall, 153 ));
        th._fgPicturesRef().back().addOffset( -28, 0 );
      }
      th._fgPicturesRef().push_back( Picture::load( ResourceGroup::wall, 168 ));
      th._fgPicturesRef().back().addOffset( 10, -8 );
      th._fgPicturesRef().push_back( Picture::load( ResourceGroup::wall, 164 ));
      th._fgPicturesRef().back().addOffset( 0, -13 );
    }*/
  break;

  case 9: index = 164; // N + W
    /*if( (directionFlags & 0x40) == 0x40 )
    {
      th._fgPicturesRef().push_back( Picture::load( ResourceGroup::wall, 174 ));
      th._fgPicturesRef().back().addOffset( -28, 2 );
    }
    else
    {
      th._fgPicturesRef().push_back( Picture::load( ResourceGroup::wall, 153 ));
      th._fgPicturesRef().back().addOffset( -28, 0 );
    }

    if( (directionFlags & 0x80) == 0x80 )
    {}
    else
    {
      index = 172;
    }

    th._fgPicturesRef().push_back( Picture::load( ResourceGroup::wall, 156 ));
    th._fgPicturesRef().back().addOffset( 28, 0 );

    th._fgPicturesRef().push_back( Picture::load( ResourceGroup::wall, 164 ));
    th._fgPicturesRef().back().addOffset( 0, -13 );*/
  break;

  case 10: index = 178; // E + W
    /*if( towerNorth )
    {
      index = 152;
    }

    if( (directionFlags & 0x40) == 0x40 )
    {
      th._fgPicturesRef().push_back( Picture::load( ResourceGroup::wall, 174 ));
      th._fgPicturesRef().back().addOffset( -28, 0 );
    }
    else
    {
      th._fgPicturesRef().push_back( Picture::load( ResourceGroup::wall, 184 ));
      th._fgPicturesRef().back().addOffset( -28, 0 );
    }*/
  break;

  case 12: index = 162; // S + W
  break;

  case 14: index = 173; // E + S + W
  break;

  case 11: index = 172; // N + E + W
    if( (directionFlags&0x80) == 0x80 )
    {
      index = 152;
    }

    if( (directionFlags & 0x40 ) == 0x40 )
    {
      th._fgPicturesRef().push_back( Picture::load( ResourceGroup::wall, 174 ));
      th._fgPicturesRef().back().addOffset( -28, 0 );
    }
    else
    {
      th._fgPicturesRef().push_back( Picture::load( ResourceGroup::wall, 153 ));
      th._fgPicturesRef().back().addOffset( -28, 0 );
    }
  break;

  case 13: index = 156; // W + S + N
    if( (directionFlags&0x80) == 0x80 )
    {
      index = 152;
    }
  break;

  case 7: index = 171;// N + E + S
    //_fgPicturesRef().push_back( Picture::load( ResourceGroup::wall, 156 ));
    //_fgPicturesRef().back().addOffset( -28, 0 );
  break;

  case 15: index = 172; // N + S + E + W (crossing)
    if( (directionFlags&0x80) == 0x80 )
    {
      index = 152;
    }
  break;

  default:
    index = 178; // it's impossible, but ...
    Logger::warning( "Impossible direction on wall building [%d,%d]", pos.getI(), pos.getJ() );
  }

  th._tmpPicture = Picture::load( ResourceGroup::wall, index );
  th._tmpPicture.addOffset( mainPicOffset.getX(), mainPicOffset.getY() );
  return _tmpPicture;
}

void Fortification::updatePicture(PlayerCityPtr city)
{
  setPicture( getPicture( city, getTilePos(), TilesArray() ) );
}
