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

#include "oc3_advisor_finance_window.hpp"
#include "oc3_picture_decorator.hpp"
#include "oc3_gettext.hpp"
#include "oc3_pushbutton.hpp"
#include "oc3_gui_label.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_stringhelper.hpp"
#include "oc3_gfx_engine.hpp"
#include "oc3_gettext.hpp"
#include "oc3_enums.hpp"
#include "oc3_city.hpp"
#include "oc3_house.hpp"
#include "oc3_color.hpp"
#include "oc3_texturedbutton.hpp"
#include "oc3_cityfunds.hpp"

class AdvisorFinanceWindow::Impl
{
public:
  CityPtr city;
  PictureRef background;

  Label* lbTaxRateNow;
  TexturedButton* btnHelp;

  void drawReportRow( const Point& pos, const std::string& title, CityFunds::IssueType type )
  {
    Font font = Font::create( FONT_1 );

    int lyvalue = city->getFunds().getIssueValue( type, CityFunds::lastYear );
    int tyvalue = city->getFunds().getIssueValue( type, CityFunds::thisYear );

    font.draw( *background, title, pos, false );
    font.draw( *background, StringHelper::format( 0xff, "%d", lyvalue ), pos + Point( 215, 0), false );
    font.draw( *background, StringHelper::format( 0xff, "%d", tyvalue ), pos + Point( 355, 0), false );
  }

  void updateTaxRateNowLabel()
  {
    std::string strCurretnTax = StringHelper::format( 0xff, "%d%% %s %d %s", city->getTaxRate(), _("##may_collect_about##"), 0, _("##denaries##") );
    lbTaxRateNow->setText( strCurretnTax );
  }
};

AdvisorFinanceWindow::AdvisorFinanceWindow( CityPtr city, Widget* parent, int id ) 
: Widget( parent, id, Rect( 0, 0, 1, 1 ) ), _d( new Impl )
{
  _d->city = city;
  setGeometry( Rect( Point( (parent->getWidth() - 640 )/2, parent->getHeight() / 2 - 242 ),
               Size( 640, 420 ) ) );

  Label* title = new Label( this, Rect( 60, 10, 60 + 210, 10 + 40) );
  title->setText( _("##Finance advisor##") );
  title->setFont( Font::create( FONT_3 ) );
  title->setTextAlignment( alignUpperLeft, alignCenter );

  _d->background.reset( Picture::create( getSize() ) );

  //main _d->_d->background
  PictureDecorator::draw( *_d->background, Rect( Point( 0, 0 ), getSize() ), PictureDecorator::whiteFrame );
  //buttons _d->_d->background
  PictureDecorator::draw( *_d->background, Rect( Point( 70, 50 ), Size( getWidth() - 86, 70 ) ), PictureDecorator::blackFrame);

  Picture& icon = Picture::load( ResourceGroup::panelBackground, 265 );
  _d->background->draw( icon, Point( 11, 11 ) );

  Font fontWhite = Font::create( FONT_1_WHITE );

  std::string moneyStr = StringHelper::format( 0xff, "%s %d %s", _("##city_have##"), city->getFunds().getValue(), _("##denaries##") );
  fontWhite.draw( *_d->background, moneyStr, 70, 55, false );
  fontWhite.draw( *_d->background, _("##tax_rate##"), 65, 75, false );

  _d->lbTaxRateNow = new Label( this, Rect( 245, 75, 245 + 350, 75 + 20 ), "" );
  _d->lbTaxRateNow->setFont( fontWhite );
  _d->updateTaxRateNowLabel();

  std::string strRegPaeyrs = StringHelper::format( 0xff, "%d%% %s", 0, _("##population_registered_as_taxpayers##") );
  fontWhite.draw( *_d->background, strRegPaeyrs, 70, 95, false );

  Font font = Font::create( FONT_1 );
  font.draw( *_d->background, _("##Last year##"), 265, 130, false );
  font.draw( *_d->background, _("##This year##"), 400, 130, false );

  Point startPoint( 75, 145 );
  Point offset( 0, 17 );

  _d->drawReportRow( startPoint, _("##Taxes##"), CityFunds::taxIncome );
  _d->drawReportRow( startPoint + offset, _("##Trade##"), CityFunds::exportGoods );
  _d->drawReportRow( startPoint + offset * 2, _("##Donations##"), CityFunds::donation );
  _d->drawReportRow( startPoint + offset * 3, _("##Debet##"), CityFunds::debet );
  _d->background->fill( 0xff000000, Rect( startPoint + offset * 3 + Point( 200, 0 ), Size( 72, 1) ) );
  _d->background->fill( 0xff000000, Rect( startPoint + offset * 3 + Point( 340, 0 ), Size( 72, 1) ) );
  
  startPoint += Point( 0, 6 );
  _d->drawReportRow( startPoint + offset * 4, _("##Import##"), CityFunds::importGoods );
  _d->drawReportRow( startPoint + offset * 5, _("##Wages##"), CityFunds::workersWages );
  _d->drawReportRow( startPoint + offset * 6, _("##Buildings##"), CityFunds::buildConstruction );
  _d->drawReportRow( startPoint + offset * 7, _("##Percents##"), CityFunds::creditPercents );
  _d->drawReportRow( startPoint + offset * 8, _("##Salary##"), CityFunds::playerSalary );
   
  _d->drawReportRow( startPoint + offset * 9, _("##Other##"), CityFunds::otherExpenditure );
  _d->drawReportRow( startPoint + offset * 10, _("##Empire tax##"), CityFunds::empireTax );
  _d->background->fill( 0xff000000, Rect( startPoint + offset * 10 + Point( 200, 0 ), Size( 72, 1) ) );
  _d->background->fill( 0xff000000, Rect( startPoint + offset * 10 + Point( 340, 0 ), Size( 72, 1) ) );

  _d->drawReportRow( startPoint + offset * 11, _("##Credit##"), CityFunds::credit );

  startPoint += Point( 0, 6 );
  _d->drawReportRow( startPoint + offset * 12, _("##Profit##"), CityFunds::profit );
  
  startPoint += Point( 0, 6 );
  _d->drawReportRow( startPoint + offset * 13, _("##Balance##"), CityFunds::balance );

  _d->btnHelp = new TexturedButton( this, Point( 12, getHeight() - 39), Size( 24 ), -1, ResourceMenu::helpInfBtnPicId );

  new TexturedButton( this, Point( 185, 70 ), Size( 24 ), -1, 601 );
  new TexturedButton( this, Point( 185+24, 70 ), Size( 24 ), -1, 605 );
}

void AdvisorFinanceWindow::draw( GfxEngine& painter )
{
  if( !isVisible() )
    return;

  painter.drawPicture( *_d->background, getScreenLeft(), getScreenTop() );

  Widget::draw( painter );
}
