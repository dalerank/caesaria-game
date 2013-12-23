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

#include "loader_map.hpp"
#include "gfx/tile.hpp"
#include "city/city.hpp"
#include "game.hpp"
#include "core/exception.hpp"
#include "objects/objects_factory.hpp"
#include "core/stringhelper.hpp"
#include "gfx/tilemap.hpp"
#include "world/empire.hpp"
#include "core/logger.hpp"
#include "objects/constants.hpp"
#include "objects/constants.hpp"

using namespace constants;

#include <fstream>

class GameLoaderC3Map::Impl
{
public:
  static const int kGraphicGrid = 0x0;
  static const int kEdgeGrid    = 0xcd08;
  static const int kTerrainGrid = 0x1338c;
  static const int kRndmTerGrid = 0x20094;
  static const int kRandomGrid  = 0x26718;
  static const int kZeroGrid    = 0x2cd9c;
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

  void loadCity(std::fstream& f, PlayerCityPtr oCity );

  void decodeTerrain(Tile &oTile, PlayerCityPtr city );

  void initClimate(std::fstream &f, PlayerCityPtr ioCity);
  void initCameraStartPos(std::fstream &f, PlayerCityPtr ioCity);

  void initEntryExit(std::fstream &f, PlayerCityPtr ioCity);
};

bool GameLoaderC3Map::load(const std::string& filename, Game& game)
{
  std::fstream f(filename.c_str(), std::ios::in | std::ios::binary);

  _d->initClimate(f, game.getCity() );

  _d->loadCity(f, game.getCity() );

  _d->initEntryExit(f, game.getCity());

  _d->initCameraStartPos(f, game.getCity() );

  game.getEmpire()->setCitiesAvailable( true );

  f.close();

  return true;
}

GameLoaderC3Map::GameLoaderC3Map() : _d( new Impl )
{

}

bool GameLoaderC3Map::isLoadableFileExtension( const std::string& filename )
{
  return filename.substr( filename.size() - 4, -1 ) == ".map";
}

void GameLoaderC3Map::Impl::loadCity(std::fstream& f, PlayerCityPtr oCity)
{
  Tilemap& oTilemap = oCity->getTilemap();

  /* get number of city */

  f.seekg(kLocation, std::ios::beg);
  unsigned int location=0;
  f.read((char*)&location, 1);
  Logger::warning( "Location of city is %d", (int)(location+1) );

  std::string cityName = "";
  switch( location+1 ) {
    case 1: case 29: cityName = "Lugdunum"; break;
    case 2: cityName = "Corinthus"; break;
    case 3: case 37: cityName = "Londinium"; break;
    case 4: case 13: case 28: cityName = "Mediolanum"; break;
    case 5: case 40: cityName = "Lindum"; break;
    case 6: cityName = "Toletum"; break;
    case 7: case 33: cityName = "Valentia"; break;
    case 8: case 35: cityName = "Caesarea"; break;
    case 9: case 30: cityName = "Carthago"; break;
    case 10: cityName = "Cyrene"; break;
    case 11: case 15: case 25: cityName = "Tarraco"; break;
    case 12: cityName = "Hierosolyma"; break;
    case 14: case 26: cityName = "Syracusae"; break;
    case 16: case 31: cityName = "Tarsus"; break;
    case 17: case 32: cityName = "Tingis"; break;
    case 18: cityName = "Augusta Trevorum"; break;
    case 19: cityName = "Carthago Nova"; break;
    case 20: cityName = "Leptis Magna"; break;
    case 21: cityName = "Athenae"; break;
    case 22: cityName = "Brundisium"; break;
    case 23: cityName = "Capua"; break;
    case 24: cityName = "Tarentum"; break;
    case 27: cityName = "Miletus"; break;
    case 34: cityName = "Lutetia"; break;
    case 36: cityName = "Sarmizegetusa"; break;
    case 38: cityName = "Damascus"; break;
    case 39: cityName = "Massilia"; break;
  }

  oCity->setName( cityName );

  f.seekg(kSize, std::ios::beg);

  int size;  // 32bits
  int size_2;
  f.read((char*)&size,   4);
  f.read((char*)&size_2, 4);
  Logger::warning( "Map size is %d", size );

  if (size != size_2)
  {
    THROW("Horisontal and vertical map sizes are different!");
  }

  oTilemap.resize(size);

  // need to rewrite better
  ScopedPtr<short> pGraphicGrid( new short[26244] );
  ScopedPtr<unsigned char> pEdgeGrid( new unsigned char[26244] );
  ScopedPtr<short> pTerrainGrid( new short[26244] );
  ScopedPtr<unsigned char> pRndmTerGrid( new unsigned char[26244] );
  ScopedPtr<unsigned char> pRandomGrid( new unsigned char[26244] );
  ScopedPtr<unsigned char> pZeroGrid( new unsigned char[26244] );
  
  if( pGraphicGrid.isNull() || pEdgeGrid.isNull() || pTerrainGrid.isNull() ||
      pRndmTerGrid.isNull() || pRandomGrid.isNull() || pZeroGrid.isNull() )
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
  f.seekg(kZeroGrid, std::ios::beg);
  f.read((char*)pZeroGrid.data(), 26244);

  std::map< int, std::map< int, unsigned char > > edgeData;

  // loads the graphics map
  int border_size = (162 - size) / 2;

  for (int itA = 0; itA < size; ++itA)
  {
    for (int itB = 0; itB < size; ++itB)
    {
      int i = itB;
      int j = size - itA - 1;

      int index = 162 * (border_size + itA) + border_size + itB;  

      Tile& tile = oTilemap.at(i, j);
      tile.setPicture( TileHelper::convId2PicName( pGraphicGrid.data()[index] ) );
      tile.setOriginalImgId( pGraphicGrid.data()[index] );

      edgeData[ i ][ j ] =  pEdgeGrid.data()[index];
      TileHelper::decode( tile, pTerrainGrid.data()[index] );
    }    
  }

  for (int i = 0; i < size; ++i)
  {
    for (int j = 0; j < size; ++j)
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
	
				Logger::warning( "Multi-tile x %d at (%d,%d)", size, i, j );
	      	
	      Tile& master = oTilemap.at(i, j - size + 1);
      	
        Logger::warning( "Master will be at (%d,%d)", master.getI(), master.getJ() );
      	
	      for (int di = 0; di < size; ++di)
        {
	        for (int dj = 0; dj < size; ++dj)
	        {
	            oTilemap.at(master.getI() + di, master.getJ() + dj).setMasterTile(&master);
	        }
        }
    	
        Logger::warning( " decoding " );
      }
      
      // Check if it is building and type of building
      //if (ttile.getMasterTile() == NULL)
      decodeTerrain( oTilemap.at( i, j ), oCity );
    }
  }
}

void GameLoaderC3Map::Impl::decodeTerrain(Tile &oTile, PlayerCityPtr city )
{
  if (!oTile.isMasterTile() && oTile.getMasterTile()!=NULL)
    return;
  
  TileOverlayPtr overlay; // This is the overlay object, if any

  if( oTile.getFlag( Tile::tlRoad ) )   // road
  {
    overlay = TileOverlayFactory::getInstance().create( construction::road );
  }
  else /*if( oTile.getFlag( Tile::tlBuilding ) )*/
  {
    switch ( oTile.getOriginalImgId() )
    {
      case 0xb0e:
      case 0xb0f:
      case 0xb0b:
      case 0xb0c:
        overlay = TileOverlayFactory::getInstance().create( building::nativeHut ).as<TileOverlay>();
      break;

      case 0xb10:
      case 0xb0d:
        overlay =  TileOverlayFactory::getInstance().create( building::nativeCenter ).as<TileOverlay>();
        Logger::warning( "creation of Native center at (%d,%d)", oTile.getI(), oTile.getJ() );
      break;

      case 0xb11:
      case 0xb44:
        overlay = TileOverlayFactory::getInstance().create( building::nativeField ).as<TileOverlay>();
      break;

      case 0x34d:
      case 0x34e:
      case 0x34f:
      case 0x350:
        overlay = TileOverlayFactory::getInstance().create( building::elevation );
        overlay->setPicture( Picture::load( TileHelper::convId2PicName( oTile.getOriginalImgId() ) ) );
      break;
    }
  }

  //terrain.setOverlay( overlay );
  if( overlay != NULL )
  {
    Logger::warning( "Building at ( %d, %d ) with ID: %x", oTile.getI(), oTile.getJ(), oTile.getOriginalImgId() );
    overlay->build( city, oTile.getIJ() );
    city->getOverlays().push_back(overlay);
  }
}

void GameLoaderC3Map::Impl::initClimate(std::fstream &f, PlayerCityPtr ioCity)
{
  // read climate
  unsigned int i = 0;
  f.seekg(kClimate, std::ios::beg);
  f.read((char*)&i, 1);

  ClimateType climate = (ClimateType) i;
  ioCity->setClimate(climate);

  Logger::warning( "Climate type is %d", climate );

  // reload all pics for the given climate
  //   PicLoader &pic_loader = PicLoader::instance();
  //   if (climate == C_CENTRAL)
  //   {
  //      pic_loader.load_archive("resources/pics/pics.zip");
  //   }
  //   else if (climate == C_NORTHERN)
  //   {
  //      pic_loader.load_archive("resources/pics/pics_north.zip");
  //   }
  //   else if (climate == C_DESERT)
  //   {
  //      pic_loader.load_archive("resources/pics/pics_south.zip");
  //   }
}

void GameLoaderC3Map::Impl::initEntryExit(std::fstream &f, PlayerCityPtr ioCity)
{
  unsigned int size = ioCity->getTilemap().getSize();

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

void GameLoaderC3Map::Impl::initCameraStartPos(std::fstream &f, PlayerCityPtr ioCity)
{
  unsigned short int i = 0;
  unsigned short int j = 0;
  f.seekg(kCamera, std::ios::beg);
  f.read((char*)&i, 2);
  f.read((char*)&j, 2);

  ioCity->setCameraPos( TilePos( i, j ) );
}
