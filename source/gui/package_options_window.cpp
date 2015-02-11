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
  EditBox* edCaesar3Path;
  TexturedButton* btnApply;
  PushButton* btnChangeCellw;
};

PackageOptions::PackageOptions( Widget* parent, const Rect& rectangle )
  : Window( parent, rectangle, "" ), _d( new Impl )
{
  setupUI( ":/gui/packageopts.gui" );
  setCenter( parent->center() );

  GET_DWIDGET_FROM_UI(_d,edResourcesPath)
  GET_DWIDGET_FROM_UI(_d,btnApply)
  GET_DWIDGET_FROM_UI(_d,btnChangeCellw)
  GET_DWIDGET_FROM_UI(_d,edCaesar3Path)

  CONNECT( _d->btnApply, onClicked(), this, PackageOptions::deleteLater );
  CONNECT( _d->btnApply, onClicked(), this, PackageOptions::_saveOptions );
  CONNECT( _d->edResourcesPath, onTextChanged(), this, PackageOptions::_setResourcesPath );
  CONNECT( _d->edCaesar3Path, onTextChanged(), this, PackageOptions::_setCaesar3Path );
  CONNECT( _d->btnChangeCellw, onClicked(), this, PackageOptions::_changeCellw );

  WidgetEscapeCloser::insertTo( this );

  _update();

  if( _d->btnApply ) _d->btnApply->setFocus();
}

PackageOptions::~PackageOptions() {}

void PackageOptions::_saveOptions()
{
  game::Settings::instance().save();
}

void PackageOptions::_setResourcesPath(std::string path)
{
  SETTINGS_SET_VALUE( resourcePath, Variant(path) );
}

void PackageOptions::_setCaesar3Path(std::string path)
{
  SETTINGS_SET_VALUE( c3gfx, Variant(path) );
}

void PackageOptions::_changeCellw()
{
  int cellWidth = SETTINGS_VALUE( cellw );
  cellWidth = cellWidth == 30 ? 60 : 30;
  SETTINGS_SET_VALUE( cellw, cellWidth );
  _update();
}

void PackageOptions::_update()
{
  if( _d->btnChangeCellw )
  {
    int cellWidth = SETTINGS_VALUE( cellw );
    _d->btnChangeCellw->setText( "CellWidth: " + utils::i2str( cellWidth ) );
  }

  if( _d->edResourcesPath )
  {
    _d->edResourcesPath->setText( SETTINGS_VALUE( resourcePath ).toString() );
  }

  if( _d->edCaesar3Path )
  {
    _d->edCaesar3Path->setText( SETTINGS_VALUE( c3gfx ).toString() );
  }
}

}//end namespace dialog

}//end namespace gui
