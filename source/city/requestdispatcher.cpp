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

#include "requestdispatcher.hpp"
#include "request.hpp"
#include "city.hpp"
#include "game/gamedate.hpp"
#include "events/showrequestwindow.hpp"


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

bool CityRequestDispatcher::add( const VariantMap& stream )
{
  std::string type = stream.get( "type" ).toString();
  if( "good_request" == type )
  {
    CityRequestPtr r = GoodRequest::create( stream );
    _d->requests.push_back( r );
    events::GameEventPtr e = events::ShowRequestInfo::create( r );
    e->dispatch();
    return true;
  }

  return false;
}

CityRequestDispatcher::~CityRequestDispatcher()
{

}

std::string CityRequestDispatcher::getDefaultName()
{
  return "requests";
}

void CityRequestDispatcher::update(const unsigned int time)
{
  if( time % (GameDate::getTickInMonth() / 4) == 1)
  foreach( CityRequestPtr rq, _d->requests )
  {
    if( rq->getFinishedDate() <= GameDate::current() )
    {
      rq->fail();
    }
  }

  for( CityRequestList::iterator i=_d->requests.begin(); i != _d->requests.end(); )
  {
    if( (*i)->isDeleted() ) { i = _d->requests.erase( i ); }
    else { i++; }
  }
}

