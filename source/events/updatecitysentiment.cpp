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

#include "updatecitysentiment.hpp"
#include "game/game.hpp"
#include "city/city.hpp"
#include "city/sentiment.hpp"

namespace events
{

GameEventPtr UpdateCitySentiment::create( int value )
{
  UpdateCitySentiment* e = new UpdateCitySentiment();
  e->_value = value;

  GameEventPtr ret( e );
  ret->drop();

  return ret;
}

bool UpdateCitySentiment::_mayExec(Game&, unsigned int) const {  return true; }

void UpdateCitySentiment::_exec(Game& game, unsigned int)
{
  PlayerCityPtr city = game.city();

  city::SentimentPtr srvc;
  srvc << city->findService( city::Sentiment::defaultName() );

  if( srvc.isValid() )
  {
    srvc->addBuff( _value, false, 12 );
  }
}

}
