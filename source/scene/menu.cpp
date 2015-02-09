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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

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
#include "gui/widgetescapecloser.hpp"
#include "core/event.hpp"
#include "gui/package_options_window.hpp"
#include "core/timer.hpp"
#include "core/variant_map.hpp"
#include "events/dispatcher.hpp"
#include "core/utils.hpp"
#include "walker/name_generator.hpp"
#include "gui/image.hpp"
#include "steam.hpp"

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
  bool isStopped;
  Game* game;
  Engine* engine;
  std::string fileMap;
  std::string playerName;
  int result;

  Picture userImage;
  gui::Label* lbSteamName;

public:
  void handleNewGame();
  void resolveCredits();
  void showLoadMenu();
  void showNewGame();
  void showOptionsMenu();
  void resolveLoadRandommap();
  void showMainMenu();
  void showSoundOptions();
  void showVideoOptions();
  void showMissionSelector();
  void resolveQuitGame() { result=closeApplication; isStopped=true; }
  void resolveSelectFile( std::string fileName );
  void setPlayerName( std::string name );
  void openSteamPage() { OSystem::openUrl( "http://store.steampowered.com/app/327640" ); }
  void openHomePage() { OSystem::openUrl( "https://bitbucket.org/dalerank/caesaria/wiki/Home" ); }
  void resolveShowLoadMapWnd();
  void resolveShowLoadGameWnd();
  void handleStartCareer();
  void showLanguageOptions();
  void showPackageOptions();
  void resolveChangeLanguage(const gui::ListBoxItem&);
  void fitScreenResolution();
  void playMenuSoundTheme();
  void resolveSteamStats();
  void reload();
};

void StartMenu::Impl::resolveShowLoadGameWnd()
{
  Widget* parent = game->gui()->rootWidget();

  vfs::Path savesPath = SETTINGS_VALUE( savedir ).toString();
  std::string defaultExt = SETTINGS_VALUE( saveExt ).toString();

  result = StartMenu::loadSavedGame;
  gui::LoadFileDialog* wnd = new gui::LoadFileDialog( parent, Rect(), savesPath, defaultExt,-1 );
  wnd->setShowExtension( false );
  wnd->setMayDelete( true );

  CONNECT( wnd, onSelectFile(), this, Impl::resolveSelectFile );
  wnd->setTitle( _("##mainmenu_loadgame##") );
  wnd->setText( _("##load_this_game##") );
}

void StartMenu::Impl::fitScreenResolution()
{
  gfx::Engine::Modes modes = game->engine()->modes();
  SETTINGS_SET_VALUE( resolution, modes.front() );
  SETTINGS_SET_VALUE( screenFitted, true );
  game::Settings::save();
}

void StartMenu::Impl::playMenuSoundTheme()
{
  audio::Engine::instance().play( "rome6", 50, audio::themeSound );
}

void scene::StartMenu::Impl::resolveSteamStats()
{
#ifdef CAESARIA_USE_STEAM
  int offset = 0;
  for( int k=0; k < steamapi::achievementNumber; k++ )
  {
    steamapi::AchievementType achivId = steamapi::AchievementType(k);
    if( steamapi::isAchievementReached( achivId ) )
    {
      gfx::Picture pic = steamapi::achievementImage( achivId );
      if( pic.isValid() )
      {
        gui::Image* img = new gui::Image( game->gui()->rootWidget(), Point( 10, 100 + offset ), pic );
        img->setTooltipText( steamapi::achievementCaption( achivId ) );
        offset += 65;
      }
    }
  }
#endif
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

void StartMenu::Impl::showLanguageOptions()
{
  Widget* parent = game->gui()->rootWidget();
  Size windowSize( 512, 384 );

  Label* frame = new Label( parent, Rect( Point(), windowSize ), "", false, gui::Label::bgWhiteFrame );
  ListBox* lbx = new ListBox( frame, Rect( 0, 0, 1, 1 ), -1, true, true );
  PushButton* btn = new PushButton( frame, Rect( 0, 0, 1, 1), "Apply" );

  WidgetEscapeCloser::insertTo( frame );
  frame->setCenter( parent->center() );
  lbx->setFocus();
  lbx->setGeometry( RectF( 0.05, 0.05, 0.95, 0.85 ) );
  btn->setGeometry( RectF( 0.1, 0.88, 0.9, 0.95 ) );

  VariantMap languages = config::load( SETTINGS_RC_PATH( langModel ) );
  std::string currentLang = SETTINGS_VALUE( language ).toString();
  int currentIndex = -1;
  foreach( it, languages )
  {
    lbx->addItem( it->first );
    std::string ext = it->second.toMap().get( lc_ext ).toString();
    if( ext == currentLang )
      currentIndex = std::distance( languages.begin(), it );
  }

  lbx->setSelected( currentIndex );

  CONNECT( lbx, onItemSelected(), this, Impl::resolveChangeLanguage );
  CONNECT( btn, onClicked(), this, Impl::reload );
}

void StartMenu::Impl::showPackageOptions()
{
  dialog::PackageOptions* dlg = new dialog::PackageOptions( game->gui()->rootWidget(), Rect() );
  dlg->setModal();
}

void StartMenu::Impl::resolveChangeLanguage(const gui::ListBoxItem& item)
{
  std::string lang;
  std::string talksArchive;
  VariantMap languages = config::load( SETTINGS_RC_PATH( langModel ) );
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
  game::Settings::save();

  Locale::setLanguage( lang );
  NameGenerator::instance().setLanguage( lang );
  audio::Helper::initTalksArchive( SETTINGS_RC_PATH( talksArchive ) );
}

void StartMenu::Impl::handleStartCareer()
{
  menu->clear();

  dialog::ChangePlayerName* dlg = new dialog::ChangePlayerName( game->gui()->rootWidget() );
  playerName = dlg->text();

  CONNECT( dlg, onNameChange(), this, Impl::setPlayerName );
  CONNECT( dlg, onNewGame(), this, Impl::handleNewGame );
  CONNECT( dlg, onClose(), this, Impl::showMainMenu );
}

void StartMenu::Impl::handleNewGame()
{  
  result=startNewGame; isStopped=true;
}

void StartMenu::Impl::resolveCredits()
{
  audio::Engine::instance().play( "combat_long", 50, audio::themeSound );
  Widget* parent = game->gui()->rootWidget();

  Size size = engine->screenSize();
  std::string strs[] = { _("##original_game##"),
                         "Caesar III (c)",
                         "Thank you, Impressions Games, for amazing game",
                         " ",
                         _("##developers##"),
                         " ",
                         "dalerank (dalerankn8@gmail.com)",
                         "gathanase (gathanase@gmail.com) render, game mechanics ",
                         "gecube (gb12335@gmail.com), Softer (softer@lin.in.ua)",
                         "pecunia (pecunia@heavengames.com) game mechanics",
                         "amdmi3 (amdmi3@amdmi3.ru) bsd fixes",
                         "greg kennedy(kennedy.greg@gmail.com) smk decoder",
                         "akuskis (?) aqueduct system",
                         "ImperatorPrime, nickers, veprbl, ramMASTER",
                         "tracertong, VladRassokhin, hellium",
                         "pufik6666, andreibranescu, rovanion",
                         " ",
                         _("##operations_manager##"),
                         " ",
                         "Max Mironchik (?) ",
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
                         "Dmitry Plotnikov",                         
                         " ",
                         _("##music##"),
                         " ",
                         "Aliaksandr BeatCheat (www.beatcheat.net)",
                         " ",
                         _("##localization##"),
                         " ",
                         "Alexander Klimenko, Manuel Alvarez",
                         " ",
                         _("##thanks_to##"),
                         " ",
                         "vk.com/caesaria-game, dimitrius (caesar-iii.ru), aneurysm (4pda.ru)",
                         "Aleksandr Egorov, Juan Font Alonso, Mephistopheles",
                         "ed19837, vladimir.rurukin, Safronov Alexey, Alexander Skidanov",
                         "Kostyantyn Moroz, Andrew, Nikita Gradovich, bogdhnu",
                         "deniskravtsov, Vhall, Dmitry Vorobiev, yevg.mord",
                         "mmagir,Yury Vidineev, Pavel Aleynikov, brickbtv",
                         "dovg1, KONSTANTIN KITMANOV, Serge Savostin, Memfis",
                         "MennyCalavera, Anastasia Smolskaya, niosus, SkidanovAlex",
                         "Zatolokinandrey, yuri_abzyanov, dmitrii.dukhonchenko, twilight.temple",
                         "holubmarek,butjer1010, Agmenor Ultime, m0nochr0mex, Alexeyco",
                         "rad.n,jsimek.cz, saintech,phdarcy, Casey Knauss, meikit2000",
                         "" };

  Label* frame = new Label( parent, Rect( Point( 0, 0), size ), "", false, gui::Label::bgSimpleBlack );
  WidgetEscapeCloser::insertTo( frame );
  frame->setAlpha( 0xa0 );
  int h = size.height();
  for( int i=0; !strs[i].empty(); i++ )
  {
    Label* lb = new Label( frame, Rect( 0, h + i * 20, size.width(), h + (i + 1) * 20), strs[i] );
    lb->setTextAlignment( align::center, align::center );
    lb->setFont( Font::create( FONT_2_WHITE ) );
    lb->setSubElement( true );
    PositionAnimator* anim = new PositionAnimator( lb, WidgetAnimator::removeSelf | WidgetAnimator::removeParent, Point( 0, -20), 10000 );
    anim->setSpeed( PointF( 0, -0.5 ) );
  }

  gui::PushButton* btn = new gui::PushButton( frame,
                                              Rect( size.width() - 150, size.height() - 34, size.width() - 10, size.height() - 10 ),
                                              _("##close##") );
  frame->setFocus();

  CONNECT( btn, onClicked(), frame, gui::Label::deleteLater );
  CONNECT( btn, onClicked(), this, Impl::playMenuSoundTheme );
}

void StartMenu::Impl::showLoadMenu()
{
  menu->clear();

  PushButton* btn = menu->addButton( _("##mainmenu_playmission##"), -1 );
  CONNECT( btn, onClicked(), this, Impl::showMissionSelector );

  btn = menu->addButton( _("##mainmenu_loadgame##"), -1 );
  CONNECT( btn, onClicked(), this, Impl::resolveShowLoadGameWnd );

  btn = menu->addButton( _("##mainmenu_loadmap##"), -1 );
  CONNECT( btn, onClicked(), this, Impl::resolveShowLoadMapWnd );

  //btn = menu->addButton( _("##mainmenu_loadcampaign##"), -1 );
  //CONNECT( btn, onClicked(), this, Impl::resolveShowLoadMapWnd );

  btn = menu->addButton( _("##cancel##"), -1 );
  CONNECT( btn, onClicked(), this, Impl::showMainMenu );
}

void StartMenu::Impl::resolveLoadRandommap()
{
  result = StartMenu::loadMission;
  fileMap = ":/missions/random.mission";
  isStopped = true;
}

void StartMenu::Impl::showOptionsMenu()
{
  menu->clear();

  PushButton* btn = menu->addButton( _("##mainmenu_language##"), -1 );
  CONNECT( btn, onClicked(), this, Impl::showLanguageOptions );

  btn = menu->addButton( _("##mainmenu_video##"), -1 );
  CONNECT( btn, onClicked(), this, Impl::showVideoOptions );

  btn = menu->addButton( _("##mainmenu_sound##"), -1 );
  CONNECT( btn, onClicked(), this, Impl::showSoundOptions );

  btn = menu->addButton( _("##mainmenu_package##"), -1 );
  CONNECT( btn, onClicked(), this, Impl::showPackageOptions );

  btn = menu->addButton( _("##cancel##"), -1 );
  CONNECT( btn, onClicked(), this, Impl::showMainMenu );
}

void StartMenu::Impl::showNewGame()
{
  menu->clear();

  PushButton* btn = menu->addButton( _("##mainmenu_startcareer##"), -1 );
  CONNECT( btn, onClicked(), this, Impl::handleStartCareer );

  btn = menu->addButton( _("##mainmenu_randommap##"), -1 );
  CONNECT( btn, onClicked(), this, Impl::resolveLoadRandommap );

  btn = menu->addButton( _("##cancel##"), -1 );
  CONNECT( btn, onClicked(), this, Impl::showMainMenu );
}

void StartMenu::Impl::showMainMenu()
{
  menu->clear();

  PushButton* btn = menu->addButton( _("##mainmenu_newgame##"), -1 );
  CONNECT( btn, onClicked(), this, Impl::showNewGame );

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
  dialog::LoadMission* wnd = dialog::LoadMission::create( parent, vfs::Path( ":/missions/" ) );

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
  Widget* parent = game->gui()->rootWidget();

  LoadFileDialog* wnd = new gui::LoadFileDialog( parent,
                                                 Rect(),
                                                 vfs::Path( ":/maps/" ), ".map,.sav,.omap",
                                                 -1 );
  wnd->setMayDelete( false );

  result = StartMenu::loadMap;
  CONNECT( wnd, onSelectFile(), this, Impl::resolveSelectFile );
  wnd->setTitle( _("##mainmenu_loadmap##") );
  wnd->setText( _("##start_this_map##") );
}

StartMenu::StartMenu( Game& game, Engine& engine ) : _d( new Impl )
{
  _d->bgPicture = Picture::getInvalid();
  _d->isStopped = false;
  _d->game = &game;
  _d->userImage = Picture::getInvalid();
  _d->engine = &engine;
}

StartMenu::~StartMenu() {}

void StartMenu::draw()
{
  _d->game->gui()->beforeDraw();

  _d->engine->draw(_d->bgPicture, 0, 0);

  _d->game->gui()->draw();

#ifdef CAESARIA_USE_STEAM
  _d->engine->draw( _d->userImage, Point( 20, 20 ) );
#endif
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
  events::Dispatcher::instance().reset();
  Logger::warning( "ScreenMenu: initialize start");
  std::string resName = SETTINGS_VALUE( titleResource ).toString();
  _d->bgPicture = Picture::load( resName, 1);

  // center the bgPicture on the screen
  Size tmpSize = (_d->engine->virtualSize() - _d->bgPicture.size())/2;
  _d->bgPicture.setOffset( Point( tmpSize.width(), -tmpSize.height() ) );

  _d->game->gui()->clear();

  _d->menu = new gui::StartMenu( _d->game->gui()->rootWidget() );

  Size scrSize = _d->engine->virtualSize();
  TexturedButton* btnHomePage = new TexturedButton( _d->game->gui()->rootWidget(),
                                                              Point( scrSize.width() - 128, scrSize.height() - 100 ), Size( 128 ), -1,
                                                              "logo_rdt", 1, 2, 2, 2 );

  TexturedButton* btnSteamPage = new TexturedButton( _d->game->gui()->rootWidget(), Point( btnHomePage->left() - 128, scrSize.height() - 100 ),  Size( 128 ), -1,
                                                                "steam_icon", 1, 2, 2, 2 );

  CONNECT( btnSteamPage, onClicked(), _d.data(), Impl::openSteamPage );
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

#ifdef CAESARIA_USE_STEAM
  steamapi::init();

  std::string steamName = steamapi::userName();
  _d->userImage = steamapi::userImage();
  if( steamName.empty() )
  {
    OSystem::error( "Error", "Cant login in Steam" );
    _d->isStopped = true;
    _d->result = closeApplication;
    return;
  }

  std::string text = utils::format( 0xff, "Build %d\n%s", CAESARIA_BUILD_NUMBER, steamName.c_str() );
  _d->lbSteamName = new Label( _d->game->gui()->rootWidget(), Rect( 100, 10, 400, 80 ), text );
  _d->lbSteamName->setTextAlignment( align::upperLeft, align::center );
  _d->lbSteamName->setWordwrap( true );
  _d->lbSteamName->setFont( Font::create( FONT_3, DefaultColors::white ) );
#endif
}

void StartMenu::afterFrame()
{
  Base::afterFrame();

  static unsigned int saveTime = 0;
  events::Dispatcher::instance().update( *_d->game, saveTime++ );

#ifdef CAESARIA_USE_STEAM
  steamapi::update();
  if( steamapi::isStatsReceived() )
    _d->resolveSteamStats();
#endif
}

int StartMenu::result() const{  return _d->result;}
bool StartMenu::isStopped() const{  return _d->isStopped;}
std::string StartMenu::mapName() const{  return _d->fileMap;}
std::string StartMenu::playerName() const { return _d->playerName; }

}//end namespace scene
