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

#include "traderoute.hpp"
#include "empire.hpp"
#include "core/variant_map.hpp"
#include "merchant.hpp"
#include "city.hpp"
#include "good/storage.hpp"
#include "core/utils.hpp"
#include "game/gamedate.hpp"
#include "core/foreach.hpp"
#include "core/logger.hpp"
#include "game/resourcegroup.hpp"
#include "core/flowlist.hpp"
#include "core/common.hpp"

using namespace gfx;

namespace world
{

class Merchants : public FlowList<Merchant>
{
public:
  Traderoute* route;

  Merchants() : route( 0 ) {}

  VariantMap save() const
  {
    VariantMap ret;
    for( auto& m : *this )
    {
      VariantMap saveRoute;
      m->save( saveRoute );
      ret[ "->" + m->destinationCity() ] = saveRoute;
    }

    return ret;
  }

  void load( const VariantMap& stream )
  {
    if( !route )
    {
      Logger::warning( "!!!WARNING: Merchants::load() route is null" );
      return;
    }

    foreach( it, stream )
    {
      good::Storage sell, buy;
      MerchantPtr m = route->addMerchant( route->beginCity()->name(), sell, buy );

      if( m.isValid() )
      {
        m->load( it->second.toMap() );
      }
      else
      {
        Logger::warning( "!!!WARNING: Merchants::load() cant load merchant index {0} for route {1}",
                         std::distance( stream.begin(), it ), route->name() );
      }
    }
  }

  void update( unsigned int time )
  {
    for( auto& it : *this )
      it->timeStep( time );
    utils::eraseIfDeleted( *this );
    merge();
  }
};

class Traderoute::Impl
{
signals public:
  Signal1<MerchantPtr> onMerchantArrivedSignal;

public:
  std::string begin;
  std::string end;
  EmpirePtr empire;
  PointsArray points;
  Rect boundingBox;
  bool seaRoute;
  gfx::Pictures pictures;

  Merchants merchants;

  void resolveMerchantArrived( MerchantPtr merchant );
  void updateBoundingBox();
  void updatePictures();
};

CityPtr Traderoute::beginCity() const {  return _d->empire->findCity( _d->begin );}
CityPtr Traderoute::endCity() const{  return _d->empire->findCity( _d->end );}
std::string Traderoute::name() const{  return _d->begin + "<->" + _d->end;}

CityPtr Traderoute::partner(const std::string& name) const
{
  return _d->empire->findCity( _d->begin == name ? _d->end : _d->begin );
}

void Traderoute::timeStep( unsigned int time )
{
  if( game::Date::isDayChanged() )
    _d->merchants.update( time );
}

PointsArray Traderoute::points( bool reverse ) const
{
  if( reverse )
  {
    PointsArray ret = _d->points;
    std::reverse( ret.begin(), ret.end() );
    return ret;
  }
  return _d->points;
}

bool Traderoute::containPoint( const Point& pos, int devianceDistance)
{
  if( !_d->boundingBox.isPointInside( pos ) )
    return false;

  for( auto& it : _d->points )
  {
    if( it.distanceTo( pos ) < devianceDistance )
      return true;
  }

  return false;
}

void Traderoute::setPoints(const PointsArray& points, bool seaRoute )
{
  _d->points = points;
  _d->seaRoute = seaRoute;

  _d->updatePictures();
  _d->updateBoundingBox();
}

const gfx::Pictures& Traderoute::pictures() const {  return _d->pictures;}
bool Traderoute::isSeaRoute() const{  return _d->seaRoute;}

MerchantPtr Traderoute::addMerchant(const std::string& begin, good::Store &sell, good::Store &buy )
{
  if( _d->points.empty() )
  {
    Logger::warning( "Traderoute::addMerchant cannot create merchant for empty trade route [{}<->{}]", _d->begin, _d->end );
    return MerchantPtr();
  }

  MerchantPtr merchant = Merchant::create( _d->empire, this, begin, sell, buy );
  _d->merchants.postpone( merchant );

  CONNECT( merchant, onDestination(), _d.data(), Impl::resolveMerchantArrived );
  return merchant;
}

Traderoute::Traderoute( EmpirePtr empire, std::string begin, std::string end )
: _d( new Impl )
{
  _d->empire = empire;
  _d->begin = begin;
  _d->end = end;
  _d->merchants.route = this;
}

Traderoute::~Traderoute() {}

MerchantPtr Traderoute::merchant( unsigned int index ) { return _d->merchants.valueOrEmpty( index ); }
const MerchantList& Traderoute::merchants() const{  return _d->merchants; }

VariantMap Traderoute::save() const
{  
  VariantMap ret;

  VARIANT_SAVE_CLASS_D( ret, _d, merchants)
  VARIANT_SAVE_CLASS_D( ret, _d, points )
  VARIANT_SAVE_ANY_D( ret, _d, seaRoute )

  return ret;
}

void Traderoute::load(const VariantMap& stream)
{
  VARIANT_LOAD_CLASS_D_LIST (_d, points, stream )
  VARIANT_LOAD_CLASS_D( _d, merchants, stream )
  VARIANT_LOAD_ANY_D( _d, seaRoute, stream )

  _d->updateBoundingBox();
  _d->updatePictures();
}

unsigned int Traderoute::getId(const std::string& begin, const std::string& end)
{
  return Hash( begin ) + Hash( end );
}

Signal1<MerchantPtr>& Traderoute::onMerchantArrived()
{
  return _d->onMerchantArrivedSignal;
}

void Traderoute::Impl::resolveMerchantArrived(MerchantPtr merchant)
{
  if( merchants.contain( merchant ) )
    merchant->deleteLater();

  CityPtr city = empire->findCity( merchant->destinationCity() );
  if( city.isValid() )
  {
    city->addObject( merchant.as<Object>() );
  }

  emit onMerchantArrivedSignal( merchant );
}

void Traderoute::Impl::updateBoundingBox()
{
  if( points.empty() )
  {
    boundingBox = Rect();
    return;
  }

  boundingBox = Rect( points.front(), points.front() );
  for( auto& it : points )
    boundingBox.addInternalPoint( it.x(), it.y() );
}

void Traderoute::Impl::updatePictures()
{
  pictures.clear();
  for( unsigned int i=1; i < points.size(); i++ )
  {
    Point offset;
    Point p = points[ i-1 ];
    Point pNext = points[ i ];

    int angle = (int)((pNext-p).getAngle() / 45.f);
    int picIndex = 91;

    switch( angle )
    {
    case 0: picIndex = 91; break;
    case 7: picIndex = 93; break;
    case 6: picIndex = 95; break;
    case 5: picIndex = 89; offset = Point( -19, 0 ); break;
    case 4: picIndex = 92; offset = Point( -21, 3); break;
    case 3: picIndex = 93; offset = Point( -19, 15); break;
    case 2: picIndex = 94; offset = Point( -4, 21); break;
    case 1: picIndex = 89; offset = Point( 0, 15); break;
    }

    Picture pic( ResourceGroup::empirebits, picIndex + (seaRoute ? 8 : 0) );
    Point randOffset( math::random(3), math::random(3) );
    pic.setOffset( offset + randOffset );
    pictures.push_back( pic );
  }
}

} // end namespace world
