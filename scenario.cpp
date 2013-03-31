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



#include "scenario.hpp"

#include <iostream>

#include "exception.hpp"

Scenario* Scenario::_instance = NULL;

Scenario& Scenario::instance()
{
   if (_instance == NULL)
   {
      THROW("No Scenario instance!");
   }
   return *_instance;
}


Scenario::Scenario()
{
   _instance = this;
   _description = "";
}

City& Scenario::getCity()
{
   return _city;
}

std::string Scenario::getDescription() const
{
   return _description;
}

void Scenario::serialize(OutputSerialStream &stream)
{
   stream.write_str(getDescription(), 1000);
   getCity().serialize(stream);
}

void Scenario::unserialize(InputSerialStream &stream)
{
   std::string magic = stream.read_fix_str(3);
   if (magic != "OC3") THROW("Not an openCaesar3 saved game file");

   int version = stream.read_int(2, 0, 100);
   if (version != 1) THROW("Unsupported version " << version);

   _description = stream.read_str(1000);
   getCity().unserialize(stream);
}

