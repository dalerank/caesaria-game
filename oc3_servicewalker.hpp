#ifndef __OPENCAESAR3_SERVICEWALKER_H_INCLUDED__
#define __OPENCAESAR3_SERVICEWALKER_H_INCLUDED__

#include "oc3_walker.hpp"

/** This walker gives a service to buildings along the road */
class ServiceWalker : public Walker
{
public:
  typedef std::set<BuildingPtr> ReachedBuildings;

  static ServiceWalkerPtr create( BuildingPtr base, const ServiceType service );

  ServiceType getService();
  BuildingPtr getBase();

  void send2City();
  virtual float getServiceValue() const;
  virtual void onDestination();
  virtual void onNewTile();  // called when the walker is on a new tile

  // evaluates the service demand on the given pathWay
  float evaluatePath(PathWay &pathWay);
  void reservePath(PathWay &pathWay);
  ReachedBuildings getReachedBuildings(const TilePos& pos );

  virtual unsigned int getReachDistance() const;

  void setMaxDistance( const int distance );

  void serialize(OutputSerialStream &stream);
  void unserialize(InputSerialStream &stream);

protected:
  ServiceWalker( BuildingPtr base, const ServiceType service );
  void init(const ServiceType service);
  void computeWalkerPath();

private:
  ServiceType _service;
  BuildingPtr _base;
  int _maxDistance;
};

#endif