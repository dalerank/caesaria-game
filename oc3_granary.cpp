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

#include "oc3_granary.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_picture.hpp"
#include "oc3_variant.hpp"

Granary::Granary() : WorkingBuilding( B_GRANARY, Size(3) )
{
  setMaxWorkers(5);
  setWorkers(0);

  setPicture( Picture::load( ResourceGroup::commerce, 140));
  _fgPictures.resize(6);  // 1 upper level + 4 windows + animation
  int maxQty = 2400;
  _goodStore.setMaxQty(maxQty);
  _goodStore.setMaxQty(G_WHEAT, maxQty);
  _goodStore.setMaxQty(G_MEAT, maxQty);
  _goodStore.setMaxQty(G_FISH, maxQty);
  _goodStore.setMaxQty(G_FRUIT, maxQty);
  _goodStore.setMaxQty(G_VEGETABLE, maxQty);

  _goodStore.setCurrentQty(G_WHEAT, 300);

  _animation.load(ResourceGroup::commerce, 146, 7, Animation::straight);
  // do the animation in reverse
  _animation.load(ResourceGroup::commerce, 151, 6, Animation::reverse);
  _animation.setFrameDelay( 4 );


  _fgPictures[0] = &Picture::load( ResourceGroup::commerce, 141);
  _fgPictures[5] = _animation.getCurrentPicture();
  computePictures();
}

void Granary::timeStep(const unsigned long time)
{
  if( getWorkers() > 0 )
  {
    _animation.update( time );

    _fgPictures[5] = _animation.getCurrentPicture();
  }
}

SimpleGoodStore& Granary::getGoodStore()
{
  return _goodStore;
}

void Granary::computePictures()
{
  int allQty = _goodStore.getCurrentQty();
  int maxQty = _goodStore.getMaxQty();

  for (int n = 0; n < 4; ++n)
  {
    // reset all window pictures
    _fgPictures[n+1] = NULL;
  }

  if (allQty > 0)
  {
    _fgPictures[1] = &Picture::load( ResourceGroup::commerce, 142);
  }
  if( allQty > maxQty * 0.25)
  {
    _fgPictures[2] = &Picture::load( ResourceGroup::commerce, 143);
  }
  if (allQty > maxQty * 0.5)
  {
    _fgPictures[3] = &Picture::load( ResourceGroup::commerce, 144);
  }
  if (allQty > maxQty * 0.9)
  {
    _fgPictures[4] = &Picture::load( ResourceGroup::commerce, 145);
  }
}

void Granary::save( VariantMap& stream) const
{
   WorkingBuilding::save( stream );
   VariantMap vm_goodstore;
   _goodStore.save( vm_goodstore );
   stream[ "goodStore" ] = vm_goodstore;
}

void Granary::load( const VariantMap& stream)
{
//   WorkingBuilding::unserialize(stream);
//   _goodStore.unserialize(stream);
}