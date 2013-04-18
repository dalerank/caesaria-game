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



#include "oc3_scenario_saver.hpp"

#include <iostream>

#include "oc3_exception.hpp"


std::string ScenarioSaver::MAGIC = "OC3";

ScenarioSaver::ScenarioSaver()
{
}


void ScenarioSaver::save(const std::string& filename)
{
   std::fstream f(filename.c_str(), std::ios::out | std::ios::binary);
   OutputSerialStream stream;
   stream.init(f, 1);  // version
   serialize(stream);
   stream.finalize_write();
   stream.close();
}

void ScenarioSaver::serialize(OutputSerialStream &stream)
{
   Scenario &scenario = Scenario::instance();

   stream.write(MAGIC.c_str(), 3);
   stream.write_int(stream._version, 2, 0, 100);
   scenario.serialize(stream);
}


void ScenarioSaver::unserialize(InputSerialStream &stream)
{
}

