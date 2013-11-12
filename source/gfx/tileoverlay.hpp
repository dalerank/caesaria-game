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

#ifndef _OPENCAESAR3_LANDOVERLAY_H_INCLUDE_
#define _OPENCAESAR3_LANDOVERLAY_H_INCLUDE_

#include "core/predefinitions.hpp"
#include "picture.hpp"
#include "animation.hpp"
#include "game/enums.hpp"
#include "core/serializer.hpp"
#include "core/scopedptr.hpp"
#include "renderer.hpp"

class TileOverlay : public Serializable, public ReferenceCounted
{
public:
  typedef int Type;
  typedef int Group;

  TileOverlay( const Type type, const Size& size=Size(1));
  virtual ~TileOverlay();

  Tile& getTile() const;  // master tile, in case of multi-tile area
  TilePos getTilePos() const;
  Size getSize() const;  // size in tiles (1=1x1, 2=2x2, ...)
  void setSize( const Size& size );

  bool isDeleted() const;  // returns true if the overlay should be forgotten
  void deleteLater();

  virtual bool isWalkable() const;
  virtual void initTerrain( Tile& terrain ) = 0;

  virtual void build( CityPtr city, const TilePos& pos );
  virtual void destroy();  // handles the delete

  virtual Point getOffset( const Point& subpos ) const;
  virtual void timeStep(const unsigned long time);  // perform one simulation step

  // graphic
  void setPicture(Picture picture);
  void setPicture(const char* resource, const int index);
  const Picture& getPicture() const;

  void setAnimation( const Animation& animation );

  virtual const PicturesArray& getPictures( Renderer::Pass pass ) const;
  virtual Renderer::PassQueue getPassQueue() const;

  std::string getName();  // landoverlay debug name
  void setName( const std::string& name );

  Type getType() const;
  Group getClass() const;
  void setType(const Type type);

  virtual void save( VariantMap& stream) const;
  virtual void load( const VariantMap& stream );

protected:
  Animation& _animationRef();
  Tile* _getMasterTile();
  CityPtr _getCity() const;
  PicturesArray& _fgPicturesRef();
  Picture& _getPicture();

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //_OPENCAESAR3_LANDOVERLAY_H_INCLUDE_
