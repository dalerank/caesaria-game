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

#include "oc3_emigrant.hpp"
#include "oc3_positioni.hpp"
#include "oc3_scenario.hpp"

Emigrant::Emigrant( City& city ) : Immigrant( city )
{
	_walkerType = WT_EMIGRANT;
	_walkerGraphic = WG_PUSHER2;
}

Emigrant* Emigrant::clone() const
{
  return 0;
}

Picture* Emigrant::getCartPicture()
{
  if( Immigrant::getCartPicture() == NULL )
	{
		setCartPicture( &CartLoader::instance().getCart( G_SCARB1, getDirection()) );
	}

	return Immigrant::getCartPicture();
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
		oPics.push_back( getCartPicture());
		oPics.push_back(&getMainPicture());
		break;
	case D_EAST:
	case D_SOUTH_EAST:
		oPics.push_back( getCartPicture());
		oPics.push_back(&getMainPicture());
		break;
	case D_SOUTH:
	case D_SOUTH_WEST:
		oPics.push_back(&getMainPicture());
		oPics.push_back( getCartPicture());
		break;
	default:
		break;
	}
}

void Emigrant::onNewDirection()
{
	Walker::onNewDirection();
	setCartPicture( 0 );  // need to get the new graphic
}

Emigrant* Emigrant::create( City& city, const Road& startPoint )
{
	Emigrant* newEmigrant = new Emigrant( city );
	newEmigrant->assignPath( startPoint.getTile() );
  city.addWalker( *newEmigrant );
	return newEmigrant;
}

Emigrant::~Emigrant()
{

}
