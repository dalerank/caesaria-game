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


class SdlBatcher::Impl
{
public:
  bool active;
  SdlBatcher::State* batched;
  SdlBatcher::State* onwork;
};

bool SdlBatcher::append(const Picture& pic, const Point& pos, Rect* clip )
{
  if( !pic.isValid() )
    return true;

  Rect rclip = clip ? *clip : Rect();
  if( !_d->onwork->texture.isValid() )
  {
    _d->onwork->texture = pic;
    _d->onwork->clip = rclip;
  }

  bool batched = true;
  bool textureSwitched = (_d->onwork->texture.texture() != pic.texture());
  bool clipSwitched = (_d->onwork->clip != rclip);

  if( textureSwitched || clipSwitched)
  {
    std::swap( _d->batched, _d->onwork );

    reset();
    _d->onwork->texture = pic;
    batched = false;
  }

  _d->onwork->srcrects.push_back( pic.originRect() );
  _d->onwork->dstrects.push_back( Rect( pos + Point( pic.offset().x(), -pic.offset().y() ), pic.size() ) );
  return batched;
}

bool SdlBatcher::append(const Picture& pic, const Rect& srcrect, const Rect& dstRect, Rect *clip )
{
  if( !pic.isValid() )
    return true;

  Rect rclip = clip ? *clip : Rect();
  if( !_d->onwork->texture.isValid() )
  {
    _d->onwork->texture = pic;
    _d->onwork->clip = rclip;
  }

  bool batched = true;
  bool textureSwitched = _d->onwork->texture.texture() != pic.texture();
  bool clipSwitched = (_d->onwork->clip == rclip);

  if( textureSwitched || clipSwitched)
  {
    std::swap( _d->batched, _d->onwork );

    reset();
    _d->onwork->texture = pic;
    batched = false;
  }

  _d->onwork->srcrects.push_back( Rect( pic.originRect().lefttop() + srcrect.lefttop(), srcrect.size() ) );
  _d->onwork->dstrects.push_back( Rect( dstRect.lefttop() + Point( pic.offset().x(), -pic.offset().y() ), dstRect.size() ) );

  return batched;
}

bool SdlBatcher::append(const Picture &pic, const Rects &srcrects, const Rects &dstrects, Rect *clip)
{
  if( !pic.isValid() )
    return true;

  Rect rclip = clip ? *clip : Rect();
  if( !_d->onwork->texture.isValid() )
  {
    _d->onwork->texture = pic;
    _d->onwork->clip = rclip;
  }

  bool batched = true;
  bool textureSwitched = _d->onwork->texture.texture() != pic.texture();
  bool clipSwitched = (_d->onwork->clip == rclip);

  if( textureSwitched || clipSwitched)
  {
    std::swap( _d->batched, _d->onwork );

    reset();
    _d->onwork->texture = pic;
    batched = false;
  }

  _d->onwork->srcrects.insert( _d->onwork->srcrects.end(), srcrects.begin(), srcrects.end() );
  _d->onwork->dstrects.insert( _d->onwork->dstrects.end(), dstrects.begin(), dstrects.end() );

  return batched;
}

void SdlBatcher::reset()
{
  _d->onwork->texture = Picture();
  _d->onwork->clip = Rect();
  _d->onwork->dstrects.clear();
  _d->onwork->srcrects.clear();
}

bool SdlBatcher::finish()
{
  if( _d->onwork->srcrects.empty() )
    return false;

  std::swap( _d->batched, _d->onwork );
  reset();
  return true;
}

bool SdlBatcher::active() const { return _d->active; }

void SdlBatcher::setActive(bool value) { _d->active = value; }

SdlBatcher::SdlBatcher() : _d(new Impl)
{
  _d->active = true;
  _d->batched = new State();
  _d->onwork = new State();
}

SdlBatcher::~SdlBatcher()
{
  delete _d->batched;
  delete _d->onwork;
}

const SdlBatcher::State& SdlBatcher::current() const
{
  return *_d->batched;
}

}//end namespace gfx
