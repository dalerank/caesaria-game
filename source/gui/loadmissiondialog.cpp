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
//
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "loadmissiondialog.hpp"
#include "gfx/picture.hpp"
#include "gfx/engine.hpp"
#include "filelistbox.hpp"
#include "widget_helper.hpp"
#include "vfs/entries.hpp"
#include "vfs/directory.hpp"
#include "core/logger.hpp"
#include "pushbutton.hpp"
#include "label.hpp"
#include "image.hpp"
#include "core/saveadapter.hpp"
#include "core/variant_map.hpp"
#include "core/gettext.hpp"
#include "dictionary_text.hpp"
#include "widgetescapecloser.hpp"

using namespace gfx;

namespace gui
{

namespace dialog
{

class LoadMission::Impl
{
public:
  FileListBox* lbxFiles;
  Label* lbTitle;
  DictionaryText* lbDescription;
  Image* imgPreview;
  vfs::Directory directory;
  std::string saveItemText;
  PushButton* btnLoad;

  void fillFiles();

slots public:
  void resolveItemSelected(const ListBoxItem& item);
  void emitSelectFile();

signals public:
  Signal1<std::string> onSelectFileSignal;
};

void LoadMission::Impl::resolveItemSelected(const ListBoxItem& item)
{
  saveItemText = item.text();

  vfs::Path fn(saveItemText);
  fn = directory/fn;

  std::string missionName = vfs::Path( fn ).baseName( false ).toString();
  VariantMap vm = config::load( fn );
  Locale::addTranslation( missionName );

  std::string text = vm.get( "preview.text" ).toString();
  std::string previewImg = vm.get( "preview.image" ).toString();
  std::string title = vm.get( "preview.title" ).toString();

  if( lbDescription ) lbDescription->setText( _(text) );
  if( imgPreview ) imgPreview->setPicture( Picture::load( previewImg ) );
  if( btnLoad ) btnLoad->setEnabled( !saveItemText.empty() );
  if( lbTitle ) lbTitle->setText( _( title ) );
}

LoadMission::LoadMission(Widget* parent , const vfs::Directory &dir)
  : Widget( parent, -1, Rect( 0, 0, 100, 100 ) ), _d( new Impl )
{
  setupUI( ":/gui/loadmissiondialog.gui" );

  WidgetEscapeCloser::insertTo( this );

  _d->directory = dir;

  GET_DWIDGET_FROM_UI( _d, lbxFiles )
  GET_DWIDGET_FROM_UI( _d, btnLoad )
  GET_DWIDGET_FROM_UI( _d, lbDescription )
  GET_DWIDGET_FROM_UI( _d, imgPreview )
  GET_DWIDGET_FROM_UI( _d, lbTitle )

  CONNECT( _d->lbxFiles, onItemSelected(), _d.data(), Impl::resolveItemSelected )
  CONNECT( _d->btnLoad, onClicked(), _d.data(), Impl::emitSelectFile );

  if( _d->lbxFiles ) _d->lbxFiles->setShowTime( false );

  _d->fillFiles();
  if( _d->lbxFiles ) _d->lbxFiles->setFocus();
}

void LoadMission::Impl::fillFiles()
{
  if( !lbxFiles )
    return;
  lbxFiles->clear();

  vfs::Entries flist = vfs::Directory( directory ).getEntries();
  flist = flist.filter( vfs::Entries::file | vfs::Entries::extFilter, ".mission" );

  StringArray names;
  foreach( it, flist )
  {
    names << (*it).fullpath.toString();
  }
  std::sort( names.begin(), names.end() );

  lbxFiles->addItems( names );
}

void LoadMission::Impl::emitSelectFile()
{
  if( saveItemText.empty() )
    return;

  vfs::Path fn(saveItemText);
  emit onSelectFileSignal( (directory/fn).toString() );
}


void LoadMission::draw( gfx::Engine& engine )
{
  if( !visible() )
    return;

  Widget::draw( engine );
}

Signal1<std::string>& LoadMission::onSelectFile() { return _d->onSelectFileSignal; }

LoadMission* LoadMission::create( Widget* parent, const vfs::Directory& dir )
{
  LoadMission* ret = new LoadMission( parent, dir );
  ret->setCenter( parent->center() );
  return ret;
}

}//end namespace dialog

}//end namespace gui
