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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com


#include "senate.hpp"
#include "gfx/picture.hpp"
#include "game/resourcegroup.hpp"
#include "game/funds.hpp"
#include "walker/taxcollector.hpp"
#include "constants.hpp"
#include "city/statistic.hpp"
#include "core/gettext.hpp"
#include "game/gamedate.hpp"
#include "core/logger.hpp"
#include "core/position_array.hpp"
#include "objects_factory.hpp"

using namespace gfx;
using namespace city;

REGISTER_CLASS_IN_OVERLAYFACTORY(object::senate, Senate)

class StateFlags
{
public:
  Pictures pics;
  PointsArray offsets;

  const Picture& getPicture( Senate::Status st ) const { return pics.valueOrEmpty( st );  }
  Point getOffset( Senate::Status st ) const { return offsets.valueOrEmpty( st ); }

  void set( Senate::Status status, Picture pic, const Point& offset )
  {
    if( status >= pics.size() )
    {
      pics.resize( status+1 );
      offsets.resize( status+1 );
    }

    offsets[ status ] = offset;
    pics[ status ] = pic;
  }

  void load( const VariantMap& stream )
  {
    VARIANT_LOAD_CLASS_LIST( offsets, stream )
  }

  VariantMap save() const
  {
    VariantMap ret;
    VARIANT_SAVE_CLASS( ret, offsets )

    return ret;
  }

  void set( Senate::Status status, const VariantMap& stream )
  {
    if( stream.empty() )
      return;

    Picture pic( stream.get( "rc"), stream.get( "index") );
    Point offset = stream.get( "offset" );
    set( status, pic, offset );
  }
};

class Senate::Impl
{
public:
  float taxValue;
  std::string errorStr;
  StateFlags flags;
};

Senate::Senate() : ServiceBuilding( Service::senate, object::senate, Size(5) ), _d( new Impl )
{
  _picture().load( ResourceGroup::govt, 4 );
  _d->taxValue = 0;

  _fgPictures().resize( 8 );
  _d->flags.set( culture,    Picture( ResourceGroup::govt, 5 ), Point( 140, -30 ) );
  _d->flags.set( prosperity, Picture( ResourceGroup::govt, 6 ), Point( 170, -25 ) );
  _d->flags.set( peace,      Picture( ResourceGroup::govt, 7 ), Point( 200, -15 ) );
  _d->flags.set( favour,     Picture( ResourceGroup::govt, 8 ), Point( 230, -10 ) );
}

bool Senate::canBuild( const city::AreaInfo& areaInfo ) const
{
  _d->errorStr = "";
  bool mayBuild = ServiceBuilding::canBuild( areaInfo );

  if( mayBuild )
  {
    bool isSenatePresent = !areaInfo.city->statistic().objects.find<Building>( object::senate).empty();
    _d->errorStr = isSenatePresent ? _("##can_build_only_one_of_building##") : "";
    mayBuild &= !isSenatePresent;
  }

  return mayBuild;
}

void Senate::applyService(ServiceWalkerPtr walker)
{
  switch( walker->type() )
  {
  case walker::taxCollector:
  {
    auto taxCollectir = walker.as<TaxCollector>();
    if( taxCollectir.isValid() )
    {
      float tax = taxCollectir->takeMoney();;
      _d->taxValue += tax;
      Logger::warning( "Senate: collect money {0}. All money {1}", tax, _d->taxValue );
    }
  }
  break;

  default:
  break;
  }

  ServiceBuilding::applyService( walker );
}

bool Senate::build( const city::AreaInfo& info )
{
  ServiceBuilding::build( info );
  _updateUnemployers();
  _updateRatings();

  return true;
}

unsigned int Senate::walkerDistance() const { return 26; }

void Senate::_updateRatings()
{
  _updateRating( culture );
  _updateRating( prosperity );
  _updateRating( peace );
  _updateRating( favour );
}

void Senate::_updateRating(Senate::Status st)
{
  if( !_fgPicture( st ).isValid() )
    _fgPicture( st ) = _d->flags.getPicture( st );

  _fgPicture( st ).setOffset( _d->flags.getOffset( st ) + Point( 0, status( st ) / 2 ) );
}

void Senate::timeStep(const unsigned long time)
{
  if( game::Date::isMonthChanged() )
  {
    _updateUnemployers();
    _updateRatings();
  }

  ServiceBuilding::timeStep( time );
}

void Senate::initialize(const object::Info& mdata)
{
  ServiceBuilding::initialize( mdata );

  VariantMap ratings = mdata.getOption( "ratings" ).toMap();
  _d->flags.set( culture,    ratings.get( CAESARIA_STR_A(culture   ) ).toMap() );
  _d->flags.set( prosperity, ratings.get( CAESARIA_STR_A(prosperity) ).toMap() );
  _d->flags.set( peace,      ratings.get( CAESARIA_STR_A(peace     ) ).toMap() );
  _d->flags.set( favour,     ratings.get( CAESARIA_STR_A(favour    ) ).toMap() );
}

void Senate::save(VariantMap& stream) const
{
  ServiceBuilding::save( stream );

  VARIANT_SAVE_ANY_D( stream, _d, taxValue )
  VARIANT_SAVE_ANY_D( stream, _d, errorStr )
  VARIANT_SAVE_CLASS_D( stream, _d, flags )
}

void Senate::load(const VariantMap& stream)
{
  ServiceBuilding::load( stream );
  VARIANT_LOAD_ANY_D( _d, taxValue, stream )
  VARIANT_LOAD_STR_D( _d, errorStr, stream )
  VARIANT_LOAD_CLASS_D( _d, flags, stream )

  _updateRatings();
}

void Senate::_updateUnemployers()
{
  Point offsets[] = { Point( 80, -15), Point( 90, -20), Point( 110, -30 ), Point( 120, -10 ) };
  int workless = status( Senate::workless );
  for( int k=0; k < 4; k++ )
  {
    Picture pic;
    if( k * 5 < workless )
    {
      pic.load( ResourceGroup::transport, 87 );
      pic.setOffset( offsets[ k ] );
    }
    _fgPicture(4 + k) = pic;
  }
}

float Senate::collectTaxes()
{
  int save = 0;

  if( _d->taxValue > 1 )
  {
    save = floor( _d->taxValue );
    _d->taxValue -= save;
  }
  return save;
}

unsigned int Senate::funds()       const { return _city()->treasury().money(); }
unsigned int Senate::thisYearTax() const { return _city()->treasury().getIssueValue( econ::Issue::taxIncome,
                                                                                     econ::Treasury::thisYear ); }
std::string Senate::errorDesc()    const { return _d->errorStr; }

int Senate::status(Senate::Status status) const
{
  if( _city().isValid() )
  {
    switch(status)
    {
    case workless:   return _city()->statistic().workers.worklessPercent();
    case culture:    return _city()->culture();
    case prosperity: return _city()->prosperity();
    case peace:      return _city()->peace();
    case favour:     return _city()->favour();
    }
  }

  return 0;
}

void Senate::deliverService()
{
  if( numberWorkers() > 0 && walkers().size() == 0 )
  {
    TaxCollectorPtr walker = Walker::create<TaxCollector>( _city() );
    walker->send2City( this, TaxCollector::goServiceMaximum|TaxCollector::anywayWhenFailed );

    addWalker( walker.object() );
  }
}
