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
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com
// Copyright 2012-2014 dalerank, dalerankn8@gmail.com

#ifndef _CAESARIA_OBJECTS_METADATA_H_INCLUDE_
#define _CAESARIA_OBJECTS_METADATA_H_INCLUDE_

#include "good/good.hpp"
#include "core/scopedptr.hpp"
#include "vfs/path.hpp"
#include "gfx/picture.hpp"
#include "core/variant.hpp"
#include "objects/overlay.hpp"

// contains some metaData for a building type
namespace object
{

class Info
{
  friend class InfoDB;
  Info( const object::Type type, const std::string& name );

  void initialize(const VariantMap& options);

public:
#define DECL_PROPERTY(type,name,def) type name(const type& t=def) const { return getOption( #name, t ); }
  DECL_PROPERTY(int,cost,0)
  DECL_PROPERTY(bool,requestDestroy,false)
  DECL_PROPERTY(int,employers,0)
  DECL_PROPERTY(bool,c3logic,false)
  DECL_PROPERTY(bool,checkWalkersOnBuild,true)
  DECL_PROPERTY(int,maxServe,0)
  DECL_PROPERTY(bool,mayBurn,true)
  DECL_PROPERTY(float,productRate,1.f)

  static Info invalid;

  Info( const Info& a );

  ~Info();

  std::string name() const;
  std::string sound() const;
  std::string prettyName() const;
  std::string description() const;
  object::Type type() const;
  object::Group group() const;
  gfx::Picture randomPicture( int size=0 ) const;
  bool isMyPicture( const std::string& name ) const;
  Desirability desirability() const;
  gfx::Picture randomPicture( const Size& size ) const;

  Variant getOption( const std::string& name, Variant defaultVal=Variant() ) const;
  bool getFlag( const std::string& name, bool defValue ) const;
  Info& operator=( const Info& a );
  void reload() const;

  static const Info& find( object::Type type );
private:
  class Impl;
  ScopedPtr<Impl> _d;
};

struct ProductConsumer
{
  ProductConsumer( good::Product product );
  object::TypeSet consumers() const;
  object::Type consumer() const;

  good::Product _product;
};

}//end namepsace object

#endif //_CAESARIA_OBJECTS_METADATA_H_INCLUDE_
