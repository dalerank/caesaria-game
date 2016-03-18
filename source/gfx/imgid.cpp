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

#include "imgid.hpp"
#include "game/resourcegroup.hpp"
#include "core/logger.hpp"
#include "core/utils.hpp"

using namespace direction;

namespace gfx
{

namespace imgid
{

std::string toResource( const unsigned int imgId )
{
  // example: for land1a_00004, pfx=land1a and id=4
  std::string res_pfx;  // resource name prefix
  int res_id = imgId;   // id of resource

  if( imgId < 245 )
  {
    res_pfx = ResourceGroup::plateau;
    res_id = imgId - 200;
  }
  else if( imgId < 548 )
  {
    res_pfx = config::rc.land1a;
    res_id = imgId - 244;
  }
  else if( imgId < 779 )
  {
    res_pfx = config::rc.land2a;
    res_id = imgId - 547;
  }
  else if( imgId < 871)
  {
    res_pfx = config::rc.land3a;
    res_id = imgId - 778;
  }
  else
  {
    res_pfx = config::rc.land1a;
    res_id = 0;

    if (imgId == 0xb10 || imgId == 0xb0d)
    {
      res_pfx = ResourceGroup::housing;
      res_id = 51;
    } // TERRIBLE HACK!

    Logger::warning( "!!! TileHelper unknown image Id={} ", imgId );
  }

  std::string ret_str = utils::format( 0xff, "%s_%05d", res_pfx.c_str(), res_id );
  return ret_str;
}

int fromResource( const std::string& pic_name )
{
  // example: for land1a_00004, return 244+4=248
  std::string res_pfx;  // resource name prefix = land1a
  int res_id = 0;   // idx of resource = 4

  // extract the name and idx from name (ex: [land1a, 4])
  int pos = pic_name.find("_");
  res_pfx = pic_name.substr(0, pos);
  std::stringstream ss(pic_name.substr(pos+1));
  ss >> res_id;

  if (res_pfx == ResourceGroup::plateau )
  {
    res_id += 200;
  }
  else if (res_pfx == config::rc.land1a) { res_id += 244; }
  else if (res_pfx == config::rc.land2a) { res_id += 547; }
  else if (res_pfx == config::rc.land3a) { res_id += 778; }
  else
  {
    Logger::warning( "!!! TileHelper unknown image " + pic_name );
    res_id = 0;
  }

  return res_id;
}

Picture toPicture(const unsigned int imgId)
{
  std::string picname = toResource( imgId );
  return Picture( picname );
}

}//end namespace imgid

}//end namespace gfx
