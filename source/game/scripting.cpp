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

#include "scripting.hpp"
#include "picoc/mujs.h"
#include <GameApp>
#include <GameVfs>
#include <GameGui>
#include <GameLogger>

using namespace gui;

namespace game
{

namespace internal
{
Game* game = nullptr;
js_State *J = nullptr;

inline std::string to(js_State *J, int n, std::string) { return js_tostring(J, n); }
inline void push(js_State* J, int value) { js_pushnumber(J,value); }

inline Rect to(js_State *J, int n, Rect)
{
  return Rect( js_toint32(J, n), js_toint32(J, n+1),
               js_toint32(J, n+2), js_toint32(J, n+3) ); }
}

void engineLog(js_State *J)
{
  const char *text = js_tostring(J, 1);
  Logger::warning( text );
  js_pushundefined(J);
}

Scripting& Scripting::instance()
{
  static Scripting inst;
  return inst;
}

void Scripting::doFile(const std::string& path)
{
  vfs::Path rpath( path );
  if( !rpath.exist() )
  {
    Logger::warning( "WARNING !!! Cant find script at {}", rpath.toString() );
    return;
  }

  js_dofile( internal::J, rpath.toCString() );
}

#define DEFINE_OBJECT_FUNCTION_0(name,rettype,funcname) rettype name##_##funcname(js_State *J) \
                                { \
                                  name* parent = (name*)js_touserdata(J, 0, "userdata"); \
                                  if( parent ) parent->funcname(); \
                                  js_pushundefined(J); \
                                }

//#define DEFINE_OBJECT_CALLBACK_0(name,callback) void name##_##callback()

void ButtonClicked(Widget* widget)
{
  if(widget)
  {
    int index = widget->getProperty( "js_callback");
    js_pcall(internal::J,index);
  }
}

void PushButton_setCallback(js_State *J)
{
  PushButton* parent = (PushButton*)js_touserdata(J, 0, "userdata");
  if (parent && js_iscallable(J,1))
  {
    int index = js_gettop(J);
    parent->onClickedEx().connect( &ButtonClicked );
    parent->addProperty( "js_callback", index );
  }

  js_pushundefined(J);
}

void PushButton_checkCallback(js_State *J)
{
  PushButton* parent = (PushButton*)js_touserdata(J, 0, "userdata");
  if (parent)
  {
    ButtonClicked(parent);
  }

  js_pushundefined(J);
}


#define DEFINE_OBJECT_GETTER(name,funcname) void name##_##funcname(js_State* J) { \
                              name* parent = (name*)js_touserdata(J, 0, "userdata"); \
                              if (parent) internal::push(J,parent->funcname()); \
                              js_pushundefined(J); \
                            }

#define DEFINE_OBJECT_FUNCTION_1(name,rettype,funcname,paramType) rettype name##_##funcname(js_State *J) { \
                                  name* parent = (name*)js_touserdata(J, 0, "userdata"); \
                                  paramType paramValue = internal::to( J, 1, paramType() ); \
                                  if( parent ) parent->funcname( paramValue ); \
                                  js_pushundefined(J); \
                                }

#define SCRIPT_OBJECT_BEGIN(name) js_getglobal(internal::J, "Object"); \
                                  js_getproperty(internal::J, -1, "prototype"); \
                                  js_newuserdata(internal::J, "userdata", nullptr, nullptr);

#define SCRIPT_OBJECT_FUNCTION(name,funcname,params) js_newcfunction(internal::J, name##_##funcname, TEXT(funcname), params); \
                                  js_defproperty(internal::J, -2, TEXT(funcname), JS_DONTENUM);


#define SCRIPT_OBJECT_CONSTRUCTOR(name) js_newcconstructor(internal::J, constructor_##name, constructor_##name, TEXT(name), 6); \
                                        js_defglobal(internal::J, TEXT(name), JS_DONTENUM);

#define SCRIPT_OBJECT_END(name)

#define DEFINE_WIDGET_CONSTRUCTOR(name) void constructor_##name(js_State *J) { \
  Widget* parent = nullptr; \
  if( js_isuserdata( J, 1, "userdata" ) ) \
    parent = (Widget*)js_touserdata(J, 1, "userdata"); \
  if( parent == 0 ) \
    parent = internal::game->gui()->rootWidget(); \
  auto* widget = internal::game->gui()->createWidget( TEXT(name), parent ); \
  js_currentfunction(J); \
  js_getproperty(J, -1, "prototype"); \
  js_newuserdata(J, "userdata", widget, nullptr); \
}

#include "scripting/window.implementation"
#include "scripting/button.implementation"

void Scripting::registerFunctions( Game& game )
{
  internal::game = &game;
#define DEF_GLOBAL_OBJECT(name) js_newobject(internal::J);
#define REGISTER_FUNCTION(func,name,params) js_newcfunction(internal::J, func, name, params); js_setproperty( internal::J, -2, name);
#define REGISTER_GLOBAL_OBJECT(name) js_setglobal(internal::J, #name);

DEF_GLOBAL_OBJECT(engine)
  REGISTER_FUNCTION(engineLog,"log",1);
REGISTER_GLOBAL_OBJECT(engine)

#include "scripting/window.interface"
#include "scripting/button.interface"
}

Scripting::Scripting()
{
  internal::J = js_newstate(NULL, NULL, JS_STRICT);
}


} //end namespace advisor
