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


ScenarioLoader::ScenarioLoader()
{
}


void ScenarioLoader::load(const std::string& filename, Scenario &oScenario)
{
   std::fstream f(filename.c_str(), std::ios::in | std::ios::binary);
   init_climate(f, oScenario.getCity());
   // init_entry_exit(f, oScenario.getCity());

   load_map(f, oScenario);
   f.close();
}


void ScenarioLoader::load_map(std::fstream& f, Scenario &oScenario)
{
   City& oCity = oScenario.getCity();
   Tilemap& oTilemap = oCity.getTilemap();

   f.seekg(0x335b4, std::ios::beg);

   int size;  // 32bits
   f.read((char*)&size, 4);
   std::cout << "map size = " << size << std::endl;
   oTilemap.init(size);

   // loads the graphics map
   int border_size = (162-size)/2;
   for (int itA=0; itA<size; ++itA)
   {
      f.seekg(162*2*(border_size+itA)+2*border_size, std::ios::beg);  // skip empty rows and empty cols

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
      f.seekg(0xcd08 + 162*(border_size+itA)+border_size, std::ios::beg);  // skip empty rows and empty cols

      for (int itB=0; itB<size; ++itB)
      {
         // for each col
         unsigned char edge;  // 8bits
         f.read((char*)&edge, 1);

         int i = itB;
         int j = size-itA-1;

         if (edge==0x00)
         {
            // this is the top of a multi-tile
            Picture& pic = oTilemap.at(i, j).get_picture();
            int tile_size = (pic.get_width()+2)/60;  // size of the multi-tile. the multi-tile is a square.
            // DEBUG
            // std::cout << "multi-tile x" << tile_size << " at " << i << "," << j << std::endl;

            // master is the left-most subtile
            int mi = i;
            int mj = j - tile_size+1;
            Tile& master = oTilemap.at(mi, mj);
            for (int di=0; di<tile_size; ++di)
            {
               // for each subrow of the multi-tile
               for (int dj=0; dj<tile_size; ++dj)
               {
                  // for each subcol of the multi-tile
                  oTilemap.at(mi+di, mj+dj).set_master_tile(&master);
               }
            }
         }
      }
   }


   // loads the terrain map (to know about terrain tiles: tree/rock/water...)
   for (int itA=0; itA<size; ++itA)
   {
      // for each row
      f.seekg(0x1338C + 162*2*(border_size+itA)+2*border_size, std::ios::beg);  // skip empty rows and empty cols

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
	     f.seekg(162 * 2 * (border_size + itA) + 2 * border_size + 2 * itB, std::ios::beg);
	     short int tmp;
	     f.read((char*)&tmp, 2);
	     std::cout.setf(std::ios::hex, std::ios::basefield);
	     std::cout << tmp << std::endl;
	     std::cout.unsetf(std::ios::hex);
	     f.seekg(old);
	     // 0xb0e, 0xb0f - Native Hut
	     // 0xb10 - Native Center
	     // 0xb11 - Native Field
	     switch (tmp)
	     {
	      case 0xb0e:
	      case 0xb0f:
	      {
		  NativeHut* build = new NativeHut();
		  tile.get_terrain().setOverlay((LandOverlay*)build);
		  overlay = tile.get_terrain().getOverlay();
		  overlay->build(i, j);
		  oCity.getOverlayList().push_back(overlay);
	      }
	      break;
	      case 0xb10:
	      {
		  // we need to find master tile
		  if (tile.is_master_tile())
		  {
		    std::cout << "Tile is master" << std::endl;
//		    NativeCenter* build = new NativeCenter();
//		    tile.get_terrain().setOverlay((LandOverlay*)build);
//		    overlay = tile.get_terrain().getOverlay();
//		    overlay->build(i, j);
//		    oCity.getOverlayList().push_back(overlay);
		  }
	      }
	      break;
	      case 0xb11:
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
   f.seekg(0x33ad8, std::ios::beg);
   f.read((char*)&i, 1);

   ClimateType climate = (ClimateType) i;
   ioCity.setClimate(climate);
      
   std::cout << "Climate type is " << climate << std::endl;

   // // reload all pics for the given climate
   // PicLoader &pic_loader = PicLoader::instance();
   // if (climate == C_CENTRAL)
   // {
   //    pic_loader.load_archive("resources/pics/pics.zip");
   // }
   // else if (climate == C_NORTHERN)
   // {
   //    pic_loader.load_archive("resources/pics/pics_north.zip");
   // }
   // else if (climate == C_DESERT)
   // {
   //    pic_loader.load_archive("resources/pics/pics_south.zip");
   // }
}


void ScenarioLoader::init_entry_exit(std::fstream &f, City &ioCity)
{
   int size = ioCity.getTilemap().getSize();

   // init road entry/exit point
   int i = 0;
   int j = 0;
   f.seekg(0x33a8c, std::ios::beg);
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
   f.seekg(0x33ab4, std::ios::beg);
   f.read((char*)&i, 2);
   f.read((char*)&j, 2);
   ioCity.setBoatEntryIJ(i, size-j-1);

   i=0;
   j=0;
   f.read((char*)&i, 2);
   f.read((char*)&j, 2);
   ioCity.setBoatExitIJ(i, size-j-1);

   std::cout << "road entry at:" << ioCity.getRoadEntryI() << "," << ioCity.getRoadEntryJ() << std::endl;
   std::cout << "road exit at:"  << ioCity.getRoadExitI()  << "," << ioCity.getRoadExitJ()  << std::endl;
   std::cout << "boat entry at:" << ioCity.getBoatEntryI() << "," << ioCity.getBoatEntryJ() << std::endl;
   std::cout << "boat exit at:"  << ioCity.getBoatExitI()  << "," << ioCity.getBoatExitJ()  << std::endl;
}


