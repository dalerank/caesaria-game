#ifndef __OPENCAESAR3_GUIENVIRONMENT_INCLUDE_
#define __OPENCAESAR3_GUIENVIRONMENT_INCLUDE_

#include "oc3_widget.h"
#include <memory>

class GfxEngine;

class GuiEnv : Widget
{
public:
    GuiEnv( GfxEngine& painter );

    ~GuiEnv();

    bool hasFocus( const Widget* element) const;
    bool setFocus( Widget* element);
    bool removeFocus( Widget* element);

    double getVersion() const;
	
    Widget* getRootWidget();								//  
    Widget* getFocus() const;

    bool isHovered( const Widget* element );
    Widget* getHoveredElement() const;

    void draw();
    void beforeDraw();

    void animate( unsigned int time );

    bool handleEvent(const NEvent& event);

    virtual void deleteLater( Widget* ptrElement );

    void clear();
   
private:
    Widget* createStandartTooltip_();
    
    void drawTooltip_( unsigned int time );
    void updateHoveredElement( const Point& mousePos);
    Widget* getNextWidget(bool reverse, bool group);

    Widget* _CheckParent( Widget* parent );
  
    void threatDeletionQueue_();

    class Impl;
    std::auto_ptr< Impl > _d;
};

#endif
