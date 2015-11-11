#ifndef _CAESARIA_WIDGET_HELPER_H_INCLUDE_
#define _CAESARIA_WIDGET_HELPER_H_INCLUDE_

#include "widget.hpp"

namespace gui
{

template< class T >
inline T findChildA( const std::string& internalName, bool recursiveFind, const Widget* p )
{
  for( auto child : p->children() )
  {
    if( child->internalName() == internalName )
      return safety_cast< T >( child );

    if( recursiveFind )
    {
      T chElm = findChildA< T >( internalName, recursiveFind, child );
      if( chElm )
         return chElm;
    }
  }
  return nullptr;
}

template< class T >
inline T findChildA( bool recursiveFind, const Widget* p )
{
  for( auto child : p->children() )
  {
    T ret = safety_cast< T >( child );
    if( ret != 0 )
      return ret;

    if( recursiveFind )
    {
      T chElm = findChildA< T >( recursiveFind, child );
      if( chElm )
         return chElm;
    }
  }

  return nullptr;
}

}//end namespace gui

#define GET_WIDGET_FROM_UI( element ) element = findChildA<__typeof__( element )>( #element, true, this ); \
                                       if( 0 == element ) { Logger::warning( "Cannot fint {} in {}:{}", #element, __FILE__, __LINE__ ); }

#define GET_DWIDGET_FROM_UI( d, element ) (d)->element = findChildA<__typeof__( (d)->element )>( #element, true, this ); \
                                          if( 0 == (d)->element ) { Logger::warning( "Cannot fint {} in {}:{}", #element, __FILE__, __LINE__ ); }

#define INIT_WIDGET_FROM_UI( type, element ) type element; GET_WIDGET_FROM_UI( element );


#endif //_CAESARIA_WIDGET_HELPER_H_INCLUDE_
