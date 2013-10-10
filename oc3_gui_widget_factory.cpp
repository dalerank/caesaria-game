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

#include "oc3_gui_widget_factory.hpp"
#include "oc3_stringhelper.hpp"
#include "oc3_gui_label.hpp"
#include "oc3_gui_editbox.hpp"
#include "oc3_pushbutton.hpp"
#include "oc3_requirements.hpp"
#include "oc3_texturedbutton.hpp"
#include "oc3_listbox.hpp"
#include <map>

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
  addCreator( OC3_STR_EXT(Label), new BaseWidgetCreator<Label>() );
  addCreator( OC3_STR_EXT(EditBox), new BaseWidgetCreator<EditBox>() );
  addCreator( OC3_STR_EXT(TexturedButton), new BaseWidgetCreator<TexturedButton>() );
  addCreator( OC3_STR_EXT(PushButton), new BaseWidgetCreator<PushButton>() );
  addCreator( OC3_STR_EXT(ListBox), new BaseWidgetCreator<ListBox>() );
}

WidgetFactory::~WidgetFactory()
{

}

void WidgetFactory::addCreator( const std::string& typeName, WidgetCreator* ctor )
{
  bool alreadyHaveConstructor = _d->constructors.find( typeName ) != _d->constructors.end();

  if( !alreadyHaveConstructor )
  {
    _d->constructors[ typeName ] = ctor;
  }
  else
  {
    StringHelper::debug( 0xff, "already have constructor for this widget type" );
  }
}

bool WidgetFactory::canCreate( const std::string& type ) const
{
  return _d->constructors.find( type ) != _d->constructors.end();
}
