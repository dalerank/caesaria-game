// This file is part of CaesarIA.
//
// CaesarIA is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CaesarIA is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CaesarIA.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "fort.hpp"
#include "constants.hpp"
#include "game/resourcegroup.hpp"
#include "city/statistic.hpp"
#include "gfx/tilemap.hpp"
#include "walker/romesoldier.hpp"
#include "core/logger.hpp"
#include "events/event.hpp"
#include "walker/patrolpoint.hpp"
#include "barracks.hpp"
#include "game/gamedate.hpp"
#include "game/settings.hpp"
#include "core/saveadapter.hpp"
#include "walker/romehorseman.hpp"
#include "walker/helper.hpp"
#include "walker/romearcher.hpp"
#include "core/stacktrace.hpp"
#include "world/playerarmy.hpp"
#include "world/empire.hpp"
#include "core/variant_map.hpp"
#include "events/clearland.hpp"
#include "city/build_options.hpp"
#include "city/statistic.hpp"

using namespace gfx;
using namespace events;
using namespace city;

namespace {
Renderer::Pass _fpq[] = { Renderer::overlayAnimation };
static Renderer::PassQueue fortPassQueue( _fpq, _fpq + 2 );
TilePos fortAreaOffset( 3, 0);

class LegionEmblem
{
public:
  std::string name;
  Picture pic;

  static LegionEmblem findFree( PlayerCityPtr city );
};

GAME_LITERALCONST(name)
GAME_LITERALCONST(img)

}

LegionEmblem LegionEmblem::findFree( PlayerCityPtr city )
{
  FortList forts = city->statistic().objects.find<Fort>();
  std::vector<LegionEmblem> availableEmblems;

  VariantMap emblemsModel = config::load( SETTINGS_RC_PATH( emblemsModel ) );
  for( auto& it : emblemsModel )
  {
    VariantMap vm_emblem = it.second.toMap();
    LegionEmblem newEmblem;

    newEmblem.name = vm_emblem[ literals::name ].toString();
    newEmblem.pic.load( vm_emblem[ literals::img ].toString() );

    if( !newEmblem.name.empty() && newEmblem.pic.isValid() )
    {
      availableEmblems.push_back( newEmblem );
    }
  }

  for( auto f : forts )
  {
    foreach( it, availableEmblems )
    {
      if( f->legionEmblem().name() == (*it).pic.name() )
      {
        availableEmblems.erase( it );
        break;
      }
    }
  }

  return availableEmblems.size() > 0
                       ? availableEmblems[ math::random( availableEmblems.size()-1 ) ]
                       : LegionEmblem();
}

class PatrolArea
{
public:
  TilePos getPos( unsigned int index ) const
  {
    if( index < positions.size() )
        return location + positions[ index ];

    return TilePos::invalid();
  }

  void expand()
  {
    switch( mode )
    {
    case Fort::frmOpen: distance.open++; break;
    case Fort::frmWestLine: distance.westLine++; break;
    case Fort::frmWestDblLine: distance.westDblLine++; break;
    case Fort::frmNorthLine: distance.northLine++; break;
    case Fort::frmNorthDblLine: distance.northDblLine++; break;
    case Fort::frmSquad: distance.squad++; break;
    case Fort::frmParade: distance.parade++; break;
    default:
      Logger::warning( "WARNING !!! PatrolArea: expand called" );
      return;
    break;
    }
  }

  TilePos append( Tilemap& tmap, unsigned int index )
  {
    int expandCounter = 0;
    do
    {
      if( index < positions.size() )
         return location + positions[ index ];

      expand();
      updatePoints( tmap );
    }
    while( ++expandCounter < 4 );

    return TilePos::invalid();
  }

  void updatePoints( Tilemap& tmap )
  {
    positions.clear();
    TilesArea area;
    fillArea( tmap, area );
    Locations locations = area.walkables( true ).locations();

    for( auto& r : locations )
      positions.push_back( r - location );
  }

  void fillArea( Tilemap& tmap, TilesArea& area )
  {
    switch( mode )
    {
    case Fort::frmOpen:
      area = TilesArea( tmap, distance.open, location );
    break;

    case Fort::frmWestLine:
      area.push_back( &tmap.at( location ) );
      for( unsigned int range=1; range < distance.westLine; range++ )
      {
        area.push_back( &tmap.at( location - TilePos( 0, range ) ) );
        area.push_back( &tmap.at( location + TilePos( 0, range ) ) );
      }
    break;

    case Fort::frmWestDblLine:
      area.push_back( &tmap.at( location ) );
      area.push_back( &tmap.at( location + TilePos( 0, 1 ) ) );
      for( unsigned int range=1; range < distance.westDblLine; range++ )
      {
        area.push_back( &tmap.at( location - TilePos( 0, range ) ) );
        area.push_back( &tmap.at( location - TilePos( -1, range ) ) );
        area.push_back( &tmap.at( location + TilePos( 0, range ) ) );
        area.push_back( &tmap.at( location + TilePos( 1, range ) ) );
      }
    break;

    case Fort::frmNorthLine:
      area.push_back( &tmap.at( location ) );
      for( unsigned int range=1; range < distance.northLine; range++ )
      {
        area.push_back( &tmap.at( location - TilePos( range, 0 ) ) );
        area.push_back( &tmap.at( location + TilePos( range, 0 ) ) );
      }
    break;

    case Fort::frmNorthDblLine:
      area.push_back( &tmap.at( location ) );
      area.push_back( &tmap.at( location - TilePos( 1, 0 ) ) );
      for( unsigned int range=1; range < distance.northDblLine; range++ )
      {
        area.push_back( &tmap.at( location - TilePos( range,0 ) ) );
        area.push_back( &tmap.at( location - TilePos( range,1 ) ) );
        area.push_back( &tmap.at( location + TilePos( range,0 ) ) );
        area.push_back( &tmap.at( location + TilePos( range,-1 ) ) );
      }
    break;

    case Fort::frmParade:
      area = TilesArea( tmap, location - TilePos( 0, distance.parade ), location + TilePos( distance.parade, 0 ) );
    break;

    case Fort::frmSquad:
    default:
      area = TilesArea( tmap, distance.squad, location );
    break;
    }
  }

  void setMode( Tilemap& tmap, Fort::TroopsFormation formation )
  {
    if( mode != formation )
       mode = formation;

    distance.reset();
    updatePoints( tmap );
  }

  TilePos location;
  Locations positions;
  struct {
    unsigned int open = 3;
    unsigned int westLine = 5;
    unsigned int westDblLine = 4;
    unsigned int northLine = 5;
    unsigned int northDblLine = 4;
    unsigned int parade = 3;
    unsigned int squad = 3;

    void reset()
    {
      open = 3;
      westLine = 5;
      westDblLine = 4;
      northLine = 5;
      northDblLine = 4;
      parade = 3;
      squad = 3;
    }
  } distance;
  Fort::TroopsFormation mode;
};

class Fort::Impl
{
public:
  FortAreaPtr area;
  unsigned int maxSoldier;
  PatrolPointPtr patrolPoint;
  LegionEmblem emblem;
  int flagIndex;
  PatrolArea patrolArea;
  Fort::TroopsFormations availableFormations;

  std::string expeditionName;
  bool attackAnimals;
};

class FortArea::Impl
{
public:
  TilePos basePos;
  bool isFlat;
};

FortArea::FortArea() : Building( object::fortArea, Size(4) ),
  _d( new Impl )
{
  setPicture( ResourceGroup::security, 13 );

  _d->isFlat = true; //picture().height() <= picture().width() / 2;

  setState( pr::inflammability, 0 );
  setState( pr::collapsibility, 0 );
}

FortArea::~FortArea() {}

bool FortArea::isFlat() const { return _d->isFlat; }
bool FortArea::isWalkable() const{ return true;}

void FortArea::destroy()
{
  Building::destroy();
  if( base().isValid() )
  {
    events::dispatch<ClearTile>( _d->basePos );
  }
}

void FortArea::setBase(FortPtr base)
{
  if( base.isValid() )
  {
    _d->basePos = base->pos();
  }
}

FortPtr FortArea::base() const
{
  return _map().overlay( _d->basePos ).as<Fort>();
}

Fort::Fort(object::Type type, int picIdLogo) : WorkingBuilding( type, Size(3) ),
  _d( new Impl )
{
  Picture logo(ResourceGroup::security, picIdLogo );
  logo.setOffset( Point( 80, 10 ) );

  Picture area(ResourceGroup::security, 13 );
  area.addOffset( TilePos( 3, 0).toScreenCoordinates() );

  _fgPictures().resize(2);
  _fgPicture( 0 ) = logo;
  _fgPicture( 1 ) = area;

  _d->area = new FortArea();
  _d->area->drop();
  _d->flagIndex = 21;
  _d->maxSoldier = 16;
  _d->patrolArea.mode = frmSquad;
  _d->attackAnimals = false;

  setState( pr::inflammability, 0 );
  setState( pr::collapsibility, 0 );
  setState( pr::destroyable, 0 );
}

void Fort::_check4newSoldier()
{
  int traineeLevel = traineeValue( walker::soldier );
  bool canProduceNewSoldier = (traineeLevel > 100);
  bool haveRoom4newSoldier = (walkers().size() < _d->maxSoldier);
  // all trainees are there for the create soldier!
  if( canProduceNewSoldier && haveRoom4newSoldier )
  {
     _readyNewSoldier();
     setTraineeValue( walker::soldier, math::clamp<int>( traineeLevel - 100, 0, _d->maxSoldier * 100 ) );
  }
}
float Fort::evaluateTrainee(walker::Type traineeType)
{
  int currentForce = walkers().size() * 100;
  int traineeForce = WorkingBuilding::evaluateTrainee( traineeType );
  int maxForce = _d->maxSoldier * 100;

  return ( maxForce - currentForce - traineeForce ) / 16;
}

Fort::~Fort() {}

void Fort::timeStep( const unsigned long time )
{
  if( game::Date::isWeekChanged() )
  {
    _check4newSoldier();
  }

  WorkingBuilding::timeStep( time );
}

bool Fort::canDestroy() const { return state( pr::destroyable ) > 0; }
Fort::TroopsFormation Fort::formation() const { return _d->patrolArea.mode; }
void Fort::setFormation(Fort::TroopsFormation formation) { _d->patrolArea.setMode( _map(), formation ); }

TilesArray Fort::enterArea() const
{
  TilesArray tiles = WorkingBuilding::enterArea();

  Tile& rtile = _map().at( pos() + TilePos( 1, -1 ) );
  if( rtile.isWalkable( true ) )
  {
    tiles.insert( tiles.begin(), &rtile );
  }

  return tiles;
}

int Fort::flagIndex() const { return 0; }

void Fort::destroy()
{
  WorkingBuilding::destroy();

  if( _d->area.isValid()  )
  {
    events::dispatch<ClearTile>( _d->area->pos() );
    _d->area = 0;
  }

  if( _d->patrolPoint.isValid() )
  {
    _d->patrolPoint->deleteLater();
    _d->patrolPoint = 0;
  }
}

TilePos Fort::findSlot(WalkerPtr who) const
{
  TilePos patrolPos;
  if( _d->patrolPoint.isNull()  )
  {
    Logger::warning( "Not patrol point assign in fort [{},{}]", pos().i(), pos().j() );
    patrolPos = _d->area->pos() + TilePos( 0, 3 );
  }
  else
  {
    patrolPos = _d->patrolPoint->pos();
    _d->patrolArea.location = patrolPos;
  }

  int index = walkers().indexOf( who );
  TilePos sldLocation =_d->patrolArea.getPos( index );
  if( !config::tilemap.isValidLocation( sldLocation ) )
  {
    sldLocation = _d->patrolArea.append( _map(), index );
  }

  return sldLocation;
}

void Fort::changePatrolArea()
{
  auto mySoldiers = walkers().select<RomeSoldier>();

  TroopsFormation formation = (patrolLocation() == _d->area->pos() + TilePos( 0, 3 )
                                         ? frmParade
                                         : _d->patrolArea.mode );
  _d->patrolArea.location = patrolLocation();
  _d->patrolArea.setMode( _map(), formation );

  for( auto soldier : mySoldiers )
    soldier->send2patrol();
}

TilePos Fort::patrolLocation() const
{
  TilePos patrolPos;
  if( _d->patrolPoint.isNull()  )
  {
    Logger::warning( "!!! WARNING: Fort::patrolLocation(): not patrol point assign in fort [{0},{1}]", pos().i(), pos().j() );
    patrolPos = _d->area->pos() + TilePos( 0, 3 );
    crashhandler::printstack(false);
  }
  else
  {
    patrolPos = _d->patrolPoint->pos();
  }

  return patrolPos;
}

Picture Fort::legionEmblem() const { return _d->emblem.pic; }
std::string Fort::legionName() const{  return _d->emblem.name; }
Fort::TroopsFormations Fort::legionFormations() const { return _d->availableFormations; }

unsigned int Fort::legionHealth() const
{
  SoldierList sldrs = soldiers();
  if( sldrs.empty() )
    return 0;

  unsigned int health = 0;
  for( auto sldr : sldrs) { health += sldr->health(); }
  return health / sldrs.size();
}

unsigned int Fort::legionTrained() const
{
  SoldierList sldrs = soldiers();
  if( sldrs.empty() )
    return 0;

  unsigned int trained = 0;
  for( auto sldr : sldrs) { trained += sldr->experience(); }
  return trained / sldrs.size();
}

int Fort::legionMorale() const
{
  SoldierList sldrs = soldiers();
  if( sldrs.empty() )
    return 0;

  int morale = 0;
  for( auto sldr : sldrs) { morale += sldr->morale(); }
  return morale / sldrs.size();
}

void Fort::save(VariantMap& stream) const
{
  WorkingBuilding::save( stream );

  VARIANT_SAVE_ANY_D ( stream, _d, maxSoldier )
  VARIANT_SAVE_ANY_D ( stream, _d, attackAnimals )
  VARIANT_SAVE_ANY_D ( stream, _d, patrolArea.location )
  VARIANT_SAVE_ENUM_D( stream, _d, patrolArea.mode )
}

void Fort::load(const VariantMap& stream)
{
  WorkingBuilding::load( stream );

  VARIANT_LOAD_ANYDEF_D( _d, patrolArea.location, TilePos::invalid(), stream )
  VARIANT_LOAD_ANY_D   ( _d, maxSoldier,                              stream )
  VARIANT_LOAD_ANY_D   ( _d, attackAnimals,                           stream )
  VARIANT_LOAD_ENUM_D  ( _d, patrolArea.mode,                         stream )

  if( !config::tilemap.isValidLocation( _d->patrolArea.location ) )
    _d->patrolArea.location = pos() + TilePos( 3, 4 );

  _d->patrolPoint->setPos( _d->patrolArea.location );
}

SoldierList Fort::soldiers() const {  return walkers().select<Soldier>(); }
int Fort::soldiers_n() const { return walkers().count<Soldier>(); }

void Fort::returnSoldiers()
{
  if( _d->patrolPoint.isValid() )
  {
    _d->patrolPoint->setPos( _d->area->pos() + TilePos( 0, 3 ) );
    changePatrolArea();
  }
}

world::PlayerArmyPtr Fort::expedition() const
{
  world::PlayerArmyPtr ret;
  ret << _city()->empire()->findObject( _d->expeditionName );

  return ret;
}

void Fort::sendExpedition(Point location)
{
  auto playerArmy = world::PlayerArmy::create( _city()->empire(), _city().as<world::City>() );
  playerArmy->setFortPos( pos() );

  auto soldiers = walkers().select<RomeSoldier>();

  playerArmy->move2location( location );
  playerArmy->addSoldiers( soldiers );

  playerArmy->attach();

  _d->expeditionName = playerArmy->name();

  for( auto it : soldiers )
    it->send2expedition( playerArmy->name() );
}

void Fort::setAttackAnimals(bool value) { _d->attackAnimals = value; }
void Fort::resetExpedition() { _d->expeditionName.clear(); }

bool Fort::isAttackAnimals() const { return _d->attackAnimals; }
void Fort::_setPatrolPoint(PatrolPointPtr patrolPoint) {  _d->patrolPoint = patrolPoint; }
void Fort::_setEmblem(Picture pic) { _d->emblem.pic = pic; }
void Fort::_setName(const std::string& name) { _d->emblem.name = name; }
int  Fort::_setFlagIndex( int index ) { return _d->flagIndex = index; }

void Fort::_addFormation(Fort::TroopsFormation formation)
{
  _d->availableFormations.push_back( formation );
}

bool Fort::canBuild( const city::AreaInfo& areaInfo ) const
{
  bool isFreeFort = Building::canBuild( areaInfo );
  city::AreaInfo fortArea = areaInfo;
  TilePos fortAreaOfffset( 3, 0);
  fortArea.pos += fortAreaOfffset;
  bool isFreeArea = _d->area->canBuild( fortArea );

  return (isFreeFort && isFreeArea);
}

bool Fort::build( const city::AreaInfo& info )
{
  size_t forts_n = info.city->statistic().objects.count<Fort>();

  const city::development::Options& bOpts = info.city->buildOptions();
  if( forts_n >= bOpts.maximumForts() )
  {
    _setError( "##not_enought_place_for_legion##" );
    return false;
  }

  Building::build( info );

  city::AreaInfo areaInfo = info;
  areaInfo.pos += fortAreaOffset;
  _d->area->build( areaInfo );
  _d->area->setBase( this );

  _d->emblem = LegionEmblem::findFree( info.city );

  info.city->addOverlay( _d->area.object() );

  _fgPictures().resize(1);

  int barracks_n = info.city->statistic().objects.count<Barracks>();

  if( !barracks_n )
  {
    _setError( "##need_barracks_for_work##" );
  }

  auto pp = Walker::create<PatrolPoint>( info.city, this,
                                         ResourceGroup::sprites, _d->flagIndex, 8,
                                         info.pos + TilePos( 3, 3 ) );
  pp->attach();
  _setPatrolPoint( pp );
  _d->patrolArea.location = pp->pos();
  setFormation( frmParade );

  return true;
}

bool Fort::isNeedRoad() const {  return false; }
