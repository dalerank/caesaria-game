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


#ifndef __OPENCAESAR3_LOADMAPWINDOW_H_INCLUDED__
#define __OPENCAESAR3_LOADMAPWINDOW_H_INCLUDED__

#include "oc3_gui_widget.hpp"
#include "oc3_scopedptr.hpp"
#include "oc3_filepath.hpp"
#include "oc3_signals.hpp"

class Picture;

class LoadMapWindow : public Widget
{
public:
  LoadMapWindow( Widget* parent, const Rect& rect, 
    const io::FilePath& dir, const std::string& ext,
    int id );
  
  ~LoadMapWindow();

  void draw( GfxEngine& engine );  // draw on screen

  Picture& getBgPicture();

  bool onEvent( const NEvent& event);

  bool isPointInside(const Point& point) const;

  void setTitle( const std::string& title );

oc3_signals public:
  Signal1<std::string>& onSelectFile();

protected:
  //void _resizeEvent();

  class Impl;
  ScopedPtr< Impl > _d;
};

#endif
