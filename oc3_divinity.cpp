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
#include "oc3_city.hpp"
#include "oc3_farm.hpp"
#include "oc3_scenario_event.hpp"
#include "oc3_gettext.hpp"

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

    _moodDescr << vm.get( "moodDescription" ).toList();
  }

  virtual VariantMap save() const { return VariantMap(); }
  virtual std::string getName() const { return _name; }
  virtual std::string getShortDescription() const { return _shortDesc; }
  virtual Service::Type getServiceType() const { return _service; }
  virtual const Picture& getPicture() const { return _pic; }
  virtual float getRelation() const { return _relation; }
  virtual float getDefaultDecrease() const { return 2.f; }
  virtual DateTime getLastFestivalDate() const { return _lastFestival; }

  virtual void updateRelation( float income, CityPtr city )
  {
    _relation += income - getDefaultDecrease();
  }

  virtual std::string getMoodDescription() const
  {
    if( _moodDescr.empty() )
      return "no_descriptions_divinity_mood";

    int delim = 100 / _moodDescr.size();
    return _moodDescr[ _relation / delim ];
  }

  RomeDivinityBase() {}

protected:
  std::string _name;
  Service::Type _service;
  std::string _shortDesc;
  DateTime _lastFestival;
  DateTime _lastActionDate;
  float _relation;
  Picture _pic;
  StringArray _moodDescr;
};

class RomeDivinityCeres : public RomeDivinityBase
{
public:
  static RomeDivinityPtr create()
  {
    RomeDivinityPtr ret( new RomeDivinityCeres() );
    ret->drop();

    return ret;
  }

  virtual void updateRelation(float income, CityPtr city)
  {
    RomeDivinityBase::updateRelation( income, city );

    if( getRelation() < 1 && _lastActionDate.getMonthToDate( GameDate::current() ) > 10 )
    {
      _lastActionDate = GameDate::current();
      ShowInfoboxEvent::create( _("##wrath_of_ceres_title##"), _("##wrath_of_ceres_description##") );

      CityHelper helper( city );
      FarmList farms = helper.getBuildings<Farm>( B_MAX );
      foreach( FarmPtr farm, farms )
      {
        farm->updateProgress( -farm->getProgress() );
      }
    }
  }
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

  RomeDivinityPtr divn = RomeDivinityCeres::create();
  divn.as<RomeDivinityBase>()->load( pantheon.get( "ceres" ).toMap() );
  _d->divinties.push_back( divn );

  const char* divNames[] = { "mars", "neptune", "venus", "mercury", 0 };
  for( int index=0; divNames[ index ] != 0; index++ )
  {
    divn = new RomeDivinityBase();
    divn->load( pantheon.get( divNames[ index ] ).toMap() );

    RomeDivinityPtr ret( divn );
    ret->drop();

    _d->divinties.push_back( ret );
  }
}

void DivinePantheon::doFestival4(RomeDivinityPtr who)
{

}
