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

#ifndef __CAESARIA_ALIGNMENT_H_INCLUDED__
#define __CAESARIA_ALIGNMENT_H_INCLUDED__

#include "enumerator.hpp"

enum Alignment
{
  //! Aligned to parent's top or left side (default)
  alignUpperLeft=0,
  //! Aligned to parent's bottom or right side
  alignLowerRight,
  //! Aligned to the center of parent
  alignCenter,
  //! Stretched to fit parent
  alignScale,
  //!
  alignAuto
};

class AlignHelper : public EnumsHelper<Alignment>
{
public:
  AlignHelper();
};

#endif // __CAESARIA_ALIGNMENT_H_INCLUDED__

