#include "oc3_buildingprefect.hpp"
#include "oc3_picture.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_positioni.hpp"

BuildingPrefect::BuildingPrefect() : ServiceBuilding(S_PREFECT)
{
  setType(B_PREFECT);
  _size = 1;
  setPicture( Picture::load( ResourceGroup::security, 1 ) );

  _animation.load( ResourceGroup::security, 2, 10);
  _animation.setFrameDelay( 4 );
  _animation.setOffset( Point( 10, 42 ) );
  _fgPictures.resize(1);
}

void BuildingPrefect::timeStep(const unsigned long time)
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

  ServiceBuilding::timeStep( time );
}

void BuildingPrefect::deliverService()
{
  if( getWorkers() > 0 )
    ServiceBuilding::deliverService();
}

BuildingPrefect* BuildingPrefect::clone() const
{
  return new BuildingPrefect(*this);
}
