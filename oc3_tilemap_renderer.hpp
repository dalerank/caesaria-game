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


#ifndef __OPENCAESAR3_GUITILEMAP_H_INCLUDED__
#define __OPENCAESAR3_GUITILEMAP_H_INCLUDED__

#include "oc3_picture.hpp"
#include "oc3_city.hpp"
#include "oc3_tilemap.hpp"
#include "oc3_tilemap_area.hpp"
#include "oc3_signals.hpp"
#include "oc3_scopedptr.hpp"
#include "oc3_tilemapchangecommand.hpp"

class ScreenGame;
struct NEvent;

/* Draws the tilemap area on the screen thanks to the GfxEngine, and handle user events */
class TilemapRenderer
{
public:
   TilemapRenderer();
   ~TilemapRenderer();
   
   void init( CityPtr city, TilemapArea &mapArea, ScreenGame *screen);

   TilemapArea &getMapArea();

   // draws the tilemap on the screen,
   // using a dumb back to front drawing of all pictures.
   void drawTilemap();
   
   void handleEvent( NEvent& event);

   Tilemap& getTilemap();

   Tile* getTile( const Point& pos, bool overborder=false );
   
   // returns the tile at the grid position
   Tile* getTile( const TilePos& pos );

   // sets the current command
   void setMode( const TilemapChangeCommandPtr command );

   void animate( unsigned int time );

oc3_signals public:
   Signal1< const Tile& >& onShowTileInfo();
   Signal1< std::string >& onWarningMessage();

protected:
   // used to discard the build/remove preview
   void discardPreview();

   // used to display the future building at mouse location
   void checkPreviewBuild(const TilePos& pos );
   
   // update preview tiles
   void updatePreviewTiles( bool force=false );

private:
   class Impl;
   ScopedPtr< Impl > _d;
};


#endif
