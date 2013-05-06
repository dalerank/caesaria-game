#ifndef __OPENCAESAR3_TRAINEEWALKER_H_INCLUDED__
#define __OPENCAESAR3_TRAINEEWALKER_H_INCLUDED__

#include "oc3_walker.hpp"

/** This walker goes to work */
class TraineeWalker : public Walker
{
public:
  static TraineeWalkerPtr create( const WalkerTraineeType traineeType );
  int getType() const;

  void checkDestination(const BuildingType buildingType, Propagator &pathPropagator);
  void send2City();
  void setOriginBuilding(Building &building);
  void computeWalkerPath();

  virtual void onDestination();

  void serialize(OutputSerialStream &stream);
  void unserialize(InputSerialStream &stream);

protected:
  TraineeWalker(const WalkerTraineeType traineeType);
  void init(const WalkerTraineeType traineeType);

private:
  WalkerTraineeType _traineeType;
  BuildingPtr _originBuilding;
  BuildingPtr _destinationBuilding;
  int _maxDistance;

  std::list<BuildingType> _buildingNeed;  // list of buildings needing this trainee
  float _maxNeed;  // evaluates the need for that trainee
};

#endif