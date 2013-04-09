#ifndef _OPENCAESAR3_MENURIGTHPANEL_H_INCLUDE_
#define _OPENCAESAR3_MENURIGTHPANEL_H_INCLUDE_

#include "oc3_widget.h"

class MenuRigthPanel;
typedef std::auto_ptr< MenuRigthPanel > MenuRigthPanelPtr;

class Picture;

class MenuRigthPanel : public Widget
{
public:
    static MenuRigthPanelPtr create( Widget* parent, const Rect& rectangle, const Picture& tilePic );
        
    void draw( GfxEngine& engine );
private:
    class Impl;
    std::auto_ptr< Impl > _d;

    MenuRigthPanel( Widget* parent );
};

#endif