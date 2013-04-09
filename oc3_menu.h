#ifndef __OPENCAESAR3_MENU_H_INCLUDE_
#define __OPENCAESAR3_MENU_H_INCLUDE_

#include "oc3_widget.h"

class Menu;
typedef std::auto_ptr< Menu > MenuPtr;

class Menu : public Widget
{
public:
    static MenuPtr create( Widget* parent, int id );

    // draw on screen
    virtual void draw( GfxEngine& engine );

    void unselect();

protected:
    class Impl;
    std::auto_ptr< Impl > _d;

    Menu( Widget* parent, int id, const Rect& rectangle );

};

#endif