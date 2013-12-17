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

#ifndef __CAESARIA_ANIMATION_H_INCLUDE_
#define __CAESARIA_ANIMATION_H_INCLUDE_

#include "picture.hpp"

// several frames for a basic visual animation
class Animation
{
public:
  static const bool reverse = true;
  static const bool straight = false;
  static const bool isLoop = true;

  Animation();
  virtual ~Animation();

  Animation( const Animation& other );
  Animation& operator=(const Animation& other);

  void start(bool loop=true);
  void stop();

  PicturesArray& getFrames();
  const PicturesArray& getFrames() const;

  unsigned int getFrameCount() const;

  int getIndex() const;
  void setIndex( int index );

  int size() const;

  void update( unsigned int time );
  const Picture& getFrame() const;

  void setDelay( const unsigned int delay );
  void setLoop( bool loop );

  void setOffset( const Point& offset );
  Point getOffset() const;

  bool isRunning() const;
  bool isStopped() const;

  void clear();
  void load( const std::string &prefix,
             const int start, const int number,
             bool reverse = false, const int step = 1);

  bool isValid() const;
private:
  PicturesArray _pictures;
  int _animIndex;  // index of the current frame
  unsigned int _frameDelay;
  unsigned int _lastTimeUpdate;

  class Impl;
  ScopedPtr<Impl> _d;
};

#endif //__CAESARIA_ANIMATION_H_INCLUDE_
