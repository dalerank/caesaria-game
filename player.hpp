// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com


#ifndef PLAYER_HPP
#define PLAYER_HPP


#include <enums.hpp>
#include <serializer.hpp>
#include <list>



class Player : public Serializable
{
public:
   Player();

   void serialize(OutputSerialStream &stream);
   void unserialize(InputSerialStream &stream);

private:
   int _funds;  // amount of money
};


#endif
