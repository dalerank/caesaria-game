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
// Copyright 2012-20155 Dalerank, dalerankn8@gmail.com

#include "sentiment.hpp"
#include "objects/constants.hpp"
#include "objects/house.hpp"
#include "objects/house_spec.hpp"
#include "game/resourcegroup.hpp"
#include "constants.hpp"
#include "core/priorities.hpp"
#include "city/statistic.hpp"
#include "core/event.hpp"
#include "gfx/tilemap_camera.hpp"

using namespace gfx;

namespace citylayer
{

enum { maxSentimentLevel=10, sentimentColumnIndex=15 };
static const char* sentimentLevelName[maxSentimentLevel] = {
                                         "##city_loathed_you##", "##sentiment_people_veryangry_you##",
                                         "##sentiment_people_angry_you##", "##sentiment_people_upset_you##",
                                         "##sentiment_people_annoyed_you##","##sentiment_people_indiffirent_you##",
                                         "##sentiment_people_pleased_you##", "##sentiment_people_extr_pleased_you##",
                                         "##sentiment_people_love_you##", "##sentiment_people_idolize_you##"
                                       };

int Sentiment::type() const {  return citylayer::sentiment; }

void Sentiment::drawTile(Engine& engine, Tile& tile, const Point& offset)
{
  Point screenPos = tile.mappos() + offset;

  if( tile.overlay().isNull() )
  {
    //draw background
    //engine.draw( tile.picture(), screenPos );

    drawPass( engine, tile, offset, Renderer::ground );
    drawPass( engine, tile, offset, Renderer::groundAnimation );
  }
  else
  {
    bool needDrawAnimations = false;
    OverlayPtr overlay = tile.overlay();
    int sentimentLevel = 0;

    if( _isVisibleObject( overlay->type() ) )
    {
      needDrawAnimations = true;
    }
    else if( overlay->type() == object::house )
    {
      HousePtr house = overlay.as<House>();

      sentimentLevel = (int)house->state( pr::happiness );
      needDrawAnimations = (house->spec().level() == 1) && house->habitants().empty();

      if( !needDrawAnimations )
      {
        drawArea( engine, overlay->area(), offset, ResourceGroup::foodOverlay, OverlayPic::inHouseBase );
      }
    }
    else
    {
      drawArea( engine, overlay->area(), offset, ResourceGroup::foodOverlay, OverlayPic::base );
    }

    if( needDrawAnimations )
    {
      Layer::drawTile( engine, tile, offset );
      registerTileForRendering( tile );
    }
    else if( sentimentLevel > 0 )
    {
      drawColumn( engine, screenPos, 100 - sentimentLevel );
    }
  }

  tile.setWasDrawn();
}

LayerPtr Sentiment::create( Camera& camera, PlayerCityPtr city)
{
  LayerPtr ret( new Sentiment( camera, city ) );
  ret->drop();

  return ret;
}

void Sentiment::handleEvent(NEvent& event)
{
  if( event.EventType == sEventMouse )
  {
    switch( event.mouse.type  )
    {
    case mouseMoved:
    {
      Tile* tile = _camera()->at( event.mouse.pos(), false );  // tile under the cursor (or NULL)
      std::string text = "";
      if( tile != 0 )
      {
        HousePtr house = tile->overlay().as<House>();
        if( house.isValid() )
        {
          int happiness = math::clamp<int>( house->state( pr::happiness ) / maxSentimentLevel, 0, maxSentimentLevel-1 );
          text = sentimentLevelName[ happiness ];
        }
      }

      _setTooltipText( text );
    }
    break;

    default: break;
    }
  }

  Layer::handleEvent( event );
}

Sentiment::Sentiment( Camera& camera, PlayerCityPtr city)
  : Info( camera, city, sentimentColumnIndex )
{
  _visibleWalkers() << walker::protestor
                    << walker::mugger
                    << walker::rioter;
  _initialize();
}

}//end namespace citylayer
