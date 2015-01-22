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

#include "loader_map.hpp"
#include "gfx/helper.hpp"
#include "city/city.hpp"
#include "game.hpp"
#include "core/exception.hpp"
#include "objects/objects_factory.hpp"
#include "core/utils.hpp"
#include "gfx/tilemap.hpp"
#include "world/empire.hpp"
#include "core/logger.hpp"
#include "objects/constants.hpp"
#include "loaderhelper.hpp"

using namespace constants;
using namespace gfx;

#include <fstream>
#include <map>

namespace game
{

namespace loader
{

class C3Map::Impl
{
public:
  static const int kGraphicGrid = 0x0;
  static const int kEdgeGrid    = 0xcd08;
  static const int kTerrainGrid = 0x1338c;
  static const int kRndmTerGrid = 0x20094;
  static const int kRandomGrid  = 0x26718;
  static const int kElevationGrid=0x2cd9c;
  static const int kCamera      = 0x33428;
  static const int kStartDate   = 0x33430;
  static const int kLocation    = 0x33434;
  static const int kRequests    = 0x3343e;
  static const int kInvasions   = 0x334de;
  static const int kFunds       = 0x335a8;
  static const int kEnemyNation = 0x335ac;
  static const int kSize        = 0x335b4;
  static const int kDescription = 0x335c4;
  static const int kImageID     = 0x33822;
  static const int kRank        = 0x33826;
  static const int kHerds       = 0x33828;
  static const int kPriceChange = 0x338b0;
  static const int kEvents      = 0x33928;
  static const int kFishing     = 0x33954;
  static const int kReqFavour   = 0x33974;
  static const int kWheatSupply = 0x339ec;
  static const int kBuildings   = 0x339f2;
  static const int kRatings     = 0x33a54;
  static const int kRatingFlags = 0x33a64;
  static const int kTimeLimit   = 0x33a68;
  static const int kWinningTime = 0x33a70;
  static const int kEarthquakes = 0x33a78;
  static const int kPopulation  = 0x33a80;
  static const int kIJEarthq    = 0x33a88;
  static const int kRoadEntry   = 0x33a8c;
  static const int kEnemyEntry  = 0x33a94;
  static const int kBoatEntry   = 0x33ab4;
  static const int kRescueLoan  = 0x33abc;
  static const int kMilestone   = 0x33ac0;
  static const int kClimate     = 0x33ad8;
  static const int kFlotsam     = 0x33ad9;

  std::string restartFile;

  void loadCity(std::fstream& f, PlayerCityPtr oCity );

  void initClimate(std::fstream &f, PlayerCityPtr ioCity);
  void initCameraStartPos(std::fstream &f, PlayerCityPtr ioCity);

  void initEntryExit(std::fstream &f, PlayerCityPtr ioCity);
};

bool C3Map::load(const std::string& filename, Game& game)
{
  _d->restartFile = filename;

  std::fstream f(filename.c_str(), std::ios::in | std::ios::binary);

  _d->initClimate(f, game.city() );

  _d->loadCity(f, game.city() );  

  _d->initEntryExit(f, game.city());

  _d->initCameraStartPos(f, game.city() );

  game.city()->setOption( PlayerCity::adviserEnabled, 1 );

  game.empire()->setCitiesAvailable( true );

  f.close();

  return true;
}

int C3Map::climateType(const std::string& filename)
{
  std::fstream f(filename.c_str(), std::ios::in | std::ios::binary);

  unsigned int i = 0;
  f.seekg(Impl::kClimate, std::ios::beg);
  f.read((char*)&i, 1);

  f.close();

  return i;
}

C3Map::C3Map() : _d( new Impl ) {}

bool C3Map::isLoadableFileExtension( const std::string& filename )
{
  return vfs::Path( filename ).isMyExtension( ".map" );
}

std::string C3Map::restartFile() const {  return _d->restartFile; }

void C3Map::Impl::loadCity(std::fstream& f, PlayerCityPtr oCity)
{
  Tilemap& oTilemap = oCity->tilemap();

  /* get number of city */

  f.seekg(kLocation, std::ios::beg);
  unsigned int location=0;
  f.read((char*)&location, 1);
  Logger::warning( "C3MapLoader: location of city is %d", (int)(location) );

  std::string cityName = LoaderHelper::getDefaultCityName( location );
  oCity->setName( cityName );

  f.seekg(kSize, std::ios::beg);

  int map_size;  // 32bits
  int size_2;
  f.read((char*)&map_size,   4);
  f.read((char*)&size_2, 4);
  Logger::warning( "C3MapLoader: map size is %d", map_size );

  if (map_size != size_2)
  {
    THROW("Horizontal and vertical map sizes are different!");
  }

  oCity->resize(map_size);

  // need to rewrite better
  ScopedPtr<short> pGraphicGrid( new short[26244] );
  ScopedPtr<unsigned char> pEdgeGrid( new unsigned char[26244] );
  ScopedPtr<short> pTerrainGrid( new short[26244] );
  ScopedPtr<unsigned char> pRndmTerGrid( new unsigned char[26244] );
  ScopedPtr<unsigned char> pRandomGrid( new unsigned char[26244] );
  ScopedPtr<unsigned char> pElevationGrid( new unsigned char[26244] );

  if( pGraphicGrid.isNull() || pEdgeGrid.isNull() || pTerrainGrid.isNull() ||
      pRndmTerGrid.isNull() || pRandomGrid.isNull() || pElevationGrid.isNull() )
  {
    THROW("NOT ENOUGH MEMORY!!!! FATAL");
  }

  // here also make copy of original arrays in memory

  f.seekg(kGraphicGrid, std::ios::beg);
  f.read((char*)pGraphicGrid.data(), 52488);
  f.seekg(kEdgeGrid, std::ios::beg);
  f.read((char*)pEdgeGrid.data(), 26244);
  f.seekg(kTerrainGrid, std::ios::beg);
  f.read((char*)pTerrainGrid.data(), 52488);
  f.seekg(kRndmTerGrid, std::ios::beg);
  f.read((char*)pRndmTerGrid.data(), 26244);
  f.seekg(kRandomGrid, std::ios::beg);
  f.read((char*)pRandomGrid.data(), 26244);
  f.seekg(kElevationGrid, std::ios::beg);
  f.read((char*)pElevationGrid.data(), 26244);

  std::map< int, std::map< int, unsigned char > > edgeData;

  // loads the graphics map
  int border_size = (162 - map_size) / 2;

  for (int itA = 0; itA < map_size; ++itA)
  {
    for (int itB = 0; itB < map_size; ++itB)
    {
      int i = itB;
      int j = map_size - itA - 1;

      int index = 162 * (border_size + itA) + border_size + itB;

      Tile& tile = oTilemap.at(i, j);
      tile.setPicture( imgid::toResource( pGraphicGrid.data()[index] ) );
      tile.setOriginalImgId( pGraphicGrid.data()[index] );
      //tile.setHeight( pElevationGrid.data()[ index ] );

      edgeData[ i ][ j ] =  pEdgeGrid.data()[index];
      tile::decode( tile, pTerrainGrid.data()[index] );
      tile::fixPlateauFlags( tile );
    }
  }

  for (int i = 0; i < map_size; ++i)
  {
    for (int j = 0; j < map_size; ++j)
    {
      unsigned char ed = edgeData[ i ][ j ];
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

				//Logger::warning( "Multi-tile x %d at (%d,%d)", size, i, j );

	      Tile& master = oTilemap.at(i, j - size + 1);

				//Logger::warning( "Master will be at (%d,%d)", master.i(), master.j() );

	      for (int di = 0; di < size; ++di)
        {
	        for (int dj = 0; dj < size; ++dj)
					{
						oTilemap.at(master.pos() + TilePos( di, dj ) ).setMasterTile(&master);
	        }
        }

        //Logger::warning( " decoding " );
      }

      // Check if it is building and type of building
      //if (ttile.getMasterTile() == NULL)
      LoaderHelper::decodeTerrain( oTilemap.at( i, j ), oCity );
    }
  }
}

void C3Map::Impl::initClimate(std::fstream &f, PlayerCityPtr ioCity )
{
  // read climate
  unsigned int i = 0;
  f.seekg(kClimate, std::ios::beg);
  f.read((char*)&i, 1);

  ClimateType climate = (ClimateType) i;
  ioCity->setClimate(climate);

  Logger::warning( "C3MapLoader: climate type is %d", climate );
}

void C3Map::Impl::initEntryExit(std::fstream &f, PlayerCityPtr ioCity)
{
  unsigned int size = ioCity->tilemap().size();

  // init road entry/exit point
  unsigned short int i = 0;
  unsigned short int j = 0;
  f.seekg(kRoadEntry, std::ios::beg);
  f.read((char*)&i, 2);
  f.read((char*)&j, 2);

  BorderInfo borderInfo;

  borderInfo.roadEntry = TilePos( i, size - j - 1 );

  f.read((char*)&i, 2);
  f.read((char*)&j, 2);
  borderInfo.roadExit = TilePos( i, size - j - 1 );

  // init boat entry/exit point
  f.seekg(kBoatEntry, std::ios::beg);
  f.read((char*)&i, 2);
  f.read((char*)&j, 2);
  borderInfo.boatEntry = TilePos( i, size - j - 1 );

  f.read((char*)&i, 2);
  f.read((char*)&j, 2);
  borderInfo.boatExit = TilePos( i, size - j - 1);

  ioCity->setBorderInfo( borderInfo );

  //std::cout << "road entry at:" << ioCity.getRoadEntryI() << "," << ioCity.getRoadEntryJ() << std::endl;
  //std::cout << "road exit at:"  << ioCity.getRoadExitI()  << "," << ioCity.getRoadExitJ()  << std::endl;
  //std::cout << "boat entry at:" << ioCity.getBoatEntryI() << "," << ioCity.getBoatEntryJ() << std::endl;
  //std::cout << "boat exit at:"  << ioCity.getBoatExitI()  << "," << ioCity.getBoatExitJ()  << std::endl;
}

void C3Map::Impl::initCameraStartPos(std::fstream &f, PlayerCityPtr ioCity)
{
  /*unsigned short int i = 0;
  unsigned short int j = 0;
  f.seekg(kCamera, std::ios::beg);
  f.read((char*)&i, 2);
  f.read((char*)&j, 2);*/
  int mapSize = ioCity->tilemap().size();

  ioCity->setCameraPos( TilePos( mapSize / 2, mapSize / 2 ) );
}

}//end namespace loader

}//end namespace game
