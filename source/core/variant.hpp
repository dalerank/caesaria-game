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
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_VARIANT_H_INCLUDE__
#define __CAESARIA_VARIANT_H_INCLUDE__

#include "referencecounted.hpp"
#include "bytearray.hpp"
#include "stringarray.hpp"
#include "time.hpp"
#include "size.hpp"
#include "position.hpp"
#include "rectangle.hpp"
#include "foreach.hpp"

#include <list>
#include <map>
#include <typeinfo>

class Variant;
class VariantList;
class VariantMap;

#define VARIANT_INIT_ANY( type, param, vm) type param = vm.get( #param );
#define VARIANT_INIT_STR( param, vm) std::string param = vm.get( #param ).toString();

#define VARIANT_SAVE_ANY(vm,param) vm[ #param ] = param;
#define VARIANT_LOAD_ANY(param,vm) param = vm.get( #param );
#define VARIANT_LOAD_ANYDEF(param, vm, defvalue) param = vm.get( #param, defvalue );
#define VARIANT_LOAD_STR(param,vm) param = vm.get( #param ).toString();
#define VARIANT_LOAD_TIME(param,vm) param = vm.get( #param ).toDateTime();

#define VARIANT_SAVE_ANY_D(vm,d,param) vm[ #param ] = d->param;
#define VARIANT_SAVE_STR_D(vm,d,param) vm[ #param ] = Variant( d->param );
#define VARIANT_LOAD_ANY_D(d,param,vm) d->param = vm.get( #param );
#define VARIANT_LOAD_ANYDEF_D(d,param,def,vm) d->param = vm.get( #param, (def) );

#define VARIANT_SAVE_ENUM_D(vm,d,param) vm[ #param ] = (int)d->param;
#define VARIANT_LOAD_ENUM_D(d,param,vm) d->param = (__typeof__(d->param))vm.get( #param ).toInt();

#define VARIANT_LOAD_STR_D(d,param,vm) d->param = vm.get( #param ).toString();
#define VARIANT_LOAD_TIME_D(d,param,vm) d->param = vm.get( #param ).toDateTime();
#define VARIANT_LOAD_VMAP( param, vm ) param = vm.get( #param ).toMap();

template <typename T>
inline Variant createVariant2FromValue(const T &);

template <typename T>
inline void variant2SetValue( Variant &, const T &);

template<typename T>
inline T getVariant2Value(const Variant &);

template<typename T>
inline bool Variant2CanConvert(const Variant &);

struct Variant2Impl
{
    inline Variant2Impl(): type(0), is_null(true) { data.ptr = 0; }
    inline Variant2Impl(const Variant2Impl& other)
        : data(other.data), type(other.type),
          is_null(other.is_null)
    {
    }

    union Data
    {
        char c;
        int i;
        unsigned int u;
        bool b;
        double d;
        float f;
        long long ll;
        unsigned long long ull;
        ReferenceCounted* o;
        void* ptr;
    } data;
    unsigned int type : 30;
    unsigned int reserved : 1;
    unsigned int is_null : 1;
};

class Variant
{
 public:
    enum Type 
    {
        Invalid = 0,
        Bool = 1,
        Int = 2,
        UInt = 3,
        LongLong = 4,
        ULongLong = 5,
        Double = 6,
        Char = 7,
        Uchar = 8,
        Ushort = 9,
        Ulong = 10,
        Long = 11,
        Float = 12,
        Short = 13,
        Map = 14,
        List = 15,
        String = 16,
        NStringArray = 17,
        NByteArray = 18,
        BitArray = 19,
        Date = 20,
        Time = 21,
        NDateTime = 22,
        Url = 23,
        NRectI = 24,
        NRectF = 25,
        NSize = 26,
        NSizeF = 27,
        Line = 28,
        LineF = 29,
        NPoint = 30,
        NPointF = 31,

        LastCoreType = NPointF,

        Font = 64,
        Pixmap = 65,
        Brush = 66,
        Color = 67,
        Palette = 68,
        Icon = 69,
        Image = 70,
        Polygon = 71,
        Region = 72,
        Bitmap = 73,
        Cursor = 74,
        SizePolicy = 75,
        KeySequence = 76,
        Pen = 77,
        TextLength = 78,
        TextFormat = 79,
        Matrix = 80,
        Transform = 81,
        Matrix4x4 = 82,
        Vector2D = 83,
        Vector3D = 84,
        Vector4D = 85,
        NTilePos = 86,
        Quaternion = 87,
        LastGuiType = Quaternion,

        UserType = 127,
        LastType = 0xffffffff // need this so that gcc >= 3.4 allocates 32 bits for Type
    };

    inline Variant();
    ~Variant();
    Variant( Type type );
    Variant( int typeOrUserType, const void *copy);
    Variant( int typeOrUserType, const void *copy, unsigned int flags);
    Variant( const Variant &other);

    Variant( int i );
    Variant( unsigned int ui);
    Variant( long long ll);
    Variant( unsigned long long ull);
    Variant( bool b);
    Variant( double d);
    Variant( float f);

    explicit Variant( const char* string );
    Variant( const ByteArray& bytearray );
    explicit Variant( const std::string& string);
    Variant( const StringArray& stringlist );
    Variant( char rchar);
    Variant( const DateTime& datetime);
    Variant( const VariantList& list);
    Variant( const TilePos& pos );
    Variant( const VariantMap& mapa);

    Variant( const Size& size);
    Variant( const SizeF& size);
    Variant( const Point& pt);
    Variant( const PointF& pt);
    //Variant( const Line& line);
    //Variant( const LineF& line);
    Variant( const Rect& rect);
    Variant( const RectF& rect);
    //Variant( const Color& color);

    Variant& operator=( const Variant& other);

    Type type() const;
    int userType() const;
    std::string typeName() const;

    bool canConvert( Type t) const;
    bool convert( Type t);

    bool isValid() const;
    bool isNull() const;

    void clear();

    int toInt(bool *ok = 0) const;
	  //Color toColor() const;
    unsigned int toUInt(bool *ok = 0) const;
    long long toLongLong(bool *ok = 0) const;
    unsigned long long toULongLong(bool *ok = 0) const;
    bool toBool() const;
    double toDouble(bool *ok = 0) const;
    float toFloat(bool *ok = 0) const;
    ByteArray toByteArray() const;
    std::string toString() const;
    StringArray toStringArray() const;
    char toChar() const;
    DateTime toDateTime() const;
    VariantList toList() const;
    VariantMap toMap() const;

    Point toPoint() const;
    PointF toPointF() const;
    Rect toRect() const;
    Size toSize() const;
    SizeF toSizeF() const;
    TilePos toTilePos() const;
    //Line toLine() const;
    //LineF toLineF() const;
    RectF toRectf() const;

    operator unsigned int() const { return toUInt(); }
    operator int() const { return toInt(); }
    operator float() const { return toFloat(); }
    operator bool() const { return toBool(); }
    operator TilePos() const { return toTilePos(); }
    operator Point() const { return toPoint(); }
    operator PointF() const { return toPointF(); }

    static std::string typeToName(Type type);
    static Type nameToType(const std::string& name);

    void *data();
    const void *constData() const;
    inline const void *data() const { return constData(); }

    template<typename T>
    static inline Variant fromValue( const T &value )
    { return createVariant2FromValue( value ); }

    template<typename T>
    bool canConvert() const
    { return Variant2CanConvert<T>(*this); }

    inline bool operator==(const Variant& v) const
    { return cmp(v); }
    inline bool operator!=(const Variant& v) const
    { return !cmp(v); }

protected:
    Variant2Impl _d;

    void create( unsigned int type, const void *copy);
    bool cmp(const Variant &other) const;

private:
    // force compile error, prevent Variant(bool) to be called
    inline Variant(void *) { _CAESARIA_DEBUG_BREAK_IF(true); }
    // force compile error, prevent Variant(QVariant::Type, int) to be called
    inline Variant(bool, int) { _CAESARIA_DEBUG_BREAK_IF(true); }
};

class VariantList : public std::list<Variant>
{
public:
  VariantList() {}

  Variant get( const unsigned int index, Variant defaultVal=Variant() ) const
  {
    VariantList::const_iterator it = begin();
    if( index >= size() )
    {
      return defaultVal;
    }
    else
    {
      std::advance( it, index );
      return *it;
    }
  }

  VariantList& operator <<( const Variant& v )
  {
    push_back( v );
    return *this;
  }

  template<class T>
  VariantList( std::vector<T> array )
  {
    //typename std::vector<T>::iterator it = array.begin();
    foreach( it, array )
    {
      push_back( Variant(*it) );
    }
  }
};

template<class T>
typename std::vector<T>& operator<<(std::vector<T>& v, const VariantList& vars)
{
  VariantList::const_iterator it = vars.begin();
  for( ; it != vars.end(); ++it )
  {
    v.push_back( (T)(*it) );
  }

  return v;
}

StringArray& operator<<(StringArray& strlist, const VariantList& vars );

inline Variant::Variant() {}

#endif // __CAESARIA_VARIANT_H_INCLUDE__
