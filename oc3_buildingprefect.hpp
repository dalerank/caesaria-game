#ifndef __OPENCAESAR3_BUILDINGPREFECT_H_INCLUDED__
#define __OPENCAESAR3_BUILDINGPREFECT_H_INCLUDED__

#include "oc3_service_building.hpp"

class BuildingPrefect : public ServiceBuilding
{
public:
  BuildingPrefect();
  BuildingPrefect* clone() const;
  void timeStep(const unsigned long time);
  void deliverService();
  int getServiceDelay() const;
};

#endif
