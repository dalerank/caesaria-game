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

#ifndef __CAESARIA_ADVISOR_EMPEROR_WINDOW_H_INCLUDED__
#define __CAESARIA_ADVISOR_EMPEROR_WINDOW_H_INCLUDED__

#include "advisor_base_window.hpp"
#include "city/request.hpp"

namespace gui
{

namespace advisorwnd
{

class Emperor : public Base
{
public:
  Emperor( PlayerCityPtr city, Widget* parent, int id );

  virtual void draw( gfx::Engine& painter );

protected:
  void _showChangeSalaryWindow();
  void _showSend2CityWindow();
  void _showGiftWindow();
  void _updateRequests();
  void _sendMoney( int money );
  void _sendGift( int money );
  void _changeSalary(int money );
  std::string _getEmperorFavourStr();
  void _resolveRequest( city::RequestPtr request );
  PlayerPtr _mayor();
  world::Emperor& _emperor();
  void _showHelp();

private:
  __DECLARE_IMPL(Emperor)
};

}

}//end namespace gui
#endif //__CAESARIA_ADVISOR_EMPEROR_WINDOW_H_INCLUDED__
