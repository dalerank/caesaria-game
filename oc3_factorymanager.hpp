#ifndef __OPENCAESAR3_FACTORYMANAGER_H_INCLUDED__
#define __OPENCAESAR3_FACTORYMANAGER_H_INCLUDED__

#include "oc3_scopedptr.hpp"
#include "oc3_smartptr.hpp"
#include "oc3_referencecounted.hpp"
#include "oc3_enums.hpp"
#include "oc3_predefinitions.hpp"

#include <map>

class FactoryManager : public ReferenceCounted
{
public:
  static FactoryManager& getInstance();
  std::map<GoodType, Factory*>& getSpecimen();

private:
  FactoryManager();

  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__OPENCAESAR3_FACTORYMANAGER_H_INCLUDED__