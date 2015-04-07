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
#include "objects/metadata.hpp"
#include "city/city.hpp"
#include "gfx/tilemap.hpp"
#include "core/variant_map.hpp"
#include "gfx/helper.hpp"
#include "core/logger.hpp"

using namespace gfx;
using namespace constants;

namespace {
static Renderer::PassQueue defaultPassQueue=Renderer::PassQueue(1,Renderer::overlayAnimation);
static Pictures invalidPictures;
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

Overlay::Overlay(const object::Type type, const Size& size)
: _d( new Impl )
{
  _d->masterTile = 0;
  _d->size = size;
  _d->isDeleted = false;
  _d->name = "unknown";

  setType( type );

#ifdef DEBUG
  OverlayDebugQueue::instance().add( this );
#endif
}

Desirability Overlay::desirability() const
{
  return MetaDataHolder::getData( type() ).desirability();
}

void Overlay::setType(const object::Type type)
{
  const MetaData& bd = MetaDataHolder::getData( type );

  _d->overlayType = type;
  _d->overlayClass = bd.group();
  _d->name = bd.name();
}

void Overlay::timeStep(const unsigned long) {}

void Overlay::changeDirection( Tile* masterTile, Direction direction)
{
  _d->masterTile = masterTile;
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
      tile.setMasterTile( _d->masterTile );

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

Tile& Overlay::tile() const
{
  if( !_d->masterTile )
  {
    Logger::warning( "master tile must be exists" );
    static Tile invalid( gfx::tilemap::invalidLocation() );
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

  MetaDataHolder& md = MetaDataHolder::instance();
  config.push_back( md.hasData( _d->overlayType )
                      ? Variant( md.getData( _d->overlayType ).name() )
                      : Variant( debugName() ) );

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
  _d->picture = Picture::load( pictureName );
  if( !_d->picture.isValid() )
  {
    Logger::warning( "TileOverlay: invalid picture for building [%d,%d] with name %s", pos().i(), pos().j(), pictureName.c_str() );
  }
  _d->picture.setOffset( stream.get( "pictureOffset" ).toPoint() );
  VARIANT_LOAD_ANYDEF_D( _d, isDeleted, false, stream )
  tile().setHeight( stream.get( "height" ) );
}

void Overlay::initialize(const MetaData& mdata)
{
  if( mdata.picture().isValid() )
  {
    setPicture( mdata.picture() );  // default picture for build tool
  }
}

bool Overlay::isWalkable() const{  return false;}
bool Overlay::isDestructible() const { return true; }
bool Overlay::isFlat() const { return false;}
void Overlay::debugLoadOld(int oldFormat, const VariantMap& stream) {}

TilePos Overlay::pos() const
{
  if( !_d->masterTile )
  {
    Logger::warning(  "master tile can't be null" );
    return gfx::tilemap::invalidLocation();
  }
  return _d->masterTile->epos();
}

std::string Overlay::sound() const
{
  const MetaData& md = MetaDataHolder::instance().getData( type() );
  return md.sound();
}

void Overlay::setName( const std::string& name ){ _d->name = name;}
void Overlay::setSize( const Size& size ){  _d->size = size;}
Point Overlay::offset( const Tile&, const Point& ) const{  return Point( 0, 0 );}
Animation& Overlay::_animationRef(){  return _d->animation;}
Tile* Overlay::_masterTile(){  return _d->masterTile;}
PlayerCityPtr Overlay::_city() const{ return _d->city;}
gfx::Pictures& Overlay::_fgPicturesRef(){  return _d->fgPictures; }
Picture& Overlay::_fgPicture( unsigned int index ){  return _d->fgPictures[index]; }
const Picture& Overlay::_fgPicture( unsigned int index ) const {  return _d->fgPictures[index]; }
Picture& Overlay::_pictureRef(){  return _d->picture;}
object::Group Overlay::group() const{  return _d->overlayClass;}
void Overlay::setPicture(const char* resource, const int index){  setPicture( Picture::load( resource, index ) );}
const Picture& Overlay::picture() const{  return _d->picture;}
void Overlay::setAnimation(const Animation& animation){  _d->animation = animation;}
const Animation& Overlay::animation() const { return _d->animation;}
void Overlay::deleteLater() { _d->isDeleted  = true;}
void Overlay::destroy(){}
const Size& Overlay::size() const{ return _d->size;}
bool Overlay::isDeleted() const{ return _d->isDeleted;}
Renderer::PassQueue Overlay::passQueue() const{ return defaultPassQueue;}
std::string Overlay::name(){  return _d->name;}
object::Type Overlay::type() const{ return _d->overlayType;}

TilesArray Overlay::area() const
{
  if( _city().isNull() )
  {
    Logger::warning( "WARNING !!!: Helper::getArea city is null" );
    return gfx::TilesArray();
  }

  return _city()->tilemap().getArea( pos(), size() );
}

Overlay::~Overlay()
{
#ifdef DEBUG
  OverlayDebugQueue::instance().rem( this );
#endif
}  // what we shall to do here?

#ifdef DEBUG
void OverlayDebugQueue::print()
{
  OverlayDebugQueue& inst = (OverlayDebugQueue&)instance();
  if( !inst._pointers.empty() )
  {
    Logger::warning( "PRINT OVERLAY DEBUG QUEUE" );
    foreach( it, inst._pointers )
    {
      Overlay* ov = (Overlay*)*it;
      Logger::warning( "%s - %s [%d,%d] ref:%d", ov->name().c_str(),
                                          object::toString( ov->type() ).c_str(),
                                          ov->pos().i(), ov->pos().j(), ov->rcount() );
    }
  }
}
#endif
