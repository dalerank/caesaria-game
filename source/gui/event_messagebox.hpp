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

#ifndef _CAESARIA_EVENT_MESSAGEBOX_H_INCLUDE_
#define _CAESARIA_EVENT_MESSAGEBOX_H_INCLUDE_

#include "info_box.hpp"

namespace gui
{

class EventMessageBox : public infobox::Simple
{
public:
  EventMessageBox(Widget* parent, const std::string& title, const std::string& message,
                   DateTime time, good::Product gtype, const std::string& additional="" );
  virtual ~EventMessageBox();
};

}//end namespace gui
#endif //_CAESARIA_EVENT_MESSAGEBOX_H_INCLUDE_
