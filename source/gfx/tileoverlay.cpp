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

#include "tileoverlay.hpp"
#include "objects/metadata.hpp"
#include "city/city.hpp"
#include "tilemap.hpp"
#include "core/logger.hpp"

namespace gfx
{

namespace {
static Renderer::PassQueue defaultPassQueue=Renderer::PassQueue(1,Renderer::foreground);
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
}


void TileOverlay::setType(const Type type)
{
  const MetaData& bd = MetaDataHolder::instance().getData( type );

   _d->overlayType = type;
   _d->overlayClass = bd.getGroup();
   _d->name = bd.getName();
}

void TileOverlay::timeStep(const unsigned long) {}

void TileOverlay::setPicture(Picture picture)
{
  _d->picture = picture;

  if (_d->masterTile != NULL)
  {
    Tilemap &tilemap = _city()->tilemap();
    // _master_tile == NULL is cloneable buildings
    TilePos pos = _d->masterTile->pos();

    for (int dj = 0; dj<_d->size.width(); ++dj)
    {
      for (int di = 0; di<_d->size.height(); ++di)
      {
        Tile &tile = tilemap.at( pos + TilePos( di, dj ) );
        tile.setPicture( &_d->picture );
      }
    }
  }
}

void TileOverlay::build( PlayerCityPtr city, const TilePos& pos )
{
  Tilemap &tilemap = city->tilemap();

  _d->city = city;
  _d->masterTile = &tilemap.at( pos );

  for (int dj = 0; dj < _d->size.height(); ++dj)
  {
    for (int di = 0; di < _d->size.width(); ++di)
    {
      Tile& tile = tilemap.at( pos + TilePos( di, dj ) );
      tile.setMasterTile( _d->masterTile );
      tile.setPicture( &_d->picture );

      if( tile.overlay().isValid() && tile.overlay() != this )
      {
        tile.overlay()->deleteLater();
      }

      tile.setOverlay( this );
      initTerrain( tile );
    }
  }
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

const Pictures& TileOverlay::getPictures( Renderer::Pass pass ) const
{
  switch( pass )
  {
  case Renderer::foreground: return _d->fgPictures;
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
                      ? Variant( md.getData( _d->overlayType ).getName() )
                      : Variant( getDebugName() ) );

  config.push_back( tile().pos() );

  stream[ "config" ] = config;
  stream[ "picture" ] = Variant( _d->picture.name() );
  stream[ "pictureOffset" ] = _d->picture.offset();
  stream[ "size" ] = _d->size;
  stream[ "isDeleted" ] = _d->isDeleted;
  stream[ "name" ] = Variant( _d->name );
}

void TileOverlay::load( const VariantMap& stream )
{
  _d->name = stream.get( "name" ).toString();
  _d->size = stream.get( "size", Size(1) ).toSize();
  //_d->overlayType = (LandOverlayType)stream.get( "overlayType" ).toInt();
  _d->picture = Picture::load( stream.get( "picture" ).toString() );
  _d->picture.setOffset( stream.get( "pictureOffset" ).toPoint() );
  _d->isDeleted = stream.get( "isDeleted", false ).toBool();  
}

bool TileOverlay::isWalkable() const{  return false;}
bool TileOverlay::isDestructible() const { return true; }
bool TileOverlay::isFlat() const{  return false;}

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
  return md.getSound();
}

void TileOverlay::setName( const std::string& name ){  _d->name = name;}
void TileOverlay::setSize( const Size& size ){  _d->size = size;}
Point TileOverlay::offset( const Tile&, const Point& ) const{  return Point( 0, 0 );}
Animation& TileOverlay::_animationRef(){  return _d->animation;}
Tile* TileOverlay::_masterTile(){  return _d->masterTile;}
PlayerCityPtr TileOverlay::_city() const{ return _d->city;}
gfx::Pictures& TileOverlay::_fgPicturesRef(){  return _d->fgPictures; }
Picture& TileOverlay::_fgPicture( unsigned int index ){  return _d->fgPictures[index]; }
Picture& TileOverlay::_pictureRef(){  return _d->picture;}
TileOverlay::Group TileOverlay::getClass() const{  return _d->overlayClass;}
void TileOverlay::setPicture(const char* resource, const int index){  setPicture( Picture::load( resource, index ) );}
const Picture& TileOverlay::getPicture() const{  return _d->picture;}
void TileOverlay::setAnimation(const Animation& animation){  _d->animation = animation;}
const Animation&TileOverlay::animation() const { return _d->animation;}
void TileOverlay::deleteLater(){  _d->isDeleted  = true;}
void TileOverlay::destroy(){}
Size TileOverlay::size() const{  return _d->size;}
bool TileOverlay::isDeleted() const{  return _d->isDeleted;}
Renderer::PassQueue TileOverlay::getPassQueue() const{ return defaultPassQueue;}
std::string TileOverlay::name(){  return _d->name;}
TileOverlay::~TileOverlay(){}  // what we shall to do here?
TileOverlay::Type TileOverlay::type() const{   return _d->overlayType;}

}//end namespace gfx
