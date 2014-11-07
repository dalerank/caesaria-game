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
#include "merchant.hpp"
#include "city.hpp"
#include "good/goodstore_simple.hpp"
#include "core/stringhelper.hpp"
#include "core/foreach.hpp"
#include "core/logger.hpp"
#include "game/resourcegroup.hpp"

using namespace gfx;

namespace world
{

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

  MerchantList newMerchants;
  MerchantList merchants;

  void resolveMerchantArrived( MerchantPtr merchant );
  void updateBoundingBox();
};

CityPtr Traderoute::beginCity() const {  return _d->empire->findCity( _d->begin );}
CityPtr Traderoute::endCity() const{  return _d->empire->findCity( _d->end );}
std::string Traderoute::getName() const{  return _d->begin + "<->" + _d->end;}

void Traderoute::update( unsigned int time )
{
  MerchantList::iterator it=_d->merchants.begin();
  while( it != _d->merchants.end() )
  {
    if( (*it)->isDeleted() ) {  it = _d->merchants.erase( it ); }
    else  { (*it)->timeStep( time ); ++it;  }
  }

  if( !_d->newMerchants.empty() )
  {
    _d->merchants << _d->newMerchants;
    _d->newMerchants.clear();
  }
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

bool Traderoute::containPoint(Point pos, int devianceDistance)
{
  if( !_d->boundingBox.isPointInside( pos ) )
    return false;

  foreach( it, _d->points )
  {
    if( it->distanceTo( pos ) < devianceDistance )
    {
      return true;
    }
  }

  return false;
}

void Traderoute::setPoints(const PointsArray& points, bool seaRoute )
{
  _d->points = points;
  _d->seaRoute = seaRoute;

  for( unsigned int i=1; i < points.size(); i++ )
  {
    Point offset;
    Point p1 = points[ i ];
    Point p2 = points[ i - 1 ];

    int angle = (int)((p2-p1).getAngle() / 45.f);
    switch( angle )
    {
    case 0: angle = 91; offset = Point( -10, -2 ); break;
    case 1: angle = 89; offset = Point( -10, -5 ); break;
    case 2: angle = 88; offset = Point( 0, -5 );break;
    case 3: angle = 93; offset = Point( 0, -10 ); break;
    case 4: angle = 91; offset = Point( 0, -10 ); break;
    case 5: angle = 89; offset = Point( 0, 5 ); break;
    case 6: angle = 94; offset = Point( 0, 12 );break;
    case 7: angle = 93; offset = Point( -10, 12 ); break;
    }

    Picture pic = Picture::load( ResourceGroup::empirebits, angle + (seaRoute ? 8 : 0) );
    pic.setOffset( offset );
    _d->pictures.push_back( pic );
  }

  _d->updateBoundingBox();
}

const gfx::Pictures& Traderoute::pictures() const {  return _d->pictures;}
bool Traderoute::isSeaRoute() const{  return _d->seaRoute;}

MerchantPtr Traderoute::addMerchant( const std::string& begin, GoodStore& sell, GoodStore& buy )
{
  if( _d->points.empty() )
  {
    Logger::warning( "Traderoute::addMerchant cannot create merchant for empty trade route [" + _d->begin + "<->" +_d->end  + "]" );
    return MerchantPtr();
  }

  MerchantPtr merchant = Merchant::create( _d->empire, this, begin, sell, buy );
  _d->newMerchants.push_back( merchant );

  CONNECT( merchant, onDestination(), _d.data(), Impl::resolveMerchantArrived );
  return merchant;
}

Traderoute::Traderoute( EmpirePtr empire, std::string begin, std::string end )
: _d( new Impl )
{
  _d->empire = empire;
  _d->begin = begin;
  _d->end = end;
}

Traderoute::~Traderoute() {}

MerchantPtr Traderoute::merchant( unsigned int index )
{
  if( index >= _d->merchants.size() )
    return 0;

  MerchantList::iterator it = _d->merchants.begin();
  std::advance( it, index );
  return *it;
}

VariantMap Traderoute::save() const
{  
  VariantMap ret;
  VariantMap merchants;
  foreach( m, _d->merchants )
  {
    VariantMap saveRoute;
    (*m)->save( saveRoute );
    merchants[ "->" + (*m)->destinationCity() ] = saveRoute;
  }

  ret[ "merchants" ] = merchants;

  VariantList vl_points;
  foreach( p, _d->points ) { vl_points.push_back( *p ); }
  ret[ "points" ] = vl_points;

  VariantList vl_pictures;
  foreach( pic, _d->pictures )
  {
    vl_pictures.push_back( Variant( pic->name() ) );
  }
  ret[ "pictures" ] = vl_pictures;
  ret[ "seaRoute" ] = _d->seaRoute;

  return ret;
}

void Traderoute::load(const VariantMap& stream)
{
  VariantList points = stream.get( "points" ).toList();
  foreach( i, points )
  {
    _d->points.push_back( (*i).toPoint() );
  }
  _d->updateBoundingBox();

  VariantList pictures = stream.get( "pictures" ).toList();
  foreach( i, pictures )
  {
    _d->pictures.push_back( Picture::load( (*i).toString() ) );
  }

  VariantMap merchants = stream.get( "merchants" ).toMap();
  foreach( it, merchants )
  {
    SimpleGoodStore sell, buy;
    MerchantPtr m = addMerchant( _d->begin, sell, buy );

    if( m.isValid() )
    {
      m->load( it->second.toMap() );
    }
    else
    {
      Logger::warning( "WARNING !!!: Traderoute::load cant load merchant index %d", std::distance( merchants.begin(), it) );
    }
  }
  _d->seaRoute = stream.get( "seaRoute" );
}

Signal1<MerchantPtr>& Traderoute::onMerchantArrived()
{
  return _d->onMerchantArrivedSignal;
}

void Traderoute::Impl::resolveMerchantArrived(MerchantPtr merchant)
{
  foreach( m, merchants )
  {
    if( *m == merchant )
    {
      (*m)->deleteLater();
      break;
    }
  }

  CityPtr city = empire->findCity( merchant->destinationCity() );
  if( city.isValid() )
  {
    city->addObject( ptr_cast<Object>( merchant ) );
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
  foreach ( it, points )
  {
    boundingBox.addInternalPoint( it->x(), it->y() );
  }
}

} // end namespace world
