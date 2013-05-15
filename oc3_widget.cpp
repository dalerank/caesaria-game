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


#include "oc3_widget.hpp"
#include "oc3_widgetprivate.hpp"
#include "oc3_guienv.hpp"
#include "oc3_event.hpp"

void Widget::beforeDraw( GfxEngine& painter )
{
	_OC3_DEBUG_BREAK_IF( !_d->parent && "Parent must be exists" );
    ChildIterator it = _d->children.begin();
    for (; it != _d->children.end(); ++it)
         (*it)->beforeDraw( painter );
}

// void Widget::setFont( Font font, u32 nA/*=0 */ )
// {
//     _d->overrideFonts[ nA ] = font;
// }
// 
// Font Widget::getFont( u32 index/*=0 */ ) const
// {
//     FontsMap::Node* ret = _d->overrideFonts.find( index );
//     return ret ? ret->getValue() : Font();
// }

// void Widget::setColor( const Color& color, u32 nA/*=0 */ )
// {
//     _d->overrideColors.set( nA, _OverrideColor( color, true ) );
// }
// 
// Color Widget::getColor( u32 index/*=0 */ ) const
// {
// 	ColorMap::Node* ret = _d->overrideColors.find( index );
// 	return ret ? ret->getValue().color : Color( 0xff000000 );
// }


// FontsMap& Widget::getFonts_()
// {
// 	return _d->overrideFonts;
// }

// unsigned int Widget::getResultOpacity()
// {
//     f32 resultAlpha = getOpacity() / 255.f;
// 
//     Widget* parent = getParent();
//     while( parent )
//     {
//         resultAlpha *= ( parent->getOpacity() / 255.f );
//         parent = parent->getParent();
//     }
// 
//     return u32( resultAlpha * 0xff );
// }
// 
// Color Widget::getResultColor( Color defColor /*= video::SColor( 0xffffffff ) */ )
// {
//     defColor.setAlpha( math::middle_<u32>( defColor.getAlpha(), getResultOpacity(), 255 ) );
//     return defColor;
// }

GuiEnv* Widget::getEnvironment()
{
    return _environment;
}

void Widget::setTextAlignment( TypeAlign horizontal, TypeAlign vertical )
{
    _OC3_DEBUG_BREAK_IF( (!(horizontal >= alignUpperLeft && horizontal <= alignAuto)
        || !(vertical >= alignUpperLeft && vertical <= alignAuto))
        && "Unknown align in SetTextAlignment" );

    _d->textHorzAlign = horizontal;
    _d->textVertAlign = vertical;
}

void Widget::styleChanged()
{
    ConstChildIterator it = _d->children.begin();
    for (; it != _d->children.end(); ++it)
         (*it)->styleChanged();
}

void Widget::setMaxWidth( unsigned int width )
{
    _maxSize.setWidth( width );
}

unsigned int Widget::getHeight() const
{
    return getRelativeRect().getHeight();
}

Widget::Widget( Widget* parent, int id, const Rect& rectangle )
: _d( new WidgetPrivate ),
  _maxSize(0,0), _minSize(1,1), _isVisible(true), _isEnabled(true),
  _isSubElement(false), _noClip(false), _id(id), _isTabStop(false), _tabOrder(-1), _isTabGroup(false),
  _alignLeft(alignUpperLeft), _alignRight(alignUpperLeft), _alignTop(alignUpperLeft), _alignBottom(alignUpperLeft),
  _environment( parent ? parent->getEnvironment() : NULL ), _eventHandler( NULL )
{
    _d->parent = parent;
	_d->relativeRect = rectangle;
	_d->absoluteRect = rectangle;
	_d->absoluteClippingRect = rectangle;
	_d->desiredRect = rectangle;

#ifdef _DEBUG
    setDebugName( "AbstractWidget" );
#endif

    // if we were given a parent to attach to
    if (parent)
    {
        parent->addChild_(this);
        recalculateAbsolutePosition(true);
		drop();
    }

    setTextAlignment( alignUpperLeft, alignCenter );
}

Widget::~Widget()
{
    // delete all children
    ChildIterator it = _d->children.begin();
    for (; it != _d->children.end(); ++it)
    {
        (*it)->_d->parent = 0;
        (*it)->drop();
    }
}

void Widget::setGeometry( const Rect& r, GeometryType mode )
{
    if( getParent() )
    {
        /*const Rect& r2 = */getParent()->getAbsoluteRect();

//         NSizeF d = r2.getSize().As<f32>();//( (f32)(r2.getSize().Width), (f32)(r2.getSize().Height) );
// 
// 		RectF& scaleRect = _d->scaleRect;
//         if (_alignLeft   == alignScale)
//             scaleRect.UpperLeftCorner.X = (f32)r.UpperLeftCorner.X / d.Width;
//         if (_alignRight  == alignScale)
//             scaleRect.LowerRightCorner.X = (f32)r.LowerRightCorner.X / d.Width;
//         if (_alignTop    == alignScale)
//             scaleRect.UpperLeftCorner.Y = (f32)r.UpperLeftCorner.Y / d.Height;
//         if (_alignBottom == alignScale)
//             scaleRect.LowerRightCorner.Y = (f32)r.LowerRightCorner.Y / d.Height;
    }

    _d->desiredRect = r;   
	updateAbsolutePosition();
}

void Widget::resizeEvent_()
{
}

void Widget::setPosition( const Point& position )
{
	const Rect rectangle( position, getSize() );
    setGeometry( rectangle );
}

// void Widget::setRelativeRectProportional( const RectF& r, GeometryType mode )
// {
//     if( !getParent() )
//         return;
// 
//     const NSizeI& d = getParent()->getSize();
// 
// 	switch( mode )
//     {
//     case ProportionalGeometry:
//         _d->desiredRect = RectI(
//             math::floor32( d.Width * r.UpperLeftCorner.X),
//             math::floor32( d.Height * r.UpperLeftCorner.Y),
//             math::floor32( d.Width * r.LowerRightCorner.X),
//             math::floor32( d.Height * r.LowerRightCorner.Y));
// 
//         _d->scaleRect = r;
//     break;
// 
//     default:
//     break;
//     }
//   
//     updateAbsolutePosition();
// }

Rect Widget::getAbsoluteRect() const
{
    return _d->absoluteRect;
}

Rect Widget::getAbsoluteClippingRect() const
{
    return _d->absoluteClippingRect;
}

void Widget::setNotClipped( bool noClip )
{
    _noClip = noClip;
    updateAbsolutePosition();
}

void Widget::setMaxSize( const Size& size )
{
    _maxSize = size;
    updateAbsolutePosition();
}

void Widget::setMinSize( const Size& size )
{
    _minSize = size;
    if (_minSize.getWidth() < 1)
        _minSize.setWidth( 1 );

    if (_minSize.getHeight() < 1)
        _minSize.setHeight( 1 );

    updateAbsolutePosition();
}

void Widget::setAlignment( TypeAlign left, TypeAlign right, TypeAlign top, TypeAlign bottom )
{
    _alignLeft = left;
    _alignRight = right;
    _alignTop = top;
    _alignBottom = bottom;

    if( getParent() )
    {
        Rect r( getParent()->getAbsoluteRect() );

//         SizeF d = r.getSize().As<f32>();
// 
// 		RectF& scaleRect = _d->scaleRect;
// 		RectF dRect = _d->desiredRect.As<f32>();
//         if (_alignLeft   == alignScale)
//             scaleRect.UpperLeftCorner.X = dRect.UpperLeftCorner.X / d.Width;
//         if (_alignRight  == alignScale)
//             scaleRect.LowerRightCorner.X = dRect.LowerRightCorner.X / d.Width;
//         if (_alignTop    == alignScale)
//             scaleRect.UpperLeftCorner.Y = dRect.UpperLeftCorner.Y / d.Height;
//         if (_alignBottom == alignScale)
//             scaleRect.LowerRightCorner.Y = dRect.LowerRightCorner.Y / d.Height;
    }
}

void Widget::updateAbsolutePosition()
{
    const Rect oldRect = _d->absoluteRect;
    recalculateAbsolutePosition(false);

    if( oldRect != _d->absoluteRect )
        resizeEvent_();

    // update all children
    ChildIterator it = _d->children.begin();
    for (; it != _d->children.end(); ++it)
    {
        (*it)->updateAbsolutePosition();
    }
}

Widget* Widget::getElementFromPoint( const Point& point )
{
    Widget* target = 0;

    // we have to search from back to front, because later children
    // might be drawn over the top of earlier ones.

    ChildIterator it = _d->children.getLast();

    if (isVisible())
    {
        while(it != _d->children.end())
        {
            target = (*it)->getElementFromPoint(point);
            if (target)
                return target;

            --it;
        }
    }

    if (isVisible() && isPointInside(point))
        target = this;

    return target;
}

bool Widget::isPointInside( const Point& point ) const
{
    return getAbsoluteClippingRect().isPointInside(point);
}

void Widget::addChild( Widget* child )
{
    addChild_(child);
    if (child)
    {
        child->updateAbsolutePosition();
    }
}

void Widget::removeChild( Widget* child )
{
    ChildIterator it = _d->children.begin();
    for (; it != _d->children.end(); ++it)
        if ((*it) == child)
        {
            (*it)->_d->parent = 0;
            (*it)->drop();
            _d->children.erase(it);
            return;
        }
}

void Widget::draw( GfxEngine& painter )
{
    if ( isVisible() )
    {
        ChildIterator it = _d->children.begin();
        for (; it != _d->children.end(); ++it)
            (*it)->draw( painter );
    }
}

bool Widget::isVisible() const
{
    return _isVisible;
}

bool Widget::isSubElement() const
{
    return _isSubElement;
}

void Widget::setSubElement( bool subElement )
{
    _isSubElement = subElement;
}

void Widget::setTabStop( bool enable )
{
    _isTabStop = enable;
}

void Widget::setTabOrder( int index )
{
    // negative = autonumber
    if (index < 0)
    {
        _tabOrder = 0;
        Widget *el = getTabGroup();
        while( _isTabGroup && el && el->getParent() )
            el = el->getParent();

        Widget *first=0, *closest=0;
        if (el)
        {
            // find the highest element number
            el->getNextWidget(-1, true, _isTabGroup, first, closest, true);
            if (first)
            {
                _tabOrder = first->getTabOrder() + 1;
            }
        }

    }
    else
        _tabOrder = index;
}

int Widget::getTabOrder() const
{
    return _tabOrder;
}

Widget* Widget::getTabGroup()
{
    Widget *ret=this;

    while (ret && !ret->hasTabGroup())
        ret = ret->getParent();

    return ret;
}

bool Widget::isEnabled() const
{
    if ( isSubElement() && _isEnabled && getParent() )
        return getParent()->isEnabled();

    return _isEnabled;
}

bool Widget::bringToFront()
{
	if( getParent() )
		return getParent()->bringChildToFront( this );

	return false;
}

bool Widget::bringChildToFront( Widget* element )
{
    ChildIterator it = _d->children.begin();
    for (; it != _d->children.end(); ++it)
    {
        if (element == (*it))
        {
            _d->children.erase(it);
            _d->children.push_back(element);
            return true;
        }
    }

    return false;
}

bool Widget::sendChildToBack( Widget* child )
{
    ChildIterator it = _d->children.begin();
    if (child == (*it))	// already there
        return true;
    for (; it != _d->children.end(); ++it)
    {
        if (child == (*it))
        {
            _d->children.erase(it);
            _d->children.push_front(child);
            return true;
        }
    }

    return false;
}

bool Widget::sendToBack()
{
	if( getParent() )
		return getParent()->sendChildToBack( this );

	return false;
}

Widget* Widget::findChild( int id, bool searchchildren/*=false*/ ) const
{
    Widget* e = 0;

    ConstChildIterator it = _d->children.begin();
    for (; it != _d->children.end(); ++it)
    {
        if ((*it)->getID() == id)
            return (*it);

        if (searchchildren)
            e = (*it)->findChild(id, true);

        if (e)
            return e;
    }

    return e;
}

bool Widget::getNextWidget( int startOrder, bool reverse, bool group, Widget*& first, Widget*& closest, bool includeInvisible/*=false*/ ) const
{
    // we'll stop searching if we find this number
    int wanted = startOrder + ( reverse ? -1 : 1 );
    if (wanted==-2)
        wanted = 1073741824; // maximum int

    ConstChildIterator it = _d->children.begin();

    int closestOrder, currentOrder;

    while(it != _d->children.end())
    {
        // ignore invisible elements and their children
        if ( ( (*it)->isVisible() || includeInvisible ) &&
            (group == true || (*it)->hasTabGroup() == false) )
        {
            // only check tab stops and those with the same group status
            if ((*it)->isTabStop() && ((*it)->hasTabGroup() == group))
            {
                currentOrder = (*it)->getTabOrder();

                // is this what we're looking for?
                if (currentOrder == wanted)
                {
                    closest = *it;
                    return true;
                }

                // is it closer than the current closest?
                if (closest)
                {
                    closestOrder = closest->getTabOrder();
                    if ( ( reverse && currentOrder > closestOrder && currentOrder < startOrder)
                        ||(!reverse && currentOrder < closestOrder && currentOrder > startOrder))
                    {
                        closest = *it;
                    }
                }
                else
                    if ( (reverse && currentOrder < startOrder) || (!reverse && currentOrder > startOrder) )
                    {
                        closest = *it;
                    }

                    // is it before the current first?
                    if (first)
                    {
                        closestOrder = first->getTabOrder();

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
            if ((*it)->getNextWidget(startOrder, reverse, group, first, closest))
            {
                return true;
            }
        }
        ++it;
    }
    return false;
}

// void Widget::save( core::VariantArray* out ) const
// {
//     out->AddString( SerializeHelper::styleProp, getStyle().getName() );
//     out->AddFloat( SerializeHelper::opacityProp, getOpacity() );
//     out->AddString( SerializeHelper::internalNameProp, _d->internalName );
//     out->AddEnum( SerializeHelper::hTextAlignProp, _d->textHorzAlign, NrpAlignmentNames );
//     out->AddEnum( SerializeHelper::vTextAlignProp, _d->textVertAlign, NrpAlignmentNames );
//     out->AddInt( SerializeHelper::idProp, _id );
//     out->AddString( SerializeHelper::captionProp, getText() );
//     out->AddString( SerializeHelper::tooltipProp, getTooltipText() );
//     out->AddRect( SerializeHelper::rectangleProp, _d->desiredRect );
//     out->AddSize( SerializeHelper::minSizeProp, _minSize );
//     out->AddSize( SerializeHelper::maxSizeProp, _maxSize );
//     out->AddEnum( SerializeHelper::leftAlignProp, _alignLeft, NrpAlignmentNames);
//     out->AddEnum( SerializeHelper::rightAlignProp, _alignRight, NrpAlignmentNames);
//     out->AddEnum( SerializeHelper::topAlignProp, _alignTop, NrpAlignmentNames);
//     out->AddEnum( SerializeHelper::bottomAlignProp, _alignBottom, NrpAlignmentNames);
//     out->AddBool( SerializeHelper::visibleProp, _isVisible);
//     out->AddBool( SerializeHelper::enabledProp, _isEnabled);
//     out->AddBool( SerializeHelper::tabStopProp, _isTabStop);
//     out->AddBool( SerializeHelper::tabGroupProp, _isTabGroup);
//     out->AddInt( SerializeHelper::tabOrderProp, _tabOrder);
//     out->AddBool( SerializeHelper::noClipProp, _noClip);
// }
// 
// void Widget::load( core::VariantArray* in )
// {
//     setStyle( in->getAttributeAsString( SerializeHelper::styleProp ) );
//     setOpacity( in->getAttributeAsFloat( SerializeHelper::opacityProp ) );
//     _d->internalName = in->getAttributeAsString( SerializeHelper::internalNameProp );
//     setTextAlignment( (TypeAlign)in->getAttributeAsEnumeration( SerializeHelper::hTextAlignProp, NrpAlignmentNames ),
//                       (TypeAlign)in->getAttributeAsEnumeration( SerializeHelper::vTextAlignProp, NrpAlignmentNames ) );
//     setID( in->getAttributeAsInt( SerializeHelper::idProp ) );
//     setText(in->getAttributeAsString( SerializeHelper::captionProp ) );
//     setTooltipText( in->getAttributeAsString( SerializeHelper::tooltipProp ) );
//     setVisible( in->getAttributeAsBool( SerializeHelper::visibleProp ) );
//     setEnabled(in->getAttributeAsBool( SerializeHelper::enabledProp ));
//     _isTabStop = in->getAttributeAsBool( SerializeHelper::tabStopProp );
//     _isTabGroup = in->getAttributeAsBool( SerializeHelper::tabGroupProp );
//     _tabOrder = in->getAttributeAsInt( SerializeHelper::tabOrderProp );
// 
//     setMaxSize( in->getAttributeAsSize( SerializeHelper::maxSizeProp ) );
//     setMinSize( in->getAttributeAsSize( SerializeHelper::minSizeProp ) );
// 
//     setAlignment( (TypeAlign) in->getAttributeAsEnumeration( SerializeHelper::leftAlignProp, NrpAlignmentNames),
//                   (TypeAlign)in->getAttributeAsEnumeration( SerializeHelper::rightAlignProp, NrpAlignmentNames),
//                   (TypeAlign)in->getAttributeAsEnumeration( SerializeHelper::topAlignProp, NrpAlignmentNames),
//                   (TypeAlign)in->getAttributeAsEnumeration( SerializeHelper::bottomAlignProp, NrpAlignmentNames));
// 
//     setGeometry( in->getAttributeAsRect( SerializeHelper::rectangleProp ));
// 
//     setNotClipped( in->getAttributeAsBool( SerializeHelper::noClipProp ) );
// }

void Widget::addChild_( Widget* child )
{
    if (child)
    {
        child->grab(); // prevent destruction when removed
        child->remove(); // remove from old parent
        child->_d->lastParentRect = getAbsoluteRect();
        child->_d->parent = this;
        _d->children.push_back(child);
    }
}

void Widget::recalculateAbsolutePosition( bool recursive )
{
    Rect parentAbsolute(0,0,0,0);
    Rect parentAbsoluteClip;
    //float fw=0.f, fh=0.f;

    if ( getParent() )
    {
        parentAbsolute = getParent()->getAbsoluteRect();

        if (_noClip)
        {
            Widget* p=this;
            while( p && p->getParent() )
                p = p->getParent();

            parentAbsoluteClip = p->getAbsoluteClippingRect();
        }
        else
            parentAbsoluteClip = getParent()->getAbsoluteClippingRect();
    }

    const int diffx = parentAbsolute.getWidth() - _d->lastParentRect.getWidth();
    const int diffy = parentAbsolute.getHeight() - _d->lastParentRect.getHeight();

    /* QUESTION: garbage code?????
    if (_alignLeft == alignScale || _alignRight == alignScale)
        fw = (float)parentAbsolute.getWidth();

    if (_alignTop == alignScale || _alignBottom == alignScale)
        fh = (float)parentAbsolute.getHeight();
    */
    
    switch (_alignLeft)
    {
    case alignAuto:
    case alignUpperLeft:
        break;
    case alignLowerRight:
        _d->desiredRect.UpperLeftCorner += Point( diffx, 0 );
        break;
    case alignCenter:
        _d->desiredRect.UpperLeftCorner += Point( diffx/2, 0 );
        break;
    case alignScale:
        //_d->desiredRect.UpperLeftCorner.setX( _d->scaleRect.UpperLeftCorner.X * fw );
        break;
    }

    switch (_alignRight)
    {
    case alignAuto:
    case alignUpperLeft:
        break;
    case alignLowerRight:
        _d->desiredRect.LowerRightCorner += Point( diffx, 0 );
        break;
    case alignCenter:
        _d->desiredRect.LowerRightCorner += Point( diffx/2, 0 );
        break;
    case alignScale:
        //_d->desiredRect.LowerRightCorner.X = math::round_(_d->scaleRect.LowerRightCorner.X * fw);
        break;
    }

    switch (_alignTop)
    {
    case alignAuto:
    case alignUpperLeft:
        break;
    case alignLowerRight:
        _d->desiredRect.UpperLeftCorner += Point( 0, diffy );
        break;
    case alignCenter:
        _d->desiredRect.UpperLeftCorner += Point( 0, diffy/2 );
        break;
    case alignScale:
        //_d->desiredRect.UpperLeftCorner.Y = math::round_(_d->scaleRect.UpperLeftCorner.Y * fh);
        break;
    }

    switch (_alignBottom)
    {
    case alignAuto:
    case alignUpperLeft:
        break;
    case alignLowerRight:
        _d->desiredRect.LowerRightCorner += Point( 0, diffy );
        break;
    case alignCenter:
        _d->desiredRect.LowerRightCorner += Point( 0, diffy/2 );
        break;
    case alignScale:
        //_d->desiredRect.LowerRightCorner.Y = math::round_(_d->scaleRect.LowerRightCorner.Y * fh);
        break;
    }

    _d->relativeRect = _d->desiredRect;

    const int w = _d->relativeRect.getWidth();
    const int h = _d->relativeRect.getHeight();

    // make sure the desired rectangle is allowed
    if (w < (int)_minSize.getWidth() )
        _d->relativeRect.LowerRightCorner.setX( _d->relativeRect.UpperLeftCorner.getX() + _minSize.getWidth() );
    if (h < (int)_minSize.getHeight() )
        _d->relativeRect.LowerRightCorner.setY( _d->relativeRect.UpperLeftCorner.getY() + _minSize.getHeight() );
    if (_maxSize.getWidth() > 0 && w > (int)_maxSize.getWidth() )
        _d->relativeRect.LowerRightCorner.setX( _d->relativeRect.UpperLeftCorner.getX() + _maxSize.getWidth() );
    if (_maxSize.getHeight() > 0 && h > (int)_maxSize.getHeight() )
        _d->relativeRect.LowerRightCorner.setY( _d->relativeRect.UpperLeftCorner.getY() + _maxSize.getHeight() );

    _d->relativeRect.repair();

    _d->absoluteRect = _d->relativeRect + parentAbsolute.UpperLeftCorner;

    if (!getParent())
        parentAbsoluteClip = getAbsoluteRect();

    _d->absoluteClippingRect = getAbsoluteRect();
    _d->absoluteClippingRect.clipAgainst(parentAbsoluteClip);

    _d->lastParentRect = parentAbsolute;

    if ( recursive )
    {
        // update all children
        ChildIterator it = _d->children.begin();
        for (; it != _d->children.end(); ++it)
        {
            (*it)->recalculateAbsolutePosition(recursive);
        }
    }
}

void Widget::animate( unsigned int timeMs )
{
    if ( isVisible() )
    {
        ChildIterator it = _d->children.begin();
        for (; it != _d->children.end(); ++it)
            (*it)->animate( timeMs );
    }
}

void Widget::remove()
{
    _OC3_DEBUG_BREAK_IF( !getParent() && "parent must be exist for element" );
    if( getParent() )
        getParent()->removeChild( this );
}

void Widget::setEnabled(bool enabled)
{
    _isEnabled = enabled;
}

// f32 Widget::getOpacity( u32 index/*=0 */ ) const
// {
//     return ( index < _d->opacity.size() ) ? (f32)_d->opacity[ index ] : 0xff;
// }
// 
// void Widget::setOpacity( f32 nA, int index/*=0 */ )
// {
//     _d->opacity[ index ] = math::clamp<f32>( nA, 0, 255 );
// }

std::string Widget::getInternalName() const
{
    return _d->internalName;
}

void Widget::setInternalName( const std::string& name )
{
    _d->internalName = name;
}

Widget* Widget::getParent() const
{
    return _d->parent;
}

Rect Widget::getRelativeRect() const
{
    return _d->relativeRect;
}

bool Widget::isNotClipped() const
{
    return _noClip;
}

void Widget::setVisible( bool visible )
{
    _isVisible = visible;
}

bool Widget::isTabStop() const
{
    return _isTabStop;
}

bool Widget::hasTabGroup() const
{
    return _isTabGroup;
}

void Widget::setText( const std::string& text )
{
    _text = text;
}

void Widget::setTooltipText( const std::string& text )
{
    _toolTipText = text;
}

std::string Widget::getText() const
{
    return _text;
}

std::string Widget::getTooltipText() const
{
    return _toolTipText;
}

int Widget::getID() const
{
    return _id;
}

void Widget::setID( int id )
{
    _id = id;
}

bool Widget::onEvent( const NEvent& event )
{
    if( _eventHandler )
        _eventHandler->onEvent( event );

    if (event.EventType == OC3_MOUSE_EVENT)
      if (getParent() && (getParent()->getParent() == NULL))
        return true;

    return getParent() ? getParent()->onEvent(event) : false;
}

const Widget::Widgets& Widget::getChildren() const
{
    return _d->children;
}

bool Widget::isMyChild( Widget* child ) const
{
    if (!child)
        return false;
    do
    {
        if( child->getParent() )
            child = child->getParent();

    } while (child->getParent() && child != this);

	return child == this;
}

Size Widget::getMaxSize() const
{
    return _maxSize;
}

Size Widget::getMinSize() const
{
    return _minSize;
}

// bool Widget::isColorEnabled( u32 index ) const
// {
//     ColorMap::Node* ret = _d->overrideColors.find( index );
//     return ret ? ret->getValue().enabled : false;
// }
// 
// void Widget::setEnabledColor( bool enable, u32 index/*=0*/ )
// {
//     ColorMap::Node* ret = _d->overrideColors.find( index );
//     if( ret )
//     {
//         _d->overrideColors.set( index, _OverrideColor( ret->getValue().color, enable ));
//     }
// }

void Widget::installEventHandler( Widget* elementHandler )
{
    _eventHandler = elementHandler;
}

bool Widget::isHovered() const
{
    return _environment->isHovered( this );
}

bool Widget::isFocused() const
{
    return _environment->hasFocus( this );
}

Rect Widget::getClientRect() const
{
    return Rect( 0, 0, getWidth(), getHeight() );
}

void Widget::setFocus()
{
    getEnvironment()->setFocus( this );
}

void Widget::removeFocus()
{
	getEnvironment()->removeFocus( this );
}

Rect& Widget::getAbsoluteClippingRectRef() const
{
	return _d->absoluteClippingRect;
}

unsigned int Widget::getWidth() const
{
	return getRelativeRect().getWidth();
}

Size Widget::getSize() const
{
	return Size( _d->relativeRect.getWidth(), _d->relativeRect.getHeight() );
}

int Widget::getScreenTop() const { return getAbsoluteRect().getTop(); }

int Widget::getScreenLeft() const { return getAbsoluteRect().getLeft(); }

int Widget::getScreenBottom() const { return getAbsoluteRect().getBottom(); }

int Widget::getScreenRight() const { return getAbsoluteRect().getRight(); }

unsigned int Widget::getArea() const { return getAbsoluteRect().getArea(); }


Point Widget::convertLocalToScreen( const Point& localPoint ) const
{
	return localPoint + _d->absoluteRect.UpperLeftCorner;
}

Rect Widget::convertLocalToScreen( const Rect& localRect ) const
{
	return localRect + _d->absoluteRect.UpperLeftCorner;
}

void Widget::move( const Point& relativeMovement )
{
	setGeometry( _d->desiredRect + relativeMovement );
}

int Widget::getBottom() const
{
    return _d->relativeRect.LowerRightCorner.getY();
}

void Widget::setTabGroup( bool isGroup )
{
    _isTabGroup = isGroup;
}

void Widget::setWidth( unsigned int width )
{
    const Rect rectangle( getRelativeRect().UpperLeftCorner, Size( width, getHeight() ) );
    setGeometry( rectangle );
}

void Widget::setHeight( unsigned int height )
{
	const Rect rectangle( getRelativeRect().UpperLeftCorner, Size( getWidth(), height ) );
	setGeometry( rectangle );
}

void Widget::setLeft( int newLeft )
{
    setPosition( Point( newLeft, getTop() ) );    
}

void Widget::setTop( int newTop )
{
	setPosition( Point( getLeft(), newTop ) );    
}

int Widget::getTop() const
{
    return getRelativeRect().UpperLeftCorner.getY();
}

int Widget::getLeft() const
{
    return getRelativeRect().UpperLeftCorner.getX();
}

int Widget::getRight() const
{
    return getRelativeRect().LowerRightCorner.getX();
}

void Widget::hide()
{
	setVisible( false );
}

void Widget::show()
{
	setVisible( true );
}

TypeAlign Widget::getHorizontalTextAlign() const
{
	return _d->textHorzAlign;
}

TypeAlign Widget::getVerticalTextAlign() const
{
	return _d->textVertAlign;
}

void Widget::deleteLater()
{
	_environment->deleteLater( this ); 
}