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

#include "throwing_weapon.hpp"
#include "core/gettext.hpp"
#include "city/city.hpp"
#include "game/resourcegroup.hpp"
#include "gfx/helper.hpp"
#include "gfx/tilemap.hpp"
#include "core/foreach.hpp"

using namespace constants;
using namespace gfx;

class ThrowingWeapon::Impl
{
public:
  Point offset;
  Point srcPos;
  Point dstPos;
  PointF deltaMove;
  Picture pic;
  PointF currentPos;
  TilePos from, dst;
  float height, deltaHeight;
};

void ThrowingWeapon::toThrow(TilePos src, TilePos dst)
{
  _d->from = src;
  _d->dst = dst;
  int yMultiplier = tilemap::cellSize().height();
  Point xOffset( yMultiplier, yMultiplier );
  _d->dstPos = Point( dst.i(), dst.j() ) * yMultiplier + xOffset;
  _d->srcPos = Point( src.i(), src.j() ) * yMultiplier + xOffset;

  _d->deltaMove = ( _d->dstPos - _d->srcPos ).toPointF() / (dst.distanceFrom( src) * 2.f);
  _d->currentPos = _d->srcPos.toPointF();

  _setWpos( _d->srcPos );

  _city()->addWalker( this );
  const Tile& tile = _city()->tilemap().at( src );
  TileOverlayPtr ov = tile.overlay();
  if( ov.isValid() )
  {
    _d->height = ov->offset( tile, xOffset ).y();
    const Tile& dTile = _city()->tilemap().at( dst );
    ov = dTile.overlay();
    if( ov.isValid() )
    {
      float dHeight = ov->offset( dTile, xOffset ).y();
      _d->deltaHeight = (dHeight - _d->height) / 20.f;
    }
  }

  turn( dst );
}

Point ThrowingWeapon::mappos() const
{
  const Point& p = _wpos();
  return Point( 2*(p.x() + p.y()), p.x() - p.y() ) + Point( 0, _d->height );
}

void ThrowingWeapon::timeStep(const unsigned long time)
{
  switch( action() )
  {
  case Walker::acMove:
  {
    PointF saveCurrent = _d->currentPos;
    _d->currentPos += _d->deltaMove;
    _d->height += _d->deltaHeight;
    const int wcell = tilemap::cellSize().height();

    Point tp = (_d->currentPos.toPoint() - tilemap::cellCenter()) / wcell;
    TilePos ij( tp.x(), tp.y() );
    setPos( ij );
    _setWpos( _d->currentPos.toPoint() );

    if( saveCurrent.getDistanceFrom( _d->dstPos.toPointF() ) <
        _d->currentPos.getDistanceFrom( _d->dstPos.toPointF() ) )
    {
      _d->currentPos = _d->dstPos.toPointF();
     _reachedPathway();
    }
  }
  break;

  default:
  break;
  }
}

void ThrowingWeapon::turn(TilePos p)
{
  int yMultiplier = tilemap::cellSize().height();
  PointF xOffset( yMultiplier, yMultiplier );
  PointF prPos = PointF( p.i(), p.j() ) * yMultiplier + xOffset;
  float t = (_d->currentPos - prPos).getAngle();
  int angle = (int)( t / 22.5f);// 0 is east

  int index = _rcStartIndex();
  switch( angle )
  {
  case 0: index = _rcStartIndex() + 12; break;
  case 1: index = _rcStartIndex() + 14; break;
  case 2: index = _rcStartIndex() + 15; break;
  case 3: index = _rcStartIndex(); break;
  case 4: index = _rcStartIndex() + 2; break;
  case 5: index = _rcStartIndex() + 2; break;
  case 6: index = _rcStartIndex() + 4; break;
  case 7: index = _rcStartIndex() + 5; break;
  case 8: index = _rcStartIndex() + 6; break;
  case 9: index = _rcStartIndex() + 6; break;
  case 10: index = _rcStartIndex() + 7; break;
  case 11: index = _rcStartIndex() + 9; break;
  case 12: index = _rcStartIndex() + 10; break;
  case 13: index = _rcStartIndex() + 10; break;
  case 14: index = _rcStartIndex() + 10; break;
  case 15: index = _rcStartIndex() + 13; break;
  }

  //if( angle > 13 ) { index = rcStartIndex() + (angle - 14); }
  //else  { index = rcStartIndex() + 2 + angle; }

  _d->pic = Picture::load( rcGroup(), index );
  _d->pic.setOffset( _d->offset );
}

const Picture& ThrowingWeapon::getMainPicture() {  return _d->pic; }
TilePos ThrowingWeapon::dstPos() const {  return _d->dst; }
TilePos ThrowingWeapon::startPos() const{  return _d->from; }
ThrowingWeapon::~ThrowingWeapon() {}

void ThrowingWeapon::_reachedPathway()
{
  _onTarget();
  deleteLater();
}

void ThrowingWeapon::setPicOffset(Point offset)
{
  _d->offset = offset;
}

ThrowingWeapon::ThrowingWeapon(PlayerCityPtr city) : Walker( city ), _d( new Impl )
{
  setName( _("##unknow_throwing_weapon##") );

  setFlag( vividly, false );
}
