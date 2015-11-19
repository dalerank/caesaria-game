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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#include "walker.hpp"

#include "gfx/tile.hpp"
#include "core/enumerator.hpp"
#include "objects/metadata.hpp"
#include "core/exception.hpp"
#include "core/position.hpp"
#include "objects/building.hpp"
#include "core/variant_map.hpp"
#include "core/utils.hpp"
#include "pathway/path_finding.hpp"
#include "city/city.hpp"
#include "gfx/animation_bank.hpp"
#include "gfx/tilemap.hpp"
#include "core/logger.hpp"
#include "thinks.hpp"
#include "gfx/tilemap_config.hpp"
#include "ability.hpp"
#include "name_generator.hpp"
#include "helper.hpp"
#include "core/foreach.hpp"
#include "walkers_factory.hpp"
#include "corpse.hpp"

using namespace gfx;

namespace {
const Tile invalidTile( gfx::tilemap::invalidLocation() );
}

class Walker::Impl
{
public:
  std::set<Walker::Flag> flags;
  PlayerCityPtr city;
  walker::Type type;
  Walker::UniqueId uid;
  Animation animation;  // current animation
  std::string name;
  std::string thinks;
  AbilityList abilities;
  world::Nation nation;

  DirectedAction action;

  struct
  {
    bool deleted;
    int health;
    int wait;
  } state;

  struct
  {
    float multiplier;
    PointF vector;
    float tileKoeff;
    float value;
    float current() const {  return multiplier * value * tileKoeff; }

    void update( const Tile& tile )
    {
      tileKoeff = (tile.getFlag( Tile::tlRoad ) || tile.getFlag( Tile::tlGarden )) ? 1.f : 0.5f;
    }
  } speed;

  struct
  {
    Tile* tile;
    Point pos;
    bool inCenter;
    Point  offset;
    Pathway path;
  } map;

  struct
  {
    PointF next;  // next way point
    PointF pos;      // current world position
    float lastDst;

    inline float dst2next() const { return pos.getDistanceFrom( next ); }
    inline PointF delta() const { return next - pos; }
  } world;

  void reset( PlayerCityPtr pcity );
};

void Walker::_awake()
{
  setName( NameGenerator::rand( gender() == male ? NameGenerator::plebMale : NameGenerator::plebFemale ) );
  initialize( WalkerHelper::getOptions( type() ) );
}

WalkerPtr Walker::create(walker::Type type, PlayerCityPtr city)
{
  auto wlk = WalkerManager::instance().create( type, city );
  if( wlk.isValid() )
    wlk->_awake();

  return wlk;
}

Walker::Walker(PlayerCityPtr city)
  : _d( new Impl )
{
  _d->reset( city );

#ifdef DEBUG
  WalkerDebugQueue::instance().add( this );
#endif
}

Walker::Walker(PlayerCityPtr city, walker::Type type)
  : _d( new Impl )
{
  _d->reset( city );
  _d->type = type;

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
  if( _d->state.wait > 0 )
  {
    _d->state.wait--;
    if( _d->state.wait == 0 )
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

      if( _d->speed.current() > 0.f )
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

  for( auto abil : _d->abilities) { abil->run( this, time ); }

  if( health() <= 0 )
  {
    die();
  }
}

void Walker::setPos( const TilePos& pos )
{
  _d->map.tile = &_map().at( pos );
  _d->world.pos = _d->map.tile->center().toPointF();

  _computeDirection();
  _updateMappos();
}

void Walker::setPathway( const Pathway& pathway)
{
  _d->map.path = pathway;
  _d->map.path.move( Pathway::forward );
  _d->map.inCenter = false;
  _centerTile();
}

void Walker::setSpeed(const float speed){   _d->speed.value = speed;}
float Walker::speed() const{ return _d->speed.value;}
float Walker::speedMultiplier() const { return _d->speed.multiplier; }
void Walker::setSpeedMultiplier(float koeff) { _d->speed.multiplier = koeff; }

void Walker::_walk()
{
  if( !_d->action.valid() || !_pathway().isValid() )
  {
    // nothing to do
    _noWay();
    _changeDirection();
    return;
  }

  float speedKoeff = 1.f;
  switch( _d->action.direction )
  {
  case direction::north:
  case direction::south:
  case direction::east:
  case direction::west:
  break;

  case direction::northEast:
  case direction::southWest:
  case direction::southEast:
  case direction::northWest:
     speedKoeff = 0.7f;
  break;

  default:
     Logger::warning( "Walker: invalid move direction: {}", _d->action.direction );
     _d->action.action = acNone;
     _d->action.direction = direction::none;
     return;
  break;
  }

  PointF delta = _d->speed.vector * _d->speed.current() * speedKoeff;

  PointF tmp = _d->world.pos;
  const int wcell = tilemap::cellSize().height();
  TilePos saveMpos( tmp.x() / wcell, tmp.y() );
  _d->world.pos += delta;
  _updateMappos();

  tmp = _d->world.pos;
  TilePos currentMapPos( tmp.x() / wcell, tmp.y() / wcell );

  if( !_d->map.inCenter )
  {
    float crntDst = _d->world.dst2next();
    if( crntDst < _d->world.lastDst )
    {
      _d->world.lastDst = crntDst;
    }
    else
    {      
      _d->map.path.next();
      _centerTile();
    }
  }

  if( saveMpos != currentMapPos )
  {
    _d->map.tile = &_map().at( currentMapPos );
    _changeTile();
  }
}

void Walker::_updateMappos()
{
  if( !_d->map.tile )
  {
    _d->map.pos = Point( 0, 0 );
    return;
  }

  const Tile& tile = *_d->map.tile;
  Point offset;
  if( tile.overlay().isValid() )
      offset = tile.overlay()->offset( tile, tilesubpos() );

  const PointF& p = _d->world.pos;
  _d->map.pos = Point( 2*(p.x() + p.y()), p.x() - p.y() ) + offset + _d->map.offset;
}

void Walker::_changeTile()
{
   _d->speed.update( *_d->map.tile );
   _d->map.inCenter = false;
}

void Walker::_centerTile()
{
  if( _d->map.inCenter )
      return;

  if( _d->map.path.isDestination() )
  {
    _reachedPathway();
  }
  else
  {
    // compute the direction to reach the destination
    _computeDirection();
    const Tile& tile = _nextTile();
    bool nextTileBlocked = !gfx::tilemap::isValidLocation( tile.epos() ) || !tile.isWalkable( true );

    if( nextTileBlocked  )
    {
      _brokePathway( tile.pos() );
    }
    _d->map.inCenter = true;
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
  _d->action.direction = _d->map.path.direction();
  _d->world.next = _nextTile().center().toPointF();
  _d->world.lastDst = _d->world.dst2next();
  _d->speed.vector = _d->world.delta() / tilemap::cellSize().height();

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
  case direction::north: p += TilePos( 0, 1 ); break;
  case direction::northEast: p += TilePos( 1, 1 ); break;
  case direction::east: p += TilePos( 1, 0 ); break;
  case direction::southEast: p += TilePos( 1, -1 ); break;
  case direction::south: p += TilePos( 0, -1 ); break;
  case direction::southWest: p += TilePos( -1, -1 ); break;
  case direction::west: p += TilePos( -1, 0 ); break;
  case direction::northWest: p += TilePos( -1, 1 ); break;
  default: /*Logger::warning( "Unknown direction: %d", _d->action.direction);*/ break;
  }

  return _map().at( p );
}

const Point& Walker::mappos() const { return _d->map.pos;}
void Walker::_brokePathway( TilePos pos ){}
void Walker::_noWay(){}
void Walker::_waitFinished() { }

world::Nation Walker::nation() const{ return _d->nation; }
void Walker::_setNation(world::Nation nation) { _d->nation = nation; }
void Walker::_setLocation( Tile* location ){ _d->map.tile = location; }
void Walker::_setLocation(const TilePos& pos) { _setLocation( &_map().at( pos ) ); }
Walker::Action Walker::action() const { return (Walker::Action)_d->action.action;}
Walker::Gender Walker::gender() const { return male; }
bool Walker::isDeleted() const{   return _d->state.deleted;}
void Walker::_changeDirection(){  _d->animation = Animation(); } // need to fetch the new animation
walker::Type Walker::type() const{ return _d->type; }
Direction Walker::direction() const {  return _d->action.direction;}
double Walker::health() const{  return _d->state.health;}
void Walker::updateHealth(double value) {  _d->state.health = math::clamp( _d->state.health + value, -100.0, 100.0 );}
void Walker::acceptAction(Walker::Action, TilePos){}
void Walker::setName(const std::string &name) {  _d->name = name; }
const std::string &Walker::name() const{  return _d->name; }
void Walker::addAbility(AbilityPtr ability) {  _d->abilities.push_back( ability );}
TilePos Walker::pos() const{ return _d->map.tile ? _d->map.tile->epos() : gfx::tilemap::invalidLocation() ;}
void Walker::deleteLater(){ _d->state.deleted = true;}
void Walker::setUniqueId( const UniqueId uid ) {  _d->uid = uid;}
Walker::UniqueId Walker::uniqueId() const { return _d->uid; }
Pathway& Walker::_pathway() { return _d->map.path; }
const Pathway& Walker::pathway() const {  return _d->map.path; }
Animation& Walker::_animation() {  return _d->animation;}
const Animation& Walker::_animation() const {  return _d->animation;}
void Walker::_setDirection(Direction direction ){  _d->action.direction = direction; }
void Walker::setThinks(std::string newThinks){  _d->thinks = newThinks;}
TilePos Walker::places(Walker::Place type) const { return gfx::tilemap::invalidLocation(); }
void Walker::_setType(walker::Type type){  _d->type = type;}
PlayerCityPtr Walker::_city() const{  return _d->city;}
void Walker::_setHealth(double value){  _d->state.health = value;}
bool Walker::getFlag(Walker::Flag flag) const{ return _d->flags.count( flag ) > 0; }
const Tile& Walker::tile() const {  return _d->map.tile ? *_d->map.tile : invalidTile; }

Tilemap& Walker::_map() const
{
  if( _city().isValid() )
    return _city()->tilemap();

  Logger::warning( "!!! WARNING: City is null at Walker::_map()" );
  return gfx::tilemap::getInvalid();
}

void Walker::setFlag(Walker::Flag flag, bool value)
{
  if( value ) _d->flags.insert( flag );
  else _d->flags.erase( flag );
}

Point Walker::tilesubpos() const
{
  Point tmp = Point( _d->map.tile->i(), _d->map.tile->j() ) * 15 + Point( 7, 7 );
  return tmp - _d->world.pos.toPoint();
}

void Walker::_setAction( Walker::Action action )
{
  if( _d->action.action != action  )
  {
    _animation().clear();
  }
  _d->action.action = action;
}

void Walker::initialize(const VariantMap &options)
{
  VARIANT_LOAD_ANYDEF_D( _d, speed.value, 1.f, options )

  float tmpSpeedMultiplier = 0.8f + math::random( 40 ) / 100.f;
  VARIANT_LOAD_ANYDEF_D( _d, speed.multiplier, tmpSpeedMultiplier, options )

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
  if( _city().isValid() && !isDeleted() )
    _city()->addWalker( this );
}

std::string Walker::thoughts(Thought about) const
{
  if( about == thCurrent )
  {
    if( _d->thinks.empty() )
    {
      const_cast< Walker* >( this )->_updateThoughts();
    }

    return _d->thinks;
  }

  Logger::warning( "WARNING : no thougths for walker " + WalkerHelper::getPrettyTypename( type() ) );
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
    if( !_d->action.valid() )
    {
      itAnimMap = animMap.find( DirectedAction(_d->action.action, direction::north ) );
    }
    else
    {
      itAnimMap = animMap.find( _d->action );
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
  VARIANT_SAVE_CLASS_D( stream, _d, map.path )
  VARIANT_SAVE_ANY_D( stream, _d, state.health )
  VARIANT_SAVE_ANY_D( stream, _d, state.deleted )
  VARIANT_SAVE_ANY_D( stream, _d, action.action )
  VARIANT_SAVE_ANY_D( stream, _d, action.direction )
  stream[ "location" ] = pos();
  VARIANT_SAVE_ANY_D( stream, _d, speed.tileKoeff )
  VARIANT_SAVE_ANY_D( stream, _d, world.pos )
  VARIANT_SAVE_ANY_D( stream, _d, world.next )
  VARIANT_SAVE_ANY_D( stream, _d, speed.value )
  VARIANT_SAVE_ANY_D( stream, _d, speed.multiplier )
  VARIANT_SAVE_ANY_D( stream, _d, uid )
  VARIANT_SAVE_STR_D( stream, _d, thinks )
  VARIANT_SAVE_ANY_D( stream, _d, speed.vector )
  VARIANT_SAVE_ANY_D( stream, _d, world.lastDst )
  VARIANT_SAVE_ANY_D( stream, _d, map.offset)
  stream[ "showDebugInfo" ] = getFlag( showDebugInfo );
}

void Walker::load( const VariantMap& stream)
{

  VARIANT_LOAD_ENUM_D( _d, nation, stream )
  VARIANT_LOAD_STR_D( _d, name, stream )
  VARIANT_LOAD_ANY_D( _d, world.pos, stream )
  TilePos pos = stream.get( "location" ).toTilePos();
  _d->map.tile = &_map().at( pos );
  VARIANT_LOAD_ANY_D( _d, world.lastDst, stream )
  _d->map.path.load( _map(), stream.get( "map.path" ).toMap() );
  VARIANT_LOAD_STR_D( _d, thinks, stream )
  VARIANT_LOAD_ANY_D( _d, speed.tileKoeff, stream )
  VARIANT_LOAD_ANY_D( _d, world.next, stream );
  VARIANT_LOAD_ANY_D( _d, state.deleted, stream );
  VARIANT_LOAD_ENUM_D( _d, action.action, stream )
  VARIANT_LOAD_ENUM_D( _d, action.direction, stream )
  VARIANT_LOAD_ENUM_D( _d, uid, stream )
  VARIANT_LOAD_ANY_D( _d, map.offset, stream )
  VARIANT_LOAD_ANY_D( _d, speed.vector, stream )
  VARIANT_LOAD_ANY_D( _d, speed.multiplier, stream )

  if( !_d->map.path.isValid() )
  {
    Logger::warning( "WARNING!!! Walker: wrong way for {0}:{1} at [{2},{3}]",
                     WalkerHelper::getTypename( _d->type ), _d->name,
                     _d->map.tile->i(), _d->map.tile->j() );
  }
  
  if( _d->speed.multiplier < 0.1 ) //Sometime this have this error in save file
  {
    Logger::warning( "WARNING!!!! Walker: Wrong speed multiplier for {0}", _d->uid );
    _d->speed.multiplier = 1;
  }

  VARIANT_LOAD_ANY_D( _d, speed.value, stream )
  VARIANT_LOAD_ANY_D( _d, state.health, stream )
  setFlag( showDebugInfo, stream.get( "showDebugInfo" ).toBool() );
}

void Walker::turn(TilePos p)
{
  Direction direction = tilemap::getDirection( pos(), p );

  if( _d->action.direction != direction )
  {
    _d->action.direction = direction;
    _animation().clear();
  }
}

void Walker::_updatePathway( const Pathway& pathway)
{
  _d->map.path = pathway;
  _d->map.path.move( Pathway::forward );
  _computeDirection();
}


void Walker::_updateAnimation( const unsigned int time )
{
  if( _d->animation.size() > 0 )
  {
    _d->animation.update( time );
  }
}

Point& Walker::_rndOffset() { return _d->map.offset; }

void Walker::_setWpos( const Point& pos)
{
  _d->world.pos = pos.toPointF();
  _updateMappos();
}

void Walker::_updateThoughts()
{
  _d->thinks = WalkerThinks::check( this, _city() );
}

Point Walker::wpos() const { return _d->world.pos.toPoint(); }

void Walker::go( float speed )
{
  setSpeed( speed );
  _setAction( acMove );
}

void Walker::wait(int ticks)
{
  _d->state.wait = ticks;
  if( ticks < 0 )
  {
    _setAction( acNone );
    setSpeed( 0.f );
  }
}

int Walker::waitInterval() const { return _d->state.wait; }

bool Walker::die()
{
  _d->state.health = 0;
  deleteLater();

  WalkerPtr corpse = Corpse::create( _city(), this );
  if( corpse.isValid() )
  {
    corpse->attach();
  }
  return corpse.isValid();
}

void Walker::mapTurned()
{
  float prevDistance = _d->world.dst2next();

  setPos( tile().epos() );

  float curDistance = _d->world.dst2next();
  float move = curDistance - prevDistance;

  PointF delta = _d->speed.vector.normalize() * move;
  _d->world.pos += delta;

  _updateMappos();
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
      Logger::warning( "{0} - {1} [{2},{3}] ref:{4}", wlk->name(),
                       WalkerHelper::getTypename( wlk->type() ),
                       wlk->pos().i(), wlk->pos().j(), wlk->rcount() );
    }
  }
}
#endif


void Walker::Impl::reset(PlayerCityPtr pcity )
{
  city = pcity;
  nation = world::nation::unknown;
  action.action = Walker::acMove;
  action.direction = direction::none;
  type = walker::unknown;

  world.lastDst = 99.f;

  map.inCenter = false;
  map.tile = nullptr;

  speed.value = 1.f; // default speed
  speed.multiplier = 1.f;
  speed.tileKoeff = 1.f;

  state.health = 100;
  state.wait = 0;
  state.deleted = false;
}
