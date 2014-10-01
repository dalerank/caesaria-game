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

#include "prefecture.hpp"
#include "gfx/picture.hpp"
#include "game/resourcegroup.hpp"
#include "core/position.hpp"
#include "walker/prefect.hpp"
#include "pathway/pathway_helper.hpp"
#include "gfx/tile.hpp"
#include "pathway/path_finding.hpp"
#include "gfx/tilemap.hpp"
#include "city/helper.hpp"
#include "objects/constants.hpp"

using namespace gfx;

class Prefecture::Impl
{
public:
  TilePos fireDetect;
};

Prefecture::Prefecture()
  : ServiceBuilding(Service::prefect, constants::building::prefecture, Size(1)),
    _d( new Impl )
{
  _d->fireDetect = TilePos( -1, -1 );
  setPicture( ResourceGroup::security, 1 );
  
  _animationRef().load( ResourceGroup::security, 2, 10);
  _animationRef().setDelay( 4 );
  _animationRef().setOffset( Point( 20, 36 ) );
  _fgPicturesRef().resize(1);
}

Prefecture::~Prefecture() {}

void Prefecture::timeStep(const unsigned long time)
{
  ServiceBuilding::timeStep( time );
}

void Prefecture::deliverService()
{
  if( numberWorkers() > 0 && walkers().size() == 0 )
  {
    bool fireDetect = _d->fireDetect.i() >= 0;
    PrefectPtr prefect = Prefect::create( _city() );
    prefect->setMaxDistance( walkerDistance() );

    if( fireDetect )
    {
      TilePos startPos = getAccessRoads().front()->pos();

      ConstructionPtr ruin = ptr_cast<Construction>( _city()->getOverlay( _d->fireDetect ) );
      Pathway pathway = PathwayHelper::create( startPos, ruin, PathwayHelper::allTerrain );

      if( pathway.isValid() )
      {
        pathway.setNextTile( ruin->tile() );
        prefect->setPos( pathway.startPos() );
        prefect->setBase( this );
        prefect->setPathway( pathway );
        prefect->go();
      }
      else
      {
        fireDetect = false;
      }

      _d->fireDetect = TilePos( -1, -1 );
    }
    
    prefect->send2City( this, Prefect::patrol, fireDetect ? 1000 : 0 );

    addWalker( prefect.object() );
  }
}

unsigned int Prefecture::walkerDistance() const { return 26; }

void Prefecture::fireDetect( const TilePos& pos ) {  _d->fireDetect = pos; }
