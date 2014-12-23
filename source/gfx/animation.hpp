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

#ifndef __CAESARIA_ANIMATION_H_INCLUDE_
#define __CAESARIA_ANIMATION_H_INCLUDE_

#include "picturesarray.hpp"
#include "core/variant.hpp"

// several frames for a basic visual animation
namespace gfx
{

class Animation
{
public:
  static const bool reverse = true;
  static const bool straight = false;
  static const bool loopAnimation = true;

  Animation();
  virtual ~Animation();

  Animation( const Animation& other );
  Animation& operator=(const Animation& other);

  void start(bool loop=true);
  void stop();

  Pictures& frames();
  const Pictures& frames() const;
  const Picture& frame( int index ) const;

  unsigned int frameCount() const;

  int index() const;
  void setIndex( int index );

  int size() const;

  void update( unsigned int time );
  const Picture& currentFrame() const;

  void setDelay( const unsigned int delay );
  unsigned int delay() const;

  void setLoop( bool loop );
  bool isLoop() const;

  void setOffset( const Point& offset );
  void addOffset( const Point& offset );
  Point offset() const;

  bool atEnd() const;

  bool isRunning() const;
  bool isStopped() const;

  void clear();
  void load( const std::string &prefix,
             const int start, const int number,
             bool reverse = false, const int step = 1);
  VariantMap save() const;
  void load( const VariantMap& stream );

  bool isValid() const;
  void addFrame( const Picture& pic );
  void addFrame(const std::string& resource, int index);

private:
  Pictures _pictures;

  __DECLARE_IMPL(Animation)
};

}//end namespace gfx
#endif //__CAESARIA_ANIMATION_H_INCLUDE_
