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

#include "climatemanager.hpp"
#include "core/saveadapter.hpp"
#include "vfs/filesystem.hpp"
#include "gfx/loader.hpp"
#include "resourcegroup.hpp"
#include "core/utils.hpp"
#include "gfx/picture_bank.hpp"
#include "core/logger.hpp"
#include "vfs/entries.hpp"
#include "resourceloader.hpp"
#include "vfs/directory.hpp"
#include "core/variant_map.hpp"
#include "settings.hpp"

using namespace vfs;

namespace game
{

namespace climate
{

void initialize(ClimateType climate)
{
  VariantMap climateArchives = config::load( SETTINGS_RC_PATH( climateModel ) );

  std::string optName;
  if( climate == game::climate::central ) { optName = "central"; }
  else if( climate == game::climate::northen )  { optName = "north"; }
  else if( climate == game::climate::desert ) { optName = "south"; }

  Path archivePath = climateArchives.get( optName ).toString();
  Directory dir = archivePath.directory();

  archivePath = dir.find( archivePath.baseName(), Path::ignoreCase );

  ArchivePtr archive = FileSystem::instance().mountArchive( archivePath );

  if( archive.isNull() )
  {
    Logger::warning( "ClimateManager: can't load file " + archivePath.toString() );
    return;
  }

  ResourceLoader rc;
  NFile atlasInfo = archive->createAndOpenFile( "info" );
  if( atlasInfo.isOpen() )
  {
    rc.loadAtlases( atlasInfo, false );
  }
  else
  {
    rc.loadFiles( archive );
  }
}

}//end namespace climate

}//end namespace game
