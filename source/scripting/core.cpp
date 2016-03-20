//This file is part of CaesarIA.
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

#include "core.hpp"
#include "picoc/mujs.h"

#include <GameApp>
#include <GameObjects>
#include <GameVfs>
#include <GameGui>
#include <GameLogger>
#include <GameScene>
#include <GameCore>
#include <GameGfx>
#include <GameCity>
#include <GameWorld>

#include "sound/engine.hpp"
#include "scripting/session.hpp"

using namespace gui;
using namespace gui::dialog;
using namespace city;
using namespace world;
using namespace gfx;
using namespace vfs;

namespace script
{

namespace internal
{
Game* game = nullptr;
std::set<std::string> files;
vfs::FileChangeObserver DirectoryChangeObserver;
Session* session = nullptr;
js_State *J = nullptr;

inline std::string to(js_State *J, int n, std::string) { return js_tostring(J, n); }
inline int32_t to(js_State *J, int n, int32_t) { return js_toint32(J, n); }

Variant to(js_State *J, int n, Variant)
{
  if(js_isboolean(J,n))
    return Variant(js_toboolean(J,n));

  if(js_isnull(J,n))
    return Variant();

  if(js_isnumber(J,n))
    return Variant(js_tonumber(J,n));

  if(js_isstring(J,n))
    return Variant(std::string(js_tostring(J,n)));

  Logger::warning("!!! Cant convert jValue to Variant");
  return Variant();
}

bool tryPCall(js_State *J, int params)
{
  try
  {
    int error = js_pcall(internal::J, params);
    if (error)
    {
      std::string str = js_tostring(internal::J, -1);
      Logger::warning(str);
    }
    js_pop(internal::J, -1);
    return false;
  }
  catch (...)
  {
    return true;
  }
  js_pop(internal::J, -1);
}

void push(js_State* J,const Size& size)
{
  js_newobject(J);
  js_pushnumber(J, size.width());
  js_setproperty(J, -2, "w");
  js_pushnumber(J, size.height());
  js_setproperty(J, -2, "h");
}

void push(js_State* J, int32_t value) { js_pushnumber(J,value); }
void push(js_State* J, const std::string& p) { js_pushstring(J,p.c_str()); }

int push(js_State* J,const Variant& param)
{
  switch( param.type() )
  {
  case Variant::Bool:
    js_pushboolean(J, param.toBool() );
    return 0;
  break;

  case Variant::Int:
  case Variant::UInt:
  case Variant::LongLong:
  case Variant::ULongLong:
  case Variant::Double:
  case Variant::Ushort:
  case Variant::Ulong:
  case Variant::Long:
  case Variant::Float:
  case Variant::Uchar:
  case Variant::Short:
    js_pushnumber(J, param.toDouble());
    return 0;
  break;

  case Variant::Date:
  case Variant::Time:
  case Variant::NDateTime:
    js_pushnumber(J, param.toDateTime().hashdate());
    return 0;
  break;

  case Variant::NStringArray:
  {
    auto items = param.toStringArray();
    js_newarray(J);
    for (uint32_t i = 0; i < items.size(); i++)
    {
      js_pushstring(J, items[i].c_str());
      js_setindex(J, -2, i);
    }
    return 0;
  }
  break;

  case Variant::Char:
  case Variant::String:
    js_pushstring(J, param.toString().c_str());
    return 0;
  break;

  default:
    js_pushnull(J);
  break;
  }
  return 1;
}

void pushud(js_State* J, const std::string& name, void* v)
{
  js_newobject(J);
  js_getglobal(J, name.c_str());
  js_getproperty( J, -1, "prototype");
  js_newuserdata(J, "userdata", v, nullptr);
}

void push(js_State *J, const StringArray& items)
{
  js_newarray(J);
  for (uint32_t i = 0; i<items.size(); i++)
  {
    js_pushstring(J, items[i].c_str());
    js_setindex(J, -2, i);
  }
}

void push(js_State *J, const VariantMap& items)
{
  js_newobject(J);
  for (const auto& item : items)
  {
    push(J, item.second);
    js_setproperty(J, -2, item.first.c_str());
  }
}

void push(js_State* J, const DateTime& t)
{
  auto pd = new DateTime(t);
  pushud(J, TEXT(DateTime), pd);
}

void push(js_State* J, const Path& p)
{
  auto pd = new Path(p);
  pushud(J, TEXT(Path), pd);
}

#define PUSH_SAVEDDATA(type) void push(js_State* J, const type& p) { push(J, p.save()); }
#define PUSH_USERDATA(type) void push(js_State* J, type* p) { pushud(J, #type, p); }

PUSH_SAVEDDATA(States)
PUSH_USERDATA(ContextMenuItem)
PUSH_USERDATA(PlayerCity)
PUSH_USERDATA(Player)
PUSH_USERDATA(Emperor)
PUSH_USERDATA(Empire)

inline DateTime to(js_State *J, int n, DateTime)
{
  if (js_isuserdata(J, 1, "userdata"))
  {
    DateTime* dt = (DateTime*)js_touserdata(J, 1, "userdata");
    return *dt;
  }

  return DateTime();
}

inline Picture to(js_State *J, int n, Picture)
{
  if (js_isuserdata(J, 1, "userdata"))
  {
    Picture* pic = (Picture*)js_touserdata(J, 1, "userdata");
    return *pic;
  }

  return Picture();
}

inline StringArray to(js_State *J, int n, StringArray)
{
  if (!js_isarray(J, 1))
  {
    Logger::warning("!!! Object is not an string array");
    return StringArray();
  }

  int length = js_getlength(J, n);
  StringArray ret;
  for (int i = 0; i < length; ++i)
  {
    js_getindex(J, n, i);
    std::string tmp = js_tostring(J, -1);
    js_pop(J, 1);
    ret.push_back(tmp);
  }

  return ret;
}

inline bool to(js_State *J, int n, bool) { return js_toboolean(J, n)>0; }
inline Size to(js_State *J, int n, Size) { return Size( js_toint32(J, n), js_toint32(J, n+1) ); }
inline Point to(js_State *J, int n, Point) { return Point( js_toint32(J, n), js_toint32(J, n+1) );}
inline PointF to(js_State *J, int n, PointF) { return PointF( (float)js_tonumber(J, n), (float)js_tonumber(J, n+1) );}

inline Rect to(js_State *J, int n, Rect)
{
  return Rect( js_toint32(J, n), js_toint32(J, n+1),
               js_toint32(J, n+2), js_toint32(J, n+3) );
}

} //end namespace internal

void enginePanic(js_State *J)
{
  Logger::warning("JSE !!! Uncaught exception: %s\n", js_tostring(J, -1));
}

void engineLog(js_State *J)
{
  const char *text = js_tostring(J, 1);
  Logger::warning( text );
  js_pushundefined(J);
}

void engineLoadArchive(js_State* J)
{
  Path archivePath = js_tostring(J, 1);
  bool ignoreCase = js_toboolean(J, 2)>0;

  Directory dir = archivePath.directory();
  Path arcPath = dir.find(archivePath.baseName(), ignoreCase ? Path::ignoreCase : Path::nativeCase);

  ArchivePtr archive = FileSystem::instance().mountArchive(arcPath);
  if (archive.isNull())
  {
    Logger::warning("!!! JS:LoadArchive can't load file " + archivePath.toString());
    return;
  }

  ResourceLoader rc;
  NFile atlasInfo = archive->createAndOpenFile("info");
  if (atlasInfo.isOpen()) { rc.loadAtlases(atlasInfo, false); }
  else { rc.loadFiles(archive); }
}

void engineReloadFile(vfs::Path path)
{
  if (internal::files.count( path.toString()))
    Core::loadModule(path.toString());
}

void engineSetVolume(js_State *J)
{
  int type = js_toint32(J, 1);
  int value = js_toint32(J, 2);
  audio::Engine::instance().setVolume((audio::SoundType)type, value);
}

void engineLoadModule(js_State *J)
{
  vfs::Path scriptName = js_tostring(J, 1);
  internal::files.insert(scriptName.toString());
  Core::loadModule(scriptName.toString());
}

void engineTranslate(js_State *J)
{
  std::string text = js_tostring(J, 1);
  text = Locale::translate(text);
  js_pushstring(J,text.c_str());
}

void engineGetOption(js_State *J)
{
  std::string name = js_tostring(J, 1);
  Variant value = game::Settings::get(name);
  int error = internal::push(J, value);
  if (error)
    Logger::warning( "!!! Undefined value for js.pcall engineGetOption when find " + name );
}

void engineSetOption(js_State *J)
{
  std::string name = js_tostring(J, 1);
  if (js_isboolean(J,2) )
    game::Settings::set(name, js_toboolean(J,2));
  else if (js_isnumber(J,2))
    game::Settings::set(name, js_tonumber(J,2));
  else if (js_isstring(J,2))
    game::Settings::set(name, std::string(js_tostring(J,2)));
  else
    Logger::warning( "!!! Undefined value for js.pcall engineSetOption when set " + name );

  game::Settings::save();
}

void reg_object_callback(const std::string& name, const std::string& funcname, js_CFunction f, int params)
{
  js_newcfunction(internal::J, f, funcname.c_str(), params);
  Logger::debug( "script-if://" + name + "_set_" + funcname + "->" + funcname);
  js_defproperty(internal::J, -2, funcname.c_str(), JS_DONTENUM);
}

void reg_object_function(const std::string& name, const std::string& funcname, js_CFunction f, int params)
{
  js_newcfunction(internal::J, f, funcname.c_str(), params);
  js_defproperty(internal::J, -2, funcname.c_str(), JS_DONTENUM);
}

void script_object_begin(const std::string& name)
{
  Logger::debug( "script-if://object.begin={} stack={}", name, js_gettop(internal::J) );
  js_getglobal(internal::J, "Object");
  js_getproperty(internal::J, -1, "prototype");
  js_newuserdata(internal::J, "userdata", nullptr, nullptr);
}

void script_object_end(const std::string& name )
{
  js_pop(internal::J,1);
  Logger::debug( "script-if://object.end={} stack={} ", name, js_gettop(internal::J) );
}

void reg_object_constructor(const std::string& name, js_CFunction f)
{
  js_newcconstructor(internal::J, f, f, name.c_str(), 1);
  js_defglobal(internal::J, name.c_str(), JS_DONTENUM);
}

Core& Core::instance()
{
  static Core inst;
  return inst;
}

void Core::loadModule(const std::string& path)
{
  vfs::Path rpath(path);
  if (!rpath.exist())
  {
    Logger::warning("!!! Cant find script at {}", rpath.toString());
    return;
  }

  int error = js_ploadfile(internal::J, rpath.toCString());
  if (error)
  {
    std::string str = js_tostring(internal::J,-1);
    Logger::warning( str );
  }

  js_getglobal(internal::J,"");
  internal::tryPCall(internal::J, 0);
}

void Core::execFunction(const std::string& funcname)
{
  execFunction(funcname, VariantList());
}

void Core::execFunction(const std::string& funcname, const VariantList& params)
{
  if (internal::J == nullptr)
    return;

  int savetop = js_gettop(internal::J);
  js_getglobal(internal::J, funcname.c_str());
  js_pushnull(internal::J);
  for (const auto& param : params)
  {
    int error = internal::push(internal::J,param);
    if (error)
      Logger::warning("!!! Undefined value for js.pcall " + funcname);
  }

  bool error = internal::tryPCall(internal::J,params.size());
  if (error)
    Logger::fatal("Fatal error on call function " + funcname);
  js_pop(internal::J,2);
  if( savetop - js_gettop(internal::J) != 0 )
  {
    Logger::warning( "STACK grow for {} in {}", js_gettop(internal::J), funcname );
  }
}

template<typename T>
void desctructor_jsobject(js_State *J, void* p)
{
  T* ptr = (T*)p;
  delete ptr;
}

template<typename T>
void constructor_jsobject(js_State *J)
{
  js_currentfunction(J);
  js_getproperty(J, -1, "prototype");
  js_newuserdata(J, "userdata", new T(), &desctructor_jsobject<T>);
}

template<typename T>
void widget_handle_callback_0(Widget* widget,const std::string& callback, const std::string& className)
{
  try
  {
    auto* ptrCheck = safety_cast<Widget*>(widget);
    if(!ptrCheck)
    {
      Logger::warning( "!!! Callback " + className + ":" + callback + " called not for widget");
      return;
    }

    if (widget)
    {
      std::string index = widget->getProperty(callback);
      js_getregistry(internal::J,index.c_str());
      js_pushnull(internal::J);
      int error = internal::tryPCall(internal::J,0);
      if (error)
        Logger::warning("Fatal error on callback " + className + ":" + callback);
      js_pop(internal::J,2); //pop func+param from stack
    }
    else
      Logger::warning(className + "_handle_" + callback + " widget is null");
  }
  catch(...)
  {
  }
}

template<typename T,typename P1>
void widget_handle_callback_1(Widget* widget, P1 value, const std::string& callback, const std::string& className)
{
  try
  {
    if(widget)
    {
      std::string index = widget->getProperty( callback );
      js_getregistry(internal::J,index.c_str());
      js_pushnull(internal::J);
      internal::push(internal::J, value);
      internal::tryPCall(internal::J,1);
      js_pop(internal::J,2);
    }
    else
      Logger::warning(className + "_handle_" + callback + " widget is null");
  }
  catch(...)
  {}
}

template<typename T>
void widget_set_callback_0(js_State *J,Signal1<Widget*>& (T::*f)(),
                           void (*handler)(Widget*),
                           const std::string& callback, const std::string& className)
{
  T* parent = (T*)js_touserdata(J, 0, "userdata");
  if (parent && js_iscallable(J,1))
  {
    js_copy(J,1);
    std::string index = js_ref(J);
    (parent->*f)().connect(handler);
    parent->addProperty(callback, Variant(index));
  }
  else
    Logger::warning( className + "_set_" + callback + " parent is null" );

  js_pushundefined(J);
}


template<typename T>
void object_call_func_0(js_State *J, void (T::*f)())
{
  try
  {
    T* parent = (T*)js_touserdata(J, 0, "userdata");
    if (parent)
      (parent->*f)();
    js_pushundefined(J);
  }
  catch(...)
  {}
}

template<typename T, typename Rtype>
void object_call_getter_0(js_State *J, Rtype (T::*f)() const)
{
  try
  {
    T* parent = (T*)js_touserdata(J, 0, "userdata");
    if (parent)
    {
      Rtype value = (parent->*f)();
      internal::push(J,value);
    }
    else
      js_pushundefined(J);
  }
  catch(...)
  {
    //something bad happens
  }
}

template<typename T,typename Rtype, typename P1Type>
void object_call_getter_1(js_State *J, Rtype (T::*f)(P1Type) const, P1Type def)
{
  T* parent = (T*)js_touserdata(J, 0, "userdata");
  if (parent)
  {
    auto paramValue1 = internal::to(J, 1, def);
    Rtype value = (parent->*f)(paramValue1);
    internal::push(J,value);
  }
  else
    js_pushundefined(J);
}

template<typename T,typename Rtype, typename P1Type>
void object_call_getter_1(js_State *J, Rtype (T::*f)(P1Type),P1Type def)
{
  T* parent = (T*)js_touserdata(J, 0, "userdata");
  if (parent)
  {
    auto paramValue1 = internal::to(J, 1, def);
    Rtype value = (parent->*f)(paramValue1);
    internal::push(J,value);
  }
  else
    js_pushundefined(J);
}

void reg_widget_constructor(js_State *J, const std::string& name)
{
  Widget* widget = nullptr;
  if (js_isstring(J,1))
  {
    std::string name = js_tostring(J, 1);
    widget = internal::game->gui()->rootWidget()->findChild(name, true);

    if (widget == nullptr)
      Logger::warning("!!! Cant found widget with name " + name);
  }
  else
  {
    Widget* parent = nullptr;
    if (js_isuserdata( J, 1, "userdata" ))
      parent = (Widget*)js_touserdata(J, 1, "userdata");
    if (parent == nullptr)
      parent = internal::game->gui()->rootWidget();

    widget = internal::game->gui()->createWidget(name, parent);
  }

  js_currentfunction(J);
  js_getproperty(J, -1, "prototype");
  js_newuserdata(J, "userdata", widget, nullptr);
}


#define DEFINE_OBJECT_DESTRUCTOR(name) void destructor_##name(js_State *J, void* p) { desctructor_jsobject<name>(J,p); }
#define DEFINE_OBJECT_CONSTRUCTOR(name) void constructor_##name(js_State *J) { constructor_jsobject<name>(J); }
#define DEFINE_OBJECT_FUNCTION_0(name,funcname) void name##_##funcname(js_State *J) { auto p=&name::funcname; object_call_func_0<name>(J,p); }

#define DEFINE_WIDGET_CALLBACK_0(name,callback) void name##_handle_##callback(Widget* widget) { widget_handle_callback_0<name>(widget, "js_"#callback, #name); } \
                                                void name##_set_##callback(js_State *J) { \
                                                      auto handler=&name##_handle_##callback; \
                                                      auto widgetCallback=&name::callback; \
                                                      widget_set_callback_0<name>(J, widgetCallback, handler, "js_"#callback, #name);  \
                                                    }

#define DEFINE_WIDGET_CALLBACK_1(name,callback,type) void name##_handle_##callback(Widget* widget,type value) { widget_handle_callback_1<name,type>(widget, value, "js_"#callback, #name); } \
                                                void name##_set_##callback(js_State *J) { \
                                                  name* parent = (name*)js_touserdata(J, 0, "userdata"); \
                                                  if (parent && js_iscallable(J,1)) { \
                                                    js_copy(J,1); \
                                                    std::string index = js_ref(J); \
                                                    parent->callback().connect( &name##_handle_##callback ); \
                                                    parent->addProperty( "js_"#callback, Variant(index) ); \
                                                  } \
                                                  else Logger::warning( #name"_set_"#callback" parent is null" ); \
                                                  js_pushundefined(J); \
                                                }


#define DEFINE_OBJECT_GETTER_0(name,rtype,funcname) void name##_##funcname(js_State* J) { rtype (name::*p)() const=&name::funcname; object_call_getter_0<name,rtype>(J,p); }
#define DEFINE_OBJECT_GETTER_1(name,rtype,funcname,p1type,def) void name##_##funcname(js_State* J) { auto p=&name::funcname; object_call_getter_1<name,rtype,p1type>(J,p,def); }

#define DEFINE_OBJECT_GETTER_3(name,rtype,funcname,p1type,p2type,p3type) void name##_##funcname(js_State* J) { \
                                                  name* object = (name*)js_touserdata(J, 0, "userdata"); \
                                                  if (object) { \
                                                      auto paramValue1 = internal::to(J, 1, p1type()); \
                                                      auto paramValue2 = internal::to(J, 2, p2type()); \
                                                      auto paramValue3 = internal::to(J, 3, p3type()); \
                                                      rtype value = object->funcname(paramValue1,paramValue2,paramValue3); \
                                                      internal::push(J,value); \
                                                  } else { \
                                                    js_pushundefined(J); \
                                                  } \
                                                }

#define DEFINE_VANILLA_CONSTRUCTOR(type,func) void constructor_##type(js_State *J) { \
                                                  js_currentfunction(J); \
                                                  js_getproperty(J, -1, "prototype"); \
                                                  js_newuserdata(J, "userdata", func, nullptr); \
                                              }

#define DEFINE_OBJECT_GETTER_2(name,funcname,paramType1,paramType2) void name##_##funcname(js_State* J) { \
  name* parent = (name*)js_touserdata(J, 0, "userdata"); \
  paramType1 paramValue1 = internal::to( J, 1, paramType1() ); \
  paramType2 paramValue2 = internal::to( J, 2, paramType2() ); \
  if (parent) internal::push(J,parent->funcname(paramValue1,paramValue2)); \
  else js_pushundefined(J); \
}

#define DEFINE_OBJECT_FUNCTION_1(name,funcname,paramType) void name##_##funcname(js_State *J) { \
                                  name* parent = (name*)js_touserdata(J, 0, "userdata"); \
                                  paramType paramValue = internal::to(J, 1, paramType()); \
                                  if( parent ) parent->funcname( paramValue ); \
                                  js_pushundefined(J); \
                                }

#define DEFINE_OBJECT_OVERRIDE_FUNCTION_1(name,funcname,ov,paramType) void name##_##funcname##_##ov(js_State *J) { \
                                  name* parent = (name*)js_touserdata(J, 0, "userdata"); \
                                  paramType paramValue = internal::to(J, 1, paramType()); \
                                  if( parent ) parent->funcname( paramValue ); \
                                  js_pushundefined(J); \
                                }

#define DEFINE_OBJECT_FUNCTION_2(name,funcname,paramType1,paramType2) void name##_##funcname(js_State *J) { \
                                  name* parent = (name*)js_touserdata(J, 0, "userdata"); \
                                  paramType1 paramValue1 = internal::to( J, 1, paramType1() ); \
                                  paramType2 paramValue2 = internal::to( J, 2, paramType2() ); \
                                  if( parent ) parent->funcname( paramValue1, paramValue2 ); \
                                  js_pushundefined(J); \
                                }

#define DEFINE_OBJECT_OVERRIDE_FUNCTION_2(name,funcname,ov,paramType1,paramType2) void name##_##funcname##_##ov(js_State *J) { \
                                  name* parent = (name*)js_touserdata(J, 0, "userdata"); \
                                  paramType1 paramValue1 = internal::to( J, 1, paramType1() ); \
                                  paramType2 paramValue2 = internal::to( J, 2, paramType2() ); \
                                  if( parent ) parent->funcname( paramValue1, paramValue2 ); \
                                  js_pushundefined(J); \
                                }

#define DEFINE_OBJECT_FUNCTION_3(name,funcname,paramType1,paramType2,paramType3) void name##_##funcname(js_State *J) { \
  name* parent = (name*)js_touserdata(J, 0, "userdata"); \
  paramType1 paramValue1 = internal::to( J, 1, paramType1() ); \
  paramType2 paramValue2 = internal::to( J, 2, paramType2() ); \
  paramType3 paramValue3 = internal::to( J, 3, paramType3() ); \
  if( parent ) parent->funcname( paramValue1, paramValue2, paramValue3 ); \
  js_pushundefined(J); \
}

#define DEFINE_OBJECT_FUNCTION_4(name,funcname,paramType1,paramType2,paramType3,paramType4) void name##_##funcname(js_State *J) { \
                                  name* parent = (name*)js_touserdata(J, 0, "userdata"); \
                                  paramType1 paramValue1 = internal::to( J, 1, paramType1() ); \
                                  paramType2 paramValue2 = internal::to( J, 2, paramType2() ); \
                                  paramType3 paramValue3 = internal::to( J, 3, paramType3() ); \
                                  paramType4 paramValue4 = internal::to( J, 4, paramType4() ); \
                                  if( parent ) parent->funcname( paramValue1, paramValue2, paramValue3, paramValue4); \
                                  js_pushundefined(J); \
                                }

#define DEFINE_OBJECT_FUNCTION_5(name,funcname,paramType1,paramType2,paramType3,paramType4,paramType5) void name##_##funcname(js_State *J) { \
                                  name* parent = (name*)js_touserdata(J, 0, "userdata"); \
                                  paramType1 paramValue1 = internal::to( J, 1, paramType1() ); \
                                  paramType2 paramValue2 = internal::to( J, 2, paramType2() ); \
                                  paramType3 paramValue3 = internal::to( J, 3, paramType3() ); \
                                  paramType4 paramValue4 = internal::to( J, 4, paramType4() ); \
                                  paramType5 paramValue5 = internal::to( J, 5, paramType5() ); \
                                  if( parent ) parent->funcname( paramValue1, paramValue2, paramValue3, paramValue4, paramValue5 ); \
                                  js_pushundefined(J); \
                                }

#define SCRIPT_OBJECT_BEGIN(name) script_object_begin(#name);

#define SCRIPT_OBJECT_CALLBACK(name,funcname,params) { auto p = &name##_set_##funcname; reg_object_callback(#name,#funcname,p,params); }
#define SCRIPT_OBJECT_FUNCTION(name,funcname,params) { auto p = &name##_##funcname; reg_object_function(#name,#funcname,p,params); }
#define SCRIPT_OBJECT_CONSTRUCTOR(name) { auto p = &constructor_##name; reg_object_constructor(#name, p); }
#define SCRIPT_OBJECT_END(name) script_object_end(#name);
#define DEFINE_WIDGET_CONSTRUCTOR(name) void constructor_##name(js_State *J) { reg_widget_constructor(J, #name); }

#include "widget.implementation"
#include "menu.implementation"
#include "window.implementation"
#include "button.implementation"
#include "session.implementation"
#include "label.implementation"
#include "dialogbox.implementation"
#include "exitbutton.implementation"
#include "listbox.implementation"
#include "texturedbutton.implementation"
#include "image.implementation"
#include "editbox.implementation"
#include "animators.implementation"
#include "path.implementation"
#include "spinbox.implementation"
#include "filelistbox.implementation"
#include "picture.implementation"

DEFINE_VANILLA_CONSTRUCTOR(Session, internal::session)
DEFINE_VANILLA_CONSTRUCTOR(PlayerCity, (internal::game)->city().object())
DEFINE_VANILLA_CONSTRUCTOR(Emperor, &(internal::game)->empire()->emperor())
DEFINE_VANILLA_CONSTRUCTOR(Player, (internal::game)->player().object())

void Core::registerFunctions(Game& game)
{
  internal::J = js_newstate(NULL, NULL, JS_STRICT);
  js_atpanic(internal::J, enginePanic);

  internal::game = &game;
  internal::session = new Session(&game);
#define DEF_GLOBAL_OBJECT(name) js_newobject(internal::J);
#define REGISTER_FUNCTION(func,name,params) js_newcfunction(internal::J, func, name, params); js_setproperty(internal::J, -2, name);
#define REGISTER_GLOBAL_OBJECT(name) js_setglobal(internal::J, #name);

#include "engine.interface"
#include "widget.interface"
#include "menu.interface"
#include "window.interface"
#include "button.interface"
#include "session.interface"
#include "label.interface"
#include "dialogbox.interface"
#include "exitbutton.interface"
#include "listbox.interface"
#include "texturedbutton.interface"
#include "image.interface"
#include "editbox.interface"
#include "animators.interface"
#include "path.interface"
#include "spinbox.interface"
#include "filelistbox.interface"
#include "picture.interface"

  Core::loadModule(":/system/modules.js");
  js_pop(internal::J,2); //restore stack after call js-function
  Logger::warning( "STACK state {}", js_gettop(internal::J));
  internal::DirectoryChangeObserver.watch( ":/system" );
  internal::DirectoryChangeObserver.onFileChange().connect( &engineReloadFile );
}

void Core::unref(const std::string& ref)
{
  js_unref(internal::J, ref.c_str());
}

Core::Core()
{
}

} //end namespace script
