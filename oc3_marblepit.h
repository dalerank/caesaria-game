#ifndef __OPENCAESAR3_MARBLEPIT_H_INCLUDED__
#define __OPENCAESAR3_MARBLEPIT_H_INCLUDED__

#include "oc3_factory_building.hpp"

class FactoryMarble : public Factory
{
public:
  FactoryMarble();

  bool canBuild(const TilePos& pos ) const;  // returns true if it can be built there
  void timeStep(const unsigned long time);
};


#endif