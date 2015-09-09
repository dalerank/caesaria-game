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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#ifndef _CAESARIA_WINDOW_LANGUAGE_SELECT_H_INCLUDE_
#define _CAESARIA_WINDOW_LANGUAGE_SELECT_H_INCLUDE_

#include "label.hpp"

namespace gui
{

class ListBoxItem;

namespace dialog
{

class LanguageSelect : public Label
{
public:
  LanguageSelect(Widget* parent, vfs::Path model,
                 const std::string& current, Size size = Size(512,384));

  virtual ~LanguageSelect();
  void setDefaultFont( const std::string& fontname );

public signals:
  Signal3<std::string,std::string,std::string> onChange;
  Signal0<> onContinue;

private:
  class Impl;
  ScopedPtr<Impl> _d;

  void _changeLanguage(const ListBoxItem& item);
  void _apply();

  std::string _defaultFont;
};

}//end namespace dialog

}//end namespace gui
#endif //_CAESARIA_WINDOW_LANGUAGE_SELECT_H_INCLUDE_
