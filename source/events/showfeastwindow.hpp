// This file is part of CaearIA.
//
// CaearIA is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CaearIA is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CaearIA.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

#ifndef _CAESARIA_EVENT_SHOWFEASTWINDOW_H_INCLUDE_
#define _CAESARIA_EVENT_SHOWFEASTWINDOW_H_INCLUDE_

#include "event.hpp"

namespace events
{

class ShowFeastival : public GameEvent
{
public:
  static GameEventPtr create(std::string text, std::string title,
                             std::string receiver, std::string video);

protected:
  virtual void _exec( Game& game, unsigned int );
  virtual bool _mayExec(Game &game, unsigned int time) const;

private:
  std::string _text,
              _title,
              _receiver,
              _video;
};

}

#endif //_OPENCAESAR_EVENT_SHOWFEASTWINDOW_H_INCLUDE_
