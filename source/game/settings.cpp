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

#include "settings.hpp"
#include "vfs/path.hpp"
#include "core/saveadapter.hpp"
#include "vfs/directory.hpp"
#include "core/foreach.hpp"

#define __REG_PROPERTY(a) const char* GameSettings::a = CAESARIA_STR_EXT(a);
__REG_PROPERTY(localePath)
__REG_PROPERTY(resourcePath )
__REG_PROPERTY(pantheonModel )
__REG_PROPERTY(houseModel )
__REG_PROPERTY(citiesModel)
__REG_PROPERTY(constructionModel)
__REG_PROPERTY(ctNamesModel)
__REG_PROPERTY(settingsPath)
__REG_PROPERTY(resolution)
__REG_PROPERTY(fullscreen)
__REG_PROPERTY(language)
__REG_PROPERTY(emigrantSalaryKoeff)
__REG_PROPERTY(savedir)
__REG_PROPERTY(worldModel)
__REG_PROPERTY(minMonthWithFood)
__REG_PROPERTY(langModel)
__REG_PROPERTY(worklessCitizenAway)
__REG_PROPERTY(fastsavePostfix)
__REG_PROPERTY(saveExt)
__REG_PROPERTY(workDir)
__REG_PROPERTY(c3gfx)
__REG_PROPERTY(lastTranslation)
__REG_PROPERTY(archivesModel)
__REG_PROPERTY(soundThemesModel)
__REG_PROPERTY(soundVolume )
__REG_PROPERTY(ambientVolume)
__REG_PROPERTY(musicVolume )
__REG_PROPERTY(animationsModel )
__REG_PROPERTY(walkerModel)
__REG_PROPERTY(giftsModel)
__REG_PROPERTY(emblemsModel )
__REG_PROPERTY(remakeModel )
__REG_PROPERTY(screenFitted)
__REG_PROPERTY(needAcceptBuild)
__REG_PROPERTY(sg2model)
__REG_PROPERTY(ranksModel)
__REG_PROPERTY(autosaveInterval)
__REG_PROPERTY(talksArchive)
__REG_PROPERTY(rectuterDistance)
__REG_PROPERTY(render)
__REG_PROPERTY(empireObjectsModel)
__REG_PROPERTY(pic_offsets)
__REG_PROPERTY(picsArchive)
__REG_PROPERTY(opengl_opts)
__REG_PROPERTY(font)
__REG_PROPERTY(freeplay_opts)
#undef __REG_PROPERTY

const vfs::Path defaultSaveDir = "saves";
const vfs::Path defaultResDir = "resources";
const vfs::Path defaultLocaleDir = "resources/locale";

class GameSettings::Impl
{
public:
  VariantMap options;
};

GameSettings& GameSettings::instance()
{
  static GameSettings inst;
  return inst;
}

GameSettings::GameSettings() : _d( new Impl )
{
  std::string application_path = vfs::Directory::getApplicationDir().toString();
  setwdir( application_path );

  _d->options[ pantheonModel       ] = Variant( std::string( "/pantheon.model" ) );
  _d->options[ sg2model            ] = Variant( std::string( "/sg2.model" ) );
  _d->options[ houseModel          ] = Variant( std::string( "/house.model" ) );
  _d->options[ constructionModel   ] = Variant( std::string( "/construction.model" ) );
  _d->options[ citiesModel         ] = Variant( std::string( "/cities.model" ) );
  _d->options[ ctNamesModel        ] = Variant( std::string( "/names.model" ) );
  _d->options[ settingsPath        ] = Variant( std::string( "/settings.model" ) );
  _d->options[ langModel           ] = Variant( std::string( "/language.model" ) );
  _d->options[ archivesModel       ] = Variant( std::string( "/archives.model" ) );
  _d->options[ soundThemesModel    ] = Variant( std::string( "/sound_themes.model" ) );
  _d->options[ language            ] = Variant( std::string( "en" ) );
  _d->options[ fastsavePostfix     ] = Variant( std::string( "_fastsave") );
  _d->options[ saveExt             ] = Variant( std::string( ".oc3save") );
  _d->options[ walkerModel         ] = Variant( std::string( "/walker.model" ) );
  _d->options[ animationsModel     ] = Variant( std::string( "/animations.model" ) );
  _d->options[ empireObjectsModel  ] = Variant( std::string( "/empire_objects.model" ) );
  _d->options[ giftsModel          ] = Variant( std::string( "/gifts.model" ) );
  _d->options[ emblemsModel        ] = Variant( std::string( "/emblems.model" ) );
  _d->options[ remakeModel         ] = Variant( std::string( "/remake.model" ) );
  _d->options[ ranksModel          ] = Variant( std::string( "/ranks.model" ) );
  _d->options[ pic_offsets         ] = Variant( std::string( "/offsets.model" ) );
  _d->options[ picsArchive         ] = Variant( std::string( "/gfx/pics.zip" ) );
  _d->options[ opengl_opts         ] = Variant( std::string( "/opengl.model" ) );
  _d->options[ freeplay_opts       ] = Variant( std::string( "/freeplay.model" ) );
  _d->options[ font                ] = Variant( std::string( "FreeSerif.ttf" ) );
  _d->options[ needAcceptBuild     ] = false;
  _d->options[ render              ] = "sdl";
  _d->options[ talksArchive        ] = Variant( std::string( "/audio/wavs_citizen_en.zip" ) );
  _d->options[ autosaveInterval    ] = 3;
  _d->options[ soundVolume         ] = 100;
  _d->options[ ambientVolume       ] = 50;
  _d->options[ musicVolume         ] = 25;
  _d->options[ resolution          ] = Size( 1024, 768 );
  _d->options[ fullscreen          ] = false;
  _d->options[ worldModel          ] = Variant( std::string( "/worldmap.model" ) );
  _d->options[ minMonthWithFood    ] = 3;
  _d->options[ worklessCitizenAway ] = 30;
  _d->options[ emigrantSalaryKoeff ] = 5.f;
  _d->options[ rectuterDistance    ] = 35;

#ifdef CAESARIA_PLATFORM_ANDROID
  _d->options[ needAcceptBuild     ] = true;
#endif
}

void GameSettings::set( const std::string& option, const Variant& value )
{
  instance()._d->options[ option ] = value;
}

Variant GameSettings::get( const std::string& option )
{
  VariantMap::iterator it = instance()._d->options.find( option );
  return  instance()._d->options.end() == it
              ? Variant()
              : it->second;
}

void GameSettings::setwdir( const std::string& wdirstr )
{
  vfs::Directory wdir( wdirstr );
  _d->options[ workDir ] = Variant( wdir.toString() );
  _d->options[ resourcePath ] = Variant( (wdir/defaultResDir).toString() );
  _d->options[ localePath ] = Variant( (wdir/defaultLocaleDir).toString() );
  _d->options[ savedir ] = Variant( (wdir/defaultSaveDir).toString() );

  vfs::Directory saveDir;
#ifdef CAESARIA_PLATFORM_LINUX
  vfs::Path dirName = vfs::Path( ".caesaria/" ) + defaultSaveDir;
  saveDir = vfs::Directory::getUserDir()/dirName;
#elif defined(CAESARIA_PLATFORM_WIN) || defined(CAESARIA_PLATFORM_HAIKU) || defined(CAESARIA_PLATFORM_MACOSX) || defined(CAESARIA_PLATFORM_ANDROID)
  saveDir = wdir/defaultSaveDir;
#endif
  _d->options[ savedir ] = Variant( saveDir.toString() );
}

static vfs::Path __concatPath( vfs::Directory dir, vfs::Path fpath )
{
  Variant vr = GameSettings::get( fpath.toString() );
  if( vr.isNull() )
  {
    return dir/fpath;
  }

  return dir/vfs::Path( vr.toString() );
}

vfs::Path GameSettings::rcpath( const std::string& option )
{
  std::string rc = get( resourcePath ).toString();

  return __concatPath( rc, option );
}

vfs::Path GameSettings::rpath( const std::string& option )
{
  std::string wd = get( workDir ).toString();

  return __concatPath( wd, option );
}

void GameSettings::load()
{
  VariantMap settings = SaveAdapter::load( rcpath( GameSettings::settingsPath ) );

  foreach( v, settings ) { set( v->first, v->second ); }
}

void GameSettings::save()
{
  SaveAdapter::save( instance()._d->options, rcpath( GameSettings::settingsPath ) );
}
