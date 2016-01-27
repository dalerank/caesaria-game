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
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "video_config.hpp"
#include "core/variant_map.hpp"
#include "core/saveadapter.hpp"
#include "core/hash.hpp"

namespace movie
{

class Config::Impl
{
public:
  typedef std::map< unsigned int, std::string > Aliases;
  typedef std::list< vfs::Directory > Folders;

  Aliases aliases;
  Folders folders;
  StringArray extensions;

public:
  void resetIfalias( std::string& sampleName );
};

void Config::loadAlias(vfs::Path filename)
{
  VariantMap alias = config::load( filename );
  for( auto& it : alias )
  {
    _d->aliases[ Hash( it.first ) ] = it.second.toString();
  }
}

void Config::addFolder(vfs::Directory dir) {  _d->folders.push_back( dir ); }

vfs::Path Config::realPath(const std::string &movie) const
{
  std::string rmovie = movie;
  _d->resetIfalias( rmovie );

  vfs::Path sPath( rmovie );
  vfs::Path realPath;

  if( realPath.extension().empty() )
  {
    for( auto& dir : _d->folders )
    {
      for( auto& ext : _d->extensions )
      {
        realPath = sPath.changeExtension( ext );
        realPath = dir.find( realPath.baseName(), vfs::Path::ignoreCase );

        if( !realPath.toString().empty() )
          return realPath;
      }
    }
  }
  else
  {
    for( auto& dir : _d->folders )
    {
      realPath = dir/sPath;

      if( realPath.exist() )
        return realPath;
    }
  }

  return vfs::Path();
}

Config::Config() : _d( new Impl )
{
  _d->extensions << ".smk";
  addFolder( vfs::Directory() );
  addFolder( vfs::Path( ":/smk/" ) );
}

Config::~Config() {}

void Config::Impl::resetIfalias(std::string& sampleName)
{
  Aliases::iterator it = aliases.find( Hash( sampleName ) );
  if( it != aliases.end() )
    sampleName = it->second;
}

}//end namespace movie
