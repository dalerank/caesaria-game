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

#include "oc3_goodhelper.hpp"
#include "oc3_good.hpp"
#include <vector>

class GoodHelper::Impl
{
public:
  std::vector<Good> mapGood;  // index=GoodType, value=Good
};

GoodHelper& GoodHelper::getInstance()
{
  static GoodHelper inst;
  return inst;
}

GoodHelper::GoodHelper() : _d( new Impl )
{
  _d->mapGood.resize(G_MAX);

  for (int n = 0; n < G_MAX; ++n)
  {
    GoodType goodType = GoodType(n);
    _d->mapGood[n].init( goodType );
  }
}

GoodHelper::~GoodHelper()
{

}

std::string GoodHelper::getName( GoodType type ) const
{
  return _d->mapGood[ type ].getName();
}