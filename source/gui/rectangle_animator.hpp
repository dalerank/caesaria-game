// This file is part of CaesarIA.
//
// CaesarIA is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CaesarIA is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CaesarIA.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#ifndef _CAESARIA_RECTANIMATOR_H_INCLUDE_
#define _CAESARIA_RECTANIMATOR_H_INCLUDE_

#include "widgetanimator.hpp"
#include "core/signals.hpp"

namespace gui
{

class RectangleAnimator : public WidgetAnimator
{
public:
  RectangleAnimator( Widget* node,
                     const Rect& startRect,
                     const Rect& stopRect,
                     unsigned int time,
                     int flags=showParent|removeSelf );

  ~RectangleAnimator(void);

  void beforeDraw( gfx::Engine& painter);
 
  void setStopRect( const Rect& r );

	unsigned int getTime() const;
	void setTime( unsigned int t );
	void setStartRect( const Rect& r );

public signals:
	virtual Signal0<>& onFinished();

protected:
	virtual void updateFinished_();
	virtual void restart();

private:
	class Impl;
	ScopedPtr<Impl> _d;
};

}

#endif //_CAESARIA_RECTANIMATOR_H_INCLUDE_
