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

#include "batch.hpp"
#include "picturesarray.hpp"
#include "core/rect_array.hpp"
#include "core/logger.hpp"
#include "engine.hpp"
#include <SDL.h>

namespace gfx
{

Batch::Batch(const Batch &other)
{
  *this = other;
}

Batch& Batch::operator=(const Batch& other)
{
  _batch = other._batch;
  return *this;
}

void Batch::destroy()
{
  Engine::instance().unloadBatch( *this );
  _batch = 0;
}

void Batch::load(const Pictures &pics, const Rects& dstrects)
{
  if( pics.empty() )
  {
    //Logger::warning( "!!! WARNING: Cant create batch from pictures, because those are empty" );
    return;
  }

  if( dstrects.size() != pics.size() )
  {
    Logger::warning( "!!! WARNING: Cant create batch from pictures because length not equale dstrects" );
    return;
  }

  SDL_Texture* tx = pics.at( 0 ).texture();
  Rects srcrects;
  foreach( it, pics )
  {
    if( it->texture() == 0 || it->width() == 0 || it->height() == 0 )
    {
      srcrects.push_back( Rect( Point( 0, 0), it->size() ) );
      continue;
    }

    if( it->texture() != tx )
    {
      Logger::warning( "!!! WARNING: Cant create batch from pictures " + pics.at( 0 ).name() + " to " + it->name() );
      srcrects.push_back( Rect( Point( 0, 0), it->size() ) );
      continue;
    }

    srcrects.push_back( it->originRect() );
  }

  *this = Engine::instance().loadBatch( pics.at( 0 ), srcrects, dstrects);
}

void Batch::load(const Pictures& pics, const Point& pos)
{
  Rects rects;
  foreach( it, pics )
  {
    rects.push_back( Rect( pos + (*it).offset(), (*it).size() ) );
  }

  load( pics, rects );
}

void Batch::load(const Picture& pic, const Rects& srcrects, const Rects& dstrects)
{
  *this = Engine::instance().loadBatch( pic, srcrects, dstrects );
}

Batch::Batch()
{
  _batch = 0;
}

Batch::Batch(SDL_Batch *batch)
{
  _batch = batch;
}

}//end namespace gfx
