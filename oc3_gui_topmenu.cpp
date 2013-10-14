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

#include "oc3_gettext.hpp"
#include "oc3_gui_topmenu.hpp"
#include "oc3_gui_label.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_contextmenuitem.hpp"
#include "oc3_stringhelper.hpp"
#include "oc3_time.hpp"
#include "oc3_gfx_engine.hpp"
#include "oc3_enums.hpp"
#include "oc3_gamedate.hpp"
#include "oc3_game_settings.hpp"

namespace {
static const int dateLabelOffset = 155;
static const int populationLabelOffset = 344;
static const int fundLabelOffset = 464;
static const int panelBgStatus = 15;
}

class TopMenu::Impl
{
public:
  DateTime saveDate;
  Label* lbPopulation;
  Label* lbFunds;
  Label* lbDate;
  ContextMenu* langSelect;
  PictureRef bgPicture;

  void resolveSave();
  void updateDate();

oc3_signals public:
  Signal0<> onExitSignal;
  Signal0<> onEndSignal;
  Signal0<> onSaveSignal;
  Signal0<> onLoadSignal;
  Signal0<> onShowVideoOptionsSignal;
  Signal0<> onShowGameSpeedOptionsSignal;
  Signal1<int> onRequestAdvisorSignal;
};

/*bool TopMenu::onEvent(const NEvent& event)
{
  return MainMenu::onEvent(event);
}*/

void TopMenu::draw( GfxEngine& engine )
{
  if( !isVisible() )
    return;

  _d->updateDate();

  engine.drawPicture( *_d->bgPicture, getScreenLeft(), getScreenTop() );

  MainMenu::draw( engine );
}

void TopMenu::setPopulation( int value )
{
  if( _d->lbPopulation )
    _d->lbPopulation->setText( StringHelper::format( 0xff, "%s %d", _("##population_short##"), value ) );
}

void TopMenu::setFunds( int value )
{
  if( _d->lbFunds )
    _d->lbFunds->setText( StringHelper::format( 0xff, "%.2s %d", _("##denarii_short##"), value) );
}

void TopMenu::Impl::updateDate()
{
  if( !lbDate || saveDate.getMonth() == GameDate::current().getMonth() )
    return;

  saveDate = GameDate::current();

  std::string month = _( StringHelper::format( 0xff, "##month_%d_short##", saveDate.getMonth() + 1).c_str() );
  std::string age = _( StringHelper::format( 0xff, "##age_%s##", saveDate.getYear() > 0 ? "ad" : "bc" ).c_str() );
  std::string text = StringHelper::format( 0xff, "%s %d %s",
                                           month.c_str(), abs( saveDate.getYear() ), age.c_str());
  lbDate->setText( text );
}

TopMenu::TopMenu( Widget* parent, const int height ) 
: MainMenu( parent, Rect( 0, 0, parent->getWidth(), height ) ),
  _d( new Impl )
{
  setupUI( GameSettings::rcpath( "/gui/topmenu.gui" ) );
  setGeometry( Rect( 0, 0, parent->getWidth(), height ) );

  PicturesArray p_marble;
  for (int i = 1; i<=12; ++i)
  {
    p_marble.push_back( Picture::load( ResourceGroup::panelBackground, i));
  }

  _d->bgPicture.reset( Picture::create( getSize() ) );

  int i = 0;
  unsigned int x = 0;
  while (x < getWidth())
  {
    const Picture& pic = p_marble[i%10];
    _d->bgPicture->draw( pic, x, 0);
    x += pic.getWidth();
    i++;
  }
  _d->lbPopulation = findChild<Label*>( "lbPopulation" );
  if( _d->lbPopulation )
    _d->lbPopulation->setPosition( Point( getWidth() - populationLabelOffset, 0 ) );

  _d->lbFunds = findChild<Label*>( "lbFunds" );
  if( _d->lbFunds )
    _d->lbFunds->setPosition(  Point( getWidth() - fundLabelOffset, 0) );

  _d->lbDate = findChild<Label*>( "lbDate" );
  if( _d->lbDate )
    _d->lbDate->setPosition( Point( getWidth() - dateLabelOffset, 0) );

  ContextMenuItem* tmp = addItem( _("##gmenu_file##"), -1, true, true, false, false );
  ContextMenu* file = tmp->addSubMenu();

  /*ContextMenuItem* newGame = */file->addItem( _("##gmenu_file_new##"), -1, true, false, false, false );
  /*ContextMenuItem* restart = */ file->addItem( _("##gmenu_file_restart##"), -1, true, false, false, false );
  ContextMenuItem* load = file->addItem( _("##gmenu_file_load##"), -1, true, false, false, false );
  ContextMenuItem* save = file->addItem( _("##gmenu_file_save##"), -1, true, false, false, false );
  ContextMenuItem* mainMenu = file->addItem( _("##gmenu_file_mainmenu##"), -1, true, false, false, false );
  ContextMenuItem* exit = file->addItem( _("##gmenu_file_exit##"), -1, true, false, false, false );

  CONNECT( exit, onClicked(), &_d->onExitSignal, Signal0<>::emit );
  CONNECT( save, onClicked(), &_d->onSaveSignal, Signal0<>::emit );
  CONNECT( load, onClicked(), &_d->onLoadSignal, Signal0<>::emit );
  CONNECT( mainMenu, onClicked(), &_d->onEndSignal, Signal0<>::emit );

  tmp = addItem( _("##gmenu_options##"), -1, true, true, false, false );
  ContextMenu* options = tmp->addSubMenu();
  ContextMenuItem* screen = options->addItem( _("##screen_options##"), -1, true, false, false, false );  
  /*ContextMenuItem* sound = */options->addItem( _("##sound_options##"), -1, true, false, false, false );
  ContextMenuItem* speed = options->addItem( _("##speed_options##"), -1, true, false, false, false );
  ContextMenuItem* language = options->addItem( _("##select_language##"), -1, true, true, false, false );
  _d->langSelect = language->addSubMenu();
  ContextMenuItem* russianLng = _d->langSelect->addItem( _("##russian_lang##"), 0xf001, true, false, false, false );
  ContextMenuItem* englishLng = _d->langSelect->addItem( _("##english_lang##"), 0xf002, true, false, false, false );

  CONNECT( russianLng, onClicked(), this, TopMenu::resolveSelectLanguage );
  CONNECT( englishLng, onClicked(), this, TopMenu::resolveSelectLanguage );

  CONNECT( screen, onClicked(), &_d->onShowVideoOptionsSignal,     Signal0<>::emit );
  CONNECT( speed,  onClicked(), &_d->onShowGameSpeedOptionsSignal, Signal0<>::emit );

  tmp = addItem( _("##gmenu_help##"), -1, true, true, false, false );
  tmp = addItem( _("##gmenu_advisors##"), -1, true, true, false, false );
  ContextMenu* advisersMenu = tmp->addSubMenu();
  advisersMenu->addItem( _("##adv_employments_m##"), ADV_EMPLOYERS );
  advisersMenu->addItem( _("##adv_military_m##"), ADV_LEGION );
  advisersMenu->addItem( _("##adv_empire_m##"), ADV_EMPIRE );
  advisersMenu->addItem( _("##adv_ratings_m##"), ADV_RATINGS );
  advisersMenu->addItem( _("##adv_trade_m##"), ADV_TRADING );
  advisersMenu->addItem( _("##adv_population_m##"), ADV_POPULATION );
  advisersMenu->addItem( _("##adv_health_m##"), ADV_HEALTH );
  advisersMenu->addItem( _("##adv_education_m##"), ADV_EDUCATION );
  advisersMenu->addItem( _("##adv_religion_m##"), ADV_RELIGION );
  advisersMenu->addItem( _("##adv_entertainment_m##"), ADV_ENTERTAINMENT );
  advisersMenu->addItem( _("##adv_finance_m##"), ADV_FINANCE );
  advisersMenu->addItem( _("##adv_main_m##"), ADV_MAIN );
  CONNECT( advisersMenu, onItemAction(), &(_d->onRequestAdvisorSignal), Signal1<int>::emit );
}

Signal0<>& TopMenu::onExit()
{
  return _d->onExitSignal;
}

Signal0<>& TopMenu::onSave()
{
  return _d->onSaveSignal;
}

Signal0<>& TopMenu::onEnd()
{
  return _d->onEndSignal;
}

Signal1<int>& TopMenu::onRequestAdvisor()
{
  return _d->onRequestAdvisorSignal;
}

void TopMenu::resolveSelectLanguage()
{
  if( ContextMenuItem* current = _d->langSelect->getSelectedItem() )
  {
    switch( current->getCommandId() )
    {
    case 0xf001: putenv( "LC_ALL=ru_RU" ); break;

    case 0xf002:
    default:
        putenv( "LC_ALL=en_US" );
    break;
    }
  }
}

Signal0<>& TopMenu::onLoad()
{
  return _d->onLoadSignal;
}

Signal0<>& TopMenu::onShowVideoOptions()
{
  return _d->onShowVideoOptionsSignal;
}

Signal0<>&TopMenu::onShowGameSpeedOptions()
{
  return _d->onShowGameSpeedOptionsSignal;
}
