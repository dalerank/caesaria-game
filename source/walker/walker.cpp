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
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

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

using namespace constants;

/* useful method for subtile movement computation
   si   = subtile coordinate in the current tile
   i    = tile coordinate
   amount = amount of the increase, returns remaining movement if any
   midPos = position of the midtile (so that walkers are not all exactly on the middle of the tile)
   newTile = return true if we are now on a new tile
   midTile = return true if we got above the treshold
 */
// ioSI: subtile index, ioI: tile index, ioAmount: distance, iMidPos: subtile offset 0, oNewTile: true if tile change, oMidTile: true if on tile center
void inc(float &ioSI, float &ioI, float &ioAmount, const int iMidPos, bool &oNewTile, bool &oMidTile)
{
   float delta = ioAmount;
   if ((ioSI<iMidPos) && (ioSI+delta>=iMidPos))  // midpos is ahead and inside the current movement
   {
      // we will stop at the mid tile!
      delta = iMidPos - ioSI;
      oMidTile = true;
   }

   if (ioSI+delta>15)  // the start of next tile is inside the current movement
   {
      // we will stop at the beginning of the new tile!
      delta = 16 - ioSI;
      oNewTile = true;
      ioI += 1;  // next tile
      ioSI = ioSI - 15;
   }

   ioAmount -= delta;
   ioSI += delta;
}

// ioSI: subtile index, ioI: tile index, ioAmount: distance, iMidPos: subtile offset 0, oNewTile: true if tile change, oMidTile: true if on tile center
void dec(float &ioSI, float &ioI, float &ioAmount, const int iMidPos, bool &oNewTile, bool &oMidTile)
{
   float delta = ioAmount;
   if ((ioSI>iMidPos) && (ioSI-delta<=iMidPos))  // midpos is ahead and inside the current movement
   {
      // we will stop at the mid tile!
      delta = ioSI - iMidPos;
      oMidTile = true;
   }

   if (ioSI-delta<0)  // the start of next tile is inside the current movement
   {
      // we will stop at the beginning of the new tile!
      delta = ioSI+1;
      oNewTile = true;
      ioI -= 1;  // next tile
      ioSI = ioSI + 15;
   }

   ioAmount -= delta;
   ioSI -= delta;
}
class Walker::Impl
{
public:
  PlayerCityPtr city;
  walker::Type type;
  bool isDeleted;
  float speed;
  TilePos pos;
  UniqueId uid;
  Point midTilePos;  // subtile coordinate in the current tile, at starting position
  float speedMultiplier;
  Point tileOffset; // subtile coordinate in the current tile: 0..15
  Animation animation;  // current animation
  Point posOnMap; // subtile coordinate across all tiles: 0..15*mapsize (ii=15*i+si)
  //PointF remainMove;  // remaining movement
  Pathway pathway;
  DirectedAction action;
  std::string name;
  int health;
  std::string thinks;
  float tileSpeedKoeff;
  AbilityList abilities;

  float getSpeed() const   {  return speedMultiplier * speed * tileSpeedKoeff;  }

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

  _d->speed = 1.f; // default speed
  _d->speedMultiplier = 1.f;
  _d->isDeleted = false;

  _d->midTilePos = Point( 7, 7 );
  //_d->remainMove = PointF( 0, 0 );
}

Walker::~Walker() {}
walker::Type Walker::type() const{ return _d->type; }

void Walker::timeStep(const unsigned long time)
{
  switch(_d->action.action)
  {
  case Walker::acMove:
    _walk();

    if( _d->getSpeed() > 0.f )
    {
      _updateAnimation( time );
    }
  break;

  default:
    _updateAnimation( time );
  break;
  }

  foreach( it, _d->abilities) { (*it)->run( this, time ); }

  if( getHealth() <= 0 )
  {
    die();
  }
}

void Walker::setPos( const TilePos& pos )
{
  _d->pos = pos;
  _d->tileOffset = _d->midTilePos;
  _d->posOnMap = Point( _d->pos.i(), _d->pos.j() ) * 15 + _d->tileOffset;
}

void Walker::setPathway( const Pathway& pathway)
{
  _d->pathway = pathway;
  _d->pathway.begin();
  _centerTile();
}

void Walker::setSpeed(const float speed){   _d->speed = speed;}
float Walker::speed() const{    return _d->speed;}
void Walker::setSpeedMultiplier(float koeff) { _d->speedMultiplier = koeff; }

void Walker::_walk()
{
  if( constants::noneDirection == _d->action.direction )
  {
     // nothing to do
     return;
  }

  PointF remainMove;

  Tile& tile = _d->city->tilemap().at( pos() );

  switch (_d->action.direction)
  {
  case constants::north:
  case constants::south:
    remainMove += PointF( 0, _d->getSpeed() );
  break;

  case constants::east:
  case constants::west:
    remainMove += PointF( _d->getSpeed(), 0 );
  break;

  case constants::northEast:
  case constants::southWest:
  case constants::southEast:
  case constants::northWest:
     remainMove += PointF( _d->getSpeed() * 0.7f, _d->getSpeed() * 0.7f );
  break;

  default:
     Logger::warning( "Invalid move direction: %d", _d->action.direction );
     _d->action.direction = constants::noneDirection;
  break;
  }

  bool newTile = false;
  bool midTile = false;
  float amountI = remainMove.x();
  float amountJ = remainMove.y();
  //remainMove -= PointF( amountI, amountJ );

  float tmpX = _d->tileOffset.x();
  float tmpY = _d->tileOffset.y();
  float tmpJ = _d->pos.j();
  float tmpI = _d->pos.i();
  int infinityLoopGuard = 0;
  while( amountI+amountJ > 0 && infinityLoopGuard < 100 )
  {
    infinityLoopGuard++;
    switch (_d->action.direction)
    {
    case constants::north:
       inc(tmpY, tmpJ, amountJ, _d->midTilePos.y(), newTile, midTile);
    break;

    case constants::northEast:
       inc(tmpY, tmpJ, amountJ, _d->midTilePos.y(), newTile, midTile);
       inc(tmpX, tmpI, amountI, _d->midTilePos.x(), newTile, midTile);
    break;

    case constants::east:
       inc(tmpX, tmpI, amountI, _d->midTilePos.x(), newTile, midTile);
    break;

    case constants::southEast:
       dec(tmpY, tmpJ, amountJ, _d->midTilePos.y(), newTile, midTile);
       inc(tmpX, tmpI, amountI, _d->midTilePos.x(), newTile, midTile);
    break;

    case constants::south:
       dec(tmpY, tmpJ, amountJ, _d->midTilePos.y(), newTile, midTile);
    break;

    case constants::southWest:
       dec(tmpY, tmpJ, amountJ, _d->midTilePos.y(), newTile, midTile);
       dec(tmpX, tmpI, amountI, _d->midTilePos.x(), newTile, midTile);
    break;

    case constants::west:
       dec(tmpX, tmpI, amountI, _d->midTilePos.x(), newTile, midTile);
    break;

    case constants::northWest:
       inc(tmpY, tmpJ, amountJ, _d->midTilePos.y(), newTile, midTile);
       dec(tmpX, tmpI, amountI, _d->midTilePos.x(), newTile, midTile);
    break;

    default:
       Logger::warning( "Invalid move direction: %d", _d->action.direction);
       _d->action.direction = constants::noneDirection;
    break;
    }    

    _d->tileOffset = Point( tmpX, tmpY );
    _d->pos = TilePos( tmpI, tmpJ );

    if( newTile ) // walker is now on a new tile!
    {       
       _changeTile();
    }

    if( midTile ) // walker is now on the middle of the tile!
    {
      _centerTile();
    }
  }

  Tile& offtile = newTile
                    ? _d->city->tilemap().at( pos() )
                    : tile;

  Point overlayOffset = offtile.overlay().isValid()
                             ? offtile.overlay()->offset( offtile, _d->tileOffset )
                             : Point( 0, 0 );

  _d->posOnMap = Point( _d->pos.i(), _d->pos.j() )*15 + _d->tileOffset + overlayOffset;
}

void Walker::_changeTile()
{
   Tilemap& tilemap = _d->city->tilemap();
   Tile& currentTile = tilemap.at( _d->pos );
   _d->updateSpeedMultiplier( currentTile );
}

void Walker::_centerTile()
{
   // std::cout << "Walker is on mid tile! coord=" << _i << "," << _j << std::endl;
   if (_d->pathway.isDestination())
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
   }
}

void Walker::_reachedPathway()
{
  _d->action.action = acNone;  // stop moving
  _d->animation = Animation();
}

void Walker::_computeDirection()
{
  Direction lastDirection = _d->action.direction;
  _d->action.direction = _d->pathway.nextDirection();

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

Point Walker::getMappos() const{  return Point( 2*(_d->posOnMap.x() + _d->posOnMap.y()), _d->posOnMap.x() - _d->posOnMap.y() );}
Point Walker::getSubpos() const{  return _d->tileOffset; }
bool Walker::isDeleted() const{   return _d->isDeleted;}
void Walker::_changeDirection(){  _d->animation = Animation(); } // need to fetch the new animation
void Walker::_brokePathway( TilePos pos ){}
Direction Walker::getDirection() const {  return _d->action.direction;}
Walker::Action Walker::getAction() const {  return (Walker::Action)_d->action.action;}
double Walker::getHealth() const{  return _d->health;}
void Walker::updateHealth(double value) {  _d->health = math::clamp( _d->health + value, -100.0, 100.0 );}
void Walker::acceptAction(Walker::Action, TilePos){}
void Walker::setName(const std::string &name) {  _d->name = name; }
const std::string &Walker::getName() const{  return _d->name; }
void Walker::addAbility(AbilityPtr ability) {  _d->abilities.push_back( ability );}
TilePos Walker::pos() const{    return _d->pos;}
void Walker::deleteLater(){   _d->isDeleted = true;}
void Walker::setUniqueId( const UniqueId uid ) {  _d->uid = uid;}
Pathway& Walker::_pathwayRef() {  return _d->pathway; }
const Pathway& Walker::getPathway() const {  return _d->pathway; }
Animation& Walker::_animationRef() {  return _d->animation;}
const Animation& Walker::_animationRef() const {  return _d->animation;}
void Walker::_setDirection(constants::Direction direction ){  _d->action.direction = direction; }
void Walker::setThinks(std::string newThinks){  _d->thinks = newThinks;}
void Walker::_setType(walker::Type type){  _d->type = type;}
PlayerCityPtr Walker::_city() const{  return _d->city;}
void Walker::_setHealth(double value){  _d->health = value;}

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
  _d->speed = options.get( "speed", 1.f ); // default speed
  _d->speedMultiplier = options.get( "speedMultiplier", 0.8f + math::random( 40 ) / 100.f );
}

int Walker::agressive() const { return 0; }

std::string Walker::getThinks() const
{
  if( _d->thinks.empty() )
  {
    const_cast< Walker* >( this )->_updateThinks();
  }

  return _d->thinks;
}

void Walker::getPictureList(PicturesArray& oPics)
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
  stream[ "type" ] = (int)_d->type;
  stream[ "pathway" ] =  _d->pathway.save();
  stream[ "health" ] = _d->health;
  stream[ "action" ] = (int)_d->action.action;
  stream[ "direction" ] = (int)_d->action.direction;
  stream[ "pos" ] = _d->pos;
  stream[ "tileoffset" ] = _d->tileOffset;
  stream[ "mappos" ] = _d->posOnMap;
  stream[ "speed" ] = _d->speed;
  stream[ "midTile" ] = _d->midTilePos;
  stream[ "speedMul" ] = (float)_d->speedMultiplier;
  stream[ "uid" ] = (unsigned int)_d->uid;
  //stream[ "remainmove" ] = _d->remainMove;
  stream[ "thinks" ] = Variant( _d->thinks );
}

void Walker::load( const VariantMap& stream)
{
  Tilemap& tmap = _city()->tilemap();

  _d->tileOffset = stream.get( "tileoffset" );
  _d->name = stream.get( "name" ).toString();
  _d->posOnMap = stream.get( "mappos" );
  _d->pos = stream.get( "pos" );
  _d->pathway.init( tmap, tmap.at( 0, 0 ) );
  _d->pathway.load( stream.get( "pathway" ).toMap() );
  _d->thinks = stream.get( "thinks" ).toString();

  _d->action.action = (Walker::Action) stream.get( "action" ).toInt();
  _d->action.direction = (Direction) stream.get( "direction" ).toInt();
  _d->uid = (UniqueId)stream.get( "uid" ).toInt();
  _d->speedMultiplier = (float)stream.get( "speedMul", 1.f );

  if( !_d->pathway.isValid() )
  {
    Logger::warning( "Walker: wrong way for %s:%s at [%d,%d]",
                     WalkerHelper::getTypename( _d->type ).c_str(), _d->name.c_str(),
                     _d->pos.i(), _d->pos.j() );
  }
  
  if( _d->speedMultiplier < 0.1 ) //Sometime this have this error in save file
  {
    Logger::warning( "Walker: Wrong speed multiplier for %d", _d->uid );
    _d->speedMultiplier = 1;
  }

  _d->speed = (float)stream.get( "speed" );
  _d->midTilePos = stream.get( "midTile" );
  //_d->remainMove = stream.get( "remainmove" ).toPointF();
  _d->health = (double)stream.get( "health" );
}

void Walker::turn(TilePos p)
{
  float t = (p - pos()).getAngleICW();
  int angle = (int)ceil( t / 45.f);

  Direction directions[] = { east, southEast, south, southWest,
                             west, northWest, north, northEast, northEast };

  if( _d->action.direction != directions[ angle ] )
  {
    _d->action.direction = directions[ angle ];
    _animationRef().clear();
  }
}

void Walker::_updatePathway( const Pathway& pathway)
{
  _d->pathway = pathway;
  _d->pathway.begin();
  _computeDirection();
}

void Walker::_updateAnimation( const unsigned int time )
{
  if( _d->animation.size() > 0 )
  {
    _d->animation.update( time );
  }
}

void Walker::_setWpos(Point pos) { _d->posOnMap = pos; }

void Walker::_updateThinks()
{
  _d->thinks = WalkerThinks::check( const_cast< Walker* >( this ), _city() );
}

Point Walker::_wpos() const{  return _d->posOnMap;}
void Walker::go(){ _d->action.action = acMove; }      // default action

void Walker::die()
{
  _d->health = 0;
  deleteLater();
}
