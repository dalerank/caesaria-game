// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.

#include "oc3_immigrant.hpp"
#include "oc3_positioni.hpp"
#include "oc3_scenario.hpp"

class Immigrant::Impl
{
public:
  Point destination;
};

Immigrant::Immigrant() : _d( new Impl )
{
  _walkerType = WT_IMMIGRANT;
  _walkerGraphic = WG_HOMELESS;
}

Immigrant* Immigrant::clone() const
{
  Immigrant* ret = new Immigrant();
  ret->_d->destination = _d->destination;
  return ret;
}

void Immigrant::assignPath( const Building& home )
{
  City& city = Scenario::instance().getCity();
  Tile& exitTile = city.getTilemap().at( city.getRoadExitI(), city.getRoadExitJ() );

  Road* exitRoad = dynamic_cast< Road* >( exitTile.get_terrain().getOverlay() );
  if( exitRoad )
  {
    Propagator pathfinder;
    PathWay pathWay;
    pathfinder.init( const_cast< Building& >( home ) );
    bool findPath = pathfinder.getPath( *exitRoad, pathWay );
    if( findPath )
    {
      setPathWay( pathWay );
      setIJ(_pathWay.getOrigin().getI(), _pathWay.getOrigin().getJ());   
    }
  }
  else
    _isDeleted = true;
}

void Immigrant::onDestination()
{  
  _isDeleted = true;
}

Immigrant* Immigrant::create( const Building& startPoint )
{
  Immigrant* newImmigrant = new Immigrant();
  newImmigrant->assignPath( startPoint );
  return newImmigrant;
}

Immigrant::~Immigrant()
{

}
