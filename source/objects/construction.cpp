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
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "construction.hpp"

#include "gfx/tile.hpp"
#include "gfx/tilemap.hpp"
#include "city/city.hpp"
#include "events/disaster.hpp"
#include "core/variant_list.hpp"
#include "core/logger.hpp"
#include "core/foreach.hpp"
#include "core/utils.hpp"
#include "extension.hpp"
#include "gfx/tilearea.hpp"
#include "core/json.hpp"
#include "core/flowlist.hpp"
#include "core/common.hpp"

using namespace gfx;
using namespace events;

class Params : public std::map<Param, double>
{
public:
  VariantList save() const
  {
    VariantList ret;
    for( auto item : *this )
      ret.push_back( VariantList( item.first, item.second ) );

    return ret;
  }

  void load( const VariantList& stream )
  {
    for( auto item : stream )
    {
      const VariantList& vl = item.toList();
      Param param = (Param)vl.get( 0 ).toInt();
      double value = vl.get( 1, 0.f ).toDouble();
      (*this)[ param ] = value;
    }
  }
};

class Extensions : public FlowList<ConstructionExtension>
{
public:
  VariantMap save() const
  {
    VariantMap ret;
    int extIndex = 0;
    for( auto ext : *this )
    {
      VariantMap vmExt;
      ext->save( vmExt );
      ret[ utils::i2str( extIndex++ ) ] = vmExt;
    }

    return ret;
  }

  void load( const VariantMap& stream )
  {
    for( auto item : stream )
    {
      ConstructionExtensionPtr extension = ExtensionsFactory::instance().create( item.second.toMap() );
      if( extension.isValid() )
      {
        this->push_back( extension );
      }
      else
      {
        Logger::warning( "Construction: cant load extension from " + Json::serialize( item.second, " " ) );
      }
    }
  }
};

class Construction::Impl
{
public:
  TilesArray accessRoads;
  Params states;
  Extensions extensions;
};

Construction::Construction(const object::Type type, const Size& size)
  : Overlay( type, size ), _d( new Impl )
{
  _d->states[ pr::fire ] = 0;
  _d->states[ pr::damage ] = 0;
}

TilesArray& Construction::_roadside() { return _d->accessRoads; }

void Construction::_checkDestroyState()
{
  if( state( pr::damage ) >= 100 )
  {
    collapse();
  }
  else if( state( pr::fire ) >= 100 )
  {
    burn();
  }
}

bool Construction::canBuild(const city::AreaInfo& areaInfo) const
{
  Tilemap& tilemap = areaInfo.city->tilemap();

  //return area for available tiles
  TilesArea area( tilemap, areaInfo.pos, size() );
  area = area.select( Tile::isConstructible );

  //on over map size
  if( (int)area.size() != size().area() )
    return false;  

  return true;
}

std::string Construction::troubleDesc() const
{
  if( isNeedRoad() && roadside().empty() )
  {
    return "##trouble_need_road_access##";
  }

  int lvlTrouble = 0;
  int damage = state( pr::damage );
  int fire = state( pr::fire );

  if( fire > 50 || damage > 50 )
  {
    const char* troubleName[] = { "some", "have", "most" };
    lvlTrouble = std::max( fire, damage );
    const char* typelvl = ( fire > damage ) ? "fire" : "damage";
    return utils::format( 0xff, "##trouble_%s_%s##", troubleName[ (int)((lvlTrouble-50) / 25) ], typelvl );
  }

  return "";
}

TilesArray Construction::roadside() const { return _d->accessRoads; }
void Construction::destroy() { Overlay::destroy(); }
bool Construction::isNeedRoad() const{ return true; }
Construction::~Construction() {}

bool Construction::build( const city::AreaInfo& info )
{
  Overlay::build( info );

  std::string name =  utils::format( 0xff, "%s_%d_%d",
                                     object::toString( type() ).c_str(),
                                     info.pos.i(), info.pos.j() );
  setName( name );

  computeRoadside();
  return true;
}

// here the problem lays: if we remove road, it is left in _accessRoads array
// also we need to recompute _accessRoads if we place new road tile
// on next to this road tile buildings
void Construction::computeRoadside()
{
  _d->accessRoads.clear();
  if( !_masterTile() )
      return;

  Tilemap& tilemap = _city()->tilemap();

  int s = size().width();
  for( int dst=1; dst <= roadsideDistance(); dst++ )
  {
    TilesArray tiles = tilemap.getRectangle( pos() + TilePos( -dst, -dst ),
                                             pos() + TilePos( s+dst-1, s+dst-1 ),
                                             !Tilemap::checkCorners );

    _d->accessRoads.append( tiles.select( Tile::tlRoad ) );
  }
}

int Construction::roadsideDistance() const{ return 1; }

void Construction::burn()
{
  deleteLater();

  GameEventPtr event = Disaster::create( tile(), Disaster::fire );
  event->dispatch();

  Logger::warning( "Construction catch fire at %d,%d!", pos().i(), pos().j() );
}

void Construction::collapse()
{
  if( isDeleted() )
    return;

  deleteLater();

  GameEventPtr event = Disaster::create( tile(), Disaster::collapse );
  event->dispatch();

  Logger::warning( "Construction collapsed at %d,%d!", pos().i(), pos().j() );
}

const Picture& Construction::picture() const { return Overlay::picture(); }

void Construction::setState( Param param, double value)
{
  _d->states[ param ] = math::clamp<double>( value, 0.f, 100.f );

  if( param == pr::damage || param == pr::fire )
  {
    _checkDestroyState();
  }
}

void Construction::updateState( Param name, double value)
{
  setState( name, state( name ) + value );
}

void Construction::save( VariantMap& stream) const
{
  Overlay::save( stream );
  VARIANT_SAVE_CLASS_D( stream, _d, extensions )
  VARIANT_SAVE_CLASS_D( stream, _d, states )
}

void Construction::load( const VariantMap& stream )
{
  Overlay::load( stream );
  VARIANT_LOAD_CLASS_D_LIST( _d, states, stream )
  VARIANT_LOAD_CLASS_D( _d, extensions, stream )
}

void Construction::addExtension(ConstructionExtensionPtr ext) { _d->extensions.postpone( ext ); }

ConstructionExtensionPtr Construction::getExtension(const std::string& name)
{
  return utils::findByName( _d->extensions, name );
}

const ConstructionExtensionList& Construction::extensions() const { return _d->extensions; }

void Construction::initialize(const MetaData& mdata)
{
  Overlay::initialize( mdata );

  VariantMap anMap = mdata.getOption( "animation" ).toMap();
  if( !anMap.empty() )
  {
    Animation anim;

    anim.load( anMap.get( "rc" ).toString(), anMap.get( "start" ).toInt(),
               anMap.get( "count" ).toInt(), anMap.get( "reverse", false ).toBool(),
               anMap.get( "step", 1 ).toInt() );

    Variant v_offset = anMap.get( "offset" );
    if( v_offset.isValid() )
    {
      anim.setOffset( v_offset.toPoint() );
    }

    anim.setDelay( (unsigned int)anMap.get( "delay", 1u ) );

    setAnimation( anim );
  }
}

double Construction::state(Param param) const { return _d->states[ param ]; }

TilesArray Construction::enterArea() const
{
  int s = size().width();
  TilesArray near = _city()->tilemap().getRectangle( pos() - TilePos(1, 1),
                                                     pos() + TilePos(s, s),
                                                     !Tilemap::checkCorners );

  return near.walkables( true );
}

void Construction::timeStep(const unsigned long time)
{  
  for( auto ext : _d->extensions )
    ext->timeStep( this, time );

  utils::eraseIfDeleted( _d->extensions );

  _d->extensions.merge();

  Overlay::timeStep( time );
}

const Picture& Construction::picture(const city::AreaInfo& areaInfo) const
{
  return Overlay::picture();
}

