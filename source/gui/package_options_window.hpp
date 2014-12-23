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

#ifndef _CAESARIA_PACKAGE_OPTIONS_H_INCLUDE_
#define _CAESARIA_PACKAGE_OPTIONS_H_INCLUDE_

#include "info_box.hpp"

namespace gui
{

namespace dialog
{

class PackageOptions : public Window
{
public:
  PackageOptions(Widget* parent, const Rect& rectangle);
  virtual ~PackageOptions();

private:
  void _saveOptions();
  void _setResourcesPath(std::string);
  void _setCaesar3Path(std::string);
  void _changeCellw();
  void _update();

  class Impl;
  ScopedPtr<Impl> _d;
};

}//end namespace dialog

}//end namespace gui
#endif //_CAESARIA_PACKAGE_OPTIONS_H_INCLUDE_
