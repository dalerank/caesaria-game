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
#include <GameVfs>
#include <GameGui>
#include <GameLogger>

namespace game
{

class Scripting::Impl
{
public:
  js_State *J;
};

static void GuiCreateWindow(js_State *J)
{
  const char *name = js_tostring(J, 1);
  printf("Hello, %s!\n", name);
  js_pushundefined(J);
}

static void EngineLog(js_State *J)
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

  js_dofile( instance()._d->J, rpath.toCString() );
}

void Scripting::registerFunctions()
{
#define DEF_GLOBAL_OBJECT(name) js_newobject(_d->J);
#define REGISTER_FUNCTION(func,name,params) js_newcfunction(_d->J, func, name, params); js_setproperty( _d->J, -2, name);
#define REGISTER_GLOBAL_OBJECT(name) js_setglobal(_d->J, #name);

DEF_GLOBAL_OBJECT(engine)
  REGISTER_FUNCTION(EngineLog,"log",1);
REGISTER_GLOBAL_OBJECT(engine)

DEF_GLOBAL_OBJECT(gui)
  REGISTER_FUNCTION(GuiCreateWindow,"window",1);
REGISTER_GLOBAL_OBJECT(gui)

}

Scripting::Scripting() : _d( new Impl )
{
  _d->J = js_newstate(NULL, NULL, JS_STRICT);
}


} //end namespace advisor
