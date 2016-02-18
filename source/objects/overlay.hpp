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

#ifndef _CAESARIA_OVERLAY_H_INCLUDE_
#define _CAESARIA_OVERLAY_H_INCLUDE_

#include "predefinitions.hpp"
#include "gfx/picture.hpp"
#include "gfx/animation.hpp"
#include "core/serializer.hpp"
#include "core/scopedptr.hpp"
#include "gfx/renderer.hpp"
#include "core/direction.hpp"
#include "game/predefinitions.hpp"
#include "city/areainfo.hpp"
#include "city/desirability.hpp"
#include "core/debug_queue.hpp"
#include "param.hpp"
#include "metadata.hpp"
#include "constants.hpp"

namespace ovconfig
{
enum { idxType=0, idxTypename, idxLocation, ixdCount };
}

class Overlay : public Serializable, public ReferenceCounted
{
public:
  template<typename ObjClass, typename... Args>
  static SmartPtr<ObjClass> create( const Args & ... args)
  {
    SmartPtr<ObjClass> instance( new ObjClass( args... ) );
    instance->initialize( object::Info::find( instance->type() ) );
    instance->drop();

    return instance;
  }

  template<typename ObjClass>
  static SmartPtr<ObjClass> create( object::Type type )
  {
    return ptr_cast<ObjClass>( create( type ));
  }

  static OverlayPtr create( object::Type type );

  Overlay( const object::Type type, const Size& size=Size(1,1));
  virtual ~Overlay();

  gfx::Tile& tile() const;  // master tile, in case of multi-tile area
  TilePos pos() const;
  const Size& size() const;  // size in tiles (1x1, 2x2, ...)
  void setSize( const Size& size );

  bool isDeleted() const;  // returns true if the overlay should be forgotten
  void deleteLater();

  virtual bool isWalkable() const;
  virtual bool isDestructible() const;
  virtual bool isFlat() const;
  virtual void initTerrain( gfx::Tile& terrain ) = 0;
  virtual std::string errorDesc() const;

  virtual bool build( const city::AreaInfo& info );
  virtual bool canDestroy() const;
  virtual void destroy();  // handles the delete
  virtual gfx::TilesArray area() const;

  virtual void burn();
  virtual void collapse();

  virtual Point offset(const gfx::Tile &tile, const Point& subpos ) const;
  virtual void timeStep(const unsigned long time);  // perform one simulation step
  virtual void changeDirection( gfx::Tile *masterTile, Direction direction);
  virtual bool getMinimapColor( int& color1, int& color2 ) const;

  // graphic
  virtual void setPicture(gfx::Picture picture);
  virtual void setPicture(const std::string& resource, const int index);
  virtual const gfx::Pictures& pictures( gfx::Renderer::Pass pass ) const;

  virtual const gfx::Picture& picture() const;
  virtual std::string sound() const;

  void setAnimation( const gfx::Animation& animation );
  const gfx::Animation& animation() const;

  virtual gfx::Renderer::PassQueue passQueue() const;
  virtual const Desirability& desirability() const;

  virtual void setState( Param name, double value );

  std::string name();  // landoverlay debug name
  void setName( const std::string& name );

  object::Type type() const;
  object::Group group() const;

  virtual void save( VariantMap& stream) const;
  virtual void load( const VariantMap& stream );

  virtual void initialize( const object::Info& mdata );
  virtual void reinit();

  const object::Info& info() const;

  virtual void debugLoadOld( int oldFormat, const VariantMap& stream );
  virtual const gfx::Picture& picture(const city::AreaInfo& info) const;

protected:
  void setType(const object::Type type);
  gfx::Animation& _animation();
  gfx::Tile* _masterTile();
  PlayerCityPtr _city() const;
  gfx::Tilemap& _map() const;
  gfx::Pictures& _fgPictures();
  gfx::Picture& _fgPicture(unsigned int index);
  const gfx::Picture &_fgPicture(unsigned int index) const;
  gfx::Picture& _picture();

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

#ifdef DEBUG
class OverlayDebugQueue : public DebugQueue<Overlay>
{
public:
  static void print();
};
#endif

#endif //_CAESARIA_OVERLAY_H_INCLUDE_
