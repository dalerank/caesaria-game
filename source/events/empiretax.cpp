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

#include "empiretax.hpp"
#include "game/game.hpp"
#include "city/city.hpp"
#include "city/funds.hpp"
#include "world/empire.hpp"
#include "fundissue.hpp"

namespace events
{

GameEventPtr EmpireTax::create(const std::string &citname)
{
  EmpireTax* ptr = new EmpireTax();
  ptr->_cityname = citname;

  GameEventPtr ret( ptr );
  ret->drop();

  return ret;
}

bool EmpireTax::_mayExec(Game&, unsigned int) const { return true;}

void EmpireTax::_exec(Game& game, unsigned int)
{
  PlayerCityPtr city = game.city();

  int profit = city->funds().getIssueValue( city::Funds::cityProfit, city::Funds::lastYear );
  int empireTax = 0;
  if( profit <= 0 )
  {
    empireTax = (city->population() / 1000) * 100;
  }
  else
  {
    int minimumExpireTax = (city->population() / 1000) * 100 + 50;
    empireTax = math::clamp( profit / 4, minimumExpireTax, 9999 );
  }

  if( city->funds().haveMoneyForAction( empireTax ) )
  {
    GameEventPtr issue = FundIssueEvent::create( city::Funds::empireTax, -empireTax );
    issue->dispatch();

    game.empire()->payTax( city->getName(), empireTax );
  }
}

}
