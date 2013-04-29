#include "oc3_factorymanager.hpp"
#include "oc3_factoryclay.hpp"
#include "oc3_factory_building.hpp"

class FactoryManager::Impl
{
public:
  std::map<GoodType, Factory*> specimen;
};

FactoryManager& FactoryManager::getInstance()
{
  static FactoryManager inst;
  return inst;
}

std::map<GoodType, Factory*>& FactoryManager::getSpecimen()
{
  return _d->specimen;
}

FactoryManager::FactoryManager()
{
  _d->specimen[G_TIMBER]    = new FactoryTimber();
  _d->specimen[G_FURNITURE] = new FactoryFurniture();
  _d->specimen[G_IRON]      = new FactoryIron();
  _d->specimen[G_WEAPON]    = new FactoryWeapon();
  _d->specimen[G_WINE]      = new FactoryWine();
  _d->specimen[G_OIL]       = new FactoryOil();
  _d->specimen[G_CLAY]      = new FactoryClay();
  _d->specimen[G_POTTERY]   = new FactoryPottery();
  _d->specimen[G_MARBLE]    = new FactoryMarble();
  // ????
  _d->specimen[G_FISH]      = new Wharf();
}