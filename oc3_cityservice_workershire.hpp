#ifndef __OPENCAESAR3_CITYSERVICE_WORKERSHIRE_H_INCLUDED__
#define __OPENCAESAR3_CITYSERVICE_WORKERSHIRE_H_INCLUDED__

#include "oc3_cityservice.hpp"
#include "oc3_scopedptr.hpp"
#include "oc3_enums.hpp"
#include "oc3_predefinitions.hpp"

class CityServiceWorkersHire : public CityService
{
public:
  static CityServicePtr create( City& city );

  void update( const unsigned int time );
private:
  CityServiceWorkersHire( City& city );

  void _hireByType( const BuildingType type );
  bool _haveHr( WorkingBuildingPtr building );
 
  class Impl;
  ScopedPtr< Impl > _d;
};

#endif