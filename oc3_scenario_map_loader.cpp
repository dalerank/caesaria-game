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

#include "oc3_scenario_map_loader.hpp"
#include "oc3_tile.hpp"
#include "oc3_city.hpp"
#include "oc3_scenario.hpp"
#include "oc3_exception.hpp"
#include "oc3_constructionmanager.hpp"
#include "oc3_stringhelper.hpp"

#include <fstream>

class ScenarioMapLoader::Impl
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

  void loadMap(std::fstream& f, Scenario &oScenario);

  void decodeDerrain(const int terrainBitset, Tile &oTile);

  void initClimate(std::fstream &f, City &ioCity);
  void initCameraStartPos(std::fstream &f, City &ioCity);

  void initEntryExit(std::fstream &f, City &ioCity);
};

bool ScenarioMapLoader::load(const std::string& filename, Scenario &oScenario)
{
  std::fstream f(filename.c_str(), std::ios::in | std::ios::binary);
  _d->initClimate(f, oScenario.getCity());

  _d->loadMap(f, oScenario);

  _d->initEntryExit(f, oScenario.getCity());

  _d->initCameraStartPos(f, oScenario.getCity());

  f.close();

  return true;
}

ScenarioMapLoader::ScenarioMapLoader() : _d( new Impl )
{

}

bool ScenarioMapLoader::isLoadableFileExtension( const std::string& filename )
{
  return filename.substr( filename.size() - 4, -1 ) == ".map";
}

void ScenarioMapLoader::Impl::loadMap(std::fstream& f, Scenario &oScenario)
{
  City& oCity = oScenario.getCity();
  Tilemap& oTilemap = oCity.getTilemap();

  /* get number of city */

  f.seekg(kLocation, std::ios::beg);
  Uint8 location;
  f.read((char*)&location, 1);
  std::cout << "Location of city is " << (int)(location+1) << std::endl;

  /* 1 - Lugdunum
  2 - Corinthus
  3 - Londinium
  4 - Mediolanum
  5 - Lindum
  6 - Toletum
  7 - Valentia
  8 - Caesarea
  9 - Carthago
  10 - Cyrene
  11 - Tarraco
  12 - Hierosolyma
  13 - Mediolanum II
  14 - Syracusae
  15 - Tarraco II
  16 - Tarsus
  17 - Tingis
  18 - Augusta Trevorum
  19 - Carthago Nova
  20 - Leptis Magna
  21 - Athenae
  22 - Brundisium
  23 - Capua
  24 - Tarentum
  25 - Tarraco II
  26 - Syracusae II
  27 - Miletus
  28 - Mediolanum III
  29 - Lugdunum II
  30 - Carthago II
  31 - Tarsus II
  32 - Tingis II
  33 - Valentia II
  34 - Lutetia
  35 - Caesarea II
  36 - Sarmizegetusa
  37 - Londinium II
  38 - Damascus
  39 - Massilia
  40 - Lindum II
  */

  f.seekg(kSize, std::ios::beg);

  int size;  // 32bits
  int size_2;
  f.read((char*)&size,   4);
  f.read((char*)&size_2, 4);
  std::cout << "map size = " << size << std::endl;

  if (size != size_2)
  {
    THROW("Horisontal and vertical map sizes are different!");
  }

  oTilemap.init(size);

  // here also make copy of original arrays in memory

  f.seekg(kGraphicGrid, std::ios::beg);
  f.read((char*)oCity.pGraphicGrid, 52488);
  f.seekg(kEdgeGrid, std::ios::beg);
  f.read((char*)oCity.pEdgeGrid, 26244);
  f.seekg(kTerrainGrid, std::ios::beg);
  f.read((char*)oCity.pTerrainGrid, 52488);
  f.seekg(kRndmTerGrid, std::ios::beg);
  f.read((char*)oCity.pRndmTerGrid, 26244);
  f.seekg(kRandomGrid, std::ios::beg);
  f.read((char*)oCity.pRandomGrid, 26244);
  f.seekg(kZeroGrid, std::ios::beg);
  f.read((char*)oCity.pZeroGrid, 26244);


  // loads the graphics map
  int border_size = (162 - size) / 2;

  for (int itA = 0; itA < size; ++itA)
  {
    f.seekg(kGraphicGrid + 162 * 2 * (border_size + itA) + 2 * border_size, std::ios::beg);  // skip empty rows and empty cols

    for (int itB = 0; itB < size; ++itB)
    {
      int i = itB;
      int j = size-itA-1;

      unsigned short int imgId;  // 16bits

      f.read((char*)&imgId, 2);
      Tile& tile = oTilemap.at(i, j);
      Picture& pic = Picture::load( TerrainTileHelper::convId2PicName( imgId ) );
      tile.set_picture( &pic );
      tile.get_terrain().setOriginalImgId( imgId );
    }
  }

  // loads the edge map (to know about multi-tile graphics)
  for (int itA = 0; itA < size; ++itA)
  {
    // for each row
    f.seekg(kEdgeGrid + 162 * (border_size + itA) + border_size, std::ios::beg);  // skip empty rows and empty cols

    for (int itB = 0; itB < size; ++itB)
    {
      // for each col
      unsigned char edge;  // 8bits
      f.read((char*)&edge, 1);

      int i = itB;
      int j = size - itA - 1;

      if (edge==0x00)
      {
        // this is the top of a multi-tile
        Picture& pic = oTilemap.at(i, j).get_picture();
        int tile_size = (pic.get_width() + 2) / 60;  // size of the multi-tile. the multi-tile is a square.
        // DEBUG
        // std::cout << "multi-tile x" << tile_size << " at " << i << "," << j << std::endl;

        // master is the left-most subtile
        int mi = i;
        int mj = j - tile_size+1;
        Tile& master = oTilemap.at(mi, mj);
        for (int di = 0; di < tile_size; ++di)
        {
          // for each subrow of the multi-tile
          for (int dj = 0; dj < tile_size; ++dj)
          {
            // for each subcol of the multi-tile
            oTilemap.at(mi+di, mj+dj).set_master_tile(&master);
          }
        }
      }
    }
  }


  // loads the terrain map (to know about terrain tiles: tree/rock/water...)
  for (int itA = 0; itA < size; ++itA)
  {
    // for each row
    f.seekg(kTerrainGrid + 162 * 2 * (border_size + itA) + 2 * border_size, std::ios::beg);  // skip empty rows and empty cols

    for (int itB=0; itB<size; ++itB)
    {
      // for each col
      TilePos pos( itB, size - itA - 1 );

      short terrainBitset;  // 16bits

      f.read((char*)&terrainBitset, 2);
      Tile &tile = oTilemap.at( pos );
      decodeDerrain(terrainBitset, tile);

      LandOverlayPtr overlay = tile.get_terrain().getOverlay();

      // Check if it is building and type of building
      if( overlay == NULL && (terrainBitset & 0x8) )
      {
        std::cout << "Building at (" << tile.getI() << "," << tile.getJ() << ")" << " with type ";
        std::streampos old = f.tellg();
        f.seekg(kGraphicGrid + 162 * 2 * (border_size + itA) + 2 * border_size + 2 * itB, std::ios::beg);
        short int tmp;

        f.read((char*)&tmp, 2);
        std::cout.setf(std::ios::hex, std::ios::basefield);
        std::cout << tmp << std::endl;
        std::cout.unsetf(std::ios::hex);
        f.seekg(old);

        // 0xb0e, 0xb0f - Native Hut
        // 0xb10 - Native Center
        // 0xb11 - Native Field

        // NOTHERN:
        // 0xb0b, 0xb0c - Native Hut
        // 0xb0d - Native Center
        // 0xb44 - Native Field
        switch (tmp)
        {	
        case 0xb0e:
        case 0xb0f:
        case 0xb0b:
        case 0xb0c:
          {
            ConstructionPtr hut = ConstructionManager::getInstance().create( B_NATIVE_HUT );
            tile.get_terrain().setOverlay( hut.as<LandOverlay>() );
            overlay = tile.get_terrain().getOverlay();
            overlay->build( pos );
            oCity.getOverlayList().push_back(overlay);
          }
          break;
        case 0xb10:
        case 0xb0d:
          {
            // we need to find master tile
            if (tile.is_master_tile())
            {
              std::cout << "Tile is master" << std::endl;
              ConstructionPtr center = ConstructionManager::getInstance().create( B_NATIVE_CENTER );
              tile.get_terrain().setOverlay( center.as<LandOverlay>() );
              overlay = tile.get_terrain().getOverlay();
              if( overlay != NULL )
              {
                overlay->build( pos );
                oCity.getOverlayList().push_back(overlay);
              }
            }
          }
          break;
        case 0xb11:
        case 0xb44:
          {
            ConstructionPtr field = ConstructionManager::getInstance().create( B_NATIVE_FIELD );
            tile.get_terrain().setOverlay( field.as<LandOverlay>() );
            overlay = tile.get_terrain().getOverlay();
            overlay->build( pos );
            oCity.getOverlayList().push_back(overlay);
          }
          break;                  
        }
      }

      if (overlay != NULL)
      {
        overlay->build( pos );
        oCity.getOverlayList().push_back(overlay);
      }
    }
  }
}

void ScenarioMapLoader::Impl::decodeDerrain(const int terrainBitset, Tile &oTile)
{
  TerrainTile& terrain = oTile.get_terrain();

  terrain.reset();

  LandOverlayPtr overlay; // This is the overlay object, if any

  terrain.decode( terrainBitset );
  if( terrain.isRoad() )   // road
  {
    overlay = ConstructionManager::getInstance().create( B_ROAD ).as<LandOverlay>();
  }
  //   else if (terrain.isBuilding())
  //   {
  //      std::cout << "Building at (" << oTile.getI() << "," << oTile.getJ() << ")" << std::endl;

  // How to read building type???
  //   }


  terrain.setOverlay( overlay );
}

void ScenarioMapLoader::Impl::initClimate(std::fstream &f, City &ioCity)
{
  // read climate
  unsigned int i = 0;
  f.seekg(kClimate, std::ios::beg);
  f.read((char*)&i, 1);

  ClimateType climate = (ClimateType) i;
  ioCity.setClimate(climate);

  std::cout << "Climate type is " << climate << std::endl;

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

void ScenarioMapLoader::Impl::initEntryExit(std::fstream &f, City &ioCity)
{
  unsigned int size = ioCity.getTilemap().getSize();

  // init road entry/exit point
  unsigned short int i = 0;
  unsigned short int j = 0;
  f.seekg(kRoadEntry, std::ios::beg);
  f.read((char*)&i, 2);
  f.read((char*)&j, 2);

  ioCity.setRoadEntry( TilePos( i, size - j - 1 ) );

  i = 0;
  j = 0;
  f.read((char*)&i, 2);
  f.read((char*)&j, 2);
  ioCity.setRoadExit( TilePos( i, size - j - 1 ) );

  // init boat entry/exit point
  i = 0;
  j = 0;
  f.seekg(kBoatEntry, std::ios::beg);
  f.read((char*)&i, 2);
  f.read((char*)&j, 2);
  ioCity.setBoatEntry( TilePos( i, size - j - 1 ) );

  i = 0;
  j = 0;
  f.read((char*)&i, 2);
  f.read((char*)&j, 2);
  ioCity.setBoatExit( TilePos( i, size - j - 1) );

  //std::cout << "road entry at:" << ioCity.getRoadEntryI() << "," << ioCity.getRoadEntryJ() << std::endl;
  //std::cout << "road exit at:"  << ioCity.getRoadExitI()  << "," << ioCity.getRoadExitJ()  << std::endl;
  //std::cout << "boat entry at:" << ioCity.getBoatEntryI() << "," << ioCity.getBoatEntryJ() << std::endl;
  //std::cout << "boat exit at:"  << ioCity.getBoatExitI()  << "," << ioCity.getBoatExitJ()  << std::endl;
}

void ScenarioMapLoader::Impl::initCameraStartPos(std::fstream &f, City &ioCity)
{
  unsigned short int i = 0;
  unsigned short int j = 0;
  f.seekg(kCamera, std::ios::beg);
  f.read((char*)&i, 2);
  f.read((char*)&j, 2);

  ioCity.setCameraPos( TilePos( i, j ) );
}