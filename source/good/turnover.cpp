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
#include "core/logger.hpp"
#include "game/gamedate.hpp"
#include "gfx/tile_config.hpp"
#include "core/variant_list.hpp"
#include "core/utils.hpp"

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
  DateTime gamedate = game::Date::current();

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
    SmHistory& history = (*info)[ gtype ];
    history.push_back( sminfo );

    for( auto itr=history.begin(); itr != history.end(); )
    {
      if( itr->birth.monthsTo( gamedate ) > DateTime::monthsInYear / 2 ) itr = history.erase( itr );
      else ++itr;
    }
  }
  else
    Logger::warning( "TurnoverDetails: cant find history" );
}

VariantMap TurnoverDetails::save() const
{
  VariantMap ret;
  for( const auto& item : *this )
  {
    std::string index = utils::i2str( item.first );
    ret[ index ] = item.second.save();
  }

  return ret;
}

void TurnoverDetails::load(const VariantMap& stream)
{
  for( const auto& item : stream )
  {
    unsigned int sender = utils::toUint( item.first );
    (*this)[ sender ].load( item.second.toMap() );
  }
}

Turnovers TurnoverDetails::items(Mode mode) const
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
      for( const auto& sminfo : history )
      {
        qty += sminfo.qty;
        minLevel = math::min( minLevel, sminfo.birth.monthsTo( gtime ) );
      }

      ret.push_back( TurnoverInfo() );
      TurnoverInfo& ti = ret.back();

      ti.sender = owner;
      ti.receiver = gfx::tile::hash2pos( item.first );

      if( mode == in )
        std::swap( ti.sender, ti.receiver );

      ti.type = gtype.first;
      ti.qty = qty;
      ti.level = minLevel;
    }
  }

  return ret;
}

VariantMap StockInfoMap::save() const
{
  VariantMap ret;
  for( const auto& item : *this )
  {
    VariantList vlist;
    for( const auto& hstep : item.second )
      vlist.push_back( VariantList( hstep.qty, hstep.birth.hashdate() ) );

    std::string section = utils::i2str( item.first );
    ret[ section ] = vlist;
  }

  return ret;
}

void StockInfoMap::load(const VariantMap& stream)
{
  for( const auto& item : stream )
  {
    Product index = Product( utils::toInt( item.first ) );
    VariantList items = item.second.toList();
    for( const auto& step : items )
    {
      VariantList vlist = step.toList();
      SmInfo info;
      info.qty   = vlist.get( 0 ).toUInt();
      info.birth = DateTime::fromhash( vlist.get( 1 ).toUInt() );
      (*this)[ index ].push_back( info );
    }
  }
}


}//end namespace good
