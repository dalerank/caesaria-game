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

#ifndef __OPENCAESAR3_SMARTPTR_H_INCLUDE_
#define __OPENCAESAR3_SMARTPTR_H_INCLUDE_

#include <cstddef>
#include "core/safetycast.hpp"
#include "core/requirements.hpp"
  
template<class T> class SmartPtr
{
protected:
  //bool managed;
  T *obj;
public:
  
  void deleteObject()
  {
    //check again
    if (obj==NULL) return;
  
    //delete object
    delete obj;
    obj = 0;
  }
  
  void dereferenceObject()
  {
    if (obj==0) return;
     
  //check if last reference
  unsigned int lastRef = obj->getReferenceCount();
  obj->drop();

  if ( lastRef == 1)
    obj = 0;
  }
  
  void referenceObject(void *aVObj)
  {
    //convert to T
    T *anObj = (T*)aVObj;

    //cancel if object the same
    if (obj == anObj) return;

    //dereference old object
    dereferenceObject();

    //reference new object
    obj = anObj;
  
    //check again
    if( obj == 0 ) return;

    obj->grab();
  }

  void detachObject()
  {
    obj = 0;
  }

  void attachObject(void *anObj)
  {
    obj = (T*)anObj;
  }
  
  T* object()
  {
    return obj;
  }

  T* operator->() const
  {
    return obj;
  }
  
  SmartPtr()
  {
    obj = 0;
  }

  ~SmartPtr()
  {
    dereferenceObject();
  }

  SmartPtr& operator=(const SmartPtr<T> &aPtr)
  {
    referenceObject(aPtr.obj);
    return *this;
  }

  SmartPtr(T *anObj)
  {
    obj = 0;
    referenceObject(anObj);
    //happens always on "= new <object>"!
  }

  SmartPtr(const SmartPtr<T> &aPtr)
  {
    obj = 0;
    referenceObject(aPtr.obj);
  }

  bool operator==(const SmartPtr<T> &aPtr) const
  {
    return (obj == aPtr.obj);
  }

  bool operator!=(const SmartPtr<T> &aPtr) const
  {
    return (obj != aPtr.obj);
  }

  bool operator<(const SmartPtr<T> &aPtr ) const
  {
    return (obj < aPtr.obj);
  }

  bool operator==(void *ptr) const
  {
    return ((void*)obj == ptr);
  }

  /*operator bool() 
  {
    return (obj != 0);
  }
  */

  bool operator != (void *ptr) const
  {
    return ((void*)obj != ptr);
  }

  T& operator[] (int index)
  {
    return (*obj)[index];
  }

  bool isNull() const
  {
    return obj == 0;
  }

  bool isValid() const
  {
    return obj != 0;
  }

  //Conversion operator
  template<class U> SmartPtr<U> as() const
  {
    SmartPtr<U> newptr( safety_cast<U*>(obj) );
    return newptr;
  }

  template<class U>
  bool is() const
  {
    U* tmp = safety_cast<U*>(obj);
    return ( tmp != 0 );
  }
};

#endif //__OPENCAESAR3_SMARTPTR_H_INCLUDE_
