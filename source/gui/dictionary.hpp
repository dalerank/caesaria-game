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

#ifndef _CAESARIA_DICTIONARY_H_INCLUDE_
#define _CAESARIA_DICTIONARY_H_INCLUDE_

#include "window.hpp"
#include "core/signals.hpp"
#include "objects/overlay.hpp"

namespace gui
{

class DictionaryWindow : public Window
{
public:
  DictionaryWindow( Widget* parent, const std::string& helpId );
  DictionaryWindow( Widget* parent, object::Type type );

  virtual ~DictionaryWindow();
  virtual bool onEvent(const NEvent &event);

  virtual void load( const std::string& uri );

private:
  void _handleUriChange(std::string);
  vfs::Path _convUri2path(std::string uri);

  class Impl;
  ScopedPtr< Impl > _d;
};

} //end namespace gui

#endif //_CAESARIA_DICTIONARY_H_INCLUDE_
