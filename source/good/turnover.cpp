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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#include "turnover.hpp"
#include "gfx/helper.hpp"
#include "core/logger.hpp"
#include "game/gamedate.hpp"

namespace good
{

void TurnoverDetails::append(const Stock& stock)
{
  append( stock.type(), stock.qty(), stock.info().sender, stock.info().birth );
}

void TurnoverDetails::append(Product gtype, int qty, int tag, const DateTime& time )
{
  auto it = this->find( tag );

  StockInfoMap* info = nullptr;
  if( it != this->end() )
  {
    info = &it->second;
  }
  else
  {
    (*this)[ tag ] = StockInfoMap();
    info = &(*this)[ tag ];
  }

  if( info )
  {
    SmInfo sminfo;
    sminfo.qty = qty;
    sminfo.birth = time;
    (*info)[ gtype ].push_back( sminfo );
  }
  else
    Logger::warning( "TurnoverDetails: cant find history" );
}

Turnovers TurnoverDetails::items() const
{
  Turnovers ret;
  DateTime gtime = game::Date::current();

  for( auto& item : *this )
  {
    const StockInfoMap& stockmap = item.second;

    for( auto gtype : stockmap )
    {
      const SmHistory& history = gtype.second;

      int qty = 0;
      int minLevel = 99;
      for( auto sminfo : history )
      {
        qty += sminfo.qty;
        minLevel = math::min( minLevel, sminfo.birth.monthsTo( gtime ) );
      }

      ret.push_back( TurnoverInfo() );
      TurnoverInfo& ti = ret.back();

      ti.sender = owner;
      ti.receiver = gfx::tile::hash2pos( item.first );
      ti.type = gtype.first;
      ti.qty = qty;
      ti.level = minLevel;
    }
  }

  return ret;
}


}//end namespace good
