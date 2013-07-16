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

#ifndef __OPENCAESAR3_WIDGETANIMATOR_H_INCLUDE_
#define __OPENCAESAR3_WIDGETANIMATOR_H_INCLUDE_

#include "oc3_widget.hpp"
#include "oc3_flagholder.hpp"

#define ANIMATOR_UNUSE_VALUE -9999

class WidgetAnimator : public Widget, public FlagHolder<int>
{
public:
	enum { showParent=0x1,
		   removeSelf=0x2, removeParent=0x4,
		   debug=0x8,
		   isActive=0x10 };

    WidgetAnimator( Widget* parent, int flags );

    //! Деструктор
    virtual ~WidgetAnimator(void);

    void beforeDraw( GfxEngine& painter );

protected:
    virtual void afterFinished_();
};

#endif
