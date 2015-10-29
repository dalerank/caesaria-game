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

#include "cityindebt.hpp"
#include "game/game.hpp"
#include "gfx/engine.hpp"
#include "core/utils.hpp"
#include "gui/environment.hpp"
#include "core/logger.hpp"
#include "world/romechastenerarmy.hpp"
#include "city/city.hpp"
#include "core/variant_map.hpp"
#include "core/gettext.hpp"
#include "showinfobox.hpp"
#include "game/funds.hpp"
#include "gui/film_widget.hpp"
#include "world/empire.hpp"
#include "game/gamedate.hpp"
#include "fundissue.hpp"
#include "factory.hpp"
#include "game/funds.hpp"

namespace events
{

REGISTER_EVENT_IN_FACTORY(CityIndebt, "city_indebt")

namespace {
enum {noDebts=0,haveDebtInPast=1,lastWarning=2,sendLegion=3};
}

class CityIndebt::Impl
{
public:
  std::vector<int> emperorMoney;
  StringArray text;
  StringArray title;
  StringArray video;
  DateTime lastMessageSent;
  unsigned int state;
  bool isDeleted;
};

GameEventPtr CityIndebt::create()
{
  GameEventPtr ret( new CityIndebt() );
  ret->drop();

  return ret;
}

bool CityIndebt::isDeleted() const { return _d->isDeleted; }

void CityIndebt::load(const VariantMap& stream)
{
  GameEvent::load( stream );
  VARIANT_LOAD_ANY_D( _d, state, stream )
  VARIANT_LOAD_ANY_D( _d, isDeleted, stream )

  _d->emperorMoney << stream.get( "emperorMoney" ).toList();

  _d->text = stream.get( "text" ).toStringArray();
  _d->title = stream.get( "title" ).toStringArray();
  _d->video = stream.get( "video" ).toStringArray();
}

VariantMap CityIndebt::save() const
{
  VariantMap ret = GameEvent::save();

  VariantList emperorMoneys( _d->emperorMoney );
  ret[ "emperorMoney" ] = emperorMoneys;

  VARIANT_SAVE_ANY_D( ret, _d, text )
  VARIANT_SAVE_ANY_D( ret, _d, isDeleted )
  VARIANT_SAVE_ANY_D( ret, _d, title )
  VARIANT_SAVE_ANY_D( ret, _d, video )
  VARIANT_SAVE_ANY_D( ret, _d, state )
  return ret;
}

bool CityIndebt::_mayExec(Game& game, unsigned int time) const
{
  if( game::Date::isWeekChanged() )
  {
    if( game.city()->treasury().money() < econ::maxDebt )
    {
      return true;
    }
  }

  return false;
}

CityIndebt::CityIndebt() : _d( new Impl )
{
  _d->isDeleted = false;
  _d->state = noDebts;
}

void CityIndebt::_exec(Game& game, unsigned int)
{
  gui::Ui* env = game.gui();
  //_d->isDeleted = _d->state > 3;

  switch( _d->state )
  {
  case noDebts:
  case haveDebtInPast:
  case lastWarning:
  {
    std::string video = _d->state < _d->video.size() ? _d->video[ _d->state ] : "unknown.video";
    std::string text = _d->state < _d->text.size() ? _d->text[ _d->state ] : "##city_in_debt_text##";
    std::string title = _d->state < _d->title.size() ? _d->title[ _d->state ] : "##city_in_debt_text##";

    unsigned int money = _d->state < _d->emperorMoney.size() ? _d->emperorMoney[ _d->state ] : 0;
    gui::FilmWidget* dlg = new gui::FilmWidget( env->rootWidget(), video );
    dlg->setText( _( text ) );
    dlg->setTitle( _( title ) );
    dlg->setTime( game::Date::current() );
    dlg->show();

    _d->state++;

    events::dispatch<Payment>( econ::Issue::caesarsHelp, money );
    _d->lastMessageSent = game::Date::current();
  }
  break;

  case sendLegion:
  {
    if( _d->lastMessageSent.monthsTo( game::Date::current() ) > 11 )
    {
      events::dispatch<ShowInfobox>( "##message_from_centurion##", "##centurion_send_army_to_player##", true, "emp_send_army" );

      world::CityPtr rome = game.empire()->rome();
      PlayerCityPtr plCity = game.city();

      world::RomeChastenerArmyPtr army = world::RomeChastenerArmy::create( game.empire() );
      army->setCheckFavor( false );
      army->setBase( rome );
      army->attack( ptr_cast<world::Object>( plCity ) );

      _d->state++;
    }
  }
  break;

  case 4:
  break;
  }
}

}
