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
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "senate_popup_info.hpp"
#include "gfx/renderer.hpp"
#include "objects/senate.hpp"
#include "gfx/decorator.hpp"
#include "core/gettext.hpp"
#include "environment.hpp"
#include "gfx/tile.hpp"
#include "gfx/engine.hpp"
#include "core/utils.hpp"
#include "core/color.hpp"
#include "core/time.hpp"
#include "gfx/tilemap_camera.hpp"

using namespace gfx;

class SenatePopupInfo::Impl
{
public:
  Point startPos;
  Point ratingStartPos;
  Point offset;
  Font font;
  gfx::Renderer* cityRenderer;
  PictureRef background;  
  int lastUpdateTime;

  void updateRatings( SenatePtr senate )
  {
    lastUpdateTime = DateTime::elapsedTime();

    background->fill( 0xffffffff, Rect( ratingStartPos.x(), ratingStartPos.y(), background->width()-2, background->height()-2 ) );
    font.draw( *background, utils::format( 0xff, "%d %%", senate->status( Senate::workless ) ), ratingStartPos, false, false );
    font.draw( *background, utils::format( 0xff, "%d", senate->status( Senate::culture ) ), ratingStartPos + offset, false, false );
    font.draw( *background, utils::format( 0xff, "%d", senate->status( Senate::prosperity ) ), ratingStartPos + offset * 2, false, false );
    font.draw( *background, utils::format( 0xff, "%d", senate->status( Senate::peace ) ), ratingStartPos + offset * 3, false, false );
    font.draw( *background, utils::format( 0xff, "%d", senate->status( Senate::favour ) ), ratingStartPos + offset * 4, false, false );

    background->update();
  }
};

SenatePopupInfo::SenatePopupInfo() : _d( new Impl )
{
  _d->startPos = Point( 6, 6 );
  _d->ratingStartPos = Point( 186, 6 );
  _d->offset = Point( 0, 14 );
  _d->lastUpdateTime = 0;
  _d->background.reset( Picture::create( Size( 240, 80 ), 0, true ) );

  _d->background->fill( 0xff000000, Rect( Point( 0, 0 ), _d->background->size() ) );
  _d->background->fill( 0xffffffff, Rect( Point( 1, 1 ), _d->background->size() - Size( 2, 2 ) ) );
  
  _d->font = Font::create( FONT_1 );

  _d->font.draw( *_d->background, _("##senatepp_unemployment##"), _d->startPos, false, false );
  _d->font.draw( *_d->background, _("##senatepp_clt_rating##"), _d->startPos + _d->offset, false, false );
  _d->font.draw( *_d->background, _("##senatepp_prsp_rating##"), _d->startPos + _d->offset * 2, false, false );
  _d->font.draw( *_d->background, _("##senatepp_peace_rating##"), _d->startPos + _d->offset * 3, false, false );
  _d->font.draw( *_d->background, _("##senatepp_favour_rating##"), _d->startPos + _d->offset * 4, false, false );

  _d->background->update();
}

void SenatePopupInfo::draw( const Point& cursorPos, gfx::Engine& painter, SenatePtr senate )
{
  if( senate.isValid() )
  {
    if( DateTime::elapsedTime() - _d->lastUpdateTime > 2000 )
    {
      _d->updateRatings( senate );
    }
    painter.draw( *_d->background, cursorPos );
  }
}

SenatePopupInfo::~SenatePopupInfo()
{

}
