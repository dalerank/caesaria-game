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
// Copyright 2014-2014 Dalerank, dalerankn8@gmail.com

#include "metric.hpp"

namespace metric
{

const char* Measure::measureType()
{
  switch( instance()._mode )
  {
  case native: return "##quantity##";
  case metric: return "##kilogram##";
  case roman:  return "##modius##";
  default: return "unknown";
  }
}

const char *Measure::measureShort()
{
  switch( instance()._mode )
  {
  case native: return "##qty##";
  case metric: return "##kg##";
  case roman:  return "##md##";
  default: return "unknown";
  }
}

int Measure::convQty(int qty)
{
  switch( instance()._mode )
  {
  case native: return qty;
  case metric: return qty / 2;
  case roman: return qty / 7;
  default: return qty;
  }
}

}//end namespace metric
