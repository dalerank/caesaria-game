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

#include "fundissue.hpp"
#include "city/funds.hpp"
#include "game/game.hpp"
#include "city/city.hpp"
#include "city/trade_options.hpp"

using namespace constants;

namespace events
{

GameEventPtr FundIssueEvent::create(int type, int value)
{
  FundIssueEvent* ev = new FundIssueEvent();
  ev->_value = value;
  ev->_type = type;
  GameEventPtr ret( ev );
  ret->drop();
  return ret;
}

GameEventPtr FundIssueEvent::import(Good::Type good, int qty)
{
  FundIssueEvent* ev = new FundIssueEvent();
  ev->_gtype = good;
  ev->_qty = qty;
  ev->_type = city::Funds::importGoods;
  GameEventPtr ret( ev );
  ret->drop();
  return ret;
}

GameEventPtr FundIssueEvent::exportg(Good::Type good, int qty)
{
  FundIssueEvent* ev = new FundIssueEvent();
  ev->_gtype = good;
  ev->_qty = qty;
  ev->_type = city::Funds::exportGoods;
  GameEventPtr ret( ev );
  ret->drop();
  return ret;
}

bool FundIssueEvent::_mayExec(Game& game, unsigned int time) const
{
  return true;
}

void FundIssueEvent::_exec(Game& game, unsigned int )
{
  if( _type == city::Funds::importGoods )
  {
    int price = game.city()->getTradeOptions().getSellPrice( _gtype );
    _value = -price * _qty / 100;
  }
  else if( _type == city::Funds::exportGoods )
  {
    int price = game.city()->getTradeOptions().getBuyPrice( _gtype );
    _value = price * _qty / 100;
  }

  game.city()->funds().resolveIssue( FundIssue( _type, _value ) );
}

} //end namespace events
