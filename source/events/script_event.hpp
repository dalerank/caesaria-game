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

#ifndef _CAESARIA_EVENT_SCRIPTFUNC_H_INCLUDE_
#define _CAESARIA_EVENT_SCRIPTFUNC_H_INCLUDE_

#include "event.hpp"
#include "core/variant_list.hpp"

namespace events
{

class SciptFunc : public GameEvent
{
public:
  static GameEventPtr create(const std::string& funcname,
                             const VariantList& params = VariantList());

protected:
  virtual void _exec( Game& game, unsigned int );
  virtual bool _mayExec(Game &game, unsigned int time) const;

private:
  SciptFunc(const std::string& funcname, const VariantList& params);
  std::string _funcname;
  VariantList _params;
};

}//end namespace events

#endif //_CAESARIA_EVENT_SCRIPTFUNC_H_INCLUDE_
