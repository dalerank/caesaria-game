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

#include "tileoverlay.hpp"
#include "objects/metadata.hpp"
#include "city/city.hpp"
#include "tilemap.hpp"
#include "core/variant_map.hpp"
#include "core/logger.hpp"

namespace gfx
{

namespace {
static Renderer::PassQueue defaultPassQueue=Renderer::PassQueue(1,Renderer::overlayAnimation);
static Pictures invalidPictures;
}

class TileOverlay::Impl
{
public:  
  Pictures fgPictures;
  TileOverlay::Type overlayType;
  TileOverlay::Group overlayClass;
  Tile* masterTile;  // left-most tile if multi-tile, or "this" if single-tile
  std::string name;
  Picture picture;
  Size size;  // size in tiles
  Animation animation;  // basic animation (if any)
  bool isDeleted;
  PlayerCityPtr city;
};

TileOverlay::TileOverlay(const Type type, const Size& size)
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

Desirability TileOverlay::desirability() const
{
  return MetaDataHolder::getData( type() ).desirability();
}

void TileOverlay::setType(const Type type)
{
  const MetaData& bd = MetaDataHolder::getData( type );

   _d->overlayType = type;
   _d->overlayClass = bd.group();
   _d->name = bd.name();
}

void TileOverlay::timeStep(const unsigned long) {}

void TileOverlay::changeDirection( Tile* masterTile, constants::Direction direction)
{
  _d->masterTile = masterTile;
}

void TileOverlay::setPicture(Picture picture)
{
  _d->picture = picture;
}

bool TileOverlay::build(const CityAreaInfo &info)
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

Tile& TileOverlay::tile() const
{
  if( !_d->masterTile )
  {
    Logger::warning( "master tile must be exists" );
    static Tile invalid( TilePos( -1, -1 ));
    return invalid;
  }
  return *_d->masterTile;
}

const Pictures& TileOverlay::pictures( Renderer::Pass pass ) const
{
  switch( pass )
  {
  case Renderer::overlayAnimation: return _d->fgPictures;
  default: break;
  }

  return invalidPictures;
}

void TileOverlay::save( VariantMap& stream ) const
{
  VariantList config;
  config.push_back( (int)_d->overlayType );

  MetaDataHolder& md = MetaDataHolder::instance();
  config.push_back( md.hasData( _d->overlayType )
                      ? Variant( md.getData( _d->overlayType ).name() )
                      : Variant( debugName() ) );

  config.push_back( tile().pos() );

  stream[ "config" ] = config;
  stream[ "picture" ] = Variant( _d->picture.name() );
  stream[ "pictureOffset" ] = _d->picture.offset();
  stream[ "size" ] = _d->size;
  stream[ "height" ] = tile().height();
  stream[ "isDeleted" ] = _d->isDeleted;
  stream[ "name" ] = Variant( _d->name );
}

void TileOverlay::load( const VariantMap& stream )
{
  _d->name = stream.get( "name" ).toString();
  _d->size = stream.get( "size", Size(1) ).toSize();
  //_d->overlayType = (LandOverlayType)stream.get( "overlayType" ).toInt();
  std::string pictureName = stream.get( "picture" ).toString();
  _d->picture = Picture::load( pictureName );
  if( !_d->picture.isValid() )
  {
    Logger::warning( "TileOverlay: invalid picture for building [%d,%d] with name %s", pos().i(), pos().j(), pictureName.c_str() );
  }
  _d->picture.setOffset( stream.get( "pictureOffset" ).toPoint() );
  _d->isDeleted = stream.get( "isDeleted", false ).toBool();  
  tile().setHeight( stream.get( "height" ).toInt() );
}

void TileOverlay::initialize(const MetaData& mdata)
{
  if( mdata.picture().isValid() )
  {
    setPicture( mdata.picture() );  // default picture for build tool
  }
}

bool TileOverlay::isWalkable() const{  return false;}
bool TileOverlay::isDestructible() const { return true; }
bool TileOverlay::isFlat() const { return false;}

TilePos TileOverlay::pos() const
{
  if( !_d->masterTile )
  {
    Logger::warning(  "master tile can't be null" );
    return TilePos( -1, -1 );
  }
  return _d->masterTile->pos();
}

std::string TileOverlay::sound() const
{
  const MetaData& md = MetaDataHolder::instance().getData( type() );
  return md.sound();
}

void TileOverlay::setName( const std::string& name ){ _d->name = name;}
void TileOverlay::setSize( const Size& size ){  _d->size = size;}
Point TileOverlay::offset( const Tile&, const Point& ) const{  return Point( 0, 0 );}
Animation& TileOverlay::_animationRef(){  return _d->animation;}
Tile* TileOverlay::_masterTile(){  return _d->masterTile;}
PlayerCityPtr TileOverlay::_city() const{ return _d->city;}
gfx::Pictures& TileOverlay::_fgPicturesRef(){  return _d->fgPictures; }
Picture& TileOverlay::_fgPicture( unsigned int index ){  return _d->fgPictures[index]; }
Picture& TileOverlay::_pictureRef(){  return _d->picture;}
TileOverlay::Group TileOverlay::group() const{  return _d->overlayClass;}
void TileOverlay::setPicture(const char* resource, const int index){  setPicture( Picture::load( resource, index ) );}
const Picture& TileOverlay::picture() const{  return _d->picture;}
void TileOverlay::setAnimation(const Animation& animation){  _d->animation = animation;}
const Animation& TileOverlay::animation() const { return _d->animation;}
void TileOverlay::deleteLater(){  _d->isDeleted  = true;}
void TileOverlay::destroy(){}
Size TileOverlay::size() const{  return _d->size;}
bool TileOverlay::isDeleted() const{  return _d->isDeleted;}
Renderer::PassQueue TileOverlay::passQueue() const{ return defaultPassQueue;}
std::string TileOverlay::name(){  return _d->name;}
TileOverlay::Type TileOverlay::type() const{ return _d->overlayType;}

TileOverlay::~TileOverlay()
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
      TileOverlay* ov = (TileOverlay*)*it;
      Logger::warning( "%s - %s [%d,%d] ref:%d", ov->name().c_str(),
                                          MetaDataHolder::findTypename( ov->type() ).c_str(),
                                          ov->pos().i(), ov->pos().j(), ov->rcount() );
    }
  }
}
#endif

}//end namespace gfx
