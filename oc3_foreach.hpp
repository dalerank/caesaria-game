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

#ifndef __OPENCAESAR3_FOREACH_INCLUDE_H__
#define __OPENCAESAR3_FOREACH_INCLUDE_H__

#define foreach(variable, container)                                \
for( ForeachContainer<__typeof__(container)> _container_(container); \
     !_container_.brk && _container_.i != _container_.e;              \
     __extension__  ({ ++_container_.brk; ++_container_.i; }))                       \
    for (variable = *_container_.i;; __extension__ ({--_container_.brk; break;}))

struct ForeachContainerBase {};

template <typename T>
class ForeachContainer : public ForeachContainerBase {
public:
    inline ForeachContainer(const T& t): c(t), brk(0), i(c.begin()), e(c.end()){}
    const T c;
    mutable int brk;
    mutable typename T::const_iterator i, e;
    inline bool condition() const { return (!brk++ && i != e); }
};

template <typename T> inline T *foreachPointer(const T &) { return 0; }

template <typename T> inline ForeachContainer<T> foreachContainerNew(const T& t)
{ return ForeachContainer<T>(t); }

template <typename T>
inline const ForeachContainer<T> *foreachContainer(const ForeachContainerBase *base, const T *)
{ return static_cast<const ForeachContainer<T> *>(base); }

#endif //__OPENCAESAR3_FOREACH_INCLUDE_H__
