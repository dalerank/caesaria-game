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

#include "postpone.hpp"
#include "game/gamedate.hpp"
#include "game/game.hpp"
#include "city/city.hpp"
#include "city/requestdispatcher.hpp"

namespace events
{

class PostponeEvent::Impl
{
public:
  DateTime date;
  VariantMap options;
};

GameEventPtr PostponeEvent::create(const VariantMap& stream)
{
  PostponeEvent* pe = new PostponeEvent();
  pe->load( stream );

  GameEventPtr ret( pe );
  ret->drop();

  return ret;
}

PostponeEvent::~PostponeEvent(){}

void PostponeEvent::exec(Game& game)
{
  PlayerCityPtr city = game.getCity();
  CityRequestDispatcherPtr dispatcher = city->findService( CityRequestDispatcher::getDefaultName() );

  if( dispatcher.isValid() )
  {
    dispatcher->add( _d->options );
  }
}

bool PostponeEvent::mayExec(unsigned int) const{  return _d->date <= GameDate::current();}
bool PostponeEvent::isDeleted() const{  return _d->date <= GameDate::current(); }
VariantMap PostponeEvent::save() const {  return _d->options; }

void PostponeEvent::load(const VariantMap& stream)
{
  _d->date = stream.get( "date" ).toDateTime();
  _d->options = stream;
}

PostponeEvent::PostponeEvent(){}

}
