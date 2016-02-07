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
#include "vfs/directory.hpp"
#include "dialogbox.hpp"
#include "environment.hpp"

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
  EditBox* edScreenshots;
  bool needRestart;

  Impl() : edResourcesPath(0), edCaesar3Music(0), edCaesar3Video(0),
           edCaesar3Path(0), needRestart(false) {}
};

PackageOptions::PackageOptions( Widget* parent, const Rect& rectangle )
  : Window( parent, rectangle, "" ), _d( new Impl )
{
  setupUI( ":/gui/packageopts.gui" );

  GET_DWIDGET_FROM_UI(_d,edResourcesPath)
  INIT_WIDGET_FROM_UI(TexturedButton*,btnApply)
  GET_DWIDGET_FROM_UI(_d,edCaesar3Path)
  GET_DWIDGET_FROM_UI(_d,edCaesar3Video)
  GET_DWIDGET_FROM_UI(_d,edCaesar3Music)
  GET_DWIDGET_FROM_UI(_d,edScreenshots)

  _update();

  CONNECT_LOCAL( btnApply,            onClicked(),     PackageOptions::_close )
  CONNECT_LOCAL( _d->edResourcesPath, onTextChanged(), PackageOptions::_setResourcesPath )
  CONNECT_LOCAL( _d->edCaesar3Path,   onTextChanged(), PackageOptions::_setCaesar3Path )
  CONNECT_LOCAL( _d->edCaesar3Video,  onTextChanged(), PackageOptions::_setCaesar3Video )
  CONNECT_LOCAL( _d->edCaesar3Music,  onTextChanged(), PackageOptions::_setCaesar3Music )
  CONNECT_LOCAL( _d->edScreenshots,   onTextChanged(), PackageOptions::_setScreenshotsDir )

  WidgetClose::insertTo( this );
  moveToCenter();
  setModal();
}

PackageOptions::~PackageOptions() {}
void PackageOptions::_setResourcesPath(std::string path) {  SETTINGS_SET_VALUE( resourcePath, path );}
void PackageOptions::_setCaesar3Music(std::string path) {  SETTINGS_SET_VALUE( c3music, path ); }
void PackageOptions::_setCaesar3Video(std::string path) {  SETTINGS_SET_VALUE( c3video, path ); }
void PackageOptions::_setScreenshotsDir(std::string path) { SETTINGS_SET_VALUE( screenshotDir, path ); }

void PackageOptions::_close()
{
  if( _d->needRestart )
  {
    Dialog& dlg = Information( ui(), "Note", "Please restart game to apply change" );
    dlg.show();
  }

  vfs::Directory screenDir = SETTINGS_STR( screenshotDir );
  if( !screenDir.exist() )
    vfs::Directory::createByPath( screenDir );

  game::Settings::instance().save();

  deleteLater();
}

void PackageOptions::_setCaesar3Path(std::string path)
{
  SETTINGS_SET_VALUE( c3gfx, path );

  if( path.empty() )
  {
    std::string noPath("");
    SETTINGS_SET_VALUE( c3music, noPath );
    SETTINGS_SET_VALUE( c3video, noPath );
  }
  else
  {
    _d->needRestart = true;
    SETTINGS_SET_VALUE( c3music, path + "/wavs" );
    SETTINGS_SET_VALUE( c3video, path + "/smk" );
  }

  _updateC3paths();
}

void PackageOptions::_update()
{
  if( _d->edResourcesPath )
  {
    _d->edResourcesPath->setText( SETTINGS_STR( resourcePath ) );
  }

  if( _d->edCaesar3Path )
  {
    _d->edCaesar3Path->setText( SETTINGS_STR( c3gfx ) );
  }

  if( _d->edScreenshots )
  {
    _d->edScreenshots->setText( SETTINGS_STR( screenshotDir ) );
  }

  _updateC3paths();
}

void PackageOptions::_updateC3paths()
{
  if( _d->edCaesar3Music )
  {
    _d->edCaesar3Music->setText( SETTINGS_STR( c3music ) );
  }

  if( _d->edCaesar3Video )
  {
    _d->edCaesar3Video->setText( SETTINGS_STR( c3video ) );
  }
}

}//end namespace dialog

}//end namespace gui
