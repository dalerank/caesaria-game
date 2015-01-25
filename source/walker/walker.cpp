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

#include "walker.hpp"

#include "gfx/tile.hpp"
#include "core/enumerator.hpp"
#include "objects/metadata.hpp"
#include "core/exception.hpp"
#include "core/position.hpp"
#include "core/variant_map.hpp"
#include "core/utils.hpp"
#include "pathway/path_finding.hpp"
#include "city/city.hpp"
#include "gfx/animation_bank.hpp"
#include "gfx/tilemap.hpp"
#include "core/logger.hpp"
#include "thinks.hpp"
#include "gfx/helper.hpp"
#include "ability.hpp"
#include "helper.hpp"
#include "core/foreach.hpp"
#include "corpse.hpp"

using namespace constants;
using namespace gfx;

namespace {
static const Tile invalidTile( TilePos(-1,-1) );
}

class Walker::Impl
{
public:
  std::set<Walker::Flag> flags;
  PlayerCityPtr city;
  walker::Type type;
  bool isDeleted;
  float speed;
  Tile* location;
  Walker::UniqueId uid;
  int waitInterval;
  float speedMultiplier;
  Animation animation;  // current animation
  PointF wpos;      // current world position
  PointF subSpeed;
  PointF nextwpos;  // next way point
  Pathway pathway;
  DirectedAction action;
  std::string name;
  float lastCenterDst;
  bool centerReached;
  int health;
  std::string thinks;
  float tileSpeedKoeff;
  AbilityList abilities;
  world::Nation nation;

  float finalSpeed() const   {  return speedMultiplier * speed * tileSpeedKoeff;  }

  void updateSpeedMultiplier( const Tile& tile ) 
  {
    tileSpeedKoeff = (tile.getFlag( Tile::tlRoad ) || tile.getFlag( Tile::tlGarden )) ? 1.f : 0.5f;
  }
};

Walker::Walker(PlayerCityPtr city) : _d( new Impl )
{
  _d->city = city;
  _d->tileSpeedKoeff = 1.f;
  _d->action.action = Walker::acMove;
  _d->action.direction = constants::noneDirection;
  _d->type = walker::unknown;
  _d->health = 100;
  _d->location = 0;
  _d->lastCenterDst = 99.f;
  _d->centerReached = false;
  _d->speed = 1.f; // default speed
  _d->speedMultiplier = 1.f;
  _d->isDeleted = false;
  _d->centerReached = false;
  _d->waitInterval = 0;
  _d->nation = world::unknownNation;

#ifdef DEBUG
  WalkerDebugQueue::instance().add( this );
#endif
}

Walker::~Walker()
{
#ifdef DEBUG
  WalkerDebugQueue::instance().rem( this );
#endif
}

void Walker::timeStep(const unsigned long time)
{
  if( _d->waitInterval > 0 )
  {
    _d->waitInterval--;
    if( _d->waitInterval == 0 )
    {
      _waitFinished();
    }
  }
  else
  {
    switch(_d->action.action)
    {
    case Walker::acMove:
      _walk();

      if( _d->finalSpeed() > 0.f )
      {
        _updateAnimation( time );
      }
    break;

    case Walker::acFight:
      _updateAnimation( time );
    break;

    case Walker::acNone:
    break;

    default:
    break;
    }
  }

  foreach( it, _d->abilities) { (*it)->run( this, time ); }

  if( health() <= 0 )
  {
    die();
  }
}

void Walker::setPos( const TilePos& pos )
{
  _d->location = &_d->city->tilemap().at( pos );
  _d->wpos = _d->location->center().toPointF();

  _computeDirection();
}

void Walker::setPathway( const Pathway& pathway)
{
  _d->pathway = pathway;
  _d->pathway.move( Pathway::forward );
  _d->centerReached = false;
  _centerTile();
}

void Walker::setSpeed(const float speed){   _d->speed = speed;}
float Walker::speed() const{ return _d->speed;}
float Walker::speedMultiplier() const { return _d->speedMultiplier; }
void Walker::setSpeedMultiplier(float koeff) { _d->speedMultiplier = koeff; }

void Walker::_walk()
{
  if( constants::noneDirection == _d->action.direction
      || !_pathwayRef().isValid() )
  {
    // nothing to do
    _noWay();
    _changeDirection();
    return;
  }

  float speedKoeff = 1.f;
  switch( _d->action.direction )
  {
  case constants::north:
  case constants::south:
  case constants::east:
  case constants::west:
  break;

  case constants::northEast:
  case constants::southWest:
  case constants::southEast:
  case constants::northWest:
     speedKoeff = 0.7f;
  break;

  default:
     Logger::warning( "Walker: invalid move direction: %d", _d->action.direction );
     _d->action.action = acNone;
     _d->action.direction = constants::noneDirection;
     return;
  break;
  }

  PointF delta = _d->subSpeed;
  delta.rx() = delta.x() * _d->finalSpeed() * speedKoeff;
  delta.ry() = delta.y() * _d->finalSpeed() * speedKoeff;

  PointF tmp = _d->wpos;
  const int wcell = tilemap::cellSize().height();
  TilePos saveMpos( tmp.x() / wcell , tmp.y() / wcell );
  _d->wpos += delta;

  tmp = _d->wpos;
  TilePos Mpos( tmp.x() / wcell, tmp.y() / wcell );

  if( !_d->centerReached  )
  {
    float crntDst = _d->wpos.getDistanceFrom( _d->nextwpos );
    if( crntDst < _d->lastCenterDst )
    {
      _d->lastCenterDst = crntDst;
    }
    else
    {      
      _d->pathway.next();
      _centerTile();
    }
  }

  if( saveMpos != Mpos )
  {
    _d->location = &_d->city->tilemap().at( Mpos );
    _changeTile();
  }
}

void Walker::_changeTile()
{
   _d->updateSpeedMultiplier( *_d->location );
   _d->centerReached = false;
}

void Walker::_centerTile()
{
  if( _d->centerReached )
      return;

  if( _d->pathway.isDestination() )
  {
    _reachedPathway();
  }
  else
  {
    // compute the direction to reach the destination
    _computeDirection();
    const Tile& tile = _nextTile();
    if( tile.i() < 0 || !tile.isWalkable( true ) )
    {
      _brokePathway( tile.pos() );
    }
    _d->centerReached = true;
  }
}

void Walker::_reachedPathway()
{
  _d->action.action = acNone;  // stop moving
  _d->animation = Animation();
}

void Walker::_computeDirection()
{
  const Direction lastDirection = _d->action.direction;
  _d->action.direction = _d->pathway.direction();
  _d->nextwpos = _nextTile().center().toPointF();
  _d->lastCenterDst = _d->wpos.getDistanceFrom( _d->nextwpos );
  _d->subSpeed = ( _d->nextwpos - _d->wpos ) / tilemap::cellSize().height();

  if( lastDirection != _d->action.direction )
  {
    _changeDirection();
  }
}

const Tile& Walker::_nextTile() const
{
  TilePos p = pos();
  switch( _d->action.direction )
  {
  case constants::north: p += TilePos( 0, 1 ); break;
  case constants::northEast: p += TilePos( 1, 1 ); break;
  case constants::east: p += TilePos( 1, 0 ); break;
  case constants::southEast: p += TilePos( 1, -1 ); break;
  case constants::south: p += TilePos( 0, -1 ); break;
  case constants::southWest: p += TilePos( -1, -1 ); break;
  case constants::west: p += TilePos( -1, 0 ); break;
  case constants::northWest: p += TilePos( -1, 1 ); break;
  default: Logger::warning( "Unknown direction: %d", _d->action.direction); break;
  }

  return _d->city->tilemap().at( p );
}

Point Walker::mappos() const
{
  if( !_d->location )
    return Point( 0, 0 );

  const Tile& tile = *_d->location;
  Point offset;
  if( tile.overlay().isValid() )
      offset = tile.overlay()->offset( tile, tilesubpos() );

  const PointF& p = _d->wpos;
  return Point( 2*(p.x() + p.y()), p.x() - p.y() ) + offset;
}

void Walker::_brokePathway( TilePos pos ){}
void Walker::_noWay(){}
void Walker::_waitFinished() { }

world::Nation Walker::nation() const{ return _d->nation; }
void Walker::_setNation(world::Nation nation) { _d->nation = nation; }
void Walker::_setLocation( Tile* location ){ _d->location = location; }
Walker::Action Walker::action() const {  return (Walker::Action)_d->action.action;}
bool Walker::isDeleted() const{   return _d->isDeleted;}
void Walker::_changeDirection(){  _d->animation = Animation(); } // need to fetch the new animation
walker::Type Walker::type() const{ return _d->type; }
Direction Walker::direction() const {  return _d->action.direction;}
double Walker::health() const{  return _d->health;}
void Walker::updateHealth(double value) {  _d->health = math::clamp( _d->health + value, -100.0, 100.0 );}
void Walker::acceptAction(Walker::Action, TilePos){}
void Walker::setName(const std::string &name) {  _d->name = name; }
const std::string &Walker::name() const{  return _d->name; }
void Walker::addAbility(AbilityPtr ability) {  _d->abilities.push_back( ability );}
TilePos Walker::pos() const{ return _d->location ? _d->location->pos() : TilePos( -1, -1 ) ;}
void Walker::deleteLater(){ _d->isDeleted = true;}
void Walker::setUniqueId( const UniqueId uid ) {  _d->uid = uid;}
Walker::UniqueId Walker::uniqueId() const { return _d->uid; }
Pathway& Walker::_pathwayRef() {  return _d->pathway; }
const Pathway& Walker::pathway() const {  return _d->pathway; }
Animation& Walker::_animationRef() {  return _d->animation;}
const Animation& Walker::_animationRef() const {  return _d->animation;}
void Walker::_setDirection(constants::Direction direction ){  _d->action.direction = direction; }
void Walker::setThinks(std::string newThinks){  _d->thinks = newThinks;}
TilePos Walker::places(Walker::Place type) const { return TilePos(-1,-1); }
void Walker::_setType(walker::Type type){  _d->type = type;}
PlayerCityPtr Walker::_city() const{  return _d->city;}
void Walker::_setHealth(double value){  _d->health = value;}
bool Walker::getFlag(Walker::Flag flag) const{ return _d->flags.count( flag ) > 0; }
const Tile& Walker::tile() const {  return _d->location ? *_d->location : invalidTile; }

void Walker::setFlag(Walker::Flag flag, bool value)
{
  if( value ) _d->flags.insert( flag );
  else _d->flags.erase( flag );
}

Point Walker::tilesubpos() const
{
  Point tmp = Point( _d->location->i(), _d->location->j() ) * 15 + Point( 7, 7 );
  return tmp - _d->wpos.toPoint();
}

void Walker::_setAction( Walker::Action action )
{
  if( _d->action.action != action  )
  {
    _animationRef().clear();
  }
  _d->action.action = action;
}

void Walker::initialize(const VariantMap &options)
{
  VARIANT_LOAD_ANYDEF_D( _d, speed, 1.f, options )

  float tmpSpeedMultiplier = 0.8f + math::random( 40 ) / 100.f;
  VARIANT_LOAD_ANYDEF_D( _d, speedMultiplier, tmpSpeedMultiplier, options )

  std::string nation;
  VARIANT_LOAD_STR( nation, options )
  if( !nation.empty() )
  {
    _d->nation = WalkerHelper::getNation( nation );
  }
}

int Walker::agressive() const { return 0; }

void Walker::attach()
{
  if( _city().isValid() )
    _city()->addWalker( this );
}

std::string Walker::thoughts(Thought about) const
{
  if( thCurrent )
  {
    if( _d->thinks.empty() )
    {
      const_cast< Walker* >( this )->_updateThoughts();
    }

    return _d->thinks;
  }

  return "";
}

void Walker::getPictures(gfx::Pictures& oPics)
{
   oPics.clear();
   oPics.push_back( getMainPicture() );
}

const Picture& Walker::getMainPicture()
{
  if( !_d->animation.isValid() )
  {
    const AnimationBank::MovementAnimation& animMap = AnimationBank::find( type() );
    AnimationBank::MovementAnimation::const_iterator itAnimMap;
    if( _d->action.direction == constants::noneDirection )
    {
      itAnimMap = animMap.find( DirectedAction(_d->action.action, constants::north ) );
    }
    else
    {
      itAnimMap = animMap.find(_d->action);
    }

    if( itAnimMap != animMap.end() )
    {
      _d->animation = itAnimMap->second;
    }
    else
    {
      _d->animation = animMap.begin()->second;
      Logger::warning( "Walker: wrong direction detected" );
    }
  }

  return _d->animation.currentFrame();
}

void Walker::save( VariantMap& stream ) const
{
  VARIANT_SAVE_STR_D( stream, _d, name )
  VARIANT_SAVE_ANY_D( stream, _d, type )
  VARIANT_SAVE_ENUM_D( stream, _d, nation )
  stream[ "pathway" ] =  _d->pathway.save();
  VARIANT_SAVE_ANY_D( stream, _d, health )
  VARIANT_SAVE_ANY_D( stream, _d, isDeleted )
  VARIANT_SAVE_ANY_D( stream, _d, action.action )
  VARIANT_SAVE_ANY_D( stream, _d, action.direction )
  stream[ "location" ] = _d->location->pos();
  VARIANT_SAVE_ANY_D( stream, _d, tileSpeedKoeff )
  VARIANT_SAVE_ANY_D( stream, _d, wpos )
  VARIANT_SAVE_ANY_D( stream, _d, nextwpos )
  VARIANT_SAVE_ANY_D( stream, _d, speed )
  VARIANT_SAVE_ANY_D( stream, _d, speedMultiplier )
  VARIANT_SAVE_ANY_D( stream, _d, uid )
  VARIANT_SAVE_STR_D( stream, _d, thinks )
  VARIANT_SAVE_ANY_D( stream, _d, subSpeed )
  VARIANT_SAVE_ANY_D( stream, _d, lastCenterDst )
  stream[ "showDebugInfo" ] = getFlag( showDebugInfo );
}

void Walker::load( const VariantMap& stream)
{
  Tilemap& tmap = _city()->tilemap();

  VARIANT_LOAD_ENUM_D( _d, nation, stream )
  VARIANT_LOAD_STR_D( _d, name, stream )
  VARIANT_LOAD_ANY_D( _d, wpos, stream )
  TilePos pos = stream.get( "location" ).toTilePos();
  _d->location = &tmap.at( pos );
  VARIANT_LOAD_ANY_D( _d, lastCenterDst, stream )
  _d->pathway.load( tmap, stream.get( "pathway" ).toMap() );
  VARIANT_LOAD_STR_D( _d, thinks, stream )
  VARIANT_LOAD_ANY_D( _d, tileSpeedKoeff, stream )
  VARIANT_LOAD_ANY_D( _d, nextwpos, stream );
  VARIANT_LOAD_ANY_D( _d, isDeleted, stream );
  VARIANT_LOAD_ENUM_D( _d, action.action, stream )
  VARIANT_LOAD_ENUM_D( _d, action.direction, stream )
  VARIANT_LOAD_ENUM_D( _d, uid, stream )
  VARIANT_LOAD_ANY_D( _d, subSpeed, stream )
  VARIANT_LOAD_ANY_D( _d, speedMultiplier, stream )


  if( !_d->pathway.isValid() )
  {
    Logger::warning( "WARNING!!! Walker: wrong way for %s:%s at [%d,%d]",
                     WalkerHelper::getTypename( _d->type ).c_str(), _d->name.c_str(),
                     _d->location->i(), _d->location->j() );
  }
  
  if( _d->speedMultiplier < 0.1 ) //Sometime this have this error in save file
  {
    Logger::warning( "WARNING!!!! Walker: Wrong speed multiplier for %d", _d->uid );
    _d->speedMultiplier = 1;
  }

  VARIANT_LOAD_ANY_D( _d, speed, stream )
  VARIANT_LOAD_ANY_D( _d, health, stream )
  setFlag( showDebugInfo, stream.get( "showDebugInfo" ).toBool() );
}

void Walker::turn(TilePos p )
{
  Direction direction = tilemap::getDirection( pos(), p );

  if( _d->action.direction != direction )
  {
    _d->action.direction = direction;
    _animationRef().clear();
  }
}

void Walker::_updatePathway( const Pathway& pathway)
{
  _d->pathway = pathway;
  _d->pathway.move( Pathway::forward );
  _computeDirection();
}

void Walker::_updateAnimation( const unsigned int time )
{
  if( _d->animation.size() > 0 )
  {
    _d->animation.update( time );
  }
}

void Walker::_setWpos( Point pos) { _d->wpos = pos.toPointF(); }

void Walker::_updateThoughts()
{
  _d->thinks = WalkerThinks::check( this, _city() );
}

Point Walker::_wpos() const{  return _d->wpos.toPoint();}

void Walker::go( float speed )
{
  setSpeed( speed );
  _setAction( acMove );
}

void Walker::wait(int ticks)
{
  _d->waitInterval = ticks;
  if( ticks < 0 )
  {
    _setAction( acNone );
    setSpeed( 0.f );
  }
}

int Walker::waitInterval() const { return _d->waitInterval; }

bool Walker::die()
{
  _d->health = 0;
  deleteLater();

  WalkerPtr corpse = Corpse::create( _city(), this );
  if( corpse.isValid() )
  {
    _city()->addWalker( corpse );
  }
  return corpse.isValid();
}

#ifdef DEBUG
void WalkerDebugQueue::print()
{
  WalkerDebugQueue& inst = (WalkerDebugQueue&)instance();
  if( !inst._pointers.empty() )
  {
    Logger::warning( "PRINT WALKER DEBUG QUEUE" );
    foreach( it, inst._pointers )
    {
      Walker* wlk = (Walker*)*it;
      Logger::warning( "%s - %s [%d,%d] ref:%d", wlk->name().c_str(),
                                          WalkerHelper::getTypename( wlk->type() ).c_str(),
                                          wlk->pos().i(), wlk->pos().j(), wlk->rcount() );
    }
  }
}
#endif
