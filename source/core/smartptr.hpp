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

#ifndef __CAESARIA_SMARTPTR_H_INCLUDE_
#define __CAESARIA_SMARTPTR_H_INCLUDE_

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
    unsigned int lastRef = obj->rcount();
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

  void detachObject()   { obj = 0; }
  void attachObject(void * anObj) {    obj = (T*)anObj;   }
  T* object()   {    return obj;  }
  T* operator->() const   { return obj; }
  
  SmartPtr()   { obj = 0;  }

  ~SmartPtr()  { dereferenceObject();  }

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

  bool operator==(const SmartPtr<T> &aPtr) const   {    return (obj == aPtr.obj);  }
  bool operator!=(const SmartPtr<T> &aPtr) const  {     return (obj != aPtr.obj);  }
  bool operator<(const SmartPtr<T> &aPtr ) const  {    return (obj < aPtr.obj);  }
  bool operator==(void *ptr) const  {    return ((void*)obj == ptr);  }

  template<class Src>
  SmartPtr& operator<<( SmartPtr<Src> ptr )
  {
    dereferenceObject();
    *this = safety_cast<T*>( ptr.object() );
    return *this;
  }

  bool operator != (void *ptr) const   {    return ((void*)obj != ptr);  }
  T& operator[] (int index)  {    return (*obj)[index];  }
  bool isNull() const  {    return obj == 0;  }
  bool isValid() const   {    return obj != 0;  }
};

template<class A, class B>
inline SmartPtr<A> ptr_cast( SmartPtr<B> ptr ) { return safety_cast<A*>( ptr.object() ); }

template<class A, class B>
inline bool is_kind_of( SmartPtr<B> ptr ) { return safety_cast<A*>( ptr.object() ) != 0; }

#endif //__CAESARIA_SMARTPTR_H_INCLUDE_
