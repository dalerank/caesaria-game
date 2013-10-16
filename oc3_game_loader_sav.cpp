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

#include "oc3_game_loader_sav.hpp"
#include "oc3_tile.hpp"
#include "oc3_exception.hpp"
#include "oc3_positioni.hpp"
#include "oc3_landoverlayfactory.hpp"
#include "oc3_game.hpp"
#include "pkwareinputstream.hpp"
#include "oc3_city.hpp"
#include "oc3_tilemap.hpp"

#include <fstream>
#include <limits>
#include <climits>
#include <stdint.h>

class GameLoaderC3Sav::Impl
{
public:

};

GameLoaderC3Sav::GameLoaderC3Sav()
{

}

void SkipCompressed( std::fstream& f )
{
  uint32_t tmp;
  f.read((char*)&tmp, 4);
  f.seekg(tmp, std::ios::cur);
}

bool GameLoaderC3Sav::load(const std::string& filename, Game& game )
{
  std::fstream f(filename.c_str(), std::ios::in | std::ios::binary);
 
  uint32_t tmp;

  // need to rewrite better
  short int     *pGraphicGrid = (short int     *)malloc(52488);
  unsigned char *pEdgeGrid    = (unsigned char *)malloc(26244);
  short int     *pTerrainGrid = (short int     *)malloc(52488);
  unsigned char *pRndmTerGrid = (unsigned char *)malloc(26244);
  unsigned char *pRandomGrid  = (unsigned char *)malloc(26244);
  unsigned char *pZeroGrid    = (unsigned char *)malloc(26244);
  
  if( pGraphicGrid == NULL || pEdgeGrid == NULL || pTerrainGrid == NULL ||
      pRndmTerGrid == NULL || pRandomGrid == NULL || pZeroGrid == NULL )
  {
    THROW("NOT ENOUGH MEMORY!!!! FATAL");
  }    
  
  if (!f.is_open())
    THROW("can't open file");
  
  f.read((char*)&tmp, 4); // read dummy
  
  f.read((char*)&tmp, 4); // read scenario flag
  
  try
  {
    f.read((char*)&tmp, 4); // read length of compressed chunk
    std::cout << "length of compressed ids is " << tmp << std::endl;
    PKWareInputStream *pk = new PKWareInputStream(&f, false, tmp);
    for (int i = 0; i < 162 * 162; i++)
    {
      pGraphicGrid[i] = pk->readShort();
    }
    pk->empty();
    delete pk;
    
    f.read((char*)&tmp, 4); // read length of compressed chunk
    std::cout << "length of compressed egdes is " << tmp << std::endl;
    pk = new PKWareInputStream(&f, false, tmp);
    for (int i = 0; i < 162 * 162; i++)
    {
      pEdgeGrid[i] = pk->readByte();
    }
    pk->empty();
    delete pk;
    
    SkipCompressed(f); // skip building ids
    
    f.read((char*)&tmp, 4); // read length of compressed chunk
    std::cout << "length of compressed terraindata is " << tmp << std::endl;
    pk = new PKWareInputStream(&f, false, tmp);
    for (int i = 0; i < 162 * 162; i++)
    {
      pTerrainGrid[i] = pk->readShort();
    }
    pk->empty();
    delete pk;
    
    SkipCompressed(f);
    SkipCompressed(f);
    SkipCompressed(f);
    SkipCompressed(f);
    
    f.read((char*)pRandomGrid, 26244);
    
    SkipCompressed(f);
    SkipCompressed(f);
    SkipCompressed(f);
    SkipCompressed(f);
    SkipCompressed(f);
    
    // here goes walkers array
    f.read((char*)&tmp, 4); // read length of compressed chunk
    std::cout << "length of compressed walkers data is " << tmp << std::endl;
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
    f.seekg(1312, std::ios::cur);
    char climate;
    f.read(&climate, 1);
    
    // here goes the WORK!
    
    CityPtr oCity = game.getCity();
    oCity->setClimate((ClimateType)climate);
    Tilemap& oTilemap = oCity->getTilemap();
    
    oTilemap.resize(size);

    oCity->setCameraPos( TilePos( 0, 0 ) );
    
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
      tile.setPicture( TileHelper::convId2PicName( pGraphicGrid[index] ) );
      tile.setOriginalImgId( pGraphicGrid[index] );
      TileHelper::decode( tile, pTerrainGrid[index] );
    }
  }    
    
  }
  catch(PKException)
  {
    THROW("fatal error when unpacking");
  }

  f.close();

  return true;
}

bool GameLoaderC3Sav::isLoadableFileExtension( const std::string& filename )
{
  return filename.substr( filename.size() - 4, -1 ) == ".sav";
}
