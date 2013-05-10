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



#include "oc3_player.hpp"


Player::Player()
{
   _funds = 5000;
}


void Player::save( VariantMap& stream) const
{
//   // std::cout << "WRITE TILEMAP @" << stream.tell() << std::endl;
//   getTilemap().serialize(stream);
//   // std::cout << "WRITE CITY @" << stream.tell() << std::endl;
//   stream.write_int(_roadEntryI, 2, 0, 1000);
//   stream.write_int(_roadEntryJ, 2, 0, 1000);
//   stream.write_int(_roadExitI, 2, 0, 1000);
//   stream.write_int(_roadExitJ, 2, 0, 1000);
//   stream.write_int(_boatEntryI, 2, 0, 1000);
//   stream.write_int(_boatEntryJ, 2, 0, 1000);
//   stream.write_int(_boatExitI, 2, 0, 1000);
//   stream.write_int(_boatExitJ, 2, 0, 1000);
//   stream.write_int((int) _climate, 2, 0, C_MAX);
//   stream.write_int(_time, 4, 0, 1000000);
//
//   // walkers
//   stream.write_int(_walkerList.size(), 2, 0, 65535);
//   for (std::list<Walker*>::iterator itWalker = _walkerList.begin(); itWalker != _walkerList.end(); ++itWalker)
//   {
//      // std::cout << "WRITE WALKER @" << stream.tell() << std::endl;
//      Walker &walker = **itWalker;
//      walker.serialize(stream);
//   }
//
//   // overlays
//   stream.write_int(_overlayList.size(), 2, 0, 65535);
//   for (std::list<LandOverlay*>::iterator itOverlay = _overlayList.begin(); itOverlay != _overlayList.end(); ++itOverlay)
//   {
//      // std::cout << "WRITE OVERLAY @" << stream.tell() << std::endl;
//      LandOverlay &overlay = **itOverlay;
//      overlay.serialize(stream);
//   }
}

void Player::load( const VariantMap& stream )
{
//   // std::cout << "READ TILEMAP @" << stream.tell() << std::endl;
//   _tilemap.unserialize(stream);
//   // std::cout << "READ CITY @" << stream.tell() << std::endl;
//   _roadEntryI = stream.read_int(2, 0, 1000);
//   _roadEntryJ = stream.read_int(2, 0, 1000);
//   _roadExitI = stream.read_int(2, 0, 1000);
//   _roadExitJ = stream.read_int(2, 0, 1000);
//   _boatEntryI = stream.read_int(2, 0, 1000);
//   _boatEntryJ = stream.read_int(2, 0, 1000);
//   _boatExitI = stream.read_int(2, 0, 1000);
//   _boatExitJ = stream.read_int(2, 0, 1000);
//   _climate = (ClimateType) stream.read_int(2, 0, 1000);
//   _time = stream.read_int(4, 0, 1000000);
//
//   // walkers
//   int nbItems = stream.read_int(2, 0, 65535);
//   for (int i = 0; i < nbItems; ++i)
//   {
//      // std::cout << "READ WALKER @" << stream.tell() << std::endl;
//      Walker &walker = Walker::unserialize_all(stream);
//      _walkerList.push_back(&walker);
//   }
//
//   // overlays
//   nbItems = stream.read_int(2, 0, 65535);
//   for (int i = 0; i < nbItems; ++i)
//   {
//      // std::cout << "READ OVERLAY @" << stream.tell() << std::endl;
//      LandOverlay &overlay = LandOverlay::unserialize_all(stream);
//      _overlayList.push_back(&overlay);
//   }
//
//   // set all pointers to overlays&walkers
//   stream.set_dangling_pointers(false); // ignore missing pointers
//
//   // finalize the buildings
//   std::list<LandOverlay*> llo = _overlayList;
//   for (std::list<LandOverlay*>::iterator itLLO = llo.begin(); itLLO!=llo.end(); ++itLLO)
//   {
//      LandOverlay &overlay = **itLLO;
//      int i = overlay.getTile().getI();
//      int j = overlay.getTile().getJ();
//
//      overlay.build(i, j);
//   }
}

