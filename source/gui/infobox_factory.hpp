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

#ifndef _CAESARIA_INFOBOXFACTORY_H_INCLUDE_
#define _CAESARIA_INFOBOXFACTORY_H_INCLUDE_

#include "infobox_construction.hpp"
#include "objects/factory.hpp"

namespace gui
{

// info box about a factory building
class InfoboxFactory : public InfoboxConstruction
{
public:
  InfoboxFactory( Widget* parent, const gfx::Tile& tile );

  virtual void showDescription();
private:
  gfx::TileOverlay::Type _type;
};

class InfoboxShipyard : public InfoboxFactory
{
public:
  InfoboxShipyard( Widget* parent, const gfx::Tile& tile );
};

class InfoboxWharf : public InfoboxFactory
{
public:
  InfoboxWharf( Widget* parent, const gfx::Tile& tile );
};


}//end namespace gui
#endif //_CAESARIA_INFOBOXFACTORY_H_INCLUDE_
