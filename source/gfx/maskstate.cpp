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

#include "maskstate.hpp"
#include "engine.hpp"

namespace gfx
{

MaskState::MaskState(Engine& painter, NColor color)
  : _painter( painter ), _color( color )
{
  if( _color.color != 0 )
    _painter.setColorMask( _color.red() << 16, _color.green() << 8,
                           _color.blue(),      _color.alpha() << 24 );
}

MaskState::~MaskState()
{
  if( _color.color != 0 )
    _painter.resetColorMask();
}

}//end namespace gfx
