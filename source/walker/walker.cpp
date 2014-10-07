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
#include "core/variant.hpp"
#include "core/stringhelper.hpp"
#include "pathway/path_finding.hpp"
#include "city/city.hpp"
#include "gfx/animation_bank.hpp"
#include "gfx/tilemap.hpp"
#include "core/logger.hpp"
#include "thinks.hpp"
#include "ability.hpp"
#include "helper.hpp"
#include "core/foreach.hpp"
#include "corpse.hpp"

using namespace constants;
using namespace gfx;

namespace {
CAESARIA_LITERALCONST(speed)
CAESARIA_LITERALCONST(speedMultiplier)
}

class Walker::Impl
{
public:
  std::set<Walker::Flag> flags;
  PlayerCityPtr city;
  walker::Type type;
  bool isDeleted;
  float speed;
  TilePos location;
  UniqueId uid;
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
  _d->lastCenterDst = 99.f;
  _d->centerReached = false;
  _d->speed = 1.f; // default speed
  _d->speedMultiplier = 1.f;
  _d->isDeleted = false;
  _d->centerReached = false;
  _d->waitInterval = 0;

  setFlag( vividly, true );
}

Walker::~Walker() {}
walker::Type Walker::type() const{ return _d->type; }

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
  _d->location = pos;
  //_d->tileOffset = _d->midTilePos;
  const Tile& tile =_d->city->tilemap().at( pos );
  _d->wpos = tile.center().toPointF();
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
  TilePos saveMpos( tmp.x() / 15, tmp.y() / 15 );
  _d->wpos += delta;

  tmp = _d->wpos;
  TilePos Mpos( tmp.x() / 15, tmp.y() / 15 );

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
    _d->location = Mpos;
    _changeTile();
  }
}

void Walker::_changeTile()
{
   Tilemap& tilemap = _d->city->tilemap();
   Tile& currentTile = tilemap.at( _d->location );
   _d->updateSpeedMultiplier( currentTile );
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
  _d->subSpeed = ( _d->nextwpos - _d->wpos ) / 15.f;

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
  const Tile& tile = _d->city->tilemap().at( pos() );
  Point offset;
  if( tile.overlay().isValid() )
      offset = tile.overlay()->offset( tile, tilesubpos() );

  const PointF p = _d->wpos;
  return Point( 2*(p.x() + p.y()), p.x() - p.y() ) + offset;
}

bool Walker::isDeleted() const{   return _d->isDeleted;}
void Walker::_changeDirection(){  _d->animation = Animation(); } // need to fetch the new animation
void Walker::_brokePathway( TilePos pos ){}
void Walker::_noWay(){}

void Walker::_waitFinished() { }
Direction Walker::direction() const {  return _d->action.direction;}
Walker::Action Walker::action() const {  return (Walker::Action)_d->action.action;}
double Walker::health() const{  return _d->health;}
void Walker::updateHealth(double value) {  _d->health = math::clamp( _d->health + value, -100.0, 100.0 );}
void Walker::acceptAction(Walker::Action, TilePos){}
void Walker::setName(const std::string &name) {  _d->name = name; }
const std::string &Walker::name() const{  return _d->name; }
void Walker::addAbility(AbilityPtr ability) {  _d->abilities.push_back( ability );}
TilePos Walker::pos() const{ return _d->location;}
void Walker::deleteLater(){ _d->isDeleted = true;}
void Walker::setUniqueId( const UniqueId uid ) {  _d->uid = uid;}
UniqueId Walker::uniqueId() const{ return _d->uid; }
Pathway& Walker::_pathwayRef() {  return _d->pathway; }
const Pathway& Walker::pathway() const {  return _d->pathway; }
Animation& Walker::_animationRef() {  return _d->animation;}
const Animation& Walker::_animationRef() const {  return _d->animation;}
void Walker::_setDirection(constants::Direction direction ){  _d->action.direction = direction; }
void Walker::setThinks(std::string newThinks){  _d->thinks = newThinks;}
void Walker::_setType(walker::Type type){  _d->type = type;}
PlayerCityPtr Walker::_city() const{  return _d->city;}
void Walker::_setHealth(double value){  _d->health = value;}
bool Walker::getFlag(Walker::Flag flag) const{ return _d->flags.count( flag ) > 0; }

void Walker::setFlag(Walker::Flag flag, bool value)
{
  if( value ) _d->flags.insert( flag );
  else _d->flags.erase( flag );
}

Point Walker::tilesubpos() const
{
  Point tmp = Point( _d->location.i(), _d->location.j() ) * 15 + Point( 7, 7 );
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
  _d->speed = options.get( lc_speed, 1.f ); // default speed
  _d->speedMultiplier = options.get( lc_speedMultiplier, 0.8f + math::random( 40 ) / 100.f );
}

int Walker::agressive() const { return 0; }

void Walker::attach()
{
  if( _city().isValid() )
    _city()->addWalker( this );
}

std::string Walker::currentThinks() const
{
  if( _d->thinks.empty() )
  {
    const_cast< Walker* >( this )->_updateThinks( );
  }

  return _d->thinks;
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
      DirectedAction action = { _d->action.action, constants::north };
      itAnimMap = animMap.find(action);
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
  stream[ "name" ] = Variant( _d->name );
  VARIANT_SAVE_ANY_D( stream, _d, type )
  stream[ "pathway" ] =  _d->pathway.save();
  stream[ "health" ] = _d->health;
  stream[ "action" ] = (int)_d->action.action;
  stream[ "direction" ] = (int)_d->action.direction;
  VARIANT_SAVE_ANY_D( stream, _d, location )
  stream[ "tileSpdKoeff" ] = _d->tileSpeedKoeff;
  stream[ "wpos" ] = _d->wpos;
  stream[ "nextwpos" ] = _d->nextwpos;
  stream[ lc_speed ] = _d->speed;
  stream[ lc_speedMultiplier ] = (float)_d->speedMultiplier;
  stream[ "uid" ] = (unsigned int)_d->uid;
  stream[ "thinks" ] = Variant( _d->thinks );
  stream[ "subspeed" ] = _d->subSpeed;
  stream[ "lastCenterDst" ] = _d->lastCenterDst;
  stream[ "showDebugInfo" ] = getFlag( showDebugInfo );
}

void Walker::load( const VariantMap& stream)
{
  Tilemap& tmap = _city()->tilemap();

  _d->name = stream.get( "name" ).toString();
  _d->wpos = stream.get( "wpos" ).toPointF();
  VARIANT_LOAD_ANY_D( _d, location, stream );
  _d->lastCenterDst = stream.get( "lastCenterDst" );
  _d->pathway.init( tmap, tmap.at( 0, 0 ) );
  _d->pathway.load( stream.get( "pathway" ).toMap() );
  _d->thinks = stream.get( "thinks" ).toString();
  _d->tileSpeedKoeff = stream.get( "tileSpdKoeff" );
  _d->nextwpos = stream.get( "nextwpos" ).toPointF();
  _d->action.action = (Walker::Action) stream.get( "action" ).toInt();
  _d->action.direction = (Direction) stream.get( "direction" ).toInt();
  _d->uid = (UniqueId)stream.get( "uid" ).toInt();
  _d->subSpeed = stream.get( "subspeed" ).toPointF();
  _d->speedMultiplier = (float)stream.get( lc_speedMultiplier, 1.f );


  if( !_d->pathway.isValid() )
  {
    Logger::warning( "Walker: wrong way for %s:%s at [%d,%d]",
                     WalkerHelper::getTypename( _d->type ).c_str(), _d->name.c_str(),
                     _d->location.i(), _d->location.j() );
  }
  
  if( _d->speedMultiplier < 0.1 ) //Sometime this have this error in save file
  {
    Logger::warning( "Walker: Wrong speed multiplier for %d", _d->uid );
    _d->speedMultiplier = 1;
  }

  _d->speed = (float)stream.get( lc_speed );
  _d->health = (double)stream.get( "health" );
  setFlag( showDebugInfo, stream.get( "showDebugInfo" ).toBool() );
}

void Walker::turn(TilePos p )
{
  Direction direction = TileHelper::getDirection( pos(), p );

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

void Walker::_updateThinks()
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
