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


#ifndef GUI_TILEMAP_HPP
#define GUI_TILEMAP_HPP


#include <list>
#include <vector>

#include "oc3_picture.hpp"
#include "oc3_city.hpp"
#include "oc3_tilemap.hpp"
#include "oc3_tilemap_area.hpp"
#include "oc3_signals.hpp"
#include "oc3_scopedptr.hpp"


class ScreenGame;
class TilemapChangeCommand;
struct NEvent;

/* Draws the tilemap area on the screen thanks to the GfxEngine, and handle user events */
class GuiTilemap
{
public:
   GuiTilemap();
   ~GuiTilemap();

   void init(City &city, TilemapArea &mapArea, ScreenGame *screen);

   TilemapArea &getMapArea();

   // draws the tilemap on the screen, using a dumb back to front drawing of all pictures.
   void drawTilemap();
   
   // returns the tile at the cursor position.
   Tile* getTileXY( const Point& pos, bool overborder=false );
   void handleEvent( NEvent& event);

   // sets the current build tool (if any)
   void setChangeCommand( const TilemapChangeCommand& command );

oc3_signals public:
   Signal1< Tile* >& onShowTileInfo();

protected:
   // used to discard the build/remove preview
   void discardPreview();

   // used to display the future building at mouse location
   void checkPreviewBuild(const TilePos& pos );
   // used to display the future removed building at mouse location
   void checkPreviewRemove(const int i, const int j);

   // returns the tile at the grid position (handles priority tiles)
   Tile& getTile( const TilePos& pos );
   
   // update preview tiles
   void updatePreviewTiles( bool force=false );

   void drawTile( const Tile &tile );

   void drawTileEx( const Tile& tile, const int depth );

   void _getSelectedArea( TilePos& outStartPos, TilePos& outStopPos );
   void _clearLand();
   void _buildAll();

private:
   City* _city;     // city to display
   Tilemap* _tilemap;
   TilemapArea* _mapArea;  // visible map area

   std::vector<Tile*> _multiTiles;  // used to avoid redisplay of a multi-tile.
   //std::list<Tile*> _priorityTiles;  // these tiles have priority over "normal" tilemap tiles!

   ScreenGame *_screenGame;

   class Impl;
   ScopedPtr< Impl > _d;
};


#endif
