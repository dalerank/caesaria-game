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
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com
// Copyright 2012 Dalerank, dalerankn7@gmail.com


#include "construction.hpp"

#include "gfx/tile.hpp"
#include "gfx/tilemap.hpp"
#include "city/city.hpp"
#include "events/disaster.hpp"
#include "core/logger.hpp"
#include "core/foreach.hpp"
#include "core/stringhelper.hpp"
#include "extension.hpp"

using namespace gfx;

class Construction::Impl
{
public:
  typedef std::map<int, double> Params;
  TilesArray accessRoads;
  Params params;
  ConstructionExtensionList extensions;
};

Construction::Construction(const Type type, const Size& size)
  : TileOverlay( type, size ), _d( new Impl )
{
  _d->params[ fire ] = 0;
  _d->params[ damage ] = 0;
}

bool Construction::canBuild(PlayerCityPtr city, TilePos pos , const TilesArray& ) const
{
  Tilemap& tilemap = city->tilemap();

  bool is_constructible = true;

  //return area for available tiles
  TilesArray area = tilemap.getArea( pos, size() );

  //on over map size
  if( (int)area.size() != size().area() )
    return false;

  foreach( tile, area ) {is_constructible &= (*tile)->getFlag( Tile::isConstructible );}

  return is_constructible;
}

std::string Construction::troubleDesc() const
{
  if( isNeedRoadAccess() && getAccessRoads().empty() )
  {
    return "##trouble_need_road_access##";
  }

  int lvlTrouble = 0;
  int damage = getState( Construction::fire );
  int fire = getState( Construction::damage );

  if( fire > 50 || damage > 50 )
  {
    const char* troubleName[] = { "some", "have", "most" };
    lvlTrouble = std::max( fire, damage );
    const char* typelvl = ( fire > damage ) ? "fire" : "damage";
    return StringHelper::format( 0xff, "##trouble_%s_%s##", troubleName[ (int)floor((lvlTrouble-50) / 25) ], typelvl );
  }

  return "";
}

std::string Construction::errorDesc() const { return ""; }
TilesArray Construction::getAccessRoads() const {   return _d->accessRoads; }
bool Construction::canDestroy() const {  return true; }
void Construction::destroy() { TileOverlay::destroy(); }
bool Construction::isNeedRoadAccess() const{  return true; }
const Picture& Construction::picture() const {  return TileOverlay::picture(); }
Construction::~Construction() {}

void Construction::build(PlayerCityPtr city, const TilePos& pos )
{
  TileOverlay::build( city, pos );

  computeAccessRoads();
}

// here the problem lays: if we remove road, it is left in _accessRoads array
// also we need to recompute _accessRoads if we place new road tile
// on next to this road tile buildings
void Construction::computeAccessRoads()
{
  _d->accessRoads.clear();
  if( !_masterTile() )
      return;

  Tilemap& tilemap = _city()->tilemap();

  int s = size().width();
  for( int dst=1; dst <= roadAccessDistance(); dst++ )
  {
    TilesArray rect = tilemap.getRectangle( pos() + TilePos( -dst, -dst ),
                                            pos() + TilePos( s+dst-1, s+dst-1 ),
                                            !Tilemap::checkCorners );
    foreach( tile, rect )
    {
      if( (*tile)->getFlag( Tile::tlRoad ) )
      {
        _d->accessRoads.push_back( *tile );
      }
    }
  }
}

int Construction::roadAccessDistance() const{  return 1; }

void Construction::burn()
{
  deleteLater();

  events::GameEventPtr event = events::DisasterEvent::create( tile(), events::DisasterEvent::fire );
  event->dispatch();

  Logger::warning( "Building catch fire at %d,%d!", pos().i(), pos().j() );
}

void Construction::collapse()
{
  deleteLater();

  events::GameEventPtr event = events::DisasterEvent::create( tile(), events::DisasterEvent::collapse );
  event->dispatch();

  Logger::warning( "Building collapsed at %d,%d!", pos().i(), pos().j() );
}

void Construction::setState( ParameterType param, double value)
{
  _d->params[ param ] = math::clamp<double>( value, 0.f, 100.f );
}

void Construction::updateState(Construction::ParameterType name, double value)
{
  setState( name, getState( name ) + value );
}

void Construction::save( VariantMap& stream) const
{
  TileOverlay::save( stream );
  VariantList vl_states;
  foreach( it, _d->params )
  {
    vl_states.push_back( VariantList() << (int)it->first << (double)it->second );
  }

  stream[ "states" ] = vl_states;
}

void Construction::load( const VariantMap& stream )
{
  TileOverlay::load( stream );
  VariantList vl_states = stream.get( "states" ).toList();
  foreach( it, vl_states )
  {
    const VariantList& param = it->toList();
    _d->params[ (Construction::Param)param.get( 0 ).toInt() ] = param.get( 1, 0.f ).toDouble();
  }
}

void Construction::addExtension(ConstructionExtensionPtr ext)
{
  _d->extensions.push_back( ext );
}

double Construction::getState( ParameterType param) const { return _d->params[ param ]; }

TilesArray Construction::enterArea() const
{
  int s = size().width();
  TilesArray near = _city()->tilemap().getRectangle( pos() - TilePos(1, 1),
                                                                  pos() + TilePos(s, s),
                                                                  !Tilemap::checkCorners );  

  return near.walkableTiles( true );
}

void Construction::timeStep(const unsigned long time)
{
  if( getState( Construction::damage ) >= 100 )
  {    
    collapse();
  }
  else if( getState( Construction::fire ) >= 100 )
  {
    burn();
  }

  for( ConstructionExtensionList::iterator it=_d->extensions.begin();
       it != _d->extensions.end(); )
  {
    (*it)->run( this, time );

    if( (*it)->isDeleted() ) { it = _d->extensions.erase( it ); }
    else { ++it; }
  }

  TileOverlay::timeStep( time );
}

const Picture& Construction::picture(PlayerCityPtr city, TilePos pos, const TilesArray& aroundTiles) const
{
  return picture();
}

