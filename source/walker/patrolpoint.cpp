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
#include "game/gamedate.hpp"

using namespace constants;
using namespace gfx;

namespace {
  const Point extOffset( 15, 0 );
  const Point animOffset( 0, 52 );
  const Point embemOffset( 0, 18 );
}

class PatrolPoint::Impl
{
public:
  Picture emblem;
  Animation animation;
  Picture standart;
  TilePos basePos;
};

PatrolPointPtr PatrolPoint::create( PlayerCityPtr city, FortPtr base,
                                    std::string prefix, int startPos, int stepNumber, TilePos position)
{
  PatrolPoint* pp = new PatrolPoint( city );
  pp->_d->basePos = base->pos();
  pp->updateMorale( base->legionMorale() );

  pp->_d->emblem = base->legionEmblem();
  pp->_d->emblem.setOffset( animOffset + embemOffset + Point( -15, 30 ) );

  Animation anim;
  anim.load( prefix, startPos, stepNumber );
  anim.setOffset( anim.offset() + animOffset  + extOffset );
  pp->_d->animation = anim;

  pp->setPos( position );
  PatrolPointPtr ptr( pp );
  ptr->drop();

  pp->attach();
  return ptr;
}

void PatrolPoint::getPictures( gfx::Pictures& oPics)
{
  oPics.push_back( _d->standart );
  oPics.push_back( _d->emblem );
  oPics.push_back( _d->animation.currentFrame() );
}

PatrolPoint::PatrolPoint( PlayerCityPtr city )
  : Walker( city ), _d( new Impl )
{
  _setType( walker::patrolPoint );

  setFlag( vividly, false );
}

void PatrolPoint::timeStep(const unsigned long time)
{
  _d->animation.update( time );
}

FortPtr PatrolPoint::base() const
{
  return ptr_cast<Fort>( _city()->getOverlay( _d->basePos ) );
}

void PatrolPoint::save(VariantMap& stream) const
{

}

void PatrolPoint::acceptPosition()
{
  FortPtr fort = ptr_cast<Fort>( _city()->getOverlay( _d->basePos ) );
  if( fort.isValid() )
  {
    fort->changePatrolArea();
  }
}

void PatrolPoint::updateMorale(int morale)
{
  int mIndex = 20 - math::clamp( morale / 5, 0, 20);
  _d->standart = Picture::load( ResourceGroup::sprites, 48 + mIndex );
  _d->standart.addOffset( extOffset.x(), extOffset.y() );
}
