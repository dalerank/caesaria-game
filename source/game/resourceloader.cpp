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

#include "resourceloader.hpp"
#include "core/logger.hpp"
#include "vfs/filesystem.hpp"
#include "vfs/directory.hpp"
#include "gfx/picture_bank.hpp"
#include "core/variant_map.hpp"
#include "core/saveadapter.hpp"
#include "game/settings.hpp"
#include "gfx/loader.hpp"

using namespace vfs;

namespace {
const char* archiveDescFile = "info";
const char* atlasListSection = "atlas";
}

class ResourceLoader::Impl
{
public signals:
  Signal1<std::string> onStartLoadingSignal;
};

ResourceLoader::ResourceLoader() : _d( new Impl )
{
}

ResourceLoader::~ResourceLoader(){  }

void ResourceLoader::loadFromModel( Path path2model, const Directory dir )
{
  VariantMap archives = config::load( path2model );
  foreach( a, archives )
  {
    Path absArchivePath( a->second.toString() );

    if( !absArchivePath.exist() )
    {
      Path rpath = a->second.toString();
      absArchivePath = dir/rpath;
    }
    Logger::warning( "ResourceLoader: try mount archive " + absArchivePath.toString() );

    Directory absDir = absArchivePath.directory();
    absArchivePath = absDir.find( absArchivePath.baseName(), Path::ignoreCase );       

    ArchivePtr archive = FileSystem::instance().mountArchive( absArchivePath );

    if( archive.isValid() )
    {
      emit _d->onStartLoadingSignal( a->first );

      NFile archiveInfo = archive->createAndOpenFile( archiveDescFile );
      loadAtlases( archiveInfo, true );

      //FileSystem::instance().unmountArchive( archive );
    }
    else
    {
      Logger::warning( "ResourceLoader: cannot load archive " + absArchivePath.toString() );
    }
  }
}

void ResourceLoader::loadAtlases(vfs::NFile archiveInfo, bool lazy)
{
  if( archiveInfo.isOpen() )
  {
    VariantMap vm = config::load( archiveInfo );

    VariantList atlasNames = vm.get( atlasListSection ).toList();
    foreach( it, atlasNames )
    {
      if( lazy )
      {        
        gfx::PictureBank::instance().addAtlas( it->toString() );
      }
      else
      {
        gfx::PictureBank::instance().loadAtlas( it->toString() );
      }
    }
  }
}

void ResourceLoader::loadFiles(Path path)
{
  ArchivePtr archive = FileSystem::instance().mountArchive( path );
  if( archive.isValid() )
  {
    loadFiles( archive );
    FileSystem::instance().unmountArchive( archive );
  }
}

void ResourceLoader::loadFiles(ArchivePtr archive)
{
  const vfs::Entries::Items& files = archive->entries()->items();
  gfx::PictureBank& pb = gfx::PictureBank::instance();

  foreach( it, files )
  {
    NFile file = archive->createAndOpenFile( it->name );
    if( file.isOpen() )
    {
      gfx::Picture pic = PictureLoader::instance().load( file );
      if( pic.isValid() )
      {
        std::string basename = it->name.baseName().toString();
        pb.setPicture( basename, pic );
      }
    }
  }
}

Signal1<std::string> &ResourceLoader::onStartLoading() { return _d->onStartLoadingSignal;}
