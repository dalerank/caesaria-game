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

#include "patrolpoint.hpp"
#include "game/resourcegroup.hpp"
#include "objects/military.hpp"
#include "city/city.hpp"

using namespace constants;

class PatrolPoint::Impl
{
public:
  Animation animation;
  Picture standart;
  TilePos basePos;
};

PatrolPointPtr PatrolPoint::create( PlayerCityPtr city, FortPtr base,
                                    std::string prefix, int startPos, int stepNumber, TilePos position)
{
  PatrolPoint* pp = new PatrolPoint( city );
  pp->_d->standart = Picture::load( ResourceGroup::sprites, 58 );
  pp->_d->basePos = base->pos();

  Point extOffset( 15, 0 );
  Animation anim;
  anim.load( prefix, startPos, stepNumber );
  anim.setOffset( anim.getOffset() + Point( 0, 52 )  + extOffset );
  pp->_d->standart.addOffset( extOffset.x(), extOffset.y() );

  pp->_d->animation = anim;
  pp->setPos( position );
  PatrolPointPtr ptr( pp );
  ptr->drop();

  city->addWalker( ptr.object() );
  return ptr;
}

void PatrolPoint::getPictureList(PicturesArray& oPics)
{
  oPics.push_back( _d->standart );
  oPics.push_back( _d->animation.getFrame() );
}

PatrolPoint::PatrolPoint( PlayerCityPtr city )
  : Walker( city ), _d( new Impl )
{
  _setType( walker::patrolPoint );
}

void PatrolPoint::timeStep(const unsigned long time)
{
  _d->animation.update( time );
}

void PatrolPoint::acceptPosition()
{
  FortPtr fort = ptr_cast<Fort>( _getCity()->getOverlay( _d->basePos ) );
  if( fort.isValid() )
  {
    fort->changePatrolArea();
  }
}
