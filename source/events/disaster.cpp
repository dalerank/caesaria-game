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
#include "city/statistic.hpp"
#include "objects/house_spec.hpp"
#include "playsound.hpp"
#include "objects/objects_factory.hpp"
#include "dispatcher.hpp"
#include "core/gettext.hpp"
#include "objects/house_level.hpp"
#include "objects/house.hpp"
#include "objects/ruins.hpp"
#include "core/utils.hpp"
#include "gfx/tilesarray.hpp"
#include "city/cityservice_peace.hpp"
#include "build.hpp"
#include "core/foreach.hpp"
#include "gfx/tilearea.hpp"

using namespace gfx;
using namespace city;

namespace events
{

static const int houseOffset=1000;

class Disaster::Impl
{
public:
  TilePos pos;
  Disaster::Type type;
  int infoType;
};

GameEventPtr Disaster::create( const Tile& tile, Type type )
{
  Disaster* event = new Disaster();
  event->_d->pos = tile.pos();
  event->_d->type = type;
  event->_d->infoType = 0;

  OverlayPtr overlay = tile.overlay();
  if( overlay.isValid() )
  {
    overlay->deleteLater();
    HousePtr house = overlay.as<House>();
    if( house.isValid() )
    {
      event->_d->infoType = houseOffset + house->spec().level();
    }
    else
    {
      event->_d->infoType = overlay->type();
    }
  }

  GameEventPtr ret( event );
  ret->drop();

  return ret;
}

void Disaster::_exec( Game& game, unsigned int )
{
  Tilemap& tmap = game.city()->tilemap();
  Tile& tile = tmap.at( _d->pos );
  TilePos rPos = _d->pos;

  bool mayContinue = tile.getFlag( Tile::isDestructible );

  if( _d->type == Disaster::rift )
  {
    mayContinue = tile.getFlag( Tile::isConstructible );
    mayContinue |= is_kind_of<Construction>( tile.overlay() );
  }

  if( mayContinue )
  {
    Size size( 1 );

    OverlayPtr overlay = tile.overlay();
    if( overlay.isValid() )
    {
      overlay->deleteLater();
      rPos = overlay->pos();
      size = overlay->size();
    }    

    switch( _d->type )
    {
    case Disaster::collapse:
    {
      GameEventPtr e = PlaySound::create( "explode", rand() % 2, 100 );      
      e->dispatch();
    }
    break;

    default:
    break;
    }

    PeacePtr peaceSrvc = statistic::getService<Peace>( game.city() ) ;
    if( peaceSrvc.isValid() )
    {
      peaceSrvc->buildingDestroyed( overlay, _d->type );
    }

    TilesArea clearedTiles( tmap, rPos, size );
    foreach( tile, clearedTiles )
    {
      bool needBuildRuins = !( _d->type == Disaster::rift && (*tile)->pos() == _d->pos );

      OverlayPtr currentTileOverlay;
      if( needBuildRuins )
      {
        object::Type dstr2constr[] = { object::burning_ruins, object::collapsed_ruins,
                                       object::plague_ruins, object::collapsed_ruins,
                                       object::collapsed_ruins };

        currentTileOverlay = TileOverlayFactory::instance().create( dstr2constr[_d->type] );

        RuinsPtr ruins = currentTileOverlay.as<Ruins>();
        if( ruins.isValid() )
        {
          std::string typev = _d->infoType > houseOffset
                                ? utils::format( 0xff, "house%02d", _d->infoType - houseOffset )
                                : object::toString( object::Type( _d->infoType ) );
          ruins->setInfo( utils::format( 0xff, "##ruins_%s_text##", typev.c_str() ) );
          ruins->afterBuild();
        }
      }
      else
      {
        currentTileOverlay = TileOverlayFactory::instance().create( object::rift );

        //TilesArray tiles = game.city()->tilemap().getNeighbors(_pos, Tilemap::FourNeighbors);

        /*foreach( it, tiles )
        {
          ConstructionPtr c = ptr_cast<Construction>( (*it)->overlay() );
          if( c.isValid() )
          {
            c->burn();
          }
        }*/
      }

      Dispatcher::instance().append( BuildAny::create( (*tile)->pos(), currentTileOverlay ) );
    }

    std::string dstr2string[] = { "##alarm_fire_in_city##", "##alarm_building_collapsed##",
                                  "##alarm_plague_in_city##", "##alarm_earthquake##" };
    emit game.city()->onDisasterEvent()( _d->pos, _( dstr2string[_d->type] ) );
  }
}

bool Disaster::_mayExec(Game&, unsigned int) const{  return true;}

Disaster::Disaster() : _d( new Impl )
{
  _d->type =count;
  _d->infoType = 0;
}

} //end namespace events
