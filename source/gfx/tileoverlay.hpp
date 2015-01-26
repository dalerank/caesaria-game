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

#ifndef _CAESARIA_TILEOVERLAY_H_INCLUDE_
#define _CAESARIA_TILEOVERLAY_H_INCLUDE_

#include "predefinitions.hpp"
#include "gfx/picture.hpp"
#include "gfx/animation.hpp"
#include "game/enums.hpp"
#include "core/serializer.hpp"
#include "core/scopedptr.hpp"
#include "gfx/renderer.hpp"
#include "core/direction.hpp"
#include "game/predefinitions.hpp"
#include "core/debug_queue.hpp"

class MetaData;

struct Desirability
{
 Desirability() : base( 0 ), range( 0 ), step( 0 ) {}

 int base;
 int range;
 int step;
};

struct CityAreaInfo
{
  PlayerCityPtr city;
  TilePos pos;
  const gfx::TilesArray& aroundTiles;
};

namespace gfx
{

class TileOverlay : public Serializable, public ReferenceCounted
{
public:
  typedef int Type;
  typedef int Group;

  TileOverlay( const Type type, const Size& size=Size(1));
  virtual ~TileOverlay();

  gfx::Tile& tile() const;  // master tile, in case of multi-tile area
  TilePos pos() const;
  Size size() const;  // size in tiles (1x1, 2x2, ...)
  void setSize( const Size& size );

  bool isDeleted() const;  // returns true if the overlay should be forgotten
  void deleteLater();

  virtual bool isWalkable() const;
  virtual bool isDestructible() const;
  virtual bool isFlat() const;
  virtual void initTerrain( gfx::Tile& terrain ) = 0;

  virtual bool build( const CityAreaInfo& info );
  virtual void destroy();  // handles the delete

  virtual Point offset(const Tile &tile, const Point& subpos ) const;
  virtual void timeStep(const unsigned long time);  // perform one simulation step
  virtual void changeDirection(Tile *masterTile, constants::Direction direction);

  // graphic
  virtual void setPicture(Picture picture);
  virtual void setPicture(const char* resource, const int index);
  virtual const gfx::Pictures& pictures( gfx::Renderer::Pass pass ) const;

  virtual const Picture& picture() const;
  virtual std::string sound() const;

  void setAnimation( const gfx::Animation& animation );
  const gfx::Animation& animation() const;

  virtual gfx::Renderer::PassQueue passQueue() const;
  virtual Desirability desirability() const;

  std::string name();  // landoverlay debug name
  void setName( const std::string& name );

  Type type() const;
  Group group() const;
  void setType(const Type type);

  virtual void save( VariantMap& stream) const;
  virtual void load( const VariantMap& stream );

  virtual void initialize( const MetaData& mdata );

protected:
  gfx::Animation& _animationRef();
  gfx::Tile* _masterTile();
  PlayerCityPtr _city() const;
  gfx::Pictures& _fgPicturesRef();
  Picture&_fgPicture(unsigned int index);
  Picture& _pictureRef();

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

#ifdef DEBUG
class OverlayDebugQueue : public DebugQueue<TileOverlay>
{
public:
  static void print();
};
#endif

}//end namespace gfx

#endif //_CAESARIA_TILEOVERLAY_H_INCLUDE_
