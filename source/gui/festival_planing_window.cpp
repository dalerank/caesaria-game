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

#include "festival_planing_window.hpp"
#include "label.hpp"
#include "texturedbutton.hpp"
#include "gfx/decorator.hpp"
#include "core/event.hpp"
#include "gui/environment.hpp"
#include "core/foreach.hpp"
#include "city/city.hpp"
#include "core/gettext.hpp"
#include "city/funds.hpp"
#include "gfx/engine.hpp"
#include "core/stringhelper.hpp"
#include "religion/pantheon.hpp"
#include "core/logger.hpp"

using namespace religion;
using namespace gfx;

namespace gui
{

class FestivalPlaningWindow::Impl
{
public:
  static const int divId=0x200;
  typedef enum { festId=0x400, smallFest=1, middleFest, greatFest } FestID;
  PictureRef background;
  Label* title;
  Label* festivalName;
  int festivalType;
  std::vector<TexturedButton*> godBtns;
  std::map<int, RomeDivinityType > divines;

  PushButton* btnHelp;
  PushButton* btnExit;
  PushButton* btnSmallFestival;
  PushButton* btnMiddleFestival;
  PushButton* btnGreatFestival;

  TexturedButton* btnYes;
  TexturedButton* btnNo;

  PlayerCityPtr city;
  RomeDivinityType currentDivinity;

public oc3_signals:
  Signal2<int, int> onFestivalAssignSignal;

public:
  void assignFestival()
  {
    onFestivalAssignSignal.emit( (int)currentDivinity, festivalType );
  }

  void addImage( Widget* parent, RomeDivinityType type, int column, int startPic )
  {
    Size imgSize( 81, 91 );
    godBtns.push_back( new TexturedButton( parent, Point( column * 100 + 60, 48),
                                           imgSize, divId + type, ResourceGroup::festivalimg,
                                           startPic, startPic, startPic+5, startPic+5 ) );
    godBtns.back()->setIsPushButton( true );
    divines[ divId + type ] = type;
  }

  void updateTitle()
  {
    RomeDivinityPtr divinity = Pantheon::get( currentDivinity );

    std::string text = StringHelper::format( 0xff, "##hold_%s_festival##", divinity.isValid()
                                                                                ? divinity->getDebugName().c_str()
                                                                                : "unknown" );
    title->setText( _(text) );
  }
};

FestivalPlaningWindow* FestivalPlaningWindow::create(Widget* parent, PlayerCityPtr city, int id )
{
  Size size( 625, 320 );

  Rect rectangle( Point( (parent->width() - size.width())/2, (parent->height() - size.height())/2 ), size );
  FestivalPlaningWindow* ret = new FestivalPlaningWindow( parent, id, rectangle, city);

  return ret;
}

FestivalPlaningWindow::~FestivalPlaningWindow()
{

}

FestivalPlaningWindow::FestivalPlaningWindow( Widget* parent, int id, const Rect& rectangle, PlayerCityPtr city)
  : Widget( parent, id, rectangle ), _d( new Impl )
{
  _d->city = city;
  _d->background.reset( Picture::create( size() ) );

  PictureDecorator::draw( *_d->background, Rect( Point( 0, 0 ), size() ), PictureDecorator::whiteFrame );

  _d->addImage( this, romeDivCeres, 0, 17 );
  _d->addImage( this, romeDivNeptune, 1, 18 );
  _d->addImage( this, romeDivMercury, 2, 19 );
  _d->addImage( this, romeDivMars, 3, 20 );
  _d->addImage( this, romeDivVenus, 4, 21 );

  _d->godBtns.front()->setPressed( true );
  _d->currentDivinity = romeDivCeres;

  _d->title = new Label( this, Rect( 16, 16, width() - 16, 16 + 30), _("##title##") );
  _d->title->setFont( Font::create( FONT_3 ) );
  _d->title->setTextAlignment( alignCenter, alignCenter );

  _d->updateTitle();

  _d->festivalName = new Label( this, Rect( 145, height() - 52, 145 + 200, height() - 22), "##small_festival##" );

  _d->festivalType = Impl::smallFest;

  _d->btnHelp = new TexturedButton( this, Point( 52, height() - 52 ), Size( 24 ), -1, ResourceMenu::helpInfBtnPicId );
  _d->btnExit = new TexturedButton( this, Point( width() - 74, height() - 52 ), Size( 24 ), -1, ResourceMenu::exitInfBtnPicId );

  /*int money = _d->city->getFunds().getValue();*/
  int greatFestivalCost = math::clamp( _d->city->getPopulation() / 10, 100, 9999 );

  _d->btnSmallFestival = new PushButton( this, Rect( 95, 170, width() - 95, 170 + 25),
                                         StringHelper::format( 0xff, "%s %d", _("##small_festival##"), greatFestivalCost / 4 ),
                                         Impl::festId+Impl::smallFest, false, PushButton::whiteBorderUp );
  _d->btnSmallFestival->setTextAlignment( alignUpperLeft, alignCenter );
  //_d->btnSmallFestival->setIsPushButton( true );

  _d->btnMiddleFestival = new PushButton( this, _d->btnSmallFestival->getRelativeRect() + Point( 0, 30 ),
                                          StringHelper::format( 0xff, "%s %d", _("##middle_festival##"), greatFestivalCost / 2 ),
                                          Impl::festId+Impl::middleFest, false, PushButton::whiteBorderUp );
  _d->btnMiddleFestival->setTextAlignment( alignUpperLeft, alignCenter );
  //_d->btnMiddleFestival->setIsPushButton( true );

  _d->btnGreatFestival = new PushButton( this, _d->btnMiddleFestival->getRelativeRect() + Point( 0, 30 ),
                                         StringHelper::format( 0xff, "%s %d", _("##great_festival##"), greatFestivalCost  ),
                                         Impl::festId+Impl::greatFest, false, PushButton::whiteBorderUp );
  _d->btnGreatFestival->setTextAlignment( alignUpperLeft, alignCenter );
  //_d->btnGreatFestival->setIsPushButton( true );

  _d->btnYes = new TexturedButton( this, Point( 350, height() - 50 ), Size( 39, 26), -1, ResourceMenu::okBtnPicId );
  _d->btnNo = new TexturedButton( this, Point( 350 + 43, height() - 50 ), Size( 39, 26), -1, ResourceMenu::cancelBtnPicId );

  CONNECT( _d->btnExit,onClicked(), this, FestivalPlaningWindow::deleteLater );
  CONNECT( _d->btnNo,  onClicked(), this, FestivalPlaningWindow::deleteLater );
  CONNECT( _d->btnYes, onClicked(), _d.data(), Impl::assignFestival );
  CONNECT( _d->btnYes, onClicked(), this, FestivalPlaningWindow::deleteLater );
}

void FestivalPlaningWindow::draw( gfx::Engine& painter )
{
  if( !isVisible() )
    return;

  if( _d->background )
  {
    painter.drawPicture( *_d->background, screenLeft(), screenTop() );
  }

  Widget::draw( painter );
}

bool FestivalPlaningWindow::onEvent(const NEvent& event)
{
  if( event.EventType == sEventGui && event.gui.type == guiButtonClicked )
  {
    PushButton* btn = safety_cast< PushButton* >( event.gui.caller );
    if( btn && (btn->getID() & 0x0200) )
    {
      foreach ( abtn, _d->godBtns )  { (*abtn)->setPressed( false ); }

      btn->setPressed( true );
      _d->currentDivinity = _d->divines[ btn->getID() ];

      _d->updateTitle();
    }
    else if( btn && (btn->getID() & 0x0400) )
    {
      const char* titles[] = { "", _("##small_festival##"), _("##middle_festival##"),  _("##great_festival##") };
      _d->festivalType = btn->getID() & 0xf;
      _d->festivalName->setText( titles[ _d->festivalType ] );      
    }

    return true;
  }

  return Widget::onEvent( event );
}

Signal2<int,int>& FestivalPlaningWindow::onFestivalAssign()
{
  return _d->onFestivalAssignSignal;
}

}//end namespace gui
