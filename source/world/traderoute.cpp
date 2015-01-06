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
#include "good/goodstore_simple.hpp"
#include "core/utils.hpp"
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
  void updatePictures();
};

CityPtr Traderoute::beginCity() const {  return _d->empire->findCity( _d->begin );}
CityPtr Traderoute::endCity() const{  return _d->empire->findCity( _d->end );}
std::string Traderoute::getName() const{  return _d->begin + "<->" + _d->end;}

CityPtr Traderoute::partner(const std::string& name) const
{
  return _d->empire->findCity( _d->begin == name ? _d->end : _d->begin );
}

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

  _d->updatePictures();

  _d->updateBoundingBox();
}

const gfx::Pictures& Traderoute::pictures() const {  return _d->pictures;}
bool Traderoute::isSeaRoute() const{  return _d->seaRoute;}

MerchantPtr Traderoute::addMerchant(const std::string& begin, good::Store &sell, good::Store &buy )
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
    return MerchantPtr();

  MerchantList::iterator it = _d->merchants.begin();
  std::advance( it, index );
  return *it;
}

MerchantList Traderoute::merchants() const
{
  return _d->merchants;
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

 /* VariantList vl_pictures;
  foreach( pic, _d->pictures )
  {
    vl_pictures.push_back( Variant( pic->name() ) );
  }
  ret[ "pictures" ] = vl_pictures; */
  VARIANT_SAVE_ANY_D( ret, _d, seaRoute )

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
  _d->updatePictures();

 /* VariantList pictures = stream.get( "pictures" ).toList();
  foreach( i, pictures )
  {
    _d->pictures.push_back( Picture::load( (*i).toString() ) );
  } */

  VariantMap merchants = stream.get( "merchants" ).toMap();
  foreach( it, merchants )
  {
    good::SimpleStore sell, buy;
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

  VARIANT_LOAD_ANY_D( _d, seaRoute, stream )
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

    Picture pic = Picture::load( ResourceGroup::empirebits, picIndex + (seaRoute ? 8 : 0) );
    pic.setOffset( offset );
    pictures.push_back( pic );
  }
}

} // end namespace world
