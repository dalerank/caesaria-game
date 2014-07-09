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
#include "build.hpp"
#include "core/foreach.hpp"

using namespace constants;
using namespace gfx;

namespace events
{

GameEventPtr DisasterEvent::create( const Tile& tile, Type type )
{
  DisasterEvent* event = new DisasterEvent();
  event->_pos = tile.pos();
  event->_type = type;
  event->_infoType = 0;

  TileOverlayPtr overlay = tile.overlay();
  if( overlay.isValid() )
  {
    overlay->deleteLater();
    HousePtr house = ptr_cast< House >( overlay );
    if( house.isValid() )
    {
      event->_infoType = 1000 + house->spec().level();
    }
    else
    {
      event->_infoType = overlay->type();
    }
  }

  GameEventPtr ret( event );
  ret->drop();

  return ret;
}

void DisasterEvent::_exec( Game& game, unsigned int )
{
  Tilemap& tmap = game.city()->tilemap();
  Tile& tile = tmap.at( _pos );
  TilePos rPos = _pos;

  bool mayContinue = tile.getFlag( Tile::isDestructible );

  if( _type == DisasterEvent::rift )
  {
    mayContinue = tile.getFlag( Tile::isConstructible );
    mayContinue |= is_kind_of<Construction>( tile.overlay() );
  }

  if( mayContinue )
  {
    Size size( 1 );

    TileOverlayPtr overlay = tile.overlay();
    if( overlay.isValid() )
    {
      overlay->deleteLater();
      rPos = overlay->pos();
      size = overlay->size();
    }

    switch( _type )
    {
    case DisasterEvent::collapse:
    {
      GameEventPtr e = PlaySound::create( "explode", rand() % 2, 100 );      
      e->dispatch();
    }
    break;

    default:
    break;
    }

    TilesArray clearedTiles = tmap.getArea( rPos, size );
    foreach( tile, clearedTiles )
    {
      bool needBuildRuins = !( _type == DisasterEvent::rift && (*tile)->pos() == _pos );      

      TileOverlayPtr ov;
      if( needBuildRuins )
      {
        TileOverlay::Type dstr2constr[] = { building::burningRuins, building::collapsedRuins,
                                            building::plagueRuins, building::collapsedRuins };

        ov = TileOverlayFactory::instance().create( dstr2constr[_type] );

        if( ov.isValid() )
        {
          SmartPtr< Ruins > ruins = ptr_cast< Ruins >( ov );
          if( ruins.isValid() )
          {
            std::string typev = _infoType > 1000
                                  ? StringHelper::format( 0xff, "house%02d", _infoType - 1000 )
                                  : MetaDataHolder::findTypename( _infoType );
            ruins->setInfo( StringHelper::format( 0xff, "##ruins_%04d_text##", typev.c_str() ) );
            ruins->afterBuild();
          }
        }
      }
      else
      {
        ov = TileOverlayFactory::instance().create( building::rift );

        TilesArray tiles = game.city()->tilemap().getNeighbors(_pos, Tilemap::EdgeNeighbors);

        /*foreach( it, tiles )
        {
          ConstructionPtr c = ptr_cast<Construction>( (*it)->overlay() );
          if( c.isValid() )
          {
            c->burn();
          }
        }*/
      }

      Dispatcher::instance().append( BuildEvent::create( (*tile)->pos(), ov ) );
    }

    std::string dstr2string[] = { "##alarm_fire_in_city##", "##alarm_building_collapsed##",
                                  "##alarm_plague_in_city##", "##alarm_earthquake##" };
    oc3_emit game.city()->onDisasterEvent()( _pos, _( dstr2string[_type] ) );
  }
}

bool DisasterEvent::_mayExec(Game&, unsigned int) const{  return true;}

DisasterEvent::DisasterEvent() : _type( count ),_infoType( 0 )
{}

} //end namespace events
