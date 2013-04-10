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


#ifndef __OPENCAESAR3_MENU_H_INCLUDE_
#define __OPENCAESAR3_MENU_H_INCLUDE_

#include "oc3_widget.h"
#include "oc3_signals.h"
#include "oc3_scopedptr.h"

class Menu : public Widget
{
public:
    static Menu* create( Widget* parent, int id );

    // draw on screen
    virtual void draw( GfxEngine& engine );

    bool onEvent(const NEvent& event);

    bool unselectAll();

oc3_signals public:
    Signal1< int >& onCreateConstruction();
    Signal0<>& onRemoveTool();
    Signal0<>& onMaximize();

protected:
    class Impl;
    ScopedPtr< Impl > _d;

    Menu( Widget* parent, int id, const Rect& rectangle );
    void _createBuildMenu( int type, Widget* parent );
};

class ExtentMenu : public Menu
{
public:
    static ExtentMenu* create( Widget* parent, int id );

    void minimize();
    void maximize();
protected:
    ExtentMenu( Widget* parent, int id, const Rect& rectangle );
};

#endif