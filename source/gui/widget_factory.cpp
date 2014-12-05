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

#include "widget_factory.hpp"
#include "core/utils.hpp"
#include "label.hpp"
#include "editbox.hpp"
#include "pushbutton.hpp"
#include "core/requirements.hpp"
#include "texturedbutton.hpp"
#include "listbox.hpp"
#include "image.hpp"
#include "smkviewer.hpp"
#include "core/logger.hpp"
#include "groupbox.hpp"
#include "filelistbox.hpp"
#include "dictionary_text.hpp"
#include <map>

namespace gui
{

template< class T > class BaseWidgetCreator : public WidgetCreator
{
public:
  Widget* create( Widget* parent )
  {
    return new T( parent );
  }
};

class WidgetFactory::Impl
{
public:
  typedef std::map< std::string, WidgetCreator* > BuildingCreators;
  BuildingCreators constructors;
};

Widget* WidgetFactory::create(const std::string& type, Widget* parent ) const
{
  Impl::BuildingCreators::iterator findConstructor = _d->constructors.find( type );

  if( findConstructor != _d->constructors.end() )
  {
    return findConstructor->second->create( parent );
  }

  return 0;
}

WidgetFactory::WidgetFactory() : _d( new Impl )
{
  // entertainment
  addCreator( CAESARIA_STR_EXT(Label),          new BaseWidgetCreator<Label>() );
  addCreator( CAESARIA_STR_EXT(EditBox),        new BaseWidgetCreator<EditBox>() );
  addCreator( CAESARIA_STR_EXT(TexturedButton), new BaseWidgetCreator<TexturedButton>() );
  addCreator( CAESARIA_STR_EXT(PushButton),     new BaseWidgetCreator<PushButton>() );
  addCreator( CAESARIA_STR_EXT(GroupBox),       new BaseWidgetCreator<GroupBox>() );
  addCreator( CAESARIA_STR_EXT(ListBox),        new BaseWidgetCreator<ListBox>() );
  addCreator( CAESARIA_STR_EXT(Image),          new BaseWidgetCreator<Image>() );
  addCreator( CAESARIA_STR_EXT(SmkViewer),      new BaseWidgetCreator<SmkViewer>() );
  addCreator( CAESARIA_STR_EXT(FileListBox),    new BaseWidgetCreator<FileListBox>() );
  addCreator( CAESARIA_STR_EXT(DictionaryText), new BaseWidgetCreator<DictionaryText>() );
}

WidgetFactory::~WidgetFactory() {}

void WidgetFactory::addCreator( const std::string& typeName, WidgetCreator* ctor )
{
  bool alreadyHaveConstructor = _d->constructors.find( typeName ) != _d->constructors.end();

  if( !alreadyHaveConstructor )
  {
    _d->constructors[ typeName ] = ctor;
  }
  else
  {
    Logger::warning( "already have constructor for this widget type" );
  }
}

bool WidgetFactory::canCreate( const std::string& type ) const
{
  return _d->constructors.find( type ) != _d->constructors.end();
}

}//end namespace gui
