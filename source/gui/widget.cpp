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

#ifdef _MSC_VER
#include <cmath>
#endif

#include "widget.hpp"
#include "widgetprivate.hpp"
#include "environment.hpp"
#include "core/event.hpp"
#include "core/variant_map.hpp"
#include "core/variant_list.hpp"
#include "core/foreach.hpp"
#include "core/saveadapter.hpp"
#include "core/utils.hpp"
#include "core/logger.hpp"
#include "core/gettext.hpp"
#include "rect_calc.hpp"

namespace gui
{

static const Variant invalidVariant;
GAME_LITERALCONST(vars)

void Widget::beforeDraw(gfx::Engine& painter )
{
  __D_IMPL(d,Widget)
  if( !d->parent )
    return;
  //"Parent must be exists";

  for( auto child : d->children )
    child->beforeDraw( painter );
}

Ui* Widget::ui() const { return _dfunc()->environment; }

void Widget::setTextAlignment(align::Type horizontal, align::Type vertical )
{
  if( !(horizontal >= align::upperLeft && horizontal <= align::automatic)
     || !(vertical >= align::upperLeft && vertical <= align::automatic) )
  {
    Logger::warning( "Unknown align in SetTextAlignment" );
    return;
  }

  __D_REF(d,Widget)
  d.textAlign.horizontal = horizontal;
  d.textAlign.vertical = vertical;
}

void Widget::setMaxWidth( unsigned int width ) { _dfunc()->size.maximimum.setWidth( width );}
unsigned int Widget::height() const            { return relativeRect().height(); }

Widget::Widget( Widget* parent, int id, const Rect& rectangle )
: __INIT_IMPL(Widget)
{
  __D_REF(_d,Widget)
  _d.align.left = align::upperLeft;
  _d.align.right = align::upperLeft;
  _d.align.top = align::upperLeft;
  _d.align.bottom = align::upperLeft;
  _d.flag.visible = true;
  _d.size.maximimum = Size(0,0);
  _d.size.mininimum = Size(1,1);
  _d.parent = parent;
  _d.id = id;
  _d.flag.enabled = true;
  _d.flag.internal = false;
  _d.noClip = false;
  _d.tabOrder = -1;
  _d.isTabGroup = false;
  _d.environment = parent ? parent->ui() : 0;

  Logger::warningIf( !parent, "Parent for widget is null" );

  _d.rect.relative = rectangle;
  _d.rect.absolute = rectangle;
  _d.rect.clipping = rectangle;
  _d.rect.desired = rectangle;
  _d.flag.tabStop = false;

#ifdef _DEBUG
  setDebugName( "AbstractWidget" );
#endif

  // if we were given a parent to attach to
  if( parent )
  {
    parent->_addChild(this);
    _recalculateAbsolutePosition(true);
    drop();
  }

  setTextAlignment( align::upperLeft, align::center );
}

Widget::~Widget()
{
  // delete all children
  for( auto child : _dfunc()->children )
  {
    child->setParent( 0 );
    child->drop();
  }
}

void Widget::setGeometry( const Rect& r, GeometryType mode )
{
  __D_IMPL(_d,Widget)
  if( parent() )
  {
    const Rect& r2 = parent()->absoluteRect();
    SizeF d = r2.size().toSizeF();

    if( _d->align.left == align::scale)
      _d->rect.scale.setLeft( (float)r.left() / d.width() );
    if (_d->align.right == align::scale)
      _d->rect.scale.setRight( (float)r.right() / d.width() );
    if (_d->align.top == align::scale)
      _d->rect.scale.setTop( (float)r.top() / d.height() );
    if (_d->align.bottom == align::scale)
      _d->rect.scale.setBottom(  (float)r.bottom() / d.height() );
  }

  _d->rect.desired = r;
  updateAbsolutePosition();
}

void Widget::_finalizeResize() {}
void Widget::_finalizeMove() {}

Widget::Widgets& Widget::_children() { return _dfunc()->children;}

void Widget::setPosition( const Point& position )
{
	const Rect rectangle( position, size() );
  setGeometry( rectangle );
}

void Widget::setPosition(int x, int y)
{
  setPosition( Point( x, y) );
}

void Widget::setGeometry( const RectF& r, GeometryType mode )
{
  if( !parent() )
    return;

  __D_REF(d,Widget)
  const Size& s = parent()->size();

  switch( mode )
  {
  case ProportionalGeometry:
    d.rect.desired = Rect(
          floor( s.width() * r.left() ),
          floor( s.height() * r.top() ),
          floor( s.width() * r.right() ),
          floor( s.height() * r.bottom() ));

    d.rect.scale = r;
  break;

  default:
  break;
  }

  updateAbsolutePosition();
}

void Widget::setGeometry(float left, float top, float rigth, float bottom)
{
  setGeometry( RectF( left, top, rigth, bottom ) );
}

const Rect& Widget::absoluteRect() const { return _dfunc()->rect.absolute; }
const Rect& Widget::absoluteClippingRect() const{ return _dfunc()->rect.clipping; }

void Widget::setNotClipped( bool noClip )
{
  _dfunc()->noClip = noClip;
  updateAbsolutePosition();
}

void Widget::setMaxSize( const Size& size )
{
  _dfunc()->size.maximimum = size;
  updateAbsolutePosition();
}

void Widget::setMinSize( const Size& size )
{
  __D_REF(d,Widget)
  d.size.mininimum = size;
  if( d.size.mininimum.width() < 1)
      d.size.mininimum.setWidth( 1 );

  if( d.size.mininimum.height() < 1)
      d.size.mininimum.setHeight( 1 );

  updateAbsolutePosition();
}

void Widget::setAlignment( Alignment left, Alignment right, Alignment top, Alignment bottom )
{
  __D_REF(d,Widget)
  d.align.left = left;
  d.align.right = right;
  d.align.top = top;
  d.align.bottom = bottom;

  if( parent() )
  {
    Rect r( parent()->absoluteRect() );
    SizeF s = r.size().toSizeF();

    RectF desiredRect = d.rect.desired.toRectF();
    if( d.align.left == align::scale )
      d.rect.scale.setLeft( desiredRect.left() / s.width() );
    if( d.align.right == align::scale )
      d.rect.scale.setRight( desiredRect.right() / s.width() );
    if( d.align.top  == align::scale )
      d.rect.scale.setTop( desiredRect.top() / s.height() );
    if( d.align.bottom == align::scale )
      d.rect.scale.setBottom( desiredRect.bottom() / s.height() );
  }
}

void Widget::updateAbsolutePosition()
{
  _recalculateAbsolutePosition(false);

  // update all children
  for( auto child : _dfunc()->children )
    child->updateAbsolutePosition();
}

Widget* Widget::getElementFromPoint( const Point& point )
{
  Widget* target = 0;
  __D_REF(_d,Widget)
  // we have to search from back to front, because later children
  // might be drawn over the top of earlier ones.

  auto it = _d.children.getLast();

  if (visible())
  {
    while(it != _d.children.end())
    {
      target = (*it)->getElementFromPoint(point);
      if( target )
      {
        return target;
      }

      --it;
    }
  }

  if( visible() && isPointInside(point) )
  {
    target = this;
  }

  return target;
}

bool Widget::isPointInside( const Point& point ) const
{
  return absoluteClippingRect().isPointInside(point);
}

void Widget::addChild( Widget* child )
{
  _addChild(child);
  if (child)
  {
    child->updateAbsolutePosition();
  }
}

void Widget::removeChild( Widget* child )
{
  __D_REF(_d,Widget)
  foreach( it, _d.children )
  {
    if ((*it) == child)
    {
      (*it)->setParent( 0 );
      (*it)->drop();
      _d.children.erase(it);
      return;
    }
  }
}

void Widget::draw(gfx::Engine& painter )
{
  if ( visible() )
  {
    for( auto child : _dfunc()->children )
      child->draw( painter );
  }
}

void Widget::debugDraw(gfx::Engine& painter)
{
  if ( visible() )
  {
    for( auto child : _dfunc()->children )
      child->debugDraw( painter );
  }
}

void Widget::setTaborder( int index )
{
  __D_REF(_d,Widget)
  // negative = autonumber
  if (index < 0)
  {
    _d.tabOrder = 0;
    Widget *el = tabgroup();
    while( _d.isTabGroup && el && el->parent() )
        el = el->parent();

    Widget *first=0, *closest=0;
    if (el)
    {
        // find the highest element number
        el->next(-1, true, _d.isTabGroup, first, closest, true);
        if (first)
        {
            _d.tabOrder = first->tabOrder() + 1;
        }
    }
  }
  else
  {
    _d.tabOrder = index;
  }
}

int Widget::tabOrder() const{  return _dfunc()->tabOrder;}

Widget* Widget::tabgroup()
{
  Widget *ret=this;

  while (ret && !ret->hasTabgroup())
      ret = ret->parent();

  return ret;
}

bool Widget::enabled() const
{
  __D_IMPL_CONST(_d,Widget)
  if ( isSubElement() && _d->flag.enabled && parent() )
  {
    return parent()->enabled();
  }

  return _d->flag.enabled;
}

bool Widget::bringToFront()
{
	if( parent() )
	{
		return parent()->bringChildToFront( this );
	}

	return false;
}

bool Widget::bringChildToFront( Widget* element )
{
  Widgets& children = _children();
  foreach( it, children )
  {
    if (element == (*it))
    {
      children.erase(it);
      children.push_back(element);
      return true;
    }
  }

  return false;
}

bool Widget::sendChildToBack( Widget* child )
{
  Widgets& children = _children();
  auto it = children.begin();
  if (child == (*it))	// already there
      return true;

  for (; it != children.end(); ++it)
  {
    if (child == (*it))
    {
      children.erase(it);
      children.push_front(child);
      return true;
    }
  }

  return false;
}

bool Widget::sendToBack()
{
	if( parent() )
	{
		return parent()->sendChildToBack( this );
	}

	return false;
}

Widget* Widget::findChild(int id, bool searchChildren) const
{
  Widget* e = 0;

  for( auto child : _dfunc()->children )
  {
    if( child->ID() == id)
      return child;

    if( searchChildren )
      e = child->findChild(id, true);

    if( e )
      return e;
  }

  return e;
}

Widget* Widget::findChild(const std::string& internalName, bool searchChildren) const
{
  Widget* e = 0;

  for( auto child : _dfunc()->children )
  {
    if( child->internalName() == internalName)
      return child;

    if( searchChildren )
      e = child->findChild(internalName, true);

    if( e )
      return e;
  }

  return e;
}

bool Widget::next( int startOrder, bool reverse, bool group, Widget*& first, Widget*& closest, bool includeInvisible/*=false*/ ) const
{
    // we'll stop searching if we find this number
    int wanted = startOrder + ( reverse ? -1 : 1 );
    if (wanted==-2)
        wanted = 1073741824; // maximum int

    __D_IMPL_CONST(_d,Widget)
    auto it = _d->children.begin();

    int closestOrder, currentOrder;

    while(it != _d->children.end())
    {
        // ignore invisible elements and their children
        if ( ( (*it)->visible() || includeInvisible ) &&
             (group == true || (*it)->hasTabgroup() == false) )
        {
            // only check tab stops and those with the same group status
            if ((*it)->isTabStop() && ((*it)->hasTabgroup() == group))
            {
                currentOrder = (*it)->tabOrder();

                // is this what we're looking for?
                if (currentOrder == wanted)
                {
                  closest = *it;
                  return true;
                }

                // is it closer than the current closest?
                if (closest)
                {
                  closestOrder = closest->tabOrder();
                  if ( ( reverse && currentOrder > closestOrder && currentOrder < startOrder)
                      ||(!reverse && currentOrder < closestOrder && currentOrder > startOrder))
                  {
                    closest = *it;
                  }
                }
                else if ( (reverse && currentOrder < startOrder) || (!reverse && currentOrder > startOrder) )
                {
                  closest = *it;
                }

                // is it before the current first?
                if (first)
                {
                  closestOrder = first->tabOrder();

                  if ( (reverse && closestOrder < currentOrder) || (!reverse && closestOrder > currentOrder) )
                  {
                      first = *it;
                  }
                }
                else
                {
                  first = *it;
                }
            }
            // search within children
            if ((*it)->next(startOrder, reverse, group, first, closest))
            {
              return true;
            }
        }
        ++it;
    }
    return false;
}

void Widget::setParent(Widget* p) {  _dfunc()->parent = p; }

static int __convStr2RelPos( Widget* w, const VariantMap& vars, std::string s, int rwidth )
{
  s = utils::replace( s, " ", "" );

  WidgetCalc wcalc( *w, vars, rwidth );
  return wcalc.eval( s );
}

void Widget::setTextAlignment(const std::string& horizontal, const std::string& vertical)
{
  align::Helper ahelper;

  setTextAlignment( ahelper.findType( horizontal ),
                    ahelper.findType( vertical ) );
}

void Widget::setupUI( const VariantMap& options )
{
  __D_REF(_d,Widget)
  _d.internalName = options.get( "name" ).toString();
  align::Helper ahelper;
  VariantList textAlign = options.get( "textAlign" ).toList();
  VariantList altAlign = options.get( "text.align" ).toList();
  if( !altAlign.empty() )
  {
    textAlign = altAlign;
  }

  if( textAlign.size() > 1 )
  {
    setTextAlignment( ahelper.findType( textAlign.front().toString() ),
                      ahelper.findType( textAlign.back().toString() ) );
  }

  Variant vAnchorBottom = options.get( "anchor.bottom" );
  if( vAnchorBottom.isValid() )
  {
    auto bottomAlign = ahelper.findType( vAnchorBottom.toString() );
    setAlignment( _d.align.left, _d.align.right, _d.align.top, bottomAlign );
  }

  Variant vAnchorTop = options.get( "anchor.top" );
  if( vAnchorTop.isValid() )
  {
    auto topAlign = ahelper.findType( vAnchorBottom.toString() );
    setAlignment( _d.align.left, _d.align.right, topAlign, _d.align.bottom );
  }

  Variant tmp;
  setID( (int)options.get( "id", _d.id ) );
  if( options.has( "text" ) ) setText( _( options.get( "text" ).toString() ) );
  if( options.has( "tooltip" ) ) setTooltipText( _( options.get( "tooltip" ).toString() ) );
  setVisible( options.get( "visible", true ).toBool() );
  setEnabled( options.get( "enabled", true ).toBool() );
  _d.flag.tabStop = options.get( "tabStop", false ).toBool();
  _d.isTabGroup = options.get( "tabGroup", -1 ).toInt();
  _d.tabOrder = options.get( "tabOrder", -1 ).toInt();
  setMaxSize( options.get( "maximumSize", Size::zero).toSize() );
  setMinSize( options.get( "minimumSize", Size(1,1) ).toSize() );
  VariantMap vars = options.get( literals::vars ).toMap();

  VariantList aRectList = options.get( "geometry" ).toList();
  if( !aRectList.empty() )
  {
    Rect cRect(
       __convStr2RelPos( this, vars, aRectList.get( 0 ).toString(), parent()->width() ),
       __convStr2RelPos( this, vars, aRectList.get( 1 ).toString(), parent()->height() ),
       __convStr2RelPos( this, vars, aRectList.get( 2 ).toString(), parent()->width() ),
       __convStr2RelPos( this, vars, aRectList.get( 3 ).toString(), parent()->height() ) );

    setGeometry( cRect );
  }

  tmp = options.get( "geometryf" );
  if( tmp.isValid() )
  {
    RectF r = tmp.toRectf();
    if (r.width() > 1 && r.height() > 1)
    {
      r = RectF(0, 0, 1, 1);
      Logger::warning( "Incorrect geometryf values [{}, {}, {}, {}]",
                       r.left(), r.top(), r.right(), r.bottom() );
    }

    setGeometry( r );
  }

  tmp = options.get( "size" );
  if( tmp.isValid() )
  {
    Size s = tmp.toSize();
    if( s.width() > 0 ) { setWidth( s.width()); }
    if( s.height() > 0 ) { setHeight( s.height() ); }
  }

  setNotClipped( options.get( "noclipped", false ).toBool() );

  for( const auto& item : options )
  {
    if( item.second.type() != Variant::Map )
      continue;

    VariantMap newWidgetOptions = item.second.toMap();
    newWidgetOptions[ literals::vars ] = vars;

    std::string widgetName = item.first;
    std::string widgetType;
    std::string::size_type delimPos = widgetName.find( '#' );
    if( delimPos != std::string::npos )
    {
      widgetType = widgetName.substr( delimPos+1 );
      widgetName = widgetName.substr( 0, delimPos );
    }
    else
    {
      widgetType = newWidgetOptions.get( "type" ).toString();
    }       

    if( !widgetType.empty() )
    {
      Widget* child = findChild( widgetName, true );
      if( child != nullptr )
      {
        child->setupUI( newWidgetOptions );
      }
      else
      {
        child = ui()->createWidget( widgetType, this );
        if( child != nullptr && child->internalName().empty() )
        {
          child->setupUI( newWidgetOptions );
          child->setInternalName( widgetName );
        }
      }
    }
  }

  VariantList positionV = options.get( "position" ).toList();
  if( !positionV.empty() )
  {
    Point poisition( __convStr2RelPos( this, vars, positionV.get( 0 ).toString(), parent()->width() ),
                     __convStr2RelPos( this, vars, positionV.get( 1 ).toString(), parent()->height() ) );
    move( poisition );
  }

  _d.properties = options.get( "properties" ).toMap();
}

void Widget::setupUI(const vfs::Path& filename)
{
  Logger::warning( "Widget: load gui model from " + filename.toString() );
  setupUI( config::load( filename ) );
}

void Widget::_addChild( Widget* child )
{
  if (child)
  {
    child->grab(); // prevent destruction when removed
    child->remove(); // remove from old parent
    child->_dfunc()->rect.lastParent = absoluteRect();
    child->setParent( this );
    _dfunc()->children.push_back(child);
  }
}

void Widget::_recalculateAbsolutePosition( bool recursive )
{
  Rect parentAbsolute(0,0,0,0);
  Rect parentAbsoluteClip;
  float fw=0.f, fh=0.f;

  __D_REF(_d,Widget)
  if ( parent() )
  {
    parentAbsolute = parent()->absoluteRect();

    if( _d.noClip )
    {
      Widget* p=this;
      while( p && p->parent() )
          p = p->parent();

      parentAbsoluteClip = p->absoluteClippingRect();
    }
    else
      parentAbsoluteClip = parent()->absoluteClippingRect();
  }

  const int diffx = parentAbsolute.width() - _d.rect.lastParent.width();
  const int diffy = parentAbsolute.height() - _d.rect.lastParent.height();


  if( _d.align.left == align::scale || _d.align.right == align::scale)
      fw = (float)parentAbsolute.width();

  if( _d.align.top == align::scale || _d.align.bottom == align::scale)
      fh = (float)parentAbsolute.height();

  switch( _d.align.left)
  {
  case align::automatic:
  case align::upperLeft: break;
  case align::lowerRight: _d.rect.desired._lefttop += Point( diffx, 0 ); break;
  case align::center: _d.rect.desired._lefttop += Point( diffx/2, 0 ); break;
  case align::scale: _d.rect.desired.setLeft( _d.rect.scale.left() * fw ); break;
  }

  switch( _d.align.right)
  {
  case align::automatic:
  case align::upperLeft:   break;
  case align::lowerRight: _d.rect.desired._bottomright += Point( diffx, 0 ); break;
  case align::center: _d.rect.desired._bottomright += Point( diffx/2, 0 ); break;
  case align::scale: _d.rect.desired.setRight( roundf( _d.rect.scale.right() * fw ) ); break;
  }

  switch( _d.align.top)
  {
  case align::automatic:
  case align::upperLeft: break;
  case align::lowerRight: _d.rect.desired._lefttop += Point( 0, diffy ); break;
  case align::center: _d.rect.desired._lefttop += Point( 0, diffy/2 ); break;
  case align::scale: _d.rect.desired.setTop( roundf(_d.rect.scale.top() * fh) ); break;
  }

  switch( _d.align.bottom)
  {
  case align::automatic:
  case align::upperLeft:  break;
  case align::lowerRight: _d.rect.desired._bottomright += Point( 0, diffy );  break;
  case align::center:  _d.rect.desired._bottomright += Point( 0, diffy/2 );  break;
  case align::scale: _d.rect.desired.setBottom( roundf(_d.rect.scale.bottom() * fh) );  break;
  }

  _d.rect.relative = _d.rect.desired;

  const int w = _d.rect.relative.width();
  const int h = _d.rect.relative.height();

  // make sure the desired rectangle is allowed
  if (w < (int)_d.size.mininimum.width() )
      _d.rect.relative.setRight( _d.rect.relative.left() + _d.size.mininimum.width() );
  if (h < (int)_d.size.mininimum.height() )
      _d.rect.relative.setBottom( _d.rect.relative.top() + _d.size.mininimum.height() );
  if (_d.size.maximimum.width() > 0 && w > (int)_d.size.maximimum.width() )
      _d.rect.relative.setRight( _d.rect.relative.left() + _d.size.maximimum.width() );
  if (_d.size.maximimum.height() > 0 && h > (int)_d.size.maximimum.height() )
      _d.rect.relative.setBottom( _d.rect.relative.top() + _d.size.maximimum.height() );

  _d.rect.relative.repair();

  _d.rect.absolute = _d.rect.relative + parentAbsolute.lefttop();

  if (!parent())
      parentAbsoluteClip = absoluteRect();

  _d.rect.clipping = absoluteRect();
  _d.rect.clipping.clipAgainst(parentAbsoluteClip);

  _d.rect.lastParent = parentAbsolute;

  if ( recursive )
  {
    // update all children
    for( auto child : _d.children )
    {
      child->_recalculateAbsolutePosition(recursive);
    }
  }

  _finalizeResize();
}

void Widget::animate( unsigned int timeMs )
{
  if( !visible() )
    return;

  for( auto child : _dfunc()->children )
    child->animate( timeMs );
}

void Widget::remove()
{
  //"parent must be exist for element"
  if( parent() )
      parent()->removeChild( this );
}

bool Widget::onEvent( const NEvent& event )
{
  bool resolved = false;
  for( auto child : _dfunc()->eventHandlers )
  {
    bool handled = child->onEvent( event );
    if( handled )
      return true;
  }

  if (event.EventType == sEventMouse)
  {
    switch( event.mouse.type )
    {
    case NEvent::Mouse::btnLeftPressed:
    case NEvent::Mouse::btnMiddlePressed:
    case NEvent::Mouse::btnRightPressed:
       resolved = _onMousePressed( event.mouse );
    break;

    default: break;
    }

    if (parent() && (parent()->parent() == NULL))
      return true;
  }
  else if( event.EventType == sEventGui )
  {
    switch( event.gui.type )
    {
    case guiButtonClicked: resolved = _onButtonClicked( event.gui.caller ); break;
    case guiListboxChanged: resolved = _onListboxChanged( event.gui.caller ); break;
    default: break;
    }
  }

  if( resolved )
    return true;

  return parent() ? parent()->onEvent(event) : false;
}

bool Widget::isMyChild( Widget* child ) const
{
  if (!child)
    return false;

  do
  {
    if( child->parent() )
     child = child->parent();

  } while (child->parent() && child != this);

  return child == this;
}

void Widget::setWidth( unsigned int width )
{
  const Rect rectangle( relativeRect().lefttop(), Size( width, height() ) );
  setGeometry( rectangle );
}

void Widget::setHeight( unsigned int height )
{
  const Rect rectangle( relativeRect().lefttop(), Size( width(), height ) );
  setGeometry( rectangle );
}

void Widget::setEnabled(bool enabled){  _dfunc()->flag.enabled = enabled;}
std::string Widget::internalName() const{    return _dfunc()->internalName;}
void Widget::setInternalName( const std::string& name ){    _dfunc()->internalName = name;}
Widget* Widget::parent() const {    return _dfunc()->parent;}
Rect Widget::relativeRect() const{  return _dfunc()->rect.relative;}
bool Widget::isNotClipped() const{  return _dfunc()->noClip;}
void Widget::setVisible( bool visible ){  _dfunc()->flag.visible = visible;}
bool Widget::isTabStop() const{  return _dfunc()->flag.tabStop;}
bool Widget::hasTabgroup() const{  return _dfunc()->isTabGroup;}
void Widget::setText( const std::string& text ){  _dfunc()->text.value = text;}
void Widget::setTooltipText( const std::string& text ) {  _dfunc()->text.tooltip = text;}
std::string Widget::text() const{  return _dfunc()->text.value;}
std::string Widget::tooltipText() const{  return _dfunc()->text.tooltip;}
int Widget::ID() const{  return _dfunc()->id;}
void Widget::setID( int id ) {  _dfunc()->id = id; }
const Widget::Widgets& Widget::children() const{  return _dfunc()->children;}
Size Widget::maxSize() const{    return _dfunc()->size.maximimum;}
Size Widget::minSize() const{    return _dfunc()->size.mininimum;}
bool Widget::isHovered() const{  return ui()->isHovered( this );}
bool Widget::isFocused() const{  return ui()->hasFocus( this );}
Rect Widget::clientRect() const{  return Rect( 0, 0, width(), height() );}
void Widget::setFont(const Font& font) {}
void Widget::setFont(const std::string& font) { setFont( Font::create( font ) ); }
void Widget::setFocus(){  ui()->setFocus( this );}
void Widget::removeFocus(){  ui()->removeFocus( this );}
Rect& Widget::absoluteClippingRectRef() const { return _dfunc()->rect.clipping; }
unsigned int Widget::width() const{  return relativeRect().width();}
Size Widget::size() const{  return Size( _dfunc()->rect.relative.width(), _dfunc()->rect.relative.height() );}
int Widget::screenTop() const { return absoluteRect().top(); }
int Widget::screenLeft() const { return absoluteRect().left(); }
int Widget::screenBottom() const { return absoluteRect().bottom(); }
int Widget::screenRight() const { return absoluteRect().right(); }
Point Widget::lefttop() const { return Point( left(), top() ); }
Point Widget::leftbottom() const { return Point( left(), bottom() ); }
Point Widget::righttop() const { return Point( right(), top() ); }
Point Widget::rightbottom() const { return Point( right(), bottom() ); }
Point Widget::localToScreen( const Point& localPoint ) const{  return localPoint + _dfunc()->rect.absolute.lefttop();}
Rect Widget::localToScreen( const Rect& localRect ) const{  return localRect + _dfunc()->rect.absolute.lefttop();}
Point Widget::screenToLocal(const Point &screenPoint) const { return screenPoint - _dfunc()->rect.absolute.lefttop(); }
void Widget::move( const Point& relativeMovement ){  setGeometry( _dfunc()->rect.relative + relativeMovement );}
int Widget::bottom() const{  return _dfunc()->rect.relative.bottom(); }
Point Widget::center() const { return (_dfunc()->rect.relative.rightbottom() + _dfunc()->rect.relative.lefttop()) / 2; }
void Widget::setTabgroup( bool isGroup ) { _dfunc()->isTabGroup = isGroup; }
bool Widget::visible() const{  return _dfunc()->flag.visible;}
bool Widget::isSubElement() const{  return _dfunc()->flag.internal;}
void Widget::setSubElement( bool subElement ){  _dfunc()->flag.internal = subElement;}
void Widget::setTabstop( bool enable ){  _dfunc()->flag.tabStop = enable;}
void Widget::setLeft( int newLeft ) { setPosition( Point( newLeft, top() ) ); }
void Widget::setTop( int newTop ) { setPosition( Point( left(), newTop ) );  }
int Widget::top() const { return relativeRect().top(); }
int Widget::left() const { return relativeRect().left(); }
int Widget::right() const { return relativeRect().right(); }
void Widget::hide() { setVisible( false ); }
void Widget::show() {  setVisible( true ); }
Alignment Widget::horizontalTextAlign() const{  return _dfunc()->textAlign.horizontal; }
Alignment Widget::verticalTextAlign() const{  return _dfunc()->textAlign.vertical;}
void Widget::deleteLater(){ ui()->deleteLater( this ); }
Font Widget::font() const { return Font(); }

void Widget::setFont(FontType type, NColor color)
{
  Font font = Font::create( type );
  if( color.color != 0 )
    font.setColor( color );
  setFont( font );
}


void Widget::setRight( int newRight )
{
  Rect r = relativeRect();
  r.rright() = newRight;
  setGeometry( r );
}

void Widget::moveToCenter() { setCenter( parent()->center() ); }

void Widget::addProperty(const std::string& name, const Variant& value)
{
  _dfunc()->properties[ name ] = value;
}

void Widget::canvasDraw(const std::string& text, const Point& point, Font font, NColor color)
{

}

void Widget::canvasDraw(const gfx::Picture& picture, const Point& point)
{

}

const Variant& Widget::getProperty(const std::string& name) const
{
  VariantMap::const_iterator it = _dfunc()->properties.find( name );
  return it != _dfunc()->properties.end() ? it->second : invalidVariant;
}

void Widget::installEventHandler( Widget* elementHandler )
{
  _dfunc()->eventHandlers.insert( elementHandler );
}

void Widget::setCenter(Point center)
{
  Rect newRect( Point( center.x() - width() / 2, center.y() - height() / 2), size() );
  setGeometry( newRect );
}

void Widget::setBottom( int b )
{
  Rect r = _dfunc()->rect.relative;
  r.setBottom(  b );
  setGeometry( r );
}

}//end namespace gui
