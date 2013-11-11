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
#include "building/constants.hpp"

using namespace constants;

TraineeWalker::TraineeWalker( CityPtr city, walker::Type traineeType)
  : Walker( city )
{
  _setType( traineeType );
  _originBuilding = NULL;
  _destinationBuilding = NULL;
  _maxDistance = 30;

  init( traineeType );
}

void TraineeWalker::init(walker::Type traineeType)
{
  switch( traineeType )
  {
  case walker::actor:
    _setGraphic( WG_ACTOR );
    _buildingNeed.push_back(building::theater);
    _buildingNeed.push_back(building::amphitheater);
  break;

  case walker::gladiator:
    _setGraphic( WG_GLADIATOR );
    _buildingNeed.push_back(building::amphitheater);
    _buildingNeed.push_back(building::colloseum);
  break;

  case walker::tamer:
    _setGraphic( WG_TAMER );
    _buildingNeed.push_back(building::colloseum);
  break;

  case walker::charioter:
    _setGraphic( WG_NONE );  // TODO
  break;

  default:
  case walker::unknown:
  case walker::all:
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

  foreach( TileOverlay::Type buildingType, _buildingNeed )
  {
    checkDestination(buildingType, pathPropagator);
  }

  if( _destinationBuilding != NULL )
  {
    // some building needs that trainee!
    // std::cout << "trainee sent!" << std::endl;
    Pathway pathWay;
    pathPropagator.getPath( _destinationBuilding.as<Construction>(), pathWay);
    setPathway( pathWay );
    setIJ( _getPathway().getOrigin().getIJ() );
  }
  else
  {
    // nobody needs him...
    // std::cout << "trainee suicide!" << std::endl;
    deleteLater();
  }
}

void TraineeWalker::checkDestination(const TileOverlay::Type buildingType, Propagator &pathPropagator)
{
  Propagator::Routes pathWayList = pathPropagator.getRoutes( buildingType );

  foreach( Propagator::Routes::value_type& item, pathWayList )
  {
    // for every building within range
    BuildingPtr building = item.first.as<Building>();

    float need = building->evaluateTrainee( getType() );
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
    _destinationBuilding->reserveTrainee( getType() );
    _getCity()->addWalker( this );
  }
}

void TraineeWalker::onDestination()
{
  Walker::onDestination();
  deleteLater();
  _destinationBuilding->applyTrainee( getType() );
}

void TraineeWalker::save( VariantMap& stream ) const
{
  Walker::save( stream );
  stream[ "originBldPos" ] = _originBuilding->getTile().getIJ();
  stream[ "destBldPos" ] = _destinationBuilding->getTile().getIJ();
  stream[ "maxDistance" ] = _maxDistance;
  stream[ "graphic" ] = _getGraphic();
  stream[ "type" ] = (int)walker::trainee;
}

void TraineeWalker::load( const VariantMap& stream )
{
  Walker::load(stream);

  init( getType() );

  CityHelper helper( _getCity() );
  _originBuilding = helper.find<Building>( building::any, stream.get( "originBldPos" ).toTilePos() );
  _destinationBuilding = helper.find<Building>( building::any, stream.get( "destBldPos" ).toTilePos() );
  _maxDistance = (int)stream.get( "maxDistance" );
  walker::Type wtype = (walker::Type)stream.get( "graphic" ).toInt();

  _setType( wtype );
  init( wtype );
}

TraineeWalkerPtr TraineeWalker::create(CityPtr city, walker::Type traineeType )
{
  TraineeWalkerPtr ret( new TraineeWalker( city, traineeType ) );
  ret->drop();
  return ret;
}
