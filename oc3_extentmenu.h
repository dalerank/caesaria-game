#ifndef _OPENCAESAR3_EXTENTMENU_H_INCLUDE_
#define _OPENCAESAR3_EXTENTMENU_H_INCLUDE_

#include "oc3_widget.h"

class Picture;

class ExtentMenu;
typedef std::auto_ptr< ExtentMenu > ExtentMenuPtr;

class ExtentMenu : public Widget
{
public:
    static ExtentMenuPtr create( Widget* parent, const Rect& rect, int id );

    // draw on screen
    virtual void draw( GfxEngine& engine );
    virtual const Picture& getBgPicture() const;

    void minimize();

private:
    ExtentMenu( Widget* parent, const Rect& rect, int id );
    
    class Impl;
    std::auto_ptr< Impl > _d;
};


#endif