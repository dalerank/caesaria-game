#ifndef __OPENCAESAR3_CITYSERVICE_H_INCLUDED__
#define __OPENCAESAR3_CITYSERVICE_H_INCLUDED__

#include "oc3_referencecounted.hpp"
#include "oc3_smartptr.hpp"

class City;

class CityService : public ReferenceCounted
{
public:
  virtual void update( const unsigned int time ) = 0;

  virtual std::string getName() const { return _name; }
  virtual bool isDeleted() const { return false; }

protected:
  CityService( City& city, const std::string& name ) 
    : _name( name ), _city( city )
  {
  }

protected:
  std::string _name;
  City& _city;
};

typedef SmartPtr<CityService> CityServicePtr;

#endif//__OPENCAESAR3_CITYSERVICE_H_INCLUDED__