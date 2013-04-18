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


#ifndef __OPENCAESAR3_GUIENVIRONMENT_INCLUDE_
#define __OPENCAESAR3_GUIENVIRONMENT_INCLUDE_

#include "oc3_widget.hpp"
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
    Point getCursorPos() const;

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
