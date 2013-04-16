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


#ifndef __OPENCAESAR3_PUSHBUTTON_H_INCLUDE_
#define __OPENCAESAR3_PUSHBUTTON_H_INCLUDE_

#include "oc3_widget.hpp"
#include "oc3_signals.hpp"

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

    void resizeEvent_();

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
