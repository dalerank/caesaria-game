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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#include "loader_sav.hpp"
#include "gfx/helper.hpp"
#include "core/exception.hpp"
#include "core/position.hpp"
#include "objects/objects_factory.hpp"
#include "game.hpp"
#include "pkwareinputstream.hpp"
#include "city/city.hpp"
#include "gfx/tilemap.hpp"
#include "core/logger.hpp"
#include "vfs/path.hpp"
#include "game/resourcegroup.hpp"
#include "loaderhelper.hpp"
#include "settings.hpp"
#include "world/empire.hpp"

#include <fstream>
#include <climits>
#include <stdint.h>
#include <map>

using namespace gfx;

namespace game
{

namespace loader
{

static const int kClimate     = 0x33ad8;

class C3Sav::Impl
{
public:
  std::map<TilePos, unsigned int> baseBuildings;

  std::string restartFile;
  bool loadCity(std::fstream& f, Game& game);
  void initEntryExit(std::fstream& f, PlayerCityPtr ioCity);
};

C3Sav::C3Sav() : _d( new Impl )
{
}

void SkipCompressed( std::fstream& f )
{
  uint32_t tmp;
  f.read((char*)&tmp, 4);
  f.seekg(tmp, std::ios::cur);
}

void C3Sav::Impl::initEntryExit(std::fstream &f, PlayerCityPtr ioCity)
{
  unsigned int size = ioCity->tilemap().size();

  const unsigned int savePos = f.tellg();

  // init road entry/exit point
  unsigned short int i = 0;
  unsigned short int j = 0;
  f.seekg(1236, std::ios::cur);
  f.read((char*)&i, 2);
  f.read((char*)&j, 2);

  BorderInfo borderInfo;

  borderInfo.roadEntry = TilePos( i, size - j - 1 );

  f.read((char*)&i, 2);
  f.read((char*)&j, 2);
  borderInfo.roadExit = TilePos( i, size - j - 1 );

  // init boat entry/exit point
  f.seekg(savePos, std::ios::beg);
  f.seekg(1276, std::ios::cur);
  f.read((char*)&i, 2);
  f.read((char*)&j, 2);
  borderInfo.boatEntry = TilePos( i, size - j - 1 );

  f.read((char*)&i, 2);
  f.read((char*)&j, 2);
  borderInfo.boatExit = TilePos( i, size - j - 1);

  ioCity->setBorderInfo( borderInfo );

  f.seekg(savePos, std::ios::beg);
}

int C3Sav::climateType(const std::string& filename)
{
  std::fstream f(filename.c_str(), std::ios::in | std::ios::binary);

  char climateType=-1;
  try
  {
//    uint32_t tmp;
//    uint32_t lengthPkBlock;
    f.seekg( 8, std::ios::cur ); // read dummy
    SkipCompressed(f); // skip graphic ids
    SkipCompressed(f); // skip edge ids

    SkipCompressed(f); // skip building ids
    SkipCompressed(f); // skip terrain ids
    SkipCompressed(f);
    SkipCompressed(f);
    SkipCompressed(f);
    SkipCompressed(f);

    f.seekg( 26244, std::ios::cur );

    SkipCompressed(f);
    SkipCompressed(f);
    SkipCompressed(f);
    SkipCompressed(f);
    SkipCompressed(f);
    SkipCompressed(f); //skip walkers array

    int length;
    f.read((char*)&length, 4); // read next length :-)

    if (length <= 0)
      f.seekg(1200, std::ios::cur);
    else
      f.seekg(length, std::ios::cur);

    SkipCompressed(f);
    SkipCompressed(f);

    // 3x int
    f.seekg(12, std::ios::cur);
    SkipCompressed(f);
    f.seekg(70, std::ios::cur);
    SkipCompressed(f); // skip building list
    f.seekg(208, std::ios::cur);
    SkipCompressed(f); // skip unknown
    f.seekg(788, std::ios::cur); // skip unused data
    f.seekg(4, std::ios::cur); //mapsize

    //initEntryExit()

    f.seekg(1312, std::ios::cur);
    f.read( &climateType, 1);
  }
  catch(...)
  {}

  f.close();

  return climateType;
}

std::string C3Sav::restartFile() const { return _d->restartFile; }

bool C3Sav::load(const std::string& filename, Game& game)
{
  std::fstream f(filename.c_str(), std::ios::in | std::ios::binary);

  _d->restartFile = filename;

  _d->loadCity(f, game );

  game.city()->setOption( PlayerCity::adviserEnabled, 1 );
  game.empire()->setCitiesAvailable( true );

  f.close();

  return true;
}

bool C3Sav::Impl::loadCity( std::fstream& f, Game& game )
{ 
  uint32_t tmp;

  // need to rewrite better
  std::vector<short int> graphicGrid; graphicGrid.resize( gfx::tilemap::c3mapSizeSq, 0 );
  std::vector<unsigned char> edgeGrid; edgeGrid.resize( gfx::tilemap::c3mapSizeSq, 0 );
  std::vector<short int> terrainGrid; terrainGrid.resize( gfx::tilemap::c3mapSizeSq, 0 );
  std::vector<unsigned char> rndmTerGrid; rndmTerGrid.resize(gfx::tilemap::c3mapSizeSq, 0);
  std::vector<unsigned char> randomGrid; randomGrid.resize( gfx::tilemap::c3mapSizeSq, 0 );
  std::vector<unsigned char> zeroGrid; zeroGrid.resize( gfx::tilemap::c3mapSizeSq, 0 );
    
  if( !f.is_open() )
  {
    Logger::warning( "GameLoaderC3Sav: can't open file " );
    return false;
  }
  
  f.read( (char*)&tmp, 4); // read dummy

  std::string cityName = LoaderHelper::getDefaultCityName( tmp );
  game.city()->setName( cityName );
  
  f.read((char*)&tmp, 4); // read scenario flag
  
  try
  {
    f.read((char*)&tmp, 4); // read length of compressed chunk
    Logger::warning( "GameLoaderC3Sav: length of compressed ids is %d", tmp );
    PKWareInputStream *pk = new PKWareInputStream(&f, false, tmp);
    for (int i = 0; i < gfx::tilemap::c3mapSizeSq; i++)
    {
      graphicGrid[i] = pk->readShort();
    }
    pk->empty();
    delete pk;
    
    f.read((char*)&tmp, 4); // read length of compressed chunk
    Logger::warning( "GameLoaderC3Sav: length of compressed egdes is %d", tmp );
    pk = new PKWareInputStream(&f, false, tmp);
    for (int i = 0; i < gfx::tilemap::c3mapSizeSq; i++)
    {
      edgeGrid[i] = pk->readByte();
    }
    pk->empty();
    delete pk;
    
    SkipCompressed(f); // skip building ids
    
    f.read((char*)&tmp, 4); // read length of compressed chunk
    Logger::warning( "GameLoaderC3Sav: length of compressed terraindata is %d", tmp );
    pk = new PKWareInputStream(&f, false, tmp);
    for (int i = 0; i < gfx::tilemap::c3mapSizeSq; i++)
    {
      terrainGrid[i] = pk->readShort();
    }
    pk->empty();
    delete pk;
    
    SkipCompressed(f);
    SkipCompressed(f);
    SkipCompressed(f);
    SkipCompressed(f);
    
    f.read((char*)&randomGrid[0], gfx::tilemap::c3mapSizeSq);
    
    SkipCompressed(f);
    SkipCompressed(f);
    SkipCompressed(f);
    SkipCompressed(f);
    SkipCompressed(f);
    
    // here goes walkers array
    f.read((char*)&tmp, 4); // read length of compressed chunk
    Logger::warning( "GameLoaderC3Sav: length of compressed walkers data is %d", tmp );
    pk = new PKWareInputStream(&f, false, tmp);    
    for (int j = 0; j < 1000; j++)
    {
      pk->skip(10);
      pk->readShort();
      pk->skip(8);
      pk->readByte();
      pk->readByte();
      pk->skip(106);
    }
    pk->empty();
    delete pk;
    int length;
    f.read((char*)&length, 4); // read next length :-)

    if (length <= 0)
      f.seekg(1200, std::ios::cur);
    else
      f.seekg(length, std::ios::cur);
    
    SkipCompressed(f);
    SkipCompressed(f);

    // 3x int
    f.read((char*)&tmp, 4);
    f.read((char*)&tmp, 4);
    f.read((char*)&tmp, 4);
    SkipCompressed(f);
    f.seekg(70, std::ios::cur);
    SkipCompressed(f); // skip building list
    f.seekg(208, std::ios::cur);
    SkipCompressed(f); // skip unknown
    f.seekg(788, std::ios::cur); // skip unused data
    f.read((char*)&tmp, 4); //mapsize

    int size = tmp;
    PlayerCityPtr oCity = game.city();
    Tilemap& oTilemap = oCity->tilemap();

    oCity->resize(size);
    oCity->setCameraPos( TilePos( 0, 0 ) );

    initEntryExit( f, game.city() );

    f.seekg(1312, std::ios::cur);
    char climate;
    f.read(&climate, 1);
    oCity->setOption( PlayerCity::climateType, climate);
    
    // here goes the WORK!

    // loads the graphics map
    int border_size = (gfx::tilemap::c3mapSize - size) / 2;

    std::map< int, std::map< int, unsigned char > > edgeData;

    game.city()->setCameraPos( TilePos( size/2, size/2 ) );

    bool oldgfx = !SETTINGS_STR( c3gfx ).empty();
    oldgfx |= KILLSWITCH( oldgfx );

    for (int itA = 0; itA < size; ++itA)
    {
      for (int itB = 0; itB < size; ++itB)
      {
        int i = itB;
        int j = size - itA - 1;

        int index = gfx::tilemap::c3mapSize * (border_size + itA) + border_size + itB;

        Tile& currentTile = oTilemap.at(i, j);

        unsigned int imgId = graphicGrid[index];
        Picture pic = imgid::toPicture( imgId );

        if( pic.isValid() )
        {
          currentTile.setPicture( pic );
          currentTile.setImgId( imgId );
        }
        else
        {
          object::Type ovType = LoaderHelper::convImgId2ovrType( imgId );
          if( ovType == object::unknown )
          {
            Logger::warning( "!!! GameLoaderC3Sav: Unknown building %x at [%d,%d]", imgId, i, j );
          }
          else
          {
             if( imgId == 0x203 || imgId == 0x207 || imgId == 0x20A || imgId == 0x20D ||
                 imgId == 0x1DA || imgId == 0x1DD || imgId == 0x1E7 || imgId == 0x1e1 ||
                 imgId == 0x1FF || imgId == 0x1FA || imgId == 0x1e2 || imgId == 0x1e9 ||
                 imgId == 0x1f8 || imgId == 0x1e5 || imgId == 0x1e6 || imgId == 0x201 ||
                 imgId == 0x208 || imgId == 0x1ea )
             {
               Picture pic = object::Info::find( oldgfx ? object::meadow : object::terrain ).randomPicture( Size(1) );
               currentTile.setPicture( pic );
               currentTile.setImgId( imgid::fromResource( pic.name() ) );
               currentTile.setFlag( Tile::clearAll, true );
               currentTile.setFlag( Tile::tlMeadow, true );
             }
          }

          baseBuildings[ currentTile.pos() ] = imgId;
          pic.load( ResourceGroup::land1a, 230 + math::random( 57 ) );
          currentTile.setPicture( pic );
          currentTile.setImgId( imgid::fromResource( pic.name() ) );
        }

        edgeData[ i ][ j ] = edgeGrid[index];
        tile::decode( currentTile, terrainGrid[index] );
        tile::fixPlateauFlags( currentTile );
      }
    }

    for (int i = 0; i < size; ++i)
    {
      for (int j = 0; j < size; ++j)
      {
        unsigned char ed = edgeData[ i][ j ];
        if( ed == 0x00)
        {
          int size = 1;

          {
            int dj;
            try
            {
              // find size, 5 is maximal size for building
              for (dj = 0; dj < tilemap::c3bldSize; ++dj)
              {
                int edd = edgeData[ i ][ j - dj ];
                // find bottom left corner
                if (edd == 8 * dj + 0x40)
                {
                  size = dj + 1;
                  break;
                }
              }
            }
            catch(...)
            {
                    size = dj + 1;
            }
          }

          Tile& master = oTilemap.at(i, j - size + 1);

                                  //Logger::warning( "Master will be at (%d,%d)", master.i(), master.j() );
          for (int di = 0; di < size; ++di)
          {
            for (int dj = 0; dj < size; ++dj)
            {
              oTilemap.at(master.i() + di, master.j() + dj).setMaster(&master);
            }
          }
        }

        // Check if it is building and type of building
        //if (ttile.getMasterTile() == NULL)
        std::map<TilePos, unsigned int>::iterator bbIt = baseBuildings.find( TilePos( i, j ) );
        unsigned int bbImgId = bbIt == baseBuildings.end() ? 0 : bbIt->second;

        Tile& tile = oTilemap.at( i, j );
        Tile* masterTile = tile.master();
        if( !masterTile )
          masterTile = &tile;

        if( masterTile->overlay().isNull() )
        {
          LoaderHelper::decodeTerrain( *masterTile, oCity, bbImgId );
        }
      }
    }
    
  }
  catch(PKException)
  {
    THROW("fatal error when unpacking");
  }

  return true;
}

bool C3Sav::isLoadableFileExtension( const std::string& filename )
{
  return vfs::Path( filename ).isMyExtension( ".sav" );
}

}//end namespace loader

}//end namespace game
