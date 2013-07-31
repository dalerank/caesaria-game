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

#include "oc3_window_mission_target.hpp"
#include "oc3_scenario.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_picture_decorator.hpp"
#include "oc3_gui_label.hpp"
#include "oc3_listbox.hpp"
#include "oc3_picture_decorator.hpp"
#include "oc3_texturedbutton.hpp"
#include "oc3_gettext.hpp"
#include "oc3_gfx_engine.hpp"
#include "oc3_groupbox.hpp"

class MissionTargetsWindow::Impl
{
public:
  Scenario* scenario;
  PictureRef background;
  Label* title;
  Label* subTitle;
  Label* lbPopulation;
  Label* lbDescription;

  ListBox* lbxHelp;
};

MissionTargetsWindow* MissionTargetsWindow::create( Widget* parent, int id, Scenario* scenario )
{
  Size size( 610, 430 );
  Rect rectangle( Point( (parent->getWidth() - size.getWidth())/2, (parent->getHeight() - size.getHeight())/2 ), size );
  MissionTargetsWindow* ret = new MissionTargetsWindow( parent, id, rectangle );
  ret->_d->scenario = scenario;
  return ret;
}

MissionTargetsWindow::~MissionTargetsWindow()
{

}

MissionTargetsWindow::MissionTargetsWindow( Widget* parent, int id, const Rect& rectangle ) 
  : Widget( parent, id, rectangle ), _d( new Impl )
{
  _d->background.reset( Picture::create( getSize() ) );

  PictureDecorator::instance().draw_white_frame( *_d->background, 0, 0, getWidth(), getHeight() );

  TexturedButton* btnExit = new TexturedButton( this, Point( getWidth() - 110, getHeight() - 40), Size( 27 ), -1, 179 );
  CONNECT( btnExit, onClicked(), this, MissionTargetsWindow::deleteLater );

  _d->title = new Label( this, Rect( 16, 16, getWidth() - 16, 16 + 30), _("##title##") );
  _d->subTitle = new Label( this, Rect( 16, _d->title->getBottom(), getWidth() - 16, _d->title->getBottom() + 20), "##sub_title##" );

  GroupBox* gbTargets = new GroupBox( this, Rect( 16, 64, getWidth() - 64, 64 + 80), Widget::noId, GroupBox::blackFrame );
  Label* lbTtargets = new Label( gbTargets, Rect( 15, 0, 490, 28), _("##mission_wnd_targets_title##") );
  lbTtargets->setFont( Font::create( FONT_1_WHITE ) );

  _d->lbPopulation = new Label( gbTargets, Rect( 16, 32, getWidth() / 2, 32 + 20), _("##mission_wnd_population##"), false, Label::bgBrown );
  _d->lbDescription = new Label( gbTargets, Rect( 16, 52, getWidth() - 16, 52 + 20), _("##mission_wnd_population##"), false, Label::bgBrown );

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