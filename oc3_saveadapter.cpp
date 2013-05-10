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

#include "oc3_saveadapter.hpp"
#include "oc3_scopedptr.hpp"
#include "oc3_json.hpp"

#include <fstream>

VariantMap SaveAdapter::load( const std::string& fileName )
{
  std::fstream f(fileName.c_str(), std::ios::in | std::ios::binary);

  f.seekg( 0, std::ios::end);
  std::ios::pos_type lastPos = f.tellp();

  f.seekg( 0, std::ios::beg );
  ScopedPtr< char > data( new char[lastPos] );

  f.read( data.data(), lastPos );

  f.close();

  bool jsonParsingOk;
  VariantMap ret = Json::parse( data.data(), jsonParsingOk ).toMap();
  if( jsonParsingOk )
    return ret;

  return VariantMap();
}

SaveAdapter::SaveAdapter()
{

}

bool SaveAdapter::save( const VariantMap& oprions )
{
  return false;
}