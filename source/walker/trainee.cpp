// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.

#include "trainee.hpp"
#include "gfx/tile.hpp"
#include "core/variant.hpp"
#include "game/path_finding.hpp"
#include "game/city.hpp"
#include "game/name_generator.hpp"

TraineeWalker::TraineeWalker( CityPtr city, const WalkerType traineeType)
  : Walker( city )
{
  _setType( traineeType );
  _originBuilding = NULL;
  _destinationBuilding = NULL;
  _maxDistance = 30;

  init( traineeType );
}

void TraineeWalker::init(const WalkerType traineeType)
{
  switch( traineeType )
  {
  case WT_ACTOR:
    _setGraphic( WG_ACTOR );
    _buildingNeed.push_back(B_THEATER);
    _buildingNeed.push_back(buildingAmphitheater);
  break;

  case WT_GLADIATOR:
    _setGraphic( WG_GLADIATOR );
    _buildingNeed.push_back(buildingAmphitheater);
    _buildingNeed.push_back(B_COLLOSSEUM);
  break;

  case WT_TAMER:
    _setGraphic( WG_TAMER );
    _buildingNeed.push_back(B_COLLOSSEUM);
  break;

  case WT_CHARIOT:
    _setGraphic( WG_NONE );  // TODO
  break;

  default:
  case WT_NONE:
  case WT_MAX:
    break;
  }

  setName( NameGenerator::rand( NameGenerator::male ) );
}

void TraineeWalker::setOriginBuilding(Building &originBuilding)
{
  _originBuilding = &originBuilding;
}

void TraineeWalker::computeWalkerPath()
{
  _maxNeed = 0;  // need of this trainee in buildings
 
  Propagator pathPropagator( _getCity() );
  pathPropagator.init( _originBuilding.as<Construction>() );
  pathPropagator.propagate( _maxDistance );

  foreach( TileOverlayType buildingType, _buildingNeed )
  {
    checkDestination(buildingType, pathPropagator);
  }

  if( _destinationBuilding != NULL )
  {
    // some building needs that trainee!
    // std::cout << "trainee sent!" << std::endl;
    PathWay pathWay;
    pathPropagator.getPath( _destinationBuilding, pathWay);
    setPathWay( pathWay );
    setIJ( _getPathway().getOrigin().getIJ() );
  }
  else
  {
    // nobody needs him...
    // std::cout << "trainee suicide!" << std::endl;
    deleteLater();
  }
}

void TraineeWalker::checkDestination(const TileOverlayType buildingType, Propagator &pathPropagator)
{
  Propagator::Routes pathWayList;
  pathPropagator.getRoutes(buildingType, pathWayList);

  foreach( Propagator::Routes::value_type& item, pathWayList )
  {
    // for every building within range
    BuildingPtr building = item.first;

    float need = building->evaluateTrainee( (WalkerType)getType() );
    if (need > _maxNeed)
    {
      _maxNeed = need;
      _destinationBuilding = building;
    }
  }
}

void TraineeWalker::send2City()
{
  computeWalkerPath();

  if( !isDeleted() )
  {
    _destinationBuilding->reserveTrainee( (WalkerType)getType() );
    _getCity()->addWalker( this );
  }
}

void TraineeWalker::onDestination()
{
  Walker::onDestination();
  deleteLater();
  _destinationBuilding->applyTrainee( (WalkerType)getType() );
}

void TraineeWalker::save( VariantMap& stream ) const
{
  Walker::save( stream );
  stream[ "originBldPos" ] = _originBuilding->getTile().getIJ();
  stream[ "destBldPos" ] = _destinationBuilding->getTile().getIJ();
  stream[ "maxDistance" ] = _maxDistance;
  stream[ "graphic" ] = _getGraphic();
  stream[ "type" ] = (int)WT_TRAINEE;
}

void TraineeWalker::load( const VariantMap& stream )
{
  Walker::load(stream);

  init( (WalkerType)getType() );

  CityHelper helper( _getCity() );
  _originBuilding = helper.find<Building>( stream.get( "originBldPos" ).toTilePos() );
  _destinationBuilding = helper.find<Building>( stream.get( "destBldPos" ).toTilePos() );
  _maxDistance = (int)stream.get( "maxDistance" );
  WalkerType wtype = (WalkerType)stream.get( "graphic" ).toInt();

  _setType( wtype );
  init( wtype );
}

TraineeWalkerPtr TraineeWalker::create(CityPtr city, const WalkerType traineeType )
{
  TraineeWalkerPtr ret( new TraineeWalker( city, traineeType ) );
  ret->drop();
  return ret;
}
