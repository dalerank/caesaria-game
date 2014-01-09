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

namespace world
{

class Traderoute::Impl
{
oc3_signals public:
  Signal1<MerchantPtr> onMerchantArrivedSignal;
public:
  std::string begin;
  std::string end;
  EmpirePtr empire;
  PointsArray points;
  bool seaRoute;
  PicturesArray pictures;

  typedef std::vector<MerchantPtr> MerchantList;
  MerchantList merchants;

  void resolveMerchantArrived( MerchantPtr merchant )
  {
    foreach( MerchantPtr m, merchants )
    {
      if( m == merchant )
      {
        m->deleteLater();
        break;
      }
    }

    CityPtr city = empire->getCity( merchant->getDestCityName() );
    if( city.isValid() )
    {
      city->arrivedMerchant( merchant );
    }

    onMerchantArrivedSignal.emit( merchant );
  }
};

CityPtr Traderoute::getBeginCity() const
{
  return _d->empire->getCity( _d->begin );
}

CityPtr Traderoute::getEndCity() const
{
  return _d->empire->getCity( _d->end );
}

std::string Traderoute::getName() const
{
  return _d->begin + "<->" + _d->end;
}

void Traderoute::update( unsigned int time )
{
  Impl::MerchantList::iterator it=_d->merchants.begin();
  while( it != _d->merchants.end() )
  {
    if( (*it)->isDeleted() )
    {
      it = _d->merchants.erase( it );
    }
    else
    {
      (*it)->update( time );
      it++;
    }
  }
}

PointsArray Traderoute::getPoints( bool reverse ) const
{
  if( reverse )
  {
    PointsArray ret = _d->points;
    std::reverse( ret.begin(), ret.end() );
    return ret;
  }
  return _d->points;
}

void Traderoute::setPoints(const PointsArray& points, bool seaRoute )
{
  _d->points = points;
  _d->seaRoute = seaRoute;

  for( int i=1; i < points.size(); i++ )
  {
    Point offset;
    Point p1 = points[ i ];
    Point p2 = points[ i - 1 ];

    int angle = (int)((p2-p1).getAngle() / 45.f);
    switch( angle )
    {
    case 0: angle = 90; offset = Point( -10, -10 ); break;
    case 1: angle = 89; offset = Point( -20, 0 ); break;
    case 2: angle = 88; offset = Point( 0, 5 );break;
    case 3: angle = 93; break;
    case 4: angle = 91; offset = Point( 0, -10 ); break;
    case 5: angle = 89; break;
    case 6: angle = 88; offset = Point( 0, 5 );break;
    case 7: angle = 93; offset = Point( -10, 5 ); break;
    }

    Picture pic = Picture::load( ResourceGroup::empirebits, angle + (seaRoute ? 8 : 0) );
    pic.setOffset( offset );
    _d->pictures.push_back( pic );
  }
}

const PicturesArray&Traderoute::getPictures() const
{
  return _d->pictures;
}

bool Traderoute::isSeaRoute() const
{
  return _d->seaRoute;
}

void Traderoute::addMerchant( const std::string& begin, GoodStore& sell, GoodStore& buy )
{
  MerchantPtr merchant = Merchant::create( this, begin, sell, buy );
  _d->merchants.push_back( merchant );  

  CONNECT( merchant, onDestination(), _d.data(), Impl::resolveMerchantArrived );
}

Traderoute::Traderoute( EmpirePtr empire, std::string begin, std::string end )
: _d( new Impl )
{
  _d->empire = empire;
  _d->begin = begin;
  _d->end = end;
}

Traderoute::~Traderoute()
{

}

MerchantPtr Traderoute::getMerchant( unsigned int index )
{
  if( index >= _d->merchants.size() )
    return 0;

  Impl::MerchantList::iterator it = _d->merchants.begin();
  std::advance( it, index );
  return *it;
}

VariantMap Traderoute::save() const
{  
  VariantMap ret;
  VariantMap merchants;
  foreach( MerchantPtr m, _d->merchants )
  {
    merchants[ StringHelper::format( 0xff, "->%s", m->getDestCityName().c_str() ) ] = m->save();
  }
  ret[ "merchants" ] = merchants;

  VariantList vl_points;
  foreach( Point p, _d->points )
  {
    vl_points.push_back( p );
  }
  ret[ "points" ] = vl_points;

  VariantList vl_pictures;
  foreach( Picture& pic, _d->pictures )
  {
    vl_pictures.push_back( Variant( pic.getName() ) );
  }
  ret[ "pictures" ] = vl_pictures;

  return ret;
}

void Traderoute::load(const VariantMap& stream)
{
  VariantList points = stream.get( "points" ).toList();
  for( VariantList::iterator i=points.begin(); i != points.end(); i++ )
  {
    _d->points.push_back( (*i).toPoint() );
  }

  VariantList pictures = stream.get( "pictures" ).toList();
  for( VariantList::iterator i=pictures.begin(); i != pictures.end(); i++ )
  {
    _d->pictures.push_back( Picture::load( (*i).toString() + ".png" ) );
  }

  VariantMap merchants = stream.get( "merchants" ).toMap();
  for( VariantMap::const_iterator it=merchants.begin(); it != merchants.end(); it++ )
  {
    SimpleGoodStore sell, buy;
    addMerchant( _d->begin, sell, buy );

    _d->merchants.back()->load( it->second.toMap() );
  }
}

Signal1<MerchantPtr>& Traderoute::onMerchantArrived()
{
  return _d->onMerchantArrivedSignal;
}

} // end namespace world
