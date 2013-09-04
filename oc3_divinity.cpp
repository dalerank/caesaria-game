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
#include "oc3_gamedate.hpp"
#include "oc3_service.hpp"

class RomeDivinityBase : public RomeDivinity
{  
public: 
  void load( const VariantMap& vm )
  {
    _name = vm.get( "name" ).toString();
    _service = ServiceHelper::getType( vm.get( "service" ).toString() );
    _pic = Picture::load( vm.get( "image" ).toString() );
    Variant vRelation = vm.get( "relation" );
    _relation = vRelation.isNull() ? 100.f : (float)vRelation;
    Variant vLastFestival = vm.get( "lastFestivalDate" );
    _lastFestival = vLastFestival.isNull() ? GameDate::current() : vLastFestival.toDateTime();
    _shortDesc = vm.get( "shortDesc" ).toString();
  }

  virtual std::string getName() const { return _name; }
  virtual std::string getShortDescription() const { return _shortDesc; }
  virtual Service::Type getServiceType() const { return _service; }
  virtual const Picture& getPicture() const { return _pic; }
  virtual float getRelation() const { return _relation; }
  virtual float getDefaultDecrease() const { return 1.f; }
  virtual DateTime getLastFestivalDate() const { return _lastFestival; }

  virtual void updateRelation( float income )
  {
    _relation += income - getDefaultDecrease();
  }

  RomeDivinityBase() {}
private:
  std::string _name;
  Service::Type _service;
  std::string _shortDesc;
  DateTime _lastFestival;
  float _relation;
  Picture _pic;
};

class DivinePantheon::Impl
{
public:  
  DivinePantheon::Divinities divinties;
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

RomeDivinityPtr DivinePantheon::get( RomeDivinityType name )
{
  if( (unsigned int)name > getInstance()._d->divinties.size() )
    return RomeDivinityPtr();

  return getInstance()._d->divinties.at( name );
}

DivinePantheon::Divinities DivinePantheon::getAll(){ return getInstance()._d->divinties; }
RomeDivinityPtr DivinePantheon::mars(){  return get( romeDivMars ); }
RomeDivinityPtr DivinePantheon::neptune() { return get( romeDivNeptune ); }
RomeDivinityPtr DivinePantheon::venus(){ return get( romeDivVenus ); }
RomeDivinityPtr DivinePantheon::mercury(){  return get( romeDivMercury ); }

void DivinePantheon::initialize( const io::FilePath& filename )
{
  VariantMap pantheon = SaveAdapter::load( filename.toString() );

  const char* divNames[] = { "ceres", "mars", "neptune", "venus", "mercury", 0 };

  for( int index=0; divNames[ index ] != 0; index++ )
  {
    RomeDivinityBase* divn = new RomeDivinityBase();
    divn->load( pantheon.get( divNames[ index ] ).toMap() );

    RomeDivinityPtr ret( divn );
    ret->drop();

    _d->divinties.push_back( ret );
  }
}

void DivinePantheon::doFestival4(RomeDivinityPtr who)
{

}
