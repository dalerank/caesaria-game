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

#include "mission_target_window.hpp"
#include "game/resourcegroup.hpp"
#include "gfx/decorator.hpp"
#include "gui/label.hpp"
#include "listbox.hpp"
#include "texturedbutton.hpp"
#include "core/gettext.hpp"
#include "gfx/engine.hpp"
#include "groupbox.hpp"
#include "environment.hpp"
#include "player/city.hpp"
#include "core/stringhelper.hpp"
#include "core/logger.hpp"

namespace gui
{

class MissionTargetsWindow::Impl
{
public:
  PlayerCityPtr city;
  PictureRef background;
  Label* title;
  Label* subTitle;
  Label* lbPopulation;
  Label* lbProsperity;
  Label* lbFavour;
  Label* lbCulture;
  Label* lbPeace;

  ListBox* lbxHelp;
};

MissionTargetsWindow* MissionTargetsWindow::create(Widget* parent, PlayerCityPtr city, int id )
{
  Size size( 610, 430 );

  Rect rectangle( Point( (parent->getWidth() - size.getWidth())/2, (parent->getHeight() - size.getHeight())/2 ), size );
  MissionTargetsWindow* ret = new MissionTargetsWindow( parent, id, rectangle );
  ret->_d->city = city;
  return ret;
}

MissionTargetsWindow::~MissionTargetsWindow()
{

}

MissionTargetsWindow::MissionTargetsWindow( Widget* parent, int id, const Rect& rectangle ) 
  : Widget( parent, id, rectangle ), _d( new Impl )
{
  _d->background.reset( Picture::create( getSize() ) );

  PictureDecorator::draw( *_d->background, Rect( Point( 0, 0 ), getSize() ), PictureDecorator::whiteFrame );

  TexturedButton* btnExit = new TexturedButton( this, Point( getWidth() - 110, getHeight() - 40), Size( 27 ), -1, 179 );
  CONNECT( btnExit, onClicked(), this, MissionTargetsWindow::deleteLater );

  _d->title = new Label( this, Rect( 16, 16, getWidth() - 16, 16 + 30), _("##title##") );
  _d->subTitle = new Label( this, Rect( 16, _d->title->getBottom(), getWidth() - 16, _d->title->getBottom() + 20), "##sub_title##" );

  GroupBox* gbTargets = new GroupBox( this, Rect( 16, 64, getWidth() - 64, 64 + 80), Widget::noId, GroupBox::blackFrame );
  Label* lbTtargets = new Label( gbTargets, Rect( 15, 0, 490, 28), _("##mission_wnd_targets_title##") );
  lbTtargets->setFont( Font::create( FONT_1_WHITE ) );
  lbTtargets->setTextAlignment( alignUpperLeft, alignUpperLeft );

  _d->lbPopulation = new Label( gbTargets, Rect( 16, 32, 16 + 240, 32 + 20), _("##mission_wnd_population##"), false, Label::bgSmBrown );
  _d->lbProsperity = new Label( gbTargets, Rect( 16, 54, 16 + 240, 54 + 20), _("##mission_wnd_prosperity##"), false, Label::bgSmBrown );

  _d->lbFavour = new Label( gbTargets, Rect( 270, 10, 270 + 240, 10 + 20), _("##mission_wnd_favour##"), false, Label::bgSmBrown );
  _d->lbCulture = new Label( gbTargets, Rect( 270, 32, 270 + 240, 32 + 20), _("##mission_wnd_culture##"), false, Label::bgSmBrown );
  _d->lbPeace = new Label( gbTargets, Rect( 270, 54, 270 + 240, 54 + 20), _("##mission_wnd_peace##"), false, Label::bgSmBrown );

  _d->lbxHelp = new ListBox( this, Rect( 16, 152, getWidth() - 20, getHeight() - 40 ) );
}

void MissionTargetsWindow::draw( GfxEngine& painter )
{
  if( !isVisible() )
    return;

  if( _d->background )
  {
    painter.drawPicture( *_d->background, getScreenLeft(), getScreenTop() );
  }

  Widget::draw( painter );
}

}//end namespace gui
