#ifndef _CAESARIA_WIDGET_HELPER_H_INCLUDE_
#define _CAESARIA_WIDGET_HELPER_H_INCLUDE_

#include "widget.hpp"
#include "core/signals.hpp"
#include "core/requirements.hpp"
#include "core/logger.hpp"

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

template<class T>
T getWidgetFromUI( const std::string& elmName, const char* filename, int line, const Widget* parent )
{
  T element = findChildA<T>( elmName, true, parent );
  if( !element )
  {
    Logger::warning( "Cannot find {} in {}:{}", elmName, filename, line );
  }
  return element;
}


}//end namespace gui

#define GET_WIDGET_FROM_UI( element ) element = getWidgetFromUI<__typeof__(element)>( TEXT(element), __FILE__, __LINE__, this );
#define GET_DWIDGET_FROM_UI( d, element ) (d)->element = getWidgetFromUI<__typeof__( (d)->element)>( TEXT(element), __FILE__, __LINE__, this );

#define INIT_WIDGET_FROM_UI( type, element ) type element; GET_WIDGET_FROM_UI( element );
#define LINK_WIDGET_ACTION( type, element, signal, receiver, slot) { type element; GET_WIDGET_FROM_UI( element ); CONNECT( element, signal, receiver, slot ); }
#define LINK_WIDGET_LOCAL_ACTION( type, element, signal, slot) { type element; GET_WIDGET_FROM_UI( element ); CONNECT_LOCAL( element, signal, slot ); }

#endif //_CAESARIA_WIDGET_HELPER_H_INCLUDE_
