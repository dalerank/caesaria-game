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

  _d->options[ resourcePath        ] = Variant( application_path + std::string( "/resources" ) );
  _d->options[ localePath          ] = Variant( application_path + std::string( "/resources/locale" ) );
  _d->options[ pantheonModel       ] = Variant( std::string( "/pantheon.model" ) );
  _d->options[ houseModel          ] = Variant( std::string( "/house.model" ) );
  _d->options[ constructionModel   ] = Variant( std::string( "/construction.model" ) );
  _d->options[ citiesModel         ] = Variant( std::string( "/cities.model" ) );
  _d->options[ ctNamesModel        ] = Variant( std::string( "/names.model" ) );
  _d->options[ settingsPath        ] = Variant( std::string( "/settings.model" ) );
  _d->options[ langModel           ] = Variant( std::string( "/language.model" ) );
  _d->options[ language            ] = Variant( std::string( "ru" ) );
  _d->options[ resolution          ] = Size( 1024, 768 );
  _d->options[ fullscreen          ] = false;
  _d->options[ worldModel          ] = Variant( std::string( "/worldmap.model" ) );
  _d->options[ minMonthWithFood    ] = 3;
  _d->options[ worklessCitizenAway ] = 15;
  _d->options[ emigrantSalaryKoeff ] = 2.f;

   vfs::Path dirName = "saves";
   vfs::Directory saveDir;
 #ifdef CAESARIA_PLATFORM_LINUX
   dirName = vfs::Path( ".caesaria/" ) + dirName;
   saveDir = vfs::Directory::getUserDir()/dirName;
 #elif defined(CAESARIA_PLATFORM_WIN)
   saveDir = vfs::Directory::getApplicationDir()/dirName;
 #endif
   _d->options[ savedir ] = Variant( saveDir.toString() );
}

void GameSettings::set( const std::string& option, const Variant& value )
{
  getInstance()._d->options[ option ] = value;
}

Variant GameSettings::get( const std::string& option )
{
  return getInstance()._d->options[ option ];
}

vfs::Path GameSettings::rcpath( const std::string& option )
{
  std::string rc = getInstance()._d->options[ resourcePath ].toString();

  VariantMap::iterator it = getInstance()._d->options.find( option );
  if( it == getInstance()._d->options.end() )
  {
    return vfs::Path(rc + option);
  }

  return vfs::Path(rc + getInstance()._d->options[ option ].toString());
}

void GameSettings::load()
{
  VariantMap settings = SaveAdapter::load( rcpath( GameSettings::settingsPath ) );

  foreach( VariantMap::value_type& v, settings )
  {
    set( v.first, v.second );
  }
}

void GameSettings::save()
{
  VariantMap saveSettings;
  saveSettings[ GameSettings::fullscreen ] = get( GameSettings::fullscreen );
  saveSettings[ GameSettings::resolution ] = get( GameSettings::resolution );

  SaveAdapter::save( saveSettings,  rcpath( GameSettings::settingsPath ) );
}
