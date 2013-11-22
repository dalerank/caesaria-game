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

#ifndef __CAESARIA_FOREACH_INCLUDE_H__
#define __CAESARIA_FOREACH_INCLUDE_H__

#define foreach(variable, container)                                \
for( ForeachContainer<__typeof__(container)> _container_(container); \
     !_container_.brk && _container_.i != _container_.e;              \
     __extension__  ({ ++_container_.brk; ++_container_.i; }))                       \
    for (variable = *_container_.i;; __extension__ ({--_container_.brk; break;}))

struct ForeachContainerBase {};

template <typename T>
class ForeachContainer : public ForeachContainerBase {
public:
    inline ForeachContainer(T& t): c(t), brk(0), i(c.begin()), e(c.end()){}
    T& c;
    mutable int brk;
    mutable typename T::iterator i, e;
    inline bool condition() const { return (!brk++ && i != e); }
};

template <typename T> inline T* foreachPointer(T &) { return 0; }

template <typename T> inline ForeachContainer<T> foreachContainerNew(T& t)
{ return ForeachContainer<T>(t); }

template <typename T>
inline ForeachContainer<T>* foreachContainer(ForeachContainerBase *base, T *)
{ return ForeachContainer<T>(base); }

#endif //__CAESARIA_FOREACH_INCLUDE_H__
