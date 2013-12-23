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

#include "divinity.hpp"
#include "gfx/picture.hpp"
#include "gamedate.hpp"
#include "service.hpp"
#include "city/helper.hpp"
#include "objects/farm.hpp"
#include "events/event.hpp"
#include "core/gettext.hpp"
#include "objects/constants.hpp"

using namespace constants;

static const char* divNames[] =
{
  "ceres",
  "mars",
  "neptune",
  "venus",
  "mercury",
  0
};

class RomeDivinityBase : public RomeDivinity
{  
public: 
  void load( const VariantMap& vm );

  void assignFestival( int type );

  virtual VariantMap save() const;

  virtual std::string getName() const { return _name; }
  virtual std::string getShortDescription() const { return _shortDesc; }
  virtual Service::Type getServiceType() const { return _service; }
  virtual const Picture& getPicture() const { return _pic; }
  virtual float getRelation() const { return _relation; }
  virtual float getDefaultDecrease() const { return 2.f; }
  virtual DateTime getLastFestivalDate() const { return _lastFestival; }

  virtual void updateRelation( float income, PlayerCityPtr city )
  {
    CityHelper helper( city );
    float cityBalanceKoeff = helper.getBalanceKoeff();

    _relation = math::clamp<float>( _relation + income - getDefaultDecrease() * cityBalanceKoeff, 0, 100 );
  }

  virtual std::string getMoodDescription() const
  {
    if( _moodDescr.empty() )
      return "no_descriptions_divinity_mood";

    int delim = 100 / _moodDescr.size();
    return _moodDescr[ _relation / delim ];
  }

  RomeDivinityBase()
  {
  }

  virtual void setInternalName(const std::string &newName)
  {
    ReferenceCounted::setDebugName( newName );
  }

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
    ret->setInternalName( divNames[ romeDivCeres ] );
    ret->drop();

    return ret;
  }

  virtual void updateRelation(float income, PlayerCityPtr city)
  {
    RomeDivinityBase::updateRelation( income, city );

    if( getRelation() < 1 && _lastActionDate.getMonthToDate( GameDate::current() ) > 10 )
    {
      _lastActionDate = GameDate::current();
      events::GameEventPtr event = events::ShowInfoboxEvent::create( _("##wrath_of_ceres_title##"),
                                                                     _("##wrath_of_ceres_description##") );
      event->dispatch();

      CityHelper helper( city );
      FarmList farms = helper.find<Farm>( building::any );
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

RomeDivinityPtr DivinePantheon::get(std::string name)
{
  Divinities divines = getInstance().getAll();
  foreach( RomeDivinityPtr current, divines )
  {
    if( current->getName() == name || current->getDebugName() == name )
      return current;
  }

  return RomeDivinityPtr();
}

DivinePantheon::Divinities DivinePantheon::getAll(){ return _d->divinties; }
RomeDivinityPtr DivinePantheon::mars(){  return get( romeDivMars ); }
RomeDivinityPtr DivinePantheon::neptune() { return get( romeDivNeptune ); }
RomeDivinityPtr DivinePantheon::venus(){ return get( romeDivVenus ); }
RomeDivinityPtr DivinePantheon::mercury(){  return get( romeDivMercury ); }

void DivinePantheon::load( const VariantMap& stream )
{
  RomeDivinityPtr divn = get( divNames[ romeDivCeres ] );

  if( divn.isNull() )
  {
    _d->divinties.push_back( RomeDivinityCeres::create() );
  }

  for( int index=0; divNames[ index ] != 0; index++ )
  {
    divn = get( divNames[ index ] );

    if( divn.isNull() )
    {
      divn = RomeDivinityPtr( new RomeDivinityBase() );
      divn->setInternalName( divNames[ index ] );
      divn->drop();
      _d->divinties.push_back( divn );
    }

    divn->load( stream.get( divNames[ index ] ).toMap() );
  }
}

void DivinePantheon::save(VariantMap& stream)
{
  Divinities divines = getInstance().getAll();
  foreach( RomeDivinityPtr current, divines )
  {
    stream[ current->getName() ] = current->save();
  }
}

void DivinePantheon::doFestival( RomeDivinityType who, int type )
{
  RomeDivinityPtr divn = get( who );
  if( divn.isValid() )
  {
    divn.as<RomeDivinityBase>()->assignFestival( type );
  }
}


void RomeDivinityBase::load(const VariantMap& vm)
{
  if( vm.empty() )
    return;

  _name = vm.get( "name" ).toString();
  _service = ServiceHelper::getType( vm.get( "service" ).toString() );
  _pic = Picture::load( vm.get( "image" ).toString() );
  _relation = (float)vm.get( "relation", 100.f );
  _lastFestival = vm.get( "lastFestivalDate", GameDate::current() ).toDateTime() ;
  _shortDesc = vm.get( "shortDesc" ).toString();

  Variant value = vm.get( "moodDescription" );
  if( value.isValid() )
  {
    _moodDescr << vm.get( "moodDescription" ).toList();
  }
}

void RomeDivinityBase::assignFestival(int type)
{
  _relation = math::clamp<float>( _relation + type * 10, 0, 100 );
}

VariantMap RomeDivinityBase::save() const
{
  VariantMap ret;
  ret[ "name" ] = Variant( _name );
  ret[ "service" ] = Variant( ServiceHelper::getName( _service ) );
  ret[ "image" ] = Variant( _pic.getName() + ".png" );
  ret[ "relation" ] = _relation;
  ret[ "lastFestivalDate" ] = _lastFestival;
  ret[ "lastActionDate"] = _lastActionDate;
  ret[ "shortDesc" ] = Variant( _shortDesc );

  return ret;
}
