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


#ifndef __OPENCAESAR3_VARIANTPRIVATE_H_INCLUDED__
#define __OPENCAESAR3_VARIANTPRIVATE_H_INCLUDED__

#include "variant.hpp"

template <typename T>
inline const T *v_cast(const Variant2Impl *d, T * = 0)
{
  return static_cast<const T *>(static_cast<const void *>(d->data.ptr));
}

template <typename T>
inline T *v_cast(Variant2Impl *d, T * = 0)
{
  return static_cast<T *>(static_cast<void *>(d->data.ptr));
}

// constructs a new variant if copy is 0, otherwise copy-constructs
template <class T>
inline void v_construct(Variant2Impl *x, const void *copy, T * = 0)
{
  if (copy)
  {
    x->data.ptr = (void*)new T(*static_cast<const T *>(copy));
  }
  else
  {
    x->data.ptr = (void*)new T();
  }
}

template <class T>
inline void v_construct(Variant2Impl* x, const T& t)
{
    x->data.ptr = (void*)new T(t);
}

// deletes the internal structures
template <class T>
inline void v_clear(Variant2Impl *d, T* = 0)
{    
  //now we need to cast
  //because Variant2::PrivateShared doesn't have a virtual destructor
  delete static_cast< T* >(d->data.ptr);
}

#endif // __OPENCAESAR3_VARIANTPRIVATE_H_INCLUDED__
