// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com

#include "oc3_caesar.hpp"
#include "oc3_screen_wait.hpp"
#include "oc3_exception.hpp"
#include "oc3_stringhelper.hpp"
#include "oc3_scenario_loader.hpp"
#include "oc3_scenario.hpp"
#include "oc3_city.hpp"
#include "oc3_picture.hpp"
#include "oc3_gfx_sdl_engine.hpp"
#include "oc3_gfx_gl_engine.hpp"
#include "oc3_sound_engine.hpp"
#include "oc3_walker.hpp"
#include "oc3_gui_info_box.hpp"
#include "oc3_astarpathfinding.hpp"
#include "oc3_building_data.hpp"
#include "oc3_picture_bank.hpp"
#include "oc3_screen_menu.hpp"
#include "oc3_screen_game.hpp"
#include "oc3_house_level.hpp"
#include "oc3_guienv.hpp"
#include "oc3_app_config.hpp"
#include "oc3_divinity.hpp"

#include <boost/filesystem.hpp>
#include <libintl.h>
#include <list>

namespace fs = boost::filesystem;

#if defined(_MSC_VER)
  #undef main
#endif

class CaesarApp::Impl
{
public:
  ScreenType nextScreen;
  GfxEngine* engine;
  GuiEnv* gui;
  
  void initLocale();
  bool load(const std::string& filename);
  void initPictures(const std::string &resourcePath);
  std::vector<fs::path> scanForMaps(const std::string &resourcePath) const;
};

void CaesarApp::Impl::initLocale()
{
  // init the internationalization library (gettext)
  setlocale(LC_ALL, "");
  bindtextdomain( "caesar", "./locale" );
  textdomain( "caesar" );
}

void CaesarApp::initVideo()
{
  StringHelper::debug( 0xff, "init graphic engine" );
  _d->engine = new GfxSdlEngine();
   
  /* Typical resolutions:
   * 640 x 480; 800 x 600; 1024 x 768; 1400 x 1050; 1600 x 1200
   */
  GfxEngine::instance().setScreenSize(1024, 768);
  GfxEngine::instance().init();
}

void CaesarApp::initSound()
{
  StringHelper::debug( 0xff, "init sound engine" );
  new SoundEngine();
  SoundEngine::instance().init();
}

void CaesarApp::initWaitPictures()
{
  std::cout << "load wait images begin" << std::endl;
  PicLoader &pic_loader = PicLoader::instance();
  pic_loader.loadWaitPics();
  std::cout << "load wait images end" << std::endl;

  std::cout << "convert images begin" << std::endl;
  GfxEngine::instance().loadPictures( pic_loader.getPictures() );
  std::cout << "convert images end" << std::endl;
}

void CaesarApp::initGuiEnvironment()
{
  _d->gui = new GuiEnv( *_d->engine );
}

void CaesarApp::Impl::initPictures(const std::string &resourcePath)
{
  std::cout << "load images begin" << std::endl;
  PicLoader &pic_loader = PicLoader::instance();
  pic_loader.loadAllPics();
  std::cout << "load images end" << std::endl;

  std::cout << "load walking begin" << std::endl;
  WalkerLoader &walker_loader = WalkerLoader::instance();
  walker_loader.loadAll();
  std::cout << "load walking end" << std::endl;

  std::cout << "load fonts begin" << std::endl;
  FontCollection::instance().initialize( resourcePath );
  std::cout << "load fonts end" << std::endl;

  std::cout << "convert images begin" << std::endl;
  GfxEngine::instance().loadPictures( pic_loader.getPictures() );
  std::cout << "convert images end" << std::endl;

  std::cout << "create pictures begin" << std::endl;
  pic_loader.createResources();
  std::cout << "create pictures end" << std::endl;
}

bool CaesarApp::Impl::load(const std::string &gameFile)
{
  std::cout << "load game begin" << std::endl;
  
  Scenario& scenario = Scenario::instance();

  bool loadok = ScenarioLoader::getInstance().load(gameFile, scenario);   

  if( !loadok )
  {
    std::cout << "LOADING ERROR: can't load game from " << gameFile << std::endl;
    return false;
  }  

  City &city = scenario.getCity();
  
  LandOverlays llo = city.getOverlayList();
  
  for ( LandOverlays::iterator itLLO = llo.begin(); itLLO!=llo.end(); ++itLLO)
  {
     LandOverlayPtr overlay = *itLLO;
     ConstructionPtr construction = overlay.as<Construction>();
     if( construction.isValid() )
     {
        // this is a construction
        construction->computeAccessRoads();
     }
  }

  Pathfinder::getInstance().update( scenario.getCity().getTilemap() );  
  
  std::cout << "load game end" << std::endl;
  return true;
}

void CaesarApp::setScreenWait()
{
   ScreenWait screen;
   screen.initialize( *_d->engine, *_d->gui);
   screen.drawFrame();
}

std::vector <fs::path> CaesarApp::Impl::scanForMaps(const std::string &resourcePath) const
{
  // scan for map-files and make their list
    
  fs::path path( resourcePath + "/maps/" );
  std::vector <fs::path> filelist;
  
  fs::recursive_directory_iterator it (path);
  fs::recursive_directory_iterator end;
  
  for (; it!=end; ++it)
  {
    if (!fs::is_directory(*it))
      filelist.push_back(*it);
  }
  
  std::sort(filelist.begin(), filelist.end());
  
  std::copy(filelist.begin(), filelist.end(), std::ostream_iterator<fs::path>(std::cout, "\n"));
  
  return filelist;
}

void CaesarApp::setScreenMenu()
{
  ScreenMenu screen;
  screen.initialize( *_d->engine, *_d->gui );
  int result = screen.run();

  switch( result )
  {
    case ScreenMenu::startNewGame:
    {
      /* temporary*/
      std::vector<fs::path> filelist = _d->scanForMaps( AppConfig::get( AppConfig::resourcePath ).toString() );
      std::srand( (Uint32)std::time(0));
      std::string file = filelist.at(std::rand() % filelist.size()).string();
      std::cout<<"Loading map:" << file << std::endl;
      bool loadok = _d->load(file);
      _d->nextScreen = loadok ? SCREEN_GAME : SCREEN_MENU;
    }
    break;
   
    case ScreenMenu::loadSavedGame:
    {  
      std::cout<<"Loading map:" << "lepcismagna.sav" << std::endl;
      bool loadok = _d->load(  AppConfig::get( AppConfig::resourcePath ).toString() + "/savs/" + "timgad.sav");
      _d->nextScreen = loadok ? SCREEN_GAME : SCREEN_MENU;
    }
    break;

    case ScreenMenu::loadMap:
    {
      bool loadok = _d->load( screen.getMapName() );
      _d->nextScreen = loadok ? SCREEN_GAME : SCREEN_MENU;
    }
    break;
   
    case ScreenMenu::closeApplication:
    {
      _d->nextScreen = SCREEN_QUIT;
    }
    break;
   
    default:
      THROW("Unexpected result event: " << result);
   }
}

void CaesarApp::setScreenGame()
{
  ScreenGame screen;
  screen.setScenario( Scenario::instance() );
  screen.initialize( *_d->engine, *_d->gui );
  int result = screen.run();

  switch( result )
  {
    case ScreenGame::mainMenu:
      _d->nextScreen = SCREEN_MENU;
    break;

    case ScreenGame::quitGame:
      _d->nextScreen = SCREEN_QUIT;
    break;

    default:
      _d->nextScreen = SCREEN_QUIT;
  }   
}


CaesarApp::CaesarApp() : _d( new Impl )
{
   _d->nextScreen = SCREEN_NONE;
}

void CaesarApp::start()
{
   //Create right PicLoader instance in the beginning   
   _d->initLocale();
   
   initVideo();
   initGuiEnvironment();
   initSound();
   //SoundEngine::instance().play_music("resources/sound/drums.wav");
   initWaitPictures();  // init some quick pictures for screenWait
   setScreenWait();

   const std::string rPath =  AppConfig::get( AppConfig::resourcePath ).toString();
   _d->initPictures( rPath );
   HouseSpecHelper::getInstance().initialize( rPath + AppConfig::get( AppConfig::houseModel ).toString() );
   DivinePantheon::getInstance().initialize( rPath + AppConfig::get( AppConfig::pantheonModel ).toString() );
   BuildingDataHolder::instance().initialize( rPath + AppConfig::get( AppConfig::constructionModel ).toString() );

   _d->nextScreen = SCREEN_MENU;
   _d->engine->setFlag( 0, 1 );

   while(_d->nextScreen != SCREEN_QUIT)
   {
      switch(_d->nextScreen)
      {
      case SCREEN_MENU:
         setScreenMenu();
         break;
      case SCREEN_GAME:
         setScreenGame();
         break;
      default:
         THROW("Unexpected screen type: " << _d->nextScreen);
      }
   }

   //setScreenWait();
   //SDL_Delay(1500);
}

int main(int argc, char* argv[])
{
   for (int i = 0; i < (argc - 1); i++)
   {
	   if( !strcmp( argv[i], "-R" ) )
	   {
       AppConfig::set( AppConfig::resourcePath, Variant( std::string( argv[i+1] ) ) );
		   break;
	   }
   }

   try
   {
      CaesarApp app;
      app.start();
   }
   catch (Exception e)
   {
      std::cout << "FATAL ERROR: " << e.getDescription() << std::endl;
   }

   return 0;
}