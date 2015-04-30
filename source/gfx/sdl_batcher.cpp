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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#include "sdl_batcher.hpp"
#include "core/logger.hpp"

namespace gfx
{
typedef std::vector<gfx::Batch> States;

class SdlBatcher::Impl
{
public:
  bool active;
  States states;
  gfx::Picture currentTx;
  Rects  currentSrcRects;
  Rects  currentDstRects;
};


void SdlBatcher::draw(Engine& engine)
{
  foreach( it, _d->states )
    engine.draw( *it, 0 );
}

void SdlBatcher::append(const Picture& pic, const Point& pos)
{
  if( !pic.isValid() )
    return;

  if( !_d->currentTx.isValid() )
    _d->currentTx = pic;

  if( _d->currentTx.texture() != pic.texture() )
  {
    Batch batch;
    batch.load( _d->currentTx, _d->currentSrcRects, _d->currentDstRects );

    //Logger::warning( "!!! WARNING: cant batch " + _d->currentTx.name() + " to " + pic.name() + " : Swith to next state" );

    _d->currentTx = pic;
    _d->states.push_back( batch );

    _d->currentDstRects.clear();
    _d->currentSrcRects.clear();
  }

  _d->currentSrcRects.push_back( pic.originRect() );
  _d->currentDstRects.push_back( Rect( pos + Point( pic.offset().x(), -pic.offset().y() ), pic.size() ) );
}

void SdlBatcher::append(const Picture &pic, const Rect &srcRect, const Rect &dstRect)
{
  if( !pic.isValid() )
    return;

  if( !_d->currentTx.isValid() )
    _d->currentTx = pic;

  if( _d->currentTx.texture() != pic.texture() )
  {
    Batch batch;
    batch.load( _d->currentTx, _d->currentSrcRects, _d->currentDstRects );

    Logger::warning( "!!! WARNING: cant batch " + _d->currentTx.name() + " to " + pic.name() + " : Swith to next state" );

    _d->currentTx = pic;
    _d->states.push_back( batch );

    _d->currentDstRects.clear();
    _d->currentSrcRects.clear();
  }

  _d->currentSrcRects.push_back( Rect( pic.originRect().lefttop() + srcRect.lefttop(), srcRect.size() ) );
  _d->currentDstRects.push_back( Rect( dstRect.lefttop() + Point( pic.offset().x(), -pic.offset().y() ), dstRect.size() ) );
}

void SdlBatcher::append(const Batch &batch)
{
  _d->states.push_back( batch );
}

void SdlBatcher::append(const Pictures& pics, const Point& pos)
{
  foreach( it, pics )
    append( *it, pos );
}

void SdlBatcher::begin()
{
  foreach( it, _d->states )
    it->destroy();

  _d->states.clear();

  _d->currentSrcRects.clear();
  _d->currentDstRects.clear();
}

void SdlBatcher::finish()
{
  if( _d->currentSrcRects.empty() )
    return;

  Batch batch;
  batch.load( _d->currentTx, _d->currentSrcRects, _d->currentDstRects );

  _d->states.push_back( batch );
}

bool SdlBatcher::active() const { return _d->active; }

void SdlBatcher::setActive(bool value) { _d->active = value; }

SdlBatcher::SdlBatcher() : _d(new Impl)
{
  _d->active = true;
}

SdlBatcher::~SdlBatcher()
{

}

}//end namespace gfx
