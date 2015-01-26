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
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

#include "saveadapter.hpp"
#include "scopedptr.hpp"
#include "json.hpp"
#include "logger.hpp"
#include "vfs/file.hpp"
#include "vfs/path.hpp"
#include "core/variant_map.hpp"

using namespace vfs;

namespace config
{

VariantMap load( const vfs::Path& filename )
{
  Logger::warning( "SaveAdapter: try load model from " + filename.toString() );
  NFile f = NFile::open( filename );

  return load( f );
}

VariantMap load( vfs::NFile f )
{
  if( f.isOpen() )
  {
    ByteArray data = f.readAll();

    bool jsonParsingOk;
    Variant ret = Json::parse( data.toString(), jsonParsingOk );
    if( jsonParsingOk )
    {
      return ret.toMap();
    }
    else
    {
     Logger::warning( "Can't parse file " + f.path().toString() + " error: " + ret.toString() );
     Logger::warning( "Last parsed object is " + Json::lastParsedObject() );
    }
  }
  else
  {
    Logger::warning( "Can't find file " + f.path().toString() );
  }

  return VariantMap();
}

bool save( const VariantMap& options, const vfs::Path& filename )
{
  std::string data = Json::serialize( options.toVariant(), " " );
  if( !data.empty() )
  {
    NFile f = NFile::open( filename, Entity::fmWrite );
    f.write( data.c_str(), data.size() );
    f.flush();
  }

  return true;
}

}
