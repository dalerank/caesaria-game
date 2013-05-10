#ifndef __OPENCAESAR3_MARKET_H_INCLUDED__
#define __OPENCAESAR3_MARKET_H_INCLUDED__

#include "oc3_service_building.hpp"
#include "oc3_predefinitions.hpp"

class Market: public ServiceBuilding
{
public:
  Market();

  SimpleGoodStore& getGoodStore();
  std::list<GoodType> getMostNeededGoods();  // used by the market buyer

  // returns the quantity of needed goods
  int getGoodDemand(const GoodType &goodType);  // used by the market buyer

  void save( VariantMap& stream) const;
  void load( const VariantMap& stream);

  void deliverService();

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

#endif