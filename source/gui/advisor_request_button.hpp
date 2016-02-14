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

#ifndef __CAESARIA_EMPEROR_REQUEST_BUTTON_H_INCLUDED__
#define __CAESARIA_EMPEROR_REQUEST_BUTTON_H_INCLUDED__

#include "pushbutton.hpp"
#include "city/request.hpp"

namespace gui
{

namespace advisorwnd
{

class RequestButton : public PushButton
{
public:
  RequestButton( Widget* parent, const Point& pos, int index, city::RequestPtr request );
  virtual void draw(gfx::Engine& painter);

protected:
  virtual void _updateTexture();

public signals:
  Signal1<city::RequestPtr>& onExecRequest();

private:
  void _acceptRequest();
  void _executeRequest();

  __DECLARE_IMPL(RequestButton)
};

}//end namespace advisorwnd

}//end namespace gui
#endif //__CAESARIA_EMPEROR_REQUEST_BUTTON_H_INCLUDED__
