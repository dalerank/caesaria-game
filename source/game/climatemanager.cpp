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
#include "settings.hpp"
#include "vfs/filesystem.hpp"
#include "gfx/loader.hpp"
#include "resourcegroup.hpp"
#include "core/stringhelper.hpp"
#include "gfx/picture_bank.hpp"
#include "core/logger.hpp"

using namespace vfs;

void __appendRange( const std::string& rc, int start, int stop, StringArray& ar )
{
  for( int index=start; index <= stop; index++ )
  {
    ar << StringHelper::format( 0xff, "%s_%05d", rc.c_str(), index );
  }
}

void ClimateManager::initialize(ClimateType climate)
{
  VariantMap climateArchives = SaveAdapter::load( GameSettings::rcpath( "climate.model" ) );

  std::string optName;
  if( climate == climateCentral ) { optName = "central"; }
  else if( climate == climateNorthen )  { optName = "north"; }
  else if( climate == climateDesert ) { optName = "south"; }

  Path archivePath = GameSettings::rcpath( climateArchives.get( optName ).toString() );
  ArchivePtr archive = FileSystem::instance().mountArchive( archivePath );

  if( archive.isNull() )
  {
    Logger::warning( "ClimateManager: can't load file " + archivePath.toString() );
    return;
  }

  StringArray fileNames;
  __appendRange( ResourceGroup::housing, 1, 51, fileNames );
  __appendRange( ResourceGroup::land1a, 1, 303, fileNames );
  __appendRange( ResourceGroup::land2a, 93, 110, fileNames );
  __appendRange( ResourceGroup::land2a, 119, 148, fileNames );
  __appendRange( ResourceGroup::land3a, 47, 92, fileNames );
  __appendRange( ResourceGroup::plateau, 1, 44, fileNames );
  __appendRange( ResourceGroup::transport, 1, 5, fileNames );
  fileNames << "transport_00017";
  fileNames << "transport_00029";
  fileNames << "transport_00041";
  fileNames << "transport_00052";
  fileNames << "transport_00053";
  fileNames << "transport_00054";
  fileNames << "transport_00055";
  fileNames << "utilitya_00001";

  foreach( it, fileNames )
  {
    NFile file = archive->createAndOpenFile( *it + ".png" );
    if( file.isOpen() )
    {
      Picture pic = PictureLoader::instance().load( file );
      if( pic.isValid() )
      {
        PictureBank::instance().setPicture( *it, pic );
      }
    }
  }
}
