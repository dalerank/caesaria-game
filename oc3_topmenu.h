#ifndef __OPENCAESAR3_TOPMENU_H_INCLUDE_
#define __OPENCAESAR3_TOPMENU_H_INCLUDE_

#include "oc3_widget.h"

class TopMenu : public Widget
{
public:
    static TopMenu* create( Widget* parent, const int height );

    // draw on screen
    virtual void draw( GfxEngine& engine );

    void setFunds( int value );
    void setPopulation( int value );
    void setDate( int value );

private:

    TopMenu( Widget* parent, const int height );
    
    class Impl;
    std::auto_ptr< Impl > _d;
};

#endif //__OPENCAESAR3_TOPMENU_H_INCLUDE_