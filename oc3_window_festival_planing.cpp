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

#include "oc3_window_festival_planing.hpp"
#include "gui/label.hpp"
#include "oc3_texturedbutton.hpp"
#include "oc3_picture_decorator.hpp"
#include "oc3_event.hpp"
#include "oc3_guienv.hpp"
#include "core/foreach.hpp"
#include "oc3_city.hpp"
#include "oc3_gettext.hpp"
#include "oc3_cityfunds.hpp"
#include "oc3_gfx_engine.hpp"
#include "oc3_stringhelper.hpp"

class FestivalPlaningWindow::Impl
{
public:
  PictureRef background;
  Label* title;
  Label* festivalName;
  int festivalType;
  std::vector< TexturedButton* > godBtns;

  PushButton* btnHelp;
  PushButton* btnExit;
  PushButton* btnSmallFestival;
  PushButton* btnMiddleFestival;
  PushButton* btnGreatFestival;

  TexturedButton* btnYes;
  TexturedButton* btnNo;

  CityPtr city;

public oc3_signals:
  Signal0<> onFestivalAssignSignal;

public:
  void assignFestival()
  {
    onFestivalAssignSignal.emit();
  }
};

FestivalPlaningWindow* FestivalPlaningWindow::create( Widget* parent, CityPtr city, int id )
{
  Size size( 625, 320 );

  Rect rectangle( Point( (parent->getWidth() - size.getWidth())/2, (parent->getHeight() - size.getHeight())/2 ), size );
  FestivalPlaningWindow* ret = new FestivalPlaningWindow( parent, id, rectangle, city);

  return ret;
}

FestivalPlaningWindow::~FestivalPlaningWindow()
{

}

FestivalPlaningWindow::FestivalPlaningWindow( Widget* parent, int id, const Rect& rectangle, CityPtr city)
  : Widget( parent, id, rectangle ), _d( new Impl )
{
  _d->city = city;
  _d->background.reset( Picture::create( getSize() ) );

  PictureDecorator::draw( *_d->background, Rect( Point( 0, 0 ), getSize() ), PictureDecorator::whiteFrame );

  Size imgSize( 81, 91 );
  _d->godBtns.push_back( new TexturedButton( this, Point( 60, 48), imgSize, 0x0217, ResourceGroup::festivalimg, 17, 17, 22, 22 ) );
  _d->godBtns.push_back( new TexturedButton( this, Point( 160, 48), imgSize, 0x0218, ResourceGroup::festivalimg, 18, 18, 23, 23 ) );
  _d->godBtns.push_back( new TexturedButton( this, Point( 260, 48), imgSize, 0x0219, ResourceGroup::festivalimg, 19, 19, 24, 24 ) );
  _d->godBtns.push_back( new TexturedButton( this, Point( 360, 48), imgSize, 0x0220, ResourceGroup::festivalimg, 20, 20, 25, 25 ) );
  _d->godBtns.push_back( new TexturedButton( this, Point( 460, 48), imgSize, 0x0221, ResourceGroup::festivalimg, 21, 21, 26, 26 ) );

  foreach ( TexturedButton* btn, _d->godBtns )
    btn->setIsPushButton( true );

  _d->godBtns.front()->setPressed( true );

  _d->title = new Label( this, Rect( 16, 16, getWidth() - 16, 16 + 30), _("##title##") );
  _d->festivalName = new Label( this, Rect( 145, getHeight() - 52, 145 + 200, getHeight() - 22) );

  _d->btnHelp = new TexturedButton( this, Point( 52, getHeight() - 52 ), Size( 24 ), -1, ResourceMenu::helpInfBtnPicId );
  _d->btnExit = new TexturedButton( this, Point( getWidth() - 74, getHeight() - 52 ), Size( 24 ), -1, ResourceMenu::exitInfBtnPicId );

  /*int money = _d->city->getFunds().getValue();*/
  int greatFestivalCost = math::clamp( _d->city->getPopulation() / 10, 100, 9999 );

  _d->btnSmallFestival = new PushButton( this, Rect( 95, 170, getWidth() - 95, 170 + 25),
                                         StringHelper::format( 0xff, "%s %d", _("##small_festival##"), greatFestivalCost / 4 ),
                                         0x0401, false, PushButton::whiteBorderUp );
  _d->btnSmallFestival->setTextAlignment( alignUpperLeft, alignCenter );

  _d->btnMiddleFestival = new PushButton( this, _d->btnSmallFestival->getRelativeRect() + Point( 0, 30 ),
                                          StringHelper::format( 0xff, "%s %d", _("##middle_festival##"), greatFestivalCost / 2 ),
                                          0x0402, false, PushButton::whiteBorderUp );
  _d->btnMiddleFestival->setTextAlignment( alignUpperLeft, alignCenter );

  _d->btnGreatFestival = new PushButton( this, _d->btnMiddleFestival->getRelativeRect() + Point( 0, 30 ),
                                         StringHelper::format( 0xff, "%s %d", _("##great_festival##"), greatFestivalCost  ),
                                         0x0403, false, PushButton::whiteBorderUp );
  _d->btnGreatFestival->setTextAlignment( alignUpperLeft, alignCenter );

  _d->btnYes = new TexturedButton( this, Point( 350, getHeight() - 50 ), Size( 39, 26), -1, ResourceMenu::okBtnPicId );
  _d->btnNo = new TexturedButton( this, Point( 350 + 43, getHeight() - 50 ), Size( 39, 26), -1, ResourceMenu::cancelBtnPicId );

  CONNECT( _d->btnExit,onClicked(), this, FestivalPlaningWindow::deleteLater );
  CONNECT( _d->btnNo,  onClicked(), this, FestivalPlaningWindow::deleteLater );
  CONNECT( _d->btnYes, onClicked(), _d.data(), Impl::assignFestival );
  CONNECT( _d->btnYes, onClicked(), this, FestivalPlaningWindow::deleteLater );
}

void FestivalPlaningWindow::draw( GfxEngine& painter )
{
  if( !isVisible() )
    return;

  if( _d->background )
  {
    painter.drawPicture( *_d->background, getScreenLeft(), getScreenTop() );
  }

  Widget::draw( painter );
}

bool FestivalPlaningWindow::onEvent(const NEvent& event)
{
  if( event.EventType == OC3_GUI_EVENT && event.GuiEvent.EventType == OC3_BUTTON_CLICKED )
  {
    PushButton* btn = safety_cast< PushButton* >( event.GuiEvent.Caller );

    if( btn && (btn->getID() & 0x0200) )
    {
      foreach ( TexturedButton* abtn, _d->godBtns )
      {
        abtn->setPressed( false );
      }

      btn->setPressed( true );

      return true;
    }

    if( btn && (btn->getID() & 0x0400) )
    {
      const char* titles[] = { "", _("##small_festival##"), _("##middle_festival##"),  _("##great_festival##") };
      _d->festivalType = btn->getID() & 0xf;
      _d->festivalName->setText( titles[ _d->festivalType ] );
    }
  }

  return Widget::onEvent( event );
}

Signal0<>&FestivalPlaningWindow::onFestivalAssign()
{
  return _d->onFestivalAssignSignal;
}
