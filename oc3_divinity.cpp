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

#include "oc3_divinity.hpp"
#include "oc3_picture.hpp"
#include "oc3_saveadapter.hpp"

class RomeDivinityBase : public RomeDivinity
{
public:
  static RomeDivinityPtr create( const VariantMap& vm )
  {
    RomeDivinityBase* divn = new RomeDivinityBase();
    divn->_name = vm.get( "name" ).toString();
    divn->_service = (ServiceType)vm.get( "service" ).toInt();
    divn->_pic = Picture::load( vm.get( "image" ).toString() );

    RomeDivinityPtr ret( divn );
    ret->drop();
    
    return ret;
  }

  std::string getName() const { return _name; }
  std::string getShortDescription() const { return _shortDesc; }
  ServiceType getServiceType() const { return _service; }
  const Picture& getPicture() const { return _pic; }

private:
  RomeDivinityBase() {}

  std::string _name;
  ServiceType _service;
  std::string _shortDesc;
  Picture _pic;
};

class DivinePantheon::Impl
{
public:
  typedef std::vector< RomeDivinityPtr > Divinities;
  Divinities divinties;
};

DivinePantheon& DivinePantheon::getInstance()
{
  static DivinePantheon inst;
  return inst;
}

DivinePantheon::DivinePantheon() : _d( new Impl )
{
}

RomeDivinityPtr DivinePantheon::ceres()
{
  return getInstance().get( romeDivCeres );
}

RomeDivinityPtr DivinePantheon::get( RomeDivinityType name ) const
{
  if( name > _d->divinties.size() )
    return RomeDivinityPtr();

  return _d->divinties.at( name );
}

RomeDivinityPtr DivinePantheon::mars()
{
  return getInstance().get( romeDivMars );
}

RomeDivinityPtr DivinePantheon::neptune()
{
  return getInstance().get( romeDivNeptune );
}

RomeDivinityPtr DivinePantheon::venus()
{
  return getInstance().get( romeDivVenus );
}

RomeDivinityPtr DivinePantheon::mercury()
{
  return getInstance().get( romeDivMercury );
}

void DivinePantheon::initialize( const std::string& filename )
{
  VariantMap pantheon = SaveAdapter::load( filename );

  _d->divinties.push_back( RomeDivinityBase::create( pantheon.get( "ceres" ).toMap() ) );
  _d->divinties.push_back( RomeDivinityBase::create( pantheon.get( "mars" ).toMap() ) );
  _d->divinties.push_back( RomeDivinityBase::create( pantheon.get( "neptune" ).toMap() ) );
  _d->divinties.push_back( RomeDivinityBase::create( pantheon.get( "venus" ).toMap() ) );
  _d->divinties.push_back( RomeDivinityBase::create( pantheon.get( "mercury" ).toMap() ) );
}