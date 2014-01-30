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

#include "saveadapter.hpp"
#include "scopedptr.hpp"
#include "json.hpp"
#include "logger.hpp"

#include <fstream>

VariantMap SaveAdapter::load( const vfs::Path& fileName )
{
  std::fstream f( fileName.toString().c_str(), std::ios::in | std::ios::binary);

  f.seekg( 0, std::ios::end);
  //std::ios::pos_type lastPos = f.tellp();
  int lastPos = f.tellp();

  if( lastPos > 0 )
  {
    f.seekg( 0, std::ios::beg );
    ByteArray data;
    data.resize( lastPos );

    f.read( &data[0], lastPos );

    f.close();

    bool jsonParsingOk;
    Variant ret = Json::parse( data.data(), jsonParsingOk );
    if( jsonParsingOk )
    {
      return ret.toMap();
    }
    else
    {
     Logger::warning( "Can't parse file %s: %s", fileName.toString().c_str(), ret.toString().c_str() );
    }
  }
  else
  {
    Logger::warning( "Can't find file %s", fileName.toString().c_str() );
  }

  return VariantMap();
}

SaveAdapter::SaveAdapter()
{

}

bool SaveAdapter::save( const VariantMap& options, const vfs::Path& filename )
{
  std::string data = Json::serialize( options.toVariant(), " " );
  if( !data.empty() )
  {
    std::fstream f( filename.toString().c_str(), std::ios::out | std::ios::binary);
    f.write( data.c_str(), data.size() ); 
    f.close();
  }

  return true;
}
