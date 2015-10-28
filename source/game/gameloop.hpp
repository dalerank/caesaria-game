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
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com
// Copyright 2012-2014 dalerank, dalerankn8@gmail.com


#ifndef __CAESARIA_GAMELOOP_H_INCLUDED__
#define __CAESARIA_GAMELOOP_H_INCLUDED__

#include "gamestate.hpp"

namespace gamestate
{

class InGame: public State
{
public:
  InGame( Game *game, gfx::Engine* engine,
          Simulation& simulation,
          std::string& nextFilename,
          std::string& restartFilename );

  virtual bool update(gfx::Engine* engine);

  virtual ~InGame();

private:
  __DECLARE_IMPL(InGame)
};

} //end namespace gamestate

#endif //__CAESARIA_GAMELOOP_H_INCLUDED__
