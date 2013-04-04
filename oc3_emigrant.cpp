#include <oc3_emigrant.h>
#include <oc3_positioni.h>
#include <scenario.hpp>

class Emigrant::Impl
{
public:
	Point destination;
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
	std::list<PathWay> pathWayList;

	std::list<LandOverlay*> houses = Scenario::instance().getCity().getBuildingList(B_HOUSE);
	House* blankHouse = 0;
	for( std::list<LandOverlay*>::iterator itHouse = houses.begin(); itHouse != houses.end(); ++itHouse )
	{
		if( House* house = dynamic_cast<House*>(*itHouse) )
		{
			if( house->getNbHabitants() < house->getMaxHabitants() )
			{
				blankHouse = house;
				_d->destination = Point( house->getTile().getI(), house->getTile().getJ() );
				break;
			}
		}
	}

	Propagator pathfinder;
	PathWay pathWay;
	pathfinder.init( const_cast< Road& >( startPoint ) );
	bool findPath = pathfinder.getPath( *blankHouse, pathWay );
	if( findPath )
	{
		setPathWay( pathWay );
		setIJ(_pathWay.getOrigin().getI(), _pathWay.getOrigin().getJ());   
	}
}

void Emigrant::onDestination()
{
	const Tile& tile = Scenario::instance().getCity().getTilemap().at( _d->destination.x, _d->destination.y );

	LandOverlay* overlay = tile.get_terrain().getOverlay();
	if( House* house = dynamic_cast<House*>( overlay ) )
	{
		if( house->getNbHabitants() < house->getMaxHabitants() )
		{
			house->addHabitants( 1 );
		}
	}

	_isDeleted = true;
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
