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

#include "disaster.hpp"
#include "game/game.hpp"
#include "gfx/tilemap.hpp"
#include "city/city.hpp"
#include "playsound.hpp"
#include "objects/objects_factory.hpp"
#include "dispatcher.hpp"
#include "core/gettext.hpp"
#include "objects/house_level.hpp"
#include "objects/house.hpp"
#include "objects/ruins.hpp"
#include "core/stringhelper.hpp"
#include "gfx/tilesarray.hpp"
#include "core/foreach.hpp"

using namespace constants;

namespace events
{

GameEventPtr DisasterEvent::create( const Tile& tile, Type type )
{
  DisasterEvent* event = new DisasterEvent();
  event->_pos = tile.pos();
  event->_type = type;
  event->_infoType = 0;

  TileOverlayPtr overlay = tile.getOverlay();
  if( overlay.isValid() )
  {
    overlay->deleteLater();
    HousePtr house = ptr_cast< House >( overlay );
    if( house.isValid() )
    {
      event->_infoType = 1000 + house->getSpec().getLevel();
    }
    else
    {
      event->_infoType = overlay->getType();
    }
  }

  GameEventPtr ret( event );
  ret->drop();

  return ret;
}

void DisasterEvent::exec( Game& game )
{
  Tilemap& tmap = game.getCity()->getTilemap();
  Tile& tile = tmap.at( _pos );
  TilePos rPos = _pos;

  if( tile.getFlag( Tile::isDestructible ) )
  {
    Size size( 1 );

    TileOverlayPtr overlay = tile.getOverlay();
    if( overlay.isValid() )
    {
      overlay->deleteLater();
      rPos = overlay->getTilePos();
      size = overlay->getSize();
    }

    switch( _type )
    {
    case DisasterEvent::collapse:
    {
      GameEventPtr e = PlaySound::create( "explode", rand() % 2, 256 );
      e->dispatch();
    }
    break;

    default:
    break;
    }

    TilesArray clearedTiles = tmap.getArea( rPos, size );
    foreach( tile, clearedTiles )
    {
      TileOverlay::Type dstr2constr[] = { building::burningRuins, building::collapsedRuins, building::plagueRuins };
      TileOverlayPtr ov = TileOverlayFactory::getInstance().create( dstr2constr[_type] );
      if( ov.isValid() )
      {
        SmartPtr< Ruins > ruins = ptr_cast< Ruins >( ov );
        if( ruins.isValid() )
        {
          ruins->setInfo( StringHelper::format( 0xff, "##ruins_%04d_text##", _infoType ) );
        }

        Dispatcher::instance().append( BuildEvent::create( (*tile)->pos(), ov ) );
      }
    }

    std::string dstr2string[] = { _("##alarm_fire_in_city##"), _("##alarm_building_collapsed##"),
                                  _("##alarm_plague_in_city##") };
    game.getCity()->onDisasterEvent().emit( _pos, dstr2string[_type] );
  }
}

} //end namespace events
