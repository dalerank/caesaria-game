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

#include "spear.hpp"
#include "core/gettext.hpp"
#include "city/city.hpp"
#include "game/resourcegroup.hpp"
#include "gfx/tilemap.hpp"
#include "core/foreach.hpp"

using namespace constants;

class Spear::Impl
{
public:
  Point srcPos;
  Point dstPos;
  PointF deltaMove;
  Picture pic;
  PointF currentPos;
  TilePos from, dst;
  float height, deltaHeight;
};

SpearPtr Spear::create(PlayerCityPtr city)
{
  SpearPtr ret( new Spear( city ) );
  ret->drop();

  return ret;
}

void Spear::toThrow(TilePos src, TilePos dst)
{
  _d->from = src;
  _d->dst = dst;
  _d->dstPos = Point( dst.getI(), dst.getJ() ) * 15 + Point( 7, 7 );
  _d->srcPos = Point( src.getI(), src.getJ() ) * 15 + Point( 7, 7 );

  _d->deltaMove = ( _d->dstPos - _d->srcPos ).toPointF() / 20.f;
  _d->currentPos = _d->srcPos.toPointF();

  _setPosOnMap( _d->currentPos.toPoint() );

  _getCity()->addWalker( this );
  Tile& tile = _getCity()->getTilemap().at( src );
  TileOverlayPtr ov = tile.getOverlay();
  if( ov.isValid() )
  {
    _d->height = ov->getOffset( tile, Point( 7, 7 ) ).getY();
    Tile& dTile = _getCity()->getTilemap().at( dst );
    ov = dTile.getOverlay();
    if( ov.isValid() )
    {
      float dHeight = ov->getOffset( dTile, Point( 7, 7) ).getY();
      _d->deltaHeight = (dHeight - _d->height) / 20.f;
    }
  }

  turn( dst );
}

void Spear::timeStep(const unsigned long time)
{
  switch( getAction() )
  {
  case Walker::acMove:
    _d->currentPos += _d->deltaMove;
    _d->height += _d->deltaHeight;
    _setPosOnMap( _d->currentPos.toPoint() - Point( 0, _d->height ) );

    if( _d->currentPos.IsEqual( _d->dstPos.toPointF(), 3.f ) )
    {
      _d->currentPos = _d->dstPos.toPointF();
     _reachedPathway();
    }
  break;

  default:
  break;
  }
}

void Spear::turn(TilePos pos)
{
  float t = (pos - getIJ()).getAngleICW();
  int angle = (int)( t / 22.5f);// 0 is east

  int index = 0;
  if( angle > 13 ) { index = 114 + (angle - 14); }
  else  { index = 116 + angle; }

  _d->pic = Picture::load( ResourceGroup::sprites, index );
}

const Picture& Spear::getMainPicture()
{
  return _d->pic;
}

void Spear::_reachedPathway()
{
  WalkerList walkers = _getCity()->getWalkers( walker::any, _d->dst );
  foreach( w, walkers )
  {
    (*w)->updateHealth( -10 );
    (*w)->acceptAction( Walker::acFight, _d->from );
  }
  deleteLater();
}

Spear::Spear(PlayerCityPtr city) : Walker( city ), _d( new Impl )
{
  _setType( walker::spear );
  _setAnimation( gfx::unknown );

  setName( _("##Spear##") );
}
