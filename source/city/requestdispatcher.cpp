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

#include "city.hpp"
#include "requestdispatcher.hpp"
#include "request.hpp"
#include "game/gamedate.hpp"
#include "events/showrequestwindow.hpp"
#include "core/foreach.hpp"
#include "core/logger.hpp"
#include "core/stringhelper.hpp"


class CityRequestDispatcher::Impl
{
public:
  PlayerCityPtr city;

  CityRequestList requests;
};

CityRequestDispatcher::CityRequestDispatcher()
  : CityService( getDefaultName() ), _d( new Impl )
{
}

CityServicePtr CityRequestDispatcher::create(PlayerCityPtr city)
{
  CityRequestDispatcher* cd = new CityRequestDispatcher();
  cd->_d->city = city;

  CityServicePtr ret( cd );
  ret->drop();

  return ret;
}

bool CityRequestDispatcher::add( const VariantMap& stream, bool showMessage )
{
  const std::string type = stream.get( "type" ).toString();
  if( type == GoodRequest::typeName() )
  {
    CityRequestPtr r = GoodRequest::create( stream );
    _d->requests.push_back( r );

    if( showMessage )
    {
      events::GameEventPtr e = events::ShowRequestInfo::create( r );
      e->dispatch();
    }
    return true;
  }

  Logger::warning( "CityRequestDispatcher: cannot load request with type " + type );
  return false;
}

CityRequestDispatcher::~CityRequestDispatcher() {}
std::string CityRequestDispatcher::getDefaultName(){  return "requests";}

void CityRequestDispatcher::update(const unsigned int time)
{
  if( time % (GameDate::ticksInMonth() / 4) == 1)
  {
    foreach( rq, _d->requests )
    {
      CityRequestPtr request = *rq;
      if( request->getFinishedDate() <= GameDate::current() )
      {
        request->fail( _d->city );
      }

      if( !request->isAnnounced() && request->mayExec( _d->city ) )
      {
        events::GameEventPtr e = events::ShowRequestInfo::create( request, true );
        request->setAnnounced( true );
        e->dispatch();
      }
    }

    for( CityRequestList::iterator i=_d->requests.begin(); i != _d->requests.end(); )
    {
      if( (*i)->isDeleted() ) { i = _d->requests.erase( i ); }
      else { i++; }
    }
  }
}

VariantMap CityRequestDispatcher::save() const
{
  VariantMap ret;
  VariantMap vm_rq;

  foreach( rq, _d->requests )
  {
    std::string name = StringHelper::format( 0xff, "request_%02d", std::distance( _d->requests.begin(), rq ) );
    vm_rq[ name ] = (*rq)->save();
  }

  ret[ "items" ] = vm_rq;
  return ret;
}

void CityRequestDispatcher::load(const VariantMap& stream)
{
  VariantMap vm_items = stream.get( "items" ).toMap();
  foreach( it, vm_items )
  {
    add( it->second.toMap(), false );
  }
}

CityRequestList CityRequestDispatcher::getRequests() const {  return _d->requests; }

