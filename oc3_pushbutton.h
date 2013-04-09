#ifndef __OPENCAESAR3_PUSHBUTTON_H_INCLUDE_
#define __OPENCAESAR3_PUSHBUTTON_H_INCLUDE_

#include "oc3_widget.h"
#include "oc3_signals.h"

class Picture;
class Font;

enum ElementState
{
    stNormal=0, 
    stPressed, 
    stHovered, 
    stDisabled, 
    StateCount
};

//Usual button class
class PushButton : public Widget
{
public:

	//! constructor
	PushButton( Widget* parent,
			    const Rect& rectangle,
                const std::string& caption="",
                int id=-1, 
			    bool noclip=false );

	//! destructor
	~PushButton();

	//! called if an event happened.
	bool onEvent(const NEvent& event);

    //! prepare render state
    void beforeDraw( GfxEngine& painter );

    //! override render function 
    void draw( GfxEngine& painter );

    void setText(const std::string& text );

    bool isPushButton() const;

    void setPressed( bool pressed );

    bool isPressed() const;

    bool isBodyVisible() const;

    void drawIcon( GfxEngine& painter );

    void setPicture( Picture* picture, ElementState state );

    void setFont( const Font& font, ElementState state );

    void setFont( const Font& font );

    Font& getFont( ElementState state );

    void setIsPushButton( bool value );

public oc3_signals:
    virtual Signal0<>& onClicked(); 

protected:

    //! when left mouse button pressed down
	bool leftMouseBtnPressed_( const NEvent& event );

    //! when left mouse button left up
	bool btnMouseUp_( const NEvent& event );

    //! when some mouse button clicked
	void btnClicked_();

	virtual ElementState getActiveButtonState_();
    virtual void _updateTexture( ElementState state );

    Picture* _getPicture( ElementState state );
private:
    class Impl;
    std::auto_ptr< Impl > _d;
};

#endif
