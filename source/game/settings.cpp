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
#include "core/variant_map.hpp"
#include "core/utils.hpp"
#include "core/osystem.hpp"
#include "core/metric.hpp"

namespace game
{

#define _CONFIG_PATH(a) std::string( #a ".model");
#define __REG_PROPERTY(a) const char* Settings::a = TEXT(a);
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
__REG_PROPERTY(workDir)
__REG_PROPERTY(c3gfx)
__REG_PROPERTY(c3music)
__REG_PROPERTY(c3video)
__REG_PROPERTY(oldgfx)
__REG_PROPERTY(lastTranslation)
__REG_PROPERTY(archivesModel)
__REG_PROPERTY(soundVolume )
__REG_PROPERTY(ambientVolume)
__REG_PROPERTY(musicVolume )
__REG_PROPERTY(animationsModel )
__REG_PROPERTY(walkerModel)
__REG_PROPERTY(emblemsModel )
__REG_PROPERTY(remakeModel )
__REG_PROPERTY(screenFitted)
__REG_PROPERTY(needAcceptBuild)
__REG_PROPERTY(sg2model)
__REG_PROPERTY(autosaveInterval)
__REG_PROPERTY(talksArchive)
__REG_PROPERTY(render)
__REG_PROPERTY(debugMenu)
__REG_PROPERTY(empireObjectsModel)
__REG_PROPERTY(pic_offsets)
__REG_PROPERTY(picsArchive)
__REG_PROPERTY(opengl_opts)
__REG_PROPERTY(font)
__REG_PROPERTY(walkerRelations)
__REG_PROPERTY(freeplay_opts)
__REG_PROPERTY(cellw)
__REG_PROPERTY(simpleAnimationModel)
__REG_PROPERTY(cartsModel)
__REG_PROPERTY(logoArchive)
__REG_PROPERTY(titleResource)
__REG_PROPERTY(forbidenTile)
__REG_PROPERTY(layersOptsModel)
__REG_PROPERTY(experimental)
__REG_PROPERTY(buildMenuModel)
__REG_PROPERTY(scrollSpeed)
__REG_PROPERTY(borderMoving)
__REG_PROPERTY(mmb_moving)
__REG_PROPERTY(lockInfobox)
__REG_PROPERTY(soundAlias)
__REG_PROPERTY(playerName)
__REG_PROPERTY(lastGame)
__REG_PROPERTY(tooltipEnabled)
__REG_PROPERTY(screenshotDir)
__REG_PROPERTY(showTabletMenu)
__REG_PROPERTY(batchTextures)
__REG_PROPERTY(ccUseAI)
__REG_PROPERTY(metricSystem)
__REG_PROPERTY(defaultFont)
__REG_PROPERTY(celebratesConfig)
__REG_PROPERTY(ambientsounds)
__REG_PROPERTY(cntrGroupsModel)
__REG_PROPERTY(logfile)
__REG_PROPERTY(rightMenu)
__REG_PROPERTY(showEmpireMapTiles)
__REG_PROPERTY(logoImageRc)
__REG_PROPERTY(showLastChanges)
__REG_PROPERTY(lastChangesNumber)
__REG_PROPERTY(citiesIdModel)
__REG_PROPERTY(fontsDirectory)
__REG_PROPERTY(showStartAware)
__REG_PROPERTY(verbose)
__REG_PROPERTY(buildNumber)
#undef __REG_PROPERTY

const vfs::Path defaultSaveDir = "saves";
const vfs::Path defaultResDir = "resources";
const vfs::Path defaultLocaleDir = "resources/locale";

class Settings::Impl
{
public:
  VariantMap options;
};

Settings& Settings::instance()
{
  static Settings inst;
  return inst;
}

Settings::Settings() : _d( new Impl )
{
  std::string application_path = vfs::Directory::applicationDir().toString();
  setwdir( application_path );

  _d->options[ pantheonModel       ] = std::string( "/pantheon.model" );
  _d->options[ sg2model            ] = std::string( "/sg2.model" );
  _d->options[ houseModel          ] = std::string( "/house.model" );
  _d->options[ constructionModel   ] = std::string( "/construction.model" );
  _d->options[ citiesModel         ] = std::string( "/cities.model" );
  _d->options[ ctNamesModel        ] = std::string( "/locale/names." );
  _d->options[ settingsPath        ] = std::string( "/settings.model" );
  _d->options[ langModel           ] = std::string( "/language.model" );
  _d->options[ archivesModel       ] = std::string( "/archives.model" );
  _d->options[ language            ] = std::string( "" );
  _d->options[ fontsDirectory      ] = std::string( "/fonts");
  _d->options[ walkerModel         ] = std::string( "/walker.model" );
  _d->options[ animationsModel     ] = std::string( "/animations.model" );
  _d->options[ empireObjectsModel  ] = std::string( "/empire_objects.model" );
  _d->options[ emblemsModel        ] = std::string( "/emblems.model" );
  _d->options[ remakeModel         ] = std::string( "/remake.model" );
  _d->options[ pic_offsets         ] = std::string( "/offsets.model" );
  _d->options[ picsArchive         ] = std::string( "/gfx/pics.zip" );
  _d->options[ opengl_opts         ] = std::string( "/opengl.model" );
  _d->options[ freeplay_opts       ] = std::string( "/freeplay.model" );
  _d->options[ walkerRelations     ] = std::string( "/relations.model" );
  _d->options[ logfile             ] = std::string( "stdout.txt" );
  _d->options[ font                ] = std::string( "FreeSerif.ttf" );
  _d->options[ defaultFont         ] = std::string( "FreeSerif.ttf" );
  _d->options[ simpleAnimationModel] = std::string( "/basic_animations.model" );
  _d->options[ cartsModel          ] = std::string( "/carts.model" );
  _d->options[ logoArchive         ] = std::string( "/gfx/pics_wait.zip" );
  _d->options[ titleResource       ] = std::string( "titlerm" );
  _d->options[ forbidenTile        ] = std::string( "oc3_land" );
  _d->options[ layersOptsModel     ] = std::string( "layers_opts.model" );
  _d->options[ buildMenuModel      ] = std::string( "build_menu.model" );
  _d->options[ soundAlias          ] = std::string( "sounds.model" );
  _d->options[ celebratesConfig    ] = std::string( "romancelebs.model" );
  _d->options[ ambientsounds       ] = std::string( "ambientsounds.model" );
  _d->options[ cntrGroupsModel     ] = std::string( "construction_groups.model" );
  _d->options[ screenshotDir       ] = vfs::Directory::userDir().toString();
  _d->options[ batchTextures       ] = true;
  _d->options[ verbose             ] = false;
  _d->options[ rightMenu           ] = true;
  _d->options[ experimental        ] = false;
  _d->options[ needAcceptBuild     ] = false;
  _d->options[ borderMoving        ] = false;
  _d->options[ showEmpireMapTiles  ] = false;
  _d->options[ render              ] = std::string( "sdl" );
  _d->options[ scrollSpeed         ] = 30;
  _d->options[ mmb_moving          ] = false;
  _d->options[ tooltipEnabled      ] = true;
  _d->options[ ccUseAI             ] = false;
  _d->options[ showStartAware      ] = true;
  _d->options[ c3gfx               ] = std::string( "" );
  _d->options[ c3video             ] = std::string( "" );
  _d->options[ c3music             ] = std::string( "" );
  _d->options[ talksArchive        ] = std::string( ":/audio/wavs_citizen_en.zip" );
  _d->options[ autosaveInterval    ] = 3;
  _d->options[ buildNumber         ] = GAME_BUILD_NUMBER;
  _d->options[ soundVolume         ] = 100;
  _d->options[ lockInfobox         ] = true;
  _d->options[ metricSystem        ] = metric::Measure::native;
  _d->options[ ambientVolume       ] = 50;
  _d->options[ cellw               ] = 60;
  _d->options[ musicVolume         ] = 25;
  _d->options[ resolution          ] = Size( 1024, 768 );
  _d->options[ fullscreen          ] = false;
  _d->options[ worldModel          ] = std::string( "worldmap.model" );
  _d->options[ citiesIdModel       ] = std::string( "cities_id.model" );
  _d->options[ minMonthWithFood    ] = 3;
  _d->options[ worklessCitizenAway ] = 30;
  _d->options[ emigrantSalaryKoeff ] = 5.f;
  _d->options[ oldgfx              ] = 1;
  _d->options[ logoImageRc         ] = "logo";
  _d->options[ showTabletMenu      ] = false;
  _d->options[ debugMenu           ] = false;
  _d->options[ showLastChanges     ] = true;
  _d->options[ lastChangesNumber   ] = 0;

#ifdef DEBUG
  _d->options[ debugMenu           ] = true;
#endif

#ifdef GAME_USE_STEAM
  _d->options[ oldgfx              ] = false;
#endif

  if( OSystem::isAndroid() )
  {
    _d->options[ needAcceptBuild     ] = true;
    _d->options[ showTabletMenu      ] = true;
  }
}

void Settings::set( const std::string& option, const Variant& value )
{
  instance()._d->options[ option ] = value;
}

Variant Settings::get(const std::string& option)
{
  VariantMap::iterator it = instance()._d->options.find( option );
  return  instance()._d->options.end() == it
              ? Variant()
              : it->second;
}

void Settings::setwdir(const std::string& wdirstr)
{
  vfs::Directory wdir( wdirstr );
  _d->options[ workDir ] = Variant( wdir.toString() );
  _d->options[ resourcePath ] = Variant( (wdir/defaultResDir).toString() );
  _d->options[ localePath ] = Variant( (wdir/defaultLocaleDir).toString() );
  _d->options[ savedir ] = Variant( (wdir/defaultSaveDir).toString() );

  vfs::Directory saveDir;
  vfs::Path dirName;
  if( OSystem::isLinux() )
  {
    dirName = ".caesaria/" + defaultSaveDir;
    saveDir = vfs::Directory::userDir()/dirName;
  }
  else
  {
    saveDir = wdir/defaultSaveDir;
  }
  _d->options[ savedir ] = Variant(saveDir.toString());
}

void Settings::resetIfNeed(char* argv[], int argc)
{
  for (int i = 0; i < argc; i++)
  {
    if( !strcmp( argv[i], "-resetConfig" ) )
    {
      vfs::NFile::remove( SETTINGS_RC_PATH( settingsPath ) );
      return;
    }
  }
}

bool Settings::checkwdir(char* argv[], int argc)
{
  for (int i = 0; i < (argc - 1); i++)
  {
    if( !strcmp( argv[i], "-R" ) )
    {
      const char* opts = argv[i+1];
      setwdir( std::string( opts, strlen( opts ) ) );
      return true;
    }
  }

  return false;
}

void Settings::checkCmdOptions(char* argv[], int argc)
{
  for (int i = 0; i < argc; i++)
  {
    if( !strcmp( argv[i], "-Lc" ) )
    {
      std::string opts = argv[i+1];
      _d->options[ language ] = Variant( opts ).toString();
      i++;
    }
    else if( !strncmp( argv[i], "-", 1 ) )
    {
      std::string name = std::string( argv[i] ).substr( 1 );
      std::string nextName = (i+1 >= argc ? "" : std::string( argv[i+1] ));
      if (nextName[0] == '-' || (i+1 == argc))
      {
        bool value = true;
        if( name[0] == '!' )
        {
          name = name.substr( 1 );
          value = false;
        }
        _d->options[name] = Variant(value);
      }
      else
      {
        _d->options[name] = Variant(nextName);
        i++;
      }
    }
  }
}

void Settings::checkC3present()
{
  bool useOldGraphics = isC3mode();

  std::map<std::string,std::string> items = {
                                              {houseModel,        "house"},
                                              {constructionModel, "construction"},
                                              {citiesModel,       "cities"},
                                              {walkerModel,       "walker"},
                                              {animationsModel,   "animations"},
                                              {empireObjectsModel,"empire_objects"},
                                              {simpleAnimationModel,"basic_animations"},
                                              {cartsModel,        "carts"},
                                              {worldModel,        "worldmap"},
                                              {buildMenuModel,    "build_menu"},
                                              {soundAlias,        "sounds"},
                                              {pic_offsets,       "offsets"},
                                            };

  std::string ext;
  if (useOldGraphics)
  {
    ext = ".c3";
    _d->options[ forbidenTile        ] = Variant(std::string( "org_land" ) );
    _d->options[ titleResource       ] = Variant(std::string( "title" ) );
    _d->options[ logoImageRc         ] = Variant(std::string( "c3title" ) );
    _d->options[ cellw ] = 30;
  }
  else
  {
    ext = ".model";
    _d->options[ forbidenTile        ] = Variant(std::string( "oc3_land" ) );
    _d->options[ titleResource       ] = Variant(std::string( "titlerm" ) );
    _d->options[ logoImageRc         ] = Variant(std::string( "logo" ) );
    _d->options[ cellw ] = 60;
  }

  for( auto& item : items )
    _d->options[ item.first ] = item.second + ext;
}

bool Settings::isC3mode() const
{
  bool haveC3path = !_d->options[c3gfx].toString().empty();
  return (haveC3path || KILLSWITCH(oldgfx));
}

void Settings::changeSystemLang(const std::string& newLang)
{
  std::string lang = newLang.empty() ? "en" : newLang;
  Settings::set( language, Variant( lang ) );
}

static vfs::Path __concatPath( vfs::Directory dir, vfs::Path fpath )
{
  Variant vr = Settings::get( fpath.toString() );
  if( vr.isNull() )
  {
    return dir/fpath;
  }

  return dir/vfs::Path( vr.toString() );
}

vfs::Path Settings::rcpath( const std::string& option )
{
  std::string rc = get( resourcePath ).toString();

  return __concatPath( rc, option );
}

vfs::Path Settings::rpath( const std::string& option )
{
  std::string wd = get( workDir ).toString();

  return __concatPath( wd, option );
}

int Settings::findLastChanges()
{
  vfs::Directory changesFolder( std::string(":/changes") );
  vfs::Entries::Items entries = changesFolder.entries().items();
  int maxFoundChanges = 0;
  for ( const auto& item : entries)
  {
    VariantMap vm = config::load(item.fullpath);
    maxFoundChanges = math::max(maxFoundChanges, vm.get("revision", 0).toInt() );
  }
  return maxFoundChanges;
}

bool Settings::haveLastConfig()
{
  vfs::Path lastConfig = SETTINGS_RC_PATH(settingsPath);
  bool result = false;
  if( lastConfig.exist() )
  {
    auto configMap = config::load( lastConfig );
    result = !configMap.empty();
  }

  return result;
}

void Settings::loadLastConfig()
{
  VariantMap settings = config::load(SETTINGS_RC_PATH(settingsPath));

  if (!settings.empty())
  {
    for (auto& v : settings) { set(v.first, v.second); }
  }
}

void Settings::save()
{
  config::save(instance()._d->options, SETTINGS_RC_PATH(settingsPath));
}

}//end namespace game
