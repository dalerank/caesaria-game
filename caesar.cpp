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


#include "caesar.hpp"

#include "screen_wait.hpp"

#include <cstdlib>
#include <string>
#include <sstream>
#include <iostream>
#include <list>
#include <SDL.h>
#include <SDL_image.h>
#include <libintl.h>
#include <locale.h>
#include <algorithm>

#include "exception.hpp"
#include "pic_loader.hpp"
#include "scenario_loader.hpp"
#include "scenario.hpp"
#include "city.hpp"
#include "picture.hpp"
#include "gfx_sdl_engine.hpp"
#include "gfx_gl_engine.hpp"
#include "sound_engine.hpp"
#include "walker.hpp"
#include "gui_info_box.hpp"
#include "model_loader.hpp"


#include "screen_menu.hpp"
#include "screen_game.hpp"
#include "house_level.hpp"
#include "oc3_guienv.h"

#if defined(_MSC_VER)
	#undef main
#endif

class CaesarApp::Impl
{
public:
	Scenario*  scenario;
	ScreenType nextScreen;
	GfxEngine* engine;
	GuiEnv* gui;
};

void CaesarApp::initLocale()
{
   // init the internationalization library (gettext)
   setlocale(LC_ALL, "");
   bindtextdomain( "caesar", "." );
   textdomain("caesar");
}

void CaesarApp::initVideo()
{
   std::cout << "init graphic engine" << std::endl;
   _d->engine = new GfxSdlEngine();
   
   /* Typical resolutions:
    * 640 x 480; 800 x 600; 1024 x 768; 1400 x 1050; 1600 x 1200
    */
   GfxEngine::instance().setScreenSize(1024, 768);
   GfxEngine::instance().init();
}

void CaesarApp::initSound()
{
   std::cout << "init sound engine" << std::endl;
   new SoundEngine();
   SoundEngine::instance().init();
}

void CaesarApp::initWaitPictures()
{
   std::cout << "load wait images begin" << std::endl;
   PicLoader &pic_loader = PicLoader::instance();
   pic_loader.load_wait();
   std::cout << "load wait images end" << std::endl;

   std::cout << "convert images begin" << std::endl;
   GfxEngine::instance().load_pictures(pic_loader.get_pictures());
   std::cout << "convert images end" << std::endl;
}

void CaesarApp::initGuiEnvironment()
{
	_d->gui = new GuiEnv( *_d->engine );
}

void CaesarApp::initPictures()
{
   std::cout << "load images begin" << std::endl;
   PicLoader &pic_loader = PicLoader::instance();
   pic_loader.load_all();
   std::cout << "load images end" << std::endl;

   std::cout << "load walking begin" << std::endl;
   WalkerLoader &walker_loader = WalkerLoader::instance();
   walker_loader.loadAll();
   std::cout << "load walking end" << std::endl;

   std::cout << "load fonts begin" << std::endl;
   FontLoader font_loader;
   font_loader.load_all();
   std::cout << "load fonts end" << std::endl;

   std::cout << "convert images begin" << std::endl;
   GfxEngine::instance().load_pictures(pic_loader.get_pictures());
   std::cout << "convert images end" << std::endl;
}

void CaesarApp::loadScenario(const std::string &scenarioFile)
{
   std::cout << "load scenario begin" << std::endl;
   
   _d->scenario = new Scenario();
   
   ScenarioLoader scenario_loader;
   scenario_loader.load(scenarioFile, *_d->scenario);

   City &city = _d->scenario->getCity();

   std::list<LandOverlay*> llo = city.getOverlayList();
   
   for (std::list<LandOverlay*>::iterator itLLO = llo.begin(); itLLO!=llo.end(); ++itLLO)
   {
      LandOverlay &overlay = **itLLO;
      Construction* construction = dynamic_cast<Construction*>(&overlay);
      if (construction != NULL)
      {
         // this is a construction
         construction->computeAccessRoads();
      }
   }

   std::cout << "load scenario end" << std::endl;
}


void CaesarApp::loadGame(const std::string &gameFile)
{
   std::cout << "load game begin" << std::endl;

   std::fstream f(gameFile.c_str(), std::ios::in | std::ios::binary);
   InputSerialStream stream;
   stream.init(f, -1);

   _d->scenario = new Scenario();
   _d->scenario->unserialize(stream);
   f.close();
   stream.finalize_read();

   std::cout << "load game end" << std::endl;
}


void CaesarApp::setScreenWait()
{
   ScreenWait screen;
   screen.initialize( *_d->engine, *_d->gui);
   screen.drawFrame();
}

std::vector <fs::path> CaesarApp::scanForMaps() const
{
  // scan for map-files and make their list
    
  fs::path path ("./resources/maps/");
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
        std::vector <fs::path> filelist = scanForMaps();
		std::srand( (Uint32)std::time(0));
		std::string file = filelist.at(std::rand()%filelist.size()).string();
		std::cout<<"Loading map:" << file << std::endl;
        loadScenario(file);
        /* end of temporary */
        _d->nextScreen = SCREEN_GAME;
      }
    break;
   
    case ScreenMenu::loadSavedGame:
	{  
		loadGame("oc3.sav");
        _d->nextScreen = SCREEN_GAME;
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
   screen.setScenario(*_d->scenario);
   screen.initialize( *_d->engine, *_d->gui );
   screen.run();
   _d->nextScreen = SCREEN_QUIT;
}


CaesarApp::CaesarApp() : _d( new Impl )
{
   _d->scenario = NULL;
   _d->nextScreen = SCREEN_NONE;
}

void CaesarApp::start()
{
   initLocale();
   initVideo();
   initGuiEnvironment();
   initSound();
   //SoundEngine::instance().play_music("resources/sound/drums.wav");
   initWaitPictures();  // init some quick pictures for screenWait
   setScreenWait();

   initPictures();
   ModelLoader().loadHouseModel("house_model.csv");
   HouseLevelSpec::init();

   _d->nextScreen = SCREEN_MENU;

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

