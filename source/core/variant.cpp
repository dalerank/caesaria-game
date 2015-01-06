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

#include <cstring>

#include "variant.hpp"
#include "variant_map.hpp"
#include "variantprivate.hpp"
#include "utils.hpp"

static Variant::Type staticNameToType( const char* name )
{
	if( !strcmp( name, typeid(bool).name() ) )
		return Variant::Bool;
	if( !strcmp( name, typeid(int).name() ) )
		return Variant::Int;
	if( !strcmp( name, typeid(bool).name() ) )
		return Variant::Bool;
	if( !strcmp( name, typeid(unsigned int).name() ) )
		return Variant::UInt;
	if( !strcmp( name, typeid(long long).name() ) )
		return Variant::LongLong;
	if( !strcmp( name, typeid(bool).name() ) )
		return Variant::Bool;
	if( !strcmp( name, typeid(double).name() ) )
		return Variant::Double;
	if( !strcmp( name, typeid(char).name() ) )
		return Variant::Char;
	if( !strcmp( name, typeid(VariantMap).name() ) )
		return Variant::Map;
	if( !strcmp( name, typeid(VariantList).name() ) )
		return Variant::List;
  if( !strcmp( name, typeid(std::string).name() ) )
		return Variant::String;
	if( !strcmp( name, typeid(StringArray).name() ) )
		return Variant::NStringArray;
	if( !strcmp( name, typeid(ByteArray).name() ) )
		return Variant::NByteArray;
	if( !strcmp( name, typeid(DateTime).name() ) )
		return Variant::NDateTime;
	if( !strcmp( name, typeid(bool).name() ) )
		return Variant::Bool;
	if( !strcmp( name, typeid(Rect).name() ) )
		return Variant::NRectI;
	//if( !strcmp( name, typeid(RectF).name() ) )
	//	return Variant::NRectF;
	if( !strcmp( name, typeid(Size).name() ) )
		return Variant::NSize;
	//if( !strcmp( name, typeid(NSizeF).name() ) )
	//	return Variant::SizeF;
// 	if( !strcmp( name, typeid(Line).name() ) )
// 		return Variant::Line;
// 	if( !strcmp( name, typeid(LineF).name() ) )
// 		return Variant::LineF;
	if( !strcmp( name, typeid(Point).name() ) )
		return Variant::NPoint;
  if( !strcmp( name, typeid(TilePos).name() ) )
    return Variant::NTilePos;
	if( !strcmp( name, typeid(PointF).name() ) )
		return Variant::NPointF;
	//if( !strcmp( name, typeid(Font).name() ) )
	//	return Variant::Font;
// 	if( !strcmp( name, typeid(Pixmap).name() ) )
// 		return Variant::Pixmap;
// 	if( !strcmp( name, typeid(Color).name() ) )
// 		return Variant::Color;
// 	
  if( !strcmp( name, typeid(unsigned char).name() ) )
		return Variant::Uchar;
	if( !strcmp( name, typeid(unsigned short).name() ) )
		return Variant::Ushort;
	if( !strcmp( name, typeid(unsigned long).name() ) )
		return Variant::Ulong;
	if( !strcmp( name, typeid(long).name() ) )
		return Variant::Long;
	if( !strcmp( name, typeid(float).name() ) )
		return Variant::Float;

	return Variant::Invalid;
}

static std::string staticTypeToName( Variant::Type t)
{
    switch( t )
	{
	case Variant::Invalid:	return "Invalid";
	case Variant::Bool : return "Bool";
	case Variant::Int : return "Int";
	case Variant::UInt : return "UInt";
	case Variant::LongLong : return "LongLong";
	case Variant::ULongLong : return "ULongLong";
	case Variant::Double : return "Double";
	case Variant::Char : return "Char";
	case Variant::Map : return "Map";
	case Variant::List : return "List";
	case Variant::String : return "String";
	case Variant::NStringArray : return "NStringArray";
	case Variant::NByteArray : return "NByteArray";
	case Variant::NDateTime : return "DateTime";
	case Variant::NRectI : return "NRectI";
	case Variant::NRectF : return "NRectF";
	case Variant::NSize : return "Size";
	case Variant::NSizeF : return "SizeF";
	case Variant::Line : return "Line";
	case Variant::LineF : return "LineF";
	case Variant::NPoint : return "Point";
  case Variant::NTilePos : return "TilePos";
	case Variant::NPointF : return "PointF";
	case Variant::Font : return "Font";
	case Variant::Pixmap : return "Pixmap";
	case Variant::Color : return "Color";
	case Variant::Uchar : return "Uchar";
	case Variant::Ushort : return "Ushort";
	case Variant::Ulong : return "ULong";
	case Variant::Long : return "Long";
	case Variant::Float : return "Float";
	case Variant::UserType : return "UserType";
	default: return "";
	}
}

typedef void (*f_construct)(Variant2Impl*, const void *);
typedef void (*f_clear)(Variant2Impl*);
typedef bool (*f_null)(const Variant2Impl*);
typedef bool (*f_compare)(const Variant2Impl*, const Variant2Impl*);
typedef bool (*f_convert)(const Variant2Impl* d, Variant::Type t, void *, bool *);
typedef bool (*f_canConvert)(const Variant2Impl* d, Variant::Type t);

struct Variant2Handler 
{
  f_construct construct;
  f_clear clear;
  f_null isNull;
  f_compare compare;
  f_convert convert;
  f_canConvert canConvert;

  Variant2Handler();
};

static Variant2Handler* varHandler = new Variant2Handler();

static bool checkVariantNull( const Variant2Impl *x )
{
  return x->is_null;
}

static void constructNewVariant( Variant2Impl *x, const void *copy)
{
	switch (x->type) 
	{
	case Variant::String:
    v_construct<std::string>(x, copy);
		break;
	case Variant::NStringArray:
		v_construct<StringArray>(x, copy);
		break;
	case Variant::Map:
		v_construct<VariantMap>(x, copy);
		break;
		/*    case Variant::Hash:
		v_construct<VariantHash>(x, copy);
		break;
		*/
	case Variant::List:
		v_construct<VariantList>(x, copy);
		break;
		/*    case Variant::Date:
		v_construct<Date>(x, copy);
		break;
		case Variant::Time:
		v_construct<Time>(x, copy);
		break;
		*/
	case Variant::NDateTime:
		v_construct<DateTime>(x, copy);
		break;
	case Variant::NByteArray:
		v_construct<ByteArray>(x, copy);
		break;
		/*    case Variant::BitArray:
		v_construct<BitArray>(x, copy);
		break;
		*/
	case Variant::NSize:
		v_construct<Size>(x, copy);
		break;
	case Variant::NSizeF:
		v_construct<SizeF>(x, copy);
		break;
	case Variant::NRectI:
		v_construct<Rect>(x, copy);
		break;
// 	case Variant::LineF:
// 		v_construct<LineF>(x, copy);
// 		break;
  case Variant::NRectF:
    v_construct<RectF>(x, copy);
    break;
	case Variant::NPoint:
		v_construct<Point>(x, copy);
		break;
  case Variant::NTilePos:
    v_construct<TilePos>(x, copy);
    break;
	case Variant::NPointF:
		v_construct<PointF>(x, copy);
		break;
// 	case Variant::Color:
// 		v_construct<Color>(x, copy);
// 		break;
	case Variant::Invalid:
	case Variant::UserType:
		break;
	default:
		_CAESARIA_DEBUG_BREAK_IF( true && "can't create variant" );
		break;
	}
	x->is_null = !copy;
}

static void clearVariant(Variant2Impl *d)
{
  switch( d->type ) 
	{
		case Variant::String: v_clear<std::string>(d); break;
		case Variant::NStringArray: v_clear<StringArray>(d); break;
		case Variant::Map: v_clear<VariantMap>(d); break;
		case Variant::List: v_clear<VariantList>(d); break;
    /*case Variant::Date:
        v_clear<Date>(d);
        break;
    case Variant::Time:
        v_clear<Time>(d);
        break;
    */
    case Variant::NDateTime: v_clear<DateTime>(d); break;
    case Variant::NByteArray: v_clear<ByteArray>(d); break;
    case Variant::NPoint: v_clear<Point>(d); break;
    case Variant::NTilePos: v_clear<TilePos>(d); break;
    case Variant::NPointF: v_clear<PointF>(d); break;
    case Variant::NSize: v_clear<Size>(d); break;
    case Variant::NSizeF: v_clear<SizeF>(d); break;
    case Variant::NRectI: v_clear<Rect>(d); break;
//     case Variant::LineF:
//         v_clear<LineF>(d);
//         break;
//     case Variant::Line:
//         v_clear<Line>(d);
//         break;
     case Variant::NRectF: v_clear<RectF>(d); break;
/*    case Variant::Url:
        v_clear<Url>(d);
        break;
  */
    case Variant::LongLong:
    case Variant::ULongLong:
    case Variant::Double:
    case Variant::Float:
    case Variant::Invalid:
    case Variant::UserType:
    case Variant::Int:
    case Variant::UInt:
    case Variant::Bool:
      break;
    default:
      _CAESARIA_DEBUG_BREAK_IF( true && "Can't clean variant" );
      break;
    }

    d->type = Variant::Invalid;
    d->is_null = true;
}

template<typename T>
inline bool compareNumericMetaType(const Variant2Impl *const a, const Variant2Impl *const b)
{
    return *static_cast<const T *>(a->data.ptr) == *static_cast<const T *>(b->data.ptr);
}

/*static bool compare(const Variant2Impl *a, const Variant2Impl *b)
{
    switch(a->type) {
    //case Variant::List:
    //    return *v_cast<VariantList>(a) == *v_cast<VariantList>(b);
    //case Variant::Map: 
    //    {
    //    const VariantMap *m1 = v_cast<VariantMap>(a);
    //    const VariantMap *m2 = v_cast<VariantMap>(b);
    //    if (m1->count() != m2->count())
    //        return false;
    //    VariantMap::ConstIterator it = m1->constBegin();
    //    VariantMap::ConstIterator it2 = m2->constBegin();
    //    while (it != m1->constEnd()) {
    //        if (*it != *it2 || it.key() != it2.key())
    //            return false;
    //        ++it;
    //        ++it2;
    //    }
    //   return true;
    //}
    // 
    //case Variant::Hash:
    //    return *v_cast<VariantHash>(a) == *v_cast<VariantHash>(b);
    case Variant::String:
     return *v_cast<std::string>(a) == *v_cast<std::string>(b);
    //case Variant::NStringArray:
    //   return *v_cast<StringList>(a) == *v_cast<StringList>(b);
    case Variant::NSize: return *v_cast<Size>(a) == *v_cast<Size>(b);
    case Variant::NSizeF: return *v_cast<SizeF>(a) == *v_cast<SizeF>(b);
    case Variant::NRectI: return *v_cast<Rect>(a) == *v_cast<Rect>(b);
    case Variant::NRectF: return *v_cast<RectF>(a) == *v_cast<RectF>(b);
//     case Variant::Line:
//         return *v_cast<Line>(a) == *v_cast<Line>(b);
//     case Variant::LineF:
//         return *v_cast<LineF>(a) == *v_cast<LineF>(b);
    case Variant::NPoint: return *v_cast<Point>(a) == *v_cast<Point>(b);
    case Variant::NPointF: return *v_cast<PointF>(a) == *v_cast<PointF>(b);
    case Variant::NTilePos: return *v_cast<TilePos>(a) == *v_cast<TilePos>(b);
//    case Variant::Url:
//        return *v_cast<Url>(a) == *v_cast<Url>(b);
    case Variant::Char:
      return a->data.c == b->data.c;
    case Variant::Int:
        return a->data.i == b->data.i;
    case Variant::UInt:
        return a->data.u == b->data.u;
    case Variant::LongLong:
        return a->data.ll == b->data.ll;
    case Variant::ULongLong:
        return a->data.ull == b->data.ull;
    case Variant::Bool:
        return a->data.b == b->data.b;
    case Variant::Double:
        return a->data.d == b->data.d;
    case Variant::Float:
        return a->data.f == b->data.f;
//    case Variant::Date:
//        return *v_cast<Date>(a) == *v_cast<Date>(b);
//    case Variant::Time:
//        return *v_cast<Time>(a) == *v_cast<Time>(b); 
    case Variant::NDateTime: return *v_cast<DateTime>(a) == *v_cast<DateTime>(b);
    case Variant::NByteArray: return *v_cast<ByteArray>(a) == *v_cast<ByteArray>(b);
    case Variant::Invalid: return true;
    case Variant::Long: return compareNumericMetaType<long>(a, b);
    case Variant::Ulong: return compareNumericMetaType<unsigned long>(a, b);
    case Variant::Short: return compareNumericMetaType<short>(a, b);
    case Variant::Ushort: return compareNumericMetaType<unsigned short>(a, b);
    case Variant::Uchar: return compareNumericMetaType<unsigned char>(a, b);
    default:
        break;
    }

    const void *a_ptr = a->data.ptr;
    const void *b_ptr = b->data.ptr;

    if (a->is_null && b->is_null)
        return true;

    return a_ptr == b_ptr;
}*/

/*!
  \internal
 */
static long long Variant2Number(const Variant2Impl *d)
{
    switch (d->type) {
    case Variant::Int:
        return d->data.i;
    case Variant::LongLong:
        return d->data.ll;
    case Variant::Char:        
        return (long long)( d->data.c );
    case Variant::Short:
        return (long long)( d->data.i );
    case Variant::Long:
        return (long long)( d->data.i );
    case Variant::Float:
        return (long long)floorf( d->data.f );
    case Variant::Double:
        return (long long)floor( d->data.d );
    }
    _CAESARIA_DEBUG_BREAK_IF( true );
    return 0;
}

static unsigned long long Variant2UNumber(const Variant2Impl *d)
{
    switch (d->type) {
    case Variant::UInt:
        return d->data.u;
    case Variant::ULongLong:
        return d->data.ull;
    case Variant::Char:
        return (unsigned long long)( d->data.c );
    case Variant::Ushort:
        return (unsigned long long)( d->data.i );
    case Variant::Ulong:
        return (unsigned long long)( d->data.ll );
    }
    _CAESARIA_DEBUG_BREAK_IF( true );
    return 0;
}

static long long ConvertToNumber(const Variant2Impl *d, bool *ok)
{
    *ok = true;

    switch( (unsigned long long)(d->type) )
    {
    case Variant::String:
      return utils::toInt( v_cast<std::string>(d)->c_str() );
    case Variant::NByteArray:
      return utils::toInt( &(*v_cast<ByteArray>(d))[0] );
    case Variant::Bool:
        return (long long)(d->data.b);
    case Variant::Double:
    case Variant::Int:
    case Variant::Char:
    case Variant::Uchar:
    case Variant::Short:
    case Variant::Long:
    case Variant::Float:
    case Variant::LongLong:
        return Variant2Number(d);
    case Variant::ULongLong:
    case Variant::UInt:
    case Variant::Ushort:
    case Variant::Ulong:
        return (long long)(Variant2UNumber(d));
    }

    *ok = false;
    return 0;
}

static unsigned long long ConvertToUnsignedNumber(const Variant2Impl *d, bool *ok)
{
    *ok = true;

    switch((unsigned int)(d->type)) 
    {
    case Variant::String:
      return utils::toUint( v_cast<std::string>(d)->c_str() );
    case Variant::NByteArray:
      return utils::toUint( &(*v_cast<ByteArray>(d))[0] );
    case Variant::Bool:
        return (unsigned long long)(d->data.b);
    case Variant::Double:
    case Variant::Int:
    case Variant::Char:
    case Variant::Short:
    case Variant::Long:
    case Variant::Float:
    case Variant::LongLong:
        return (unsigned long long)(Variant2Number(d));
    case Variant::ULongLong:
    case Variant::UInt:
    case Variant::Uchar:
    case Variant::Ushort:
    case Variant::Ulong:
        return Variant2UNumber(d);
    }

    *ok = false;
    return 0;
}

template<typename TInput, typename LiteralWrapper>
inline bool convertToBool(const Variant2Impl *const d)
{
    TInput str = v_cast<TInput>(d)->toLower();
    return !(str == LiteralWrapper("0") || str == LiteralWrapper("false") || str.isEmpty());
}

/*!
 \internal

 Converts \a d to type \a t, which is placed in \a result.
 */
static bool convertVariantType2Type(const Variant2Impl *d, Variant::Type t, void *result, bool *ok)
{
    _CAESARIA_DEBUG_BREAK_IF ( d->type == (unsigned int)( t ) );
    _CAESARIA_DEBUG_BREAK_IF( !result );

    bool dummy;
    if (!ok)
        ok = &dummy;

    switch( (unsigned int)(t) )
    {
    /*case Variant::Url:
        switch (d->type) {
        case Variant::String:
            *static_cast<Url *>(result) = Url(*v_cast<String>(d));
            break;
        default:
            return false;
        }
        break;
    */
        case Variant::String: 
        {
          std::string *str = static_cast<std::string*>(result);
            switch (d->type) 
            {
            case Variant::Char: *str = d->data.c; 
			      break;

            case Variant::Uchar: *str = utils::format( 0xff, "%d", (unsigned char)Variant2Number(d) );
            break;
            
            case Variant::Short:
            case Variant::Long:
            case Variant::Int:
            case Variant::LongLong:
              *str = utils::format( 0xff, "%d", (long long)Variant2Number(d) );
            break;

            case Variant::UInt:
            case Variant::ULongLong:
            case Variant::Ushort:
            case Variant::Ulong:
                *str = utils::format( 0xff, "%u", (unsigned long)Variant2UNumber(d) );
            break;

            case Variant::Float:
                *str = utils::format( 0xff, "%f", d->data.f );
            break;
            case Variant::Double:
                *str = utils::format( 0xff, "%g", d->data.d );
            break;
    
            case Variant::NDateTime:
              {
                const DateTime* dt = v_cast<DateTime>(d);
                *str = utils::format( 0xff, "%04d.%02d.%02d %02d:%02d:%02d", dt->year(), dt->month(),
                                             dt->day(), dt->hour(), dt->minutes(), dt->seconds() );
              }
            break;
            case Variant::Bool:
              *str = std::string( d->data.b ? "true" : "false" );
            break;
            case Variant::NByteArray:
              *str = std::string( &(*v_cast<ByteArray>(d))[0] );
            break;
            case Variant::NStringArray:
              if( v_cast<StringArray>(d)->size() == 1)
              {
                *str = (*v_cast<StringArray>(d))[0];
              }
            break;
            /*case Variant::Url:
                *str = v_cast<Url>(d)->toString();
            break;*/
            default:
                return false;
        }
        break;
    }
    case Variant::Char: {
        char *c = static_cast<char*>(result);
        switch (d->type) 
        {
        case Variant::Int:
        case Variant::LongLong:
        case Variant::Char:
        case Variant::Short:
        case Variant::Long:
        case Variant::Float:
            *c = (char)(static_cast<unsigned short>(Variant2Number(d)));
            break;
        case Variant::UInt:
        case Variant::ULongLong:
        case Variant::Uchar:
        case Variant::Ushort:
        case Variant::Ulong:
            *c = (char)(static_cast<unsigned short>(Variant2UNumber(d)));
            break;
        default:
            return false;
        }
        break;
    }

    case Variant::NSize: 
      {
        Size *s = static_cast<Size*>(result);
        switch (d->type)
        {
        case Variant::NSizeF:
          *s = v_cast<SizeF>(d)->toSize();
        break;

        case Variant::List:
        {
          const VariantList *list = v_cast< VariantList >(d);
          s->setWidth( list->get( 0, 0 ).toInt() );
          s->setHeight( list->get( 1, 0 ).toInt() );
        }
        break;
		           
        default:
        return false;
        }
        break;
      }

    case Variant::NSizeF: 
    {
        SizeF *s = static_cast<SizeF*>(result);
        switch (d->type) 
        {
        case Variant::NSize:
          *s = v_cast<Size>(d)->toSizeF();
        break;
		           
        default:
            return false;
        }
        break;
    }

//     case Variant::Line:
//     {
//         Line *s = static_cast<Line*>(result);
//         switch (d->type) 
//         {
//         case Variant::LineF:
//             *s = v_cast<LineF>(d)->As<int>();
//             break;
//         default:
//             return false;
//         }
//         break;
//     }

//     case Variant::LineF: 
//     {
//         LineF *s = static_cast<LineF *>(result);
//         switch (d->type) 
//         {
//         case Variant::Line:
//             *s = v_cast<Line>(d)->As<float>();
//             break;
//         default:
//             return false;
//         }
//         break;
//     }
    case Variant::NTilePos:
      if( d->type == Variant::List )
      {
        TilePos *pos = static_cast< TilePos* >( result );
        const VariantList *list = v_cast< VariantList >(d);
        VariantList::const_iterator it = list->begin(); 
        pos->setI( it->toInt() ); ++it;
        pos->setJ( it->toInt() );
      }
    break;

    case Variant::NPoint:
      if( d->type == Variant::List )
      {
        Point *pos = static_cast< Point* >( result );
        const VariantList *list = v_cast< VariantList >(d);
        pos->setX( list->get( 0, 0 ).toInt() );
        pos->setY( list->get( 1, 0 ).toInt() );
      }
      else if (d->type == Variant::NPoint)
        *static_cast<PointF*>(result) = v_cast<Point>(d)->toPointF();
      else
        return false;
    break;

    case Variant::NPointF:
      if( d->type == Variant::List )
      {
        PointF *pos = static_cast< PointF* >( result );
        const VariantList *list = v_cast< VariantList >(d);
        pos->setX( list->get( 0, 0 ).toFloat() );
        pos->setY( list->get( 1, 0 ).toFloat() );
      }
      else if (d->type == Variant::NPointF)
        *static_cast<Point*>(result) = v_cast<PointF>(d)->toPoint();
      else
        return false;
    break;

    case Variant::NRectI:
      if( d->type == Variant::List )
      {
        Rect *rect = static_cast< Rect* >( result );
        const VariantList *list = v_cast< VariantList >(d);
        int x1 = list->get( 0, 0 ).toInt();
        int y1 = list->get( 1, 0 ).toInt();
        int x2 = list->get( 2, 0 ).toInt();
        int y2 = list->get( 3, 0 ).toInt();

        *rect = Rect( x1, y1, x2, y2 );
      }
      else
        return false;
    break;

    case Variant::NRectF:
      if( d->type == Variant::List )
      {
        RectF *rect = static_cast< RectF* >( result );
        const VariantList *list = v_cast< VariantList >(d);
        float x1 = list->get( 0, 0 ).toFloat();
        float y1 = list->get( 1, 0 ).toFloat();
        float x2 = list->get( 2, 0 ).toFloat();
        float y2 = list->get( 3, 0 ).toFloat();

        *rect = RectF( x1, y1, x2, y2 );
      }
      else
        return false;
    break;


    case Variant::NStringArray:
        if (d->type == Variant::List) 
        {
            StringArray *slst = static_cast<StringArray*>(result);
            const VariantList *list = v_cast< VariantList >(d);
            VariantList::const_iterator it = list->begin(); 
            for( ; it != list->end(); ++it)
                slst->push_back( it->toString() );
        } 
        else if( d->type == Variant::String ) 
        {
            StringArray *slst = static_cast<StringArray*>(result);
            slst->clear();
            slst->push_back( *v_cast<std::string>(d) );
        } 
        else 
        {
            return false;
        }
        break;
//     case Variant::Date: {
//         Date *dt = static_cast<Date *>(result);
//         if (d->type == Variant::DateTime)
//             *dt = v_cast<DateTime>(d)->date();
//         else if (d->type == Variant::String)
//             *dt = Date::fromString(*v_cast<String>(d), t::ISODate);
//         else
//             return false;
// 
//         return dt->isValid();
//     }
//     case Variant::Time: {
//         Time *t = static_cast<Time *>(result);
//         switch (d->type) {
//         case Variant::DateTime:
//             *t = v_cast<DateTime>(d)->time();
//             break;
//         case Variant::String:
//             *t = Time::fromString(*v_cast<String>(d), t::ISODate);
//             break;
//         default:
//             return false;
//         }
//         return t->isValid();
//     }
    case Variant::NDateTime: 
    {
        DateTime *dt = static_cast<DateTime *>(result);
        switch (d->type) 
        {
        case Variant::String:
          *dt = DateTime( v_cast<std::string>(d)->c_str() );
            break;
//         case Variant::Date:
//             *dt = DateTime(*v_cast<Date>(d));
//             break;
        default:
            return false;
        }
        return dt->isValid();
    }
    
    case Variant::NByteArray: 
    {
        ByteArray *ba = static_cast<ByteArray *>(result);
        switch (d->type) 
        {
        case Variant::String:
          *ba = *v_cast<std::string>(d);
        break;

        case Variant::Double:
          *ba = utils::format( 0xff, "%f", d->data.d );
        break;
        
        case Variant::Float:
            *ba = utils::format( 0xff, "%f", d->data.f );
            break;
        case Variant::Char:
            ba->clear();
            ba->push_back( d->data.c );
            break;
        case Variant::Int:
        case Variant::LongLong:
        case Variant::Short:
        case Variant::Long:
            *ba = utils::format( 0xff, "%d", (long)Variant2Number(d) );
            break;
        case Variant::UInt:
        case Variant::ULongLong:
        case Variant::Ushort:
        case Variant::Ulong:
        case Variant::Uchar:
            *ba = utils::format( 0xff, "%u", (unsigned long)Variant2UNumber(d) );
            break;
        case Variant::Bool:
          *ba = std::string( d->data.b ? "true" : "false" );
            break;
        default:
            return false;
        }
    }
    break;
    case Variant::Short:
        *static_cast<short*>(result) = static_cast<short>(ConvertToNumber(d, ok));
        return *ok;
    case Variant::Long:
        *static_cast<long *>(result) = static_cast<long>(ConvertToNumber(d, ok));
        return *ok;
    case Variant::Ushort:
        *static_cast<unsigned short*>(result) = static_cast<unsigned short>(ConvertToUnsignedNumber(d, ok));
        return *ok;
    case Variant::Ulong:
        *static_cast<unsigned int*>(result) = static_cast<unsigned int>(ConvertToUnsignedNumber(d, ok));
        return *ok;
    case Variant::Int:
        *static_cast<int*>(result) = static_cast<int>(ConvertToNumber(d, ok));
        return *ok;
    case Variant::UInt:
        *static_cast<unsigned int*>(result) = static_cast<unsigned int>(ConvertToUnsignedNumber(d, ok));
        return *ok;
    case Variant::LongLong:
        *static_cast<long long*>(result) = ConvertToNumber(d, ok);
        return *ok;
    case Variant::ULongLong: {
        *static_cast<unsigned long long*>(result) = ConvertToUnsignedNumber(d, ok);
        return *ok;
    }
    case Variant::Uchar: 
    {
        *static_cast<unsigned char*>(result) = static_cast<unsigned char>(ConvertToUnsignedNumber(d, ok));
        return *ok;
    }
    case Variant::Bool: 
    {
        bool *b = static_cast<bool *>(result);
        switch(d->type) 
        {
        case Variant::NByteArray:
          *b = ( strcmp( v_cast<ByteArray>( d )->data(), "true" ) == 0 );
        break;
        
        case Variant::String:
          *b = ( *v_cast<std::string>(d) == "true" );
        break;
        
        case Variant::Char:       
        case Variant::Double:
        case Variant::Int:
        case Variant::LongLong:
        case Variant::Short:
        case Variant::Long:
        case Variant::Float:
            *b = Variant2Number(d) != 0;
            break;
        case Variant::UInt:
        case Variant::ULongLong:
        case Variant::Uchar:
        case Variant::Ushort:
        case Variant::Ulong:
            *b = Variant2UNumber(d) != 0;
            break;
        default:
            *b = false;
            return false;
        }
        break;
    }
    case Variant::Double: 
        {
        double *f = static_cast<double*>(result);
        switch (d->type) 
        {
        case Variant::String:
          *f = utils::toFloat( v_cast<std::string>(d)->c_str() );
        break;
        
        case Variant::NByteArray:
          *f = utils::toFloat( v_cast<ByteArray>(d)->data() );
        break;
        
        case Variant::Bool:
            *f = double(d->data.b);
            break;
        case Variant::Float:
            *f = double(d->data.f);
            break;
        case Variant::LongLong:
        case Variant::Int:
        case Variant::Char:
        case Variant::Short:
        case Variant::Long:
            *f = double(Variant2Number(d));
            break;
        case Variant::UInt:
        case Variant::ULongLong:
        case Variant::Uchar:
        case Variant::Ushort:
        case Variant::Ulong:
            *f = double(Variant2UNumber(d));
        break;

        default:
            *f = 0.0;
            return false;
        }
        break;
    }
    case Variant::Float: 
    {
        float *f = static_cast<float*>(result);
        switch (d->type) {
        case Variant::String:
          *f =utils::toFloat( v_cast<std::string>(d)->c_str() );
        break;
        
        case Variant::NByteArray:
          *f = utils::toFloat( v_cast<ByteArray>(d)->data() );
        break;
        
        case Variant::Bool:
            *f = float(d->data.b);
            break;
        case Variant::Double:
            *f = float(d->data.d);
            break;
        case Variant::LongLong:
        case Variant::Int:
        case Variant::Char:
        case Variant::Short:
        case Variant::Long:
            *f = float(Variant2Number(d));
            break;
        case Variant::UInt:
        case Variant::ULongLong:
        case Variant::Uchar:
        case Variant::Ushort:
        case Variant::Ulong:
            *f = float(Variant2UNumber(d));
        break;
        default:
            *f = 0.0f;
            return false;
        }
        break;
    }
    case Variant::List:
        if (d->type == Variant::NStringArray) 
        {
            VariantList *lst = static_cast<VariantList *>(result);
            const StringArray *slist = v_cast<StringArray>(d);
            for (unsigned int i = 0; i < slist->size(); ++i)
                lst->push_back( Variant( (*slist)[i] ) );
        } 
        else if( Variant::typeToName( Variant::Type( d->type ) ) == "list<Variant>" )
        {
            *static_cast<VariantList*>(result) = *static_cast< VariantList* >(d->data.ptr);
        } 
        else 
        {
            return false;
        }
        break;
    case Variant::Map:
        if ( Variant::typeToName( Variant::Type( d->type ) ) == "map<string, Variant>" )
        {
          VariantMap* tmp = static_cast< VariantMap*>(d->data.ptr);
          VariantMap* rMap = static_cast<VariantMap*>(result);

          rMap->clear();
          foreach( it, *tmp )
          {
            rMap->insert( std::make_pair( it->first, it->second ) );
          }
        } 
        else 
        {
            return false;
        }
        break;
//     case Variant::Hash:
//         if ( _wcsicmp( Variant::typeToName(d->type), L"Hash<String, Variant>") == 0) {
//             *static_cast<Variant2Hash *>(result) =
//                 *static_cast<Hash<String, Variant> *>(d->data.shared->ptr);
//         } else {
//             return false;
//         }
//         break;
//     case Variant::NRectI:
//         if(d->type == Variant::NRectF)
//             *static_cast<Rect*>( result ) = (v_cast<RectF>(d))->As<int>();
//         else
//             return false;
//         break;
//     case Variant::NRectF:
//         if (d->type == Variant::NRectI)
//             *static_cast<RectF*>(result) = v_cast<RectI>(d)->As<float>();
//         else
//             return false;
//         break;
    default:
        return false;
    }
    return true;
}

void Variant::create(unsigned int type, const void *copy)
{
    _d.type = type;
    varHandler->construct( &_d, copy);
}

/*!
    \fn Variant::~Variant()

    Destroys the Variant and the contained object.

    Note that subclasses that reimplement clear() should reimplement
    the destructor to call clear(). This destructor calls clear(), but
    because it is the destructor, Variant::clear() is called rather
    than a subclass's clear().
*/

Variant::~Variant()
{
    if( _d.type > Short && _d.type < UserType )
        varHandler->clear( &_d );
}

/*!
  \fn Variant::Variant(const Variant &p)

    Constructs a copy of the variant, \a p, passed as the argument to
    this constructor.
*/

Variant::Variant(const Variant &p)
    : _d(p._d)
{
  if (p._d.type > Short && p._d.type < UserType) 
  {
    varHandler->construct(&_d, p.constData());
    _d.is_null = p._d.is_null;
  }
}

/*
Variant::Variant(const char *val)
{
  std::string tmp( val );
  create( String, &tmp );
}*/

Variant::Variant(Type type)
{
  create(type, 0);
}

Variant::Variant(int typeOrUserType, const void *copy)
{
  create(typeOrUserType, copy);
  _d.is_null = false;
}

/*! \internal
    flags is true if it is a pointer type
 */
Variant::Variant(int typeOrUserType, const void *copy, unsigned int flags)
{
    if (flags) { //type is a pointer type
        _d.type = typeOrUserType;
        _d.data.ptr = *reinterpret_cast<void *const*>(copy);
        _d.is_null = false;
    } else {
        create(typeOrUserType, copy);
        _d.is_null = false;
    }
}

Variant::Variant(int val)
{ _d.is_null = false; _d.type = Int; _d.data.i = val; }

Variant::Variant(unsigned int val)
{ _d.is_null = false; _d.type = UInt; _d.data.u = val; }

Variant::Variant(long long val)
{ _d.is_null = false; _d.type = LongLong; _d.data.ll = val; }

Variant::Variant(unsigned long long val)
{ _d.is_null = false; _d.type = ULongLong; _d.data.ull = val; }

Variant::Variant(bool val)
{ _d.is_null = false; _d.type = Bool; _d.data.b = val; }

Variant::Variant(double val)
{ _d.is_null = false; _d.type = Double; _d.data.d = val; }

Variant::Variant(const ByteArray& val)
{ _d.is_null = false; _d.type = NByteArray; v_construct<ByteArray>(&_d, val); }

Variant::Variant(const std::string& val)
{ _d.is_null = false; _d.type = Variant::String; v_construct<std::string>(&_d, val);  }

Variant::Variant(char val)
{ _d.is_null = false; _d.type = Variant::Char; _d.data.c = val;  }

Variant::Variant(const char* string)
{
  const std::string rstring( string );
  _d.is_null = false; _d.type = Variant::String; v_construct<std::string>(&_d, rstring);
}

Variant::Variant(const DateTime &val)
{ _d.is_null = false; _d.type = Variant::NDateTime; v_construct<DateTime>(&_d, val); }

Variant::Variant(const StringArray &strArr)
{ _d.is_null = false; _d.type = Variant::NStringArray; v_construct<StringArray>(&_d, strArr); }

Variant::Variant(const VariantList& rlist)
{ _d.is_null = false; _d.type = Variant::List; v_construct<VariantList>(&_d, rlist); }

Variant::Variant(const VariantMap& rmap)
{ _d.is_null = false; _d.type = Variant::Map; v_construct<VariantMap>(&_d, rmap); }
// Variant::Variant(const Hash<String, Variant> &hash)
// { _d.is_null = false; _d.type = Hash; v_construct<Variant2Hash>(&_d, hash); }

Variant::Variant(const Point &pt) { _d.is_null = false; _d.type = Variant::NPoint; v_construct<Point>(&_d, pt); }
Variant::Variant(const PointF &pt) { _d.is_null = false; _d.type = Variant::NPointF; v_construct<PointF>(&_d, pt); }
Variant::Variant(const TilePos &pt) { _d.is_null = false; _d.type = Variant::NTilePos; v_construct<TilePos>(&_d, pt); }
Variant::Variant(const RectF &r) { _d.is_null = false; _d.type = Variant::NRectF; v_construct<RectF>(&_d, r); }
// Variant::Variant(const core::LineF &l) { _d.is_null = false; _d.type = Variant::LineF; v_construct<core::LineF>(&_d, l); }
// Variant::Variant(const core::Line &l) { _d.is_null = false; _d.type = Variant::Line; v_construct<core::Line>(&_d, l); }
Variant::Variant(const Rect &r) { _d.is_null = false; _d.type = Variant::NRectI; v_construct<Rect>(&_d, r); }
Variant::Variant(const Size &s) { _d.is_null = false; _d.type = Variant::NSize; v_construct<Size>(&_d, s); }
Variant::Variant(const SizeF &s) { _d.is_null = false; _d.type = Variant::NSizeF; v_construct<SizeF>(&_d, s); }

//Variant::Variant(const Url &u) { _d.is_null = false; _d.type = Url; v_construct<Url>(&_d, u); }
//Variant::Variant(const Color& color) { create( Variant::Color, &color); }

Variant::Type Variant::type() const
{
    return _d.type >= UserType ? UserType : static_cast<Type>(_d.type);
}

/*!
    Returns the storage type of the value stored in the variant. For
    non-user types, this is the same as type().

    \sa type()
*/

int Variant::userType() const
{
    return _d.type;
}

/*!
    Assigns the value of the variant \a variant to this variant.
*/
Variant& Variant::operator=(const Variant &variant)
{
  if (this == &variant)
      return *this;

  clear();
  if( variant._d.type > Short && variant._d.type < UserType ) 
  {
    _d.type = variant._d.type;
    varHandler->construct(&_d, variant.constData());
    _d.is_null = variant._d.is_null;
  } 
  else
  {
    _d = variant._d;
  }

  return *this;
}

std::string Variant::typeName() const
{
    return typeToName( Type(_d.type) );
}

/*!
    Convert this variant to type Invalid and free up any resources
    used.
*/
void Variant::clear()
{
    if ( _d.type < UserType && _d.type > Short )
        varHandler->clear(&_d);

    _d.type = Invalid;
    _d.is_null = true;
}

/*!
    Converts the enum representation of the storage type, \a typ, to
    its string representation.

    Returns a null pointer if the type is Variant::Invalid or doesn't exist.
*/
std::string Variant::typeToName(Type typ)
{
    if( typ == Variant::Invalid )
        return "";
    if (typ == UserType)
        return "UserType";

    return staticTypeToName(typ);
}


/*!
    Converts the string representation of the storage type given in \a
    name, to its enum representation.

    If the string representation cannot be converted to any enum
    representation, the variant is set to \c Invalid.
*/
Variant::Type Variant::nameToType(const std::string &name)
{
    if ( name.empty() )
        return Variant::Invalid;

    if( utils::isEquale( name,  "UserType" ) )
        return UserType;

    int metaType = staticNameToType( name.c_str() );
    return metaType <= int(LastGuiType) ? Variant::Type(metaType) : UserType;
}

/*!
    \fn bool Variant::isValid() const

    Returns true if the storage type of this variant is not
    Variant::Invalid; otherwise returns false.
*/

template <typename T>
inline T Variant2ToHelper(const Variant2Impl &d, Variant::Type t,
                          const Variant2Handler* , T * = 0)
{
  if (d.type == t)
      return *v_cast<T>(&d);

  T ret;
	varHandler->convert(&d, t, &ret, 0);
  return ret;
}

/*!
    \fn StringArray Variant::toStringArray() const

    Returns the variant as a StringArray if the variant has type()
    StringArray, \l String, or \l Array of a type that can be converted
    to String; otherwise returns an empty list.

    \sa canConvert(), convert()
*/
StringArray Variant::toStringArray() const
{
    return Variant2ToHelper<StringArray>(_d, Variant::NStringArray, varHandler);
}

/*!
    Returns the variant as a String if the variant has type() \l
    String, \l Bool, \l ByteArray, \l Char, \l Date, \l DateTime, \l
    Double, \l Int, \l LongLong, \l StringList, \l Time, \l unsigned int, or
    \l ULongLong; otherwise returns an empty string.

    \sa canConvert(), convert()
*/
std::string Variant::toString() const
{
    return Variant2ToHelper<std::string>(_d, Variant::String, varHandler);
}

/*!
    Returns the variant as a Map<String, Variant> if the variant
    has type() \l Map; otherwise returns an empty map.

    \sa canConvert(), convert()
*/
VariantMap Variant::toMap() const
{
    return Variant2ToHelper<VariantMap>(_d, Variant::Map, varHandler);
}


/*!
    \fn DateTime Variant::toDateTime() const

    Returns the variant as a DateTime if the variant has type() \l
    DateTime, \l Date, or \l String; otherwise returns an invalid
    date/time.

    If the type() is \l String, an invalid date/time will be returned
    if the string cannot be parsed as a format date/time.

    \sa canConvert(), convert()
*/
DateTime Variant::toDateTime() const
{
    return Variant2ToHelper<DateTime>(_d, Variant::NDateTime, varHandler);
}

/*!
    \fn ByteArray Variant::toByteArray() const

    Returns the variant as a ByteArray if the variant has type() \l
    ByteArray or \l String (converted using String::fromCharArray());
    otherwise returns an empty byte array.

    \sa canConvert(), convert()
*/
ByteArray Variant::toByteArray() const
{
  return Variant2ToHelper<ByteArray>(_d, Variant::NByteArray, varHandler);
}

/*!
    \fn Point Variant::toPoint() const

    Returns the variant as a Point if the variant has type()
    \l Point or \l PointF; otherwise returns a null Point.

    \sa canConvert(), convert()
*/
Point Variant::toPoint() const
{
  return Variant2ToHelper<Point>(_d, Variant::NPoint, varHandler);
}

TilePos Variant::toTilePos() const
{
  return Variant2ToHelper<TilePos>( _d, Variant::NTilePos, varHandler );
}

/*!
    \fn Rect Variant::toRect() const

    Returns the variant as a Rect if the variant has type() \l Rect;
    otherwise returns an invalid Rect.

    \sa canConvert(), convert()
*/
Rect Variant::toRect() const
{
    return Variant2ToHelper<Rect>(_d, Variant::NRectI, varHandler);
}

RectF Variant::toRectf() const
{
    return Variant2ToHelper<RectF>(_d, Variant::NRectF, varHandler);
}

/*!
    \fn NSizeU Variant::toSize() const

    Returns the variant as a NSizeU if the variant has type() \l Size;
    otherwise returns an invalid NSizeU.

    \sa canConvert(), convert()
*/
Size Variant::toSize() const
{
    return Variant2ToHelper<Size>(_d, Variant::NSize, varHandler);
}

/*!
    \fn SizeF Variant::toSizeF() const

    Returns the variant as a SizeF if the variant has type() \l
    SizeF; otherwise returns an invalid SizeF.

    \sa canConvert(), convert()
*/

SizeF Variant::toSizeF() const
{
    return Variant2ToHelper<SizeF>(_d, Variant::NSizeF, varHandler);
}

/*!
    \fn LineF Variant::toLineF() const

    Returns the variant as a LineF if the variant has type() \l
    LineF; otherwise returns an invalid LineF.

    \sa canConvert(), convert()
*/
// LineF Variant::toLineF() const
// {
//     return Variant2ToHelper<core::LineF>(_d, Variant::LineF, varHandler);
// }

/*!
    \fn Line Variant::toLine() const

    Returns the variant as a Line if the variant has type() \l Line;
    otherwise returns an invalid Line.

    \sa canConvert(), convert()
*/
// core::Line Variant::toLine() const
// {
//     return Variant2ToHelper<core::Line>(_d, Variant::Line, varHandler);
// }

/*!
    \fn PointF Variant::toPointF() const

    Returns the variant as a PointF if the variant has type() \l
    Point or \l PointF; otherwise returns a null PointF.

    \sa canConvert(), convert()
*/
PointF Variant::toPointF() const
{
    return Variant2ToHelper<PointF>(_d, Variant::NPointF, varHandler);
}

// /*!
//     \fn Url Variant::toUrl() const
// 
//     Returns the variant as a Url if the variant has type()
//     \l Url; otherwise returns an invalid Url.
// 
//     \sa canConvert(), convert()
// */
// Url Variant::toUrl() const
// {
//     return Variant2ToHelper<Url>(_d, Url, handler);
// }

/*!
    \fn Char Variant::toChar() const

    Returns the variant as a char if the variant has type() \l Char,
    \l Int, or \l unsigned int; otherwise returns an invalid char.

    \sa canConvert(), convert()
*/
char Variant::toChar() const
{
    return Variant2ToHelper<char>(_d, Variant::Char, varHandler);
}

template <typename T>
inline T numVariantToHelper(const Variant2Impl &d,
                             const Variant2Handler *handler, bool *ok, const T& val)
{
    unsigned int t = Variant::nameToType( std::string( typeid(T).name() ) );
    if (ok)
        *ok = true;
    if (d.type == t)
        return val;

    T ret;
    if (!handler->convert(&d, Variant::Type(t), &ret, ok) && ok)
        *ok = false;
    return ret;
}

/*!
    Returns the variant as an int if the variant has type() \l Int,
    \l Bool, \l ByteArray, \l Char, \l Double, \l LongLong, \l
    String, \l unsigned int, or \l ULongLong; otherwise returns 0.

    If \a ok is non-null: \c{*}\a{ok} is set to true if the value could be
    converted to an int; otherwise \c{*}\a{ok} is set to false.

    \bold{Warning:} If the value is convertible to a \l LongLong but is too
    large to be represented in an int, the resulting arithmetic overflow will
    not be reflected in \a ok. A simple workaround is to use String::toInt().
    Fixing this bug has been postponed in order to avoid breaking existing code.

    \sa canConvert(), convert()
*/
int Variant::toInt(bool *ok) const
{
    return numVariantToHelper<int>(_d, varHandler, ok, _d.data.i);
}

// nrp::Color Variant::toColor() const
// {
// 	return Variant2ToHelper<nrp::Color>(_d, Variant::Color, varHandler);
// }

/*!
    Returns the variant as an unsigned int if the variant has type()
    \l unsigned int,  \l Bool, \l ByteArray, \l Char, \l Double, \l Int, \l
    LongLong, \l String, or \l ULongLong; otherwise returns 0.

    If \a ok is non-null: \c{*}\a{ok} is set to true if the value could be
    converted to an unsigned int; otherwise \c{*}\a{ok} is set to false.

    \bold{Warning:} If the value is convertible to a \l ULongLong but is too
    large to be represented in an unsigned int, the resulting arithmetic overflow will
    not be reflected in \a ok. A simple workaround is to use String::toUInt().
    Fixing this bug has been postponed to order to avoid breaking existing code.

    \sa canConvert(), convert()
*/
unsigned int Variant::toUInt(bool *ok) const
{
    return numVariantToHelper<unsigned int>(_d, varHandler, ok, _d.data.u);
}

/*!
    Returns the variant as a long long int if the variant has type()
    \l LongLong, \l Bool, \l ByteArray, \l Char, \l Double, \l Int,
    \l String, \l unsigned int, or \l ULongLong; otherwise returns 0.

    If \a ok is non-null: \c{*}\c{ok} is set to true if the value could be
    converted to an int; otherwise \c{*}\c{ok} is set to false.

    \sa canConvert(), convert()
*/
long long Variant::toLongLong(bool *ok) const
{
    return numVariantToHelper<long long>(_d, varHandler, ok, _d.data.ll);
}

/*!
    Returns the variant as as an unsigned long long int if the
    variant has type() \l ULongLong, \l Bool, \l ByteArray, \l Char,
    \l Double, \l Int, \l LongLong, \l String, or \l unsigned int; otherwise
    returns 0.

    If \a ok is non-null: \c{*}\a{ok} is set to true if the value could be
    converted to an int; otherwise \c{*}\a{ok} is set to false.

    \sa canConvert(), convert()
*/
unsigned long long Variant::toULongLong(bool *ok) const
{
    return numVariantToHelper<unsigned long long>(_d, varHandler, ok, _d.data.ull);
}

/*!
    Returns the variant as a bool if the variant has type() Bool.

    Returns true if the variant has type() \l Bool, \l Char, \l Double,
    \l Int, \l LongLong, \l unsigned int, or \l ULongLong and the value is
    non-zero, or if the variant has type \l String or \l ByteArray and
    its lower-case content is not empty, "0" or "false"; otherwise
    returns false.

    \sa canConvert(), convert()
*/
bool Variant::toBool() const
{
    if (_d.type == Bool)
        return _d.data.b;

    bool res = false;
    varHandler->convert(&_d, Bool, &res, 0);

    return res;
}

/*!
    Returns the variant as a double if the variant has type() \l
    Double, \l Variant::Float, \l Bool, \l ByteArray, \l Int, \l LongLong, \l String, \l
    unsigned int, or \l ULongLong; otherwise returns 0.0.

    If \a ok is non-null: \c{*}\a{ok} is set to true if the value could be
    converted to a double; otherwise \c{*}\a{ok} is set to false.

    \sa canConvert(), convert()
*/
double Variant::toDouble(bool *ok) const
{
    return numVariantToHelper<double>(_d, varHandler, ok, _d.data.d);
}

/*!
    Returns the variant as a float if the variant has type() \l
    Double, \l Variant::Float, \l Bool, \l ByteArray, \l Int, \l LongLong, \l String, \l
    unsigned int, or \l ULongLong; otherwise returns 0.0.

    \since 4.6

    If \a ok is non-null: \c{*}\a{ok} is set to true if the value could be
    converted to a double; otherwise \c{*}\a{ok} is set to false.

    \sa canConvert(), convert()
*/
float Variant::toFloat(bool *ok) const
{
    return numVariantToHelper<float>(_d, varHandler, ok, _d.data.f);
}

/*!
    Returns the variant as a Variant2List if the variant has type()
    \l List or \l StringList; otherwise returns an empty list.

    \sa canConvert(), convert()
*/
VariantList Variant::toList() const
{
    return Variant2ToHelper<VariantList>(_d, List, varHandler);
}

/*! \fn Variant::canCast(Type t) const
    Use canConvert() instead.
*/

/*! \fn Variant::cast(Type t)
    Use convert() instead.
*/


static const unsigned int CanConvertMatrix[Variant::LastCoreType + 1] =
{
/*Invalid*/     0,

/*Bool*/          1 << Variant::Double     | 1 << Variant::Int        | 1 << Variant::UInt
                | 1 << Variant::LongLong   | 1 << Variant::ULongLong  | 1 << Variant::NByteArray
                | 1 << Variant::String     | 1 << Variant::Char,

/*Int*/           1 << Variant::UInt       | 1 << Variant::String     | 1 << Variant::Double
                | 1 << Variant::Bool       | 1 << Variant::LongLong   | 1 << Variant::ULongLong
                | 1 << Variant::Char       | 1 << Variant::NByteArray,

/*unsigned int*/          1 << Variant::Int        | 1 << Variant::String     | 1 << Variant::Double
                | 1 << Variant::Bool       | 1 << Variant::LongLong   | 1 << Variant::ULongLong
                | 1 << Variant::Char       | 1 << Variant::NByteArray,

/*LLong*/         1 << Variant::Int        | 1 << Variant::String     | 1 << Variant::Double
                | 1 << Variant::Bool       | 1 << Variant::UInt       | 1 << Variant::ULongLong
                | 1 << Variant::Char       | 1 << Variant::NByteArray,

/*ULlong*/        1 << Variant::Int        | 1 << Variant::String     | 1 << Variant::Double
                | 1 << Variant::Bool       | 1 << Variant::UInt       | 1 << Variant::LongLong
                | 1 << Variant::Char       | 1 << Variant::NByteArray,

/*double*/        1 << Variant::Int        | 1 << Variant::String     | 1 << Variant::ULongLong
                | 1 << Variant::Bool       | 1 << Variant::UInt       | 1 << Variant::LongLong
                | 1 << Variant::NByteArray,

/*char*/         1 << Variant::Int        | 1 << Variant::UInt       | 1 << Variant::LongLong
                | 1 << Variant::ULongLong,

/*unsigned char*/ 1 << Variant::Int        | 1 << Variant::UInt       | 1 << Variant::LongLong
                | 1 << Variant::ULongLong,

/*unsigned short*/           1 << Variant::UInt       | 1 << Variant::String     | 1 << Variant::Double
                | 1 << Variant::Bool       | 1 << Variant::LongLong   | 1 << Variant::ULongLong
                | 1 << Variant::Char       | 1 << Variant::NByteArray,

/*ULong*/         1 << Variant::Int        | 1 << Variant::String     | 1 << Variant::Double
                | 1 << Variant::Bool       | 1 << Variant::UInt       | 1 << Variant::ULongLong
                | 1 << Variant::Char       | 1 << Variant::NByteArray,

/*Long*/         1 << Variant::Int        | 1 << Variant::String     | 1 << Variant::Double
                | 1 << Variant::Bool       | 1 << Variant::UInt       | 1 << Variant::ULongLong
                | 1 << Variant::Char       | 1 << Variant::NByteArray,


/*float*/        1 << Variant::Int        | 1 << Variant::String     | 1 << Variant::ULongLong
                | 1 << Variant::Bool       | 1 << Variant::UInt       | 1 << Variant::LongLong
                | 1 << Variant::NByteArray,

/*short*/           1 << Variant::UInt       | 1 << Variant::String     | 1 << Variant::Double
                | 1 << Variant::Bool       | 1 << Variant::LongLong   | 1 << Variant::ULongLong
                | 1 << Variant::Char       | 1 << Variant::NByteArray,

/*Map*/          0,

/*List*/         1 << Variant::NStringArray,

/*String*/       1 << Variant::NStringArray | 1 << Variant::NByteArray  | 1 << Variant::Int
                | 1 << Variant::UInt       | 1 << Variant::Bool       | 1 << Variant::Double
                | 1 << Variant::Date       | 1 << Variant::Time       | 1 << Variant::NDateTime
                | 1 << Variant::LongLong   | 1 << Variant::ULongLong  | 1 << Variant::Char
                | 1 << Variant::Url,

/*StringArray*/   1 << Variant::List       | 1 << Variant::String,

/*ByteArray*/    1 << Variant::String     | 1 << Variant::Int        | 1 << Variant::UInt | 1 << Variant::Bool
                | 1 << Variant::Double     | 1 << Variant::LongLong   | 1 << Variant::ULongLong,

/*BitArray*/     0,

/*Date*/         1 << Variant::String     | 1 << Variant::NDateTime,

/*Time*/         1 << Variant::String     | 1 << Variant::NDateTime,

/*DateTime*/     1 << Variant::String     | 1 << Variant::Date,

/*Url*/          1 << Variant::String,

/*RectI*/         1 << Variant::NRectF,

/*RectF*/        1 << Variant::NRectI,

/*Size*/         1 << Variant::NSizeF,

/*SizeF*/        1 << Variant::NSize,

/*Line*/         1 << Variant::LineF,

/*LineF*/        1 << Variant::Line,

/*Point*/        1 << Variant::NPointF,

/*PointF*/       1 << Variant::NPoint

};

/*!
    Returns true if the variant's type can be cast to the requested
    type, \a t. Such casting is done automatically when calling the
    toInt(), toBool(), ... methods.

    The following casts are done automatically:

    \table
    \header \o Type \o Automatically Cast To
    \row \o \l Bool \o \l Char, \l Double, \l Int, \l LongLong, \l String, \l unsigned int, \l ULongLong
    \row \o \l ByteArray \o \l Double, \l Int, \l LongLong, \l String, \l unsigned int, \l ULongLong
    \row \o \l Char \o \l Bool, \l Int, \l unsigned int, \l LongLong, \l ULongLong
    \row \o \l Color \o \l String
    \row \o \l Date \o \l DateTime, \l String
    \row \o \l DateTime \o \l Date, \l String, \l Time
    \row \o \l Double \o \l Bool, \l Int, \l LongLong, \l String, \l unsigned int, \l ULongLong
    \row \o \l Font \o \l String
    \row \o \l Int \o \l Bool, \l Char, \l Double, \l LongLong, \l String, \l unsigned int, \l ULongLong
    \row \o \l KeySequence \o \l Int, \l String
    \row \o \l List \o \l StringList (if the list's items can be converted to strings)
    \row \o \l LongLong \o \l Bool, \l ByteArray, \l Char, \l Double, \l Int, \l String, \l unsigned int, \l ULongLong
    \row \o \l Point \o PointF
    \row \o \l Rect \o RectF
    \row \o \l String \o \l Bool, \l ByteArray, \l Char, \l Color, \l Date, \l DateTime, \l Double,
                         \l Font, \l Int, \l KeySequence, \l LongLong, \l StringList, \l Time, \l unsigned int,
                         \l ULongLong
    \row \o \l StringList \o \l List, \l String (if the list contains exactly one item)
    \row \o \l Time \o \l String
    \row \o \l unsigned int \o \l Bool, \l Char, \l Double, \l Int, \l LongLong, \l String, \l ULongLong
    \row \o \l ULongLong \o \l Bool, \l Char, \l Double, \l Int, \l LongLong, \l String, \l unsigned int
    \endtable

    \sa convert()
*/
bool Variant::canConvert(Type t) const
{
    //we can treat floats as double
    //the reason for not doing it the "proper" way is that Variant::Float's value is 135,
    //which can't be handled by canConvertMatrix
    //In addition Variant::Type doesn't have a Float value, so we're using Variant::Float
    const unsigned int currentType = ((_d.type == Variant::Float) ? Variant::Double : _d.type);
    if (static_cast<unsigned int>(t) == static_cast<unsigned int>(Variant::Float)) t = Variant::Double;

    if (currentType == static_cast<unsigned int>(t))
        return true;

    if (currentType > Variant::LastCoreType || t > Variant::LastCoreType) {
        switch (static_cast<unsigned int>(t)) {
        case Variant::Int:
            return currentType == Variant::KeySequence
                   || currentType == Variant::Ulong
                   || currentType == Variant::Long
                   || currentType == Variant::Ushort
                   || currentType == Variant::Uchar
                   || currentType == Variant::Char
                   || currentType == Variant::Short;
        case Variant::Image:
          return currentType == Variant::Pixmap || currentType == Variant::Bitmap;
        case Variant::NTilePos:
          return currentType == Variant::List;
        case Variant::Pixmap:
          return currentType == Variant::Image || currentType == Variant::Bitmap
                              || currentType == Variant::Brush;
        case Variant::Bitmap:
          return currentType == Variant::Pixmap || currentType == Variant::Image;
        case Variant::NByteArray:
          return currentType == Variant::Color;
        case Variant::String:
          return currentType == Variant::KeySequence || currentType == Variant::Font
                              || currentType == Variant::Color;
        case Variant::KeySequence:
          return currentType == Variant::String || currentType == Variant::Int;
        case Variant::Font:
          return currentType == Variant::String;
        case Variant::Color:
          return currentType == Variant::String || currentType == Variant::NByteArray
                              || currentType == Variant::Brush;
        case Variant::Brush:
          return currentType == Variant::Color || currentType == Variant::Pixmap;
        case Variant::Long:
        case Variant::Char:
        case Variant::Uchar:
        case Variant::Ulong:
        case Variant::Short:
        case Variant::Ushort:
          return CanConvertMatrix[Variant::Int] & (1 << currentType) || currentType == Variant::Int;
        default:
            return false;
        }
    }

    if(t == String && currentType == NStringArray)
        return v_cast<StringArray>(&_d)->size() == 1;
    else
        return ((CanConvertMatrix[t] & (1 << currentType)) != 0);
}

/*!
    Casts the variant to the requested type, \a t. If the cast cannot be
    done, the variant is cleared. Returns true if the current type of
    the variant was successfully cast; otherwise returns false.

    \warning For historical reasons, converting a null Variant results
    in a null value of the desired type (e.g., an empty string for
    string) and a result of false.

    \sa canConvert(), clear()
*/

bool Variant::convert(Type t)
{
    if (_d.type == static_cast<unsigned int>(t))
        return true;

    Variant oldValue = *this;

    clear();
    if (!oldValue.canConvert(t))
        return false;

    create(t, 0);
    if (oldValue.isNull())
        return false;

    bool isOk = true;
    if (!varHandler->convert(&oldValue._d, t, data(), &isOk))
        isOk = false;
    _d.is_null = !isOk;
    return isOk;
}

static bool isNumericType(unsigned int tp)
{
    return (tp >= Variant::Bool && tp <= Variant::Double)
           || (tp >= Variant::Long && tp <= Variant::Float);
}

static bool isFloatingPoint(unsigned int tp)
{
    return tp == Variant::Double || tp == Variant::Float;
}

/*! \internal
 */
bool Variant::cmp(const Variant &v) const
{
    Variant v2 = v;
    if (_d.type != v2._d.type) 
    {
        if (isNumericType(_d.type) && isNumericType(v._d.type)) 
        {
            if (isFloatingPoint(_d.type) || isFloatingPoint(v._d.type))
                return math::isEqual( toFloat(), v.toFloat() );
            else
                return toLongLong() == v.toLongLong();
        }
        if (!v2.canConvert(Type(_d.type)) || !v2.convert(Type(_d.type)))
            return false;
    }
    return varHandler->compare(&_d, &v2._d);
}

/*! \internal
 */

const void *Variant::constData() const
{
    return reinterpret_cast<const void *>(_d.data.ptr);
}

/*!
    \fn const void* Variant::data() const

    \internal
*/

/*! \internal */
void* Variant::data()
{
    return const_cast<void *>(constData());
}

/*!
  Returns true if this is a NULL variant, false otherwise.
*/
bool Variant::isNull() const
{
    return varHandler->isNull(&_d);
}

std::ostream & operator << (std::ostream& os, const Variant::Type p)
{
    os << "Variant::" << Variant::typeToName(p);
    return os;
}

Variant::Variant( float f )
{
  _d.is_null = false; _d.type = Float; _d.data.f = f;
}

bool Variant::isValid() const
{
    return _d.type != Variant::Invalid;
}

Variant2Handler::Variant2Handler()
{
	construct = constructNewVariant;
	compare = 0;
	canConvert = 0;
	convert = convertVariantType2Type;
	clear = clearVariant;
  isNull = checkVariantNull;
}


StringArray& operator<<(StringArray &strlist, const VariantList &vars)
{
  for( VariantList::const_iterator it=vars.begin(); it != vars.end(); ++it )
  {
    strlist.push_back( (*it).toString() );
  }

  return strlist;
}
