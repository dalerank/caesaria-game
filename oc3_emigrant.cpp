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

#include "oc3_emigrant.h"
#include "oc3_positioni.h"
#include "scenario.hpp"

class Emigrant::Impl
{
public:
	TilePos destination;
	Picture* cartPicture;
};

Emigrant::Emigrant() : _d( new Impl )
{
	_walkerType = WT_EMIGRANT;
	_walkerGraphic = WG_PUSHER2;
}

Emigrant* Emigrant::clone() const
{
	Emigrant* ret = new Emigrant();
	ret->_d->destination = _d->destination;
	return ret;
}

Picture& Emigrant::_getCartPicture()
{
	if( _d->cartPicture == NULL )
	{
		_d->cartPicture = &CartLoader::instance().getCart( G_SCARB1, getDirection());
	}

	return *_d->cartPicture;
}

void Emigrant::getPictureList(std::vector<Picture*> &oPics)
{
	oPics.clear();

	// depending on the walker direction, the cart is ahead or behind
	switch (getDirection())
	{
	case D_WEST:
	case D_NORTH_WEST:
	case D_NORTH:
	case D_NORTH_EAST:
		oPics.push_back(&_getCartPicture());
		oPics.push_back(&getMainPicture());
		break;
	case D_EAST:
	case D_SOUTH_EAST:
		oPics.push_back(&_getCartPicture());
		oPics.push_back(&getMainPicture());
		break;
	case D_SOUTH:
	case D_SOUTH_WEST:
		oPics.push_back(&getMainPicture());
		oPics.push_back(&_getCartPicture());
		break;
	default:
		break;
	}
}

void Emigrant::assignPath( const Road& startPoint )
{
    City& city = Scenario::instance().getCity();
	std::list<PathWay> pathWayList;

	std::list<LandOverlay*> houses = city.getBuildingList(B_HOUSE);
	House* blankHouse = 0;
	for( std::list<LandOverlay*>::iterator itHouse = houses.begin(); itHouse != houses.end(); ++itHouse )
	{
		if( House* house = dynamic_cast<House*>(*itHouse) )
		{
			if( house->getNbHabitants() < house->getMaxHabitants() )
			{
				blankHouse = house;
				_d->destination = house->getTile().getIJ();
				break;
			}
		}
	}

	Propagator pathfinder;
	PathWay pathWay;
	pathfinder.init( const_cast< Road& >( startPoint ) );
    if( blankHouse )
    {
	    bool findPath = pathfinder.getPath( *blankHouse, pathWay );
	    if( findPath )
	    {
		    setPathWay( pathWay );
		    setIJ(_pathWay.getOrigin().getI(), _pathWay.getOrigin().getJ());   
	    }
    }
    else
    {
        Road* exitTile = dynamic_cast< Road* >( city.getTilemap().at( city.getRoadExitIJ() ).get_terrain().getOverlay() );
        _d->destination = TilePos( -1, -1 );
        bool findPath = pathfinder.getPath( *exitTile, pathWay );
        if( findPath )
        {
            setPathWay( pathWay );
        }
        else
            _isDeleted = true;
    }
}

void Emigrant::onDestination()
{
    _isDeleted = true;
    bool gooutCity = true;
    if( _d->destination.getI() > 0 && _d->destination.getJ() > 0 )
    {
	    const Tile& tile = Scenario::instance().getCity().getTilemap().at( _d->destination );

	    LandOverlay* overlay = tile.get_terrain().getOverlay();
	    if( House* house = dynamic_cast<House*>( overlay ) )
	    {
		    if( house->getNbHabitants() < house->getMaxHabitants() )
		    {
			    house->addHabitants( 1 );
                Walker::onDestination();
                gooutCity = false;
		    }
	    }
    }
    else
    {
        return;
    }

    if( gooutCity )
    {
        if( const Road* r = dynamic_cast< const Road* >( _pathWay.getDestination().get_terrain().getOverlay() ))
        {
            _isDeleted = false;
            assignPath( *r );
        }
    }
}

void Emigrant::onNewDirection()
{
	Walker::onNewDirection();
	_d->cartPicture = NULL;  // need to get the new graphic
}

Emigrant* Emigrant::create( const Road& startPoint )
{
	Emigrant* newEmigrant = new Emigrant();
	newEmigrant->assignPath( startPoint );
	return newEmigrant;
}

Emigrant::~Emigrant()
{

}
