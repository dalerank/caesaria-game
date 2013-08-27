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

#ifndef __OPENCAESAR3_VARIANT_H_INCLUDE__
#define __OPENCAESAR3_VARIANT_H_INCLUDE__

#include "oc3_referencecounted.hpp"
#include "oc3_bytearray.hpp"
#include "oc3_stringarray.hpp"
#include "oc3_time.hpp"
#include "oc3_size.hpp"
#include "oc3_positioni.hpp"
#include "oc3_rectangle.hpp"

#include <list>
#include <map>
#include <typeinfo>

class Variant;
class VariantList;
class VariantMap;

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
    //Variant( const RectF& rect);
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
    //RectF toRectF() const;

    operator int() const { return toInt(); }
    operator float() const { return toFloat(); }

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
    inline Variant(void *) { _OC3_DEBUG_BREAK_IF(true); }
    // force compile error, prevent Variant(QVariant::Type, int) to be called
    inline Variant(bool, int) { _OC3_DEBUG_BREAK_IF(true); }
};

class VariantList : public std::list<Variant>
{
public:
};

StringArray& operator<<(StringArray& strlist, const VariantList& vars );

class VariantMap : public std::map<std::string, Variant>
{
public:
  VariantMap() {}

  VariantMap( const VariantMap& other )
  {
    *this = other;
  }

  VariantMap& operator=(const VariantMap& other )
  {
    for( VariantMap::const_iterator it=other.begin(); it != other.end(); it++ )
    {
      (*this)[ it->first ] = it->second;
    }

    return *this;
  }

  Variant get( const std::string& name ) const
  {
    return const_cast< VariantMap& >( *this )[ name ];
  }

  Variant toVariant() const
  {
    return Variant( *this );
  }
};

inline Variant::Variant() {}

#endif // QVARIANT_H
