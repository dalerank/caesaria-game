#ifndef __OPENCAESAR3_CITYSERVICE_EMIGRANT_H_INCLUDED__
#define __OPENCAESAR3_CITYSERVICE_EMIGRANT_H_INCLUDED__

#include "oc3_cityservice.hpp"

class CityServiceEmigrant : public CityService
{
public:
  static CityServicePtr create( City& city );

  void update( const unsigned int time );
private:
  CityServiceEmigrant( City& city );
};

#endif