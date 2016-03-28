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
// Copyright 2012-2014 Gregoire Athanase, gathanase@gmail.com
// Copyright 2012-2014 Dalerank, dalerank@gmail.com

#ifndef _CAESARIA_SCENE_LOGO_HPP_INCLUDE_
#define _CAESARIA_SCENE_LOGO_HPP_INCLUDE_

#include "base.hpp"
#include "core/scopedptr.hpp"

namespace scene
{

// displays a background image
class SplashScreen: public Base
{
public:
  enum { hideDevText=0, showDevText=1, exit=5 };

  SplashScreen();
  virtual ~SplashScreen();

  void initialize();

  virtual void draw(gfx::Engine& engine);
  void setBackground(const std::string& image, int index);
  void exitScene(bool devText);
  void setText( std::string text );
  void setPrefix( std::string prefix );
  virtual void setOption(const std::string& name, Variant value);

protected:
  virtual int result() const;

private:
  class Impl;
  ScopedPtr<Impl> _d;
};

}//end namespace scene

#endif //_CAESARIA_SCENE_LOGO_HPP_INCLUDE_
