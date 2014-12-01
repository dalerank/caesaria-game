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

#include <cstdio>

#include "package_options_window.hpp"
#include "core/stringhelper.hpp"

using namespace constants;

namespace gui
{

namespace dialog
{

PackageOptions::PackageOptions( Widget* parent, const Rect& rectangle )
  : Window( parent, rectangle, "" )
{
  setupUI( ":/gui/packageopts.gui" );
}

PackageOptions::~PackageOptions()
{
}

}//end namespace dialog

}//end namespace gui
