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
//
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com



#include "scenario_loader.hpp"

#include <iostream>

#include "exception.hpp"
#include "pic_loader.hpp"
#include <oc3_positioni.h>


ScenarioLoader::ScenarioLoader()
{
}


void ScenarioLoader::load(const std::string& filename, Scenario &oScenario)
{
   std::fstream f(filename.c_str(), std::ios::in | std::ios::binary);
   init_climate(f, oScenario.getCity());
  
   load_map(f, oScenario);

   init_entry_exit(f, oScenario.getCity());
   _initEntryExitPicture( oScenario.getCity() );
   f.close();
}

namespace {

  // offsets in map-file

  const int kGraphicGrid = 0x0;
  const int kEdgeGrid    = 0xcd08;
  const int kTerrainGrid = 0x1338c;
  const int kRandomGrid  = 0x26718;
  const int kZeroGrid    = 0x2cd9c;
  const int kCamera      = 0x33428;
  const int kStartDate   = 0x33430;
  const int kLocation    = 0x33434;
  const int kRequests    = 0x3343e;
  const int kInvasions   = 0x334de;
  const int kFunds       = 0x335a8;
  const int kEnemyNation = 0x335ac;
  const int kSize        = 0x335b4;
  const int kDescription = 0x335c4;
  const int kImageID     = 0x33822;
  const int kRank        = 0x33826;
  const int kHerds       = 0x33828;
  const int kPriceChange = 0x338b0;
  const int kEvents      = 0x33928;
  const int kFishing     = 0x33954;
  const int kReqFavour   = 0x33974;
  const int kWheatSupply = 0x339ec;
  const int kBuildings   = 0x339f2;
  const int kRatings     = 0x33a54;
  const int kRatingFlags = 0x33a64;
  const int kTimeLimit   = 0x33a68;
  const int kWinningTime = 0x33a70;
  const int kEarthquakes = 0x33a78;
  const int kPopulation  = 0x33a80;
  const int kIJEarthq    = 0x33a88;
  const int kRoadEntry   = 0x33a8c;
  const int kEnemyEntry  = 0x33a94;
  const int kBoatEntry   = 0x33ab4;
  const int kRescueLoan  = 0x33abc;
  const int kMilestone   = 0x33ac0;
  const int kClimate     = 0x33ad8;
  const int kFlotsam     = 0x33ad9;

}

void ScenarioLoader::load_map(std::fstream& f, Scenario &oScenario)
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

   // loads the graphics map
   int border_size = (162 - size) / 2;
   
   for (int itA = 0; itA < size; ++itA)
   {
      f.seekg(kGraphicGrid + 162 * 2 * (border_size + itA) + 2 * border_size, std::ios::beg);  // skip empty rows and empty cols

      for (int itB=0; itB<size; ++itB)
      {
         int i = itB;
         int j = size-itA-1;

         short int imgId;  // 16bits
         f.read((char*)&imgId, 2);
         decode_img(imgId, oTilemap.at(i, j));
      }
   }

   // loads the edge map (to know about multi-tile graphics)
   for (int itA=0; itA<size; ++itA)
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
            std::cout << "multi-tile x" << tile_size << " at " << i << "," << j << std::endl;

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
         int i = itB;
         int j = size - itA - 1;

         short int terrainBitset;  // 16bits
         f.read((char*)&terrainBitset, 2);
         Tile &tile = oTilemap.at(i, j);
         decode_terrain(terrainBitset, tile);

 
         LandOverlay *overlay = tile.get_terrain().getOverlay();
	 
	 // Check if it is building and type of building
	 if (overlay == NULL && (terrainBitset & 0x8))
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
		  NativeHut* build = new NativeHut();
		  tile.get_terrain().setOverlay((LandOverlay*)build);
		  overlay = tile.get_terrain().getOverlay();
		  overlay->build(i, j);
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
		    NativeCenter* build = new NativeCenter();
		    tile.get_terrain().setOverlay((LandOverlay*)build);
		    overlay = tile.get_terrain().getOverlay();
		    if (overlay != NULL)
		    {
		      overlay->build(i, j);
		      oCity.getOverlayList().push_back(overlay);
		    }
		  }
	      }
	      break;
	      case 0xb11:
	      case 0xb44:
	      {
		  NativeField* build = new NativeField();
		  tile.get_terrain().setOverlay((LandOverlay*)build);
		  overlay = tile.get_terrain().getOverlay();
		  overlay->build(i, j);
		  oCity.getOverlayList().push_back(overlay);
	      }
	      break;
	     }
	 }
	 
         if (overlay != NULL)
         {
            overlay->build(i, j);
            oCity.getOverlayList().push_back(overlay);
         }
      }
   }

}


Picture& ScenarioLoader::get_pic_by_id(const int imgId)
{
   // example: for land1a_00004.png, pfx=land1a and id=4
   std::string res_pfx;  // resource name prefix
   int res_id = imgId;   // id of resource

   if (201<=imgId && imgId < 245)
   {
      res_pfx = "plateau";
      res_id = imgId - 200;
   }
   else if (245<=imgId && imgId < 548)
   {
      res_pfx = "land1a";
      res_id = imgId - 244;
   }
   else if (548<=imgId && imgId < 779)
   {
      res_pfx = "land2a";
      res_id = imgId - 547;
   }
   else if (779<=imgId && imgId < 871)
   {
      res_pfx = "land3a";
      res_id = imgId - 778;
   }
   else
   {
      res_pfx = "land1a";
      res_id = 1;
      
      // std::cout.setf(std::ios::hex, std::ios::basefield);
      // std::cout << "Unknown image Id " << imgId << std::endl;
      // std::cout.unsetf(std::ios::hex);
      
      if (imgId == 0xb10 || imgId == 0xb0d) {res_pfx = "housng1a", res_id = 51;} // TERRIBLE HACK!

      // THROW("Unknown image Id " << imgId);
   }

   return PicLoader::instance().get_picture(res_pfx, res_id);
}


void ScenarioLoader::decode_img(const int imgId, Tile &oTile)
{
   Picture& pic = get_pic_by_id(imgId);
   oTile.set_picture(&pic);
}


void ScenarioLoader::decode_terrain(const int terrainBitset, Tile &oTile)
{
   TerrainTile& terrain = oTile.get_terrain();
   terrain.reset();

   LandOverlay *overlay = NULL; // This is the overlay object, if any

   terrain.decode(terrainBitset);
   if (terrain.isRoad())   // road
   {
      Road *road = new Road();
      overlay = road;
   }
//   else if (terrain.isBuilding())
//   {
//      std::cout << "Building at (" << oTile.getI() << "," << oTile.getJ() << ")" << std::endl;
      
      // How to read building type???
//   }

   terrain.setOverlay(overlay);
}

void ScenarioLoader::init_climate(std::fstream &f, City &ioCity)
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

void ScenarioLoader::init_entry_exit(std::fstream &f, City &ioCity)
{
   int size = ioCity.getTilemap().getSize();

   // init road entry/exit point
   int i = 0;
   int j = 0;
   f.seekg(kRoadEntry, std::ios::beg);
   f.read((char*)&i, 2);
   f.read((char*)&j, 2);
   ioCity.setRoadEntryIJ(i, size-j-1);

   i=0;
   j=0;
   f.read((char*)&i, 2);
   f.read((char*)&j, 2);
   ioCity.setRoadExitIJ(i, size-j-1);

   // init boat entry/exit point
   i = 0;
   j = 0;
   f.seekg(kBoatEntry, std::ios::beg);
   f.read((char*)&i, 2);
   f.read((char*)&j, 2);
   ioCity.setBoatEntryIJ(i, size-j-1);

   i=0;
   j=0;
   f.read((char*)&i, 2);
   f.read((char*)&j, 2);
   ioCity.setBoatExitIJ(i, size-j-1);

//   std::cout << "road entry at:" << ioCity.getRoadEntryI() << "," << ioCity.getRoadEntryJ() << std::endl;
//   std::cout << "road exit at:"  << ioCity.getRoadExitI()  << "," << ioCity.getRoadExitJ()  << std::endl;
//   std::cout << "boat entry at:" << ioCity.getBoatEntryI() << "," << ioCity.getBoatEntryJ() << std::endl;
//   std::cout << "boat exit at:"  << ioCity.getBoatExitI()  << "," << ioCity.getBoatExitJ()  << std::endl;
}

static void initEntryExitTile( const TilePos& tlPos, Tilemap& tileMap, const Uint32 picIdStart, bool exit )
{
    Uint32 idOffset=0;
    TilePos tlOffset;
    if( tlPos.getI() == 0 || tlPos.getI() == tileMap.getSize()-1 )
    {
        tlOffset = TilePos( 0, 1 );
        idOffset = exit 
                    ? ( tlPos.getI() == 0 ? 1 : 3 )
                    : ( tlPos.getI() == 0 ? 3 : 1 );

    }
    else if( tlPos.getJ() == 0 || tlPos.getJ() == tileMap.getSize()-1 )
    {
        tlOffset = TilePos( 1, 0 );
        idOffset = exit 
                    ? ( tlPos.getJ() == 0 ? 2 : 0 )
                    : ( tlPos.getJ() == 0 ? 0 : 2 );
    }

    Tile& signTile = tileMap.at( tlPos + tlOffset );
    signTile.set_picture( &PicLoader::instance().get_picture( "land3a", picIdStart + idOffset ) );
    signTile.get_terrain().setRock( true );
};

void ScenarioLoader::_initEntryExitPicture( City &ioCity )
{
    Tilemap& tileMap = ioCity.getTilemap();

    initEntryExitTile( ioCity.getRoadEntryIJ(), tileMap, 89, false );
    initEntryExitTile( ioCity.getRoadExitIJ(), tileMap, 85, true );    
}

