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

#include "animation.hpp"
#include "core/position.hpp"
#include "core/foreach.hpp"
#include "core/stringhelper.hpp"

class Animation::Impl
{
public:
  bool loop;
};

void Animation::start(bool loop)
{
  _animIndex = 0;
  _lastTimeUpdate = 0;
  _d->loop = loop;
}

PicturesArray& Animation::getFrames() {  return _pictures;}
const PicturesArray& Animation::getFrames() const{  return _pictures;}
unsigned int Animation::getFrameCount() const{  return _pictures.size();}

void Animation::setOffset( const Point& offset )
{
  foreach( pic, _pictures ) { pic->setOffset( offset ); }
}

Point Animation::getOffset() const
{
  if( _pictures.empty() )
  {
    return Point();
  }

  return _pictures.front().getOffset();
}

void Animation::update( unsigned int time )
{  
  if( _animIndex < 0 )
    return;

  if( _frameDelay > 0 )
  {
    if( time - _lastTimeUpdate < _frameDelay )
      return;
  }

  _animIndex += 1;
  _lastTimeUpdate = time;

  if( _animIndex >= (int)_pictures.size() ) 
  {
    _animIndex = _d->loop ? 0 : -1;
  }
}

const Picture& Animation::getFrame() const
{
  return ( _animIndex >= 0 && _animIndex < (int)_pictures.size())
                  ? _pictures[_animIndex] 
                  : Picture::getInvalid();
}

int Animation::getIndex() const {  return _animIndex;}
void Animation::setIndex(int index){  _animIndex = math::clamp<int>( index, 0, _pictures.size()-1 );}

Animation::Animation() : _d( new Impl )
{
  _frameDelay = 0;
  start( true );
}

Animation::~Animation() {}
Animation::Animation(const Animation& other) : _d( new Impl ){  *this = other;}
void Animation::setDelay( const unsigned int delay ){  _frameDelay = delay;}
void Animation::setLoop( bool loop ){  _d->loop = loop;}

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

void Animation::clear() { _pictures.clear();}
bool Animation::isRunning() const{  return _animIndex >= 0;}
bool Animation::isStopped() const{  return _animIndex == -1;}
void Animation::stop(){  _animIndex = -1;}

Animation& Animation::operator=( const Animation& other )
{
  _pictures = other._pictures;
  _animIndex = other._animIndex;  // index of the current frame
  _frameDelay = other._frameDelay;
  _lastTimeUpdate = _lastTimeUpdate;
  _d->loop = other._d->loop;

  return *this;
}

int Animation::size() const {  return _pictures.size();}
bool Animation::isValid() const{  return _pictures.size() > 0;}
