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

#ifndef __CAESARIA_ADVISOR_RELIGION_INFO_H_INCLUDED__
#define __CAESARIA_ADVISOR_RELIGION_INFO_H_INCLUDED__

#include "label.hpp"
#include "gfx/predefinitions.hpp"
#include "religion/romedivinity.hpp"
#include "game/predefinitions.hpp"

namespace gui
{

namespace advisorwnd
{

class ReligionInfoLabel : public Label
{
public:
  ReligionInfoLabel( Widget* parent, const Rect& rect, religion::DivinityPtr divinity,
                     int smallTempleCount, int bigTempleCount  );

  virtual void draw( gfx::Engine &painter);

protected:
  virtual void _updateTexture( gfx::Engine& painter );

private:
  __DECLARE_IMPL(ReligionInfoLabel)
};

}

}//end namespace gui
#endif //__CAESARIA_ADVISOR_RELIGION_INFO_H_INCLUDED__
