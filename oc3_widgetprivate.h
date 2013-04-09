#ifndef __OPENCAESAR3_WIDGET_PRIVATE_H_INCLUDE_
#define __OPENCAESAR3_WIDGET_PRIVATE_H_INCLUDE_

#include "oc3_widget.h"

//typedef core::Map< s32, f32 > OpacityMap;

/*struct _OverrideColor
{
	Color color;
	bool  enabled;

	_OverrideColor( const Color& c, bool e)
	{
		color = c;
		enabled = e;
	}
};*/

//typedef core::Map< u32, _OverrideColor > ColorMap;

class WidgetPrivate
{
public:
	
	//FontsMap overrideFonts;
	//OpacityMap opacity;
	//ColorMap overrideColors;
	//ElementStyle* style;

    //! Pointer to the parent
    Widget* parent;

	//! List of all children of this element
	Widget::Widgets children;

	//! relative rect of element
	Rect relativeRect;

	//! absolute rect of element
	Rect absoluteRect;

	//! absolute clipping rect of element
	Rect absoluteClippingRect;

	//! the rectangle the element would prefer to be,
	//! if it was not constrained by parent or max/min size
	Rect desiredRect;

	//! for calculating the difference when resizing parent
	Rect lastParentRect;

	//! relative scale of the element inside its parent
	//RectF scaleRect;

	TypeAlign textHorzAlign, textVertAlign;

    std::string internalName;
};

#endif