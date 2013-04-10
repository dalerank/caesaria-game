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


#ifndef __OPENCAESAR3_EXTENTMENU_H_INCLUDE_
#define __OPENCAESAR3_EXTENTMENU_H_INCLUDE_

#include "oc3_widget.h"
#include "oc3_scopedptr.h"

class Picture;

class ExtentMenu : public Widget
{
public:
    static ExtentMenu* create( Widget* parent, const Rect& rect, int id );

    // draw on screen
    virtual void draw( GfxEngine& engine );
    virtual const Picture& getBgPicture() const;

    void minimize();

private:
    ExtentMenu( Widget* parent, const Rect& rect, int id );
    
    class Impl;
    ScopedPtr< Impl > _d;
};


#endif