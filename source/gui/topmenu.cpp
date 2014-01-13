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

#include "topmenu.hpp"

#include "core/gettext.hpp"
#include "label.hpp"
#include "game/resourcegroup.hpp"
#include "contextmenuitem.hpp"
#include "core/stringhelper.hpp"
#include "core/time.hpp"
#include "gfx/engine.hpp"
#include "game/enums.hpp"
#include "game/gamedate.hpp"
#include "game/settings.hpp"
#include "environment.hpp"
#include "core/logger.hpp"
#include "texturedbutton.hpp"

namespace gui
{

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
  void showAboutInfo();

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
    _d->lbPopulation->setText( StringHelper::format( 0xff, "%s %d", _("##pop##"), value ) );
}

void TopMenu::setFunds( int value )
{
  if( _d->lbFunds )
    _d->lbFunds->setText( StringHelper::format( 0xff, "%.2s %d", _("##denarii_short##"), value) );
}

void TopMenu::Impl::updateDate()
{
  if( !lbDate || saveDate.month() == GameDate::current().month() )
    return;

  saveDate = GameDate::current();

  std::string month = _( StringHelper::format( 0xff, "##month_%d_short##", saveDate.month() + 1).c_str() );
  std::string age = _( StringHelper::format( 0xff, "##age_%s##", saveDate.year() > 0 ? "ad" : "bc" ).c_str() );
  std::string text = StringHelper::format( 0xff, "%s %d %s",
                                           month.c_str(), abs( saveDate.year() ), age.c_str());
  lbDate->setText( text );
}

void TopMenu::Impl::showAboutInfo()
{
  Widget* parent = lbDate->getEnvironment()->getRootWidget();
  Size pSize = parent->getSize();
  Size mySize( 500, 300 );
  Rect rect( Point( (pSize.getWidth() - mySize.getWidth()) / 2, (pSize.getHeight() - mySize.getHeight()) / 2 ), mySize );
  Label* bg = new Label( parent, rect, "", false, Label::bgWhiteFrame );
  bg->setText( _("##about_caesaria_game##") );

  TexturedButton* btnExit = new TexturedButton( bg, Point( pSize.getWidth() - 39, pSize.getHeight() - 39), Size( 24 ), -1, ResourceMenu::exitInfBtnPicId );
  CONNECT( btnExit, onClicked(), bg, Label::deleteLater );
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

  /*ContextMenuItem* restart = */ file->addItem( _("##gmenu_file_restart##"), -1, true, false, false, false );
  ContextMenuItem* load = file->addItem( _("##mainmenu_loadgame##"), -1, true, false, false, false );
  ContextMenuItem* save = file->addItem( _("##gmenu_file_save##"), -1, true, false, false, false );
  ContextMenuItem* mainMenu = file->addItem( _("##gmenu_file_mainmenu##"), -1, true, false, false, false );
  ContextMenuItem* exit = file->addItem( _("##gmenu_exit_game##"), -1, true, false, false, false );

  CONNECT( exit, onClicked(), &_d->onExitSignal, Signal0<>::emit );
  CONNECT( save, onClicked(), &_d->onSaveSignal, Signal0<>::emit );
  CONNECT( load, onClicked(), &_d->onLoadSignal, Signal0<>::emit );
  CONNECT( mainMenu, onClicked(), &_d->onEndSignal, Signal0<>::emit );

  tmp = addItem( _("##gmenu_options##"), -1, true, true, false, false );
  ContextMenu* options = tmp->addSubMenu();
  ContextMenuItem* screen = options->addItem( _("##screen_settings##"), -1, true, false, false, false );
  /*ContextMenuItem* sound = */options->addItem( _("##sound_settings##"), -1, true, false, false, false );
  ContextMenuItem* speed = options->addItem( _("##speed_settings##"), -1, true, false, false, false );

  CONNECT( screen, onClicked(), &_d->onShowVideoOptionsSignal,     Signal0<>::emit );
  CONNECT( speed,  onClicked(), &_d->onShowGameSpeedOptionsSignal, Signal0<>::emit );

  tmp = addItem( _("##gmenu_help##"), -1, true, true, false, false );
  ContextMenu* helpMenu = tmp->addSubMenu();
  ContextMenuItem* aboutItem = helpMenu->addItem( _("##gmenu_about##"), -1 );
  CONNECT( aboutItem, onClicked(), _d.data(), Impl::showAboutInfo );

  tmp = addItem( _("##gmenu_advisors##"), -1, true, true, false, false );
  ContextMenu* advisersMenu = tmp->addSubMenu();
  advisersMenu->addItem( _("##adv_employments_m##"), ADV_EMPLOYERS );
  advisersMenu->addItem( _("##adv_military_m##"   ), ADV_LEGION );
  advisersMenu->addItem( _("##adv_empire_m##"     ), ADV_EMPIRE );
  advisersMenu->addItem( _("##adv_ratings_m##"    ), ADV_RATINGS );
  advisersMenu->addItem( _("##trade_advisor##"      ), ADV_TRADING );
  advisersMenu->addItem( _("##adv_population_m##" ), ADV_POPULATION );
  advisersMenu->addItem( _("##adv_health_m##"     ), ADV_HEALTH );
  advisersMenu->addItem( _("##adv_education_m##"  ), ADV_EDUCATION );
  advisersMenu->addItem( _("##adv_religion_m##"   ), ADV_RELIGION );
  advisersMenu->addItem( _("##adv_entertainment_m##"), ADV_ENTERTAINMENT );
  advisersMenu->addItem( _("##adv_finance_m##"    ), ADV_FINANCE );
  advisersMenu->addItem( _("##chief_advisor##"       ), ADV_MAIN );
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

}//end namespace gui
