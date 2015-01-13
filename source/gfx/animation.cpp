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

#include "animation.hpp"
#include "core/position.hpp"
#include "core/variant_map.hpp"
#include "core/foreach.hpp"
#include "core/utils.hpp"
#include "core/logger.hpp"

namespace gfx
{

class Animation::Impl
{
public:
  unsigned int delay;
  bool loop;
  unsigned int lastTimeUpdate;
  Point offset;
  int index;  // index of the current frame
};

void Animation::start(bool loop)
{
  __D_IMPL(d, Animation)
  d->index = 0;
  d->lastTimeUpdate = 0;
  d->loop = loop;
}

Pictures& Animation::frames() {  return _pictures;}
const Pictures& Animation::frames() const{  return _pictures;}
unsigned int Animation::frameCount() const{  return _pictures.size();}

void Animation::setOffset( const Point& offset )
{
  foreach( pic, _pictures ) { pic->setOffset( offset ); }
}

void Animation::addOffset(const Point& offset)
{
  foreach( pic, _pictures) { pic->addOffset( offset );}
}

Point Animation::offset() const
{
  if( _pictures.empty() )
  {
    return Point();
  }

  return _pictures.front().offset();
}

bool Animation::atEnd() const
{
  return _dfunc()->index == (int)( _pictures.size()-1 );
}

void Animation::update( unsigned int time )
{  
  __D_IMPL(_d,Animation)

  if( _d->index < 0 )
    return;

  if( _d->delay > 0 )
  {
    if( time - _d->lastTimeUpdate < _d->delay )
      return;
  }

  _d->index += 1;
  _d->lastTimeUpdate = time;

  if( _d->index >= (int)_pictures.size() )
  {
    _d->index = isLoop() ? 0 : -1;
  }
}

const Picture& Animation::currentFrame() const
{
  __D_IMPL_CONST(d,Animation)
  return ( d->index >= 0 && d->index < (int)_pictures.size())
                  ? _pictures[d->index]
                  : Picture::getInvalid();
}

int Animation::index() const { return _dfunc()->index;}
void Animation::setIndex(int index){  _dfunc()->index = math::clamp<int>( index, -1, _pictures.size()-1 );}

Animation::Animation() : __INIT_IMPL(Animation)
{
  setDelay( 0 );
  start( true );
}

Animation::~Animation() {}
Animation::Animation(const Animation& other) : __INIT_IMPL(Animation){  *this = other;}

void Animation::setDelay( const unsigned int delay ){ _dfunc()->delay = delay;}
unsigned int Animation::delay() const{  return _dfunc()->delay; }
void Animation::setLoop( bool loop ){ _dfunc()->loop = loop;}
bool Animation::isLoop() const { return _dfunc()->loop; }

void Animation::load( const std::string &prefix, const int start, const int number, 
                      bool reverse /*= false*/, const int step /*= 1*/ )
{
  int revMul = reverse ? -1 : 1;
  for( int i = 0; i < number; ++i)
  {
    const Picture& pic = Picture::load(prefix, start + revMul*i*step);
    _pictures.push_back( pic );
  }
}

VariantMap Animation::save() const
{
  __D_IMPL_CONST(d,Animation)
  VariantMap ret;
  VARIANT_SAVE_ANY_D( ret, d, index )
  VARIANT_SAVE_ANY_D( ret, d, delay )
  VARIANT_SAVE_ANY_D( ret, d, loop )

  VariantList pics;
  foreach( i, _pictures)
    pics << Variant( (*i).name() );

  ret[ "pictures" ] = pics;

  return ret;
}

void Animation::load(const VariantMap &stream)
{
  __D_IMPL(d,Animation)
  VARIANT_LOAD_ANY_D( d, index, stream )
  VARIANT_LOAD_ANY_D( d, delay, stream )
  VARIANT_LOAD_ANY_D( d, loop, stream );

  VariantMap range = stream.get( "range" ).toMap();
  if( !range.empty() )
  {
    std::string rc = range.get( "rc" ).toString();
    int start = range.get( "start" );
    int number = range.get( "number" );
    for( int k=0; k < number; k++ )
      _pictures.push_back( Picture::load( rc, start + k ) );
  }

  VariantList vl_pics = stream.get( "pictures" ).toList();
  foreach( i, vl_pics )
    _pictures.push_back( Picture::load( (*i).toString() ) );
}

void Animation::clear() { _pictures.clear();}
bool Animation::isRunning() const{  return _dfunc()->index >= 0;}
bool Animation::isStopped() const{  return _dfunc()->index == -1;}
void Animation::stop(){ _dfunc()->index = -1; }

Animation& Animation::operator=( const Animation& other )
{
  __D_IMPL(_d,Animation)
  _pictures = other._pictures;
  _dfunc()->index = other._dfunc()->index;  // index of the current frame
  _d->delay = other.delay();
  _d->lastTimeUpdate = other._dfunc()->lastTimeUpdate;
  _d->loop = other.isLoop();

  return *this;
}

int Animation::size() const {  return _pictures.size();}
bool Animation::isValid() const{  return _pictures.size() > 0;}

void Animation::addFrame(const std::string& resource, int index)
{
  _pictures.push_back( Picture::load( resource, index ) );
}

const Picture& Animation::frame(int index) const
{
  return ( index >= 0 && index < (int)_pictures.size() )
           ? _pictures[ index ]
           : Picture::getInvalid();
}

void Animation::addFrame(const Picture& pic)
{
  _pictures.push_back( pic );
}

}//end namespace gfx
