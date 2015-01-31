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
  /*std::fstream f(filename.c_str(), std::ios::in | std::ios::binary);

  unsigned int i = 0;
  f.seekg(kClimate, std::ios::beg);
  f.read((char*)&i, 1);

  f.close();

  return i; */
  return -1;
}

std::string C3Sav::restartFile() const { return _d->restartFile; }

bool C3Sav::load(const std::string& filename, Game& game)
{
  std::fstream f(filename.c_str(), std::ios::in | std::ios::binary);

  //int climateType = getClimateType( filename );
  //game.city()->setClimate( (ClimateType)climateType );

  _d->restartFile = filename;

  _d->loadCity(f, game );

  game.city()->setOption( PlayerCity::adviserEnabled, 1 );

  f.close();

  return true;
}

bool C3Sav::Impl::loadCity( std::fstream& f, Game& game )
{ 
  uint32_t tmp;

  // need to rewrite better
  std::vector<short int> graphicGrid; graphicGrid.resize( 26244, 0 );
  std::vector<unsigned char> edgeGrid; edgeGrid.resize( 26244, 0 );
  std::vector<short int> terrainGrid; terrainGrid.resize( 26244, 0 );
  std::vector<unsigned char> rndmTerGrid; rndmTerGrid.resize(26244, 0);
  std::vector<unsigned char> randomGrid; randomGrid.resize( 26244, 0 );
  std::vector<unsigned char> zeroGrid; zeroGrid.resize( 26244, 0 );
    
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
    for (int i = 0; i < 162 * 162; i++)
    {
      graphicGrid[i] = pk->readShort();
    }
    pk->empty();
    delete pk;
    
    f.read((char*)&tmp, 4); // read length of compressed chunk
    Logger::warning( "GameLoaderC3Sav: length of compressed egdes is %d", tmp );
    pk = new PKWareInputStream(&f, false, tmp);
    for (int i = 0; i < 162 * 162; i++)
    {
      edgeGrid[i] = pk->readByte();
    }
    pk->empty();
    delete pk;
    
    SkipCompressed(f); // skip building ids
    
    f.read((char*)&tmp, 4); // read length of compressed chunk
    Logger::warning( "GameLoaderC3Sav: length of compressed terraindata is %d", tmp );
    pk = new PKWareInputStream(&f, false, tmp);
    for (int i = 0; i < 162 * 162; i++)
    {
      terrainGrid[i] = pk->readShort();
    }
    pk->empty();
    delete pk;
    
    SkipCompressed(f);
    SkipCompressed(f);
    SkipCompressed(f);
    SkipCompressed(f);
    
    f.read((char*)&randomGrid[0], 26244); 
    
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
    oCity->setClimate((ClimateType)climate);
    
    // here goes the WORK!
    
    
  // loads the graphics map
  int border_size = (162 - size) / 2;

  std::map< int, std::map< int, unsigned char > > edgeData;

  game.city()->setCameraPos( TilePos( size/2, size/2 ) );

  for (int itA = 0; itA < size; ++itA)
  {
    for (int itB = 0; itB < size; ++itB)
    {
      int i = itB;
      int j = size - itA - 1;

      int index = 162 * (border_size + itA) + border_size + itB;

      Tile& tile = oTilemap.at(i, j);

      unsigned int imgId = graphicGrid[index];
      Picture pic = imgid::toPicture( imgId );

      if( pic.isValid() )
      {
        tile.setPicture( pic );
        tile.setOriginalImgId( imgId );
      }
      else
      {
        TileOverlay::Type ovType = LoaderHelper::convImgId2ovrType( imgId );
        if( ovType == constants::objects::unknown )
        {
          Logger::warning( "!!! GameLoaderC3Sav: Unknown building %x at [%d,%d]", imgId, i, j );
        }

        baseBuildings[ tile.pos() ] = imgId;
        pic = Picture::load( ResourceGroup::land1a, 230 + math::random( 57 ) );
        tile.setPicture( pic );
        tile.setOriginalImgId( imgid::fromResource( pic.name() ) );
      }

      edgeData[ i ][ j ] = edgeGrid[index];
      tile::decode( tile, terrainGrid[index] );
      tile::fixPlateauFlags( tile );
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
						for (dj = 0; dj < 5; ++dj)
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
            oTilemap.at(master.i() + di, master.j() + dj).setMasterTile(&master);
          }
        }
      }

      // Check if it is building and type of building
      //if (ttile.getMasterTile() == NULL)      
      std::map<TilePos, unsigned int>::iterator bbIt = baseBuildings.find( TilePos( i, j ) );
      unsigned int bbImgId = bbIt == baseBuildings.end() ? 0 : bbIt->second;

      Tile& tile = oTilemap.at( i, j );
      Tile* masterTile = tile.masterTile();
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
