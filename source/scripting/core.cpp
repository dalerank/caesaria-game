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
#include <GameGood>
#include <GameLogger>
#include <GameScene>
#include <GameCore>
#include <GameGfx>
#include <GameCity>
#include <GameWorld>
#include <GameWalkers>

#include "religion/divinities.hpp"
#include "religion/pantheon.hpp"
#include "sound/engine.hpp"
#include "scripting/session.hpp"
#include "core/alignment.hpp"

using namespace gui;
using namespace gui::dialog;
using namespace city;
using namespace world;
using namespace good;
using namespace gfx;
using namespace vfs;
using namespace religion;

namespace script
{

namespace internal
{
  Game* game = nullptr;
  std::set<std::string> files;
  std::set<std::string> files2load;
  vfs::FileChangeObserver DirectoryChangeObserver;
  Session* session = nullptr;
  js_State *J = nullptr;
} //end namespace internal

void engine_js_push(js_State* J, const Variant& param);
void engine_js_push(js_State* J, const DateTime& param);
void engine_js_push(js_State* J, const NEvent& param);
void engine_js_push(js_State* J, const WalkerPtr& w);
void engine_js_push(js_State* J, const Tile& param);
void engine_js_push(js_State* J, const Tilemap& param);
void engine_js_push(js_State* J, Widget* param);
void engine_js_push(js_State* J, gfx::Camera* param);

inline std::string engine_js_to(js_State *J, int n, std::string) { return js_tostring(J, n); }
inline int32_t engine_js_to(js_State *J, int n, int32_t) { return js_toint32(J, n); }
inline good::Product engine_js_to(js_State *J, int n, good::Product) { return (good::Product)js_toint32(J, n); }
inline Service::Type engine_js_to(js_State *J, int n, Service::Type) { return (Service::Type)js_toint32(J, n); }
inline Tile::Type engine_js_to(js_State *J, int n, Tile::Type) { return (Tile::Type)js_toint32(J, n); }
inline Orders::Order engine_js_to(js_State *J, int n, Orders::Order) { return (Orders::Order)js_toint32(J, n); }
inline gui::ElementState engine_js_to(js_State *J, int n, gui::ElementState) { return (gui::ElementState)js_toint32(J, n); }
inline Walker::Flag engine_js_to(js_State *J, int n, Walker::Flag) { return (Walker::Flag)js_toint32(J, n); }
inline Alignment engine_js_to(js_State *J, int n, Alignment) { return (Alignment)js_toint32(J, n); }
inline float engine_js_to(js_State *J, int n, float) { return (float)js_tonumber(J, n); }

PlayerCityPtr engine_js_to(js_State *J, int n, PlayerCityPtr) {
  PlayerCity* parent = (PlayerCity*)js_touserdata(J, n, "userdata");
  if (parent != nullptr) {
    return PlayerCityPtr(parent);
  }

  return PlayerCityPtr();
}

Variant engine_js_to(js_State *J, int n, Variant)
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

Font engine_js_to(js_State *J, int n, Font)
{
  Font f;
  if (js_isobject(J, n)) {
    std::string family = Font::defname;
    if (js_hasproperty(J, n, "family")) {
      js_getproperty(J, n, "family");
      family = js_tostring(J, -1);
    }

    int size = 12;
    if (js_hasproperty(J, n, "size")) {
      js_getproperty(J, n, "size");
      size = js_toint32(J, -1);
    }

    int color = ColorList::pink.color;
    if (js_hasproperty(J, n, "color")) {
      js_getproperty(J, n, "color");
      color = js_toint32(J, -1);
    }

    js_getproperty(J, n, "bold"); bool bold = js_toboolean(J, -1);
    js_getproperty(J, n, "italic"); bool italic = js_toboolean(J, -1);
    f = Font::create(family, size, bold, italic, NColor(color));
  }
  else if (js_isstring(J, n)) {
    std::string alias = js_tostring(J, n);
    f = Font::create(alias);
  }

  f.fallback(16, false, false, ColorList::pink);
  return f;
}

bool engine_js_tryPCall(js_State *J, int params)
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

void engine_js_push(js_State* J, const NEvent& event)
{
  if (event.EventType == sEventKeyboard) {
    auto kb = event.keyboard;
    js_newobject(J);
    js_pushnumber(J, kb.alt);  js_setproperty(J, -2, "alt");
    js_pushnumber(J, kb.control); js_setproperty(J, -2, "control");
    js_pushnumber(J, kb.key); js_setproperty(J, -2, "key");
    js_pushnumber(J, kb.pressed); js_setproperty(J, -2, "pressed");
    js_pushnumber(J, kb.shift); js_setproperty(J, -2, "shift");
  }
  else js_pushundefined(J);
}

void engine_js_push(js_State* J,const Size& size)
{
  js_newobject(J);
  js_pushnumber(J, size.width());  js_setproperty(J, -2, "w");
  js_pushnumber(J, size.height()); js_setproperty(J, -2, "h");
}

void engine_js_push(js_State* J, const Point& pos)
{
  js_newobject(J);
  js_pushnumber(J, pos.x()); js_setproperty(J, -2, "x");
  js_pushnumber(J, pos.y()); js_setproperty(J, -2, "y");
}

void engine_js_push(js_State* J, const TilePos& pos)
{
  js_newobject(J);
  js_pushnumber(J, pos.i()); js_setproperty(J, -2, "i");
  js_pushnumber(J, pos.j()); js_setproperty(J, -2, "j");
}

void engine_js_push(js_State* J, int32_t value) { js_pushnumber(J,value); }
void engine_js_push(js_State* J, const good::Product& value) { js_pushnumber(J, value); }
void engine_js_push(js_State* J, const Walker::Flag& value) { js_pushnumber(J, value); }
void engine_js_push(js_State* J, float value) { js_pushnumber(J, value); }
void engine_js_push(js_State* J, uint32_t value) { js_pushnumber(J, value); }
void engine_js_push(js_State* J, const std::string& p) { js_pushstring(J,p.c_str()); }

void engine_js_push(js_State* J, const Rect& p)
{
  js_newobject(J);
  js_pushnumber(J, p.left()); js_setproperty(J, -2, "x");
  js_pushnumber(J, p.top()); js_setproperty(J, -2, "y");
  js_pushnumber(J, p.width()); js_setproperty(J, -2, "w");
  js_pushnumber(J, p.height()); js_setproperty(J, -2, "h");
}

void engine_js_pushud(js_State* J, const std::string& name, void* v, js_Finalize destructor)
{
  js_newobject(J);
  js_getglobal(J, name.c_str());
  js_getproperty( J, -1, "prototype");
  js_newuserdata(J, "userdata", v, destructor);
}

void engine_js_push(js_State *J, const StringArray& items)
{
  js_newarray(J);
  for (uint32_t i = 0; i<items.size(); i++)
  {
    js_pushstring(J, items[i].c_str());
    js_setindex(J, -2, i);
  }
}

void engine_js_push(js_State *J, const good::Stock& stock) { engine_js_pushud(J, TEXT(Stock), &const_cast<good::Stock&>(stock), nullptr); }
void engine_js_push(js_State *J, const gfx::Tile& tile) { engine_js_pushud(J, TEXT(Tile), &const_cast<Tile&>(tile), nullptr); }
void engine_js_push(js_State *J, const good::Store& store) { engine_js_pushud(J, TEXT(Store), &const_cast<good::Store&>(store), nullptr); }
void engine_js_push(js_State *J, const gfx::Tilemap& tmap) { engine_js_pushud(J, TEXT(Tilemap), &const_cast<gfx::Tilemap&>(tmap), nullptr); }

void engine_js_push(js_State *J, const VariantMap& items)
{
  js_newobject(J);
  for (const auto& item : items)
  {
    engine_js_push(J, item.second);
    js_setproperty(J, -2, item.first.c_str());
  }
}

void engine_js_push(js_State *J, const TilesArray& items)
{
  js_newobject(J);
  for (uint32_t i=0; i < items.size(); i++)
  {
    engine_js_push(J, *items[i]);
    js_setproperty(J, -2, utils::i2str(i).c_str());
  }
}

void engine_js_push(js_State *J, const SmartList<Walker>& items)
{
  js_newobject(J);
  for (uint32_t i=0; i < items.size(); i++)
  {
    engine_js_push(J, items[i]);
    js_setproperty(J, -2, utils::i2str(i).c_str());
  }
}


void engine_js_push(js_State* J,const Variant& param)
{
  switch( param.type() )
  {
  case Variant::Bool:
    js_pushboolean(J, param.toBool() );
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
  break;

  case Variant::NPoint:
  case Variant::NPointF:
    engine_js_push(J, param.toPoint());
  break;

  case Variant::NTilePos:
    engine_js_push(J, param.toTilePos());
  break;

  case Variant::Date:
  case Variant::Time:
  case Variant::NDateTime:
    engine_js_push(J, param.toDateTime());
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
  }
  break;

  case Variant::Char:
  case Variant::String:
    js_pushstring(J, param.toString().c_str());
  break;

  default:
    js_pushnull(J);
    Logger::warning( "!!! Undefined value for js.pcall engine_js_push when find " + param.typeName() );
  break;
  }
}

template<class Type>
void engine_js_pushud_new(js_State *J, const Type& p, const std::string& tname, js_Finalize destructor)
{
  auto pd = new Type(p);
  engine_js_pushud(J, tname, pd,  destructor);
}

#define PREDEFINE_TYPE_DESTRUCTOR(type) void destructor_##type(js_State* J, void* p);

PREDEFINE_TYPE_DESTRUCTOR(Path)
PREDEFINE_TYPE_DESTRUCTOR(DateTime)
PREDEFINE_TYPE_DESTRUCTOR(Picture)

#define PUSH_SAVEDDATA(type) void engine_js_push(js_State* J, const type& p) { engine_js_push(J, p.save()); }
#define PUSH_USERDATA(type) void engine_js_push(js_State* J, type* p) { engine_js_pushud(J, #type, p, nullptr); }
#define PUSH_USERDATA_SMARTPTR(type) void engine_js_push(js_State* J, const SmartPtr<type>& p) { engine_js_pushud(J, #type, p.object(), nullptr); }
#define PUSH_USERDATA_WITHNEW(type) void engine_js_push(js_State* J, const type& p) { engine_js_pushud_new<type>(J, p, #type, destructor_##type); }

PUSH_USERDATA(ContextMenuItem)
PUSH_USERDATA(Widget)
PUSH_USERDATA(Stock)
PUSH_USERDATA(Store)

PUSH_USERDATA_SMARTPTR(PlayerCity)
PUSH_USERDATA_SMARTPTR(Player)
PUSH_USERDATA_SMARTPTR(Soldier)
PUSH_USERDATA_SMARTPTR(Overlay)
PUSH_USERDATA_SMARTPTR(Empire)
PUSH_USERDATA_SMARTPTR(Walker)
PUSH_USERDATA_SMARTPTR(Ruins)
PUSH_USERDATA_SMARTPTR(Factory)
PUSH_USERDATA_SMARTPTR(Divinity)
PUSH_USERDATA(Emperor)
PUSH_USERDATA(Camera)

PUSH_USERDATA_WITHNEW(Path)
PUSH_USERDATA_WITHNEW(DateTime)
PUSH_USERDATA_WITHNEW(Picture)
PUSH_SAVEDDATA(States)
PUSH_SAVEDDATA(VictoryConditions)

inline DateTime engine_js_to(js_State *J, int n, DateTime)
{
  if (js_isuserdata(J, 1, "userdata"))
  {
    DateTime* dt = (DateTime*)js_touserdata(J, 1, "userdata");
    return *dt;
  }

  return DateTime();
}

inline Picture engine_js_to(js_State *J, int n, Picture)
{
  if (js_isuserdata(J, 1, "userdata"))
  {
    Picture* pic = (Picture*)js_touserdata(J, 1, "userdata");
    return *pic;
  }

  return Picture();
}

inline StringArray engine_js_to(js_State *J, int n, StringArray)
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

inline bool engine_js_to(js_State *J, int n, bool) { return js_toboolean(J, n)>0; }
inline Size engine_js_to(js_State *J, int n, Size) { return Size( js_toint32(J, n), js_toint32(J, n+1) ); }
inline Path engine_js_to(js_State *J, int n, Path) { return vfs::Path( js_tostring(J, n)); }

inline PointF engine_js_to(js_State *J, int n, PointF)
{
  if (js_isobject(J, n))
  {
    js_getproperty(J, n, "x"); float x = js_tonumber(J, -1);
    js_getproperty(J, n, "y"); float y = js_tonumber(J, -1);
    return PointF(x, y);
  }
  return PointF(js_tonumber(J, n), js_tonumber(J, n + 1));
}

inline Point engine_js_to(js_State *J, int n, Point)
{
  PointF p = engine_js_to(J, n, PointF());
  return p.toPoint();
}

inline TilePos engine_js_to(js_State *J, int n, TilePos)
{
  if (js_isobject(J, n))
  {
    js_getproperty(J, n, "i"); int i = js_toint32(J, -1);
    js_getproperty(J, n, "j"); int j = js_toint32(J, -1);

    return TilePos(i, j);
  }
  return TilePos(js_toint32(J, n), js_toint32(J, n + 1));
}

inline Rect engine_js_to(js_State *J, int n, Rect)
{
  if (js_isobject(J, n))
  {
    js_getproperty(J, n, "x"); int x = js_toint32(J, -1);
    js_getproperty(J, n, "y"); int y = js_toint32(J, -1);
    js_getproperty(J, n, "w"); int w = js_toint32(J, -1);
    js_getproperty(J, n, "h"); int h = js_toint32(J, -1);

    return Rect(x, y, x+w, y+h);
  }
  return Rect( js_toint32(J, n), js_toint32(J, n+1),
               js_toint32(J, n+2), js_toint32(J, n+3) );
}

void engine_js_Panic(js_State *J)
{
  Logger::warning("JSE !!! Uncaught exception: %s\n", js_tostring(J, -1));
}

void engine_js_Log(js_State *J)
{
  if (js_isstring(J, 1)) {
    const char* text = js_tostring(J, 1);
    Logger::warning( text );
  } else if (js_isnumber(J, 1)) {
    int severity = js_toint16(J, 1);
    const char* text = js_tostring(J, 2);
    switch(severity) {
    case 0: Logger::debug(text); break;
    case 1: Logger::info(text); break;
    case 3: Logger::error(text); break;
    case 4: Logger::fatal(text); break;
    default: Logger::warning(text); break;
    }
  }

  js_pushundefined(J);
}

void engine_js_LoadArchive(js_State* J)
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

void engine_js_ReloadFile(vfs::Path path)
{
  internal::files2load.insert(path.toString());
}

void engine_js_SetVolume(js_State *J)
{
  int type = js_toint32(J, 1);
  int value = js_toint32(J, 2);
  audio::Engine::instance().setVolume((audio::SoundType)type, value);
}

void engine_js_LoadModule(js_State *J)
{
  vfs::Path scriptName = js_tostring(J, 1);
  internal::files.insert(scriptName.absolutePath().toString());
  Core::loadModule(scriptName.toString());
}

void engine_js_Translate(js_State *J)
{
  std::string text = js_tostring(J, 1);
  text = Locale::translate(text);
  js_pushstring(J,text.c_str());
}

void engine_js_GetOption(js_State *J)
{
  std::string name = js_tostring(J, 1);
  Variant value = game::Settings::get(name);
  engine_js_push(J, value);
}

void engine_js_SetOption(js_State *J)
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
  engine_js_tryPCall(internal::J, 0);
}

void Core::synchronize()
{
  if (internal::files2load.size() > 0) {
    for (const auto& path : internal::files2load) {
      if (internal::files.count(path) > 0) {
        Logger::warning("JS: script {} reloaded ", path);
        Core::loadModule(path);
      }
    }

    internal::files2load.clear();
  }
}

void Core::execFunction(const std::string& funcname)
{
  execFunction(funcname, VariantList());
}

void Core::execFunction(const std::string& funcname, const VariantList& params)
{
  if (internal::J == nullptr)
    return;
  Logger::info("script-if:// exec function " + funcname);
  int savetop = js_gettop(internal::J);
  js_getglobal(internal::J, funcname.c_str());
  js_pushnull(internal::J);
  for (const auto& param : params) {
    engine_js_push(internal::J,param);
  }

  bool error = engine_js_tryPCall(internal::J,params.size());
  if (error) {
    Logger::fatal("Fatal error on call function " + funcname);
  }

  js_pop(internal::J,2);
  if( savetop - js_gettop(internal::J) != 0 ) {
    Logger::warning( "STACK grow for {} in {}", js_gettop(internal::J), funcname );
  }
}

template<typename T>
void destructor_jsobject(js_State *J, void* p)
{
  T* ptr = (T*)p;
  delete ptr;
}

template<typename T>
void constructor_jsobject(js_State *J)
{
  js_currentfunction(J);
  js_getproperty(J, -1, "prototype");
  js_newuserdata(J, "userdata", new T(), &destructor_jsobject<T>);
}


template<typename T, typename ObjectType>
void object_handle_callback_0(ObjectType* object,const std::string& callback, const std::string& className)
{
  try {
    auto* ptrCheck = safety_cast<ObjectType*>(object);
    if(!ptrCheck) {
      Logger::warning( "!!! Callback " + className + ":" + callback + " called not for widget");
      return;
    }

    if (object) {
      std::string index = object->getProperty(callback);
      js_getregistry(internal::J,index.c_str());
      js_pushnull(internal::J);
      int error = engine_js_tryPCall(internal::J,0);
      if (error)
        Logger::warning("Fatal error on callback " + className + ":" + callback);
      js_pop(internal::J,2); //pop func+param from stack
    } else {
      Logger::warning(className + "_handle_" + callback + " widget is null");
    }
  } catch(...) {

  }
}

template<typename T, class ObjectType, typename P1>
void object_handle_callback_1(ObjectType* object, P1 value, const std::string& callback, const std::string& className)
{
  try {
    if(object) {
      std::string index = object->getProperty(callback);
      js_getregistry(internal::J,index.c_str());
      js_pushnull(internal::J);
      engine_js_push(internal::J, value);
      engine_js_tryPCall(internal::J,1);
      js_pop(internal::J,2);
    } else {
      Logger::warning(className + "_handle_" + callback + " object is null");
    }
  }
  catch(...) {

  }
}

template<typename T, typename ObjectType>
void object_set_callback_0(js_State *J,Signal1<ObjectType*>& (T::*f)(),
                           void (*handler)(Widget*),
                           const std::string& callback, const std::string& className)
{
  T* parent = (T*)js_touserdata(J, 0, "userdata");
  if (parent && js_iscallable(J,1)) {
    js_copy(J,1);
    std::string index = js_ref(J);
    (parent->*f)().connect(handler);
    parent->addProperty(callback, Variant(index));
  } else {
    Logger::warning( className + "_set_" + callback + " parent is null" );
  }

  js_pushundefined(J);
}

void constructor_Tile(js_State *J)
{
  js_currentfunction(J);
  js_getproperty(J, -1, "prototype");
  Tile* t = new Tile(TilePos());
  js_newuserdata(J, "userdata", t, &destructor_jsobject<Tile>);
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
  try {
    T* parent = (T*)js_touserdata(J, 0, "userdata");
    if (parent) {
      Rtype value = (parent->*f)();
      engine_js_push(J,value);
    } else {
      js_pushundefined(J);
    }
  } catch(...) {
    //something bad happens
  }
}

template<typename T,typename Rtype, typename P1Type>
void object_call_getter_1(js_State *J, Rtype (T::*f)(P1Type) const, P1Type def)
{
  T* parent = (T*)js_touserdata(J, 0, "userdata");
  if (parent) {
    auto paramValue1 = engine_js_to(J, 1, def);
    Rtype value = (parent->*f)(paramValue1);
    engine_js_push(J,value);
  } else {
    js_pushundefined(J);
  }
}

template<typename T,typename Rtype, typename P1Type>
void object_call_getter_1(js_State *J, Rtype (T::*f)(P1Type),P1Type def)
{
  T* parent = (T*)js_touserdata(J, 0, "userdata");
  if (parent)
  {
    auto paramValue1 = engine_js_to(J, 1, def);
    Rtype value = (parent->*f)(paramValue1);
    engine_js_push(J,value);
  }
  else
    js_pushundefined(J);
}

void reg_divinity_constructor(js_State *J)
{
  religion::DivinityPtr divn;
  if (js_isstring(J, 1))
  {
    std::string name = js_tostring(J, 1);
    divn = religion::rome::Pantheon::instance().get(name);
  }
  else if (js_isuserdata(J, 1, "userdata"))
  {
    //divn. = (T*)js_touserdata(J, 1, "userdata");
  }

  js_currentfunction(J);
  js_getproperty(J, -1, "prototype");
  js_newuserdata(J, "userdata", divn.object(), nullptr);
}

template<class T>
void constructor_go_jsobject(js_State *J, const std::string& tname)
{
  T* udata = nullptr;
  if (js_isuserdata(J, 1, "userdata")) {
    udata = (T*)js_touserdata(J, 1, "userdata");
    if (udata != nullptr )
    {
      js_currentfunction(J);
      js_getproperty(J, -1, "prototype");
      js_newuserdata(J, "userdata", udata, nullptr);
      return;
    }
  }

  js_pushnull(J);
}

template<class T>
void reg_overlay_constructor(js_State *J, const std::string& tname)
{
  T* ov = nullptr;
  if (js_isstring(J,1))
  {
    std::string name = js_tostring(J, 1);
    ov = safety_cast<T*>(TileOverlayFactory::instance().create(name).object());
  }
  else if(js_isuserdata(J, 1, "userdata"))
  {
    auto ptr = (Overlay*)js_touserdata(J, 1, "userdata");
    ov = safety_cast<T*>(ptr);

    if (!ov && ptr)
      Logger::warning("Cant convert {} to {}", ptr->info().typeName(), tname);
  }

  if (ov) {
    js_currentfunction(J);
    js_getproperty(J, -1, "prototype");
    js_newuserdata(J, "userdata", safety_cast<T*>(ov), nullptr);
  } else {
    js_pushnull(J);
  }
}

template<class T>
void reg_walker_constructor(js_State *J, const std::string& tname)
{
  T* wlk = nullptr;
  if (js_isstring(J,1))
  {
    std::string name = js_tostring(J, 1);
    walker::Type type = walker::toType(name);
    wlk = safety_cast<T*>(WalkerManager::instance().create(type, internal::game->city()).object());
  }
  else if(js_isuserdata(J, 1, "userdata"))
  {
    auto ptr = (Walker*)js_touserdata(J, 1, "userdata");
    wlk = safety_cast<T*>(ptr);

    if (!wlk && ptr)
      Logger::warning("Cant convert {} to {}", ptr->info().typeName(), tname);
  }

  if (wlk) {
    js_currentfunction(J);
    js_getproperty(J, -1, "prototype");
    js_newuserdata(J, "userdata", safety_cast<T*>(wlk), nullptr);
  } else {
    js_pushnull(J);
  }
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

  if (widget) {
    js_currentfunction(J);
    js_getproperty(J, -1, "prototype");
    js_newuserdata(J, "userdata", widget, nullptr);
  } else {
    js_pushnull(J);
  }
}

#define DEFINE_OBJECT_DESTRUCTOR(name) void destructor_##name(js_State *J, void* p) { destructor_jsobject<name>(J,p); }
#define DEFINE_OBJECT_CONSTRUCTOR(name) void constructor_##name(js_State *J) { constructor_jsobject<name>(J); }
#define DEFINE_GAMEOBJECT_CONSTRUCTOR(name) void constructor_##name(js_State *J) { constructor_go_jsobject<name>(J, #name); }
#define DEFINE_OBJECT_FUNCTION_0(name,funcname) void name##_##funcname(js_State *J) { auto p=&name::funcname; object_call_func_0<name>(J,p); }

#define DEFINE_OBJECT_CALLBACK_0(name,objType,callback) void name##_handle_##callback(objType* object) { object_handle_callback_0<name, objType>(object, "js_"#callback, #name); } \
                                                        void name##_set_##callback(js_State *J) { \
                                                               auto handler=&name##_handle_##callback; \
                                                               auto objectCallback=&name::callback; \
                                                               object_set_callback_0<name, Widget>(J, objectCallback, handler, "js_"#callback, #name);  \
                                                        }

#define DEFINE_OBJECT_CALLBACK_1(name,objType,callback,type) void name##_handle_##callback(objType* object,type value) { object_handle_callback_1<name,objType,type>(object, value, "js_"#callback, #name); } \
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

#define DEFINE_WIDGET_CALLBACK_0(name,callback) DEFINE_OBJECT_CALLBACK_0(name,Widget,callback)
#define DEFINE_WIDGET_CALLBACK_1(name,callback,type) DEFINE_OBJECT_CALLBACK_1(name,Widget,callback,type)


#define DEFINE_OBJECT_GETTER_0(name,rtype,funcname) void name##_##funcname(js_State* J) { rtype (name::*p)() const=&name::funcname; object_call_getter_0<name,rtype>(J,p); }
#define DEFINE_OBJECT_GETTER_1(name,rtype,funcname,p1type,def) void name##_##funcname(js_State* J) { auto p=&name::funcname; object_call_getter_1<name,rtype,p1type>(J,p,def); }

#define DEFINE_OBJECT_GETTER_3(name,rtype,funcname,p1type,p2type,p3type) void name##_##funcname(js_State* J) { \
                                                  name* object = (name*)js_touserdata(J, 0, "userdata"); \
                                                  if (object) { \
                                                      auto paramValue1 = engine_js_to(J, 1, p1type()); \
                                                      auto paramValue2 = engine_js_to(J, 2, p2type()); \
                                                      auto paramValue3 = engine_js_to(J, 3, p3type()); \
                                                      rtype value = object->funcname(paramValue1,paramValue2,paramValue3); \
                                                      engine_js_push(J,value); \
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
  paramType1 paramValue1 = engine_js_to( J, 1, paramType1() ); \
  paramType2 paramValue2 = engine_js_to( J, 2, paramType2() ); \
  if (parent) engine_js_push(J,parent->funcname(paramValue1,paramValue2)); \
  else js_pushundefined(J); \
}

#define DEFINE_OBJECT_FUNCTION_1(name,funcname,paramType) void name##_##funcname(js_State *J) { \
                                  name* parent = (name*)js_touserdata(J, 0, "userdata"); \
                                  paramType paramValue = engine_js_to(J, 1, paramType()); \
                                  if( parent ) parent->funcname( paramValue ); \
                                  js_pushundefined(J); \
                                }

#define DEFINE_OBJECT_OVERRIDE_FUNCTION_1(name,funcname,ov,paramType) void name##_##funcname##_##ov(js_State *J) { \
                                  name* parent = (name*)js_touserdata(J, 0, "userdata"); \
                                  paramType paramValue = engine_js_to(J, 1, paramType()); \
                                  if( parent ) parent->funcname( paramValue ); \
                                  js_pushundefined(J); \
                                }

#define DEFINE_OBJECT_OVERRIDE_GETTER_0(name,rtype,funcname,ov) void name##_##funcname##_##ov(js_State* J) { rtype (name::*p)() const=&name::funcname; object_call_getter_0<name,rtype>(J,p); }
#define DEFINE_OBJECT_OVERRIDE_GETTER_1(name,rtype,funcname,ov,p1type,def) void name##_##funcname##_##ov(js_State* J) { rtype (name::*p)(p1type) const=&name::funcname; object_call_getter_1<name,rtype,p1type>(J,p,def); }


#define DEFINE_OBJECT_FUNCTION_2(name,funcname,paramType1,paramType2) void name##_##funcname(js_State *J) { \
                                  name* parent = (name*)js_touserdata(J, 0, "userdata"); \
                                  paramType1 paramValue1 = engine_js_to( J, 1, paramType1() ); \
                                  paramType2 paramValue2 = engine_js_to( J, 2, paramType2() ); \
                                  if( parent ) parent->funcname( paramValue1, paramValue2 ); \
                                  js_pushundefined(J); \
                                }

#define DEFINE_OBJECT_OVERRIDE_FUNCTION_2(name,funcname,ov,paramType1,paramType2) void name##_##funcname##_##ov(js_State *J) { \
                                  name* parent = (name*)js_touserdata(J, 0, "userdata"); \
                                  paramType1 paramValue1 = engine_js_to( J, 1, paramType1() ); \
                                  paramType2 paramValue2 = engine_js_to( J, 2, paramType2() ); \
                                  if( parent ) parent->funcname( paramValue1, paramValue2 ); \
                                  js_pushundefined(J); \
                                }

#define DEFINE_OBJECT_FUNCTION_3(name,funcname,paramType1,paramType2,paramType3) void name##_##funcname(js_State *J) { \
  name* parent = (name*)js_touserdata(J, 0, "userdata"); \
  paramType1 paramValue1 = engine_js_to( J, 1, paramType1() ); \
  paramType2 paramValue2 = engine_js_to( J, 2, paramType2() ); \
  paramType3 paramValue3 = engine_js_to( J, 3, paramType3() ); \
  if( parent ) parent->funcname( paramValue1, paramValue2, paramValue3 ); \
  js_pushundefined(J); \
}

#define DEFINE_OBJECT_FUNCTION_4(name,funcname,paramType1,paramType2,paramType3,paramType4) void name##_##funcname(js_State *J) { \
                                  name* parent = (name*)js_touserdata(J, 0, "userdata"); \
                                  paramType1 paramValue1 = engine_js_to( J, 1, paramType1() ); \
                                  paramType2 paramValue2 = engine_js_to( J, 2, paramType2() ); \
                                  paramType3 paramValue3 = engine_js_to( J, 3, paramType3() ); \
                                  paramType4 paramValue4 = engine_js_to( J, 4, paramType4() ); \
                                  if( parent ) parent->funcname( paramValue1, paramValue2, paramValue3, paramValue4); \
                                  js_pushundefined(J); \
                                }

#define DEFINE_OBJECT_FUNCTION_5(name,funcname,paramType1,paramType2,paramType3,paramType4,paramType5) void name##_##funcname(js_State *J) { \
                                  name* parent = (name*)js_touserdata(J, 0, "userdata"); \
                                  paramType1 paramValue1 = engine_js_to( J, 1, paramType1() ); \
                                  paramType2 paramValue2 = engine_js_to( J, 2, paramType2() ); \
                                  paramType3 paramValue3 = engine_js_to( J, 3, paramType3() ); \
                                  paramType4 paramValue4 = engine_js_to( J, 4, paramType4() ); \
                                  paramType5 paramValue5 = engine_js_to( J, 5, paramType5() ); \
                                  if( parent ) parent->funcname( paramValue1, paramValue2, paramValue3, paramValue4, paramValue5 ); \
                                  js_pushundefined(J); \
                                }

#define SCRIPT_OBJECT_BEGIN(name) script_object_begin(#name);

#define SCRIPT_OBJECT_CALLBACK(name,funcname,params) { auto p = &name##_set_##funcname; reg_object_callback(#name,#funcname,p,params); }
#define SCRIPT_OBJECT_FUNCTION(name,funcname,params) { auto p = &name##_##funcname; reg_object_function(#name,#funcname,p,params); }
#define SCRIPT_OBJECT_CONSTRUCTOR(name) { auto p = &constructor_##name; reg_object_constructor(#name, p); }
#define SCRIPT_OBJECT_END(name) script_object_end(#name);
#define DEFINE_WIDGET_CONSTRUCTOR(name) void constructor_##name(js_State *J) { reg_widget_constructor(J, #name); }
#define DEFINE_OVERLAY_CONSTRUCTOR(name) void constructor_##name(js_State *J) { reg_overlay_constructor<name>(J, #name); }
#define DEFINE_WALKER_CONSTRUCTOR(name) void constructor_##name(js_State *J) { reg_walker_constructor<name>(J, #name); }
#define DEFINE_DIVINITY_CONSTRUCTOR(name) void constructor_##name(js_State *J) { reg_divinity_constructor(J); }

#include "widget.template"
#include "overlay.template"
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
#include "city.implementation"
#include "overlay.implementation"
#include "religion.implementation"
#include "walker.implementation"

DEFINE_VANILLA_CONSTRUCTOR(Session, internal::session)
DEFINE_VANILLA_CONSTRUCTOR(PlayerCity, (internal::game)->city().object())
DEFINE_VANILLA_CONSTRUCTOR(Tilemap, &(internal::game)->city()->tilemap());
DEFINE_VANILLA_CONSTRUCTOR(Emperor, &(internal::game)->empire()->emperor())
DEFINE_VANILLA_CONSTRUCTOR(Player, (internal::game)->player().object())

void Core::registerFunctions(Game& game)
{
  internal::J = js_newstate(NULL, NULL, JS_STRICT);
  js_atpanic(internal::J, engine_js_Panic);

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
#include "city.interface"
#include "overlay.interface"
#include "religion.interface"
#include "walker.interface"

  Core::loadModule(":/system/modules.js");
  js_pop(internal::J,2); //restore stack after call js-function
  Logger::warning( "STACK state {}", js_gettop(internal::J));
  internal::DirectoryChangeObserver.watch( ":/system" );
  internal::DirectoryChangeObserver.onFileChange().connect( &engine_js_ReloadFile );
}

void Core::unref(const std::string& ref)
{
  js_unref(internal::J, ref.c_str());
}

Core::Core()
{
}

} //end namespace script
