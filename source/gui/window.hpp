#ifndef _CAESARIA_WINDOW_H_INCLUDE_
#define _CAESARIA_WINDOW_H_INCLUDE_

#include "widget.hpp"
#include "pushbutton.hpp"

namespace gui
{

class Window : public Widget
{
public:
	typedef enum { buttonClose=0, buttonMin, buttonMax, buttonCount } ButtonName;
	typedef enum { draggable=0x1, backgroundVisible=0x2, titleVisible=0x4 } FlagName;
	//! constructor
	Window( Widget* parent, const Rect& rectangle, const std::string& title, int id=-1 );

	//! destructor
	virtual ~Window();

	//! called if an event happened.
	virtual bool onEvent(const NEvent& event);

	//! draws the element and its children
	virtual void draw( gfx::Engine& painter );

	//! Returns pointer to the close button
	virtual PushButton* getButton( ButtonName btn ) const;

	//!
	virtual void beforeDraw(gfx::Engine &painter);

	//! Set if the window background will be drawn
	virtual void setBackgroundVisible(bool draw);

	//! Get if the window background will be drawn
	virtual bool isBackgroundVisible() const;

	//! Set if the window titlebar will be drawn
	//! Note: If the background is not drawn, then the titlebar is automatically also not drawn
	virtual void setHeaderVisible(bool draw);

	//! Get if the window titlebar will be drawn
	virtual bool isHeaderVisible() const;

	virtual void setBackground( gfx::Picture texture );

	virtual gfx::Picture getBackground() const;

	virtual Rect getClientRect() const;

	virtual void setModal();

	void setWindowFlag( FlagName flag, bool enabled=true );

	void setTextAlignment( Alignment horizontal, Alignment vertical );

	void setText( const std::string& text );

protected:
	void _createSystemButton( ButtonName btnName, /*ELEMENT_STYLE_TYPE configName,*/ const std::string& tooltip, bool visible );
	void _init();

private:
	class Impl;
	ScopedPtr<Impl> _d;
};

}//end namespace gui

#endif
