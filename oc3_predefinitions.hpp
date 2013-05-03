#ifndef __OPENCAESAR3_PREDEFINITIONS_H_INCLUDED__
#define __OPENCAESAR3_PREDEFINITIONS_H_INCLUDED__

#include "oc3_smartptr.hpp"
#include <list>

class Walker;
typedef SmartPtr<Walker> WalkerPtr;

typedef std::list< WalkerPtr > Walkers;

class CartPusher;
typedef SmartPtr< CartPusher > CartPusherPtr;

#endif