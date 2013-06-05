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
#include "oc3_label.hpp"
#include "oc3_pic_loader.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_contextmenuitem.hpp"
#include "oc3_stringhelper.hpp"

namespace {
static const int dateLabelOffset = 155;
static const int populationLabelOffset = 344;
static const int fundLabelOffset = 464;
static const int panelBgStatus = 15;
};

class TopMenu::Impl
{
public:
  Label* lbPopulation;
  Label* lbFunds;
  Label* lbDate;
  Picture* bgPicture;

  void resolveSave();

oc3_signals public:
  Signal0<> onExitSignal;
  Signal0<> onEndSignal;
  Signal0<> onSaveSignal;
};

TopMenu* TopMenu::create( Widget* parent, const int height )
{
  TopMenu* ret = new TopMenu( parent, height);
  ret->setGeometry( Rect( 0, 0, parent->getWidth(), height ) );

  GfxEngine& engine = GfxEngine::instance();

  std::vector<Picture> p_marble;
  for (int i = 1; i<=12; ++i)
  {
    p_marble.push_back( Picture::load( ResourceGroup::panelBackground, i));
  }

  ret->_d->bgPicture = &engine.createPicture( ret->getWidth(), ret->getHeight() );

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
  ret->_d->lbPopulation = new Label( ret, Rect( Point( ret->getWidth() - populationLabelOffset, 0 ), lbSize ),
                                     "Pop 34,124", false, true, -1 );
  ret->_d->lbPopulation->setBackgroundPicture( Picture::load( ResourceGroup::panelBackground, panelBgStatus ) );
  ret->_d->lbPopulation->setFont( Font(FONT_2_WHITE) );
  ret->_d->lbPopulation->setTextAlignment( alignCenter, alignCenter );
  ret->_d->lbPopulation->setTooltipText( _("##population_tooltip##") );
  //_populationLabel.setTextPosition(20, 0);

  ret->_d->lbFunds = new Label( ret, Rect( Point( ret->getWidth() - fundLabelOffset, 0), lbSize ),
      "Dn 10,000", false, true, -1 );
  ret->_d->lbFunds->setFont( Font( FONT_2_WHITE ));
  ret->_d->lbFunds->setTextAlignment( alignCenter, alignCenter );
  ret->_d->lbFunds->setBackgroundPicture( Picture::load( ResourceGroup::panelBackground, panelBgStatus ) );
  ret->_d->lbFunds->setTooltipText( _("##funds_tooltip##") );
  //_fundsLabel.setTextPosition(20, 0);

  ret->_d->lbDate = new Label( ret, Rect( Point( ret->getWidth() - dateLabelOffset, 0), lbSize ),
      "Feb 39 BC", false, true, -1 );
  ret->_d->lbDate->setFont( Font( FONT_2_YELLOW ));
  ret->_d->lbDate->setTextAlignment( alignCenter, alignCenter );
  ret->_d->lbDate->setBackgroundPicture( Picture::load( ResourceGroup::panelBackground, panelBgStatus ) );
  ret->_d->lbDate->setTooltipText( _("##date_tooltip##") );
  //_dateLabel.setTextPosition(20, 0);

  GfxEngine::instance().loadPicture(*ret->_d->bgPicture);

  ContextMenuItem* tmp = ret->addItem( _("##gmenu_file##"), -1, true, true, false, false );
  tmp->setBackgroundPicture( *ret->_d->bgPicture );
  ContextMenu* file = tmp->addSubMenu();

  ContextMenuItem* save = file->addItem( _("##gmenu_file_save##"), -1, true, false, false, false );
  //ContextMenuItem* load = file->addItem( "Load", -1, true, false, false, false );

  file->addItem( "", -1, false, false, false, false );
  ContextMenuItem* mainMenu = file->addItem( _("##gmenu_file_mainmenu##"), -1, true, false, false, false );

  file->addItem( "", -1, true, false, false, false );
  ContextMenuItem* exit = file->addItem( _("##gmenu_file_exit##"), -1, true, false, false, false );

  CONNECT( exit, onClicked(), &ret->_d->onExitSignal, Signal0<>::emit );
  CONNECT( save, onClicked(), &ret->_d->onSaveSignal, Signal0<>::emit );
  CONNECT( mainMenu, onClicked(), &ret->_d->onEndSignal, Signal0<>::emit );

  tmp = ret->addItem( _("##gmenu_options##"), -1, true, true, false, false );
  tmp->setBackgroundPicture( *ret->_d->bgPicture, Point( -tmp->getLeft(), 0 ) );
  tmp = ret->addItem( _("##gmenu_help##"), -1, true, true, false, false );
  tmp->setBackgroundPicture( *ret->_d->bgPicture, Point( -tmp->getLeft(), 0 ) );
  tmp = ret->addItem( _("##gmenu_advisors##"), -1, true, true, false, false );
  tmp->setBackgroundPicture( *ret->_d->bgPicture, Point( -tmp->getLeft(), 0 ) );
  ContextMenu* advisersMenu = tmp->addSubMenu();
  advisersMenu->addItem( _("##adv_employments_m##"), -1 );
  advisersMenu->addItem( _("##adv_military_m##"), -1 );
  advisersMenu->addItem( _("##adv_empire_m##"), -1 );
  advisersMenu->addItem( _("##adv_ratings_m##"), -1 );
  advisersMenu->addItem( _("##adv_trade_m##"), -1 );
  advisersMenu->addItem( _("##adv_population_m##"), -1 );
  advisersMenu->addItem( _("##adv_health_m##"), -1 );
  advisersMenu->addItem( _("##adv_education_m##"), -1 );
  advisersMenu->addItem( _("##adv_religion_m##"), -1 );
  advisersMenu->addItem( _("##adv_entertainment_m##"), -1 );
  advisersMenu->addItem( _("##adv_finance_m##"), -1 );
  advisersMenu->addItem( _("##adv_main_m##"), -1 );

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

void TopMenu::setDate( int value )
{
  std::string month = _( StringHelper::format( 0xff, "##month_%d_short##", (value % 12) + 1).c_str() );
  std::string age = _( StringHelper::format( 0xff, "##age_%s##", ( ((int)value/12-39) > 0 ? "ad" : "bc" ) ).c_str() );
  std::string text = StringHelper::format( 0xff, "%.3s %d %.2s", 
                                           month.c_str(), (int)std::abs(((int)value/12-39)), age.c_str());

  //_dateLabel.setText("Feb 39 BC");
  _d->lbDate->setText( text );
}

TopMenu::TopMenu( Widget* parent, const int height ) 
: MainMenu( parent, Rect( 0, 0, parent->getWidth(), height ) ),
  _d( new Impl )
{
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
