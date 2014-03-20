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

#include "settings.hpp"
#include "vfs/path.hpp"
#include "core/saveadapter.hpp"
#include "vfs/directory.hpp"
#include "core/foreach.hpp"

const char* GameSettings::localePath = "localePath";
const char* GameSettings::resourcePath = "resourcePath";
const char* GameSettings::pantheonModel = "pantheonConfig";
const char* GameSettings::houseModel = "houseModel";
const char* GameSettings::citiesModel = "citiesModel";
const char* GameSettings::constructionModel = "constructionModel";
const char* GameSettings::ctNamesModel = "ctNamesModel";
const char* GameSettings::settingsPath = "settingsPath";
const char* GameSettings::resolution = "resolution";
const char* GameSettings::fullscreen = "fullscreen";
const char* GameSettings::language = "language";
const char* GameSettings::emigrantSalaryKoeff = "emigrantSalaryKoeff";
const char* GameSettings::savedir = "savedir";
const char* GameSettings::worldModel = "worldModel";
const char* GameSettings::minMonthWithFood = "minMonthWithFood";
const char* GameSettings::langModel = "langModel";
const char* GameSettings::worklessCitizenAway = "worklessCitizenAway";
const char* GameSettings::fastsavePostfix = "fastsavePostfix";
const char* GameSettings::saveExt = "saveExt";
const char* GameSettings::workDir = "workDir";
const char* GameSettings::adviserEnabled = "adviserEnabled";
const char* GameSettings::c3gfx = "c3gfx";
const char* GameSettings::lastTranslation = "lastTranslation";
const char* GameSettings::archivesModel = "archivesModel";
const char* GameSettings::soundThemesModel = "soundThemesModel";

const vfs::Path defaultSaveDir = "saves";
const vfs::Path defaultResDir = "resources";
const vfs::Path defaultLocaleDir = "resources/locale";

class GameSettings::Impl
{
public:
  VariantMap options;
};

GameSettings& GameSettings::getInstance()
{
  static GameSettings inst;
  return inst;
}

GameSettings::GameSettings() : _d( new Impl )
{
  std::string application_path = vfs::Directory::getApplicationDir().toString();
  setwdir( application_path );

  _d->options[ pantheonModel       ] = Variant( std::string( "/pantheon.model" ) );
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
  _d->options[ resolution          ] = Size( 1024, 768 );
  _d->options[ fullscreen          ] = false;
  _d->options[ worldModel          ] = Variant( std::string( "/worldmap.model" ) );
  _d->options[ minMonthWithFood    ] = 3;
  _d->options[ worklessCitizenAway ] = 30;
  _d->options[ emigrantSalaryKoeff ] = 5.f;
}

void GameSettings::set( const std::string& option, const Variant& value )
{
  getInstance()._d->options[ option ] = value;
}

Variant GameSettings::get( const std::string& option )
{
  return getInstance()._d->options[ option ];
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
#elif defined(CAESARIA_PLATFORM_WIN) || defined(CAESARIA_PLATFORM_HAIKU) || defined(CAESARIA_PLATFORM_MACOSX)
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
  VariantMap saveSettings;
  saveSettings[ GameSettings::fullscreen ] = get( GameSettings::fullscreen );
  saveSettings[ GameSettings::resolution ] = get( GameSettings::resolution );

  SaveAdapter::save( saveSettings,  rcpath( GameSettings::settingsPath ) );
}
