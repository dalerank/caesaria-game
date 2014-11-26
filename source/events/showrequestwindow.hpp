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
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

#ifndef _CAESARIA_EVENT_SHOWREQUESTINFO_H_INCLUDE_
#define _CAESARIA_EVENT_SHOWREQUESTINFO_H_INCLUDE_

#include "event.hpp"
#include "city/request.hpp"

namespace events
{

class ShowRequestInfo : public GameEvent
{
public:
  static GameEventPtr create( city::request::RequestPtr request,
                              bool available4exec=false,
                              const std::string& message="",
                              const std::string& video="",
                              const std::string& title="" );

protected:
  virtual void _exec( Game& game, unsigned int );
  virtual bool _mayExec(Game &game, unsigned int time) const;

private:
  ShowRequestInfo();

  city::request::RequestPtr _request;
  bool _reqAvailable;
  std::string _message, _video, _title;
};

}

#endif //_CAESARIA_EVENT_SHOWREQUESTINFO_H_INCLUDE_
