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

#include "listboxitem.hpp"

using namespace gfx;

namespace gui
{

class ListBoxItem::Impl
{
public:
  std::string text;
  Variant data;
	int tag;
  float currentHovered;
  Picture icon;
  ElementState state;
  bool enabled;
  Point offset;

  Alignment horizontal;
  Alignment vertical;
  bool alignEnabled;
};

void ListBoxItem::setText(const std::string& text){    _d->text = text;}
void ListBoxItem::setIcon( Picture icon ){    _d->icon = icon; }
const std::string& ListBoxItem::text() const{    return _d->text;}
const Alignment& ListBoxItem::verticalAlign() const{    return _d->vertical;}
bool ListBoxItem::isAlignEnabled() const{    return _d->enabled;}
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
		OverrideColors[ i ].Use = other.OverrideColors[ i ].Use;
		OverrideColors[ i ].font = other.OverrideColors[ i ].font;
		OverrideColors[ i ].color = other.OverrideColors[ i ].color;
	}

	return *this;
}

ListBoxItem& ListBoxItem::getInvalidItem()
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

ListBoxItem::~ListBoxItem(){}
void ListBoxItem::setTag( int tag ){	_d->tag = tag;}
int ListBoxItem::tag() const{	return _d->tag;}
bool ListBoxItem::isEnabled() const{    return _d->enabled;}
void ListBoxItem::setEnabled( bool en ){    _d->enabled = en;}
ElementState ListBoxItem::state() const{    return _d->state;}
void ListBoxItem::setState( const ElementState& st ){    _d->state = st;}
Point ListBoxItem::offset() const{  return _d->offset;}
void ListBoxItem::setOffset(Point p){  _d->offset = p;}
Variant ListBoxItem::data() const{ return _d->data; }
void ListBoxItem::setData(const Variant& value){ _d->data = value; }
float ListBoxItem::currentHovered() const {   return _d->currentHovered;}
void ListBoxItem::updateHovered( float delta ){    _d->currentHovered = math::clamp<float>( _d->currentHovered + delta, 0.f, 255.f );}
Picture ListBoxItem::icon() const { return _d->icon; }

}//end namespace gui
