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
