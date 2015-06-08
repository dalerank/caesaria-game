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

#include "showlayer.hpp"
#include "game/game.hpp"
#include "core/logger.hpp"
#include "factory.hpp"
#include "layers/constants.hpp"
#include "scene/level.hpp"
#include "core/variant_map.hpp"

namespace events
{

REGISTER_EVENT_IN_FACTORY(ShowLayer, "switch2layer")

GameEventPtr ShowLayer::create()
{
  GameEventPtr ret( new ShowLayer() );
  ret->drop();
  return ret;
}

GameEventPtr ShowLayer::create(bool show, int layer)
{
  ShowLayer* ev = new ShowLayer();
  ev->_show = show;
  ev->_layer = layer;
  GameEventPtr ret( ev );
  ret->drop();
  return ret;
}

void ShowLayer::load(const VariantMap &stream)
{
  GameEvent::load( stream );

  _show = stream.get( "show" );
  Variant lvayer = stream.get( "layer" );

  if( lvayer.type() == Variant::String ) { _layer = citylayer::Helper::instance().findType( lvayer.toString() ); }
  else { _layer = lvayer.toInt(); }
}

bool ShowLayer::_mayExec(Game& game, unsigned int time) const {  return true; }

ShowLayer::ShowLayer()
  : _show( false ), _layer( citylayer::simple )
{
}

void ShowLayer::_exec(Game& game, unsigned int)
{
  if( _show )
  {
    scene::Level* level = safety_cast<scene::Level*>( game.scene() );
    if( level )
      level->switch2layer( _layer );
  }
  else
  {
  }
}

} //end namespace events
