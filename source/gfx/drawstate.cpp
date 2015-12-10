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

#include "drawstate.hpp"
#include "engine.hpp"
#include "batch.hpp"

namespace gfx
{

DrawState::DrawState(Engine& painter)
  : _painter( painter )
{

}

DrawState::DrawState(Engine& painter, const Point& lefttop, Rect* clip)
  :  _painter( painter ), _lefttop( lefttop ), _clip( clip )
{

}

DrawState& DrawState::draw(const Picture& picture)
{
  _ok = picture.isValid();
  if( _ok )
    _painter.draw( picture, _lefttop, _clip );

  return *this;
}

DrawState&DrawState::draw(const Picture& picture, const Point& offset)
{
  _ok = picture.isValid();
  if( _ok )
    _painter.draw( picture, _lefttop+offset, _clip );

  return *this;
}

DrawState& DrawState::draw(const Batch& batch)
{
  _ok = batch.valid();
  if( _ok )
    _painter.draw( batch, _clip );

  return *this;
}

DrawState& DrawState::fallback(const Batch& batch)
{
  if( !_ok )
  {
    _ok = batch.valid();
    if( _ok )
      _painter.draw( batch, _clip );
  }

  return *this;
}

DrawState& DrawState::fallback(const Pictures& pics)
{
  if( !_ok )
  {
    _ok = !pics.empty();
    if( _ok )
      _painter.draw( pics, _lefttop, _clip );
  }

  return *this;
}


}//end namespace gfx
