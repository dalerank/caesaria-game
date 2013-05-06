#include "oc3_marblepit.h"
#include "oc3_resourcegroup.hpp"
#include "oc3_scenario.hpp"

FactoryMarble::FactoryMarble() : Factory(G_NONE, G_MARBLE, B_MARBLE, Size(2) )
{
  _productionRate = 9.6f;
  _picture = &Picture::load(ResourceGroup::commerce, 43);

  _animation.load( ResourceGroup::commerce, 44, 10);
  _animation.setFrameDelay( 4 );
  _fgPictures.resize(2);
}

bool FactoryMarble::canBuild(const TilePos& pos ) const
{
  bool is_constructible = Construction::canBuild( pos );
  bool near_mountain = false;  // tells if the factory is next to a mountain

  Tilemap& tilemap = Scenario::instance().getCity().getTilemap();
  PtrTilesArea rect = tilemap.getRectangle( pos + TilePos( -1, -1 ), Size( _size + 2 ), Tilemap::checkCorners);
  for( PtrTilesArea::iterator itTiles = rect.begin(); itTiles != rect.end(); ++itTiles)
  {
    near_mountain |= (*itTiles)->get_terrain().isRock();
  }

  return (is_constructible && near_mountain);
}

void FactoryMarble::timeStep( const unsigned long time )
{
  bool mayAnimate = getWorkers() > 0;

  if( mayAnimate && _animation.isStopped() )
  {
    _animation.start();
  }

  if( !mayAnimate && _animation.isRunning() )
  {
    _animation.stop();
  }

  Factory::timeStep( time );
}
