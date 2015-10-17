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

bool Batch::load(const Pictures &pics, const Rects& dstrects)
{
  if( pics.empty() )
  {
    //Logger::warning( "!!! WARNING: Cant create batch from pictures, because those are empty" );
    return true;
  }

  if( dstrects.size() != pics.size() )
  {
    Logger::warning( "!!! WARNING: Cant create batch from pictures because length not equale dstrects" );
    return false;
  }

  SDL_Texture* tx = pics.at( 0 ).texture();
  Rects srcrects;
  bool haveErrors = false;
  for( auto& pic : pics )
  {
    if( pic.texture() == 0 || pic.width() == 0 || pic.height() == 0 )
    {
      srcrects.push_back( Rect( Point( 0, 0), pic.size() ) );
      continue;
    }

    if( pic.texture() != tx )
    {
      Logger::warning( "!!! WARNING: Cant create batch from pictures {0} to {1}", pics.at( 0 ).name(), pic.name() );
      srcrects.push_back( Rect( Point( 0, 0), pic.size() ) );
      haveErrors = true;
      continue;
    }

    srcrects.push_back( pic.originRect() );
  }

  *this = Engine::instance().loadBatch( pics.at( 0 ), srcrects, dstrects);
  if( _batch == 0 )
    haveErrors = true;

  return !haveErrors;
}

bool Batch::load(const Pictures& pics, const Point& pos)
{
  Rects rects;
  for( auto& pic : pics )
    rects.push_back( Rect( pos + pic.offset(), pic.size() ) );

  bool isOk = load( pics, rects );
  return isOk;
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
