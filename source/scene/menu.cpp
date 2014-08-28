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
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "menu.hpp"

#include "core/gettext.hpp"
#include "gui/loadfiledialog.hpp"
#include "gfx/engine.hpp"
#include "core/exception.hpp"
#include "gui/startmenu.hpp"
#include "gui/environment.hpp"
#include "game/game.hpp"
#include "game/player.hpp"
#include "gui/pushbutton.hpp"
#include "gui/label.hpp"
#include "game/settings.hpp"
#include "gui/playername_window.hpp"
#include "core/logger.hpp"
#include "core/foreach.hpp"
#include "vfs/directory.hpp"
#include "gui/label.hpp"
#include "gui/listbox.hpp"
#include "core/locale.hpp"
#include "core/saveadapter.hpp"
#include "gui/smkviewer.hpp"
#include "gui/dialogbox.hpp"
#include "core/osystem.hpp"
#include "gui/texturedbutton.hpp"
#include "sound/engine.hpp"
#include "events/setvideooptions.hpp"
#include "events/setsoundoptions.hpp"
#include "gui/widgetpositionanimator.hpp"
#include "gui/loadmissiondialog.hpp"
#include "core/event.hpp"
#include "core/timer.hpp"

using namespace gfx;
using namespace gui;

namespace scene
{

CAESARIA_LITERALCONST(ext)
CAESARIA_LITERALCONST(talks)

class StartMenu::Impl
{
public:
  Picture bgPicture;
  gui::StartMenu* menu;         // menu to display
  int result;
  bool isStopped;
  Game* game;
  Engine* engine;
  std::string fileMap;
  std::string playerName;

  void handleNewGame();
  void resolveCredits();
  void showLoadMenu();
  void showOptionsMenu();
  void resolveLoadRandommap();
  void showMainMenu();
  void showSoundOptions();
  void showVideoOptions();
  void showMissionSelector();
  void resolveQuitGame() { result=closeApplication; isStopped=true; }
  void resolveSelectFile( std::string fileName );
  void setPlayerName( std::string name );
  void openGreenlightPage() { OSystem::openUrl( "http://steamcommunity.com/sharedfiles/filedetails/?id=249746982" ); }
  void openHomePage() { OSystem::openUrl( "https://bitbucket.org/dalerank/caesaria/wiki/Home" ); }
  void resolveShowLoadMapWnd();
  void resolveShowLoadGameWnd();
  void handleStartCareer();
  void resolveShowChangeLanguageWindow();
  void resolveChangeLanguage(const gui::ListBoxItem&);
  void fitScreenResolution();
  void playMenuSoundTheme();
  void reload();
};

void StartMenu::Impl::resolveShowLoadGameWnd()
{
  gui::Widget* parent = game->gui()->rootWidget();

  vfs::Path savesPath = SETTINGS_VALUE( savedir ).toString();
  std::string defaultExt = SETTINGS_VALUE( saveExt ).toString();

  result = StartMenu::loadSavedGame;
  gui::LoadFileDialog* wnd = new gui::LoadFileDialog( parent, Rect(), savesPath, defaultExt,-1 );
  wnd->setCenter( parent->center() );
  wnd->setMayDelete( true );

  CONNECT( wnd, onSelectFile(), this, Impl::resolveSelectFile );
  wnd->setTitle( _("##mainmenu_loadgame##") );
}

void StartMenu::Impl::fitScreenResolution()
{
  gfx::Engine::Modes modes = game->engine()->modes();
  SETTINGS_SET_VALUE( resolution, modes.front() );
  SETTINGS_SET_VALUE( screenFitted, true );
  GameSettings::save();
}

void StartMenu::Impl::playMenuSoundTheme()
{
  audio::Engine::instance().play( "rome6", 50, audio::themeSound );
}

void StartMenu::Impl::reload()
{
  result = StartMenu::reloadScreen;
  isStopped = true;
}

void StartMenu::Impl::showSoundOptions()
{
  events::GameEventPtr e = events::SetSoundOptions::create();
  e->dispatch();
}

void StartMenu::Impl::resolveShowChangeLanguageWindow()
{
  gui::Widget* parent = game->gui()->rootWidget();
  Size rootSize = parent->size();
  Size windowSize( 512, 384 );
  Rect rect( Point( (rootSize - windowSize).width() / 2, ( rootSize - windowSize ).height() / 2),
             windowSize );

  gui::Label* frame = new gui::Label( parent, rect, "", false, gui::Label::bgWhiteFrame );
  gui::ListBox* lbx = new gui::ListBox( frame, Rect( 0, 0, 1, 1 ), -1, true, true );
  gui::PushButton* btn = new gui::PushButton( frame, Rect( 0, 0, 1, 1), "Apply" );

  lbx->setGeometry( RectF( 0.05, 0.05, 0.95, 0.85 ) );
  btn->setGeometry( RectF( 0.1, 0.88, 0.9, 0.94 ) );

  VariantMap languages = SaveAdapter::load( SETTINGS_RC_PATH( langModel ) );
  std::string currentLang = SETTINGS_VALUE( language ).toString();
  int currentIndex = -1;
  foreach( it, languages )
  {
    lbx->addItem( it->first );
    std::string ext = it->second.toMap().get( "ext" ).toString();
    if( ext == currentLang )
      currentIndex = std::distance( languages.begin(), it );
  }

  lbx->setSelected( currentIndex );

  CONNECT( lbx, onItemSelected(), this, Impl::resolveChangeLanguage );
  CONNECT( btn, onClicked(), this, Impl::reload );
}

void StartMenu::Impl::resolveChangeLanguage(const gui::ListBoxItem& item)
{
  std::string lang;
  std::string talksArchive;
  VariantMap languages = SaveAdapter::load( SETTINGS_RC_PATH( langModel ) );
  foreach( it, languages )
  {
    if( item.text() == it->first )
    {
      VariantMap vm = it->second.toMap();
      lang = vm[ lc_ext ].toString();
      talksArchive = vm[ lc_talks ].toString();
      break;
    }
  }

  SETTINGS_SET_VALUE( language, Variant( lang ) );
  SETTINGS_SET_VALUE( talksArchive, Variant( talksArchive ) );

  Locale::setLanguage( lang );
  audio::Helper::initTalksArchive( SETTINGS_RC_PATH( talksArchive ) );
}

void StartMenu::Impl::handleStartCareer()
{
  gui::WindowPlayerName* dlg = new gui::WindowPlayerName( game->gui()->rootWidget() );
  dlg->setModal();
  playerName = dlg->text();
  CONNECT( dlg, onNameChange(), this, Impl::setPlayerName );
  CONNECT( dlg, onClose(), this, Impl::handleNewGame );
}

void StartMenu::Impl::handleNewGame()
{  
  result=startNewGame; isStopped=true;
}

void StartMenu::Impl::resolveCredits()
{
  audio::Engine::instance().play( "combat_long", 50, audio::themeSound );
  gui::Widget* parent = game->gui()->rootWidget();

  Size size = engine->screenSize();
  std::string strs[] = { _("##developers##"),
                         " ",
                         "dalerank (dalerankn8@gmail.com)",
                         "gathanase (gathanase@gmail.com) render, game mechanics ",
                         "gecube (gb12335@gmail.com)",
                         "pecunia (pecunia@heavengames.com) game mechanics",
                         "tracertong",
                         "VladRassokhin",
                         "hellium",
                         "pufik6666",
                         "andreibranescu",
                         "amdmi3 (amdmi3@amdmi3.ru) bsd fixes",
                         "akuskis (?) aqueduct system",
                         "rovanion",
                         "nickers (2nickers@gmail.com)",
                         "ImperatorPrime",
                         "veprbl",
                         "ramMASTER",
                         "greg kennedy(kennedy.greg@gmail.com) smk decoder",
                         " ",
                         _("##testers##"),
                         " ",
                         "radek liška",
                         "dimitrius (caesar-iii.ru)",
                         "shibanirm",
                         "Pavel Aleksandrov (krabanek@gmail.com)",
                         " ",
                         _("##graphics##"),
                         " ",
                         "dimitrius (caesar-iii.ru)",
                         "aneurysm (4pda.ru)",
                         " ",
                         _("##localization##"),
                         "",
                         "Alexander Klimenko (?)"
                         "",
                         _("##thanks_to##"),
                         " ",
                         "doc (doc@nnm.me)",
                         "Juan Font Alonso ",
                         "Mephistopheles",
                         "" };

  gui::Label* frame = new gui::Label( parent, Rect( Point( 0, 0), size ), "", false, gui::Label::bgSimpleBlack );
  frame->setAlpha( 0xa0 );
  int h = size.height();
  for( int i=0; !strs[i].empty(); i++ )
  {
    Label* lb = new gui::Label( frame, Rect( 0, h + i * 20, size.width(), h + (i + 1) * 20), strs[i] );
    lb->setTextAlignment( align::center, align::center );
    lb->setFont( Font::create( FONT_2_WHITE ) );
    PositionAnimator* anim = new PositionAnimator( lb, WidgetAnimator::removeSelf | WidgetAnimator::removeParent, Point( 0, -20), 10000 );
    anim->setSpeed( PointF( 0, -0.5 ) );
  }

  gui::PushButton* btn = new gui::PushButton( frame,
                                              Rect( size.width() - 150, size.height() - 34, size.width() - 10, size.height() - 10 ),
                                              _("##close##") );
  CONNECT( btn, onClicked(), frame, gui::Label::deleteLater );
  CONNECT( btn, onClicked(), this, Impl::playMenuSoundTheme );
}

void StartMenu::Impl::showLoadMenu()
{
  menu->clear();

  gui::PushButton* btn = menu->addButton( _("##mainmenu_playmission##"), -1 );
  CONNECT( btn, onClicked(), this, Impl::showMissionSelector );

  btn = menu->addButton( _("##mainmenu_loadgame##"), -1 );
  CONNECT( btn, onClicked(), this, Impl::resolveShowLoadGameWnd );

  btn = menu->addButton( _("##mainmenu_loadmap##"), -1 );
  CONNECT( btn, onClicked(), this, Impl::resolveShowLoadMapWnd );

  btn = menu->addButton( _("##mainmenu_loadcampaign##"), -1 );
  //CONNECT( btn, onClicked(), this, Impl::resolveShowLoadMapWnd );

  btn = menu->addButton( _("##mainmenu_randommap##"), -1 );
  CONNECT( btn, onClicked(), this, Impl::resolveLoadRandommap );

  btn = menu->addButton( _("##cancel##"), -1 );
  CONNECT( btn, onClicked(), this, Impl::showMainMenu );
}

void StartMenu::Impl::resolveLoadRandommap()
{
  result = StartMenu::loadMission;
  fileMap = GameSettings::rcpath( "/missions/random.mission" ).toString();
  isStopped = true;
}

void StartMenu::Impl::showOptionsMenu()
{
  menu->clear();

  gui::PushButton* btn = menu->addButton( _("##mainmenu_language##"), -1 );
  CONNECT( btn, onClicked(), this, Impl::resolveShowChangeLanguageWindow );

  btn = menu->addButton( _("##mainmenu_video##"), -1 );
  CONNECT( btn, onClicked(), this, Impl::showVideoOptions );

  btn = menu->addButton( _("##mainmenu_sound##"), -1 );
  CONNECT( btn, onClicked(), this, Impl::showSoundOptions );

  //btn = menu->addButton( _("##mainmenu_game##"), -1 );
  //CONNECT( btn, onClicked(), this, Impl::resolveShowChangeLanguageWindow );

  btn = menu->addButton( _("##cancel##"), -1 );
  CONNECT( btn, onClicked(), this, Impl::showMainMenu );
}

void StartMenu::Impl::showMainMenu()
{
  menu->clear();

  DebugTimer::reset( "init_b" );
  gui::PushButton* btn = menu->addButton( _("##mainmenu_newgame##"), -1 );
  CONNECT( btn, onClicked(), this, Impl::handleStartCareer );
  DebugTimer::check( "", "init_b" );

  btn = menu->addButton( _("##mainmenu_load##"), -1 );
  CONNECT( btn, onClicked(), this, Impl::showLoadMenu );

  btn = menu->addButton( _("##mainmenu_options##"), -1 );
  CONNECT( btn, onClicked(), this, Impl::showOptionsMenu );

  btn = menu->addButton( _("##mainmenu_credits##"), -1 );
  CONNECT( btn, onClicked(), this, Impl::resolveCredits );

  btn = menu->addButton( _("##mainmenu_quit##"), -1 );
  CONNECT( btn, onClicked(), this, Impl::resolveQuitGame );
}

void StartMenu::Impl::showVideoOptions()
{
  events::GameEventPtr event = events::SetVideoSettings::create();
  event->dispatch();
}

void StartMenu::Impl::showMissionSelector()
{
  Widget* parent = game->gui()->rootWidget();

  result = StartMenu::loadMission;
  LoadMissionDialog* wnd = LoadMissionDialog::create( parent, vfs::Path( ":/missions/" ) );

  CONNECT( wnd, onSelectFile(), this, Impl::resolveSelectFile );
}

void StartMenu::Impl::resolveSelectFile(std::string fileName)
{
  fileMap = fileName;
  isStopped = true;
}

void StartMenu::Impl::setPlayerName(std::string name) {  playerName = name; }

void StartMenu::Impl::resolveShowLoadMapWnd()
{
  gui::Widget* parent = game->gui()->rootWidget();

  gui::LoadFileDialog* wnd = new gui::LoadFileDialog( parent,
                                                    Rect(),
                                                    GameSettings::rcpath( "/maps/" ), ".map",
                                                    -1 );
  wnd->setCenter( parent->center() );
  wnd->setMayDelete( false );

  result = StartMenu::loadMap;
  CONNECT( wnd, onSelectFile(), this, Impl::resolveSelectFile );
  wnd->setTitle( _("##mainmenu_loadmap##") );
}

StartMenu::StartMenu( Game& game, Engine& engine ) : _d( new Impl )
{
  _d->bgPicture = Picture::getInvalid();
  _d->isStopped = false;
  _d->game = &game;
  _d->engine = &engine;
}

StartMenu::~StartMenu() {}

void StartMenu::draw()
{
  _d->game->gui()->beforeDraw();

  _d->engine->draw(_d->bgPicture, 0, 0);

  _d->game->gui()->draw();
}

void StartMenu::handleEvent( NEvent& event )
{
  if (event.EventType == sEventQuit)
  {
    _d->resolveQuitGame();
  }

  _d->game->gui()->handleEvent( event );
}

void StartMenu::initialize()
{
  Logger::warning( "ScreenMenu: initialize start");
  _d->bgPicture = Picture::load("title", 1);


  // center the bgPicture on the screen
  Size tmpSize = (_d->engine->screenSize() - _d->bgPicture.size())/2;
  _d->bgPicture.setOffset( Point( tmpSize.width(), -tmpSize.height() ) );

  _d->game->gui()->clear();

  _d->menu = new gui::StartMenu( _d->game->gui()->rootWidget() );

  gui::TexturedButton* btnGreenlight = new gui::TexturedButton( _d->game->gui()->rootWidget(), Point(), Size( 250, 155), -1,
                                                                "greenlight", 1, 2, 2, 2 );
  CONNECT( btnGreenlight, onClicked(), _d.data(), Impl::openGreenlightPage );

  Size scrSize = _d->engine->screenSize();
  gui::TexturedButton* btnHomePage = new gui::TexturedButton( _d->game->gui()->rootWidget(),
                                                              Point( scrSize.width() - 97, scrSize.height() - 75 ), Size( 97, 75 ), -1,
                                                              "logo_rdt", 1, 2, 2, 2 );
  CONNECT( btnHomePage, onClicked(), _d.data(), Impl::openHomePage );

  _d->showMainMenu();

#ifdef CAESARIA_PLATFORM_ANDROID
  bool screenFitted = SETTINGS_VALUE( screenFitted );
  if( !screenFitted )
  {
    gui::DialogBox* dialog = new gui::DialogBox( _d->game->gui()->rootWidget(),  Rect( 0, 0, 400, 150 ),
                                                 "Information", "Is need autofit screen resolution?",
                                                 gui::DialogBox::btnOk | gui::DialogBox::btnCancel );
    CONNECT(dialog, onOk(), dialog, gui::DialogBox::deleteLater );
    CONNECT(dialog, onCancel(), dialog, gui::DialogBox::deleteLater );
    CONNECT(dialog, onOk(), _d.data(), Impl::fitScreenResolution );
    dialog->show();
  }
#else
  _d->playMenuSoundTheme();
#endif
}

int StartMenu::result() const{  return _d->result;}
bool StartMenu::isStopped() const{  return _d->isStopped;}
std::string StartMenu::mapName() const{  return _d->fileMap;}
std::string StartMenu::playerName() const { return _d->playerName; }

}//end namespace scene
