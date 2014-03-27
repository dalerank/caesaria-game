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
#include "core/stringhelper.hpp"
#include "gui/environment.hpp"
#include "core/logger.hpp"
#include "city/city.hpp"
#include "core/gettext.hpp"
#include "city/funds.hpp"
#include "gui/film_widget.hpp"
#include "game/gamedate.hpp"
#include "fundissue.hpp"

namespace events
{

GameEventPtr CityIndebt::create()
{
  GameEventPtr ret( new CityIndebt() );
  ret->drop();

  return ret;
}

bool CityIndebt::isDeleted() const { return _isDeleted; }

void CityIndebt::load(const VariantMap& stream)
{
  _emperorMoney = stream.get( "emperorMoney" );
  _text = stream.get( "text" ).toString();
}

VariantMap CityIndebt::save() const
{
  VariantMap ret;

  ret[ "emperorMoney" ] = _emperorMoney;
  ret[ "text" ] = Variant( _text );
  return ret;
}

bool CityIndebt::_mayExec(Game& game, unsigned int time) const
{
  if( time % GameDate::ticksInMonth() == 1 )
  {
    if( game.city()->funds().treasury() < 0 )
    {
      return true;
    }
  }

  return false;
}

CityIndebt::CityIndebt() : _text( "##city_indebt_text##" )
{
  _emperorMoney = 0;
  _isDeleted = false;
}

void CityIndebt::_exec(Game& game, unsigned int)
{
  gui::GuiEnv* env = game.gui();

  _isDeleted = true;
  gui::FilmWidget* dlg = new gui::FilmWidget( env->rootWidget(), "" );
  dlg->setText( _( _text ) );
  dlg->setTitle( _("##city_indebt_title##") );
  //dlg->setReceiver( _receiver );
  dlg->setTime( GameDate::current() );

  dlg->show();

  GameEventPtr e = FundIssueEvent::create( city::Funds::donation, _emperorMoney );
  e->dispatch();
}

}
