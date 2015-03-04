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

#include <cstdio>

#include "package_options_window.hpp"
#include "core/utils.hpp"
#include "widget_helper.hpp"
#include "texturedbutton.hpp"
#include "game/settings.hpp"
#include "editbox.hpp"
#include "widgetescapecloser.hpp"
#include "core/logger.hpp"

using namespace constants;

namespace gui
{

namespace dialog
{

class PackageOptions::Impl
{
public:
  EditBox* edResourcesPath;
  EditBox* edCaesar3Music;
  EditBox* edCaesar3Video;
  EditBox* edCaesar3Path;
  TexturedButton* btnApply;
};

PackageOptions::PackageOptions( Widget* parent, const Rect& rectangle )
  : Window( parent, rectangle, "" ), _d( new Impl )
{
  setupUI( ":/gui/packageopts.gui" );
  setCenter( parent->center() );

  GET_DWIDGET_FROM_UI(_d,edResourcesPath)
  GET_DWIDGET_FROM_UI(_d,btnApply)
  GET_DWIDGET_FROM_UI(_d,edCaesar3Path)
  GET_DWIDGET_FROM_UI(_d,edCaesar3Video)
  GET_DWIDGET_FROM_UI(_d,edCaesar3Music)

  CONNECT( _d->btnApply, onClicked(), this, PackageOptions::deleteLater );
  CONNECT( _d->btnApply, onClicked(), this, PackageOptions::_saveOptions );
  CONNECT( _d->edResourcesPath, onTextChanged(), this, PackageOptions::_setResourcesPath );
  CONNECT( _d->edCaesar3Path, onTextChanged(), this, PackageOptions::_setCaesar3Path );

  WidgetEscapeCloser::insertTo( this );

  _update();

  if( _d->btnApply ) _d->btnApply->setFocus();
}

PackageOptions::~PackageOptions() {}
void PackageOptions::_saveOptions() {  game::Settings::instance().save(); }
void PackageOptions::_setResourcesPath(std::string path) {  SETTINGS_SET_VALUE( resourcePath, Variant(path) );}
void PackageOptions::_setCaesar3Path(std::string path) {  SETTINGS_SET_VALUE( c3gfx, Variant(path) );}
void PackageOptions::_setCaesar3Music(std::string path) {  SETTINGS_SET_VALUE( c3music, Variant(path) ); }
void PackageOptions::_setCaesar3Video(std::string path) {  SETTINGS_SET_VALUE( c3video, Variant(path) ); }

void PackageOptions::_update()
{
  if( _d->edResourcesPath )
  {
    _d->edResourcesPath->setText( SETTINGS_VALUE( resourcePath ).toString() );
  }

  if( _d->edCaesar3Path )
  {
    _d->edCaesar3Path->setText( SETTINGS_VALUE( c3gfx ).toString() );
  }

  if( _d->edCaesar3Music )
  {
    _d->edCaesar3Music->setText( SETTINGS_VALUE( c3music).toString() );
  }

  if( _d->edCaesar3Video )
  {
    _d->edCaesar3Video->setText( SETTINGS_VALUE( c3video ).toString() );
  }
}

}//end namespace dialog

}//end namespace gui
