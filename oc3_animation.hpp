// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.

#ifndef __OPENCAESAR3_ANIMATION_H_INCLUDE_
#define __OPENCAESAR3_ANIMATION_H_INCLUDE_

#include "oc3_picture.hpp"

class Point;

// several frames for a basic visual animation
class Animation
{
public:
    static const bool reverse = true;
    static const bool straight = false;
    static const bool isLoop = true; 

    typedef std::vector<Picture*> Pictures;

    Animation();

    void start(bool loop=true);
    void stop();

    Pictures& getPictures();
    const Pictures& getPictures() const;

    int getCurrentIndex() const;

    void update( unsigned int time );
    Picture* getCurrentPicture();

    void setFrameDelay( const unsigned int delay );
    void setLoop( bool loop );
   
    void setOffset( const Point& offset );

    bool isRunning() const;
    bool isStopped() const;

    void clear();
    void load(const std::string &prefix, const int start, const int number, bool reverse = false, const int step = 1);
private:
    Pictures _pictures;
    int _animIndex;  // index of the current frame
    unsigned int _frameDelay;
    unsigned int _lastTimeUpdate;

    bool _loop;
};

#endif
