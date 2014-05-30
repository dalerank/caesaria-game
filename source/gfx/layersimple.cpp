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

#include "layersimple.hpp"
#include "layerconstants.hpp"
#include "walker/constants.hpp"
#include "city_renderer.hpp"
#include "city/city.hpp"

using namespace constants;

namespace gfx
{

int LayerSimple::type() const
{
  return citylayer::simple;
}

std::set<int> LayerSimple::getVisibleWalkers() const
{
  std::set<int> ret;
  ret.insert( walker::all );

  return ret;
}

LayerPtr LayerSimple::create( Camera& camera, PlayerCityPtr city)
{
  LayerPtr ret( new LayerSimple( camera, city ) );
  ret->drop();

  return ret;
}

LayerSimple::LayerSimple( Camera& camera, PlayerCityPtr city)
  : Layer( &camera, city )
{
}

}//end namespace gfx
