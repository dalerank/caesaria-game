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

#include "cityservice_military.hpp"
#include "city.hpp"
#include "game/gamedate.hpp"
#include "core/stringhelper.hpp"

namespace city
{

class Military::Impl
{
public:
  PlayerCityPtr city;  
  NotificationArray notifications;
};

city::SrvcPtr Military::create(PlayerCityPtr city )
{
  SrvcPtr ret( new Military( city ) );
  ret->drop();

  return SrvcPtr( ret );
}

Military::Military(PlayerCityPtr city )
  : city::Srvc( *city.object(), "military" ), _d( new Impl )
{
  _d->city = city;
}

void Military::update( const unsigned int time )
{
  if( GameDate::isMonthChanged() )
  {
    DateTime curDate = GameDate::current();
    //clear old notificationse
    for( NotificationArray::iterator it=_d->notifications.begin(); it != _d->notifications.end(); )
    {
      if( (*it).date.monthsTo( curDate ) > 3 * DateTime::monthsInYear )
      {
        it = _d->notifications.erase( it );
      }
      else { ++it; }
    }
  }
}

void Military::addNotification(const std::string& text, const Point& location)
{
  Notification n;
  n.date = GameDate::current();
  n.message = text;
  n.location = location;

  _d->notifications.push_back( n );
}

Military::Notification Military::getPriorityNotification() const
{
  return Notification();
}

const Military::NotificationArray& Military::notifications() const
{
  return _d->notifications;
}

VariantMap Military::save() const
{
  VariantMap ret;

  VariantMap vlNts;
  int index = 0;
  foreach( it, _d->notifications )
  {
    VariantList vlNt;
    vlNt << (*it).date << (*it).location << Variant( (*it).message );

    vlNts[ StringHelper::format( 0xff, "note_%03d", index ) ] = vlNt;
  }

  ret[ "notifications" ] = vlNts;

  return ret;
}

void Military::load(const VariantMap& stream)
{
  VariantMap vlNts = stream.get( "notifications" ).toMap();
  foreach( it, vlNts )
  {
    VariantList vlN = it->second.toList();
    Notification n;
    n.date = vlN.get( 0 ).toDateTime();
    n.location = vlN.get( 1 ).toPoint();
    n.message = vlN.get( 2 ).toString();

    _d->notifications.push_back( n );
  }
}

std::string Military::getDefaultName()
{
  return CAESARIA_STR_EXT(Military);
}

}//end namespace city
