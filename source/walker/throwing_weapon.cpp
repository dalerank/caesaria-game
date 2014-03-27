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
#include "gfx/tilemap.hpp"
#include "core/foreach.hpp"

using namespace constants;

class ThrowingWeapon::Impl
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

void ThrowingWeapon::toThrow(TilePos src, TilePos dst)
{
  _d->from = src;
  _d->dst = dst;
  _d->dstPos = Point( dst.i(), dst.j() ) * 15 + Point( 7, 7 );
  _d->srcPos = Point( src.i(), src.j() ) * 15 + Point( 7, 7 );

  _d->deltaMove = ( _d->dstPos - _d->srcPos ).toPointF() / 20.f;
  _d->currentPos = _d->srcPos.toPointF();

  _setPosOnMap( _d->srcPos );

  _getCity()->addWalker( this );
  Tile& tile = _getCity()->tilemap().at( src );
  TileOverlayPtr ov = tile.overlay();
  if( ov.isValid() )
  {
    _d->height = ov->offset( tile, Point( 7, 7 ) ).y();
    Tile& dTile = _getCity()->tilemap().at( dst );
    ov = dTile.overlay();
    if( ov.isValid() )
    {
      float dHeight = ov->offset( dTile, Point( 7, 7) ).y();
      _d->deltaHeight = (dHeight - _d->height) / 20.f;
    }
  }

  turn( dst );
}

void ThrowingWeapon::timeStep(const unsigned long time)
{
  switch( getAction() )
  {
  case Walker::acMove:
  {
    _d->currentPos += _d->deltaMove;
    _d->height += _d->deltaHeight;
    TilePos ij( (_d->currentPos.x() - 7) / 15, (_d->currentPos.y() - 7) / 15 );
    setPos( ij );
    _setPosOnMap( _d->currentPos.toPoint() - Point( 0, _d->height ) );

    if( _d->currentPos.IsEqual( _d->dstPos.toPointF(), 3.f ) )
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
  PointF prPos = PointF( p.i(), p.j() ) * 15 + PointF( 7, 7 );
  float t = (_d->currentPos - prPos).getAngle();
  int angle = (int)( t / 22.5f);// 0 is east

  int index = rcStartIndex();
  switch( angle )
  {
  case 0: index = rcStartIndex() + 12; break;
  case 1: index = rcStartIndex() + 14; break;
  case 2: index = rcStartIndex() + 15; break;
  case 3: index = rcStartIndex(); break;
  case 4: index = rcStartIndex() + 1; break;
  case 5: index = rcStartIndex() + 2; break;
  case 6: index = rcStartIndex() + 2; break;
  case 7: index = rcStartIndex() + 3; break;
  case 8: index = rcStartIndex() + 5; break;
  case 9: index = rcStartIndex() + 6; break;
  case 10: index = rcStartIndex() + 7; break;
  case 11: index = rcStartIndex() + 8; break;
  case 12: index = rcStartIndex() + 9; break;
  case 13: index = rcStartIndex() + 10; break;
  case 14: index = rcStartIndex() + 11; break;
  case 15: index = rcStartIndex() + 11; break;
  }

  //if( angle > 13 ) { index = rcStartIndex() + (angle - 14); }
  //else  { index = rcStartIndex() + 2 + angle; }

  _d->pic = Picture::load( rcGroup(), index );
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

ThrowingWeapon::ThrowingWeapon(PlayerCityPtr city) : Walker( city ), _d( new Impl )
{
  setName( _("##unknow_throwing_weapon##") );
}
