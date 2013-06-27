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

#include <cstdio>

#include "oc3_granary_special_orders_window.hpp"
#include "oc3_gfx_engine.hpp"
#include "oc3_groupbox.hpp"
#include "oc3_label.hpp"
#include "oc3_texturedbutton.hpp"
#include "oc3_gettext.hpp"
#include "oc3_gui_paneling.hpp"
#include "oc3_event.hpp"
#include "oc3_granary.hpp"
#include "oc3_goodhelper.hpp"

class GranaryGoodLabel : public Label
{
public:
  GranaryGoodLabel( Widget* parent, const Rect& rect, GoodType good, GranaryPtr granary )
    : Label( parent, rect, "", false, true )
  {
    _type = good;
    _granary = granary;
    setFont( Font::create( FONT_1_WHITE ) );

    _btnChangeRule = new PushButton( this, Rect( 140, 0, 140 + 240, getHeight() ), "", -1, false, PushButton::BlackBorderUp );
    _btnChangeRule->setFont( Font::create( FONT_1_WHITE ) );
    updateBtnText();

    CONNECT( _btnChangeRule, onClicked(), this, GranaryGoodLabel::changeGranaryRule );
  }

  virtual void _updateTexture( GfxEngine& painter )
  {
    Label::_updateTexture( painter );

    Picture goodIcon = GoodHelper::getPicture( _type );
    std::string goodName = GoodHelper::getName( _type );
    
    Picture& texture = getPicture();

    texture.draw( goodIcon, 15, 0);
    texture.draw( goodIcon, 390, 0);

    Font font = getFont();    
    font.draw( texture, goodName, 55, 0 );   
  }

  void updateBtnText()
  {
    Granary::GoodRule rule = _granary->getGoodRule( _type );
    std::string ruleName[] = { _("##accept##"), _("##reject##"), _("##deliver##"), _("##none##") };
    _btnChangeRule->setText( ruleName[ rule ] );
  }

  void changeGranaryRule()
  {
    Granary::GoodRule rule = _granary->getGoodRule( _type );
    _granary->setGoodRule( _type, Granary::GoodRule( (rule+1) % (Granary::none)) );
    updateBtnText();
  }

private:
  GoodType _type;
  GranaryPtr _granary;
  PushButton* _btnChangeRule;
};

class GranarySpecialOrdersWindow::Impl
{
public:
  GranaryPtr granary;
  PictureRef bgPicture;
  GroupBox* gbOrders;
  Label* lbTitle;
  PushButton* btnExit;
  PushButton* btnHelp;
  PushButton* btnEmpty;
};

GranarySpecialOrdersWindow::GranarySpecialOrdersWindow( Widget* parent, const Point& pos, GranaryPtr granary )
  : Widget( parent, -1, Rect( pos, Size( 510, 450 ) ) ), _d( new Impl )
{
  // create the title
  _d->granary = granary;
  _d->lbTitle = new Label( this, Rect( 50, 10, getWidth()-50, 10 + 30 ), _("##granary_orders##"), true );
  _d->lbTitle->setFont( Font::create( FONT_3 ) );
  _d->lbTitle->setTextAlignment( alignCenter, alignCenter );

  _d->btnExit = new TexturedButton( this, Point( 472, getHeight() - 39 ), Size( 24 ), -1, ResourceMenu::exitInfBtnPicId );
  _d->btnExit->setTooltipText( _("##infobox_tooltip_exit##") );

  _d->btnHelp = new TexturedButton( this, Point( 14, getHeight() - 39 ), Size( 24 ), -1, ResourceMenu::helpInfBtnPicId );
  _d->btnHelp->setTooltipText( _("##infobox_tooltip_help##") );

  CONNECT( _d->btnExit, onClicked(), this, GranarySpecialOrdersWindow::deleteLater );

  _d->bgPicture.reset( Picture::create( getSize() ) );

  // draws the box and the inner black box
  GuiPaneling::instance().draw_white_frame(*_d->bgPicture, 0, 0, getWidth(), getHeight() );

  _d->gbOrders = new GroupBox( this, Rect( 17, 42, getWidth() - 17, getHeight() - 70), -1, GroupBox::blackFrame );

  Point offset( 0, 25 );
  Size wdgSize( _d->gbOrders->getWidth() - 5, 25 );
  int index=0;
  for( int goodType=G_WHEAT; goodType < G_OLIVE; goodType++ )
  {
    Granary::GoodRule rule = _d->granary->getGoodRule( (GoodType)goodType );
    if( rule != Granary::none )
    {
      new GranaryGoodLabel( _d->gbOrders, Rect( Point( 2, 2 ) + offset * index, wdgSize), (GoodType)goodType, _d->granary );
      index++;
    }
  }
}


GranarySpecialOrdersWindow::~GranarySpecialOrdersWindow()
{
  
}

void GranarySpecialOrdersWindow::draw( GfxEngine& engine )
{
  engine.drawPicture( *_d->bgPicture, getScreenLeft(), getScreenTop() );
  Widget::draw( engine );
}

bool GranarySpecialOrdersWindow::isPointInside( const Point& point ) const
{
  //resolve all screen for self using
  return getParent()->getAbsoluteRect().isPointInside( point );
}

bool GranarySpecialOrdersWindow::onEvent( const NEvent& event)
{
  switch( event.EventType )
  {
  case OC3_MOUSE_EVENT:
    if( event.MouseEvent.Event == OC3_RMOUSE_LEFT_UP )
    {
      deleteLater();
      return true;
    }
    else if( event.MouseEvent.Event == OC3_LMOUSE_LEFT_UP )
    {
      return true;
    }
    break;
  }

  return Widget::onEvent( event );
}