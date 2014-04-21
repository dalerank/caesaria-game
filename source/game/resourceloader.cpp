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
#include "vfs/filesystem.hpp"
#include "core/logger.hpp"
#include "core/saveadapter.hpp"
#include "game/settings.hpp"
#include "core/gettext.hpp"

class ResourceLoader::Impl
{
public:


public oc3_signals:
  Signal1<std::string> onStartLoadingSignal;
};


ResourceLoader::ResourceLoader() : _d( new Impl )
{

}

ResourceLoader::~ResourceLoader(){  }

void ResourceLoader::loadFromModel(vfs::Path path2model)
{
  VariantMap archives = SaveAdapter::load( path2model );
  foreach( a, archives )
  {
    vfs::Path absArchivePath = GameSettings::rcpath( a->second.toString() );
    Logger::warning( "Game: try mount archive " + absArchivePath.toString() );
    Logger::warningIf( !absArchivePath.exist(), "Game: cannot load archive " + absArchivePath.toString() );
    vfs::FileSystem::instance().mountArchive( absArchivePath );

    std::string outText = _("##loading_resources##") + std::string( " " ) + a->first;
    _d->onStartLoadingSignal.emit( outText );
  }
}

Signal1<std::string> &ResourceLoader::onStartLoading() { return _d->onStartLoadingSignal;}
