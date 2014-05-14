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

#include "advisor_legion_window.hpp"
#include "gfx/decorator.hpp"
#include "core/gettext.hpp"
#include "gui/pushbutton.hpp"
#include "gui/label.hpp"
#include "game/resourcegroup.hpp"
#include "core/stringhelper.hpp"
#include "gfx/engine.hpp"
#include "texturedbutton.hpp"
#include "objects/military.hpp"
#include "walker/soldier.hpp"
#include "core/logger.hpp"

using namespace gfx;

namespace gui
{

namespace {
  Point legionButtonOffset = Point( 0, 50 );
  Size legionButtonSize = Size( 565, 42 );
}

class LegionButton : public PushButton
{
public:
  LegionButton( Widget* parent, const Point& pos, int index, FortPtr fort )
    : PushButton( parent, Rect( pos + legionButtonOffset * index, legionButtonSize), "", -1, false, PushButton::blackBorderUp )
  {
    _fort = fort;
    _resizeEvent();

    PushButton* gotoLegion    = new PushButton( this, Rect( 360, 5, 360 + 32, 6 + 32 ), "", -1, false, PushButton::blackBorderUp );
    gotoLegion->setIcon( ResourceGroup::panelBackground, 563 );
    gotoLegion->setIconOffset( Point( 4, 4 ) );
    PushButton* return2fort   = new PushButton( this, Rect( 450, 5, 450 + 32, 6 +32 ), "", -1, false, PushButton::blackBorderUp );
    return2fort->setIcon(  ResourceGroup::panelBackground, 564 );
    return2fort->setIconOffset( Point( 4, 4 ) );
    PushButton* empireService = new PushButton( this, Rect( 530, 5, 530 + 32, 6 + 32), "", -1, false, PushButton::blackBorderUp );
    empireService->setIcon( ResourceGroup::panelBackground, 566 );
    empireService->setIconOffset( Point( 4, 4 ) );

    CONNECT( gotoLegion, onClicked(), this, LegionButton::_resolveMove2Legion );
    CONNECT( return2fort, onClicked(), this, LegionButton::_resolveReturnLegion2Fort );
    CONNECT( empireService, onClicked(), this, LegionButton::_resolveEmpireService );
  }

  virtual void _updateTexture( ElementState state )
  {
    PushButton::_updateTexture( state );

    PictureRef& pic = _textPictureRef( state );

    Font fontW = Font::create( FONT_1_WHITE );
    Font fontB = Font::create( FONT_1 );

    if( _fort.isValid() )
    {
      pic->draw( _fort->legionEmblem(), Point( 6, 4 ), false );

      fontW.draw( *pic, _fort->legionName(), 70, 4 );

      std::string qtyStr = StringHelper::format( 0xff, "%d %s", _fort->soldiers().size(), _("##soldiers##") );
      fontB.draw( *pic, qtyStr, 70, 22 );

      int moraleValue = _fort->legionMorale() / 10;
      std::string moraleStr = StringHelper::format( 0xff, "##legion_morale_%d##", moraleValue );
      fontB.draw( *pic, _( moraleStr ), 180, 15 );
    }
  }

public oc3_signals:
  Signal1<FortPtr> onShowLegionSignal;
  Signal1<FortPtr> onLegionRetreatSignal;
  Signal1<FortPtr> onEmpireServiceSignal;

private oc3_slots:
  void _resolveMove2Legion() { onShowLegionSignal.emit( _fort ); }
  void _resolveReturnLegion2Fort() { onLegionRetreatSignal.emit( _fort ); }
  void _resolveEmpireService() { onEmpireServiceSignal.emit( _fort ); }

private:
  FortPtr _fort;
};

class AdvisorLegionWindow::Impl
{
public:
  PictureRef background;
  gui::Label* alarm;
  gui::Label* helpRequest;
};

AdvisorLegionWindow::AdvisorLegionWindow( Widget* parent, int id, FortList forts )
: Widget( parent, id, Rect( 0, 0, 1, 1 ) ), _d( new Impl )
{
  setGeometry( Rect( Point( (parent->width() - 640 )/2, parent->height() / 2 - 242 ),
                     Size( 640, 416 ) ) );

  _d->background.reset( Picture::create( size() ) );
  //main background
  PictureDecorator::draw( *_d->background, Rect( Point( 0, 0 ), size() ), PictureDecorator::whiteFrame );

  //buttons background
  PictureDecorator::draw( *_d->background, Rect( Point( 32, 70 ), Size( 574, 270 )), PictureDecorator::blackFrame );

  gui::Label* title = new gui::Label( this, Rect( 10, 10, width() - 10, 10 + 40) );
  title->setText( _("##advlegion_window_title##") );
  title->setFont( Font::create( FONT_3 ) );
  title->setTextAlignment( align::center, align::center );

  _d->alarm = new gui::Label( this, Rect( 60, height()-60, width() - 60, height() - 40 ), _("##advlegion_noalarm##") );
  _d->helpRequest = new gui::Label( this, Rect( 60, height()-40, width() - 60, height() - 20 ), _("##advlegion_norequest##") );

  new gui::Label( this, Rect( 290, 60, 340, 80 ), _("##advlegion_morale##") );
  new gui::Label( this, Rect( 380, 50, 435, 80 ), _("##advlegion_gotolegion##") );
  new gui::Label( this, Rect( 465, 50, 520, 80 ), _("##advlegion_return2fort##") );
  new gui::Label( this, Rect( 550, 50, 600, 80 ), _("##advlegion_empireservice##") );

  int index=0;
  foreach( it, forts )
  {
    LegionButton* btn = new LegionButton( this, legionButtonOffset, index++, *it );
  }
}

void AdvisorLegionWindow::draw( Engine& painter )
{
  if( !isVisible() )
    return;

  painter.draw( *_d->background, screenLeft(), screenTop() );

  Widget::draw( painter );
}

}//end namespace gui
