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

#include "overlay.hpp"
#include "gfx/tilemap_config.hpp"
#include "objects/infodb.hpp"
#include "city/city.hpp"
#include "gfx/tilesarray.hpp"
#include "gfx/tilemap.hpp"
#include "core/variant_map.hpp"
#include "core/variant_list.hpp"
#include "objects_factory.hpp"
#include "core/logger.hpp"

using namespace gfx;

namespace {
static Renderer::PassQueue defaultPassQueue=Renderer::PassQueue(1,Renderer::overlayAnimation);
static Pictures invalidPictures;
static SimpleLogger LOG_OVERLAY(TEXT(Overlay));
}


class Overlay::Impl
{
public:  
  Pictures fgPictures;
  object::Type overlayType;
  object::Group overlayClass;
  Tile* masterTile;  // left-most tile if multi-tile, or "this" if single-tile
  std::string name;
  Picture picture;
  Size size;  // size in tiles
  Animation animation;  // basic animation (if any)
  bool isDeleted;
  PlayerCityPtr city;
};

OverlayPtr Overlay::create(object::Type type)
{
  return TileOverlayFactory::instance().create( type );
}

Overlay::Overlay(object::Type type, const Size& size)
  : _d(new Impl)
{
  _d->masterTile = 0;
  _d->size = size;
  _d->isDeleted = false;
  _d->name = "unknown";

  setType(type);
}

const Desirability& Overlay::desirability() const
{
  return info().desirability();
}

void Overlay::setState(Param, double) {}

void Overlay::setType(const object::Type type)
{
  auto info = object::Info::find( type );

  _d->overlayType = type;
  _d->overlayClass = info.group();
  _d->name = info.name();
}

void Overlay::changeDirection( Tile* masterTile, Direction direction)
{
  _d->masterTile = masterTile;
}

Tilemap& Overlay::_map() const
{
  if( _city().isValid() )
    return _city()->tilemap();

  Logger::warning( "!!! WARNING: City is null at Overlay::_map()" );
  return config::tilemap.invalid();
}

void Overlay::setPicture(Picture picture)
{
  _d->picture = picture;
}

bool Overlay::build(const city::AreaInfo &info)
{
  Tilemap &tilemap = info.city->tilemap();

  _d->city = info.city;
  _d->masterTile = &tilemap.at( info.pos );

  for (int dj = 0; dj < _d->size.height(); ++dj)
  {
    for (int di = 0; di < _d->size.width(); ++di)
    {
      Tile& tile = tilemap.at( info.pos + TilePos( di, dj ) );
      tile.setMaster( _d->masterTile );

      if( tile.overlay().isValid() && tile.overlay() != this )
      {
        tile.overlay()->deleteLater();
      }

      tile.setOverlay( this );
      initTerrain( tile );
    }
  }
  info.city->setOption( PlayerCity::updateTiles, 1 );

  return true;
}

bool Overlay::canDestroy() const { return true; }
std::string Overlay::errorDesc() const { return ""; }

Tile& Overlay::tile() const
{
  if( !_d->masterTile )
  {
    LOG_OVERLAY.warn( "Master tile can't be null. Problem in tile with type " + object::toString( type() ) );
    static Tile invalid( TilePos::invalid() );
    return invalid;
  }
  return *_d->masterTile;
}

const Pictures& Overlay::pictures( Renderer::Pass pass ) const
{
  switch( pass )
  {
  case Renderer::overlayAnimation: return _d->fgPictures;
  default: break;
  }

  return invalidPictures;
}

void Overlay::save( VariantMap& stream ) const
{
  VariantList config;
  config.push_back( _d->overlayType );

  std::string name = info().name();
  config.push_back( Variant( name.empty() ? debugName() : name ) );

  config.push_back( tile().pos() );

  stream[ "config" ] = config;
  stream[ "picture" ] = Variant( _d->picture.name() );
  stream[ "pictureOffset" ] = _d->picture.offset();
  stream[ "height" ] = tile().height();
  VARIANT_SAVE_ANY_D( stream, _d, size )
  VARIANT_SAVE_ANY_D( stream, _d, isDeleted )
  VARIANT_SAVE_STR_D( stream, _d, name )
}

void Overlay::load( const VariantMap& stream )
{
  VARIANT_LOAD_STR_D( _d, name, stream )
  VARIANT_LOAD_ANY_D( _d, size, stream )

  std::string pictureName = stream.get( "picture" ).toString();
  _d->picture.load( pictureName );
  if( !_d->picture.isValid() )
  {
    LOG_OVERLAY.warn( "Invalid picture for building {0}{1} with name {2}", pos().i(), pos().j(), pictureName);
  }
  _d->picture.setOffset( stream.get( "pictureOffset" ).toPoint() );
  VARIANT_LOAD_ANYDEF_D( _d, isDeleted, false, stream )
  tile().setHeight( stream.get( "height" ) );
}

void Overlay::initialize(const object::Info& mdata)
{
  Size size = mdata.getOption( "size" );
  if( size.area() > 0 )
    setSize( size );

  if( mdata.randomPicture().isValid() )
  {
    setPicture( mdata.randomPicture() );  // default picture for build tool
  }
}

const object::Info& Overlay::info() const { return object::Info::find( type() ); }

void Overlay::timeStep(const unsigned long) {}
void Overlay::reinit() {}
void Overlay::burn() {}
void Overlay::collapse() {} //nothing to do, neck for virtual function
bool Overlay::isWalkable() const{  return false;}
bool Overlay::isDestructible() const { return true; }
bool Overlay::isFlat() const { return false;}
void Overlay::debugLoadOld(int oldFormat, const VariantMap& stream) {}
void Overlay::setName( const std::string& name ){ _d->name = name;}
void Overlay::setSize( const Size& size ){  _d->size = size;}
Point Overlay::offset( const Tile&, const Point& ) const{  return Point( 0, 0 );}
Animation& Overlay::_animation(){  return _d->animation;}
Tile* Overlay::_masterTile(){  return _d->masterTile;}
PlayerCityPtr Overlay::_city() const{ return _d->city;}
gfx::Pictures& Overlay::_fgPictures(){  return _d->fgPictures; }
Picture& Overlay::_fgPicture( unsigned int index ){  return _d->fgPictures[index]; }
const Picture& Overlay::_fgPicture( unsigned int index ) const {  return _d->fgPictures[index]; }
Picture& Overlay::_picture(){  return _d->picture; }
object::Group Overlay::group() const{  return _d->overlayClass;}
void Overlay::setPicture(const std::string& resource, const int index){ _picture().load( resource, index ); }
const Picture& Overlay::picture() const{  return _d->picture;}
void Overlay::setAnimation(const Animation& animation){  _d->animation = animation;}
const Animation& Overlay::animation() const { return _d->animation;}
void Overlay::deleteLater() { _d->isDeleted  = true;}
void Overlay::destroy() {}
const Size& Overlay::size() const{ return _d->size;}
bool Overlay::isDeleted() const{ return _d->isDeleted;}
Renderer::PassQueue Overlay::passQueue() const{ return defaultPassQueue;}
std::string Overlay::name(){  return _d->name;}
object::Type Overlay::type() const { return _d->overlayType;}

const TilePos& Overlay::pos() const
{
  if( !_d->masterTile )
  {
    LOG_OVERLAY.warn( "Master tile can't be null. Problem in tile with type " + object::toString( type() ) );
    return TilePos::invalid();
  }
  return _d->masterTile->epos();
}

const Picture& Overlay::picture(const city::AreaInfo& areaInfo) const
{
  return picture();
}

std::string Overlay::sound() const
{
  return info().sound();
}

TilesArray Overlay::area() const
{
  if( _city().isNull() )
  {
    LOG_OVERLAY.error( "In Overlay::area() city is null. Tile type is " +object::toString( type() ) );
    return gfx::TilesArray();
  }

  return _map().area( pos(), size() );
}

bool Overlay::getMinimapColor(int& color1, int& color2) const { return false; }

Overlay::~Overlay()
{
}  // what we shall to do here?
