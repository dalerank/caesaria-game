#ifndef __NRP_POSITION_ATTRIBUTE_H_INCLUDED__
#define __NRP_POSITION_ATTRIBUTE_H_INCLUDED__

#include "EdAttributeRect.h"

NERPA_MODULE_BEGIN(gui)

class NrpPositionAttribute : public AttributeRect
{
public:
    //
    NrpPositionAttribute( Widget* parent, s32 myParentID ) :
        AttributeRect( parent, myParentID)
    {
    }

	virtual AbstractAttribute* GetChildAttribute( u32 index ) 
	{
		if( index >= GetChildAttributesCount() )
			return 0;

		String text[] = { L"\t\t\tX", L"\t\t\tY" };

		AttributeString* attr = findChild< AttributeString* >( text[ index ] );
		if( !attr )
		{
			attr = new AttributeString( this, -1 );
			attr->setAttributeName( text[ index ] );
			attr->setParent4Event( this );
			attr->setEditText( tokens_[ index ] );
		}

		return attr; 
	}

	virtual u32 GetChildAttributesCount() const 
	{ 
		return 2; 
	}

    //! Returns the type name of the gui element.
    virtual String getTypeName() const
    {
        return EditorElementTypeNames[EGUIEDIT_POSITIONATTRIBUTE];
    }
};

NERPA_MODULE_END(gui)

#endif
