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
#include "oc3_topmenu.hpp"
#include "oc3_gui_label.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_contextmenuitem.hpp"
#include "oc3_stringhelper.hpp"
#include "oc3_time.hpp"
#include "oc3_gfx_engine.hpp"
#include "oc3_enums.hpp"
#include "oc3_gamedate.hpp"

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
  PictureRef bgPicture;

  void resolveSave();
  void updateDate();

oc3_signals public:
  Signal0<> onExitSignal;
  Signal0<> onEndSignal;
  Signal0<> onSaveSignal;
  Signal0<> onLoadSignal;
  Signal1<int> onRequestAdvisorSignal;
};

TopMenu* TopMenu::create( Widget* parent, const int height )
{
  TopMenu* ret = new TopMenu( parent, height);
  ret->setGeometry( Rect( 0, 0, parent->getWidth(), height ) );

  PicturesArray p_marble;
  for (int i = 1; i<=12; ++i)
  {
    p_marble.push_back( Picture::load( ResourceGroup::panelBackground, i));
  }

  ret->_d->bgPicture.reset( Picture::create( ret->getSize() ) );

  int i = 0;
  unsigned int x = 0;
  while (x < ret->getWidth())
  {
    const Picture& pic = p_marble[i%10];
    ret->_d->bgPicture->draw( pic, x, 0);
    x += pic.getWidth();
    i++;
  }

  Size lbSize( 120, 23 );
  ret->_d->lbPopulation = new Label( ret, Rect( Point( ret->getWidth() - populationLabelOffset, 0 ), lbSize ), "Pop 34,124" );
  ret->_d->lbPopulation->setBackgroundPicture( Picture::load( ResourceGroup::panelBackground, panelBgStatus ) );
  ret->_d->lbPopulation->setFont( Font::create(FONT_2_WHITE) );
  ret->_d->lbPopulation->setTextAlignment( alignCenter, alignCenter );
  ret->_d->lbPopulation->setTooltipText( _("##population_tooltip##") );
  //_populationLabel.setTextPosition(20, 0);

  ret->_d->lbFunds = new Label( ret, Rect( Point( ret->getWidth() - fundLabelOffset, 0), lbSize ), "Dn 10,000" );
  ret->_d->lbFunds->setFont( Font::create( FONT_2_WHITE ));
  ret->_d->lbFunds->setTextAlignment( alignCenter, alignCenter );
  ret->_d->lbFunds->setBackgroundPicture( Picture::load( ResourceGroup::panelBackground, panelBgStatus ) );
  ret->_d->lbFunds->setTooltipText( _("##funds_tooltip##") );
  //_fundsLabel.setTextPosition(20, 0);

  ret->_d->lbDate = new Label( ret, Rect( Point( ret->getWidth() - dateLabelOffset, 0), lbSize ), "Feb 39 BC" );
  ret->_d->lbDate->setFont( Font::create( FONT_2_YELLOW ));
  ret->_d->lbDate->setTextAlignment( alignCenter, alignCenter );
  ret->_d->lbDate->setBackgroundPicture( Picture::load( ResourceGroup::panelBackground, panelBgStatus ) );
  ret->_d->lbDate->setTooltipText( _("##date_tooltip##") );
  //_dateLabel.setTextPosition(20, 0);

  ContextMenuItem* tmp = ret->addItem( _("##gmenu_file##"), -1, true, true, false, false );
  ContextMenu* file = tmp->addSubMenu();

  ContextMenuItem* newGame = file->addItem( _("##gmenu_file_new##"), -1, true, false, false, false );
  ContextMenuItem* restart = file->addItem( _("##gmenu_file_restart##"), -1, true, false, false, false );
  ContextMenuItem* load = file->addItem( _("##gmenu_file_load##"), -1, true, false, false, false );
  ContextMenuItem* save = file->addItem( _("##gmenu_file_save##"), -1, true, false, false, false );
  ContextMenuItem* mainMenu = file->addItem( _("##gmenu_file_mainmenu##"), -1, true, false, false, false );
  ContextMenuItem* exit = file->addItem( _("##gmenu_file_exit##"), -1, true, false, false, false );

  CONNECT( exit, onClicked(), &ret->_d->onExitSignal, Signal0<>::emit );
  CONNECT( save, onClicked(), &ret->_d->onSaveSignal, Signal0<>::emit );
  CONNECT( load, onClicked(), &ret->_d->onLoadSignal, Signal0<>::emit );
  CONNECT( mainMenu, onClicked(), &ret->_d->onEndSignal, Signal0<>::emit );

  tmp = ret->addItem( _("##gmenu_options##"), -1, true, true, false, false );
  tmp = ret->addItem( _("##gmenu_help##"), -1, true, true, false, false );
  tmp = ret->addItem( _("##gmenu_advisors##"), -1, true, true, false, false );
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
  CONNECT( advisersMenu, onItemAction(), &(ret->_d->onRequestAdvisorSignal), Signal1<int>::emit );

  return ret;
}

/*bool TopMenu::onEvent(const NEvent& event)
{
  return MainMenu::onEvent(event);
}*/

void TopMenu::draw( GfxEngine& engine )
{
  engine.drawPicture( *_d->bgPicture, getScreenLeft(), getScreenTop() );

  MainMenu::draw( engine );
}

void TopMenu::setPopulation( int value )
{
  _d->lbPopulation->setText( StringHelper::format( 0xff, "%.3s %d", _("##population_short##"), value ) );
}

void TopMenu::setFunds( int value )
{
  _d->lbFunds->setText( StringHelper::format( 0xff, "%.2s %d", _("##denarii_short##"), value) );
}

void TopMenu::Impl::updateDate()
{
  if( saveDate.getMonth() == GameDate::current().getMonth() )
    return;

  saveDate = GameDate::current();

  std::string month = _( StringHelper::format( 0xff, "##month_%d_short##", saveDate.getMonth() + 1).c_str() );
  std::string age = _( StringHelper::format( 0xff, "##age_%s##", saveDate.getYear() > 0 ? "ad" : "bc" ).c_str() );
  std::string text = StringHelper::format( 0xff, "%.3s %d %.2s", 
                                           month.c_str(), abs( saveDate.getYear() ), age.c_str());
  lbDate->setText( text );
}

TopMenu::TopMenu( Widget* parent, const int height ) 
: MainMenu( parent, Rect( 0, 0, parent->getWidth(), height ) ),
  _d( new Impl )
{
  _d->updateDate();
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
