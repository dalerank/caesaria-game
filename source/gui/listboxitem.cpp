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

#include "listboxitem.hpp"
#include "core/variant_map.hpp"

using namespace gfx;

namespace gui
{

class ListBoxItem::Impl
{
public:
  Picture textPic;
  std::string text;
  std::string tooltip;
  VariantMap data;
  Variant tag;
  float currentHovered;
  Picture icon;
  Point iconOffset;
  ElementState state;
  bool enabled;
  Point offset;
  std::string url;

  Alignment horizontal;
  Alignment vertical;
  bool alignEnabled;
};

void ListBoxItem::setText(const std::string& text){ _d->text = text;}
void ListBoxItem::setTooltip(const std::string& text) { _d->tooltip = text; }
const std::string&ListBoxItem::tooltip() const{ return _d->tooltip; }
void ListBoxItem::setIcon( Picture icon ){    _d->icon = icon; }
void ListBoxItem::setIcon( const std::string& rc, int index ) { _d->icon.load( rc, index ); }
const std::string& ListBoxItem::text() const{    return _d->text;}
const Alignment& ListBoxItem::verticalAlign() const{    return _d->vertical;}
bool ListBoxItem::isAlignEnabled() const{ return _d->enabled; }
const Alignment& ListBoxItem::horizontalAlign() const{    return _d->horizontal;}

ListBoxItem::ListBoxItem() : _d( new Impl )
{
  _d->currentHovered = 0.f;
  _d->enabled = true;
  _d->horizontal = align::upperLeft;
  _d->vertical = align::center;
  _d->alignEnabled = false;
  _d->state = stNormal;
}

ListBoxItem::ListBoxItem( const ListBoxItem& other ) : _d( new Impl )
{
  *this = other;
}

ListBoxItem& ListBoxItem::operator=( const ListBoxItem& other )
{
  _d->enabled = other._d->enabled;
  _d->horizontal = other._d->horizontal;
  _d->vertical = other._d->vertical;
  _d->alignEnabled = other._d->alignEnabled;

  _d->icon = other._d->icon;
	_d->tag = other._d->tag;
  _d->state = other._d->state;
	_d->text = other._d->text;
	_d->data = other._d->data;

	for( unsigned int i=0; i < count;i++ )
	{
		overrideColors[ i ].Use = other.overrideColors[ i ].Use;
		overrideColors[ i ].font = other.overrideColors[ i ].font;
		overrideColors[ i ].color = other.overrideColors[ i ].color;
	}

	return *this;
}

ListBoxItem& ListBoxItem::invalidItem()
{
	static ListBoxItem invalidItem = ListBoxItem();
	return invalidItem;
}

void ListBoxItem::setTextAlignment( Alignment horizontal, Alignment vertical )
{
  _d->vertical = vertical;
  _d->horizontal = horizontal;
  _d->alignEnabled = true;
}

void ListBoxItem::setTextColor(ListBoxItem::ColorType type, NColor color)
{
  overrideColors[ type ].color = color;
  overrideColors[ type ].Use = true;
}

void ListBoxItem::updateText(const Point &p, Font f, const Size &s)
{
  resetPicture( s );

  if( _d->textPic.isValid() )
  {
    f.draw( _d->textPic, _d->text, p );
  }
}

void ListBoxItem::resetPicture( const Size& s )
{
  if( !_d->textPic.isValid() )
  {
    _d->textPic = Picture( s, 0, true );
  }
  else if( _d->textPic.size() != s )
  {
    _d->textPic = Picture( s, 0, true );
  }
}

void ListBoxItem::draw(const std::string& text, Font f, const Point& p )
{
  if( _d->textPic.isValid() )
  {
    f.draw( _d->textPic, text, p );
  }
}

void ListBoxItem::clear()
{
  if( _d->textPic.isValid() )
  {
    _d->textPic.fill( 0x0 );
  }
}

ListBoxItem::~ListBoxItem(){}
void ListBoxItem::setTag( const Variant& tag ){	_d->tag = tag;}
const Variant& ListBoxItem::tag() const{	return _d->tag;}
bool ListBoxItem::isEnabled() const{    return _d->enabled;}
void ListBoxItem::setEnabled( bool en ){    _d->enabled = en;}
ElementState ListBoxItem::state() const{    return _d->state;}
void ListBoxItem::setState( const ElementState& st ){    _d->state = st;}
Point ListBoxItem::textOffset() const{  return _d->offset;}
void ListBoxItem::setTextOffset(Point p){  _d->offset = p;}
Point ListBoxItem::iconOffset() const{ return _d->iconOffset; }
void ListBoxItem::setIconOffset(Point p) { _d->iconOffset = p; }
const Picture& ListBoxItem::picture() const { return _d->textPic; }
void ListBoxItem::setUrl(const std::string& url) { _d->url = url; }
const std::string&ListBoxItem::url() const { return _d->url; }
Variant ListBoxItem::data( const std::string &name) const{ return _d->data[ name ]; }
void ListBoxItem::setData( const std::string &name, const Variant& value ){ _d->data[name] = value; }
void ListBoxItem::setData(const VariantMap& map) { _d->data = map; }
float ListBoxItem::currentHovered() const {   return _d->currentHovered;}
void ListBoxItem::updateHovered( float delta ){    _d->currentHovered = math::clamp<float>( _d->currentHovered + delta, 0.f, 255.f );}
Picture ListBoxItem::icon() { return _d->icon; }

}//end namespace gui
